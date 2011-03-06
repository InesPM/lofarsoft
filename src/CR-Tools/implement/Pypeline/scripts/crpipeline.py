#! /usr/bin/env python
# A prototype Cosmic Ray pipeline based on PyCRtools. 
# Created by Arthur Corstanje, Jan 17, 2011. Email: a.corstanje@astro.ru.nl

import os
import sys
import time
import numpy as np
import matplotlib.pyplot as plt
import subprocess

import datacheck as dc
import rficlean as rf
import pulsefit as pf
import matching as match # possibly push this down to pulsefit?
# import beamformer as bf

antennaset = 'LBA_OUTER' # hack around missing info in data files

def writeDictOneLine(outfile, dict):
    for key in dict:
        if type(dict[key]) == np.float64:
            outfile.write('%s: %7.2f\n' % (str(key), dict[key]))
        else:
            outfile.write('%s: %s; ' % (str(key), ''.join(repr(dict[key]).strip('[]').split(','))))

def writeDict(outfile, dict):
    for key in dict:
#        print '%s %s %s' % (key, dict[key], type(dict[key]))
        if type(dict[key]) == dict: # if we have a dict inside our dict, print it on one line (and hope it's small)
            writeDictOneLine(outfile, dict[key])
        elif type(dict[key]) == np.float64:        
            outfile.write('%s: %7.2f\n' % (str(key), dict[key]))
        else:
            outfile.write('%s: %s\n' % (str(key), ''.join(repr(dict[key]).strip('[]').split(','))))
    outfile.write('\n')

def writeResultLine(outfile, pulseCountResult, triggerFitResult, fullDirectionResult, filename, timestamp):
    d = triggerFitResult
    az = str(d["az"])
    el = str(d["el"])
    mse = str(d["mse"])
    outString = az + ' ' + el + ' ' + mse + ' '
    
    d = fullDirectionResult["odd"]
    az = str(d["az"])
    el = str(d["el"])
    R = str(d["R"])
    optHeightOdd = str(d["optValue"])
    outString += az + ' ' + el + ' ' + R + ' ' + optHeightOdd + ' '
    
    d = fullDirectionResult["even"]
    az = str(d["az"])
    el = str(d["el"])
    R = str(d["R"])
    optHeightEven = str(d["optValue"])
    outString += az + ' ' + el + ' ' + R + ' ' + optHeightEven + ' '
    
    d = pulseCountResult
    avgCount = str(d["avgPulseCount"])
    maxCount = str(d["maxPulseCount"])
    outString += avgCount + ' ' + maxCount + ' '  
    
    summedPulseHeight = str(fullDirectionResult["summedPulseHeight"])
    coherencyFactor = str(fullDirectionResult["coherencyFactor"])
    outString += summedPulseHeight + ' ' + coherencyFactor + ' '
       
    outString += str(timestamp) + ' '
    outString += filename
    outfile.write(outString + '\n')

def runAnalysis(files, outfilename, asciiFilename, doPlot = False):
    """ Input: list of files to process, trigger info as read in by match.readtriggers(...), filename for results output
    """
    outfile = open(outfilename, mode='a')
    if not os.path.isfile(asciiFilename):
        asciiOutfile = open(asciiFilename, mode='a')
        headerString = 'trig.az trig.el trig.mse odd.az odd.el odd.R odd.height even.az even.el even.R even.height avgCount maxCount summedHeight coherency time filename\n'
        asciiOutfile.write(headerString)
    else:
        asciiOutfile = open(asciiFilename, mode='a') # hmm, duplicate code
    
    n = 0
    for file in files:
        n += 1
        print 'Processing file %d out of %d' % (n, len(files))
        result = dc.safeOpenFile(file, antennaset)
        #print result
        
        writeDict(outfile, result)
        if not result["success"]:
            continue
        crfile = result["file"] # blocksize is 2 * 65536 by default ('almost' entire file)
        cr_efield = result["efield"] # gets all data
