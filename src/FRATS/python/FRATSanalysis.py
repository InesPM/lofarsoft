import numpy as np
import bfdata as bf # self-written library to read LOFAR beamformed data, included in pycrtools
import dedispersion as dd # self-written library for dedispersion, included in pycrtools
import glob
import matplotlib.pyplot as plt
import struct
from bisect import bisect

DM=26.83

# Data of some of the observations
eventdb=dict()
eventdb['obs49630']=dict()
eventdb['obs49630']['beam2']=dict()
eventdb['obs49630']['beam2']['DM']=12.455
eventdb['obs49630']['beam2']['period']=1.3373021601895
eventdb['obs49630']['beam2']['pulsar']='B1919+21'
eventdb['obs49630']['beam2']['samperiod']=1020.2805787578583 # period/ timeresolution
eventdb['obs49630']['beam2']['pulses']=np.arange(-183+eventdb['obs49630']['beam2']['samperiod'],2600*192,eventdb['obs49630']['beam2']['samperiod'])
eventdb['obs49630']['beam1']=dict()
eventdb['obs49630']['beam1']['DM']=26.83
eventdb['obs49630']['beam1']['period']=0.714519699726
eventdb['obs49630']['beam1']['pulsar']='B0329+54'
eventdb['obs49630']['beam1']['samperiod']=eventdb['obs49630']['beam1']['period']/(5e-9*1024*64*4) # period/ timeresolution
eventdb['obs49630']['beam1']['pulses']=np.arange(-195+eventdb['obs49630']['beam2']['samperiod'],2600*192,eventdb['obs49630']['beam2']['samperiod'])

anadb=eventdb['obs49630']['beam2']

# Directories with analysed data
dirs=glob.glob('/Users/STV/Astro/Analysis/FRAT/analysis/obs49630/beam*/')
dirs2=glob.glob('/Users/STV/Astro/Analysis/FRAT/analysis/obs48409/run*/')
dirs3=glob.glob('/Users/STV/Astro/Analysis/FRAT/analysis/L518*/SAP*/')
dirs4=glob.glob('/Users/STV/Astro/Analysis/FRAT/analysis/L49630/SAP*/')
dirs5=glob.glob('/Users/STV/Astro/Analysis/FRAT/analysis/L48409/SAP*/')
dirs6=glob.glob('/Users/STV/Astro/Analysis/FRAT/analysis/L63186/SAP*/')
dirs7=glob.glob('/Users/STV/Astro/Analysis/FRAT/analysis/NoZeroDM/L518*/SAP*/')
dirs8=glob.glob('/Users/STV/Astro/Analysis/FRAT/analysis/ZeroDM/L518*/SAP*/')
dirs9=glob.glob('/Users/STV/Astro/Analysis/FRAT/analysis/1kpc/L518*/SAP*/')
dirs10=glob.glob('/Users/STV/Astro/Analysis/FRAT/analysis/1kpc/L48*/SAP*/')
dirs11=glob.glob('/Users/STV/Astro/Analysis/FRAT/analysis/1kpc/L49*/SAP*/')
dirs12=glob.glob('/Users/STV/Astro/Analysis/FRAT/analysis/coma/L49*/SAP*/')
dirs13=glob.glob('/Users/STV/Astro/Analysis/FRAT/analysis/coma/multiLength/L49*/SAP*/')
dirs14=glob.glob('/Users/STV/Astro/Analysis/FRAT/analysis/coma/multiLength/L5*/SAP*/')
dirs15=glob.glob('/Users/STV/Astro/Analysis/FRAT/analysis/coma/multiLength/L48*/SAP*/')


def runAnalysis(sigma=4.0,window=70):
    import FRATSanalysis as fa
    msgs=[fa.loadTriggerMsg(d) for d in fa.dirs3][0:1]
    #fa.selectPulsar(msgs,'B1919')
    fa.selectPulsar(msgs,'DM55')
    fa.selectThreshold(msgs,sigma)
    timesSub=fa.getTimeSubband(msgs[1])
    timesSub.sort()
    timesSub=np.array(timesSub)
    pulses=fa.anadb['pulses']
    ans=[(selectTriggers(triggercoincidenceSets(timesSub,number,window),pulses,number,window),number) for number in range(1,10)]
    purity=[(c[1],c[0][0]*1.0/(c[0][0]+c[0][1]))  for c in ans if c[0][0]>0]
    efficiency=[(c[1],c[0][0]*1.0/len(pulses)) for c in ans if c[0][0]>0]
    plt.figure()
    plt.bar([a[0]-0.4 for a in efficiency],[a[1] for a in efficiency],width=0.4,color='g')
    plt.bar([a[0] for a in purity],[a[1] for a in purity],width=0.4,color='b')
    plt.xlabel('number of coincidence channels')
    plt.title('windowsize='+str(window)+', threshold='+str(sigma))
    plt.ylabel('Eff (G) and purity (B) fraction') 
    return efficiency,purity

