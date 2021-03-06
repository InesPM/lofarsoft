"""
Test script to gather processed timeseries data from results directory of a given event.
Reprocess cross correlation using MultiTBBData datareader, which reads and bundles data from all
datafiles for a given event.

Reprocess cross correlation over all stations (polarisations separately).
Obtain arrival times; apply calibration delays and inter-station delays.

Get wavefront shape and fit curvature using point-source approximation.

later?: Reprocess cross correlation over 0/1 polarisation inside each station.
======================

Created by Arthur Corstanje, Apr. 2012
"""

from pycrtools import hArray
from pycrtools import srcfind as sf
import pycrtools as cr
import pycrtools.tasks as tasks
import os
from pycrtools.tasks.shortcuts import *
from pycrtools import metadata as md

import numpy as np
from scipy.optimize import fmin
import matplotlib.pyplot as plt
from datetime import datetime
from pycrtools import xmldict

deg2rad = np.pi / 180

def mseMinimizer(direction, pos, times, outlierThreshold=0, allowOutlierCount=0):
    # Quality function for simplex search 
    az = direction[0]
    el = direction[1]
    R = 1000.0 / direction[2] # using 1/R as parameter - curvature dependence is on 1/R to lowest order; will work better in simplex search.
    mse = sf.mseWithDistance(az, el, R, pos, times, outlierThreshold, allowOutlierCount)
    print 'Evaluated direction: az = %f, el = %f, R = %f: MSE = %f' % (az / deg2rad, el / deg2rad, R, mse)

    return mse

eventdir = '/Users/acorstanje/triggering/CR/results_withcaltables/VHECR_LORA-20110714T174749.986Z'
#eventdir = '/Users/acorstanje/triggering/CR/testRFIevent'
#eventdir = '/data/VHECR/Radiotriggered/results_mailed/L42358_D2012'
def gatherresults(eventdir):
    """
    Gather results from 1st pipeline analysis if status is 'good', group into a list of dicts containing
    - station name
    - results pol 0
    - results pol 1
    - data filename
    """
#    results = {}
    
    outputstring = "" # make output string summarizing outlier counts etc.

    stations = []

    print "Processing event in directory:",eventdir
    if os.path.isdir(os.path.join(eventdir,"pol0")) or os.path.isdir(os.path.join(eventdir, "pol1")):
        datadirs=cr.listFiles(os.path.join(os.path.join(eventdir,"pol?"),"*"))

    for datadir in datadirs:
        if not os.path.isfile(os.path.join(datadir,"results.xml")):
            continue # doesn't matter if 'datadir' isn't a directory...
        resfile = os.path.join(datadir,"results.xml")
        try:
            res = xmldict.load(os.path.join(datadir,"results.xml"))
        except Exception as e:
            print "File skipped, NaN found or whatever!"
            print "Error = ", e
            continue

        print 'Antenna set: %s ' % res["ANTENNA_SET"]
        # check result status
        noutliers = res["delay_outliers"] if "delay_outliers" in res.keys() else 0 
        print '# outliers = %d' % noutliers
        if res["delay_quality_error"] >= 1 or noutliers > 5: # have status criterion in pipeline...
            print 'File status not OK, dir = %s, delay quality = %s, # outliers = %s' % (datadir, res["delay_quality_error"], noutliers)
            continue
        # get station name
        firstantenna = int(res["antennas"][0])
        thisStationName = md.idToStationName(firstantenna / 1000000)
        fromlist = [x for x in stations if x["stationName"] == thisStationName]
        if len(fromlist) > 0:
            thisDict = fromlist[0]
        else:
            thisDict = dict(stationName = thisStationName)
            stations.append(thisDict)
        pol = res["polarization"]
        thisDict[pol] = res # adds the key 0 or 1 as number
        thisDict.update(filename = os.path.join(res["filedir"], res["FILENAME"]))
        
        timeseriesFilename = os.path.splitext(res["FILENAME"])[0] + '-pol'+str(pol) + '-calibrated-timeseries-cut.pcr'
        thisTimeseries = cr.hArrayRead(os.path.join(datadir, timeseriesFilename))
        thisDict[pol].update(timeseries = thisTimeseries)
    
    return stations

