###################################################################
#
# Class Observation description similar to obsinfo class used to gather
# info about observation
# (rewritten to handle only new format of parset files)
# Additional classes SAPBeam and TABeam are defined
#

import os, sys
import numpy as np
import time
import re
from pulp_logging import PulpLogger

# Class SAPBeam or "Station" beam describes the parameters specific for this particular 
# station beam
class SAPBeam:
	# si - system info object
	# root - "parent" class of the sap beam
	def __init__(self, id, parset, si, root):
		self.sapid = id            # ID number of the SAP beam

		self.rarad=self.decrad=0
		self.source=""

		self.nrTiedArrayBeams = 0  # number of TA beams including possible IS beam
		self.tabs=[]               # list of TA Beams (including possible IS beam)

		self.nrRings = 0           # number of TA rings (if used)
		self.ringSize = 0          # size of TA ring (in deg)
		self.nrSubbands = 0        # number of subbands (can be different for different SAPs)
		self.subbandList=""        # range of subbands, e.g. 77..320
		self.subbands=[]           # list of subbands

		# Getting the list of subbands
		cmd="grep 'Observation.Beam\[%d\].subbandList' %s" % (self.sapid, parset)
		status=os.popen(cmd).readlines()
		if np.size(status)>0:
			# getting range of subbands
			self.subbandList=status[0][:-1].split(" = ")[-1].split("[")[1].split("]")[0]
			# parsing the string with subband ranges to get list of subbands
			self.subbands = root.getSubbands(self.subbandList)
			# getting total number of Subbands
			self.nrSubbands = np.size(self.subbands)

	        # getting info about the pointing
        	cmd="grep 'Observation.Beam\[%d\].angle1' %s | grep -v AnaBeam" % (self.sapid, parset)
        	status=os.popen(cmd).readlines()
        	if np.size(status)>0:
                	# RA info exists in parset file
			try:
                		self.rarad=np.float64(status[0][:-1].split(" = ")[-1])
			except: pass

        	cmd="grep 'Observation.Beam\[%d\].angle2' %s | grep -v AnaBeam" % (self.sapid, parset)
        	status=os.popen(cmd).readlines()
        	if np.size(status)>0:
                	# DEC info exists in parset file
			try:
                		self.decrad=np.float64(status[0][:-1].split(" = ")[-1])
			except: pass

	        # getting info about Source name
        	cmd="grep 'Observation.Beam\[%d\].target' %s" % (self.sapid, parset)
        	status=os.popen(cmd).readlines()
        	if np.size(status)>0:
                	# Source name exists in parset file
                	self.source=status[0][:-1].split(" = ")[-1]
			if self.source != "":
				if self.source[0] == "'":
					self.source=self.source.split("'")[1]
				if self.source[0] == "\"":
					self.source=self.source.split("\"")[1]

		# Getting number of TA Beams in central station beam (Beam 0)
		# including possible IS beam
		cmd="grep 'Observation.Beam\[%d\].nrTiedArrayBeams' %s" % (self.sapid, parset)
		status=os.popen(cmd).readlines()
		if np.size(status)>0:
			# getting number of TA beams
			try:
				self.nrTiedArrayBeams=int(status[0][:-1].split(" = ")[-1])
			except: pass

		# Getting number of TA rings
		cmd="grep 'Observation.Beam\[%d\].nrTabRings' %s" % (self.sapid, parset)
		status=os.popen(cmd).readlines()
		if np.size(status)>0:
			# getting number of TA rings
			try:
				self.nrRings=int(status[0][:-1].split(" = ")[-1])
			except: pass

		# Getting the size of the TA ring (in deg)
		if self.nrRings != 0:
			cmd="grep 'Observation.Beam\[%d\].nrTabRingSize' %s" % (self.sapid, parset)
			status=os.popen(cmd).readlines()
			if np.size(status)>0:
				# getting size of the TA ring
				try:
					self.ringSize=np.float64(status[0][:-1].split(" = ")[-1])
					self.ringSize = self.ringSize * (180./3.1415926)
				except: pass

		# initializing SAP beams objects and making the list of SAP beams
		for tid in np.arange(self.nrTiedArrayBeams):
			tab=TABeam(tid, parset, si, root, self.sapid, self.nrSubbands, self.rarad, self.decrad)
			self.tabs.append(tab)



