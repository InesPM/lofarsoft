"""
Module documentation
====================

Wavefront Task: produces the shape of the incoming wavefront, using pulse arrival times at each antenna.

Reprocess cross correlation using MultiTBBData datareader, which reads and bundles data from all
datafiles for a given event.

Reprocess cross correlation over all stations (polarisations separately).
Obtain arrival times; apply calibration delays and inter-station delays.

Get wavefront shape and fit curvature using point-source approximation.

later?: Reprocess cross correlation over 0/1 polarisation inside each station.


.. moduleauthor:: Arthur Corstanje <a.corstanje@astro.ru.nl>
"""

from pycrtools.tasks import Task, crosscorrelateantennas, shower, directionfitplanewave
import pycrtools as cr
from pycrtools import srcfind as sf
from pycrtools import lora
import os
import numpy as np
from numpy import sin, cos, tan, sqrt
from scipy.optimize import fmin
import matplotlib.pyplot as plt

cr.tasks.__raiseTaskDeprecationWarning(__name__)

deg2rad = np.pi / 180
c = 299792458.0

def mseMinimizer(direction, pos, times, outlierThreshold=0, allowOutlierCount=0):
    # Quality function for simplex search
    az = direction[0]
    el = direction[1]
    R = 1000.0 / direction[2]  # using 1/R as parameter - curvature dependence is on 1/R to lowest order; will work better in simplex search.
    mse = sf.mseWithDistance(az, el, R, pos, times, outlierThreshold, allowOutlierCount)
    print 'Evaluated direction: az = %f, el = %f, R = %f: MSE = %f' % (az / deg2rad, el / deg2rad, R, mse)

    return mse

def fitQualityFromCore(core, az, el, positions2D, times):
    cartesianDirection = - np.array([cos(el) * sin(az), cos(el) * cos(az), sin(el)]) # minus sign for incoming vector!
    axisDistance = []
    showerPlaneTimeDelay = []

    for pos in positions2D:
        relpos = pos - core
        delay = (1/c) * np.dot(cartesianDirection, relpos)
        distance = np.linalg.norm(relpos - np.dot(cartesianDirection, relpos) * cartesianDirection)
        axisDistance.append(distance)
        showerPlaneTimeDelay.append(delay)

    axisDistance = np.array(axisDistance)
    showerPlaneTimeDelay = np.array(showerPlaneTimeDelay)

    reducedArrivalTimes = 1e9 * (times - showerPlaneTimeDelay)

    polyfit = np.polyfit(axisDistance, reducedArrivalTimes, 4)
    polyvalues = np.poly1d(polyfit)
    chi_squared = np.sum((np.polyval(polyfit, axisDistance) - reducedArrivalTimes) ** 2) / (len(axisDistance) - 4)
    reducedArrivalTimes -= polyfit[4]

    return (chi_squared, axisDistance, reducedArrivalTimes, polyfit, polyvalues)

def chi2Minimizer_azel(azel, core, positions2D, times):
    (az, el) = azel
    chi2 = fitQualityFromCore(core, az, el, positions2D, times)[0]
    print 'Evaluated direction: az = %3.3f, el = %2.3f, chi^2 = %1.4f' % (az / deg2rad, el / deg2rad, chi2)

    return chi2


def flaggedIndicesForOutliers(inarray, k_sigma = 5):
    # Remove outliers beyond k-sigma (above and below)
    # Use robust estimators, i.e. median and percentile-based sigma.
    n = len(inarray)
    sortedArray = np.sort(inarray)
    median = np.median(sortedArray)
    # ~ sigma estimator assuming normal distribution, robust against 30% bad data
    noise = (sortedArray[0.7*n] - sortedArray[0.3*n]) / 1.05

    goodIndices = np.where( abs(inarray - median) < k_sigma * noise )[0] # indices in original array!
    flaggedIndices = np.array( [i for i in range(n) if i not in goodIndices] ) # inversion

    return (flaggedIndices, goodIndices)

def getLORAinfo(lora_dir, datafile):
    lora_info = lora.loraInfo(datafile["TIME"][0], lora_dir)
    return lora_info