def selectPulsar(msgs,pulsar):
    """ selects trigger messages only in a certain DM range
        options: 1919 ,filters between 12.4 and 12.5
                 0329 , filters between 26.45 and 27.465
        
        *mgs*     Dict of triggermessages
        *pulsar*  Pulsar
    """
    if '1919' in pulsar:
        [filterDatabase(m,'DM',12.4,12.5) for m in msgs]
    if '0329' in pulsar:
        [filterDatabase(m,'DM',26.45,27.465) for m in msgs]

def selectThreshold(msgs,threshold):
    """ Filters trigger messages above the threshold """
    [filterDatabase(m,'max',threshold,100000000) for m in msgs]

def getTimeSubband(msg):
    """ Returns time and subband from the trigger messages 
        * msg * Database with trigger messages
    """
    return filterTriggers2(msg,-1,'time','subband')

def filterDatabase(database,key,min_select_value,max_select_value):
    """ Filters database by removing keys that are not in range.
        * database *         database
        * key *              key to check for
        * min_select_value * minimum value
        * max_select_value * maximum value

    """
    wrongkeys=[]
    for k,v in database.iteritems():
        if min_select_value>v[key] or v[key]>max_select_value:
            wrongkeys.append(k)
    for k in wrongkeys:
        database.pop(k)

def numberBeams(triggermsgDBlist):
    for num,m in triggermsgDBlist:
        for k,v in m.iteritems():
            v['beam']=num

def loadTriggerMsg(directory='',filename='TriggerMsg.log'):
    """
    Trigger messages are stored in a log file, made by the writetriggermessages program. These can be read in by this function. The underlying format is:
    
    fmt='i3L3i2f2i1f'

    Returns a dictionary with the following entries:
    trmsg[i]['magic']
    trmsg[i]['time']
    trmsg[i]['utc_second']
    trmsg[i]['utc_nanosecond']
    trmsg[i]['length']
    trmsg[i]['sample']
    trmsg[i]['block']
    trmsg[i]['subband']
    trmsg[i]['sum']
    trmsg[i]['max']
    trmsg[i]['obsID']
    trmsg[i]['beam']
    trmsg[i]['DM']
        

    """
# Needed to unpack the data format
    import struct
# open filename and read content
    if len(directory)>0:
        ft=open(directory+'/'+filename)
    else:
        ft=open(filename)
    data=ft.read()
# the data will be put in a dictionary, with a number for the trigger number and different properties
    trmsg=dict()
# the format in which the data is stored 
    fmt='<3q4i2f2i1f1i'
    #magic=struct.unpack('i',data[0:4])
    #if(magic==0x65):
    #    fmt=fmt+'i'
# number of bytes for one trigger message
    fmtlen=struct.calcsize(fmt)
    if len(data)%fmtlen>0:
        fmt='3L4i2f2i1f'
        fmtlen=struct.calcsize(fmt)

# read in all triggermessage (size is total datasize/ size per trigger) and code them according to the definition from FRATcoincidence.h
    for i in range(len(data)/fmtlen):
        msg=struct.unpack(fmt,data[i*fmtlen:(i+1)*fmtlen])
        trmsg[i]=dict()
        trmsg[i]['time']=msg[0]
        trmsg[i]['utc_second']=msg[1]
        trmsg[i]['utc_nanosecond']=msg[2]
        trmsg[i]['length']=msg[3]
        trmsg[i]['sample']=msg[4]
        trmsg[i]['block']=msg[5]
        trmsg[i]['subband']=msg[6]
        trmsg[i]['sum']=msg[7]
        trmsg[i]['max']=msg[8]
        trmsg[i]['obsID']=msg[9]
        trmsg[i]['beam']=msg[10]
        trmsg[i]['DM']=msg[11]
        if(msg[-1]==0x65):
            trmsg[i]['magic']=msg[-1]
# return the dictionary
    return trmsg

def filterTriggers(trmsg,subband, keyword,strictKeywordchecking=True):
    """ Returns the keyword at which the specified subband was triggered. A negative subband returns all keywords.
    
    The keyword should be out of
    
    'time',
    'utc_second',
    'utc_nanosecond',
    'length',
    'sample',
    'block',
    'subband',
    'sum',
    'max',
    'obsID',
    'beam',
    'DM', 
    
     """
    if strictKeywordchecking and keyword not in ['time','utc_second','utc_nanosecond','length','sample','block','subband','sum','max','obsID','beam','DM']:
        raise ValueError("Keyword "+str(keyword)+" not in database")
    if type(subband) != type(1):
        raise ValueError("Subband "+str(subband)+" is not an integer")
    if subband < 0: 
        return [a[keyword] for a in trmsg.values()]
    return [a[keyword] for a in trmsg.values() if a['subband']==subband]

def filterTriggers2(trmsg,subband, keyword1, keyword2):
    """ Returns the values of both keywords at which the specified subband was triggered. A negative subband returns values for all subbands.
    
    The keyword should be one out of
    
    'time',
    'utc_second',
    'utc_nanosecond',
    'length',
    'sample',
    'block',
    'subband',
    'sum',
    'max',
    'obsID',
    'beam',
    'DM', 

    * trmsg     * database with trigger messages
    * subband   * specified observation band, or negative for all streams
    * keyword 1 * first keyword for which values are returned
    * keyword 2 * second keyword for which values are returned
    
    return value:
        list of tuple(value1,value2) for each message.
     """

