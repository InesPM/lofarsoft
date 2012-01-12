"""This module reads in calibration metadata from file in the early fases of LOFAR. In the future this should be replaced by reading the metadata from the files.

.. moduleauthor:: Sander ter Veen <s.terveen@astro.ru.nl>
"""

## Imports
import numpy as np
import struct
import os
import pycrtools as cr
    
    
lonlatCS002=cr.hArray([6.869837540*np.pi/180.,52.91512249*np.pi/180.]) # 49.344
ITRFCS002=cr.hArray([3826577.109500000,461022.916000,5064892.758])

def mapAntennasetKeyword(antennaset):
    """Ugly fix to map correct antenna names in input to wrong antenna names
    for metadata module.
    """

    # Strip whitespace
    antennaset = antennaset.strip()

    incorrect = { 'LBA_INNER' : 'LBA_INNER',
                  'LBA_OUTER' : 'LBA_OUTER',
                  'HBA_ZERO' : 'HBA_0',
                  'HBA_ONE' : 'HBA_1',
                  'HBA_DUAL' : 'HBA',
                  'HBA_JOINED' : 'HBA' }

    if antennaset in incorrect:
        antennaset = incorrect[antennaset]
    elif antennaset == "HBA_BOTH":
        # This keyword is also wrong but present in file headers
        print "Keyword " + antennaset + " does not comply with ICD, mapping..."
        antennaset = "HBA"
    else:
        pass

    return antennaset

def get(keyword, antennaIDs, antennaset, return_as_hArray=False):
    """Return metadata values, given the antennaIDs and the antennaset.

    Required arguments:

    ================== ====================================================
    Parameter          Description
    ================== ====================================================
    *keyword*          Any of StationPhaseCalibration, CableDelays,
                       RelativeAntennaPositions, ClockCorrection,
                       StationPositions
    *antennaIDs*       AntennaIDs as given by crfile["antennaIDs"]
    *antennaset*       Antennaset used for this station. Options:

                       * LBA_INNER
                       * LBA_OUTER
                       * LBA_X
                       * LBA_Y
                       * LBA_SPARSE0
                       * LBA_SPARSE1
                       * HBA_0
                       * HBA_1
                       * HBA

    ================== ====================================================

    Optional arguments:

    ================== ====================================================
    Parameter          Description
    ================== ====================================================
    *return_as_hArray* Default False
    ================== ====================================================

    """

    antennaset = mapAntennasetKeyword(antennaset)

    dim2=0
    if keyword is "StationPhaseCalibration":
        functionname=getStationPhaseCalibration
    elif keyword is "CableDelays":
        functionname=getCableDelays
        dim2=1
    elif keyword is "RelativeAntennaPositions":
        functionname=getRelativeAntennaPositions
    elif keyword is "AbsoluteAntennaPositions":
        functionname=getAbsoluteAntennaPositions
    elif keyword is "AntennaPositions":
        functionname=getAntennaPositions
    elif keyword is "ClockCorrection":
        functionname=getClockCorrection
        dim2=1
    elif keyword is "StationPositions":
        functionname=getStationPositions
    else:
        print "function not supported."
        return False

    #import pycrtools as hf
    if isinstance(antennaIDs,cr.IntArray):
        antennaIDs=antennaIDs.vec()
    if isinstance(antennaIDs,list):
        if isinstance(antennaIDs[0], str):
            antennaIDs=cr.Vector([int(ID) for ID in antennaIDs])
        else:
            antennaIDs=cr.Vector(antennaIDs)
    stationIDs=np.array(list(antennaIDs))/1000000
    rcuIDs=np.mod(antennaIDs,1000)
    allStIDs=np.unique(stationIDs)
    allValues={}

    for station in allStIDs:
        allValues[station]=functionname(int(station),antennaset,return_as_hArray)

    if not return_as_hArray:
        if keyword not in ["StationPositions","ClockCorrection"]:
            if dim2==1:
                mdata=np.zeros(len(antennaIDs),dtype=allValues[allStIDs[0]].dtype)
            else:
                mdata=np.zeros((len(antennaIDs),allValues[allStIDs[0]].shape[1]),dtype=allValues[allStIDs[0]].dtype)

            for st in allStIDs:
                mdata[stationIDs==st]=allValues[st][rcuIDs[stationIDs==st]]
        else:
            if dim2==1:
                mdata=np.zeros(len(antennaIDs))
            else:
                mdata=np.zeros((len(antennaIDs),allValues[allStIDs[0]].shape[0]),dtype=allValues[allStIDs[0]].dtype)

            for st in allStIDs:
                mdata[stationIDs==st]=allValues[st]
    else:
        #import pycrtools as hf
        if keyword in ["StationPhaseCalibration"]:
            if dim2==1:
                #mdata=cr.hArray(allValues[allStIDs[0]],len(antennaIDs))
                mdata=cr.hArray(complex,len(antennaIDs))
            else:
                mdata=cr.hArray(complex,[len(antennaIDs),allValues[allStIDs[0]].shape()[1]])
                #mdata=cr.hArray(allValues[allStIDs[0]],[len(antennaIDs),allValues[allStIDs[0]].shape()[1]])
            for st in allStIDs:
                for num, check in enumerate(stationIDs==st):
                    if check:
                        mdata[num]=allValues[st][int(rcuIDs[num])]

        if keyword not in ["StationPositions","ClockCorrection"]:
            if dim2==1:
                #mdata=cr.hArray(allValues[allStIDs[0]],len(antennaIDs))
                mdata=cr.hArray(float,len(antennaIDs))
            else:
                mdata=cr.hArray(float,[len(antennaIDs),allValues[allStIDs[0]].shape()[1]])
                #mdata=cr.hArray(allValues[allStIDs[0]],[len(antennaIDs),allValues[allStIDs[0]].shape()[1]])
            for st in allStIDs:
                for num, check in enumerate(stationIDs==st):
                    if check:
                        mdata[num]=allValues[st][int(rcuIDs[num])]
        else:
            if keyword == "ClockCorrection": #dim2==1
                mdata=cr.hArray(float,len(antennaIDs),fill=0)
            else:
                mdata=cr.hArray(float,[len(antennaIDs),allValues[allStIDs[0]].shape()[0]])
            for st in allStIDs:
                for num, check in enumerate(stationIDs==st):
                    if check:
                        mdata[num]=allValues[st]
    return mdata