def pulseTimeFromLORA(lora_dir, datafile):
    print "---> Reading pulse timing information from LORA"
    tbb_starttime = datafile["TIME"][0]
    tbb_samplenumber = max(datafile["SAMPLE_NUMBER"])
    blocksize = datafile["BLOCKSIZE"]

    lora_event_info = 0 # to check with 'if lora_event_info:'
    lora_logfile = os.path.join(lora_dir,"LORAtime4")
    if os.path.exists(lora_logfile):
        (tbb_starttime_sec,tbb_starttime_nsec)=lora.nsecFromSec(tbb_starttime,logfile=lora_logfile)

        if tbb_starttime_sec:
            try:
                (block_number_lora,sample_number_lora)=lora.loraTimestampToBlocknumber(tbb_starttime_sec,tbb_starttime_nsec,tbb_starttime,tbb_samplenumber,blocksize=blocksize)
            except ValueError:
                raise ValueError("#ERROR - LORA trigger information not found")

            print "---> Estimated block number from LORA: block =",block_number_lora,"sample =",sample_number_lora
            block_number = block_number_lora
            sample_number = sample_number_lora
            print "---> From LORA: block =",block_number,"sample =",sample_number
        else:
            raise ValueError("LORA trigger not found in database")
            # Write parameters to database
            # print "WARNING: LORA logfile found but no info for this event time in LORAtime4 file!"
    else:
        raise ValueError( "WARNING: No LORA logfile found - " + str(lora_logfile) )

    return (block_number, sample_number)


