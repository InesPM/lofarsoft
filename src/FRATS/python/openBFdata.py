# import libraries, either locally or from global installation
# bfdata reads in beamformed LOFAR data
# dedispersion does incoherent dedispersion on arrays or fitsimages
import FRATSanalysis as fa

def myplot(data,doSquare=False):
    fig=plt.figure()
    fig.subplots_adjust(wspace=0.3,hspace=0.3)
    ax1=fig.add_subplot(221)
    ax1.set_xlabel("frequency channel [nr]")
    ax1.set_ylabel("time sample")
    ax1.imshow(data[:,:],aspect='auto')
    ax2=fig.add_subplot(222)
    ax2.plot(np.sum(data[:,:],axis=1))
    ax2.set_xlabel("time sample")
    ax2.set_ylabel("power")
    ax3=fig.add_subplot(223)
    if doSquare:
        ax3.plot(np.sum(np.square(data[:,:]),axis=0))
    else:
        ax3.plot(np.sum(data[:,:],axis=0))
    ax3.set_ylabel("power")
    ax3.set_xlabel("frequency channel [nr]")
    ax4=fig.add_subplot(224)
    ax4.imshow(np.log(data[:,:]),aspect='auto')
    ax4.set_xlabel("frequency channel [nr]")
    ax4.set_ylabel("time sample")
    



def cleanPercentage(data,lowerfraction,upperfraction):
    data2=np.copy(data)
    data2=data2.reshape(np.product(data2.shape))
    data2.sort()
    low=data2[lowerfraction*len(data2)]
    high=data2[upperfraction*len(data2)]
    data[data<low]=0
    data[data>high]=0
    del data2


def dedispersionShift(data,dr,DM,markboundary):
    delays=dedispersion.freq_to_delay(DM,dr.par['frequencies'],dr.par['timeresolution'])
    if markboundary:
        data[0,:]=1e-6
    (sa,ch)=data.shape
    temparray=np.zeros(sa,data.dtype)
    offsets=-1*np.round(delays)
    print offsets
    for c in range(ch):
        offset=offsets[c]
        sab=sa-offset
        saa=offset
        temparray[0:sab]=data[offset:,c]
        temparray[sab:]=data[0:offset,c]
        data[:,c]=temparray


def dedispersionToTimeseries(data,startChan,nChan,offsets,dr,DM):
    endChan=startChan+nChan
    #delays=dedispersion.freq_to_delay(DM,dr.par['frequencies'][startChan:startChan+nChan],dr.par['timeresolution'])
    (sa,ch)=data.shape
    temparray=np.zeros(sa,data.dtype)
    #offsets=-1*np.round(delays)
    
    for c in range(nChan):
        offset=offsets[c]
        sab=sa-offset
        saa=offset
        temparray[offset:]+=data[0:sa-offset,startChan+c]
    return temparray

    
def divideBaseline(data,blocksize=None):
    if not blocksize:
        baseline=np.average(data,axis=0)
        data/=baseline
    else:
        nblocks=data.shape[0]/blocksize
        for b in range(nblocks):
            baseline=np.average(data[b*blocksize:(b+1)*blocksize],axis=0)
            data[b*blocksize:(b+1)*blocksize]/=baseline

def subtractBaseline(data,blocksize=None):
    if not blocksize:
        baseline=np.average(data,axis=0)
        data-=baseline
    else:
        nblocks=data.shape[0]/blocksize
        for b in range(nblocks):
            baseline=np.average(data[b*blocksize:(b+1)*blocksize],axis=0)
            data[b*blocksize:(b+1)*blocksize]-=baseline


def dataToFits(data,dr,fitsname,overwrite):
    import pyfits
    hdu = pyfits.PrimaryHDU(data)
    new_image_hdr=hdu.header
    metadata=dict()
    metadata['NAXIS']=2
    metadata['CDELT0']=1#dr.par['timeresolution']
    metadata['CTYPE0']='TIME'
    metadata['CRVAL0']=startblock*blocksize
    metadata['CUNIT0']='samples'
    metadata['CRPIX0']=0
    metadata['CDELT1']=1#dr.par['timeresolution']
    metadata['CTYPE1']='FREQ'
    metadata['CRVAL1']=0
    metadata['CRPIX1']=0
    metadata['CUNIT1']='channels'
    for key in metadata.keys():
        new_image_hdr.update(key,metadata[key])
    print "saving fits file to",fitsname
    hdu.writeto(fitsname,clobber=overwrite) 