def getStationPhaseCalibration(station, antennaset,return_as_hArray=False, caltable_location=None):
    """Read phase calibration data for a station.

    Required arguments:

    ================== ====================================================
    Parameter          Description
    ================== ====================================================
    *station*          station name or ID.
    *mode*             observation mode. Currently supported: "LBA_OUTER"
    ================== ====================================================

    Optional arguments:

    ================== ====================================================
    Parameter          Description
    ================== ====================================================
    *return_as_hArray* Default False
    ================== ====================================================

    returns weights for 512 subbands.

    Examples::

        >>> metadata.getStationPhaseCalibration("CS002","LBA_OUTER")
        array([[ 1.14260161 -6.07397622e-18j,  1.14260161 -6.05283530e-18j,
             1.14260161 -6.03169438e-18j, ...,  1.14260161 +4.68675289e-18j,
             1.14260161 +4.70789381e-18j,  1.14260161 +4.72903474e-18j],
           [ 0.95669876 +2.41800591e-18j,  0.95669876 +2.41278190e-18j,
             0.95669876 +2.40755789e-18j, ...,  0.95669876 -2.41017232e-19j,
             0.95669876 -2.46241246e-19j,  0.95669876 -2.51465260e-19j],
           [ 0.98463207 +6.80081617e-03j,  0.98463138 +6.89975906e-03j,
             0.98463069 +6.99870187e-03j, ...,  0.98299670 +5.71319125e-02j,
             0.98299096 +5.72306908e-02j,  0.98298520 +5.73294686e-02j],
           ...,
           [ 1.03201290 +7.39535744e-02j,  1.03144532 +8.14880844e-02j,
             1.03082273 +8.90182487e-02j, ..., -0.82551740 -6.23731331e-01j,
            -0.82094046 -6.29743206e-01j, -0.81631975 -6.35721497e-01j],
           [ 1.12370332 -1.15296909e-01j,  1.12428451 -1.09484545e-01j,
             1.12483564 -1.03669252e-01j, ..., -0.92476286 +6.48703460e-01j,
            -0.92810503 +6.43912711e-01j, -0.93142239 +6.39104744e-01j],
           [ 1.10043006 -6.18995646e-02j,  1.10075250 -5.58731668e-02j,
             1.10104193 -4.98450938e-02j, ..., -1.01051042 +4.40052904e-01j,
            -1.01290481 +4.34513198e-01j, -1.01526883 +4.28960464e-01j]])

        >>> metadata.getStationPhaseCalibration(122,"LBA_OUTER")
        Calibration data not yet available. Returning 1
        array([[ 1.+0.j,  1.+0.j,  1.+0.j, ...,  1.+0.j,  1.+0.j,  1.+0.j],
           [ 1.+0.j,  1.+0.j,  1.+0.j, ...,  1.+0.j,  1.+0.j,  1.+0.j],
           [ 1.+0.j,  1.+0.j,  1.+0.j, ...,  1.+0.j,  1.+0.j,  1.+0.j],
           ...,
           [ 1.+0.j,  1.+0.j,  1.+0.j, ...,  1.+0.j,  1.+0.j,  1.+0.j],
           [ 1.+0.j,  1.+0.j,  1.+0.j, ...,  1.+0.j,  1.+0.j,  1.+0.j],
           [ 1.+0.j,  1.+0.j,  1.+0.j, ...,  1.+0.j,  1.+0.j,  1.+0.j]])

    """

    # Return mode nr depending on observation mode
    antennasetToMode = { "LBA_OUTER" : "1",
                         "LBA_INNER" : "3",
                         "HBA" : "5",
                         "HBA0" : "5",
                         "HBA1" : "5",
                         "HBA_ZERO" : "5",
                         "HBA_ONE" : "5",
                         "HBA_DUAL" : "5",
                         "HBA_JOINED" : "5" }

    if antennaset not in antennasetToMode.keys():
        raise KeyError("Not a valid antennaset "+antennaset)

    modenr=antennasetToMode[antennaset]
    if isinstance(station, int):
        # Convert a station id to a station name
        station=idToStationName(station)

    station=stationNameToNR(station)

    # filename
    if not caltable_location:
        caltable_location = LOFARSOFT=os.environ["LOFARSOFT"].rstrip('/')+'/'+'/data/lofar/StaticMetaData/CalTables'

    filename=caltable_location+'/CalTable_'+station+'_mode'+modenr+'.dat'
    if os.path.isfile(filename):
        file=open(filename)
    else:
        print "Calibration data not yet available for station",station,"Returning 1"
        if return_as_hArray:
            #import pycrtools as hf
            complexdata=cr.hArray(complex,[96,512],fill=complex(1,0))
        else:
            complexdata=np.zeros(shape=(96,512),dtype=complex)
            complexdata.real=1
        return complexdata



    # reading in 96 * 512 * 2 doubles
    fmt='98304d'

    # Calculate the size to be read
    sz=struct.calcsize(fmt)

    # read from the file
    rawdata=file.read(sz)

    # unpack so the data is represented as doubles
    data=struct.unpack(fmt,rawdata)

    #
    if return_as_hArray:
        #import pycrtools as hf
        data=cr.hArray(data,[512*96,2])
        realdata=cr.hArray(float,[512*96])
        imagdata=cr.hArray(float,[512*96])
        complexdata=cr.hArray(complex,[96,512])
        realdata[...].copy(data[...,0])
        imagdata[...].copy(data[...,1])
        complexdata2=cr.hArray(complex,[512*96])
        complexdata2.fill(complex(0,1))
        complexdata2.mul(imagdata)
        complexdata2.add(realdata)
        complexdata2.reshape([512,96])
        complexdata.transpose(complexdata2)

        return complexdata
    else:
        data=np.array(data)
        data.resize(512,96,2)

        complexdata=np.empty(shape=(512,96),dtype=complex)
        complexdata.real=data[:,:,0]
        complexdata.imag=data[:,:,1]

        return complexdata.transpose()