#        positions.extend(res["antenna_positions_array_XYZ_m"]) # in flat list NB! All antennas? Same order as for timeseries data?
        
        # timeseries inlezen, orig of calibrated? Zie evt cr_physics.py.
        # ....    

stations = gatherresults(eventdir)
# now accumulate arrays with:
# - all timeseries data for all stations
# - all corresponding antenna ids and positions
# - all calibration delays, and (station) clock offsets for those ids
# - array of all reference offsets per antenna id. (starting point of timeseries array)

pol = 1 # later do both together

filefilter = '/Users/acorstanje/triggering/CR/*.986Z*.h5'
filelist = cr.listFiles(filefilter)
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

firstDataset = stations[0][pol] # obtained from cr_event results (1st stage pipeline), used to locate pulse.
# assume file shifts are of the order ~ 200 samples << blocksize for cut-out timeseries
block = firstDataset["BLOCK"]
blocksize = firstDataset["BLOCKSIZE"]
#tbb_samplenr = thisDataset["SAMPLE_NUMBER"]
pulse_samplenr = firstDataset["pulse_start_sample"]

f = cr.open(filelist, blocksize = 65536) # need to cut out timeseries around pulse
# select only even/odd antennas according to 'pol'
selected_dipoles = [x for x in f["DIPOLE_NAMES"] if int(x) % 2 == pol]
f["SELECTED_DIPOLES"] = selected_dipoles
antennaPositions = f["ANTENNA_POSITIONS"]

timeseries = f.empty("TIMESERIES_DATA")

# get timeseries data with pulse, then cut out a region around the pulse.
cutoutSize = 256
f.getTimeseriesData(timeseries, block = block)
nofChannels = timeseries.shape()[0]
cutoutTimeseries = hArray(float, dimensions = [nofChannels, cutoutSize])
start = pulse_samplenr - cutoutSize / 2
end = pulse_samplenr + cutoutSize / 2
cutoutTimeseries[...].copy(timeseries[..., start:end])
y = cutoutTimeseries.toNumpy()
plt.plot(y[0])
plt.plot(y[140])

refant = firstDataset["pulses_refant"]
sample_interval = 5.0e-9
# is the index of the ref antenna also in the full list of antids / antpos

# now cross correlate all channels in full_timeseries, get relative times
crosscorr = cr.trerun('CrossCorrelateAntennas', "crosscorr", cutoutTimeseries, oversamplefactor=64)

#And determine the relative offsets between them
maxima_cc = cr.trerun('FitMaxima', "Lags", crosscorr.crosscorr_data, doplot = True, plotend=5, sampleinterval = sample_interval / crosscorr.oversamplefactor, peak_width = 11, splineorder = 3, refant = refant)

#print startTimes
#print maxima_cc.lags

# plot lags, plot flagged lags from a k-sigma criterion on the crosscorr maximum

plt.figure()
hArray(maxima_cc.lags).plot()

# Plot arrival times, do plane-wave fit, plot residuals wrt plane wave
arrivaltime = hArray(maxima_cc.lags)
times = arrivaltime.toNumpy()
positions = antennaPositions.toNumpy().ravel()

# HACK out outlying values
arrivaltime[206] = 0.0

# Apply calibration delays (per antenna)
arrivaltime -= hArray(f["DIPOLE_CALIBRATION_DELAY"])
# Apply sub-sample inter-station clock offsets (LOFAR)
subsampleOffsets = f["SUBSAMPLE_CLOCK_OFFSET"]
# Apply inter-station delays from RFIlines Task
# Assuming ordering CS002, 3, 4, 5, 7 for this event (sorted alphabetically)
#import pdb; pdb.set_trace()
# Hack values in here
subsampleOffsets[1] -= 0.11e-9
subsampleOffsets[2] -= -1.30e-9
subsampleOffsets[3] -= -1.32e-9
subsampleOffsets[4] -= 0.71e-9

stationList = f["STATION_LIST"]
print stationList
stationStartIndex = f["STATION_STARTINDEX"]
for i in range(len(subsampleOffsets)):
    arrivaltime[stationStartIndex[i]:stationStartIndex[i+1]] -= subsampleOffsets[i]
    # Sign + or - ???