# Check if keyword is a valid keyword. Maybe replace this with:
# if keyword in trmsg[0].keys()
    if keyword1 not in ['time','utc_second','utc_nanosecond','length','sample','block','subband','sum','max','obsID','beam','DM']:
        raise ValueError("Keyword "+str(keyword1)+" not in database")
    if keyword2 not in ['time','utc_second','utc_nanosecond','length','sample','block','subband','sum','max','obsID','beam','DM']:
        raise ValueError("Keyword "+str(keyword2)+" not in database")
    if type(subband) != type(1):
        raise ValueError("Subband "+str(subband)+" is not an integer")
    if subband < 0: 
        return [(a[keyword1],a[keyword2]) for a in trmsg.values()]
    return [(a[keyword1],a[keyword2]) for a in trmsg.values() if a['subband']==subband]

def valueInRange(array1, array2, margin):
    """ Calculates which values of array 1 are within the margin of any value in array2 assuming sorted arrays"""
    return [a1 for a1 in array1 if bisect(array2,a1-margin,0)-bisect(array2,a1+margin,0) < 0 ]
    #return [a1 for a1 in array1 if len([a2 for a2 in array2 if abs(a1-a2)<= margin])>0 ]

def valueNotInRange(array1, array2, margin):
    """ Calculates which values of array 1 are not within the margin of any value in array2 assuming sorted arrays"""
# bisect calculates the index where to insert the value in the list, assuming \
# the list is sorted. If the two values will be inserted at the same place,
# there is no value in between.
    return [a1 for a1 in array1 if bisect(array2,a1-margin,0)-bisect(array2,a1+margin,0) == 0 ]

def valueInRangeExclude(array1, array2, margin):
    """ Calculates which values of array 1 are within the margin but not equal to any value in array2 assuming sorted arrays"""

    return [a1 for a1 in array1 if len([a2 for a2 in array2 if 0 < abs(a1-a2)<= margin])>0 ]

def valueInRangeTuple(array1, array2, margin):
    """ Calculates which values of array 1 are within the margin of any value in array2 for the first value of a tupple."""

    return [a1 for a1 in array1 if len([a2[0] for a2 in array2 if abs(a1[0]-a2[0])<= margin])>0 ]

def loadTimeseries(directory='',DM=DM):
    """ loads all streams of a dedispersed timeseries from files with name
        directory/dedisp*DM*
        * directory * Directory where files are stored
        * DM        * DM of file, should be in filename
        returns list of numpy arrays
    """

    if len(directory) > 0 and directory[-1]!='/':
        directory+='/'
    dfiles=glob.glob(directory+'dedisp*'+str(DM)+'*')
    ts=[np.fromfile(f,'float32') for f in dfiles]
    return ts

def obsParameters(directory='',file=None):
    """ 

    """
    if len(directory) > 0 and directory[-1]!='/':
        directory+='/'
    if not file:
        files=glob.glob(directory+'*beam*.sh')
        if len(files)<1:
            files=glob.glob(directory+'frats_dedisperse.sh')
            if len(files)<1:
                raise NameError('No *beam*.sh or frats_dedisperse.sh file found in directory '+directory)
            else:
                print 'Using frats_dedisperse.sh file'
        else:
            print "Using beam*.sh file"
        
    else:
        files=[directory+'/'+file]
    if len(files)>1:
        raise ValueError("Too many beam files in directory")
    rawdata=open(files[0]).readlines()
    if len(rawdata)==1:
        rawdata=rawdata[0].strip().split()
    else:
        rawdata=[l for l in rawdata if ('FRATStestRFI' in l and 'cat' not in l) or ( 'FRATSdedisperse' in l and 'cat' not in l )]
        if ';' in rawdata[0]:
            rawdata=rawdata[0].split(';')[0]
        else:
            rawdata=rawdata[0]
        rawdata=rawdata.strip().split()
    parameters=dict()
    newkeyword=False
    nrset=False
    count=0
    nr=0
    keyword=''
    intkeywords=['ch', 'Cnr', 'stb', 'startSB', 'n', 'nrCSB', 'il', 'tInt', 'sa', 'obsstart', 'Ctime']
    floatkeywords=['tl']
    listkeywords=['freq', 'DM','ilrange'] 
    rangekeywords=['DMrange']
    for i in rawdata[1:]:
        if not newkeyword:
            if i[0]=='-':
                newkeyword=True
                keyword=i[1:]
        else: #new keyword
            if i[0]=='-':
                parameters[keyword]=True
                keyword=i[1:]
            else: # fill in single value of keyword     
                if keyword not in listkeywords and keyword not in rangekeywords:
                    if keyword in intkeywords:
                        parameters[keyword]=int(i)
                    elif keyword in floatkeywords:
                        parameters[keyword]=float(i)
                    else:
                        parameters[keyword]=i
                    newkeyword=False
                elif keyword in rangekeywords:
                    print keyword,'!'
                    if not nrset:
                        nr=3
                        nrset=True
                        count=1
                        parameters[keyword]=[]
                        parameters[keyword].append(float(i))
                    else:
                        if count<nr-1:
                            parameters[keyword].append(float(i))
                            count+=1
                        else:
                            parameters[keyword].append(float(i))
                            count+=1
                            nrset=False
                            newkeyword=False
                else: # keyword is DM or freq or ilrange
                    if not nrset:
                        nr=int(i)
                        nrset=True
                        count=0
                        parameters[keyword]=[]
                    else:
                        if count<nr-1:
                            parameters[keyword].append(float(i))
                            count+=1
                        else:
                            parameters[keyword].append(float(i))
                            count+=1
                            nrset=False
                            newkeyword=False
    for k in rangekeywords:
        if k in parameters.keys():
            parameters[k[0:-5]]=np.arange(parameters[k][0],parameters[k][2],parameters[k][1])
    return parameters

