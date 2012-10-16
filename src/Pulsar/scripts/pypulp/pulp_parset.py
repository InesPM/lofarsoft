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
import math
from pulp_logging import PulpLogger

# Global function that calculates the distance between two points on sphere
def radial_distance(rarad1, decrad1, rarad2, decrad2):
	"""
	radial_distance: prog calculates radial distance between
	given (rarad1, decrad1) and (rarad2, decrad2) position (in radians). 
	It uses the formula used in ATNF pulsar catalogue to calculate distances 
	between the pulsars (Haversine formula).
	Return value: distance (in deg)
	"""
	dist = 2.*math.asin(math.sqrt((math.sin((decrad1-decrad2)/2.))**2 + math.cos(decrad1)*math.cos(decrad2)*(math.sin((rarad1-rarad2)/2.))**2))
	dist = (180. * dist) / math.pi
	return dist

# find all pulsars within max distance from selected center, sort them by flux
# and return 3 brightest ones
# coordinates are in radians
# max distance in degrees
def find_pulsars(rarad, decrad, cmdline, max_distance):
	radeg = (180. * rarad) / math.pi
	decdeg = (180. * decrad) / math.pi
	psrras=np.abs(cmdline.ras - radeg)
	psrdecs=np.abs(cmdline.decs - decdeg)
	# first, select only pulsars _roughly_ within required distance
	crit=(psrras<1.5*max_distance)&(psrdecs<1.5*max_distance)
	psrbs=cmdline.psrbs[crit]
	psrras=cmdline.ras[crit]
	psrdecs=cmdline.decs[crit]
	psrs400=cmdline.s400[crit]
	# for selected pulsars calculate the distance precisely
	psrdist=np.array([radial_distance(rarad, decrad, (psrras[ii]/180.)*math.pi, (psrdecs[ii]/180.)*math.pi) for ii in np.arange(np.size(psrras))])
	psrdist-=max_distance
	crit=(psrdist<0)
	psrs400=psrs400[crit]
	psrbs=psrbs[crit]
	psrs400=[float(re.sub(r'^\*$', '0.0', s)) for s in psrs400]
	# sort by flux in reverse order (getting the reversed list of indices)
	ind=[ii for ii in reversed(np.argsort(psrs400))]
	psrbs=psrbs[ind]
	if np.size(psrbs) > 3: psrbs=psrbs[:3]
	return psrbs

# Class SAPBeam or "Station" beam describes the parameters specific for this particular 
# station beam
class SAPBeam:
	# si - system info object
	# root - "parent" class of the sap beam
	def __init__(self, id, parset, si, cmdline, root, log=None):
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
		self.psrs = []             # up to 3 brightest pulsars in the SAP

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
			cmd="grep 'Observation.Beam\[%d\].tabRingSize' %s" % (self.sapid, parset)
			status=os.popen(cmd).readlines()
			if np.size(status)>0:
				# getting size of the TA ring
				try:
					self.ringSize=np.float64(status[0][:-1].split(" = ")[-1])
					self.ringSize = self.ringSize * (180./3.1415926)
				except: pass

		# find up to 3 brightest pulsars in the SAP
		if not cmdline.opts.is_nofold:
			if len(cmdline.psrs) == 0 or (len(cmdline.psrs) != 0 and cmdline.psrs[0] == "sapfind") or \
				(len(cmdline.psrs) != 0 and cmdline.psrs[0] == "sapfind3"):
				if root.antenna == "LBA": self.psrs = find_pulsars(self.rarad, self.decrad, cmdline, si.fov_lba)
				if root.antenna == "HBA": self.psrs = find_pulsars(self.rarad, self.decrad, cmdline, si.fov_hba)
				if len(cmdline.psrs) != 0 and cmdline.psrs[0] == "sapfind" and len(self.psrs)>0: self.psrs = self.psrs[:1]

		# initializing SAP beams objects and making the list of SAP beams
		for tid in np.arange(self.nrTiedArrayBeams):
			tab=TABeam(tid, parset, si, cmdline, root, self.sapid, self.nrSubbands, self.rarad, self.decrad, log)
			self.tabs.append(tab)



