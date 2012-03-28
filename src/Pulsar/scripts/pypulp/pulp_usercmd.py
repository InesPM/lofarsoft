#
# Class CMDLine collects info about user input from the command line
#
import optparse as opt
import os, sys
import numpy as np
from pulp_sysinfo import CEP2Info
from pulp_logging import PulpLogger
import logging
import time
import re

# checks if given pulsar is good ones, i.e. either in ATNF catalog
# or par-file do exis
# return value is True (psr is good), or False (psr is bad)
def check_pulsars(psr, cmdline, cep2, log=None):
	if psr not in cmdline.psrbs and psr not in cmdline.psrjs:
		msg="warning: Pulsar %s is not in the catalog: '%s'! Checking for par-file..." % (psr, cep2.psrcatalog)
		if log != None: log.warning(msg)
		# checking if par-file exist
		if cmdline.opts.parfile != "":
			if os.path.exists(cmdline.opts.parfile): 
				msg="Found parfile '%s'. Continue..." % (parfile)
				if log != None: log.info(msg)
				return True
			else: 
				msg="Can't find user parfile '%s'. Exiting..." % (cmdline.opts.parfile)
				if log != None: log.error(msg)
				else: print msg
				quit(1)
		parfile="%s/%s.par" % (cep2.parfile_dir, re.sub(r'[BJ]', '', psr))
		if not os.path.exists(parfile):
			# checking another parfile name
			parfile="%s/%s.par" % (cep2.parfile_dir, psr)
			if not os.path.exists(parfile): 
				return False
		msg="Found parfile '%s'. Continue..." % (parfile)
		if log != None: log.info(msg)
	return True