class Wavefront_full(Task):
    """
    Obtain the shape of the wavefront of a CR pulse.
    Re-calculate all relative arrival times using cross correlation on the full event dataset.
    Uses MultiTBBData to open all files, applying clock offset shifts etc.

    Subtract best-fitting plane wave solution in order to obtain second-order wavefront shape (i.e. curvature).

    Compare with point-source approximation, get best-fitting point source and look at residuals.
    """

    parameters = dict(
        filefilter = dict(default=None, doc="File filter for multiple data files in one event, e.g. '/my/data/dir/L45472_D20120206T030115.786Z*.h5' "),
        filelist = dict(default=None, doc="List of filenames in one event. "),
        f = dict( default = None,
            doc = "File object. Blocksize, polarisation + antenna selection etc. are taken from the given file object, and should have been set before calling this Task." ),
        loradir = dict( default = None, doc = "Directory with LORA trigger data. Required for obtaining pulse location in the data. "),
        interStationDelays = dict( default=None, doc="Inter-station delays as a correction on current LOFAR clock offsets. To be obtained e.g. from the CalibrateFM Task. Assumed to be in alphabetic order in the station name e.g. CS002, CS003, ... If not given, zero correction will be assumed." ),
        blocksize = dict ( default = 65536, doc = "Blocksize." ),
        nantennas = dict( default = lambda self : self.f["NOF_SELECTED_DATASETS"],
            doc = "Number of selected antennas." ),
#        nofblocks = dict( default = -1, doc = "Number of data blocks to process. Set to -1 for entire file." ),
        pulse_block = dict( default = 0, doc = "Block nr. where the pulse is" ),
        refant = dict(default = None, doc = "Optional parameter to set reference antenna number."),
        pol = dict(default = 0, doc = "Polarization. Only used if no file object is given."),
#        timeseries_data = dict( default = lambda self : cr.hArray(float, dimensions=(self.nantennas, self.blocksize)),
#            doc = "Timeseries data." ),
        #fftwplan = dict( default = lambda self : cr.FFTWPlanManyDftR2c(self.blocksize, self.nantennas, 1, 1, 1, 1, cr.fftw_flags.MEASURE),
        #    doc = "Forward plan for FFTW, by default use a plan that works over all antennas at once and is measured for speed because this is applied many times." ),
        arrivaltimes = dict( default = None, doc = "Pulse arrival times per antenna; give as input parameter together with 'positions' if not using 'filefilter' or 'filelist'."),
        positions = dict( default = None, doc = "Antenna positions array in [Nx3] format (2D)."),
        pointsourceArrivalTimes = dict( default = None, doc = "Arrival times from best-fit point source approximation", output = True),
        planewaveArrivalTimes = dict( default = None, doc = "Arrival times from best-fit plane-wave approximation.", output = True),
        fitPlaneWave = dict( default = None, doc = "Fit results (az, el, mse) for planar fit.", output = True ),
        fitPointSource = dict( default = None, doc = "Fit results (az, el, R, mse) for point source simplex search.", output = True ),
        bad_antennas = dict( default = [],
            doc = "Antennas found to be bad.", output = True ),
        good_antennas = dict( default = lambda self : [name for name in self.f["SELECTED_DIPOLES"] if name not in self.bad_antennas],
            doc = "Antennas found to be good.", output = True ),
        save_plots = dict( default = False,
            doc = "Store plots" ),
        plot_prefix = dict( default = "",
            doc = "Prefix for plots" ),
        plotlist = dict( default = [],
            doc = "List of plots" ),
        plot_antennas = dict( default = lambda self : range(self.nantennas),
            doc = "Antennas to create plots for." ),
        verbose = dict( default = True, doc = "Verbose output."),
    )

    def run(self):
        """Run the task.
        """

        if self.positions is None and self.arrivaltimes is None:

            if not self.f and not self.filefilter and not self.filelist:
                raise RuntimeError("Give a file object or a filefilter or a filelist")
            if not self.f:
                #filefilter = '/Users/acorstanje/triggering/CR/*.986Z*.h5'
                if not self.filelist:
                    self.filelist = cr.listFiles(self.filefilter)
                superterpStations = ["CS002", "CS003", "CS004", "CS005", "CS006", "CS007", "CS021"]
                if len(self.filelist) == 1:
                    print 'There is only 1 file'
                    self.filelist = self.filelist[0]
                else:  # sort files on station ID
                    sortedlist = []
                    for station in superterpStations:
                        thisStationsFile = [filename for filename in self.filelist if station in filename]
                        if len(thisStationsFile) > 0:
                            sortedlist.append(thisStationsFile[0])
                    self.filelist = sortedlist
                print '[Wavefront] Processing files: '
                print self.filelist
                self.f = cr.open(self.filelist, blocksize=self.blocksize)
                selected_dipoles = [x for x in self.f["DIPOLE_NAMES"] if int(x) % 2 == self.pol]
                self.f["SELECTED_DIPOLES"] = selected_dipoles

            self.blocksize = self.f["BLOCKSIZE"]

            # Get the pulse location in the data from LORA timing: block and sample number
            (block, pulse_samplenr) = pulseTimeFromLORA(self.loradir, self.f)

            # assume LORA-LOFAR delay and file shifts are of the order ~ 200 samples << blocksize for cut-out timeseries

            antennaPositions = self.f["ANTENNA_POSITIONS"]
            self.positions = antennaPositions

            timeseries = self.f.empty("TIMESERIES_DATA")
            # get timeseries data with pulse, then cut out a region around the pulse.
            cutoutSize = 2048
            self.f.getTimeseriesData(timeseries, block = block)
            nofChannels = timeseries.shape()[0]
            cutoutTimeseries = cr.hArray(float, dimensions=[nofChannels, cutoutSize])
            start = pulse_samplenr - cutoutSize / 2
            end = pulse_samplenr + cutoutSize / 2
            cutoutTimeseries[...].copy(timeseries[..., start:end])

            # Get reference antenna, take the one with the highest maximum.
            y = cutoutTimeseries.toNumpy()
            refant = self.refant
            if not self.refant:
                refant = int(np.argmax(np.max(y, axis=1)))
            print 'Taking channel %d as reference antenna' % refant

            #import pdb; pdb.set_trace()
            plt.plot(y[0])
            plt.plot(y[140])

            sample_interval = 5.0e-9 # hardcoded...
            # is the index of the ref antenna also in the full list of antids / antpos
            yy = cutoutTimeseries.toNumpy()
            plt.figure()
            plt.plot(yy[0])
            plt.plot(yy[1])
            plt.plot(yy[2])
            plt.plot(yy[23])
            # now cross correlate all channels in full_timeseries, get relative times
            crosscorr = cr.trerun('CrossCorrelateAntennas', "crosscorr", timeseries_data=cutoutTimeseries, oversamplefactor=10, save_plots=True)

            yy = crosscorr.crosscorr_data.toNumpy()
            plt.figure()
            plt.plot(yy[5])
            plt.plot(yy[17])

            # And determine the relative offsets between them
            #maxima_cc = cr.trerun('FitMaxima', "Lags", crosscorr.crosscorr_data, doplot=True, plotend=5, sampleinterval=sample_interval / crosscorr.oversamplefactor, peak_width=11, splineorder=3, refant=refant)
            maxima_cc = cr.trerun('FindPulseDelay', "delay", trace=crosscorr.crosscorr_data, refant=0, sampling_frequency=200.0e6*10)

            # plot lags, plot flagged lags from a k-sigma criterion on the crosscorr maximum

            #cr.hArray(maxima_cc.lags).plot()

            # Plot arrival times, do plane-wave fit, plot residuals wrt plane wave
            #arrivaltime = cr.hArray(maxima_cc.lags)
            arrivaltime = cr.hArray(maxima_cc.delays)
            times = arrivaltime.toNumpy()
            positions = antennaPositions.toNumpy().ravel()

            # Apply calibration delays (per antenna)
            arrivaltime -= cr.hArray(self.f["DIPOLE_CALIBRATION_DELAY"])
            # Apply sub-sample inter-station clock offsets (LOFAR)
            subsampleOffsets = self.f["SUBSAMPLE_CLOCK_OFFSET"] # numpy array!
            # Apply inter-station delays from RFIlines Task
            # Assuming ordering CS002, 3, 4, 5, 7 for this event (sorted alphabetically)
            if not type(self.interStationDelays) == type(None): # cannot do if self.interStationDelays, apparently...
                subsampleOffsets -= self.interStationDelays

            subsampleOffsets[1] -= 0.11e-9
            subsampleOffsets[2] -= -1.30e-9
            subsampleOffsets[3] -= -1.32e-9
            subsampleOffsets[4] -= 0.71e-9

            stationList = self.f["STATION_LIST"]
            print stationList
            stationStartIndex = self.f["STATION_STARTINDEX"]
            for i in range(len(subsampleOffsets)):
                arrivaltime[stationStartIndex[i]:stationStartIndex[i + 1]] -= subsampleOffsets[i]
                # Sign + or - ???

        # plt.figure()
        # arrivaltime.plot()
        # plt.title('Arrival times, matched with offsets per station (check!)')
        times = arrivaltime.toNumpy()
        self.arrivaltimes = times
        positions = antennaPositions.toNumpy().ravel()
        positions2D = antennaPositions.toNumpy()
        # now make footprint plot of all arrival times

        signals = np.copy(times)
        signals.fill(2.71) # signal power not used here; do not give all 1.0 as the log is taken.

        # Get event core etc.
        loraInfo = getLORAinfo(self.loradir, self.f)
        core = loraInfo["core"]
        direction = loraInfo["direction"] # az, el in degrees??

        fptask = cr.trerun("Shower", "1", positions=positions2D, signals=signals, timelags=times, core=core, footprint_colormap='jet', footprint_enable=True, footprint_shower_enable=False)

        plt.title('Footprint using crosscorrelated arrival times')
        plt.figure()
        # Do plane-wave direction fit on full arrivaltimes
        # Fit pulse direction
        print 'Do plane wave fit on full arrival times (cross correlations here)...'
        direction_fit_plane_wave = cr.trun("DirectionFitPlaneWave", positions=antennaPositions, timelags=arrivaltime, verbose=True)

        direction_fit_plane_wave.residual_delays.plot()
        plt.title('Residual delays after plane wave fit')

        residu = direction_fit_plane_wave.residual_delays.toNumpy()
        bestfitDelays = sf.timeDelaysFromDirection(positions, direction_fit_plane_wave.meandirection_azel)
        self.planewaveArrivalTimes = bestfitDelays

        # Flag outlying residues. Determine valid range for residues, and set the zero-offset value
        (flaggedIndices, goodIndices) = flaggedIndicesForOutliers(residu)
        print 'Flagged channels: '
        print flaggedIndices

        goodResidues = residu[goodIndices]
        goodPositions2D = positions2D[goodIndices]
        goodPositions = goodPositions2D.ravel()
        goodTimes = times[goodIndices]
        goodSignals = signals[goodIndices]
        goodResidues -= min(goodResidues)
        # now the good one: difference between measured arrival times and plane wave fit!
        fptask_delta = cr.trerun("Shower", "3", positions=goodPositions2D, signals=goodSignals, timelags=goodResidues, footprint_colormap='jet', footprint_enable=True, footprint_shower_enable=False)
        plt.title('Footprint of residual delays w.r.t. planewave fit')

        (az, el) = direction_fit_plane_wave.meandirection_azel  # check
        planarDirection = cr.hArray([az / deg2rad, el / deg2rad, 0.])