def getCableDelays(station,antennaset,return_as_hArray=False):
    """ Get cable delays in seconds.

    Required arguments:

    ================== ====================================================
    Parameter          Description
    ================== ====================================================
    *station*          Station name or ID e.g. "CS302", 142
    *antennaset*       Antennaset used for this station. Options:

                       * LBA_INNER
                       * LBA_OUTER
                       * LBA_X
                       * LBA_Y
                       * LBA_SPARSE0
                       * LBA_SPARSE1
                       * HBA_0
                       * HBA_1
                       * HBA

    ================== ====================================================

    Optional arguments:

    ================== ====================================================
    Parameter          Description
    ================== ====================================================
    *return_as_hArray* Returns numpy array if False (default)
    ================== ====================================================

    returns "array of (rcus * cable delays ) for all dipoles in a station"

    """

    # Check station id type
    if isinstance(station, int):
        # Convert a station id to a station name
        station=idToStationName(station)
    # LBA_OUTER = LBL
    # LBA_INNER = LBH
    # HBA = HBA
    if "LBA_OUTER" in antennaset:
        rcu_connection="LBL"
    elif "LBA_INNER" in antennaset:
        rcu_connection="LBH"
    elif "HBA" in antennaset:
        rcu_connection="HBA"
    else:
        print "Antenna set not yet supported"
        return "Antenna set not yet supported"

    LOFARSOFT=os.environ["LOFARSOFT"].rstrip('/')+'/'
    cabfilename=LOFARSOFT+'/data/lofar/StaticMetaData/CableDelays/'+station+'-CableDelays.conf'
    cabfile=open(cabfilename)

    if return_as_hArray:
        #import pycrtools as hf
        cable_delays=cr.hArray(float,dimensions=[96])
    else:
        cable_delays=np.zeros(96)

    str_line=''
    while "RCUnr" not in str_line:
        str_line = cabfile.readline()
        if len(str_line) == 0:
            #end of file reached, no data available
            assert False

    str_line = cabfile.readline()
    for i in range(96):
        str_line = cabfile.readline()
        sep_line = str_line.split()
        if rcu_connection == "LBL":
            cable_delays[int(sep_line[0])]=float(sep_line[2])*1e-9
        elif rcu_connection == "LBH":
            cable_delays[int(sep_line[0])]=float(sep_line[4])*1e-9
        elif rcu_connection == "HBA":
            cable_delays[int(sep_line[0])]=float(sep_line[6])*1e-9

    return cable_delays