#plt.figure()
#arrivaltime.plot()
#plt.title('Arrival times, matched with offsets per station (check!)')
times = arrivaltime.toNumpy()
positions = antennaPositions.toNumpy().ravel()

# now make footprint plot of all arrival times
loradir = '/Users/acorstanje/triggering/CR/LORA'
# first show originally derived arrival times
fptask_orig = cr.trerun("plotfootprint", "0", colormap = 'jet', filefilter = eventdir, loradir = loradir, plotlora=False, plotlorashower=False, pol=pol) # no parameters set...
plt.title('Footprint using original cr_event arrival times')
plt.figure()
# now our arrival times and antenna positions

fptask = cr.trerun("plotfootprint", "1", colormap = 'jet', filefilter = eventdir, positions = antennaPositions, arrivaltime=1.0e9*arrivaltime, loradir = loradir, plotlora=False, plotlorashower=False, pol=pol) # no parameters set...
plt.title('Footprint using crosscorrelated arrival times')
delta = arrivaltime - 1.0e-9 * fptask_orig.arrivaltime
delta -= delta.mean()
plt.figure()
fptask_delta = cr.trerun("plotfootprint", "2", colormap = 'jet', filefilter = eventdir, positions = antennaPositions, arrivaltime=1.0e9*delta, loradir = loradir, plotlora=False, plotlorashower=False, pol=pol) # no parameters set...
plt.title('Footprint of difference between cr_event and full-cc method')
plt.figure()
delta.plot()
plt.title('Difference between plotfootprint / cr_event arrival times\nand full crosscorrelation times')

# Do plane-wave direction fit on full arrivaltimes
# Fit pulse direction
print 'Do plane wave fit on full arrival times (cross correlations here)...'
direction_fit_plane_wave = cr.trun("DirectionFitPlaneWave", positions = antennaPositions, timelags = arrivaltime, verbose=True)

plt.figure()
direction_fit_plane_wave.residual_delays.plot()
plt.title('Residual delays after plane wave fit')

print 'Do plane wave fit on stored arrival times, from plotfootprint...'
direction_fit_plane_wave_orig = cr.trun("DirectionFitPlaneWave", positions = fptask_orig.positions, timelags = 1.0e-9 * fptask_orig.arrivaltime, verbose=True)

plt.figure()
direction_fit_plane_wave_orig.residual_delays.plot()
plt.title('Residual delays after plane wave fit\n to plotfootprint timelags')

residu = direction_fit_plane_wave.residual_delays.toNumpy()
#import pdb; pdb.set_trace()
residu[np.where(abs(residu) > 100e-9)] = 0.0 # hack
residu -= residu.mean()
residu[np.where(abs(residu) > 15e-9)] = np.float('nan') # hack

residu[np.argmax(residu)] = 0.0
residu[np.argmin(residu)] = 0.0
residu -= min(residu)

plt.figure()
# now the good one: difference between measured arrival times and plane wave fit!
fptask_delta = cr.trerun("plotfootprint", "3", colormap = 'jet', filefilter = eventdir, positions=antennaPositions, arrivaltime=hArray(1.0e9*residu), power = 140, loradir = loradir, plotlora=False, plotlorashower=False, pol=pol) # no parameters set...
plt.title('Footprint of residual delays w.r.t. planewave fit')


# Simplex fit point source...
(az, el) = direction_fit_plane_wave.meandirection_azel # check
startPosition = (az, el, 1.0) # 1.0 means R = 1000.0 ...
print 'Doing simplex search for minimum MSE...'
optimum = fmin(mseMinimizer, startPosition, (positions, times, 0, 4), xtol=1e-5, ftol=1e-5, full_output=1)
#raw_input('Done simplex search.')

simplexDirection = optimum[0]
simplexMSE = optimum[1]
simplexDirection[2] = 1000.0 / simplexDirection[2] # invert again to get R
(simplexAz, simplexEl, simplexR) = simplexDirection