#todo        stationStartIndex = []
#        stationList = []
#        for i in range(len(goodStationNames)):
#            if goodStationNames[i] != goodStationNames[i-1]:
#                stationList.append(goodStationNames[i])
#                stationStartIndex.append(i)
#        stationStartIndex.append(len(goodStationNames)) # last index + 1 for indexing with start:end

        # Make 1-D projected plot of curvature:
        # - take shower direction and core position from LORA
        # - project antenna positions to shower coordinates (in the plane with shower axis as normal vector)
        # - subtract the (plane-wave) time difference due to the distance shower plane - real antenna position
        #az = np.radians(self.lora_direction[0])
        #el = np.radians(self.lora_direction[1])
        (az, el) = direction_fit_plane_wave.meandirection_azel
        print 'Fitted az = %3.2f, el = %3.2f' % (az / deg2rad, el / deg2rad)
        (az, el) = (318.73 * deg2rad, 50.93 * deg2rad)
        print 'LORA az = %3.2f, el = %3.2f' % (az / deg2rad, el / deg2rad)
        # HACK LORA values in here
#        print (az, el)
        #core = np.array([self.loracore[0], self.loracore[1], 0.0])
        core = np.array([-132.5, 101.8, 0])
        # Fit optimum az, el for this core
        optimum = fmin(chi2Minimizer_azel, (az, el), (core, goodPositions2D, goodTimes), xtol=1e-5, ftol=1e-5, full_output=1)
        (bestAz, bestEl) = optimum[0]
        print 'Optimum az/el:'
        print 'az = %3.3f, el = %2.3f' % (bestAz / deg2rad, bestEl / deg2rad)

        print 'Core position: '
        print 'x = %3.2f, y = %3.2f' % (core[0], core[1])
        (chi_squared, axisDistance, reducedArrivalTimes, polyfit, polyvalues) = fitQualityFromCore(core, bestAz, bestEl, goodPositions2D, goodTimes)

        plt.figure()
        start = 0
        colors = ['b', 'g', 'r', 'c', 'm', 'y'] * 4 # don't want to run out of colors array
        for i in range(len(stationList)):
            start = stationStartIndex[i]
            end = stationStartIndex[i+1]
            plt.scatter(axisDistance[start:end], reducedArrivalTimes[start:end], 20, label=stationList[i], c = colors[i], marker='o')

        plt.plot(np.sort(axisDistance), polyvalues(np.sort(axisDistance)) - polyfit[4], marker='-', lw=3, c='r')
        plt.xlim([0.0, 50 * int(max(axisDistance) / 50) + 50])

        plt.legend()
        plt.xlabel('Distance from axis [m]')
        plt.ylabel('Arrival time in fitted shower plane [ns]')
        #plt.plot(a, expectedDelays*1e9, c='g')
        plt.title('Arrival times vs distance from fit-centered shower axis\n Polyfit coeffs (r=r_100): t = %2.2f r + %2.2f r^2 + %2.2f r^3 + %2.2f r^4\nchi^2 = %2.2f' % (polyfit[3] * 100, polyfit[2] * 10000, polyfit[1] * 10**6, polyfit[0]*10**8, chi_squared))
