#
#  pulsefit.py - fit the direction of the triggered pulse based on the full data
#
#
#  Created by Andreas Horneffer on Aug. 1, 2010.
#  Copyright (c) 2010, Andreas Horneffer. All rights reserved.
#  Ported to pycrtools on Nov. 15, 2010 (AC)
"""
This module supplies different functions to fit the direction of a triggered
pulse

"""
import os
from pycrtools import *
# from pycrtools import IO
from pycrtools import matching as match
from pycrtools import srcfind as sfind
from pycrtools import beamformer as bf

import numpy as np
import matplotlib.pyplot as plt
from scipy.optimize import fmin, brute
import pdb

rad2deg = 180. / np.pi
#-------------------------------------------------------------------------------

#------------------------------------------------------------ simplexPositionFit


def simplexPositionFit(crfile, cr_fft, antenna_positions, start_position, ant_indices,
                       FarField=True, blocksize=-1, method='smoothedAbs', doPlot=False):
#  print 'ANT INDICES: '
#  print ant_indices
#  print ' '
    thisBF = bf.Beamformer(crfile, cr_fft)  # initialize object. Isn't this an ugly memory leak?
    # import pdb; pdb.set_trace()
    if method == 'smoothedAbs':
        maximizer = thisBF.pulseSmoothedAbsMaximizer
    elif method == 'abs':
        maximizer = thisBF.pulseAbsMaximizer
    elif method == 'smoothedPower':
        maximizer = thisBF.pulseSmoothedPowerMaximizer
    optimum = fmin(maximizer, start_position, (cr_fft, antenna_positions, ant_indices, FarField), xtol=1e-1, ftol=1e-1, full_output=1)

    if (not FarField) and (len(start_position) == 3):
        nPoints = 30
        distResult = np.zeros(nPoints)
        distances = np.zeros(nPoints)
        R = 10.0
        for i in range(nPoints):
            distances[i] = R
            R *= pow(10000.0, 1.0 / nPoints)
#      print distances
        for i in range(nPoints):
            pos = [optimum[0][0], optimum[0][1], 2000.0 / distances[i]]
            distResult[i] = - maximizer(pos, cr_fft, antenna_positions, ant_indices, False)

        optR = distances[np.argmax(distResult)]
        pos[2] = 2000.0 / optR
        if doPlot:
            plt.clf()
            plt.semilogx(distances, distResult)
            raw_input("--- Plotted bf pulse height versus distance - press Enter to continue...")
        optimum = fmin(maximizer, pos, (cr_fft, antenna_positions, ant_indices, FarField), xtol=1e-2, ftol=1e-4, full_output=1)  # extra simplex fit to finish it off
        pos = [optimum[0][0], optimum[0][1], optimum[0][2]]  # done at once?

        optBeam = thisBF.getTiedArrayBeam(pos, cr_fft, antenna_positions, ant_indices, False)
    else:
        if len(optimum[0]) == 2:
            pos = [optimum[0][0], optimum[0][1], 1.0]
        else:
            pos = optimum[0]
        optBeam = thisBF.getTiedArrayBeam(pos, cr_fft, antenna_positions, ant_indices, FarField)

#  bruteRanges = ((start_position[0] - 20.0, start_position[0] + 20.0), (start_position[1] - 10.0, start_position[1] + 10.0))
    # optErg = brute(beamform_function, bruteRanges, Ns = 50, full_output = 1)
    return (optimum, optBeam)

#------------------------------------------------------------- triggerMessageFit


def initialDirectionFit(crfile, efield, fitType='bruteForce'):
    # pass efield itself, with right dims?
#    efield = crfile["emptyFx"]
#    blockNo = 1
#    crfile.getTimeseriesData(efield, blockNo)
    # change to abs, then max? Destroys input array...

    mx = efield[...].max()
    mn = efield[...].min()
    mxpos = efield[...].maxpos()
    mnpos = efield[...].minpos()

    Tdiffs = np.zeros(len(mx))
    for i in range(len(mx)):
        if mx[i] > - mn[i]:
            Tdiffs[i] = mxpos[i]
        else:
            Tdiffs[i] = mnpos[i]

    Tdiffs -= np.mean(Tdiffs)
    Tdiffs *= 5e-9