def derivedParameters(par,DM=DM):
    """ This sets some extra parameters, given a set of parameters derived from a parset.
        * par *            set of parameters obtained with obsParameters()
        * DM *             Dispersion measure used for deriving delays.

        * timeresolution * of the data
        * nrfreqs *        total number of channels
        * startchannels *  of each band
        * refFreqs *       of each band
        * DMforDelays *    DM used when deriving the delays of each band
        * delays *         delays for each band for the specified DM
        * alldelays *      delays for all channels for the specified DM
        * delaysPerDM *    multiply with DM to get delays for each band
        * alldelaysPerDM * mulyiply with DM to get delays for all frequencies
        * nrdivs *         how many streams are used
        * nrstreams *      how many streams are used
    
    """ 
    par['timeresolution']=5e-9*1024*par['ch']*par['tInt']
    par['nrfreqs']=len(par['freq'])
    par['startchannels']=range(0,par['nrfreqs'],par['nrCSB']*par['ch'])
    par['refFreqs']=np.array(par['freq'])[par['startchannels']]
    par['DMforDelays']=DM
    par['delays']=dd.freq_to_delay(DM,par['refFreqs'],par['timeresolution'])
    par['alldelays']=dd.freq_to_delay(DM,par['freq'],par['timeresolution'])
    par['delaysPerDM']=dd.freq_to_delay(1,par['refFreqs'],par['timeresolution'])
    par['alldelaysPerDM']=dd.freq_to_delay(1,par['freq'],par['timeresolution'])
    par['nrdivs']=len(par['startchannels'])
    par['nrstreams']=par['nrdivs']

def copyPart(par,DM,sample):
    """ Cuts out part of the data from its location on helios around "sample" for the
    given DM.
    * par *    observation parameters
    * DM *     Dispersion measure
    * sample * Sample to cut around

    """
    derivedParameters(par,DM)
    delaylength=int(par['alldelays'][0])
    startSample=int(sample-delaylength-150)
    nrSamples=int(delaylength+300)
    infile=par['i'].split('/')[-1]
    remotedir="/mnt/lofar1/lofar/FRATS/data/"
    from os import environ
    localdir=environ["FRATS_DATA_DIR"]    
    if startSample<0:
        startSample=0
    size=int(par['nrfreqs']*4)
    if size%1024==0:
        size/=1024
        sizestr=str(size)+'k'
    else:
        size/=2
        sizestr=size+'w'
    print "ssh helios dd bs="+sizestr+" count="+str(nrSamples)+" skip="+str(startSample)+" if="+remotedir+str(infile)+" of="+remotedir+"temp.raw"
    print "scp helios:"+remotedir+"temp.raw "+localdir+str(infile)[0:-4]+"st_"+str(startSample)+".raw"

        


def getDirectory(obsid,run,maindir='/Users/STV/Astro/Analysis/FRAT/analysis/'):
    """ Returns maindir/obs$obsid/run$run , the location of previous analysed data """
    obsid=str(obsid)
    run=str(run)
    if 'obs' not in obsid:
        obsid='obs'+obsid
    if 'run' not in run:
        run='run'+run
    return maindir+'/'+obsid+'/'+run+'/'

def getPulsedir(obsid,sap,pulse,maindir=None,checkdir=True):
    """ Returns directory where a pulse is stored. 
        * obsid *    Observation ID. L????? or ?????
        * sap *      Sub Array Pointing SAP???
        * pulse *    pulse nr (integer or pulse??)
        * maindir *  default: FRATS_ANALYSIS_DIR
        * checkdir * Check if directory exists

        returns maindir/L?????/SAP???/pulse?/
    
    """
    if type(obsid) == type(1): #int
        obsid='L'+str(obsid)
    if type(sap) == type(1): #int
        if sap < 10:
            sap='SAP00'+str(sap)
        elif sap < 100:
            sap='SAP0'+str(sap)
        else:
            sap='SAP'+str(sap)
    if type(pulse) == type(1):
        pulse='pulse'+str(pulse)
    if not maindir:
        from os import environ
        maindir=environ['FRATS_ANALYSIS_DIR']
    from os import path
    pulsedir=maindir+'/'+obsid+'/'+sap+'/'+pulse+'/' 
    if checkdir:
        if not path.isdir(pulsedir):
            raise NameError('Directory '+pulsedir+' does not exist. If you want to skip this check, use checkdir=False as an argument')
    return pulsedir

                
    

