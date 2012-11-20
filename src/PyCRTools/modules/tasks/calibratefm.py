"""
Task: 

Calibrate delays between antennas, and especially inter-station delays, using narrow-band radio transmitters.
Especially transmitters with a known location are useful, e.g. the FM tower in Smilde (GPS long = 6.403565 E, lat = 52.902671 N).

Uses the phases (average and rms) obtained in the FindRFI Task; or will re-run that task if no phases are given
as input.

======================

First version (rfilines Task) by Arthur Corstanje, June 2012.
CalibrateFM Task by Arthur Corstanje, Nov 2012.
"""

from pycrtools.tasks import Task
from pycrtools.tasks.shortcuts import * # only needed when using {default: None, ...} parameter style
# using dict(boe = something) instead is better...
import numpy as np
import pycrtools as cr
import matplotlib.pyplot as plt
import pycrtools as cr
from pycrtools import srcfind as sf
#import pytmf
import datetime

twopi = 2 * np.pi
deg2rad = twopi / 360.0
halfpi = twopi / 4


def getOneSampleShifts(timeDiff, stationStartIndex, interStationDelays):
    """
    Scan the array of time differences (between measured phases and the modeled incoming wave)
    for presence of offsets that are a multiple of 5 ns.
    Note: due to the modulo-2pi from the phases, only small multiples (effectively +/- 1) can be 
    reliably corrected.
    
    Input stationStartIndex contains the starting points in the array for each station.
    interStationDelays are subtracted here.
    
    Returns array oneSampleShifts, containing integer # samples by which the data is shifted. 
    So when it returns [0, 0, 1, 0, -1, ...] a positive number means that the data has to be advanced 1 sample
    i.e. sample 0 <-- sample 1
    Negative number the other way round.
    
    (NB. check!)
    
    """
    
    n = len(timeDiff)
    delays = np.zeros(n)
    oneSampleShifts = np.zeros(n)
    nofStations = len(stationStartIndex) - 1
    for i in range(nofStations):
        start = stationStartIndex[i]
        end = stationStartIndex[i+1]
        for j in range(start, end):
            #delays[j] = timeDiff[j] - interStationDelays[i]
            thisDelay = timeDiff[j] - interStationDelays[i]
            if abs(thisDelay) > 3:  # 3 ns threshold to call it a glitch
                oneSampleShifts[j] = int(round(thisDelay / 5)) 
    
    return oneSampleShifts