def idToStationName(station_id):
    """Returns the station name from a station_id
    The station_id is crfile["antennaIDs"]/1000000
    The station name is more commonly used to identify the station
    for example for the files with metadata"""

    if ( station_id > 255 ):
        print "Unvalid station id"
        return "Unvalid station id"
    digit1=station_id/100
    digit2=np.mod(station_id,100)/10
    digit3=np.mod(station_id,10)

    if digit1 == 0:
        station_name="CS"+str(digit1)+str(digit2)+str(digit3)
    elif digit1 == 2:
        digit1=6
        if digit2 != 0:
            print "Unkown international station please add it to the list"
            return "Unknown international station"
        if digit3 > 4:
            print "Unkown international station please add it to the list"
            return "Unknown international station"
        if digit3 <= 4:
            station_nr=digit1*100+digit2*10+digit3;
            station_name="DE"+str(station_nr)
    elif digit1==1:
        station_nr=digit1*100+(digit2/2)*100+np.mod(digit2,2)*10+digit3
        if digit3<=2 or digit3==3 and digit2==0:
            station_name="CS"+str(station_nr)
        else:
            station_name="RS"+str(station_nr)

    return station_name

def stationNameToID(station_name):
    """Returns the station id from a station name
    """

    digit1=int(station_name[2])
    digit2=int(station_name[3])
    digit3=int(station_name[4])

    if digit1==6:
        digit1=2
    elif digit1>1:
        digit2=(digit1-1)*2+digit2
        digit1=1

    station_id=100*digit1+10*digit2+digit3

    return station_id


def stationNameToNR(station_name):
    """Returns the station id from a station name
    """

    digit1=int(station_name[2])
    digit2=int(station_name[3])
    digit3=int(station_name[4])

    if digit1==6:
        digit1=2
    elif digit1>1:
        digit2=(digit1-1)*2+digit2
        digit1=1

    station_nr=str(digit1)+str(digit2)+str(digit3)

    return station_nr