# Class TABeam describes the parameters specific for particular TA beam, which can be truly
# TA beam or beam from individual station in FE mode
class TABeam:
	def __init__(self, id, parset, si, cmdline, root, sapid, nrSubbands, sapRA, sapDEC, log=None):
		self.tabid = id            # ID number of the TA beam
		self.parent_sapid = sapid  # ID of the parent SAP beam

		self.rarad=self.decrad=0
		self.is_coherent=False     # coherent or incoherent beam
		self.DM=0                  # dispersion measure
		self.stationList=[]        # stations that form this beam, only used to FE now (?) to indicate one station
		self.specificationType=""  # "flyseye" for FE, "manual" (or "ring"?) for coherent 
		self.location=[]           # list of locus nodes with the data for this beam
		self.rawfiles={}           # dictionary that keeps info about all raw files
                                           # key - locus node, value - list of rawfiles on this node (with full path)
                                           # self.location - is just a list of keys
		self.assigned_files=[]     # list of assigned files from the parset file (for this particular beam)
					   # we use this to check its number against the actual number of present files
		self.nrSubbands = nrSubbands # duplicating number of subbands from parent SAP
		self.numfiles=0            # number of all files for this beam (sum of rawfiles for each node)

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
                        self.stationList = status[0][:-1].split(" = ")[-1].split("[")[1].split("]")[0].split(",")

		# Determining where the raw data are....
                # forming string with all locus nodes needed to check in one cexec command
		# here we are using only nodes that are alive
		if cmdline.opts.is_locate_rawdata: nodeslist=si.alive_nodes
		else: nodeslist=root.nodeslist
		if len(nodeslist) > 0:
                	cexeclocus=si.cexec_nodes[nodeslist[0]]
                	if len(nodeslist) > 1:
                        	for s in nodeslist[1:]:
                       	        	cexeclocus += ",%s" % (si.cexec_nodes[s].split(":")[1])
			cmd="%s %s 'ls -1 %s/%s/%s_SAP%03d_B%03d_S*_bf.raw' | grep -v such | grep -v match | grep -v xauth | grep -v connect | egrep -v \'\\*\\*\\*\\*\\*\'" % (si.cexeccmd, cexeclocus, si.rawdir, root.id, root.id, sapid, self.tabid)
                	cexec_output=[line[:-1] for line in os.popen(cmd).readlines()]
			for l in range(len(cexec_output)):
				if re.match("^-----", cexec_output[l]) is not None:
					loc=cexec_output[l].split(" ")[1]
				else:
					# first we are checking that this file (exactly the same with full path) DO NOT already present
					# in the current dictionary self.rawfiles. This is to deal with the case when rawdata are in /home area
					# that is visible from all locus nodes
					rawfile_exist=False
					for cloc in self.rawfiles:
						if cexec_output[l] in self.rawfiles[cloc]:
							rawfile_exist=True
							break
					# adding the file if it's not already added
					if not rawfile_exist:
						if loc in self.rawfiles: self.rawfiles[loc].append(cexec_output[l])
						else: self.rawfiles[loc]=[cexec_output[l]]	
			# list of all nodes
			self.location=self.rawfiles.keys()
			if len(self.location) == 0:
				msg="No data available for beam %d:%d" % (self.parent_sapid, self.tabid)
				if log != None: log.warning(msg)
				else: print msg

			# getting the total number of files available
			for loc in self.location:
				self.numfiles += len(self.rawfiles[loc])

		# Now getting the list of assigned files for this beam from the Parset file
        	cmd="grep %s_SAP%03d_B%03d %s | awk '{print $3}' - | tr -d '[]'" % (root.id, sapid, self.tabid, parset,)
		status=os.popen(cmd).readlines()
		if np.size(status)>0:
			self.assigned_files=[el for el in status[0][:-1].split(",") if re.search("%s_SAP%03d_B%03d" % (root.id, sapid, self.tabid), el)]
		# Now checking that the number of available files is the same as assigned number
		if self.numfiles != np.size(self.assigned_files):
			missing_files=self.assigned_files[:]
			# checking first what files are missing
			for loc in self.location:
				missing_files=list(set(missing_files)-set([s.split("/")[-1] for s in self.rawfiles[loc]]))
			if len(missing_files) == 0:
				msg="This is weird... There are should be at least one file missing..."	
				if log != None: log.warning(msg)
				else: print msg
			missing_nodes=list(set(root.assigned_nodeslist)-set(self.location))
			msg="Error: The number of available files (%d) is less than assigned (%d) for the beam %d:%d!" % (np.size(self.assigned_files), self.numfiles, sapid, self.tabid)
			if len(missing_files) > 0:
				msg += "\nFiles %s are missing" % (",".join(missing_files))
			if len(missing_nodes) > 0:
				msg += " on nodes %s" % (",".join(missing_nodes))
			if log != None: log.error(msg)
			else: print msg
			quit(1)