class CalibrateFM(Task):
    """
    **Description:**

    Get cable delays.

    **Usage:**

    **See also:**
    :class:`plotfootprint`

    **Example:**

    ::
        filefilter="/Volumes/Data/sandertv/VHECR/LORAtriggered/results/VHECR_LORA-20110716T094509.665Z/"
        crfootprint=cr.trun("plotfootprint",filefilter=filefilter,pol=polarization)
   """
    parameters=dict(
        filefilter={default:None,doc:"File filter for multiple data files in one event, e.g. '/my/data/dir/L45472_D20120206T030115.786Z*.h5' "},
        filelist = {default: None, doc: "List of filenames in one event. "},
        f = dict( default = None, doc = "File object." ),
        
        #outputDataFile = {default: None, doc: "Optional: output text file where results can be written."},
        #batch = {default: False, doc: "Suppress plot display when set to True."},
        averagePhases = {default: None, doc: "Average phase spectrum per antenna, as output from FindRFI Task. Optional, if not given 'filefilter' or 'filelist' will be used to (re)run FindRFI." },
        medians = {default: None, doc: "Output from FindRFI: Median (over all antennas) standard-deviation, per frequency channel. 1-D array with length blocksize/2 + 1." },
        referenceTransmitterGPS = {default: None, doc: "GPS [long, lat] in degrees (N, E is positive) for a known transmitter. Typically used when tuning to a known frequency. The Smilde tower is at (6.403565, 52.902671)."},

        doplot = {default:True, doc:"Produce output plots"},
        testplots = {default: False, doc: "Produce testing plots for calibration on RF lines"},
        pol={default:0,doc:"0 or 1 for even or odd polarization. Only used when re-running FindRFI."},
#        maxlines = {default: 1, doc: "Max number of RF lines to consider"},
        lines = {default: None, doc: "(List of) RF line(s) to use, by frequency channel index"},
#        antennaselection = {default: None, doc: "Optional: list of antenna numbers (RCU/2) to include"},
#        minSNR = {default: 50, doc: "Minimum required SNR of the line"},
        blocksize = {default: 8192, doc: "Blocksize of timeseries data, for FFTs. Only used when no averagePhases are given."},
        nofblocks = {default:100, doc: "Max. number of blocks to process"},
#        smooth_width = {default: 16, doc: "Smoothing window for FFT, for determining base level (cheap way of doing what AverageSpectrum class does)"},
        correctOneSampleShifts = {default: False, doc: "Automatically correct for +/- 5 ns shifts in the data. Only works correctly if the direction fit / the reference transmitter GPS is good. Output is in oneSampleShifts list."},
        direction_resolution = {default: [1, 5], doc: "Resolution in degrees [az, el] for direction search"},
        directionFromAllStations = {default: False, doc: "Set True if you want to use all stations together to calculate the incoming direction. Otherwise, it is done per station and the results are averaged. "},
        freq_range = {default: [30, 70], doc: "Frequency range in which to search for calibration sources"}, 
        # OUTPUT PARAMETERS:
        nofchannels = {default: -1, doc: "nof channels", output:True},
        dirtychannels = {default: None, doc: "Output array of dirty channels, based on stability of relative phases from consecutive data blocks. Deviations from uniform-randomness are quite small, unless there is an RF transmitter present at a given frequency.", output:True},
        oneSampleShifts = {default: None, doc: "Output array containing the 5 ns-shifts per antenna, if they occur. Output contains integer number of samples the data is shifted. To correct for it, shift data _forward_ by this number of samples.", output: True},
        interStationDelays = {default: None, doc: "Output dict containing station name - delay value pairs.", output: True},
        maxPhaseError = {default: 1.0, doc: "Maximum allowed phase error in ns^2 (added from all antennas) to call it a good fit and set calibrationStatus to 'OK'."},
        phaseError = {default: None, doc: "Phase error for the first station in the list. Used to check if the fit is good, e.g. when using fixed reference transmitter", output: True},
        calibrationStatus = {default: None, doc: "Output status string. ", output: True},         
        strongestDirection = {default: None, doc: "Output list [az, el] with the direction of the strongest transmitter", output: True},
        strongestFrequency = {default: None, doc: "The frequency with the smallest phaseRMS, from the range specified in the freq_range parameter", output: True},
        bestPhaseRMS = {default: None, doc: "PhaseRMS for the best frequency", output: True},
        timestamp = {default: None, doc: "Unix timestamp of input file(s)", output: True},
#        results=p_(lambda self:gatherresults(self.topdir, self.maxspread, self.antennaSet),doc="Results dict containing cabledelays_database, antenna positions and names"),
#        positions=p_(lambda self:obtainvalue(self.results,"positions"),doc="hArray of dimension [NAnt,3] with Cartesian coordinates of the antenna positions (x0,y0,z0,...)"),
#        antid = p_(lambda self:obtainvalue(self.results,"antid"), doc="hArray containing strings of antenna ids"),
#        names=p_(lambda self:obtainvalue(self.results,"names"),doc="hArray of dimension [NAnt] with the names or IDs of the antennas"),
        plot_finish={default: lambda self:cr.plotfinish(doplot=True,filename="rfilines",plotpause=False),doc:"Function to be called after each plot to determine whether to pause or not (see ::func::plotfinish)"},
        plot_name={default:"rfilines",doc:"Extra name to be added to plot filename."},
        nofantennas=p_(lambda self: self.positions.shape()[-2],"Number of antennas.",output=True),
        filetype={default:"pdf",doc:"extension/type of plot output files"},
        save_images = {default:False,doc:"Enable if images should be saved to disk in default folder"},
#        generate_html = {default:False,doc:"Default output to altair webserver"}
        
        )
                
    def call(self):
        pass

    def run(self):

        if self.filelist and self.filefilter:
            raise RuntimeError("Both filelist and filefilter given; provide only one of them")

        if not self.doplot and self.testplots:
            print 'Warning: setting self.testplots to False because self.doplot = False'
            self.testplots = False
        
        if not self.f or not (self.averagePhases and self.medians): # re-run FindRFI to get those.
            if self.filelist:
                filelist = self.filelist
            else:
                filelist = cr.listFiles(self.filefilter)
            superterpStations = ["CS002", "CS003", "CS004", "CS005", "CS006", "CS007", "CS021"]
            if len(filelist) == 1:
                print 'There is only 1 file'
                filelist = filelist[0]
            else: # sort files on station ID
                sortedlist = []
                for station in superterpStations:
                    thisStationsFile = [filename for filename in filelist if station in filename]
                    if len(thisStationsFile) > 0:
                        sortedlist.append(thisStationsFile[0])
                filelist = sortedlist
            print '[rfilines] Processing files: '
            print filelist
            self.f = cr.open(filelist, blocksize = self.blocksize)
            findrfi = cr.trerun("FindRFI", "0", f = self.f) # do all blocks by default. Do both polarisations.
            # select channels with polarisation 'self.pol'