#    print "ANTENNA SET"
#    print crfile["ANTENNA_SET"]
    crfile["ANTENNA_SET"] = "LBA_OUTER"  # HACK !!
    match_positions = crfile["RELATIVEANTENNA_POSITIONS"].toNumpy().reshape(3 * len(Tdiffs))
    (radaz, radel, bftime) = sfind.directionBruteForceSearch(match_positions, Tdiffs)
    mse = sfind.mse(radaz, radel, match_positions, Tdiffs)
    degaz = radaz * rad2deg
    degel = radel * rad2deg

    result = dict(az=degaz, el=degel, mse=mse, success=True, action='New initial direction fit')

    print mx
    print ' '
    print mn
    print ' '
    print Tdiffs
    print ' '
    print mse
    print degaz
    print degel
    print ' '
    return result


def triggerMessageFit(crfile, triggers, fitType='bruteForce'):  # crfile has to be imported using IO module!
    # Get the trigger-message information
    result = dict(success=False, action='Trigger message fit')
    fileDate = crfile["TIME"][0]
    fileSamplenum = crfile["SAMPLE_NUMBER"][0]                  # assuming start sample nr. is the same for all antennas
    ddate = fileDate + fileSamplenum / 200.0e6
    (mIDs, mdDates, mTdiffs, mTriggerDates, mSampleNums, mMissed) = match.matchTriggerfileToTime((ddate + 0.00033024), triggers)
    if len(mIDs) == 0:
        print 'NO TRIGGERS FOUND'
        result.update(reason='No matching triggers found!')
        return result

    # get the position for that
    nofAntennas = crfile["NOF_DIPOLE_DATASETS"]
    # now trim the matched trigger IDs to include only antennas which are actually in the data!
    validChannels = np.argwhere(mIDs < nofAntennas)  # get indices in array
#  print mTdiffs
#  print len(mTdiffs)
    mTdiffs = mTdiffs[validChannels].ravel()
#  print mTdiffs
#  print len(mTdiffs)
#  print ' '
    mIDs = mIDs[validChannels].ravel()
    mMissed = mMissed[validChannels].ravel()
    avgmissed = np.average(mMissed)
#  print len(mIDs)
    crfile["ANTENNA_SET"] = "LBA_OUTER"  # HACK !!
    match_positions = crfile["RELATIVEANTENNA_POSITIONS"].toNumpy()[mIDs].reshape(3 * len(mIDs))
    # print mTdiffs
    # print len(mTdiffs)
#  print match_positions
#  print match_positions
#  print len(match_positions)

    if (fitType == 'linearFit'):
        (radaz, radel) = sfind.directionForHorizontalArray(match_positions, mTdiffs)
        if np.isnan(radel) or np.isnan(radaz):
            (radaz, radel, bftime) = sfind.directionBruteForceSearch(match_positions, mTdiffs)
            result.update(warning='Linear fit failed (produced NaN). Falling back to brute force method.')
    elif (fitType == 'bruteForce'):
        (radaz, radel, bftime) = sfind.directionBruteForceSearch(match_positions, mTdiffs)
    else:
        radaz = 0.
        radel = 0.
    mse = sfind.mse(radaz, radel, match_positions, mTdiffs)
    degaz = radaz * rad2deg
    degel = radel * rad2deg
    if (degaz < 0.):
        degaz += 360.
    # get the average time-offset
    toffset = np.mean((mTriggerDates - fileDate) + (mSampleNums - fileSamplenum) / 200.0e6)  # matching.py returns np array, so keep it (doing mean anyway)
    print ' T OFFSET: %e' % toffset

    result.update(success=True, az=degaz, el=degel, mse=mse, avgmissed=avgmissed, avgToffset=toffset, nofAntennasUsed=len(mIDs))
    return result

#------------------------------------------------------------------ fullPulseFit


def fullDirectionFit(crfile, cr_efield, triggerFitResult, blocksize, flaggedList=[], FarField=True,
                     method='smoothedAbs', doPlot=False):
    # Set the parameters
    samplefreq = 200.0e6  # must be
    # crfile.set("blocksize", blocksize) # proper way, apparently
#  crfile.set("shift", 512)
    dataLength = crfile["DATA_LENGTH"][0]
    # blocksize = crfile["BLOCKSIZE"]
    print 'Data length is %d' % dataLength  # once again assume all the same; move to Datacheck?
    print 'Block size is now: %d' % blocksize
    print 'So there are: %d blocks' % (int(dataLength) / int(blocksize))
#    pulseMidpoint = int(triggerFitResult["avgToffset"] * samplefreq)
    nofAntennas = crfile["NOF_DIPOLE_DATASETS"]
    # import pdb; pdb.set_trace()
    # here we select the region of the data such that the pulse is (on average over antennas)
    # is in the middle of the block