def coincidenceSummary(trmsg,pulses=anadb['pulses'],nrstreams=10,windows=[10,20,30,40,50,60,70,80,90,100]):
    """ Make a summary of the coincidences w.r.t. known pulses
    
        * trmsg *       database with triggermessages
        * pulses *      list with pulse times
        * nrstreams *   number of streams
        * windows *     timewindows of coincidences
        

        returns data base with:
            coincidence[number][window] * from coincidencesCount
            triggercoincidence[number][window] * from triggercoincidencesCount
            anticoincidence[number][window] * from anticoincidencesCount
        and as extra
            times  | trigger times per band
            totnr  | triggers per band
          

    """
    coinDB=dict()
    # all the triggered times per subband
    coinDB['times']=[filterTriggers(trmsg,s,'time') for s in range(nrstreams)]
    # nr of triggers per subband 
    coinDB['totnr']=[len(t) for t in coinDB['times']]
    # triggers in each subband that are within a range of the normal pulses
    coinDB['coincidence']=dict()
    coinDB['anticoincidence']=dict()
    coinDB['triggercoincidence']=dict()
    for number in range(1,nrstreams+1):
        coinDB['coincidence'][number]=dict()
        coinDB['triggercoincidence'][number]=dict()
        coinDB['anticoincidence'][number]=dict()
        for window in windows:
            coinDB['coincidence'][number][window]=coincidencesCount(pulses,coinDB['times'],number,window)
            coinDB['triggercoincidence'][number][window]=triggercoincidencesCount(coinDB['times'],number,window)
            coinDB['anticoincidence'][number][window]=anticoincidencesCount(pulses,coinDB['times'],number,window)
    return coinDB

def coincidencesCount(pulses,times,number,window):
    """ Check how often a coincidence between streams falls within the timewindow of the
    assumed arrival time of the pulses.
     
        * pulses * times of arrival of actual pulses
        * times *  trigger times
        * number * number of coincidences
        * window * timewindow in which the coincidence should fall. NOTE: could be double the window.
    
        returns: number of coincidences
    
    """
    nrstreams=len(times)
    if number > nrstreams:
        print "Warning, not possible to get this number of counts. There are not so many streams"
        return 0
    import itertools
    comb=itertools.combinations(range(nrstreams),number)
    totalfound=0
    all=[]
    for c in comb:
        d=pulses
        for j in c:
            d=valueInRange(d,times[j],window)
        all+=list(d)
    return len(set(all))

def coincidencesCountFast(pulses,timesSub,number,window):
    """ Check how often a coincidence between streams falls within the timewindow of the
    assumed arrival time of the pulses. Faster method than coincidencesCount.
     
        * pulses * times of arrival of actual pulses
        * times *  trigger times
        * number * number of coincidences
        * window * timewindow in which the coincidence should fall. NOTE: could be double the window.
    
        returns: number of coincidences
    """
    #timesSub=np.array(timesSub)
    import itertools
    nrcoin=[len(set(timesSub[np.searchsorted(timesSub[:,0],p-window/2):np.searchsorted(timesSub[:,0],p+window/2)][:,1])) for p in pulses]
    nrcoin=np.array(nrcoin)
    if type(number) == type([]):
        all=[nrcoin[nrcoin>=n] for n in number]
    return all


def coincidencesFast(pulses,timesSub,number,window):
    """ Returns number of coincidences with each assumed pulse. 
    
        * pulses * times of arrival of actual pulses
        * times *  trigger times
        * number * number of coincidences
        * window * timewindow in which the coincidence should fall. NOTE: could be double the window.
    
        returns: number of coincidences
    """
    
    #timesSub=np.array(timesSub)
    import itertools
    nrcoin=[len(set(timesSub[np.searchsorted(timesSub[:,0],p-window/2):np.searchsorted(timesSub[:,0],p+window/2)][:,1])) for p in pulses]
    return nrcoin

def triggercoincidenceSets(timesSub,number,window):
    """ See triggercoincidenceSets2, though i'm not sure what input this function needs. It looks a bit different.
    """
    pulses=timesSub[:,0]
    coinsets=[timesSub[np.searchsorted(timesSub[:,0],p):np.searchsorted(timesSub[:,0],p+window)] for p in pulses]
# the minimum here is not necessary the minimum at each subband, because one subband could appear twice in the window and this takes the mimimum of those.
    coinsets2=[(c[[0,-1],0],len(set(c[:,1])),min(c[:,2])) for c in coinsets]
    coinsets3=[c for num,c in enumerate(coinsets2) if c[0][1] != coinsets2[num-1][0][1] ]#and c[1]!=1]
    return coinsets3

def triggercoincidenceSets2(timesSub,window):
    """ 
    Function checks for all messages withing time to time+window for all 
    timestamps. For each timestamp it checks the number of unique subbands 
    and the minimum trigger level. It doesn't check if a subband appears 
    twice with different trigger levels. It also doesn't check for 
    overlapping triggers.


    * timesSub * time-sorted array with 4 columns. The colums are: 
    time, subband, max, DM. The DM should be selected beforehand.
    * window * timewindow in which coincidences fall 
    

    Returns: time, nrOfCoincidences, minimum triggerlevel, duration"""


    pulses=timesSub[:,0]
    coinsets=[timesSub[np.searchsorted(timesSub[:,0],p):np.searchsorted(timesSub[:,0],p+window)] for p in pulses]