# Class Observation with info from the parset file
class Observation:
	# si - system info
	# log - logger class
	def __init__(self, id, si, cmdline, log=None):
		self.id = id
		self.parset = cmdline.opts.parset
		self.parset_dir = si.parset_dir

		self.project = "" # the name of the campaign
		self.projectPI = "" # PI of the project

		self.nrBeams = 0  # number of station beams
		self.saps=[]      # list of SAP beams (objects of SAPBeam class)

		self.startdate=self.starttime=""
                self.duration=0  # duration of obs in seconds

                self.antenna=self.antenna_config=self.band=""
		self.nstations=self.ncorestations=self.nremotestations=0
		self.stations=[]
		self.assigned_nodeslist=[]
		self.nodeslist=[]
		self.IM=self.IS=self.CS=self.CV=self.FE=self.OCD=False    # False until it's checked to be otherwise
		self.stokesIS = ""         # Stokes parameters for IS and CS
		self.stokesCS = ""

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
		if self.is_parset(): self.update(si, cmdline, log)
		else:
			msg="Can't find the parset file '%s' for ObsID = %s" % (self.parset, self.id)
			if log != None: log.error(msg)
			else: print msg
			quit(1)

	# return True if parset file was found, and False otherwise
	def is_parset (self):
		if self.parset != "": 
			if os.path.exists(self.parset):	return True
			else: return False
		else:   # checking old parset location
			self.parset = "%s/%s/%s.parset" % (self.parset_dir, self.id, self.id)
			if os.path.exists(self.parset):	return True
			else:   # checking new parset location
				self.parset = "%s/%s.parset" % (self.parset_dir, self.id)
				if os.path.exists(self.parset):	return True
				else: return False

	# parsing the string with ranges of subbands recorded to get list of subbands
	def getSubbands(self, sblist):
		subs=[]
		sbparts=sblist.split(",")
		for ss in sbparts:
			sedges=ss.split("..")
			if len(sedges) == 1: subs.append(int(sedges[0]))
			else: subs.extend(np.arange(int(sedges[0]), int(sedges[1])+1))