def flagChannels(data,directory,startblock,blocksize,method=1):
    par=fa.obsParameters(directory)
    fa.derivedParameters(par)
    if blocksize != par['sa']:
        raise ValueError("Blocksize incorrect. It should be "+str(par['sa']))
    nofblocks=data.shape[0]/blocksize
    startblock-=par['stb']
    fc=fa.getFlaggedChannels(directory,channels=par['nrfreqs'])
    fc=fc[startblock:startblock+nofblocks]
    if method=="average":
        replacevalue=np.average(data)
    else:
        replacevalue=method
    for block in range(nofblocks):
        print block,blocksize,fc[block]
        data[block*blocksize:(block+1)*blocksize,fc[block]==1]=replacevalue

def flagSamples(data,directory,startblock,blocksize,method=1):
    par=fa.obsParameters(directory)
    fa.derivedParameters(par)
    if blocksize != par['sa']:
        raise ValueError("Blocksize incorrect. It should be "+str(par['sa']))
    nofblocks=data.shape[0]/blocksize
    fs=fa.getFlaggedSamples(directory,matrixoutput=False)
    startblock-=par['stb']
    fs=fs[startblock:startblock+nofblocks]
    if method=="average":
        replacevalue=np.average(data)
    else:
        replacevalue=method
    for block in range(nofblocks):
        flagindex=fs[block]+blocksize*block
        data[flagindex,:]=replacevalue
    
def RFIcalcbaseline(data,skipHighest=False):
    if not skipHighest:
        return np.sum(data,axis=0)
    else:
        correction=np.sum(np.sort(data,axis=1)[-0.05*data.shape[0]:,:],axis=0)
        return (np.sum(data,axis=0)-correction)/0.95
        
def RFIcalcSqrBaseline(data,skipHighest=False):
    if not skipHighest:
        return np.sum(np.square(data),axis=0)
    else:
        correction=np.sum(np.square(np.sort(data,axis=1)[-0.05*data.shape[0]:,:]),axis=0)
        return (np.sum(np.square(data),axis=0)-correction)/0.95

def RFIcalcTimeseries(data):
    return np.sum(data,axis=1)

def RFIcalcAverageTimeseries(data):
    return np.average(data,axis=1)

def RFIaverageTimeseres(data,timeseries):
    timeseries/=data.shape[1]

def RFIcalcSqrTimeseries(data):
    return np.sum(np.square(data),axis=1)

def RFIcalcBadChannels(cutlevel,baseline,baselinesqr):
    sqrDivBaseline=baselinesqr/np.square(baseline)    
    sqrDivBaselineSort=np.copy(sqrDivBaseline)
    sqrDivBaselineSort.sort()
    nrchan=len(sqrDivBaselineSort)
    av=np.average(sqrDivBaselineSort[0.1*nrchan:0.9*nrchan])
    std=np.stddev(sqrDivBaselineSort[0.1*nrchan:0.9*nrchan])
    limit=av+cutlevel*std
    lowerlimit=av-cutlevel*std
    index=np.arange(len(sqrDivBaseline))
    badChans=index[sqrDivBaseline>limit]
    badChans=np.append(badchans,index[sqrDivBaseline<lowerlimit])
    return badChans

def RFIcalcBadSamples(cutlevel,data,subdiv=8,reqsubdiv=2):
    channelsPerPart=data.shape[1]/subdiv
    data2=np.copy(data)
    data2=data2.reshape(data.shape[0],subdiv,channelsPerPart)
    collapsedData=np.sum(data2,axis=2)
    collapsedDataSort=np.copy(collapsedData)
    collapsedDataSort.sort(axis=0)
    nrsa=collapsedDataSort.shape[0]
    av=np.average(collapsedDataSort[0.1*nrsa:0.9*nrsa],axis=0)
    std=np.std(collapsedDataSort[0.1*nrsa:0.9*nrsa],axis=0)
    limit=av+cutlevel*std
    index=np.arange(nrsa)
    badSamples=np.copy(collapsedData)
    badSamples.fill(0)
    badSamples[collapsedData>limit]=1
    return index[np.sum(badSamples,axis=1)>=reqsubdiv]