# the minimum here is not necessary the minimum at each subband, because one subband could appear twice in the window and this takes the mimimum of those.
    coinsets2=[(c[0,0],len(set(c[:,1])),min(c[:,2]),c[-1,0]-c[0,0]) for c in coinsets]
    coinsets3=[c for num,c in enumerate(coinsets2) if c[0] != coinsets2[num-1][0] ]#and c[1]!=1]
    return coinsets3

def selectTriggers(coinsets,pulses,number,window):
    """ returns number of triggers found with pulse within window and number of triggers found where pulses is outside the window.
    input:
    * coinsets *    sets of coincidence triggers
    * pulses *      arrival time of real triggers
    * number *      number of coincidences required.
    * window *      timewindow of the trigger. Actually takes double timewindow for searching.

    return tuple of #real coincicedence pulses, # non-coincidence triggers
    
    """
    wstart=[c[0][0] for c in coinsets if c[1]>=number]
    wstop=[c[0][1] for c in coinsets if c[1]>=number]
    pulsefound=[(True in [a<p<b or c<p<d for p in pulses],a) for a,b,c,d in zip(wstart,[w+window for w in wstart],[w-window for w in wstop],wstop)]
    return len([a for a in pulsefound if a[0]]),len([a for a in pulsefound if not a[0]])

def anticoincidencesCount(pulses,times,number,window):
    """ Check how often a time from a coincidence of streams doesn't fall in the correct window 
    of a pre-assumed position of a pulse.
    
    * pulses * known timestamps of arrival of a pulse
    * times * trigger times for each stream
    * number * number of coincidences required
    * window * timewindow in which the coincidence should take place.

    returns number of coindidences outside the pulse arrival times.

    """
    nrstreams=len(times)
    if number > nrstreams:
        print "Warning, not possible to get this number of counts. There are not so many streams"
        return 0
    import itertools
    comb=itertools.combinations(range(nrstreams),number)
    totalfound=0
    antitimes=[]
    for j in range(nrstreams):
        antitimes.append(valueNotInRange(times[j],pulses,window))
    all=[]
    for c in comb:
        d=antitimes[c[0]]
        for j in c[1:]:
            d=valueInRange(d,antitimes[j],window)
        all+=list(d)
    return len(set(all))

def triggercoincidencesCount(times,number,window):
    """ Count how many time a coincidence trigger over NUMBER streams is found.

    * times * list of lists of time stamp of triggers for each stream
    * number * how many coincidences should there be
    * window * time window in which the coincidences should fall.
    
    returns number of coincidences
    """
    nrstreams=len(times)
    if number > nrstreams:
        print "Warning, not possible to get this number of counts. There are not so many streams"
        return 0
    import itertools
    # all combinations of number out or nrstreams
    comb=itertools.combinations(range(nrstreams),number)

    totalfound=0
    all=[]
    for c in comb:
        d=times[c[0]]
        for j in c[1:]:
            d=valueInRange(d,times[j],window)
        all+=list(d)
    return len(set(all))

def plotCoincidence(coinDB,types,coinNrs,pretitle='',newfigure=True,totalwidth=10):
    """ Plot number of coincidences, anticoincidences and triggercoincidences as a bar diagram

    input:
    * coinDB *      coincidence database with different coincidnece types and numbers
    * types *       list of types to plot ( anticoincidence [or ac], coincidence [or c], triggercoincidence [or tc]
    * coinNrs *     which coinNrs to plot
    * pretitle *    additional start of the title of the plot
    * newfigure *   make a new figure
    * totalwidth *  width of all type bars together.


    """
    width=(totalwidth-1)/len(types)
    mycolor=dict()
    mycolor['anticoincidence']='r'
    mycolor['ac']='r'
    mycolor['coincidence']='g'
    mycolor['c']='g'
    mycolor['triggercoincidence']='m'
    mycolor['tc']='m'
    print types
    for c in coinNrs:
        if newfigure:
            plt.figure()
        for num,t in enumerate(types):
            plt.bar([k-width+num*width for k in coinDB[t][c].keys() if k < 60],[v for k,v in coinDB[t][c].iteritems() if k < 60],width=width,color=mycolor[t])
            #plt.legend(t)
            plt.title(pretitle+' C='+str(c))
            plt.xlabel('trigger window size (W)')
            plt.ylabel('number of triggers')

def plotCoincidenceWindowfixed(coinDB,types,coinNrs,window,pretitle='',newfigure=True,totalwidth=10):
    """ Plot number of coincidences, anticoincidences and triggercoincidences as a bar diagram

    input:
    * coinDB *      coincidence database with different coincidnece types and numbers
    * types *       list of types to plot ( anticoincidence [or ac], coincidence [or c], triggercoincidence [or tc]
    * coinNrs *     which coinNrs to plot
    * pretitle *    additional start of the title of the plot
    * newfigure *   make a new figure
    * totalwidth *  width of all type bars together.
    """    

    width=(totalwidth-1)/len(types)
    mycolor2=dict()
    mycolor['anticoincidence']='r'
    mycolor['ac']='r'
    mycolor['coincidence']='g'
    mycolor['c']='g'
    mycolor['triggercoincidence']='m'
    mycolor['tc']='m'
    if newfigure:
        plt.figure()
    for num,t in enumerate(types):
        plt.bar([c-width+num*width for c in coinNrs],[coinDB[t][c][window] for c in coinNrs],width=width,color=mycolor2[t])
            #plt.legend(t)
        plt.title(pretitle+' W='+str(window))
        plt.xlabel('number of coincidence channels')
        plt.ylabel('number of triggers')