#		subs=np.unique(subs)
		subs.sort() # sorting with smallest being the first
		return subs

	# update info based on parset file
	def update (self, si, cmdline, log=None):
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
			self.startdate  = self.startdate.split(" ")[0]
		
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

		# Getting the name of the Campaign
        	cmd="grep 'Observation.Campaign.name' %s" % (self.parset,)
        	status=os.popen(cmd).readlines()
        	if np.size(status)>0:
                	# Campaign name setting exists in parset file
                	self.project=status[0][:-1].split(" = ")[-1]

		# Getting the name of the Campaign's PI
        	cmd="grep 'Observation.Campaign.PI' %s" % (self.parset,)
        	status=os.popen(cmd).readlines()
        	if np.size(status)>0:
                	# Campaign's PI name setting exists in parset file
                	self.projectPI=status[0][:-1].split(" = ")[-1].split("'")[1]

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
        	cmd="grep Output_Beamformed.locations %s | awk '{print $3}' - | tr -d '[]'" % (self.parset,)
		status=os.popen(cmd).readlines()
		if np.size(status)>0:
			self.assigned_nodeslist=status[0][:-1].split(",")
			self.assigned_nodeslist=[n.split(":")[0] for n in self.assigned_nodeslist]
			self.assigned_nodeslist=np.unique(self.asigned_nodeslist)
		# checking if all nodes in assigned_nodeslist are in alive
		if not cmdline.opts.is_locate_rawdata and len(self.assigned_nodeslist) > 0:
			nodes_unavail=list(set(self.assigned_nodeslist)-set(si.alive_nodes).intersection(set(self.assigned_nodeslist)))
			self.nodeslist=list(set(si.alive_nodes).intersection(set(self.assigned_nodeslist)))
			if len(nodes_unavail) > 0:
				msg="Warning! Some raw data are on nodes that are not available [%d]: %s" % (len(nodes_unavail), ", ".join(nodes_unavail))
				if log != None: log.warning(msg)
				else: print msg

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
                                        self.stokesCS=status[0][:-1].split(" = ")[-1]
                                        # in the transition phase there were some parset with just XY
                                        # this means just 2 files, one for X, another Y
                                        # now is always XXYY, i.e. 4 files get written
                                        if self.stokesCS == "XXYY" or self.stokesCS == "XY":
                                                self.CV = True
                                                self.CS = False

	        # check if data are incoherent stokes data
        	cmd="grep Output_IncoherentStokes.enabled %s" % (self.parset,)
        	status=os.popen(cmd).readlines()
        	if np.size(status) > 0:
                	# this info exists in parset file
                	if status[0][:-1].split(" = ")[-1].lower()[:1] == 't':
                        	self.IS = True
				cmd="grep OLAP.CNProc_IncoherentStokes.which %s" % (self.parset,)
				status=os.popen(cmd).readlines()
				if np.size(status) > 0:
					# getting Stokes string
					self.stokesIS=status[0][:-1].split(" = ")[-1]

		# at ~05.07.2012 the logic in the Parset files has changed, and the flag Output_CoherentStokes.enabled = True
		# ONLY for CS data and not for CV data.  For CV data one needs to check Output_Beamformed.enabled flag
		if self.IS == False and self.CS == False:
			# checking the Output_Beamformed flag
        		cmd="grep Output_Beamformed.enabled %s" % (self.parset,)
			status=os.popen(cmd).readlines()
	        	if np.size(status) > 0:
        	        	# this info exists in parset file
                		if status[0][:-1].split(" = ")[-1].lower()[:1] == 't':
                        		self.CV = True
					cmd="grep OLAP.CNProc_CoherentStokes.which %s" % (self.parset,)
					status=os.popen(cmd).readlines()
					if np.size(status) > 0:
						# getting Stokes string
						self.stokesCS=status[0][:-1].split(" = ")[-1]
						if self.stokesCS != "XXYY" and self.stokesCS != "XY":
							msg="Wrong Stokes setting '%s' for CV observation for ObsID = %s" % (self.stokesCS, self.id)
							if log != None: log.error(msg)
							else: print msg
							quit(1)

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
                if self.stokesCS == "":
                        cmd="grep OLAP.CNProc_CoherentStokes.which %s" % (self.parset,)
                        status=os.popen(cmd).readlines()
                        if np.size(status) > 0:
                                # getting Stokes string
                                self.stokesCS=status[0][:-1].split(" = ")[-1]
                if self.stokesIS == "":
                        cmd="grep OLAP.CNProc_IncoherentStokes.which %s" % (self.parset,)
                        status=os.popen(cmd).readlines()
                        if np.size(status) > 0:
                                # getting Stokes string
                                self.stokesIS=status[0][:-1].split(" = ")[-1]

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
			sap=SAPBeam(sid, self.parset, si, cmdline, self, log)
			self.saps.append(sap)

		# updating the nodeslist if we are looking up for raw data in all alive nodes rather than using the nodeslist from Parset
		if cmdline.opts.is_locate_rawdata:
			self.nodeslist=[]
			for sap in self.saps:
				for tab in sap.tabs:
					self.nodeslist.extend(tab.location)
			self.nodeslist=np.unique(self.nodeslist)


	# prints info about the observation
	def print_info(self, log=None):
		"""
		prints info about the observation
		"""
		if log != None:
			log.info("\n================================================================")
			log.info("ObsID: %s   Project: %s   PI: %s" % (self.id, self.project, self.projectPI))	
			log.info("Parset: %s" % (self.parset))
			log.info("Start UTC: %s %s  Duration: %s" % \
				(self.startdate, self.starttime, self.duration>=3600. and "%.1fh" % \
				(float(self.duration/3600.)) or "%.1fm" % (float(self.duration/60.))))
			mode=""
			if self.FE: 
				if not self.CS and not self.CV:
					mode+="FE (" + self.stokesCS + ") "
				else:
					mode+="FE "
			if self.IM: mode+="Im "
			if self.IS: mode+="IS (" + self.stokesIS + ") "
			if self.CS: mode+="CS (" + self.stokesCS + ") "
			if self.CV: mode+="CV "
			log.info("%s   Band: %s   Mode: %s   OCD: %s" % (self.antenna_config, self.band, mode, self.OCD and "yes" or "no"))
			log.info("#stations: %d [%dCS, %dRS]" % (self.nstations, self.ncorestations, self.nremotestations))
			log.info("Clock: %d MHz   Fc: %g MHz   BW: %g MHz" % (self.sampleClock, self.cfreq, self.bw))
			log.info("#subbands: %d [%s]   SubWidth: %g kHz" % (self.nrSubbands, self.subbandList, self.subbandWidth))
			if self.nrChanPerSubIS == self.nrChanPerSubCS or self.nrChanPerSubIS == 0 or self.nrChanPerSubCS == 0:
				nchanspersub = (self.nrChanPerSubIS != 0 and str(self.nrChanPerSubIS) or str(self.nrChanPerSubCS))
			elif self.IS and not self.CS and not self.CV:
				nchanspersub = str(self.nrChanPerSubIS)
			elif not self.IS and (self.CS or self.CV):
				nchanspersub = str(self.nrChanPerSubCS)
			else: nchanspersub = "%d (IS), %d (%s)" % (self.nrChanPerSubIS, self.nrChanPerSubCS, self.CS and "CS" or "CV")
			if self.downsample_factorIS == self.downsample_factorCS or self.downsample_factorIS == 0 or self.downsample_factorCS == 0:
				dfactor = (self.downsample_factorIS != 0 and str(self.downsample_factorIS) or str(self.downsample_factorCS))
			elif self.IS and not self.CS and not self.CV:
				dfactor = str(self.downsample_factorIS)
			elif not self.IS and (self.CS or self.CV):
				dfactor = str(self.downsample_factorCS)
			else: dfactor = "%d (IS), %d (%s)" % (self.downsample_factorIS, self.downsample_factorCS, self.CS and "CS" or "CV")
			if self.samplingIS == self.samplingCS or self.samplingIS == 0.0 or self.samplingCS == 0.0:
				sampling = (self.samplingIS != 0.0 and str(self.samplingIS) or str(self.samplingCS)) + " ms"
			elif self.IS and not self.CS and not self.CV:
				sampling = str(self.samplingIS) + " ms"
			elif not self.IS and (self.CS or self.CV):
				sampling = str(self.samplingCS) + " ms"
			else: sampling = "%g ms (IS), %g ms (%s)" % (self.samplingIS, self.samplingCS, self.CS and "CS" or "CV")
			log.info("#chans/sub: %s   Downsample Factor: %s" % (nchanspersub, dfactor))
			log.info("Sampling: %s" % (sampling))
			if self.nrBeams > 1:
				log.info("#SAPs: %d" % (self.nrBeams))
				for sap in self.saps:
					log.info("%d Target: %s   #TABs: %d%s" % (sap.sapid, sap.source, sap.nrTiedArrayBeams, \
						sap.nrRings > 0 and " #Rings: %d RingSize: %g deg" % (sap.nrRings, sap.ringSize) or ""))
			else:
				log.info("#SAPs: %d   Target: %s   #TABs: %d%s" % (self.nrBeams, self.saps[0].source, self.saps[0].nrTiedArrayBeams, \
					self.saps[0].nrRings > 0 and " #Rings: %d RingSize: %g deg" % (self.saps[0].nrRings, self.saps[0].ringSize) or ""))
			log.info("================================================================")