def RFIcleanChannels(data,badChannels,method="1"):
    if method==1:
        data[:,badChannels]=1
    elif method==0:
        data[:,badChannels]=0
    elif method=="prevfreq":
        data[:,badChannels]=data[:badChannels-3]

def RFIcleanChannels0(data,chPerSB,method):
    if method==1:
        data[:,range(0,data.shape[1],chPerSB)]=1 
    elif method==0:
        data[:,range(0,data.shape[1],chPerSB)]=0 

def RFIcleanSamples(data,badSamples):
    if method==1:
        data[badSamples,:]=1
    elif method==0:
        data[badSamples,:]=0


def RFIdivideBaseline(data,baseline):
    data/=baseline 

def subtractAverage(data,blocksize,nrstreams,zeroDM=False):
    nblocks=data.shape[0]/blocksize
    nchans=data.shape[1]/nrstreams
    for block in range(nblocks):
        for ch in range(nrstreams):
            if zeroDM:
                bl_av=np.average(data[block*blocksize:(block+1)*blocksize,ch*nchans:(ch+1)*nchans],axis=1).reshape(blocksize,1)
            else:
                bl_av=np.average(data[block*blocksize:(block+1)*blocksize,ch*nchans:(ch+1)*nchans])
            data[block*blocksize:(block+1)*blocksize,ch*nchans:(ch+1)*nchans]-=bl_av



try:
    import bfdata as bf
    import dedispersion
except:
    from pycrtools import bfdata as bf
    from pycrtools import dedispersion
import numpy as np
import sys
from optparse import OptionParser
usage = "usage: %prog -o <obsid> [options]"
parser=OptionParser(usage=usage)
parser.add_option("-o","--obsid",type="string",default="UNDEFINED")
parser.add_option("-p","--parsetdir",type="string",default="/vol/astro/lofar/frats/bf/parsets/")
parser.add_option("-d","--datadir",type="string",default="/vol/astro/lofar/frats/bf/")
parser.add_option("-l","--cutlevel",type="float",default=5.0)
parser.add_option("-s","--startblock",type="int",default=0)
parser.add_option("-n","--nofblocks",type="int",default=1)
parser.add_option("-z","--blocksize","--sa",type="int",default=512)
parser.add_option("-y","--dividebaseline",action="store_true",dest="dividebaseline",default=False)
parser.add_option("-x","--subtractbaseline",action="store_true",dest="subtractbaseline",default=False)
parser.add_option("-a","--savefits",action="store_true",dest="savefits",default=False)
parser.add_option("-w","--overwritefits",action="store_true",dest="overwritefits",default=False)
parser.add_option("-f","--fitsname",type="string",default="temp.fits")
parser.add_option("--sp","--saveplot",action="store_true",dest="saveplot",default=False)
parser.add_option("--pn","--plotname",type="string",dest="plotname",default="temp.png")
parser.add_option("-m","--beam",type="int",default=2)
parser.add_option("-c","--chPerSB",type="int",default=None,dest='chPerSB')
parser.add_option("-R","--flagrfi",action="store_true",default=False,dest="flagrfi")
parser.add_option("-D","--rfidir",type="string",dest="rfidir",default=None)
parser.add_option("--DM","--dedisperse",type=float,dest="DM",default=None)
parser.add_option("-t","--plot",action="store_true",default=False,dest="bPlot")
parser.add_option("--ts","--timeseries",action="store_true",default=False,dest="bTimeseries")
parser.add_option("--pd","--plotdistance",type=float,dest="plotdistance",default=30)
parser.add_option("--pw","--plotwindow",type=float,dest="plotwindow",default=0)
parser.add_option("--pc","--plotcenter",type=float,dest="plotcentertime",default=0)
parser.add_option("--il","--integrationlength",type=int,dest="integrationlength",default=1)
parser.add_option("--subav","--subtractaverage",action="store_true",default=False,dest="subtractaverage")


(options,args)=parser.parse_args()

if not options.obsid:
    parser.print_help()
    sys.exit(1)