def getRelativeAntennaPositions(station,antennaset,return_as_hArray=False):
    """Returns the antenna positions of all the antennas in the station
    relative to the station center for the specified antennaset.
    station can be the name or id of the station. Default returns as numpy
    array, option to return as hArray.

    Required arguments:

    =================== ==============================================
    Parameter           Description
    =================== ==============================================
    *station*           Name or id of the station. e.g. "CS302" or 142
    *antennaset*        Antennaset used for this station. Options:

                        * LBA_INNER
                        * LBA_OUTER
                        * LBA_X
                        * LBA_Y
                        * LBA_SPARSE0
                        * LBA_SPARSE1
                        * HBA_0
                        * HBA_1
                        * HBA

    =================== ==============================================

    Optional arguments:

    =================== ==============================================
    Parameter           Description
    =================== ==============================================
    *return_as_hArray*  Return as hArray.
    =================== ==============================================

    Examples::

        >>> metadata.getRelativeAntennaPositions(142,"LBA_INNER",True)
        hArray(float,[96, 3]) # len=288, slice=[0:288], vec -> [-0.0,0.0,-0.0,-0.0,0.0,-0.0,-0.0004,2.55,...]


        >>> getRelativeAntennaPositions("CS005","HBA",False)
        array([[  1.07150000e+01,   7.58900000e+00,   1.00000000e-03],
               [  1.07150000e+01,   7.58900000e+00,   1.00000000e-03],
                [  1.28090000e+01,   2.88400000e+00,   1.00000000e-03],....


        >>> antenna_ids=file["antennaIDs"]
        [1420000005,142000008,142000080]
        >>> station_id=antenna_ids[0]/1000000
        142
        >>> rcu_ids=np.mod(antenna_ids,1000)
        array([5, 8, 80])
        >>> all_antenna_pos=getRelativeAntennaPositions(station_id,"LBA_INNER",False)
        >>> used_antenna_pos=all_antenna_pos[rcu_ids]
        array([[  2.25000000e+00,   1.35000000e+00,  -1.00000000e-03],
               [  4.00000000e-04,  -2.55000000e+00,   1.00000000e-03],
               [  8.53000000e-01,   1.37240000e+01,  -3.00000000e-03]])

    """

    # Known antennasets
    names = ['LBA_INNER', 'LBA_OUTER', 'LBA_X', 'LBA_Y', 'LBA_SPARSE0', 'LBA_SPARSE1', 'HBA_0', 'HBA_1', 'HBA']

    # Check if requested antennaset is known
    assert antennaset in names

    # Check station id type
    if isinstance(station, int):
        # Convert a station id to a station name
        station=idToStationName(station)

    # Obtain filename of antenna positions
    LOFARSOFT=os.environ["LOFARSOFT"].rstrip('/')+'/'
    filename=LOFARSOFT+"data/lofar/StaticMetaData/AntennaArrays/"+station+"-AntennaArrays.conf"

    # Open file
    f = open(filename, 'r')

    if station[0:2] != "CS":
        if antennaset is "HBA_0" or antennaset is "HBA_1":
            antennaset = "HBA"

    # Find position of antennaset in file
    str_line = ''
    while antennaset != str_line.strip():
        str_line = f.readline()
        if len(str_line) == 0:
            #end of file reached, no data available
            assert False

    # Skip name and station reference position
    str_line = f.readline()
    str_line = f.readline()

    # Get number of antennas and the number of directions
    nrantennas = int(str_line.split()[0])
    nrdir = int(str_line.split()[4])

    antpos = []
    for i in range(nrantennas):
        line = f.readline().split()

        # Odd numbered antennas
        antpos.extend([float(line[0]),float(line[1]),float(line[2])])

        # Even numbered antennas
        antpos.extend([float(line[3]),float(line[4]),float(line[5])])

    # Return requested type
    if return_as_hArray:
        #import pycrtools as hf
        antpos=cr.hArray(antpos,dimensions=[2*int(nrantennas),int(nrdir)])
    else:
        antpos=np.asarray(antpos).reshape(2*int(nrantennas),int(nrdir))

    return antpos