#        if self.save_plots:
#            p = self.plot_prefix + "wavefront_arrivaltime_showerplane.{0}".format(self.plot_type)
#            plt.savefig(p)
#            self.plotlist.append(p)
        # rotate antenna position axes to a, p coordinates. a = along time gradient, p = perpendicular to that

        (x, y) = (goodPositions2D[:, 0], goodPositions2D[:, 1])
        a = - sin(az) * (x - core[0]) - cos(az) * (y - core[1])
        p = cos(az) * (x - core[0]) - sin(az) * (y - core[1])
        z = goodPositions2D[:, 2]
        nofGoodChannels = len(a)
        rotatedPositions2D = np.concatenate([a, p, z]).reshape(3, nofGoodChannels).T

        expectedDelays = sf.timeDelaysFromDirection(goodPositions, (az, el))
        expectedDelays += (goodTimes[0] - expectedDelays[0]) # cheap linear fit

        plt.figure()
        start = 0
        colors = ['b', 'g', 'r', 'c', 'm', 'y']
        for i in range(len(stationList)):
            start = stationStartIndex[i]
            end = stationStartIndex[i+1]
            plt.scatter(a[start:end], goodTimes[start:end] * 1e9, 20, label=stationList[i], c = colors[i], marker='o')
        plt.legend()
        plt.xlabel('Distance along propagation direction [m]')
        plt.ylabel('Arrival time [ns]')
        #plt.plot(a, expectedDelays*1e9, c='g')
        plt.title('Arrival times versus distance along propagation direction')
        plt.figure()

        for i in range(len(stationList)):
            start = stationStartIndex[i]
            end = stationStartIndex[i+1]
            plt.scatter(p[start:end], goodTimes[start:end] * 1e9, 20, label=stationList[i], c = colors[i], marker = 'o')

        plt.legend()
        plt.xlabel('Distance along propagation direction [m]')
        plt.ylabel('Arrival time [ns]')
        plt.title('Arrival times versus distance perpendicular to propagation direction')

        plt.figure()
        for i in range(len(stationList)):
            start = stationStartIndex[i]
            end = stationStartIndex[i+1]
            plt.scatter(a[start:end], goodResidues[start:end] * 1e9, 20, label=stationList[i], c = colors[i], marker='o')
        plt.legend()
        plt.xlabel('Distance along propagation direction [m]')
        plt.ylabel('Arrival time minus plane-wave [ns]')

        plt.title('Second-order wavefront, along direction of propagation')
        plt.figure()
        for i in range(len(stationList)):
            start = stationStartIndex[i]
            end = stationStartIndex[i+1]
            plt.scatter(p[start:end], goodResidues[start:end] * 1e9, 20, label=stationList[i], c=colors[i], marker='o')
        plt.legend()
        plt.xlabel('Distance along propagation direction [m]')
        plt.ylabel('Arrival time minus plane-wave [ns]')

        plt.title('Second-order wavefront, perpendicular to propagation')