print '-----'
print 'Simplex search: '
print 'Best MSE: %f; best direction = (%f, %f, %f)' % (simplexMSE, simplexAz / deg2rad, simplexEl / deg2rad, simplexR)
msePlanar = sf.mseWithDistance(simplexAz, simplexEl, 1.0e6, positions, times, allowOutlierCount = 4)
print 'Best MSE for R = 10^6 (approx. planar): %f' % msePlanar
print '-----'
# get the calculated delays according to this plane wave
#simplexDirection[2] = 4000.0
bestfitDelays = sf.timeDelaysFromDirectionAndDistance(positions, simplexDirection)

residu = times - bestfitDelays
#import pdb; pdb.set_trace()
#residu[np.where(abs(residu) > 100e-9)] = 0.0 # hack
residu -= residu.mean()
#residu[np.where(abs(residu) > 15e-9)] = np.float('nan') # hack

residu[np.argmax(residu)] = 0.0
residu[np.argmin(residu)] = 0.0
residu[np.argmax(residu)] = 0.0
residu[np.argmin(residu)] = 0.0
#residu -= min(residu)

plt.figure()
# now the good one: difference between measured arrival times and plane wave fit!
fptask_delta = cr.trerun("plotfootprint", "4", colormap = 'jet', filefilter = eventdir, positions=antennaPositions, arrivaltime=hArray(1.0e9*residu), power = 140, loradir = loradir, plotlora=False, plotlorashower=False, pol=pol) # no parameters set...
plt.title('Footprint of residual delays w.r.t. point source fit')


