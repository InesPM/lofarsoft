#
# $Id: pulpEnv.py 7119 2011-02-17 14:00:43Z kenneth $

#                                                          LOFAR PULSAR PIPELINE
#
#                                             Pulsar.pipeline.support.pulpEnv.py
#                                                          Ken Anderson, 2010-10
#                                                            k.r.anderson@uva.nl
# ------------------------------------------------------------------------------
# Operating environment for the pulsar pipeline, pulp.


import os

# Repository info ...
__svn_revision__ = ('$Rev: 7119 $').split()[1]
__svn_revdate__  = ('$Date: 2011-02-17 15:00:43 +0100 (Thu, 17 Feb 2011) $')[7:26]
__svn_author__   = ('$Author: kenneth $').split()[1]


class PulpEnv:

    """
    Builds an environment object for a given user and obseration.

    archPaths sets up a dict for pulp pipeline processing archive.
    Nominally called, PULP__ARCHIVE.

    Attributes available through the pulp environment class (PulpEnv)

    self.obsid        :: LOFAR observation ID, like L<YYYY>_<nnnnn>
    self.pulsar       :: Name of targeted pulsar
    self.arch         :: arch, 'arch134', etc.
    self.subnet       :: subnet in use
    self.environ      :: some user environ values, self.__dictify(uEnv)    
    self.LOFARSOFT    :: ${LOFARSOFT}
    self.TEMPO        :: ${TEMPO}
    self.PRESTO       :: ${PRESTO}
    self.oldParsetName:: legacy parset filename 'RTCP.parset.0'
    self.parsetPath   :: path to actual parset found.
    self.parsetName   :: name of actual parset found.
    self.transpose2   :: data through 2nd transpose, True or False.
    self.stokes       :: either 'incoherent' or ...
    self.archPaths    :: Possible Pulsar Processing Archives
    self.pArchive     :: User selected PULP PULSAR ARCHIVE
    self.oldLogName   :: old log type: 'run.Storage.R00.log' !! gone.
    self.logfilepath  :: full path of log file.
    self.obsidPath    :: full path to obsid output.
    self.stokesPath   :: full path,  <obsidPath>/[incoherent,raw]/

    'arch' will be a string of the form (see archPaths below)

    'arch134', 'arch184', 'arch154', etc. 

    """

    def __init__(self, obsid, pulsar, arch, uEnv):

        """Constuctor recieves args and builds an appropriate
        environment for the observation.  This environment object
        (an instance of PulpEnv), is instantiated by each recipe,
        and used as needed.  The framework enforces, to some degree,
        recipe independence, which is why each recipe builds its
        environment sans dependencies on other recipe parameters.

        This constructor also calls the private methods,
        .__testParset()
        .__readParset()
        
        which will check for the location of an observational parset
        and, once found, read the parameter file as needed.

        The "pulsar" is passed as a raw string for the command line.
        This string is resolved from a potential csv string of pulsar
        names.  The first pulsar is grabbed as the namer of the 

        Note:
        ---- 
        For now, archPaths is a path library only to those
	storage node paths on subclusters 5 and 6 (/net/sub5,
	/net/sub6). Add to and take from this dict as subclusters
	become available and not.

        """

        self.archPaths = {
            'arch134': '/net/sub5/lse013/data4/PULP_ARCHIVE',
            'arch144': '/net/sub5/lse014/data4/PULP_ARCHIVE',
            'arch154': '/net/sub5/lse015/data4/PULP_ARCHIVE',
            'arch164': '/net/sub6/lse016/data4/PULP_ARCHIVE',
            'arch174': '/net/sub6/lse017/data4/PULP_ARCHIVE',
            'arch184': '/net/sub6/lse018/data4/PULP_ARCHIVE',
            }

        self.obsid     = obsid
        self.pulsar    = pulsar.split(',')[0]  # 1st pulsar in csv string
        self.xPulsars  = pulsar.split(',')[1:] # list of extra pulsars
        self.arch      = arch
        self.pArchive  = self.archPaths[self.arch]
        self.subnet    = self.__getsubnet()
        self.nodes     = self.__getNodes()
        self.environ   = self.__dictify(uEnv)
        self.LOFARSOFT = self.environ["LOFARSOFT"]
        self.user      = os.environ["LOGNAME"]

        self.parsetPath    = '/globalhome/lofarsystem/log/'
        self.logLocation   = '/globalhome/lofarsystem/log/'
        self.oldParsetName = 'RTCP.parset.0'
        self.oldLogName    = 'run.Storage.R00.log'
        self.logfilepath   = os.path.join(self.logLocation,obsid,self.oldLogName)
        self.stokes        = 'incoherentstokes'  #  default stokes type
        self.defaultInf    = os.path.join(self.LOFARSOFT,
                                          'release/share/pulsar/data/lofar_default.inf'
                                          )

        # The following method calls will populate attributes,
        #
        #     self.transpose2
        #     self.stokes
        #
        # N.B. when 'coherentstokes' processing is commissioned for Pulp,
        # the pipeline parameter, self.stokes, should be, or rather, could be
        # used in the same way '$mode_str' is employed in the shell script.
        # self.stokes is currently a string, but could be refactored to be a list
        # or handled internally, as the pulsars list is managed.

        self.__testParset()
        self.__readParset()

        if self.transpose2:
            self.transpose2  = True
        else: self.tranpose2 = False

        # ------- envars for node recipe execution ---- #
        try:
            self.TEMPO  = self.environ["TEMPO"]
        except KeyError, err:
            raise KeyError, "$TEMPO not defined."
        try:
            self.PRESTO  = self.environ["PRESTO"]
        except KeyError, err:
            raise KeyError, "$PRESTO not defined"
        # ----------------------------------------------- #

        self.obsidPath  = os.path.join(self.pArchive,self.obsid)
        self.stokesPath = os.path.join(self.obsidPath,self.stokes)



        # ------------ formatted parameter set display ---------- #

    def show(self):

        """
        Display command line and other parameters, either read or constructed.

        """
        
        print "\n\n\tObservation & Pipeline parameters for OBSID:",self.obsid,"\n"
        print "---------------------"
        for key in self.__dict__.keys():
            print "Parameter: ",key, "\t= ",self.__dict__[key]
        print "---------------------"
        return



    # ------------- private helper methods ------------------ #

    def __getsubnet(self):
        subn = self.pArchive.split("/")[2]
        subnet = os.path.join('/net',subn)
        return subnet


    def __getNodes(self):

        """
        Determine search nodes from subnet attribute..
        
        eg.,

        /net/sub5 --> nodes = ['lse013','lse014','lse015']
        /net/sub6 --> nodes = ['lse016','lse017','lse018']
        
        """

        if self.subnet == "/net/sub5":
            nodes = ["lse013","lse014","lse015"]
        elif self.subnet == "/net/sub6":
            nodes = ["lse016","lse017","lse018"]
        else:
            raise RuntimeError, "Cannot resolve subnet descriptor."
        return nodes
    

    def __dictify(self, uEnv):
        
        """ Convert the uEnv string into a usable dictionary."""

        userEnviron = {}
        varparts = uEnv.split(':')
        for var in varparts:
            splitvar = var.split('=')
            userEnviron[splitvar[0].strip()]=splitvar[1].strip()
        return userEnviron
            

    def __testParset(self):
        """
        test for parset location type

        old location style

          /globalhome/lofarsystem/log/<obsid>/RTCP.parset.0
        
        new location style,

          /globalhome/lofarsystem/log/L<obsid>/L<obsid>.parset

        or possibly,

          /globalhome/lofarsystem/log/L<yyy-mm-dd>_<hhmmss>/RTCP-<obsid>.parset

        where hhmmss is the time of file write on day, <yyyy-mm-dd>

        self.__makePath() method builds a new "nominal" parset file name and path.
        This new form looks like,

        $LOG/L<obsid>/L<obsid>.parset

        where $LOG is  /globalhome/lofarsystem/log/ 

        """

        oldNominalParsetFile = os.path.join(self.parsetPath, self.obsid, self.oldParsetName)
        newNominalParsetFile = self.__makeParsetPath()

        if os.path.isfile(oldNominalParsetFile):
            print "Found old nominal parset file."
            self.parsetName = oldNominalParsetFile

        elif os.path.isfile(newNominalParsetFile):
            print "Found new nominal parset file."
            self.parsetName = newNominalParsetFile

        else:
            print "searching for alternate parset..."
            parsetTuple     = self.__findParsetFile()
            self.parsetName = os.path.join(parsetTuple[0],parsetTuple[1])
        return


    def __readParset(self):
        """

        Read a LOFAR observation parset file for the passed LOFAR obsid.

        This method has been adapted to read 'nominal', re: old, and 'new'
        parameter sets, the kind determined by testParset() and indicated
        by instance variable, self.parsetKind

        Parameters grabbed from  parsets:

        OLAP.outputIncoherentStokes   -- True or False 
        OLAP.BeamsAreTransposed       -- 2nd transpose, 'True' or 'False'

        TBD:
        ===

        -- parameters for new processing modes

        COHERENTSTOKES   'OLAP.outputCoherentStokes'
        2ND TRANSPOSE    'OLAP.BeamsAreTransposed'
        STOKES_TYPE      'OLAP.outputIncoherentStokes'
        FLYSEYE          'OLAP.PencilInfo.flysEye'
        NBEAMS           'OLAP.storageStationNames'

        """

        pars = open(self.parsetName).readlines()

        self.parTarget  = False
        self.transpose2 = False

        for line in pars:
            if ('OLAP.BeamsAreTransposed' in line):
                transpose2 = line.split()
                if len(transpose2) == 3:
                    if transpose2      =="True":
                        self.transpose2 = True
                    elif transpose2    =="False":
                        pass
                else:pass
                continue

            if ('OLAP.outputIncoherentStokes' in line):
                incoherentstokes = line.split('=')[1].strip()
                if incoherentstokes == 'True':
                    self.stokes  = 'incoherentstokes'
                else: pass

                if ('Observation.Beam[0].target' in line):
                    self.parTarget = True
                    target = line.split('=')[1].strip()
                    if target:
                        self.pulsar = target
                continue

            if self.transpose2 and self.stokes and self.parTarget:
                break
        return


    def __findParsetFile(self):
        """
        Hunt down the parset for a given obsid.
        Host site is

        /globalhome/lofarsystem/log/

        which is self.parsetPath

        Invocation of this method necessarily implies that "nominalparsetFile"
        did not exist, and this obsid has an "other form" parset file.

        """

        parsetFile   = None
        parsetLogDir = None
        searchPath   = self.parsetPath
        
        for root, dir, files in os.walk(searchPath,topdown=False):
            for file in files:
                if file == "RTCP-"+self.obsid.split("_")[1]+".parset":
                    parsetLogDir = root
                    parsetFile   = file
                    break
                continue
            if parsetFile and parsetLogDir:
                break
            else: continue
            
        return (parsetLogDir, parsetFile)

                          
    def __makeParsetPath(self):

        """
        In building a new nominal parset name, the obsid string

        L2010_<obsid> 

        must now be

        L<obsid>

        """
        
        oid          = "L"+self.obsid.split("_")[1]
        parsetLogDir = os.path.join(self.parsetPath,oid)
        parsetFile   = os.path.join(parsetLogDir,oid+".parset")
        return parsetFile

        