#        ldf_raw = cr.trerun("Shower", "z", positions=goodPositions2D, signals=goodTimes, core =core, direction=planarDirection, ldf_enable=True, ldf_logplot=False, footprint_enable=False, ldf_remove_outliers=False, ldf_total_signal=True)

        # Simplex fit point source...
#        (az, el) = direction_fit_plane_wave.meandirection_azel  # check
        startPosition = (az, el, 1.0)  # 1.0 means R = 1000.0 ...
        print 'Doing simplex search for minimum MSE...'
        times -= times[0]
        optimum = fmin(mseMinimizer, startPosition, (goodPositions, goodTimes, 0, 4), xtol=1e-5, ftol=1e-5, full_output=1)
        # raw_input('Done simplex search.')

        simplexDirection = optimum[0]
        simplexMSE = optimum[1]
        simplexDirection[2] = 1000.0 / simplexDirection[2]  # invert again to get R
        (simplexAz, simplexEl, simplexR) = simplexDirection

        print '-----'
        print 'Simplex search: '
        print 'Best MSE: %f; best direction = (%f, %f, %f)' % (simplexMSE, simplexAz / deg2rad, simplexEl / deg2rad, simplexR)
        msePlanar = sf.mseWithDistance(simplexAz, simplexEl, 1.0e6, goodPositions, goodTimes, allowOutlierCount=4)
        print 'Best MSE for R = 10^6 (approx. planar): %f' % msePlanar
        print '-----'
        # get the calculated delays according to this plane wave
        # simplexDirection[2] = 4000.0
        bestfitDelays = sf.timeDelaysFromDirectionAndDistance(goodPositions, simplexDirection)
        self.pointsourceArrivalTimes = bestfitDelays

        residu = goodTimes - bestfitDelays
        residu -= min(residu)

        # Difference between measured arrival times and point source fit. Check plot for fit errors!

        fptask_delta_pointsource = cr.trerun("Shower", "4", positions=goodPositions2D, signals=goodSignals, timelags=residu, footprint_colormap='jet', footprint_enable=True, footprint_shower_enable=False)

        plt.title('Footprint of residual delays w.r.t. point source fit')

        """
        see test script testCrosscorr_MultiTBB.py for commented-out stuff

        ysteps = 50
        imarray = np.zeros((xsteps, ysteps))
        bestCore = None
        bestChi2 = 1.0e9
        for x in range(xsteps):
            print x
            for y in range(ysteps):
                core = np.array([-150.0 + 300.0 * float(x)/xsteps, -150.0 + 300.0 * float(y)/ysteps, 0.0])
                chi_squared = fitQualityFromCore(core, az, el, goodPositions2D, goodTimes)[0]
                imarray[x, y] = chi_squared
                if chi_squared < bestChi2:
                    bestChi2 = chi_squared
                    bestCore = core

        plt.figure()
        plt.imshow(imarray, cmap=plt.cm.hot_r, extent=(-150.0, 150.0, -150.0, 150.0), origin='lower')
        plt.colorbar()

        fakecoreX = 0.0
        fakecoreY = 0.0
        for pos in goodPositions2D:
            fakecoreX += pos[0]
            fakecoreY += pos[1]
        fakecore = (1.0 / len(goodPositions2D)) * np.array([fakecoreX, fakecoreY, 0.0])
        print 'Fit-center core: x = %3.2f, y = %3.2f' % (fakecore[0], fakecore[1])
#        core = np.array([-132.5, 101.8, 0.0])
        core = bestCore

        """