def getAbsoluteAntennaPositions(station,antennaset,return_as_hArray=False):
    """Returns the antenna positions of all the antennas in the station
    relative to the station center for the specified antennaset.
    station can be the name or id of the station. Default returns as numpy
    array, option to return as hArray.

    Required arguments:

    =================== ==============================================
    Parameter           Description
    =================== ==============================================
    *station*           Name or id of the station. e.g. "CS302" or 142
    *antennaset*        Antennaset used for this station. Options:

                        * LBA_INNER
                        * LBA_OUTER
                        * LBA_X
                        * LBA_Y
                        * LBA_SPARSE0
                        * LBA_SPARSE1
                        * HBA_0
                        * HBA_1
                        * HBA

    =================== ==============================================

    Optional arguments:

    =================== ==============================================
    Parameter           Description
    =================== ==============================================
    *return_as_hArray*  Return as hArray.
    =================== ==============================================

    Examples::

        >>> metadata.getRelativeAntennaPositions(142,"LBA_INNER",True)
        hArray(float,[96, 3]) # len=288, slice=[0:288], vec -> [-0.0,0.0,-0.0,-0.0,0.0,-0.0,-0.0004,2.55,...]

        >>> getRelativeAntennaPositions("CS005","HBA",False)
        array([[  1.07150000e+01,   7.58900000e+00,   1.00000000e-03],
               [  1.07150000e+01,   7.58900000e+00,   1.00000000e-03],
                [  1.28090000e+01,   2.88400000e+00,   1.00000000e-03],....

        >>> antenna_ids=file["antennaIDs"]
        [1420000005,142000008,142000080]
        >>> station_id=antenna_ids[0]/1000000
        142
        >>> rcu_ids=np.mod(antenna_ids,1000)
        array([5, 8, 80])
        >>> all_antenna_pos=getRelativeAntennaPositions(station_id,"LBA_INNER",False)
        >>> used_antenna_pos=all_antenna_pos[rcu_ids]
        array([[  2.25000000e+00,   1.35000000e+00,  -1.00000000e-03],
               [  4.00000000e-04,  -2.55000000e+00,   1.00000000e-03],
               [  8.53000000e-01,   1.37240000e+01,  -3.00000000e-03]])

    """

    # Known antennasets
    names = ['LBA_INNER', 'LBA_OUTER', 'LBA_X', 'LBA_Y', 'LBA_SPARSE_EVEN', 'LBA_SPARSE_ODD', 'HBA_0', 'HBA_1', 'HBA']

    # Check if requested antennaset is known
    assert antennaset in names

    if "LBA" in antennaset:
        antennatype="LBA"
    elif "HBA" in antennaset:
        antennatype="HBA"


    # Check station id type
    if isinstance(station, int):
        # Convert a station id to a station name
        station=idToStationName(station)

    # Obtain filename of antenna positions
    LOFARSOFT=os.environ["LOFARSOFT"].rstrip('/')+'/'
    filename=LOFARSOFT+"data/lofar/StaticMetaData/AntennaFields/"+station+"-AntennaField.conf"

    # Open file
    f = open(filename, 'r')

    if station[0:2] != "CS":
        if antennaset is "HBA_0" or antennaset is "HBA_1":
            antennaset = "HBA"

    # Find position of antennaset in file
    str_line = ''
    while antennatype != str_line.strip():
        str_line = f.readline()
        if len(str_line) == 0:
            #end of file reached, no data available
            assert False

    # Skip name and station reference position
    str_line = f.readline()
    str_split=str_line.split()
    stationX=float(str_split[2])
    stationY=float(str_split[3])
    stationZ=float(str_split[4])

    str_line = f.readline()

    # Get number of antennas and the number of directions
    nrantennas = int(str_line.split()[0])
    nrdir = int(str_line.split()[4])

    antpos = []
    for i in range(nrantennas):
        line = f.readline().split()

        # Odd numbered antennas
        antpos.extend([float(line[0])+stationX,float(line[1])+stationY,float(line[2])+stationZ])

        # Even numbered antennas
        antpos.extend([float(line[3])+stationX,float(line[4])+stationY,float(line[5])+stationZ])


    # Make the appropriate selection
    antpos=np.asarray(antpos).reshape(2*int(nrantennas),int(nrdir))

    if antennatype == "LBA":
        # There are three types of feed
        # H for HBA
        # h for lbh
        # l for lbl
        feed={}
        feed["CS"]={}
        feed["RS"]={}
        feed["CS"]["LBA_SPARSE_EVEN"]="24llhh"
        feed["CS"]["LBA_SPARSE_ODD"]="24hhll"
        feed["CS"]["LBA_X"]="48hl"
        feed["CS"]["LBA_Y"]="48lh"
        feed["CS"]["LBA_INNER"]="96h"
        feed["CS"]["LBA_OUTER"]="96l"
        feed["RS"]["LBA_SPARSE_EVEN"]="24llhh"
        feed["RS"]["LBA_SPARSE_ODD"]="24hhll"
        feed["RS"]["LBA_X"]="48hl"
        feed["RS"]["LBA_Y"]="48lh"
        feed["RS"]["LBA_INNER"]="96h"
        feed["RS"]["LBA_OUTER"]="96l"
        if station[0:2] == "CS" or "RS":
            feedsel=feed[station[0:2]][antennaset]
            nrset = int(feedsel.split('l')[0].split('h')[0].split('H')[0])
            feeds=''
            feedsel=feedsel[len(str(nrset)):]
            for i in range(nrset):
                feeds+=feedsel

        indexselection=[]
        for i in range(len(feeds)):
            if feeds[i]=='l':
                # The 'l' feeds are the last 96 numbers of the total list
                indexselection.append(i+96)
            elif feeds[i]=='h':
                # The 'h' feeds are the first 96 numbers of the total list
                indexselection.append(i)
            else:
                # This selection is not yet supported
                assert False
        antpos=antpos[indexselection]



    # Return requested type
    if return_as_hArray:
        #import pycrtools as hf
        antpos=cr.hArray(antpos)
    #else:
    #    antpos=np.asarray(antpos).reshape(2*int(nrantennas),int(nrdir))

    return antpos