#            if self.antennaselection:
#                selected_dipoles = [x for x in self.f["DIPOLE_NAMES"] if (int(x) % 2 == self.pol) and ((int(x) % 100) / 2 in self.antennaselection)]
#            else:
            selected_dipoles = [x for x in self.f["DIPOLE_NAMES"] if int(x) % 2 == self.pol]
            
            self.f["SELECTED_DIPOLES"] = selected_dipoles
    # test shift:        f.shiftTimeseriesData([-1, 1, 0, -1, 0, -1])
            self.nofchannels = len(selected_dipoles)
            print '# channels = %d' % self.nofchannels
            
            # Get output of FindRFI into our parameter space. Apply channel selection for polarisation
            findrfi_channels = findrfi.nantennas # total nr. of channels
            thisPolsChannels = range(self.pol, findrfi_channels, 2)
            a = findrfi.phase_average[thisPolsChannels, ...]
            self.averagePhases = cr.hArray(copy = a)
            a = findrfi.medians[thisPolsChannels]
            self.medians = cr.hArray(copy = a)
            

        # get calibration delays from file
        caldelays = cr.hArray(self.f["DIPOLE_CALIBRATION_DELAY"]) # f[...] outputs list!
        # add subsample clock offsets to caldelays
#        if isinstance(filelist, list):
        if isinstance(self.f, cr.io.tbb.MultiTBBData):
            subsample_clockoffsets = self.f["SUBSAMPLE_CLOCK_OFFSET"]
            station_startindex = self.f["STATION_STARTINDEX"]
            for i in range(len(subsample_clockoffsets)):
                start = station_startindex[i]
                end = station_startindex[i+1]
                caldelays[start:end] += subsample_clockoffsets[i]
            
        # make calibration phases, i.e. convert delays to phases for every frequency
        freqs = self.f["FREQUENCY_DATA"]
        calphases = hArray(float, dimensions = [self.nofchannels, len(freqs)]) # check dim.

        cr.hDelayToPhase(calphases, freqs, caldelays) # check!
        # Use these calphases instead of those below for a single freq. Check if output is the same...
#        import pdb; pdb.set_trace()
        
#        self.phase_average = cr.hArray(copy = phaseAvg)
           
        if not self.lines: # if no value given, take the one with best phase stability
            if not self.freq_range: # take overall best channel
                bestchannel = medians.argmin()
            else: # take the best channel in the given range
                f0 = 200.0e6 / self.blocksize # hardcoded sampling rate
                startindex = int(self.freq_range[0] * 1.0e6 / f0)
                stopindex = 1 + int(self.freq_range[1] * 1.0e6 / f0) 
                bestchannel = startindex + medians[startindex:stopindex].argmin()

            bestchannel = int(bestchannel) # ! Needed for use in hArray slicing etc. Type numpy.int64 not recognized otherwise
        elif type(self.lines) == type([]):
            bestchannel = self.lines[0] # known strong transmitter channel for this event (may vary +/- 5 channels?)
        else: # either list or number assumed
            bestchannel = self.lines

        channelsSortedByStability = medians.argsort()
        freqsByStability = freqs.toNumpy()[channelsSortedByStability]
        print 'The best 10 channels: '
        for i in range(10):
            print 'Channel %d, freq %2.3f: phase RMS = %1.4f' % (channelsSortedByStability[i], freqsByStability[i] / 1.0e6, medians[channelsSortedByStability[i]])

        print ' Median phase-sigma is lowest (i.e. best phase stability) at channel %d, value = %f' % (bestchannel, medians[bestchannel])