# Class TABeam describes the parameters specific for particular TA beam, which can be truly
# TA beam or beam from individual station in FE mode
class TABeam:
	def __init__(self, id, parset, si, root, sapid, nrSubbands, sapRA, sapDEC):
		self.tabid = id            # ID number of the TA beam
		self.parent_sapid = sapid  # ID of the parent SAP beam

		self.rarad=self.decrad=0
		self.is_coherent=False     # coherent or incoherent beam
		self.DM=0                  # dispersion measure
		self.stationList=[]        # stations that form this beam, only used to FE now (?) to indicate one station
		self.specificationType=""  # "flyseye" for FE, "manual" (or "ring"?) for coherent 
		self.location=[]           # list of locus nodes with the data for this beam
		self.nrSubbands = nrSubbands # duplicating number of subbands from parent SAP

	        # getting info about the pointing (offsets from the SAP center)
        	cmd="grep 'Observation.Beam\[%d\].TiedArrayBeam\[%d\].angle1' %s | grep -v AnaBeam" % (sapid, self.tabid, parset)
        	status=os.popen(cmd).readlines()
        	if np.size(status)>0:
                	# RA info exists in parset file
			try:
                		self.rarad=np.float64(status[0][:-1].split(" = ")[-1]) + sapRA
			except: pass

        	cmd="grep 'Observation.Beam\[%d\].TiedArrayBeam\[%d\].angle2' %s | grep -v AnaBeam" % (sapid, self.tabid, parset)
        	status=os.popen(cmd).readlines()
        	if np.size(status)>0:
                	# DEC info exists in parset file
			try:
                		self.decrad=np.float64(status[0][:-1].split(" = ")[-1]) + sapDEC
			except: pass

		# getting if beam is coherent or not
		cmd="grep 'Observation.Beam\[%d\].TiedArrayBeam\[%d\].coherent' %s | awk '{print $3}' - | grep 'True\|T\|true\|t\|1'" % (sapid, self.tabid, parset)
		status=os.popen(cmd).readlines()
		if np.size(status) > 0:
			self.is_coherent=True

	        # getting DM
        	cmd="grep 'Observation.Beam\[%d\].TiedArrayBeam\[%d\].dispersionMeasure' %s" % (sapid, self.tabid, parset)
        	status=os.popen(cmd).readlines()
        	if np.size(status)>0:
			try:
                		self.DM=np.float64(status[0][:-1].split(" = ")[-1])
			except: pass

	        # getting specification type of the beam
        	cmd="grep 'Observation.Beam\[%d\].TiedArrayBeam\[%d\].specificationType' %s" % (sapid, self.tabid, parset)
        	status=os.popen(cmd).readlines()
        	if np.size(status)>0:
	       		self.specificationType=status[0][:-1].split(" = ")[-1]

		# getting station list
        	cmd="grep 'Observation.Beam\[%d\].TiedArrayBeam\[%d\].stationList' %s" % (sapid, self.tabid, parset)
                status=os.popen(cmd).readlines()
                if np.size(status)>0:
                        stations=status[0][:-1].split(" = ")[-1].split("[")[1].split("]")[0]
                        # removing LBA and HBA from station names, replacing HBA ears HBA0 to /0 and HBA1 to /1
                        stations = re.sub("HBA0", "/0", stations)
                        stations = re.sub("HBA1", "/1", stations)
                        stations = re.sub("HBA", "", stations)
                        stations = re.sub("LBA", "", stations)
                        self.stationList = stations.split(",")

		# Determining where the raw data are....
                # forming string with all locus nodes needed to check in one cexec command
                cexeclocus=si.cexec_nodes[root.nodeslist[0]]
                if len(root.nodeslist) > 1:
                        for s in root.nodeslist[1:]:
                                cexeclocus += ",%s" % (si.cexec_nodes[s].split(":")[1])
                cmd="%s %s 'ls -1 /data/%s/%s_SAP%03d_B%03d_S*_bf.raw 2>/dev/null' 2>/dev/null | grep -v such | grep -v xauth | grep -v connect | egrep -v \'\\*\\*\\*\\*\\*\'" % (si.cexeccmd, cexeclocus, root.id, root.id, sapid, self.tabid)
                cexec_output=[line[:-1] for line in os.popen(cmd).readlines()]
                # finding all locus nodes that have the dir with raw data
                for l in np.arange(len(cexec_output)):
                        if re.match("^-----", cexec_output[l]) is None:
                                self.location.append(cexec_output[l-1].split(" ")[1])