def getAntennaPositions(station,antennaset,return_as_hArray=False):
    """Returns the antenna positions of all the antennas in the station
    relative to the center of the CS002 LBA-field for the specified antennaset.
    station can be the name or id of the station. Default returns as numpy
    array, option to return as hArray.

    Required arguments:

    ================== ==============================================
    Parameter          Description
    ================== ==============================================
    *station*          Name or id of the station. e.g. "CS302" or 142
    *antennaset*       Antennaset used for this station. Options:

                       * LBA_INNER
                       * LBA_OUTER
                       * LBA_X
                       * LBA_Y
                       * LBA_SPARSE0
                       * LBA_SPARSE1
                       * HBA_0
                       * HBA_1
                       * HBA

    ================== ==============================================

    Optional arguments:

    ================== ==============================================
    Parameter          Description
    ================== ==============================================
    *return_as_hArray* Return as hArray.
    ================== ==============================================

    """

    itrfpos=getAbsoluteAntennaPositions(station,antennaset,return_as_hArray=True)
    returnpos=convertITRFToLocal(itrfpos,refpos=ITRFCS002,reflonlat=lonlatCS002)

    if not return_as_hArray:
        returnpos=returnpos.toNumpy()

    return returnpos


def getClockCorrection(station,antennaset="HBA",time=1278480000):
    """Get clock correction for superterp stations in seconds. Currently static values.

    *station* Station name or number for which to get the correction.
    *time* Optional. Linux time of observation. As clocks drift the value from the correct time should be given. Not yet implemented.
    """
    # Convert a station id to a station name
    if type(station)==type(1): # name is a station_id number
        station=idToStationName(station)

    clockcorrection={}
    clockcorrection["CS002"]=8.2724449975934222e-06
    clockcorrection["CS003"]=6.8756447631247787e-06
    clockcorrection["CS004"]=7.8462185662334179e-06
    clockcorrection["CS005"]=8.4978152695672203e-06
    clockcorrection["CS006"]=7.8374740231534724e-06
    clockcorrection["CS007"]=7.8673363857885218e-06

    if station in clockcorrection.keys():
        return clockcorrection[station]
    else:
        print "NO CLOCK CORRECTION VALUE AVAILABLE IN THE DATABASE"
        return 0