class CMDLine:
	# parsing a command line
        def __init__(self, version=""):
		self.prg = sys.argv[0]
		self.options = sys.argv[1:]  # storing original cmd line
		self.version = version
		self.psrs = []  # list of pulsars to fold
		self.beams = [] # list of beams to process
		self.user_beams = [] # list of User beams to process
		self.user_excluded_beams = [] # list of User excluded beams
		self.psrbs = self.psrjs = [] # list of B and J names of pulsars from ATNF catalog
		self.ras = self.decs = self.s400 = [] # lists of RA, DEC, and S400 of catalog pulsars
        	self.usage = "Usage: %prog <--id ObsID> [-h|--help] [OPTIONS]"
        	self.cmd = opt.OptionParser(self.usage, version="%prog " + self.version)
        	self.cmd.add_option('--id', '--obsid', dest='obsid', metavar='ObsID',
                           help="specify the Observation ID (i.e. L30251). This option is required", default="", type='str')
        	self.cmd.add_option('-p', '-P', '--pulsar', dest='psr', metavar='PSRS|word',
                           help="specify the Pulsar Name or comma-separated list of Pulsars for folding (w/o spaces) or \
                                 give one of the 4 special words: \"parset\" - to take pulsar name from the source field for each SAP \
                                 separately from the parset file, or \"sapfind\", \"sapfind3\" to find the best (3 best) pulsars in FOV \
                                 of the particular SAP, or \"tabfind\" to find the brightest pulsar for each TAB individually. \
                                 If no pulsars are given and no special words used, then pipeline will try to take source names from \
                                 parset file first, and then look for the best pulsars in SAP's FOV (same as \"sapfind\"). \
                                 Word 'NONE' as a pulsar name is ignored", default="", type='str')
        	self.cmd.add_option('-o', '-O', '--output', dest='outdir', metavar='DIR',
                           help="specify the Output Processing Location relative to /data/LOFAR_PULSAR_ARCHIVE_locus*. \
                                 Default is corresponding *_red or *_redIS directory", default="", type='str')
        	self.cmd.add_option('--par', '--parfile', '--eph', dest='parfile', metavar='PARFILE',
                           help="specify the parfile for one pulsar to fold. Pulsar name should be given explicitely using --pulsar option \
                                 and only one pulsar name should be given for --par option to work", default="", type='str')
        	self.cmd.add_option('--nodecode', action="store_true", dest='is_nodecode',
                           help="optional parameter to skip decoding the data (2bf2fits/bf2puma2)", default=False)
        	self.cmd.add_option('--nofold', action="store_true", dest='is_nofold',
                           help="optional parameter to turn off folding of data (prepfold is not run)", default=False)
        	self.cmd.add_option('--summary', action="store_true", dest='is_summary', 
                           help="making only summaries on already processed data", default=False)
        	self.cmd.add_option('--plots-only', action="store_true", dest='is_plots_only', 
                           help="creating diagnostic plots only on processing nodes assuming the data required for plots is there already", default=False)
        	self.cmd.add_option('--beams', dest='beam_str', metavar='[^]SAP#:TAB#[,SAP#:TAB#,...]',
                           help="user-specified beams to process separated by commas and written as station beam number, colon, \
                                 TA beam number, with no spaces. The argument can have leading hat character '^' to indicate that \
                                 specified beams are to be excluded from processing", default="", type='str')
        	self.cmd.add_option('--noIS', action="store_true", dest='is_noIS',
                           help="optional parameter to turn off processing of Incoherent sum (IS) data", default=False)
        	self.cmd.add_option('--noCS', action="store_true", dest='is_noCS',
                           help="optional parameter to turn off processing of Coherent sum (CS) data", default=False)
        	self.cmd.add_option('--noCV', action="store_true", dest='is_noCV',
                           help="optional parameter to turn off processing of Complex voltages (CV) data", default=False)
        	self.cmd.add_option('--noFE', action="store_true", dest='is_noFE',
                           help="optional parameter to turn off processing of Fly's Eye (FE) data", default=False)
        	self.cmd.add_option('--del', '--delete', action="store_true", dest='is_delete',
                           help="optional parameter to delete the previous entire output processing location if it exists. \
                                 Otherwise, the new results will be overwritten/added to existing directory", default=False)
        	self.cmd.add_option('--parset', dest='parset', metavar='FILE',
                           help="specify explicitely the input parameter file (parset file). By default, it will be looked for \
                                 in standard system directory", default="", type='str')
        	self.cmd.add_option('--raw', dest='rawdir', metavar='RAWDIR',
                           help="specify the location of input raw data. Directory structure is assumed as RAWDIR/<ObsID>.", default="/data", type='str')
        	self.cmd.add_option('--locate-rawdata', action="store_true", dest='is_locate_rawdata',
                           help="search for input raw data in all alive nodes instead of using the list of nodes from the parset file", default=False)
        	self.cmd.add_option('--log-append', action="store_true", dest='is_log_append',
                           help="optional parameter to append log output to already existent log files. Default is overwrite", default=False)
        	self.cmd.add_option('--nthreads', dest='nthreads', metavar='#THREADS',
                           help="number of threads for all dspsr calls. Default: %default", default=2, type='int')
        	self.cmd.add_option('--debug', action="store_true", dest='is_debug',
                           help="optional for testing: turns on debug level logging in Python", default=False)
        	self.cmd.add_option('-q', '--quiet', action="store_true", dest='is_quiet',
                           help="optional parameter to turn off user's warnings and waiting time of 10 seconds in the beginning", default=False)
        	self.cmd.add_option('--noinit', action="store_true", dest='is_noinit',
                           help="do not check for down nodes and available input raw data. Observation config is read from saved file \
                                 rather then is initialized using parset file (mostly for _internal_ use only)", default=False)
        	self.cmd.add_option('--local', action="store_true", dest='is_local', 
                           help="to process the data locally on current locus node for one beam only. Should only be used together \
                                 with --beams option and only the first beam will be used if there are several specified in --beams \
                                 (mostly for _internal_ use only)", default=False)
		# adding CS/IS/FE extra options
	        self.groupCS = opt.OptionGroup(self.cmd, "CS/IS/FE extra options")
        	self.groupCS.add_option('--norfi', action="store_true", dest='is_norfi',
                           help="optional parameter to skip rfifind and subdyn.py RFI checker", default=False)
        	self.groupCS.add_option('--nopdmp', action="store_true", dest='is_nopdmp',
                           help="turn off running pdmp in the pipeline", default=False)
        	self.groupCS.add_option('--skip-dspsr', action="store_true", dest='is_skip_dspsr',
                           help="optional parameter to turn off running dspsr part of the pipeline (including pdmp and creation of corresponding plots)", default=False)
		self.cmd.add_option_group(self.groupCS)
		# adding CV extra options
	        self.groupCV = opt.OptionGroup(self.cmd, "Complex voltage (CV) extra options")
        	self.groupCV.add_option('--hist-cutoff', dest='hist_cutoff', metavar='FRACTION',
                           help="clip FRACTION off the edges of the samples histogram. Be noted, it eliminates spiky RFI, but may also \
                                 clip bright pulsar pulses. Default: %default (no clipping)", default=0.02, type='float')
        	self.groupCV.add_option('--nblocks', dest='nblocks', metavar='#BLOCKS',
                           help="only read the first #BLOCKS blocks. Default: all blocks", default=-1, type='int')
        	self.groupCV.add_option('--all-for-scaling', action="store_true", dest='is_all_times',
                           help="normalize the data based on entire data set. Otherwise, the scaling is updated after every data block", default=False)
        	self.groupCV.add_option('--write-ascii', action="store_true", dest='is_write_ascii',
                           help="write out also ascii files (.rv) containing complex values", default=False)
        	self.groupCV.add_option('--tsubint', dest='tsubint', metavar='SECS',
                           help="set the length of each subintegration to SECS. Default is %default secs", default=5, type='int')
        	self.groupCV.add_option('--output-chans-per-subband', dest='output_chans_per_subband', metavar='#CHANS',
                           help="set output number of channels per subband. Default: %default (all channels in subband are collapsed)", default=1, type='int')
        	self.groupCV.add_option('--skip-rmfit', action="store_true", dest='is_skip_rmfit',
                           help="skip running rmfit program", default=False)
		self.cmd.add_option_group(self.groupCV)
        
		# reading cmd options
		(self.opts, self.args) = self.cmd.parse_args()

		# check if any input parameters are given
		if len(sys.argv[1:]) == 0:
			self.cmd.print_usage()
			quit(0)

	# prints countdown (only to terminal)
	def press_controlc(self, fr, cur):
		msg="\bPress Control-C to stop in: %s" % (" ".join(str(i) for i in range(fr, cur-1, -1)))
		print "\b" * int("%d" % (29 + 3*(fr-cur))), msg,
		sys.stdout.flush()

	# waiting for user to make a decision about "Stop or continue"
	def waiting_for_user(self, secs_to_wait, log=None):
		msg="Waiting %d seconds before starting..." % (secs_to_wait)
		if log != None: log.info(msg)
		else: print msg
		for sec in range(secs_to_wait, 0, -1):
			self.press_controlc(secs_to_wait, sec)
			time.sleep(1)
		if log != None: log.info("")
		else: print ""
		

	# checks if given arguments are OK and not mutually exclusive, etc.
	def check_options(self, cep2, log=None):

		# changing the logging level for debug if option is given
		if log != None and self.opts.is_debug:
			log.set_loglevel(logging.DEBUG)

		# checking if --local is used without specified beam
		if self.opts.is_local and self.opts.beam_str == "":
			msg="You have to use --beams option with --local!"
			if log != None: log.error(msg)
			else: print msg
			quit(1)

		# check if all required options are given
		if self.opts.obsid == "":
			msg="ObsID is not given. What do you want to process?"
			if log != None: log.error(msg)
			else: print msg
			quit(1)

		# checking that if --beams used then beams are specified correctly
		# we have this complicated "if" because we used --beams to pass summary locus node when --summary and --local
		if self.opts.beam_str != "" and (not self.opts.is_summary or (self.opts.is_summary and not self.opts.is_local)):
			# checking first if our list of beams is actually the list of excluded beams
			if self.opts.beam_str[0] == '^':
				is_excluded = True
				self.opts.beam_str = self.opts.beam_str[1:]
				if self.opts.beam_str == "":
					msg="Option --beams should have at least one excluded beam!"
					if log != None: log.error(msg)
					else: print msg
					quit(1)
			else: is_excluded = False
			if re.search(r'[^\,\:\d]+', self.opts.beam_str) is not None:
				msg="Option --beams can only has digits, colons and commas!"
				if log != None: log.error(msg)
				else: print msg
				quit(1)
			elif re.search(r'[\:]+', self.opts.beam_str) is None:
				msg="Option --beams should have at least one colon!"
				if log != None: log.error(msg)
				else: print msg
				quit(1)
			else:   # forming array of beams
				beams=self.opts.beam_str.split(",")
				# also, we have to remove --beams option with argument from self.options
				# deleting here all instances of --beams (if several) and its arguments
				for jj in reversed([ii for ii in range(len(self.options)) if self.options[ii] == '--beams']):
					del(self.options[jj:jj+2])
				# checking if neither SAP or TAB are empty
				for bb in beams:
					(sap, tab) = bb.split(":")
					if sap == "" or tab == "":
						msg="Option --beams has at least one empty SAP or TAB value!"
						if log != None: log.error(msg)
						else: print msg
						quit(1)
				# defining proper lists of beams
				if is_excluded: self.user_excluded_beams = beams
				else: self.user_beams = beams

		# warning user that some of the results can still be overwritten, if --del is not used
		if not self.opts.is_delete:
			if not self.opts.is_quiet:
				msg="***\n*** Warning: Some of the previous results still can be overwritten.\n\
*** You may want to use --del to have clean run, or specify new output directory.\n***"
				if log != None: log.warning(msg)
				else: print msg
				self.waiting_for_user(10, log)

		# checking if rawdir is specified
		if self.opts.rawdir != "/data":
			cep2.rawdir = self.opts.rawdir

		# NONE is ignored as pulsar name
		if self.opts.psr != "":
       	        	self.psrs=[psr for psr in self.opts.psr.split(",") if psr != "NONE"]
			self.psrs=list(np.unique(self.psrs))

		# checking --nofold and pulsar list
		if not self.opts.is_nofold and len(self.psrs) == 0:
			if not self.opts.is_quiet:
				msg="***\n*** Warning: Pulsar is not specified and PULP will use source names\n\
*** from the parset file first if given, and then will look for the best\n\
*** pulsar in the SAP's FOV for each SAP separately! You also can use\n\
*** predefined words: \"parset\", \"sapfind\", \"sapfind3\", or \"tabfind\".\n\
*** See help for more details.\n***"
				if log != None: log.warning(msg)
				else: print msg
				self.waiting_for_user(10, log)

		# checking if number of threads is good
		if self.opts.nthreads <= 0:
			msg="Number of threads should be positive! Given is %d. Exiting..." % (self.opts.nthreads)
			if log != None: log.error(msg)
			else: print msg
			quit(1)
		if self.opts.nthreads > 6:
			msg="\n*** Warning *** Number of threads %d is too high! Safe is 2-6. Best is 2-3\n" % (self.opts.nthreads)
			if log != None: log.warning(msg)
			else: print msg

		if not self.opts.is_nofold:
			# reading B1950 and J2000 pulsar names from the catalog and checking if our pulsars are listed there
			# also reading coordinates and flux
			self.psrbs, self.psrjs, self.s400 = np.loadtxt(cep2.psrcatalog, comments='#', usecols=(1,2,9), dtype=str, unpack=True)
			self.ras, self.decs = np.loadtxt(cep2.psrcatalog, comments='#', usecols=(5,6), dtype=float, unpack=True)

			# checking if given psr(s) names are valid, and these pulsars are in the catalog
			if len(self.psrs) != 0 and self.psrs[0] != "parset" and self.psrs[0] != "sapfind" and \
				self.psrs[0] != "sapfind3" and self.psrs[0] != "tabfind":
				if self.opts.parfile != "" and len(self.psrs) > 1:
					msg="Parfile '%s' is given, but more than 1 pulsar are given to fold. Exiting..." % (self.opts.parfile)
					if log != None: log.error(msg)
					else: print msg
					quit(1)
				# copying parfile (if given) to temporary ~/.pulp/<obsid> dir
				# and re-defining parfile as located either in .pulp/<obsid> dir or dir with all par-files
				if self.opts.parfile != "":
					# checking first if parfile is in the current directory. If not then checking
					# if this parfile exists in the directory with all parfiles
					if os.path.exists(self.opts.parfile):
						# now checking if the path in parfile has '.pulp'. If not, then copy the file to ~/.pulp/<obsid>
						if re.search(r'\.pulp', self.opts.parfile) is None:
							msg="Copying parfile '%s' to %s..." % (self.opts.parfile, cep2.get_logdir())
							if log != None: log.info(msg)
							else: print msg
							cmd="cp -f %s %s" % (self.opts.parfile, cep2.get_logdir())
							os.system(cmd)
							# we should also change the parfile name in self.options in order to pass correct file to processing nodes
							newpar="%s/%s" % (cep2.get_logdir(), self.opts.parfile.split("/")[-1])
							self.options = [re.search(self.opts.parfile, opt) is None and opt or re.sub(self.opts.parfile, newpar, opt) \
								for opt in self.options]
							self.opts.parfile=newpar
					else:
						msg="Checking if given parfile '%s' exists in %s directory..." % (self.opts.parfile.split("/")[-1], cep2.parfile_dir)
						if log != None: log.info(msg)
						else: print msg
						newpar="%s/%s" % (cep2.parfile_dir, self.opts.parfile.split("/")[-1])
						self.options = [re.search(self.opts.parfile, opt) is None and opt or re.sub(self.opts.parfile, newpar, opt) for opt in self.options]
						self.opts.parfile=newpar
						if not os.path.exists(self.opts.parfile):
							msg="Can't find parfile '%s'. Exiting..." % (self.opts.parfile.split("/")[-1])
							if log != None: log.error(msg)
							else: print msg
							quit(1)

				if len(self.psrs) > 3:
					msg="%d pulsars are given, but only first 3 will be used for folding" % (len(self.psrs))
					if log != None: log.warning(msg)
					else: print msg
					self.psrs=self.psrs[:3]
				# checking if given pulsar names are good
				for psr in self.psrs:
					if not check_pulsars(psr, self, cep2, log):
						msg="No parfile found for pulsar %s. Exiting..." % (psr)
						if log != None: log.error(msg)
						else: print msg
						quit(1)
			else:
				msg="No pulsar names are given. PULP will find the proper pulsar(s) to fold..."
				if log != None: log.info(msg)
				else: print msg

        # checking if raw data for specified beams are located on one of the down nodes
        def is_rawdata_available(self, cep2, obs, log=None):
                msg="Checking if all data/nodes are available for user-specified beams..."
                if log != None: log.info(msg)
                else: print msg

                # if some TABs have raw data in several locations
                # we also need to check if hoover nodes are up
                avail_hoover_nodes=list(set(cep2.hoover_nodes).intersection(set(cep2.alive_nodes)))

                # first forming the actual list of beams to process taking also into account
                # cmdline flags, like --noIS, --noCS, --noCV, --noFE
                if len(self.user_beams) > 0: self.beams = self.user_beams
                else:
                        self.beams = []
                        for sap in obs.saps:
                                for tab in sap.tabs:
                                        beam="%d:%d" % (sap.sapid, tab.tabid)
                                        # checking if this beam is already excluded
                                        if beam in self.user_excluded_beams: continue
                                        # ignoring IS beams
                                        if self.opts.is_noIS and not tab.is_coherent: continue
                                        # ignoring FE beams (both CS and CV)
                                        if self.opts.is_noFE and tab.is_coherent and tab.specificationType == "flyseye": continue
                                        # ignoring CS beams
                                        if self.opts.is_noCS and obs.CS and tab.is_coherent and tab.specificationType != "flyseye": continue
                                        # ignoring CV beams
                                        if self.opts.is_noCV and obs.CV and tab.is_coherent and tab.specificationType != "flyseye": continue
                                        self.beams.append(beam)

                # now we are checking if raw data are available for beams we want to process
                excluded_beams_id=[]
                for ii in range(len(self.beams)):
                        sapid=int(self.beams[ii].split(":")[0])
                        tabid=int(self.beams[ii].split(":")[1])
                        tab = obs.saps[sapid].tabs[tabid]
                        if len(tab.location) > 0:
                                # if here, it means node is available for this beam
                                if len(tab.location) > 1 and len(avail_hoover_nodes) != len(cep2.hoover_nodes):
                                        loc=""
                                        if tab.is_coherent and "locus101" not in avail_hoover_nodes: loc="locus101"
                                        if not tab.is_coherent and "locus102" not in avail_hoover_nodes: loc="locus102"
                                        if loc != "":
                                                excluded_beams_id.append(ii)
                                                msg="Hoover node %s is not available for the beam %d:%d [#locations = %d] - excluded" % (loc, sapid, tabid, len(tab.location))
                                                if log != None: log.warning(msg)
                                                else: print msg
                        else: # no data available
                                excluded_beams_id.append(ii)
                                msg="No data available for the beam %d:%d - excluded" % (sapid, tabid)
                                if log != None: log.warning(msg)
                                else: print msg
                # now giving summary of excluded beams and deleted them from the list
                if len(excluded_beams_id) > 0:
                        msg="Excluded beams [%d]: %s" % (len(excluded_beams_id), ", ".join([self.beams[id] for id in excluded_beams_id]))
                        if log != None: log.info(msg)
                        else: print msg
                        # deleting these excluded beams from the cmdline.beams list
                        for id in reversed(excluded_beams_id):
                                del(self.beams[id])
                else:
                        if len(self.beams) > 0:
                                msg="All data/nodes are available"
                                if log != None: log.info(msg)
                                else: print msg

	# print summary of all set input parameters
	def print_summary(self, cep2, obs, log=None):
		if log != None:
			log.info("")
			log.info("Pulsar Pipeline, %s" % (self.version))
			log.info("Prg: %s" % (self.prg))
			if len(self.user_beams) == 0 and len(self.user_excluded_beams) == 0:
				log.info("Cmdline: %s %s" % (self.prg.split("/")[-1], " ".join(self.options)))
			elif len(self.user_beams) != 0:
				log.info("Cmdline: %s %s" % (self.prg.split("/")[-1], " ".join(self.options + ['--beams'] + [",".join(self.user_beams)])))
			else:
				log.info("Cmdline: %s %s" % (self.prg.split("/")[-1], " ".join(self.options + ['--beams'] + ["^"+",".join(self.user_excluded_beams)])))
			log.info("")
			log.info("ObsID = %s" % (self.opts.obsid))
			if self.opts.is_nofold: pulsar_status = "No folding"
			else:
				if len(self.psrs) == 0: pulsar_status = "default:parset -> sapfind"
				else: 
					if self.psrs[0] == "parset" or self.psrs[0] == "sapfind" or self.psrs[0] == "sapfind3" or self.psrs[0] == "tabfind":
						pulsar_status = self.psrs[0]
					else: pulsar_status = ", ".join(self.psrs)
			log.info("PSR(s) = %s" % (pulsar_status))
			if not self.opts.is_nofold:
				if len(self.psrs) == 0:
					for sap in obs.saps:
						if sap.source != "" and check_pulsars(sap.source, self, cep2, None): log.info("SAP=%d   PSR: %s" % (sap.sapid, sap.source))
						else: log.info("SAP=%d   PSR(s): %s" % (sap.sapid, len(sap.psrs)>0 and sap.psrs[0] or ""))
				if len(self.psrs) != 0 and self.psrs[0] == "sapfind":
					for sap in obs.saps: log.info("SAP=%d   PSR(s): %s" % (sap.sapid, len(sap.psrs)>0 and sap.psrs[0] or ""))
				if len(self.psrs) != 0 and self.psrs[0] == "sapfind3":
					for sap in obs.saps: log.info("SAP=%d   PSR(s): %s" % (sap.sapid, ", ".join(sap.psrs)))
				if len(self.psrs) != 0 and self.psrs[0] == "parset":
					for sap in obs.saps: log.info("SAP=%d   PSR: %s" % (sap.sapid, sap.source))
			if self.opts.parfile != "":
				log.info("User-specified Parfile = %s" % (self.opts.parfile))
			if self.opts.rawdir != "/data":
				log.info("User-specified Raw data directory = %s" % (self.opts.rawdir))
			if self.opts.parset != "":
				log.info("User-specified Parset file = %s" % (self.opts.parset))
			log.info("Output Dir = %s_*/%s" % (cep2.processed_dir_prefix, self.opts.outdir != "" and self.opts.outdir or self.opts.obsid + "_red*"))
			log.info("Delete previous results = %s" % (self.opts.is_delete and "yes" or "no"))
			log.info("Log files mode = %s" % (self.opts.is_log_append and "append" or "overwrite"))
			if self.opts.is_summary: log.info("Summaries ONLY")
			else:
				skipped=""
				if obs.CS and self.opts.is_noCS: skipped += " CS"
				if obs.IS and self.opts.is_noIS: skipped += " IS"
				if obs.CV and self.opts.is_noCV: skipped += " CV"
				if obs.FE and self.opts.is_noFE: skipped += " FE"
				if obs.CV and self.opts.is_skip_rmfit: log.info("rmfit = ")
				if len(self.user_beams) != 0:
					log.info("User-specified BEAMS to process: %s" % (", ".join(self.user_beams)))
				if len(self.user_excluded_beams) != 0:
					log.info("User-specified BEAMS to be excluded: %s" % (", ".join(self.user_excluded_beams)))
				if self.opts.is_plots_only: log.info("Diagnostic plots ONLY")
				else:
					log.info("Data decoding = %s" % (self.opts.is_nodecode and "no" or "yes"))
					log.info("RFI Checking = %s" % (self.opts.is_norfi and "no" or "yes"))
					log.info("DSPSR = %s" % (self.opts.is_skip_dspsr and "no" or "yes"))
					if not self.opts.is_skip_dspsr:
						log.info("pdmp = %s" % ((self.opts.is_nopdmp or self.opts.is_nofold) and "no" or "yes"))
					if obs.CV: log.info("rmfit = %s" % (self.opts.is_skip_rmfit and "no" or "yes"))
			log.info("")