# Class Observation with info from the parset file
class Observation:
	# si - system info
	# log - logger class
	def __init__(self, id, si, log=None, parset=""):
		self.id = id
		self.parset = parset
		self.parset_dir = si.parset_dir

		self.nrBeams = 0  # number of station beams
		self.saps=[]      # list of SAP beams (objects of SAPBeam class)

		self.startdate=self.starttime=""
                self.duration=0  # duration of obs in seconds

                self.antenna=self.antenna_config=self.band=self.stokes=""
		self.nstations=self.ncorestations=self.nremotestations=0
		self.stations=[]
		self.nodeslist=[]
		self.IM=self.IS=self.CS=self.CV=self.FE=self.OCD=False    # False until it's checked to be otherwise

		self.nrSubbands = 0        # number of subbands
		self.subbandList=""        # range of subbands, e.g. 77..320
		self.subbands=[]           # list of subbands
		self.subbandWidth = 0      # width of subband in kHz
		self.nrSubsPerFileIS = 0   # number of subbands per file
		self.nrSubsPerFileCS = 0    
		self.nrChanPerSubIS = 0    # number of channels per subband (for IS)
		self.nrChanPerSubCS = 0    # number of channels per subband (for CS)
		self.sampleClock = 0       # clock in MHz (200 or 160)
		self.downsample_factorIS = 0  # time integration (downsampling) factor, can be different for IS and CS 
		self.downsample_factorCS = 0  
		self.samplingIS = 0        # sampling interval in ms (depends on on integration steps, clock, number of channels)
		self.samplingCS = 0
		self.bw = 0                # bandwidth (in MHz) - recorded bandwidth
		self.freq_extent = 0       # freq separation between lowest and highest channels recorded (if there are gaps in frequency)
		self.cfreq = 0             # central freq (in MHz)

		# check if parset file exists and if so, then update the info
		if self.is_parset(): self.update(si)
		else:
			msg="Can't find the parset file '%s' for ObsID = %s" % (self.parset, self.id)
			if log != None: log.error(msg)
			else: print msg
			sys.exit(1)

		# checking if raw data is located on one of the down nodes
		# if so, then terminate the pipeline
		no_nodes = self.is_rawdata_available(si)
		if len(no_nodes) > 0:
			msg="Data are not available on these nodes: %s\nExiting." % (", ".join(no_nodes))
			if log != None: log.error(msg)
			else: print msg
			sys.exit(1)

	# return True if parset file was found, and False otherwise
	def is_parset (self):
		if self.parset == "": 
			self.parset = "%s/%s/%s.parset" % (self.parset_dir, self.id, self.id)
		if os.path.exists(self.parset):
			return True
		else:
			return False

	# checking if raw data is located on one of the down nodes	
	# returns list of unavailable nodes
	def is_rawdata_available(self, si):
		# first will check if nodes in nodeslist are all alive
		no_nodes=list(set(self.nodeslist)-set(self.nodeslist).intersection(set(si.alive_nodes)))
		# now checking if there are TABs that have raw data in several locations
		# so we would need to process them on hoover nodes
		# and we need to check if hoover nodes are up
		avail_hoover_nodes=list(set(si.hoover_nodes).intersection(set(si.alive_nodes)))
		# all hoover nodes are up, so no need to check each TAB
		if len(avail_hoover_nodes) == len(si.hoover_nodes): return no_nodes
		for sap in self.saps:
			for tab in sap.tabs:
				# only interested in beams with >1 locations
				if len(tab.location) > 1:
					# if beam is coherent and locus101 is unavailable
					if tab.is_coherent and "locus101" not in avail_hoover_nodes: no_nodes.append("locus101")
					if not tab.is_coherent and "locus102" not in avail_hoover_nodes: no_nodes.append("locus102")
		no_nodes=np.unique(no_nodes)
		return no_nodes
		

	# check if raw data are indeed exist in nodeslist from parset file
	# if not, we call rawdata_search to update them
	# alive_nodes - the complete list of alive locus nodes on CEP2
	# cexec_nodes - the full dictionary of all CEP2 nodes, key - is the node name (e.g. locus014)
	#    and value is how it's used with cexec (locus:13)
	# cexeccmd - exact cexec command to use (cexec -f /etc/c3.conf.full)
	def rawdata_check (self, alive_nodes, cexec_nodes, cexeccmd):
		# getting the sizes only from the intersection of oi.nodeslist and storage_nodes
		insecnodes=list(set(self.nodeslist).intersection(set(storage_nodes)))
		is_ok = True  # if False, we will update nodeslist
		if len(insecnodes) == 0: return
		# forming string with all locus nodes needed to check in one cexec command
		cexeclocus=cexec_nodes[insecnodes[0]] # there is always at least one insecnodes nodes (we'd do return above otherwise)
		if len(insecnodes) > 1:
			for s in insecnodes[1:]:
				cexeclocus += ",%s" % (cexec_nodes[s].split(":")[1])
		cmd="%s %s 'ls -d %s 2>/dev/null' 2>/dev/null | grep -v such | grep -v xauth | grep -v connect | egrep -v \'\\*\\*\\*\\*\\*\'" % (cexeccmd, cexeclocus, "/data/" + self.id)
		cexec_output=[line[:-1] for line in os.popen(cmd).readlines()]
		# see how many directories with raw data we have. Their number should be the same as number of insecnodes
		dirlines=[line for line in cexec_output if re.match("^-----", line) is None]
		if len(dirlines) < len(insecnodes): is_ok = False
		# updating nodeslist
		if not is_ok:
			self.rawdata_search(alive_nodes, cexec_nodes, cexeccmd)

	# search for raw data in all alive locus nodes
	def rawdata_search (self, alive_nodes, cexec_nodes, cexeccmd):
		self.nodeslist=[]
		# forming string with all locus nodes to check in one cexec command
		cexeclocus=cexec_nodes[alive_nodes[0]] # there is always at least one alive locus node
		if len(alive_nodes) > 1:
			for s in alive_nodes[1:]:
				cexeclocus += ",%s" % (cexec_nodes[s].split(":")[1])
		cmd="%s %s 'ls -d %s 2>/dev/null' 2>/dev/null | grep -v such | grep -v xauth | grep -v connect | egrep -v \'\\*\\*\\*\\*\\*\'" % (cexeccmd, cexeclocus, "/data/" + self.id)
		cexec_output=[line[:-1] for line in os.popen(cmd).readlines()]
		# finding all locus nodes that have the dir with raw data
		for l in np.arange(len(cexec_output)):
			if re.match("^-----", cexec_output[l]) is None: 
				self.nodeslist.append(cexec_output[l-1].split(" ")[1])

	# parsing the string with ranges of subbands recorded to get list of subbands
	def getSubbands(self, sblist):
		subs=[]
		sbparts=sblist.split(",")
		for ss in sbparts:
			sedges=ss.split("..")
			if len(sedges) == 1: subs.append(int(sedges))
			else: subs.extend(np.arange(int(sedges[0]), int(sedges[1])+1))
		subs=np.unique(subs)
		subs.sort() # sorting with smallest being the first
		return subs

	# update info based on parset file
	def update (self, si):
		# Getting the Date of observation
	        cmd="grep Observation.startTime %s | tr -d \\'" % (self.parset,)
        	status=os.popen(cmd).readlines()
        	if np.size(status) > 0:
                	# it means that this keyword exist and we can extract the info
                	self.startdate=status[0][:-1].split(" = ")[-1]
			self.starttime = self.startdate.split(" ")[1]

		# Getting the Duration
		cmd="grep Observation.stopTime %s | tr -d \\'" % (self.parset,)
		status=os.popen(cmd).readlines()
		if np.size(self.startdate) > 0 and np.size(status) > 0:
			# it means that both start and stop Times exist in parset file
			stoptime=status[0][:-1].split(" = ")[-1]
			c1 = time.strptime(self.startdate, "%Y-%m-%d %H:%M:%S")
			c2 = time.strptime(stoptime, "%Y-%m-%d %H:%M:%S")
			self.duration=time.mktime(c2)-time.mktime(c1)  # difference in seconds
		
		# Getting the Antenna info (HBA or LBA)
        	cmd="grep 'Observation.bandFilter' %s" % (self.parset,)
        	status=os.popen(cmd).readlines()
        	if np.size(status)>0:
                	# Antenna array setting exists in parset file
                	self.antenna=status[0][:-1].split(" = ")[-1].split("_")[0]

		# Getting the Antenna config (LBA_OUTER, LBA_INNER, HBA_JOINED, etc.)
        	cmd="grep 'Observation.antennaSet' %s" % (self.parset,)
        	status=os.popen(cmd).readlines()
        	if np.size(status)>0:
                	# Antenna Set setting exists in parset file
                	self.antenna_config=status[0][:-1].split(" = ")[-1]

		# Getting the Filter setting
        	cmd="grep 'Observation.bandFilter' %s" % (self.parset,)
        	status=os.popen(cmd).readlines()
        	if np.size(status)>0:
                	# band filter setting exists in parset file
                	self.band=status[0][:-1].split(" = ")[-1].split("A_")[-1]

		# Getting the stations and their number (including separately the number of CS and RS)
        	cmd="grep 'OLAP.storageStationNames' %s" % (self.parset,)
        	status=os.popen(cmd).readlines()
        	if np.size(status)>0:
                	# Stations setting exists in parset file
                	stations=status[0][:-1].split(" = ")[-1].split("[")[1].split("]")[0]
			# removing LBA and HBA from station names, replacing HBA ears HBA0 to /0 and HBA1 to /1
			stations = re.sub("HBA0", "/0", stations)
			stations = re.sub("HBA1", "/1", stations)
			stations = re.sub("HBA", "", stations)
			stations = re.sub("LBA", "", stations)
			self.stations = stations.split(",")
			self.nstations = np.size(self.stations)
			self.ncorestations = stations.count("CS")
			self.nremotestations = stations.count("RS")

		# checking "locations" keywords first as in the new parset files (as from Jan 27, 2012), 
		# "mountpoints" can give wrong values
		self.nodeslist_string = "[]"
        	cmd="grep Output_Beamformed.locations %s | awk '{print $3}' - | tr -d '[]'" % (self.parset,)
		status=os.popen(cmd).readlines()
		if np.size(status)>0:
			self.nodeslist=status[0][:-1].split(",")
			self.nodeslist=[n.split(":")[0] for n in self.nodeslist]

	        # check if online coherent dedispersion (OCD) was used
        	cmd="grep OLAP.coherentDedisperseChannels %s" % (self.parset,)
        	status=os.popen(cmd).readlines()
        	if np.size(status) > 0:
                	# this info exists in parset file
                	if status[0][:-1].split(" = ")[-1].lower()[:1] == 't': self.OCD=True

                # getting info about the Type of the data (CV, CS, IS, FE, Imaging, etc.)
                cmd="grep Output_CoherentStokes.enabled %s" % (self.parset,)
                status=os.popen(cmd).readlines()
                if np.size(status) > 0:
                        # this info exists in parset file
                        if status[0][:-1].split(" = ")[-1].lower()[:1] == 't':
                                self.CS = True
                                self.CV = False
                                cmd="grep OLAP.CNProc_CoherentStokes.which %s" % (self.parset,)
                                status=os.popen(cmd).readlines()
                                if np.size(status) > 0:
                                        # getting Stokes string
                                        self.stokes=status[0][:-1].split(" = ")[-1]
                                        # in the transition phase there were some parset with just XY
                                        # this means just 2 files, one for X, another Y
                                        # now is always XXYY, i.e. 4 files get written
                                        if self.stokes == "XXYY" or self.stokes == "XY":
                                                self.CV = True
                                                self.CS = False

	        # check if data are incoherent stokes data
        	cmd="grep Output_IncoherentStokes.enabled %s" % (self.parset,)
        	status=os.popen(cmd).readlines()
        	if np.size(status) > 0:
                	# this info exists in parset file
                	if status[0][:-1].split(" = ")[-1].lower()[:1] == 't':
                        	self.IS = True
				if self.stokes == "":
					cmd="grep OLAP.CNProc_IncoherentStokes.which %s" % (self.parset,)
					status=os.popen(cmd).readlines()
					if np.size(status) > 0:
						# getting Stokes string
						self.stokes=status[0][:-1].split(" = ")[-1]

	        # check if data are imaging
        	cmd="grep Output_Correlated.enabled %s" % (self.parset,)
        	status=os.popen(cmd).readlines()
        	if np.size(status) > 0:
                	# this info exists in parset file
                	if status[0][:-1].split(" = ")[-1].lower()[:1] == 't': self.IM = True


	        # check if data are fly's eye mode data
        	cmd="grep PencilInfo.flysEye %s" % (self.parset,)
        	status=os.popen(cmd).readlines()
        	if np.size(status) > 0:
                	# this info exists in parset file
                	if status[0][:-1].split(" = ")[-1].lower()[:1] == 't': self.FE = True

                # if Stokes are still undetermined (e.g. if obs is IM), then
                # rereading default stokes for CS
                if self.stokes == "":
                        cmd="grep OLAP.CNProc_CoherentStokes.which %s" % (self.parset,)
                        status=os.popen(cmd).readlines()
                        if np.size(status) > 0:
                                # getting Stokes string
                                self.stokes=status[0][:-1].split(" = ")[-1]

		# Getting the list of subbands
		cmd="grep Observation.subbandList %s" % (self.parset,)
		status=os.popen(cmd).readlines()
		if np.size(status)>0:
			# getting range of subbands
			self.subbandList=status[0][:-1].split(" = ")[-1].split("[")[1].split("]")[0]
			# parsing the string with subband ranges to get list of subbands
			self.subbands = self.getSubbands(self.subbandList)
			# getting total number of Subbands
			self.nrSubbands = np.size(self.subbands)

		# in new parset files (after Jan 27, 2012) there are new keywords for number of
		# chans per subband and this number can be different for IS and CS
		cmd="grep OLAP.CNProc_IncoherentStokes.channelsPerSubband %s" % (self.parset,)
		status=os.popen(cmd).readlines()
		if np.size(status)>0:
			# getting number of channels
			try:
				self.nrChanPerSubIS=int(status[0][:-1].split(" = ")[-1])
			except: pass

		cmd="grep OLAP.CNProc_CoherentStokes.channelsPerSubband %s" % (self.parset,)
		status=os.popen(cmd).readlines()
		if np.size(status)>0:
			# getting number of channels
			try:
				self.nrChanPerSubCS=int(status[0][:-1].split(" = ")[-1])
			except: pass

		# Getting the sample clock
		cmd="grep Observation.sampleClock %s" % (self.parset,)
		status=os.popen(cmd).readlines()
		if np.size(status)>0:
			# getting the clock
			try:
				self.sampleClock=int(status[0][:-1].split(" = ")[-1])
			except: pass
		if self.sampleClock == 0: # if keyword 'Observation.sampleClock' is missing in the parset file
			cmd="grep Observation.clockMode %s" % (self.parset,)
			status=os.popen(cmd).readlines()
			if np.size(status)>0:
				# getting the clock
				try:
					self.sampleClock=int(status[0][:-1].split(" = ")[-1].split("Clock")[1])
				except: pass

		# Getting width of the subband (in kHz)
		cmd="grep Observation.subbandWidth %s" % (self.parset,)
		status=os.popen(cmd).readlines()
		if np.size(status)>0:
			# getting the width of the subband
			try:
				self.subbandWidth=float(status[0][:-1].split(" = ")[-1])
			except: pass
		if self.subbandWidth == 0 and self.sampleClock != 0:
			self.subbandWidth = ( ( self.sampleClock / 2. ) / 512. ) * 1000.
		
		# getting timeIntegrationFactors for IS and CS and calculating sampling intervals
		cmd="grep OLAP.CNProc_IncoherentStokes.timeIntegrationFactor %s" % (self.parset,)
		status=os.popen(cmd).readlines()
		if np.size(status)>0:
			# getting number of channels
			try:
				self.downsample_factorIS=int(status[0][:-1].split(" = ")[-1])
				if self.downsample_factorIS != 0 and self.sampleClock != 0 and self.nrChanPerSubIS != 0:
					self.samplingIS = self.downsample_factorIS / ((self.sampleClock * 1000. * 1000. / 1024.) / self.nrChanPerSubIS) * 1000.
			except: pass

		cmd="grep OLAP.CNProc_CoherentStokes.timeIntegrationFactor %s" % (self.parset,)
		status=os.popen(cmd).readlines()
		if np.size(status)>0:
			# getting number of channels
			try:
				self.downsample_factorCS=int(status[0][:-1].split(" = ")[-1])
				if self.downsample_factorCS != 0 and self.sampleClock != 0 and self.nrChanPerSubCS != 0:
					self.samplingCS = self.downsample_factorCS / ((self.sampleClock * 1000. * 1000. / 1024.) / self.nrChanPerSubCS) * 1000.
			except: pass

		# Calculating the total BW (in MHz)
		if self.nrSubbands != 0 and self.subbandWidth != 0:
			self.bw = self.subbandWidth * self.nrSubbands / 1000.

		# Calculating the central freq (in MHz)
		if self.band != "":
			try:
				lower_band_freq = int(self.band.split("_")[0])
				if self.sampleClock == 200:
					if lower_band_freq > 200:
						lower_band_edge = 200.0
					elif lower_band_freq < 200 and lower_band_freq > 100:
						lower_band_edge = 100.0
					else: lower_band_edge = 0.0
				if self.sampleClock == 160:
					if lower_band_freq >= 160:
						lower_band_edge = 160
					elif lower_band_freq < 160 and lower_band_freq >= 80:
						lower_band_edge = 80
					else: lower_band_edge = 0

				if np.size(self.subbands) > 0 and self.subbandWidth != 0 and (self.nrChanPerSubIS != 0 or self.nrChanPerSubCS != 0):
					try:
						# CS has a priority
						if self.nrChanPerSubCS != 0: nchanpersub = self.nrChanPerSubCS
						else: nchanpersub = self.nrChanPerSubIS
						if nchanpersub > 1: # it means that we ran 2nd polyphase
							lofreq = lower_band_edge + (self.subbandWidth / 1000.) * self.subbands[0] - 0.5 * (self.subbandWidth / 1000.) - 0.5 * (self.subbandWidth / 1000. / nchanpersub)
							hifreq = lower_band_edge + (self.subbandWidth / 1000.) * (self.subbands[-1] + 1) - 0.5 * (self.subbandWidth / 1000.) - 0.5 * (self.subbandWidth / 1000. / nchanpersub)
						else:
							lofreq = lower_band_edge + (self.subbandWidth / 1000.) * self.subbands[0] - 0.5 * (self.subbandWidth / 1000.)
							hifreq = lower_band_edge + (self.subbandWidth / 1000.) * (self.subbands[-1] + 1) - 0.5 * (self.subbandWidth / 1000.)
						self.freq_extent = hifreq - lofreq
						self.cfreq = lofreq + self.freq_extent/2.
					except: pass
			except: pass

		# Getting number of Station beams
		cmd="grep Observation.nrBeams %s" % (self.parset,)
		status=os.popen(cmd).readlines()
		if np.size(status)>0:
			# getting number of station beams
			try:
				self.nrBeams=int(status[0][:-1].split(" = ")[-1])
			except: pass

		# initializing SAP beams objects and making the list of SAP beams
		for sid in np.arange(self.nrBeams):
			sap=SAPBeam(sid, self.parset, si, self)
			self.saps.append(sap)
		