def getStationPositions(station,antennaset,return_as_hArray=False,coordinatesystem="WGS84",):
    """Returns the antenna positions of all the antennas in the station
    relative to the station center for the specified antennaset.
    station can be the name or id of the station. Default returns as numpy
    array, option to return as hArray.

    Required arguments:

    ================== ==============================================
    Argument           Description
    ================== ==============================================
    *station*          Name or id of the station. e.g. "CS302" or 142
    *antennaset*       Antennaset used for this station. Options:

                       * LBA_INNER
                       * LBA_OUTER
                       * LBA_X
                       * LBA_Y
                       * LBA_SPARSE0
                       * LBA_SPARSE1
                       * HBA_0
                       * HBA_1
                       * HBA

    *coordinatesystem* Currently only WGS84. in the future also ITRF
    ================== ==============================================

    Optional arguments

    ================== ==============================================
    Argument           Description
    ================== ==============================================
    *return_as_hArray* Return as hArray.
    ================== ==============================================

    Examples::

        >>> import pycr_metadata as md
        >>>
        >>> ant_pos=get_antenna_positions(142,"LBA_INNER",True)
        >>> ant_pos
        hArray(float,[96, 3]) # len=288, slice=[0:288], vec -> [-0.0,0.0,-0.0,-0.0,0.0,-0.0,-0.0004,2.55,...]

        >>> ant_pos=get_antenna_positions("CS005","HBA",False)
        >>> ant_pos
        array([[  1.07150000e+01,   7.58900000e+00,   1.00000000e-03],
               [  1.07150000e+01,   7.58900000e+00,   1.00000000e-03],
                [  1.28090000e+01,   2.88400000e+00,   1.00000000e-03],....

        >>> antenna_ids=file["antennaIDs"]
        [1420000005,142000008,142000080]
        >>> station_id=antenna_ids[0]/1000000
        142
        >>> rcu_ids=np.mod(antenna_ids,1000)
        array([5, 8, 80])
        >>> all_antenna_pos=get_antenna_positions(station_id,"LBA_INNER",False)
        >>> used_antenna_pos=all_antenna_pos[rcu_ids]
        array([[  2.25000000e+00,   1.35000000e+00,  -1.00000000e-03],
               [  4.00000000e-04,  -2.55000000e+00,   1.00000000e-03],
               [  8.53000000e-01,   1.37240000e+01,  -3.00000000e-03]])

    """

    # Known antennasets
    names = ['LBA_INNER', 'LBA_OUTER', 'LBA_X', 'LBA_Y', 'LBA_SPARSE0', 'LBA_SPARSE1', 'HBA_0', 'HBA_1', 'HBA']

    # Check if requested antennaset is known
    assert antennaset in names
    assert coordinatesystem in ["WGS84"]
    # Check station id type
    if isinstance(station, int):
        # Convert a station id to a station name
        station=idToStationName(station)

    # Obtain filename of antenna positions
    LOFARSOFT=os.environ["LOFARSOFT"].rstrip('/')+'/'
    filename=LOFARSOFT+"data/lofar/StaticMetaData/AntennaArrays/"+station+"-AntennaArrays.conf"

    # Open file
    f = open(filename, 'r')

    if station[0:2] != "CS":
        if antennaset is "HBA_0" or antennaset is "HBA_1":
            antennaset = "HBA"

    # Find position of antennaset in file
    str_line = ''
    while antennaset != str_line.strip():
        str_line = f.readline()
        if len(str_line) == 0:
            #end of file reached, no data available
            print "Antenna set not found in calibration file",filename
            return "None"

    # Skip name and station reference position
    str_line = f.readline()

    # Get number of antennas and the number of directions
    stationpos_str = str_line.split("[")[1].split("]")[0].split() # Get direction value between backets
    lon=float(stationpos_str[0])
    lat=float(stationpos_str[1])
    height=float(stationpos_str[2])

    stationpos = [lon, lat, height]
    # Return requested type
    if return_as_hArray:
        #import pycrtools as hf
        stationpos=cr.hArray(stationpos,3)
    else:
        stationpos=np.asarray(stationpos)

    return stationpos

def convertITRFToLocal(itrfpos,refpos=ITRFCS002,reflonlat=lonlatCS002):
    """.. todo:: Document :func:`~metadata.convertITRFToLocal`.
    """

    from numpy import sin
    from numpy import cos

    lon=reflonlat[0]
    lat=reflonlat[1]
    Arg0=cr.hArray([-sin(lon),-sin(lat)*cos(lon),cos(lat)*cos(lon)])
    Arg1=cr.hArray([cos(lon),-sin(lat)*sin(lon),cos(lat)*sin(lon)])
    Arg2=cr.hArray([0.0,cos(lat),sin(lat)])
    
    #Arg0=cr.hArray([-0.1195950000,  -0.7919540000,   0.5987530000]) 
    #Arg1=cr.hArray([0.9928230000,  -0.0954190000,   0.0720990000]) 
    #Arg2=cr.hArray([0.0000330000,   0.6030780000,   0.7976820000])


    itrfpos.sub(refpos)

    returnpos=cr.hArray(float,itrfpos.shape())

    returnpos[...].muladd(Arg0,itrfpos[...,0])
    returnpos[...].muladd(Arg1,itrfpos[...,1])
    returnpos[...].muladd(Arg2,itrfpos[...,2])

    return returnpos