def getBaseline(directory,filename="baseline.log",channels=5120,matrixoutput=True):
    """ Load the baseline of each block of data.
    
    * directory * directory where the file is stored
    * filename *  name of the file
    * channels *  total number of channels in the file
    * matrixoutput * if True returns 2-dimensional array, else returns list of arrays.

    """
    flaggedchans=np.fromfile(directory+'/'+filename,dtype='float32')
    fc=flaggedchans
    start=0
    fc2=[]
    a=-1
    while 1:#fc[start]==a:
        start+=1
        nr=fc[start]
        start+=1
        if fc[start-2]==a+1:
            a+=1
            fc2.append(fc[start:start+nr])
        elif fc[start-2]==a:
            fc2[-1]=np.append(fc2[-1],fc[start:start+nr])
        else:
            break
        start+=nr
        if start >= len(fc):
            break
    if matrixoutput:
        times=len(fc2)
        #channels=mymax([mymax(f) for f in fc2])
        fcs=np.zeros([times,channels])
        for num,f in enumerate(fc2):
            fcs[num]=f
        return fcs
    else:
        return fc2
    
def getFlaggedChannels(directory,filename="flaggedchans.log",channels=5120,matrixoutput=True):
    """ Load the flagged channels of each block of data.

    * directory * directory where the file is stored
    * filename *  name of the file
    * channels *  total number of channels in the file
    * matrixoutput * if True returns 2-dimensional array for blocks and channels.
                     if a channel is flagged in a certain block, the value is 1. Else it's 0.
                     if False, returns list of arrays of indices of flagged channels.

    """
    flaggedchans=np.fromfile(directory+'/'+filename,dtype='int32')
    fc=flaggedchans
    start=0
    fc2=[]
    fc2val=[]
    a=-1
    while 1:#fc[start]==a:
        start+=1
        nr=fc[start]
        start+=1
        if fc[start-2]==a+1:
            a+=1
            fc2.append(fc[start:start+2*nr:2])
            fc2val.append(fc[start+1:start+2*nr+1:2])
        elif fc[start-2]==a:
            fc2[-1]=np.append(fc2[-1],fc[start:start+2*nr:2])
            fc2val[-1]=np.append(fc2val[-1],fc[start+1:start+2*nr+1:2])
        else:
            break
        start+=2*nr
        if start >= len(fc):
            break
    if matrixoutput:
        times=len(fc2)
        #channels=mymax([mymax(f) for f in fc2])
        fcs=np.zeros([times,channels])
        for num,f in enumerate(fc2):
            fcs[num,f]=1
        return fcs
    else:
        return fc2

def getFlaggedSamples(directory,filename="flaggedsamples.log",blocksize=192,matrixoutput=True):
    """ Load the flagged samples of each block of data.

    * directory * directory where the file is stored
    * filename *  name of the file
    * blocksize *  samples per block
    * matrixoutput * if True returns 2-dimensional array for blocks and samples.
                     if a sample is flagged in a certain block, the value is 1. Else it's 0.
                     if False, returns list of arrays of indices of flagged samples.

    """
    flaggedsamps=np.fromfile(directory+'/'+filename,dtype='int32')
    fs=flaggedsamps
    start=0
    fs2=[]
    a=0
    while fs[start]==a:
        start+=1
        nr=fs[start]
        start+=1
        fs2.append(fs[start:start+nr])
        start+=nr
        a+=1
        if start >= len(fs):
            break
    if matrixoutput:
        times=len(fs2)
        fss=np.zeros([times,blocksize])
        for num,f in enumerate(fs2):
            fss[num,f]=1
        return fss
    else:
        return fs2


def mymax(lst):
    """ return maximum of a list, if list has any values. Else returns 0"""
    print lst
    if len(lst)<1:
        return 0
    else:
        return max(list)

def loadAverage(nrDMs,nrStreams,directory,filename='average.log'):
    """ load averages of each block
        * nrDMs *  number of DMs on which the trigger is run and thus for which an average exists
        * nrStreams * number of streams, that all have a seperate overage.
        * directory * directory where the file is stored
        * filename *  name of file
    
        returns dictionary with averages for each DM and stream 
    """ 
    f=open(directory+'/'+filename)
    f.seek(0)
    datadict=dict()
    for DM in range(nrDMs):
        for st in range(nrStreams):
            (DMr,stream,nr)=struct.unpack('fii',f.read(12))
            if DMr not in datadict.keys():
                datadict[DMr]=dict()
            datadict[DMr][stream]=np.fromfile(f,'float32',nr)
    return datadict