#        crfile.set("blocksize", 32768)
#        import pdb; pdb.set_trace()
#        rf.cleanSpectrum(crfile)
        
        outfile.write('\n')
        # do quality check and rfi cleaning here
        result = dc.qualityCheck(crfile, cr_efield, doPlot = doPlot)
        flaggedList = result["flagged"]
        writeDict(outfile, result)
        if not result["success"]: 
            continue
        qualityCheckResult = result
        fileTimestamp = crfile["TIME"][0]
        triggers = match.readtriggers(crfile) 
        if len(triggers) == 0:
            writeDict(outfile, dict(success=False, reason="Trigger file couldn't be read"))
            continue
        #print flaglist
        # find initial direction of incoming pulse, using trigger logs
        result = pf.initialDirectionFit(crfile, cr_efield, fitType = 'linearFit')
        writeDict(outfile, result)
        result = pf.triggerMessageFit(crfile, triggers, 'linearFit') 
        writeDict(outfile, result)
        if not result["success"]:
            continue
        triggerFitResult = result
        # now find the final direction based on all data, using initial direction as starting point
        try: # apparently it's dangerous...
          result = pf.fullDirectionFit(crfile, triggerFitResult, 512, flaggedList = flaggedList, FarField = False, doPlot = doPlot)     
          fullDirectionResult = result
          writeDict(outfile, result)
          if not result["success"]:
              continue
          
        except (ZeroDivisionError, IndexError), msg:
          print 'EROR!'
          print msg
        writeResultLine(asciiOutfile, qualityCheckResult, triggerFitResult, fullDirectionResult, 
                        crfile.files[0].filename, fileTimestamp)
        bfEven = result["even"]["optBeam"]
        bfOdd = result["odd"]["optBeam"]
        
        if doPlot:
            bfEven.plot()
            raw_input("--- Plotted optimal beam for even antennas - press Enter to continue...")
            bfOdd.plot()
            raw_input("--- Plotted optimal beam for odd antennas - press Enter to continue...")
        
        outfile.flush()
    # end for
    outfile.close()
    asciiOutfile.close()
    return (bfEven, bfOdd)

# get list of files to process
if len(sys.argv) > 2:
    datafiles = sys.argv[1] 
#    triggerMessageFile = sys.argv[2]
    print datafiles
    print 'Too many options!'
elif len(sys.argv) > 1:
    datafiles = sys.argv[1]
    print 'Taking default trigger message file (i.e. name constructed from date and station name in the hdf5 data file).'
else:
    print 'No files given on command line, using a default set instead.'
#    datafiles = '/Users/acorstanje/triggering/stabilityrun_15feb2011/automatic_obs_test-15febOvernight--147-10*.h5' 
    datafiles = '/Users/acorstanje/triggering/stabilityrun_15feb2011/*.h5'
#    datafiles = '/Users/acorstanje/triggering/MACdatarun_2feb2011/automatic_obs_test-2feb-2-26.h5'

sortstring = 'sort -n --field-separator="-" --key=18'
outfile = 'crPipelineResults.txt'
outfileAscii = 'asciiPipelineResults.txt'
antennaset="LBA_OUTER"

#fd = os.popen('ls '+ datafiles+' | ' + sortstring)
#p1 = subprocess.Popen(['ls '+ datafiles + ' | ' + sortstring], shell=True, stdout=subprocess.PIPE)
(directory, files) = os.path.split(datafiles)
p1 = subprocess.Popen(['find ' + directory + ' -type f -name "'+ files + '" | ' + sortstring], shell=True, stdout=subprocess.PIPE)

output = p1.communicate()[0]
files = output.splitlines()
nofiles = len(files)
print "Number of files to process:", nofiles

if nofiles > 10:
    print '--- Spawning new processes for each file ---'

    thisProcess = subprocess.Popen(['./crpipeline.py', files[0]])
    thisProcess2 = subprocess.Popen(['./crpipeline.py', files[1]])
    i = 2
    while i < nofiles:
        time.sleep(0.33)
        if thisProcess.poll() != None:
            print 'Going to do: %s' % files[i]
            thisProcess = subprocess.Popen(['./crpipeline.py', files[i]])
            i += 1
        if thisProcess2.poll() != None:
            print 'Going to do: %s' % files[i]
            thisProcess2 = subprocess.Popen(['./crpipeline.py', files[i]])
            i += 1          
        
else:
    (bfEven, bfOdd) = runAnalysis(files, outfile, outfileAscii, doPlot = False)
#fitergs = dict()




#       n_az = len(fitDataEven[2][0, 0])
#  n_el = len(fitDataOdd[2][1, 0])
#  az_max = fitDataEven[2][0].max()
#  az_min = fitDataEven[2][0].min()
#  el_max = fitDataEven[2][1].max()
#  el_min = fitDataEven[2][1].min()

#  pixarray = np.zeros( (n_el,n_az) ) 
#  pixarray = -1.0 * fitDataEven[3].T # of transpose?
#  plt.imshow(pixarray,cmap=plt.cm.hot,extent=(az_min, az_max, el_min, el_max) )


