#! /usr/bin/env python
#
# A test script to plot wavefront curvature using the pycrtools.plotfootprint task.
# Arthur Corstanje, 2011 

from pycrtools import *
import numpy as np
from pycrtools import srcfind

deg2rad = 2*np.pi / 360.0

#event = '/Users/acorstanje/triggering/CR/results_withcaltables/VHECR_LORA-20110716T094509.665Z/'
event = '/Users/acorstanje/triggering/CR/results_3jan2012/VHECR_LORA-20120103T003111.187Z/'

polarization = 0

# run task to get arrival times for all antennas in the event
#fptask = trerun("plotfootprint", "0", colormap = 'jet', filefilter = event, pol=polari) # no parameters set...
fptask = trerun("plotfootprint", "0", colormap = 'jet', filefilter = event, pol=polarization) # no parameters set...

times = fptask.arrivaltime.toNumpy()
plotsizes = hArray(dimensions = fptask.arrivaltime, fill=50.0)
fptask = trerun("plotfootprint", "0", colormap = 'jet', filefilter = event, power = plotsizes, pol=polarization) 

times *= 1.0e-9 # in seconds
times -= times.mean()

positions = fptask.positions.toNumpy()
positions2D = positions
positions = positions.flatten()

# get the best-fitting plane wave through all the measured time delays
# which are already corrected for cable delays if event processed with Cabledelays metadata present
bestfitDirection = srcfind.directionForHorizontalArray(positions, times) 

# get the calculated delays according to this plane wave
bestfitDelays = srcfind.timeDelaysFromDirection(positions, bestfitDirection)

# is it necessary / correct to subtract mean times and mean calculated times?
#times -= times.mean()
bestfitDelays -= bestfitDelays.mean()

residu = times - bestfitDelays

# remove outside 3-sigma residues
sigma = residu.std()
#for i in range(len(residu)):
#    if abs(residu[i]) > 3 * sigma: # mean = 0, subtracted.
#        residu[i] = np.float('nan') # can be done in a oneliner or with masked arrays or with hArrays... (lazy)

residu *= 1.0e9
oldresidu = residu
# try new direction finding by putting NaNs in times array at these positions...
newPositions2D = positions2D[np.where(abs(residu) < 15.0)]
newPositions = newPositions2D.flatten()
newTimes = times[np.where(abs(residu) < 15.0)]

# doesnt work! times[np.where(abs(residu) > 20.0)] = np.float('nan')
bestfitDirection = srcfind.directionForHorizontalArray(newPositions, newTimes) 
bestfitDelays = srcfind.timeDelaysFromDirection(newPositions, bestfitDirection)
bestfitDelays -= bestfitDelays.mean()

residu = newTimes - bestfitDelays # new

residuForAllAntennas = []
newindex = 0
for i in range(len(oldresidu)):
    if abs(oldresidu[i]) < 15.0:
        thisValue = residu[newindex] 
        newindex += 1
    else:
        thisValue = float('nan')
    residuForAllAntennas.extend([thisValue]) # the new value


residues = hArray(residuForAllAntennas)
residues *= 1.0e9

plotsizes = hArray(dimensions=residues, fill=50.0)
fptask = trerun("plotfootprint", "1", colormap = 'jet', filefilter = event, arrivaltime=residues, power = plotsizes, pol=polarization) 
#fptask = trerun("plotfootprint", "2", colormap = 'jet', filefilter = event, arrivaltime=residues, pol=1)

#plt.figure()
# Make lateral distribution of curvature delays. 
# ! check if the ant-id indices match with those from plotfootprint!
#ldftask = trerun("ldf", "0", topdir = '/Users/acorstanje/triggering/CR/results_withcaltables', events='VHECR_LORA-20110716T094509.665Z', signals0 = residues, Draw3D = False, logplot=False)
#plt.axis(xmin=0, xmax=300, ymin=-30, ymax=30)