obsid=options.obsid
#obsid='L39700'
cutlevel=options.cutlevel
parsetdir=options.parsetdir
datadir=options.datadir
nrstreams=8
obsids=[obsid]
chunksize=1
obsid=obsids[0]
blocksize=options.blocksize #dr.samples
nofblocks=options.nofblocks
startblock=options.startblock
fileindex=options.beam
plotwindow=options.plotwindow
plotcentertime=options.plotcentertime
integrationlength=options.integrationlength
if not options.saveplot:
    plotname=False
else:
    plotname=True
# open a datareader
dr=bf.BFDataReader(parsetdir+'/'+obsid+'.parset',datadir,True)
# need to fix some issues, but we circumfent them for MSSS bf data by setting:
dr.nrsubbands=80
dr.par['nrsubbands']=80
dr.h5=True
dr.setDatatype('coherentstokes')
if obsid=="L39700":
    dr.files=[open('/data/L39700/L39700_SAP000_B000_S0_P000_bf.raw')]
#    dr.nrsubbands=240
totCh=dr.par['nrsubbands']*dr.par['channels']
# set the blocksize
dr.setblocksize(blocksize)
mutech={}
f1=1500
f2=3000
import matplotlib.pyplot as plt
import pickle
title=''
shape1=list(dr.data.shape)
shape1[0]=nofblocks
alldata=np.zeros(shape1)
# read a block of data
for num,block in enumerate(range(startblock,startblock+nofblocks)):
    print "reading block",block
    alldata[num]=dr.read(block)[fileindex]
# divide the data in chunks
data=alldata.reshape(shape1[0]*shape1[1],shape1[2])

data1=data.reshape((data.shape[0]/chunksize,chunksize,totCh))
# sum over the chunks?
#subspec=np.sum(data1,axis=1)
# power of frequency spectrum
#powerspec=np.sum(subspec,axis=0)
#std=np.std(subspec,axis=0)
# determine which channels to flag
# The flagging itself can be done by setting 
#stdNorm=std/powerspec
#stdNormSort=np.copy(stdNorm)
#stdNormSort.sort()
#Lstd=len(stdNormSort)
#av=np.average(stdNormSort[int(0.1*Lstd):int(0.9*Lstd)])
#std1=np.std(stdNormSort[int(0.1*Lstd):int(0.9*Lstd)])
#index=np.arange(0,Lstd)
#muteindex=index[stdNorm>av+cutlevel*std1]
# flag data
print "Available for interactive use: data and dr (datareader) with dr.par for parameters"

if options.subtractbaseline:
    baseline=[]
    nblocks=data.shape[0]/blocksize
    for b in range(nblocks):
        baseline.append(np.average(data[b*blocksize:(b+1)*blocksize],axis=0))
    subtractBaseline(data)
    for b in range(nblocks):
        data[b*blocksize:(b+1)*blocksize]/=baseline[b]
    flagmethod=0
    cleanPercentage(data,0.1,0.9)
    
    

if options.dividebaseline:
    divideBaseline(data,blocksize) 
    flagmethod=1
else:
    flagmethod="average"


rfidir=options.rfidir
if options.flagrfi:
    if not rfidir:
        raise ValueError("No RFI directory specified")
    else:
        flagChannels(data,rfidir,startblock,blocksize,flagmethod)
        if not options.chPerSB:
            print "WARNING, not flagging 0-th channel of the subband. Please specify -c" 
        else:
            data[:,np.arange(0,data.shape[1],options.chPerSB)]=0


if options.flagrfi:
    if not rfidir:
        raise ValueError("No RFI directory specified")
    else:
        flagSamples(data,rfidir,startblock,blocksize,flagmethod)

if options.subtractaverage:
    zeroDM=True
    subtractAverage(data,blocksize,nrstreams,zeroDM)
    flagmethod=0
    data[:,np.arange(0,data.shape[1],options.chPerSB)]=0
    if options.flagrfi and not zeroDM:
        if not rfidir:
            raise ValueError("No RFI directory specified")
        else:
            flagSamples(data,rfidir,startblock,blocksize,flagmethod)
            flagChannels(data,rfidir,startblock,blocksize,flagmethod)
            if not options.chPerSB:
                print "WARNING, not flagging 0-th channel of the subband. Please specify -c" 
            else:
                data[:,np.arange(0,data.shape[1],options.chPerSB)]=flagmethod