#        if self.testplots:
#            # test phase of one antenna in consecutive data blocks, show in plot versus block nr.
#            plt.figure()
#            x = []
#            for block in phaseblocks:
#                x.append(block[2, bestchannel])
#            x = np.array(x)
#            plt.plot(x)
#            plt.title('Phase of one antenna for freq channel %d in consecutive data blocks' % bestchannel)        
            
#        if self.testplots:
#            # diagnostic test, plot avg phase, also plot all / many individual measured phases
#            # at the frequency that gives the best phase stability
#            plt.figure()
#            x = phaseAvg.toNumpy()[:, bestchannel]
#            #...
#            plt.plot(x)
 
#        if self.doplot:
        # do calibration plots
        freqs = self.f["FREQUENCY_DATA"]
        freq = freqs[bestchannel]

        # apply calibration phases here (?) See if that is different from doing it to all phases before...
        calphases = (twopi * freq) * caldelays
        print calphases
        for i in range(self.nofchannels):
            self.averagePhases[i, bestchannel] += calphases[i]
            #if i > 0: # Needed????
             #   self.averagePhases[i, bestchannel] -= 0.3
        #self.averagePhases[0, bestchannel] = 0.0
        cr.hPhaseWrap(self.averagePhases, self.averagePhases)
        print '---'