"""
startTimes = []
nofantennas = 0
sample_interval = 0.0
for dataset in stations:
    thisDataset = dataset[pol]
    stationName = dataset["stationName"]
    
    block = thisDataset["BLOCK"]
    blocksize = thisDataset["BLOCKSIZE"]
    tbb_samplenr = thisDataset["SAMPLE_NUMBER"]
    pulse_samplenr = thisDataset["pulse_start_sample"]
    sample_interval = thisDataset["SAMPLE_INTERVAL"]
    clock_correction = md.getClockCorrection(stationName)
    nofchannels = len(thisDataset["antennas"])
    nofantennas += nofchannels
#    pulse_end = thisDataset["pulse_end_sample"]
    timeOffset = sample_interval * (tbb_samplenr + block * blocksize + pulse_samplenr) + clock_correction
    
    startTimes.extend([timeOffset] * nofchannels) # this offset for every antenna in this dataset

startTimes = hArray(startTimes)

# accumulate ant-ids, ant-positions and timeseries data
antids = []
antpos = []
datalen = stations[0][0]["timeseries"].shape()[1] # and assume the same length for cut-timeseries for all others...
full_timeseries = hArray(float, dimensions = [nofantennas, datalen])
print full_timeseries.shape()
row = 0
for dataset in stations: 
    antids.extend(dataset[pol]["antennas"].values())
    antpos.extend(dataset[pol]["antenna_positions_array_XYZ_m"])
    thisTimeseries = dataset[pol]["timeseries"]
    nofchannels = thisTimeseries.shape()[0]
    print row
    print row+nofchannels
    print thisTimeseries.shape()
    full_timeseries[row:row+nofchannels].copy(thisTimeseries) # check...
    row += nofchannels

antpos = hArray(antpos)
antids = hArray(antids)
# get a reference antenna with a strong pulse...
# for now, just station[0]'s reference antenna... May want to use overall-best pulse (highest SNR)
refant = stations[0][pol]["pulses_refant"]
# is the index of the ref antenna also in the full list of antids / antpos

# now cross correlate all channels in full_timeseries, get relative times
crosscorr = cr.trerun('CrossCorrelateAntennas', "crosscorr", full_timeseries, oversamplefactor=128)

#And determine the relative offsets between them
maxima_cc = cr.trerun('FitMaxima', "Lags", crosscorr.crosscorr_data, doplot = True, plotend=5, sampleinterval = sample_interval / crosscorr.oversamplefactor, peak_width = 11, splineorder = 3, refant = refant)

#print startTimes
#print maxima_cc.lags

# plot lags, plot flagged lags from a k-sigma criterion on the crosscorr maximum

plt.figure()
hArray(maxima_cc.lags).plot()

sdev = crosscorr.crosscorr_data[...].stddev()
maxima = crosscorr.crosscorr_data[...].max() # need to look at positive maximum only!

ksigma = hArray(maxima / sdev)
k = ksigma.toNumpy()

x = np.where(k < 4.5) # bad indices
print 'Number of BAD channels: ', len(x[0])
if len(x[0]) > 0:
    y = hArray(maxima_cc.lags).toNumpy()
    yy = y[x]
    plt.scatter(x, yy)
    plt.title('Delays from cross correlations\nBlue dots represent flagged delays (no clear maximum)')

plt.figure()
hArray(ksigma).plot()
plt.title('Strength (in sigmas) of the crosscorrelation maximum')

arrivaltime = startTimes + hArray(maxima_cc.lags) 
# BUG! startTimes + maxima_cc.lags gives all values equal to startTimes[0] + maxima_cc.lags[0]
# happens when adding hArray and Vector... why?

#for badchannel in x[0]: 
#    arrivaltime[int(badchannel)] = arrivaltime[int(badchannel)-1] # HACK !!

plt.figure()
arrivaltime.plot()
plt.title('Arrival times, matched with offsets per station (check!)')

# now make footprint plot of all arrival times
loradir = '/Users/acorstanje/triggering/CR/LORA'
# first show originally derived arrival times
fptask_orig = cr.trerun("plotfootprint", "0", colormap = 'jet', filefilter = eventdir, loradir = loradir, plotlora=False, plotlorashower=False, pol=pol) # no parameters set...
plt.title('Footprint using original cr_event arrival times')
plt.figure()
# now our arrival times
fptask = cr.trerun("plotfootprint", "1", colormap = 'jet', filefilter = eventdir, arrivaltime=arrivaltime, loradir = loradir, plotlora=False, plotlorashower=False, pol=pol) # no parameters set...
plt.title('Footprint using crosscorrelated arrival times')
delta = arrivaltime - 1.0e-9 * fptask_orig.arrivaltime
delta -= delta.mean()
plt.figure()
fptask_delta = cr.trerun("plotfootprint", "2", colormap = 'jet', filefilter = eventdir, arrivaltime=delta, loradir = loradir, plotlora=False, plotlorashower=False, pol=pol) # no parameters set...
plt.title('Footprint of difference between cr_event and full-cc method')
plt.figure()
delta.plot()
plt.title('Difference between plotfootprint / cr_event arrival times\nand full crosscorrelation times')

# Do plane-wave direction fit on full arrivaltimes
# Fit pulse direction
print 'Do plane wave fit on full arrival times (cross correlations here)...'
direction_fit_plane_wave = cr.trun("DirectionFitPlaneWave", positions = antpos, timelags = arrivaltime, verbose=True)

plt.figure()
direction_fit_plane_wave.residual_delays.plot()
plt.title('Residual delays after plane wave fit')

print 'Do plane wave fit on stored arrival times, from plotfootprint...'
direction_fit_plane_wave_orig = cr.trun("DirectionFitPlaneWave", positions = antpos, timelags = 1.0e-9 * fptask_orig.arrivaltime, verbose=True)

plt.figure()
direction_fit_plane_wave_orig.residual_delays.plot()
plt.title('Residual delays after plane wave fit\n to plotfootprint timelags')

residu = direction_fit_plane_wave.residual_delays.toNumpy()
residu -= residu.mean()
residu[np.where(abs(residu) > 15e-9)] = np.float('nan') # hack

plt.figure()
fptask_delta = cr.trerun("plotfootprint", "3", colormap = 'jet', filefilter = eventdir, arrivaltime=hArray(1.0e9*residu), loradir = loradir, plotlora=False, plotlorashower=False, pol=pol) # no parameters set...
plt.title('Footprint of residual delays w.r.t. planewave fit')

#fftplan = FFTWPlanManyDftR2c(blocksize, 1, 1, 1, 1, 1, fftw_flags.ESTIMATE)
#hFFTWExecutePlan(fft_data[...], timeseries_data[...], fftplan)

"""