def loadStddev(nrDMs,nrStreams,directory,filename='stddev.log'):
    """ load standard deviation of each block
        * nrDMs *  number of DMs on which the trigger is run and thus for which an average exists
        * nrStreams * number of streams, that all have a seperate overage.
        * directory * directory where the file is stored
        * filename *  name of file
    
        returns dictionary with averages for each DM and stream 
    """ 
    f=open(directory+'/'+filename)
    f.seek(0)
    datadict=dict()
    for DM in range(nrDMs):
        for st in range(nrStreams):
            (DMr,stream,nr)=struct.unpack('fii',f.read(12))
            if DMr not in datadict.keys():
                datadict[DMr]=dict()
            datadict[DMr][stream]=np.fromfile(f,'float32',nr)
    return datadict

def makeTimeAxes(reftime,delays,length,unit=1):
    """ Make time axis for timeseries data. 
     * reftime: reference time
     * delays:  list of delays with respect to reftime
     * length:  lenght of array
     * unit:    timestep 

    """
    timeaxes=np.zeros([len(delays),length])
    for num,d in enumerate(delays):
        timeaxes[num]=np.arange(reftime-d,reftime-d+length*unit,unit)
    return timeaxes

def makeSets(msg,threshold,window):
    """ 
    Returns coincidence sets for each DM. 
    
    * msg *       database with triggermessages
    * threshold * only uses messages above this threshold
    * window *    time window for finding coincidences
    
    Returns array of time, nr of streams triggered, lowest trigger threshold and DM as database entry for each DM.


    """
    filterDatabase(msg,'max',threshold,1000000000)
    if len(msg.keys())<1:
        return False
    trval=dict()
    for k in msg.values()[0].keys():
        trval[k]=filterTriggers(msg,-1,k)
    trval['DM']=[round(d,2) for d in trval['DM']]
    DMvalues=list(set(trval['DM']))
    timesSubband=zip(trval['time'],trval['subband'],trval['max'],trval['DM'])
# sort on time
    timesSubband.sort() 
    timesSubband=np.array(timesSubband)
    alltimesSubband=np.copy(timesSubband)
    sets=dict()
    for DM in DMvalues:
        sets[DM]=np.array(triggercoincidenceSets2(timesSubband[timesSubband[:,3]==DM],window))   
    print "tuples of time, nr subband, lowest trigger threshold, DM"
    return sets

def filterSets(sets,coincidenceThreshold):
    """ filters set made by makeSets, such that the pulse power should at least be coincidenceThreshold in all streams.
    

    """
    sets2=dict()    
    for k,v in sets.iteritems():
        sets2[k]=[s for s in v if s[1]>=coincidenceThreshold]
    return sets2

def plotSets(sets,saveFig,coincidences,timeres,pdfp,obsid,sap,cumulative=False,title=None):
    """ plot coincidence triggers in time vs DM and save to figure if required
    input:
    * sets *            set of trigger coincidence messages
    * saveFig *         save to provided pdf file
    * coincidences *    list of number of coincidences required for the plot
    * timeres *         time resolution
    * pdfp *            pdf pages object for saving figures.
    """
    colors=['k','r','g','b','c','m']
    sizes=[5,15,15,15,15,15]
    for c in coincidences:
        if c<len(colors)-1:
            col=colors[c-1]
            siz=sizes[c-1]
        else:
            col=colors[-1]
            siz=sizes[-1]
        if c>1:
            mysets=filterSets(sets,c)
        else:
            mysets=sets
        print c 
        totallen=np.sum([len(v) for k,v in mysets.iteritems()])
        xdata=np.zeros(totallen)
        ydata=np.ones(totallen)
        start=0
        end=0
        for k,v in mysets.iteritems():
            start=end
            end+=len(v)
            try:
                if len(v)>0:
                    xdata[start:end]=np.array(v)[:,0]
                    ydata[start:end]*=k
            except:
                return xdata,start,end,v
        xdata*=timeres
        plt.plot(xdata,ydata,'.',color=col,ms=siz)
        #[xdata.append(np.array(v)*timeres) for k,v in mysets.iteritems()]
        #[plt.plot(np.array(v)*timeres,k*np.ones(len(v)),'.',color=col,ms=siz) for k,v in mysets.iteritems()]
        plt.xlabel('time (seconds), dedispersed to lowest frequency')
        plt.ylabel('DM (cm^-3 parsec)')
        #plt.ylim(ymin=11.25,ymax=13.7)
        plt.ylim(ymin=1,ymax=31)
        #plt.yticks(range(1,22,2))
        if not title:
            plt.title('Triggers in at least '+str(c)+' channel for '+obsid+' '+sap)
        else:
            plt.title(title)
        if not cumulative:
            if saveFig:
                pdfp.savefig() 
            plt.figure()
    if cumulative and saveFig:
        pdfp.savefig() 
        
        
        

def addmsg(msg,msgnew):
    """ Adds databases to another database. The keys of each database are 
    continuous numbers. The new keys are continously numbered. 

    * msg * Database that gets new database added to it
    * msgnew * Database that the data is taken from.
    """
    msgk=msg.keys()
    if len(msg.keys())==0:
        nextk=0
    else:
        nextk=max(msgk)+1
    msgnewk=msgnew.keys()
    firstk=min(msgnewk)
    for k,v in msgnew.iteritems():
        msg[k-firstk+nextk]=v

        