#        print self.averagePhases

        # FIX: condition only if no reference transmitter given...
        # find direction of this source; plot image to check quality of fit.
        print 'Find direction of source based on averaged phases per antenna...'
        
        allpositions = self.f["ANTENNA_POSITIONS"].toNumpy()
        azSteps = int(360 / self.direction_resolution[0])
        elSteps = int(90 / self.direction_resolution[1])
        
        if not isinstance(filelist, list):
        # for one file being processed
            positions = allpositions.ravel() # only pol 'polarisation'
            # NB. Indexing to the end is done by thisArray[start::step] !
            
            averagePhasePerAntenna = self.averagePhases.toNumpy()[:, bestchannel]
            (fitaz, fitel, minPhaseError) = sf.directionBruteForcePhases(positions, averagePhasePerAntenna, freq, azSteps = azSteps, elSteps = elSteps, allowOutlierCount = 4, showImage = (self.doplot and self.testplots), verbose = True)
            if self.doplot and self.testplots:
                self.plot_finish(filename=self.plot_name + "-phaseimage",filetype=self.filetype)

            print 'Best fit az = %f, el = %f' % (fitaz / deg2rad, fitel / deg2rad)
            print 'Phase error = %f' % minPhaseError
            
            if not self.referenceTransmitterGPS:
                modelphases = sf.phasesFromDirection(positions, (fitaz, fitel), freq)
                modelphases -= modelphases[0]
                modelphases = sf.phaseWrap(modelphases) # have to wrap again as we subtracted the ref. phase
            else:
                modelphases = sf.timeDelaysFromGPSPoint(self.referenceTransmitterGPS, positions, np.array(self.f["CHANNEL_ID"]), self.f["ANTENNA_SET"])
                modelphases -= modelphases[0]
                modelphases *= - twopi * freq # !! Minus sign to be compatible with FFT's phase sign
                modelphases = sf.phaseWrap(modelphases)
            
            if self.testplots:
                plt.figure()
                plt.plot(modelphases, label='Modeled phases (plane wave)')
                plt.plot(averagePhasePerAntenna, label='Avg. measured phases')
                plt.legend()
                self.plot_finish(filename=self.plot_name + "-avg-measuredphases",filetype=self.filetype)

            phaseDiff = averagePhasePerAntenna - modelphases

            nanosecondPhase = twopi * freq * 1.0e-9
            timeDiff = sf.phaseWrap(phaseDiff) / nanosecondPhase
            
            if self.doplot:
                plt.figure()
                
                plt.plot(timeDiff, label='Measured - expected phase')

                #plt.figure()
                rms_phase = phaseRMS.toNumpy()[:, bestchannel]
                plt.plot(rms_phase, 'r', label='RMS phase noise')
                plt.plot( - rms_phase, 'r')
                plt.title(self.filefilter+'\nPhase difference between measured and best-fit modeled phase\nChannel %d,   f = %2.4f MHz,   pi rad = %1.3f ns' % (bestchannel, freq/1.0e6, 1.0e9 / (2 * freq)))
                plt.ylabel('Time difference from phase [ns]')
                plt.xlabel('Antenna number (RCU/2)')
                plt.legend()
                
                self.plot_finish(filename=self.plot_name + "-calibration-phases",filetype=self.filetype)
            
                cr.trerun("PlotAntennaLayout","0", positions = self.f["ANTENNA_POSITIONS"], colors = cr.hArray(list(timeDiff)), sizes=100, title="Measured - expected time",plotlegend=True)
            if self.outputDataFile:
                outf = open(self.outputDataFile, 'a') # Append to file
                timestamp = self.f["TIME"][0]
                datestring = datetime.datetime.fromtimestamp(timestamp).strftime('%Y-%m-%d')
                timestring = datetime.datetime.fromtimestamp(timestamp).strftime('%H:%M:%S')
                bestFrequency = freqs[bestchannel] / 1.0e6

                outstr = filelist + ' ' + datestring + ' ' + timestring + ' ' + str(timestamp) + ' ' + format('%3.4f' % bestFrequency) + ' ' + format('%3.2f' % (fitaz / deg2rad)) + ' '
                outstr += format('%3.2f' % (fitel / deg2rad)) + ' ' + format('%3.4f' % minPhaseError) + ' '
                outstr += format('%3.6f' % medians[bestchannel])
                
                outf.write(outstr + '\n')
                outf.close()

        else:
            # get list of stations in this dataset
            stationlist = self.f["STATION_LIST"]
            stationStartIndex = self.f["STATION_STARTINDEX"]
            nofStations = len(stationlist)
            directions = []
            if not self.referenceTransmitterGPS:
            # Get direction fits for each station if no fixed reference transmitter is given.
                for i in range(nofStations):
                    print 'Processing station: %s' % stationlist[i]
                    start = stationStartIndex[i]
                    end = stationStartIndex[i+1]
                    thesePositions = allpositions[start:end]
                    thesePositions = thesePositions.ravel()

                    averagePhasePerAntenna = self.averagePhases.toNumpy()[start:end, bestchannel]
                    plt.figure()
                    (fitaz, fitel, minPhaseError) = sf.directionBruteForcePhases(thesePositions, averagePhasePerAntenna, freq, azSteps = azSteps, elSteps = elSteps, allowOutlierCount = 4, showImage = (self.doplot and self.testplots), verbose = True)
                    if self.doplot and self.testplots:
                        self.plot_finish(filename=self.plot_name + "-phaseimage",filetype=self.filetype)                   
                    
                    print 'Best fit az = %f, el = %f' % (fitaz / deg2rad, fitel / deg2rad)
                    print 'Phase error = %f' % minPhaseError                    
                    directions.append((fitaz, fitel))
                    
                averageIncomingDirection = vectorAverage(directions)
            
    # HACK: do incoming direction using all stations together and see if that gets better results...
                if self.directionFromAllStations:
                    averagePhasePerAntenna = self.averagePhases.toNumpy()[:, bestchannel]
                    plt.figure()
                    (fitaz, fitel, minPhaseError) = sf.directionBruteForcePhases(allpositions.ravel(), averagePhasePerAntenna, freq, azSteps = azSteps, elSteps = elSteps, allowOutlierCount = 4*nofStations, showImage = (self.doplot and self.testplots), verbose = False)
                    averageIncomingDirection = (fitaz, fitel)
                
                print 'Averaged incoming direction: az = %f, el = %f' % (averageIncomingDirection[0] / deg2rad, averageIncomingDirection[1] / deg2rad)
            # get modeled phases for a plane wave of given overall direction, for all stations together
            allpositions = allpositions.ravel()
            
            if not self.referenceTransmitterGPS:
                modelphases = sf.phasesFromDirection(allpositions, averageIncomingDirection, freq)
                modelphases -= modelphases[0]
                modelphases = sf.phaseWrap(modelphases) # have to wrap again as we subtracted the ref. phase
            else:
                #import pdb; pdb.set_trace()
                modelphases = sf.timeDelaysFromGPSPoint(self.referenceTransmitterGPS, allpositions, np.array(self.f["SELECTED_DIPOLES"]), self.f["ANTENNA_SET"][0])
                modelphases -= modelphases[0]
                modelphases *= - twopi * freq # !! Minus sign to be compatible with FFT's phase sign
                modelphases = sf.phaseWrap(modelphases)
                
            averagePhasePerAntenna = self.averagePhases.toNumpy()[:, bestchannel] # now for all antennas
            
            phaseDiff = averagePhasePerAntenna - modelphases
            phaseError = sf.phaseErrorFromDifference(phaseDiff[0:stationStartIndex[1]], freq, allowOutlierCount = 4)
            
            print 'Phase error for first station: %3.3f' % phaseError
            self.phaseError = phaseError
            self.calibrationStatus = "OK" if phaseError < self.maxPhaseError else "BAD"
            print 'Fit is %s as the max allowed phase error is set to %3.3f' % (self.calibrationStatus, self.maxPhaseError)
            nanosecondPhase = twopi * freq * 1.0e-9
            timeDiff = sf.phaseWrap(phaseDiff) / nanosecondPhase

            interStationDelays = np.zeros(nofStations)
            refdelay = 0.0
            if self.doplot:
                plt.figure()

            for i in range(nofStations):
                start = stationStartIndex[i]
                end = stationStartIndex[i+1]
                interStationDelays[i] = np.median(timeDiff[start:end])
                if i == 0:
                    refdelay = interStationDelays[i]
                    interStationDelays[i] -= refdelay
                    if self.doplot:
                        plt.plot(np.array([start, end]), np.array([interStationDelays[i], interStationDelays[i]]), c='g', lw=3, label='Median station delay')
                else:
                    interStationDelays[i] -= refdelay
                    if self.doplot:
                        plt.plot(np.array([start, end]), np.array([interStationDelays[i], interStationDelays[i]]), c='g', lw=3)