#if options.dividebaseline:
#    divideBaseline(data,blocksize) 
#    flagmethod=1
#else:
#    flagmethod="average"

if False:
    rfidir=options.rfidir
    if options.flagrfi:
        if not rfidir:
            raise ValueError("No RFI directory specified")
        else:
            flagChannels(data,rfidir,startblock,blocksize,flagmethod)
            flagSamples(data,rfidir,startblock,blocksize,flagmethod)
            if not options.chPerSB:
                print "WARNING, not flagging 0-th channel of the subband. Please specify -c" 
            else:
                data[:,np.arange(0,data.shape[1],options.chPerSB)]=1

    if options.dividebaseline:
        divideBaseline(data,blocksize) 
        flagmethod=1
    else:
        flagmethod="average"



if options.DM!=None and not options.bTimeseries:
    dedispersionShift(data,dr,options.DM,markboundary=True)

if options.DM!=None and options.bTimeseries:
    print "Making dedispersed timeseries"
    par=fa.obsParameters(rfidir)
    fa.derivedParameters(par,options.DM)
    nDMs=len(par['DM'])
    nrstreams=par['nrstreams']
    offsets=fa.loadOffset(nDMs,nrstreams,rfidir)
    DM=options.DM
    if DM not in offsets.keys():
        DM=[d for d in offsets.keys() if np.abs(d-options.DM) < 0.5*min(np.diff(par['DM']))][0] 
        if DM not in offsets.keys():
            raise ValueError("Invalid DM, options "+str(offsets.keys()))
    chPerBand=par['nrfreqs']/par['nrstreams']
    ts=[dedispersionToTimeseries(data, i*chPerBand, chPerBand, offsets[DM][i], dr, DM) for i in range(nrstreams)]
    reftime=startblock*par['sa']
    delays=par['delays']
    length=min([len(t) for t in ts])




    ta=fa.makeTimeAxes(reftime,delays,length)
    av=fa.loadAverage(len(par['DM']),par['nrstreams'],rfidir)
    std=fa.loadStddev(len(par['DM']),par['nrstreams'],rfidir)
    #trmsg=fa.loadTriggerMsg(rfidir,bReturnArray=True,newVersion=3)
    trmsg=None
    if DM not in av.keys():
        DM=[d for d in pv.keys() if np.abs(d-DM) < 0.5*min(np.diff(par['DM']))][0] 
        raise ValueError("Invalid DM, options "+str(av.keys()))
    else:
        av=av[DM]
        std=std[DM]



if options.bPlot:
    mycolors=['r','g','b','k','y','m','c','0.5','r','g','b','k','y','m','c','0.5']
    if not options.bTimeseries:
        myplot(data)
    else:
        fa.plotTimeseries(ts,DM,par,trmsg,av,std,integrationlength,plotcentertime,plotwindow,offset=options.plotdistance,legend=True,plotThreshold=True)
    if options.saveplot:
        print "Figure saved as",options.plotname
        plt.savefig(options.plotname)


if options.savefits:
    dataToFits(data,dr,options.fitsname,options.overwritefits)




# plot dynamic spectrum and collapsed spectrum
    
# reshape data into streams
#data3=data.reshape(dr.samples,nrstreams,totCh/nrstreams)
# calculate frequencies for each stream
#freqs=dr.par['frequencies']
#freqs=np.array(freqs[0:totCh]).reshape(nrstreams,totCh/nrstreams)
#stream=5
# dedisperse data for the specified stream
#dd=dedispersion.dedisperse_array(np.transpose(data3[:,stream,:]),12.455,freqs[0],dr.par['timeresolution'])


#mutelist=[]
##for k,v in mutech.iteritems():
#    mutelist.extend(v)
#myhist=plt.hist(mutelist,3904,[0,3903],histtype='bar',edgecolor='none')
#noncomplete=[int(b)+1 for a,b in zip(myhist[0],myhist[1]) if 0.5<a<5.5]
#complete=[int(b)+1 for a,b in zip(myhist[0],myhist[1]) if a==6]
#nc=[(n,[k for k,v in mutech.iteritems() if n in v]) for n in noncomplete]
#