#    cr_alldata = crfile["EMPTY_TIMESERIES_DATA"]
#    crfile.getTimeseriesData(cr_alldata, 0) # MOVE upward to crpipeline?
#    cr_efield = hArray(copy=cr_alldata, dimensions = [nofAntennas, blocksize])
#    start = pulseMidpoint - blocksize/2
#    stop = pulseMidpoint + blocksize/2
#    cr_efield[...].copy(cr_alldata[..., start:stop])

    crfile["BLOCKSIZE"] = blocksize  # workaround, needed for correct settings in Beamformer

    # blockNo = int((triggerFitResult["avgToffset"] * samplefreq) / blocksize)
    # print "fullPulseFit: set block-number to:", blockNo
#  crfile.set("block", blockNo)
# Apply flagged antenna list - select those out
#  selectedList = range(nofAntennas)
#  for entry in flaggedList:
#      selectedList.remove(entry)
#  crfile.setAntennaSelection(selectedList)

    # Get the data
    cr_time = hArray(float, dimensions=[blocksize])
    # cr_freqs = hArray(float, dimensions=[blocksize/2 + 1])

#  cr_efield = crfile["emptyFx"]
#  crfile.getTimeseriesData(cr_efield, blockNo) # crfile["Fx"] crashes on invalid block number ???? While it was set to a valid value...

    # temp: plot to see if pulse is in this block...
#  efield = cr_efield.toNumpy()
#  plt.clf()
#  plt.plot(efield.T)
#  raw_input("--- Plotted pulse, check if pulse is in block - press Enter to continue...")

    cr_fft = crfile["EMPTY_FFT_DATA"]
#  import pdb; pdb.set_trace()
    cr_fft = hArray(complex, dimensions=[nofAntennas, blocksize / 2 + 1])

    print 'Number of antennas = %d' % cr_fft.shape()[0]
    print 'Block length fft = %d' % cr_fft.shape()[1]

    crfile["ANTENNA_SET"] = "LBA_OUTER"  # HACK !!
    antenna_positions = crfile["RELATIVEANTENNA_POSITIONS"]
#  print 'ANTENNA POSITIONS: '
#  print antenna_positions
#  print ' '
    # Now calculate the Fourier transform of the data

    cr_fft[...].fftw(cr_efield[...])
    for i in range(cr_fft.shape()[0]):
        cr_fft[i, 0] = 0.0  # kill DC offsets

    if (FarField):
        start_position = [triggerFitResult["az"], triggerFitResult["el"]]
    else:
        start_position = [triggerFitResult["az"], triggerFitResult["el"], 1.0]

    # HACK
#  start_position = [255.0, 25.0]
    ant_indices = range(0, nofAntennas, 2)
    # for the time being: apply flags here instead of using crfile.setAntennaSelection (avoiding complications)
    for entry in flaggedList:
        if entry in ant_indices:
            ant_indices.remove(entry)  # or [ant_indices.remove(x) for x in flaggedList if x in ant_indices] ...
    # import pdb; pdb.set_trace()
    (fitDataEven, optBeamEven) = simplexPositionFit(crfile, cr_fft, antenna_positions, start_position, ant_indices, FarField=FarField, blocksize=blocksize, method=method, doPlot=doPlot)

    ant_indices = range(1, nofAntennas, 2)
    for entry in flaggedList:
        if entry in ant_indices:
            ant_indices.remove(entry)

    (fitDataOdd, optBeamOdd) = simplexPositionFit(crfile, cr_fft, antenna_positions, start_position, ant_indices,
                                        FarField=FarField, blocksize=blocksize, method=method, doPlot=doPlot)
    if FarField:
        Reven = -1.0
        Rodd = -1.0
    else:
        Reven = 2000.0 / fitDataEven[0][2]
        Rodd = 2000.0 / fitDataOdd[0][2]

    evenHeight = - fitDataEven[1]  # optimized pulse height
    oddHeight = - fitDataOdd[1]

    result = dict(success=True, action='Full direction fit',
                  even=dict(az=fitDataEven[0][0], el=fitDataEven[0][1], R=Reven,
                  optValue=evenHeight, optBeam=optBeamEven),
                  odd=dict(az=fitDataOdd[0][0], el=fitDataOdd[0][1], R=Rodd,
                  optValue=oddHeight, optBeam=optBeamOdd))

    return result

## Executing a module should run doctests.
#  This examines the docstrings of a module and runs the examples
#  as a selftest for the module.
#  See http://docs.python.org/library/doctest.html for more information.
if __name__ == '__main__':
    import doctest
    doctest.testmod()