#                        plt.annotate(stationlist[i])
            # Subtract reference station-delay
            timeDiff -= refdelay   
            print '--- Inter-station delays: ---'
            self.interStationDelays = {}
            for i in range(nofStations):
                self.interStationDelays[stationlist[i]] = interStationDelays[i] # write to Task output param
                print '%s: %2.3f ns' % (self.f["STATION_LIST"][i], interStationDelays[i])

            if self.correctOneSampleShifts:
                self.oneSampleShifts = getOneSampleShifts(timeDiff, stationStartIndex, interStationDelays)
            else:
                self.oneSampleShifts = np.zeros(len(timeDiff))
            timeDiff_glitches = np.zeros(len(timeDiff)) + np.float('nan')
            timeDiff_fixed = timeDiff - 5.0 * self.oneSampleShifts
            for i in range(len(timeDiff)): # separate fixed data, real data and glitches
                if timeDiff[i] != timeDiff_fixed[i]:
                    timeDiff_glitches[i] = timeDiff[i]
                                
#            timeDiff -= self.oneSampleShifts * 5.0
            #interStationDelays -= interStationDelays[0]
            if self.doplot:
                plt.plot(timeDiff_fixed, 'o-', c = 'b', label = 'Measured - expected phase')
                if self.correctOneSampleShifts:
                    plt.plot(timeDiff_glitches, 'o-', c = 'r', label='5 ns shifts found')
                #plt.figure()
                rms_phase = phaseRMS.toNumpy()[:, bestchannel]
                plt.plot(rms_phase, 'r', label='RMS phase noise')
                plt.plot( - rms_phase, 'r')
                plt.title(self.filefilter+'\nPhase difference between measured and best-fit modeled phase\nChannel %d,   f = %2.4f MHz,   pi rad = %1.3f ns' % (bestchannel, freq/1.0e6, 1.0e9 / (2 * freq)))
                plt.ylabel('Time difference from phase [ns]')
                plt.xlabel('Antenna number (RCU/2)')
                plt.legend()
                
                self.plot_finish(filename=self.plot_name + "-calibration-phases",filetype=self.filetype)

                cr.trerun("PlotAntennaLayout","0", positions = self.f["ANTENNA_POSITIONS"], colors = cr.hArray(list(timeDiff)), sizes=100, title="Measured - expected time",plotlegend=True)
                
                self.plot_finish(filename = self.plot_name + "-calibration-layout", filetype=self.filetype)

        # set output params
#        self.oneSampleShifts = getOneSampleShifts(timeDiff, stationStartIndex, interStationDelays)
#        print self.oneSampleShifts
        self.strongestFrequency = freqs[bestchannel]
        #self.strongestDirection = [fitaz / deg2rad, fitel / deg2rad]
        self.bestPhaseRMS = medians[bestchannel]
        self.timestamp = self.f["TIME"][0]
        #print self.interStationDelays
