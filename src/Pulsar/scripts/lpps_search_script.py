#!/usr/bin/env python
'''
MSSS pulsar search script, based on the search script for the GBT driftscan
and Vishal Gajjar's work on blind searching of LOFAR data. 

By Thijs Coenen (october 2010)


This module contains a class SearchRun that implements the pulsar search
pipeline. When you instantiate it, it will check whether the input data 
matches expectation (the input data directory should point to one of the 
RSP* directories containing the .subXXXX files and a .sub.inf file), 
whether the output and working directories are empty and writeable.

You make a list of DedispPlan instances describing your desired dedipersion
plan and pass that to the run_search method of your SearchRun instance. The 
run_search method also take the number of available processor cores as an
optional argument. The run_search method then does all the work. The first
thing it does is call the rebatch method that figures out how to call 
prepsubband and mpiprepsubband, this step creates a new list of DedispPlan 
instances with all the constraints that prepsubband and mpiprepsubband put om
a dedispersion plan taken into account (the actual logic that figure out the
constraints is in the mpiprepsubband_helper and prepsubband_helper functions).
Each step in the pulsar search pipeline  can be found in the run_search method
and each step that corresponds to a call out to a PRESTO executable is wrapped
in a Python function (see for instance the run_rfifind and run_prepsubband 
functions). 

The way commandlines are actually built can be found in the get_command and 
run_command functions. 

'''

# Standard library imports:
from __future__ import division
import optparse
import sys
import os
import re
import copy
import glob
import math
import subprocess
import shutil
import time
import logging
import stat
import pprint

# Common python library imports:
import numpy

# PRESTO / PSR UTILS / LOFAR PULSAR library imports:
import presto
import sifting
import psr_utils

# TODO : commit fixed .inf reader to ssps Git repository. 
#from ssps.presto.files.inf import inf_reader
from lpps_search.inf import inf_reader, ra2ascii, dec2ascii
import lpps_search.crawler as crawler
#import folder
import lpps_fold_script as folder
from lpps_search.util import create_script, run_as_script
from lpps_search.util import get_command, run_command

# ----------------------------------------------------------------------------
# -- Utility functions from GBT or Vishal's scripts --------------------------

class DedispPlan(object):
    """
    class dedisp_plan(lodm, dmstep, dmsperpass, numpasses, numsub, downsamp)
       A class describing a de-dispersion plan for prepsubband in detail. 
       Imported from GBT_search pipe-line.       
    """
    def __init__(self, lodm, dmstep, dmsperpass, numpasses, numsub, downsamp):
        self.lodm = float(lodm)
        self.dmstep = float(dmstep)
        self.dmsperpass = int(dmsperpass)
        self.numpasses = int(numpasses)
        self.numsub = int(numsub)
        self.downsamp = int(downsamp)
        self.sub_dmstep = self.dmsperpass * self.dmstep
        self.dmlist = []  # These are strings for comparison with filenames
        self.subdmlist = []
        for ii in range(self.numpasses):
             self.subdmlist.append("%.2f"%(self.lodm + (ii+0.5)*self.sub_dmstep))
             lodm = self.lodm + ii * self.sub_dmstep
             dmlist = ["%.2f"%dm for dm in \
                       numpy.arange(self.dmsperpass)*self.dmstep + lodm]
             self.dmlist.append(dmlist)

        self.prepsubband_time = None
        self.search_time = None

def get_baryv(ra, dec, mjd, T, obs="LF"):
    """ 
    get_baryv(ra, dec, mjd, T):
        Determine the average barycentric velocity towards 'ra', 'dec'
        during an observation from 'obs'.  The RA and DEC are in the
        standard string format (i.e. 'hh:mm:ss.ssss' and 'dd:mm:ss.ssss').
        'T' is in sec and 'mjd' is (of course) in MJD.
    """
    tts = psr_utils.span(mjd, mjd+T/86400.0, 100)
    nn = len(tts)
    bts = numpy.zeros(nn, dtype=numpy.float64)
    vel = numpy.zeros(nn, dtype=numpy.float64)
    presto.barycenter(tts, bts, vel, nn, ra, dec, obs, "DE200")
    return vel.mean()


# ----------------------------------------------------------------------------

def read_bad_observing_frequencies(rfi_file):
    '''Read system wide master.rfi file. (Note: path hardcoded for now!)'''

    bad_freq_ranges = []
    
    try:
        f = open(rfi_file, 'r')
        try:
            # parse master.rfi file:
            # assumes that comments are allowed on lines that start with #
            # assumes that there are numbers per line
            # 1 the middle of the bad frequency range
            # 2 the width in frequency of that bad observing frequency range
            # ignores anything else
            for l in f:
                if l[0] == '#': continue
                tmp = l.split()
                if len(tmp) != 2: continue
                try:
                    middle = float(tmp[0])
                    width = float(tmp[1])
                except ValueError, e:
                    pass
                else:
                    bad_freq_ranges.append((middle - width, middle + width))
        finally:
            f.close()
    except IOError, e:
        print 'Failed to open :', rfi_file
 
    return bad_freq_ranges

def intersect_channel_map_with_freq_range(bad_freq_mapping, freq_range):
    '''Intersect channel index 2 frequency range mapping with another range.'''
    indexes = []
    for channel_idx, fr in bad_freq_mapping.iteritems():
        if fr[1] >= freq_range[0] and fr[0] <= freq_range[1]:
            indexes.append(channel_idx)
    return indexes


def determine_numout(n_bins):
    '''
    Determine an appropriate length for the dedispersed timeseries.
    
    Note : an appropriate length is one that is easily factorisable in 
    factors of 2, 3, 5, 7 and 11. (This speads up the Fast Fourier
    Transforms, i.e. the realfft calls.)
    '''
    # Needs the itertools module from Python 2.6, since the LOFAR cluster
    # has Python 2.5 installed, here is a little code copied from the 
    # Python documenation:
    # (Retrieved from http://docs.python.org/library/itertools.html on
    # october 25 2010.)
    
    def combinations_with_replacement(iterable, r):
        # combinations_with_replacement('ABC', 2) --> AA AB AC BB BC CC
        pool = tuple(iterable)
        n = len(pool)
        if not n and r:
            return
        indices = [0] * r
        yield tuple(pool[i] for i in indices)
        while True:
            for i in reversed(range(r)):
                if indices[i] != n - 1:
                    break
            else:
                return
            indices[i:] = [indices[i] + 1] * (r - i)
            yield tuple(pool[i] for i in indices)       

    # Find the length of the data, then see whether it is easily 
    # factorisable, if not pad the data a little so that the data has an
    # easily factorisable length.  
    # If the number of bins is already a power of 2 just return:
    if math.log10(n_bins) / math.log10(2) == 0:
        return n_bins
    # If the data is very short just use a minimum length of 1024 .
    # Note : this 1024 should be larger than the maxmimum downsample
    # factor from the dedispersion plan.
    elif n_bins < 1024:
        return 1024
    # Find a length that is slightly larger than the data length that is
    # easily factorisable. 
    else:
        p = math.log10(n_bins) / math.log10(2)
        base = 2 ** int(p - 5)
        best_trial = n_bins * n_bins
        for t in combinations_with_replacement((1,2,3,5,7,11), 20):
            trial = base
            for i in t:
                trial *= i
            if trial >= n_bins and trial < best_trial:
                best_trial = trial
        return best_trial

# ----------------------------------------------------------------------------
# -- Individual steps in the pipeline (functions wrapping parts of presto   --
# -- whether they are python modules or system calls does not matter).      --
# ----------------------------------------------------------------------------

def run_rfifind(subband_globpattern, result_dir, basename, bad_channel_str):
    '''Get the commandline for rfifind. '''
    files = subband_globpattern
    # Jump to the results directory to run rfifind.
    current_dir = os.getcwd()
    os.chdir(result_dir)
  
    options = {
        '-blocks' : str(512),
        '-o' : basename,
    }

    if bad_channel_str:
        options['-zapchan'] = bad_channel_str

    status = run_command('rfifind', options, [files])
    os.chdir(current_dir)
    return status

def run_prepsubband(ddplan, n_cores, subband_globpattern, result_dir,
    basename, mask_file, numout):
    '''
    Run the (mpi)prepsubband commandline given a DDplan instance.

    Note : only use with the DDplan instances that are created by the 
    rebatch member fuction of the SearchRun class (since they always have
    ddplan.numpasses == 1 and an appropriate ddplan.dmsperpass --- exactly
    what this function expects).
    '''
    new_numout = numout / ddplan.downsamp
    print numout, ddplan.downsamp, new_numout
    assert numout % ddplan.downsamp == 0
    new_numout = int(new_numout)
    prepsubband_options = {
        '-noclip' : '',
        '-mask' : mask_file, 
        '-o' : os.path.join(result_dir, basename),
        '-lodm' : '%.2f' % ddplan.lodm,
        '-numdms' : str(ddplan.dmsperpass),
        '-downsamp' : str(ddplan.downsamp),
        '-dmstep' : '%.2f' % ddplan.dmstep,
        '-numout' : str(new_numout),
        '-runavg' : '', 
    }
    

    MPIRUN_OPTIONS = {
       '--mca' : 'btl',
        '^openib' : '',
    }


    prepsubband_parameters =  [subband_globpattern]

    if n_cores > 1:
        tmp = get_command('mpiprepsubband', prepsubband_options,
            prepsubband_parameters)
        mpirun_options = copy.copy(MPIRUN_OPTIONS)
        mpirun_options['-np'] = str(n_cores)
        print get_command('mpirun', mpirun_options, [tmp])
        status = run_command('mpirun', mpirun_options, [tmp])
    else:
        print get_command('prepsubband', prepsubband_options,
            prepsubband_parameters)
        status = run_command('prepsubband', prepsubband_options,
            prepsubband_parameters)

    return status

def get_fourier_and_correct_commands(work_dir, basename, dm, bary_v, zap_file):
    '''Get PRESTO realfft and rednoise commands.'''
    # TODO : add exception handling.
    # TODO : add zapbirds support (ask about zap files)

    cmds = ['cd %s' % work_dir]

    dat_file = basename + '_DM%.2f' % dm + '.dat'
    fft_file = basename + '_DM%.2f' % dm + '.fft'
    red_fft_file = basename + '_DM%.2f' % dm + '_red.fft'    

    # Fourier transform takes place in the same directory as the .dat file.
    cmds.append(get_command('realfft', {'-outdir' : '.'}, [dat_file]))
    
    # Run zapbirds if zap_file is available
    if zap_file:
        cmds.append(get_command('zapbirds', {
                '-zap' : '',
                '-zapfile' : zap_file,
                '-baryv' : '%6g' % bary_v,
            }, [fft_file])
        )
        cmds.append(get_command('rednoise', {}, [fft_file]))
    # Run rednoise correction.
    cmds.append(get_command('rednoise', {}, [fft_file])) 
    # copy rednoise corrected fft back to the normal fft
    cmds.append(get_command('mv', {}, [red_fft_file, fft_file]))
    
    return cmds
  
 
def get_accelsearch_command(work_dir, basename, dm, z_max):
    '''Get PRESTO accelsearch command.'''

    ACCELSEARCH_OPTIONS = {
        # like Vishal's script
        '-numharm' : str(8),
        '-sigma' : str(6),
        '-flo' : '%.2f' % 1,
        '-zmax' : '%d' % z_max,
    }
    fft_file = basename + '_DM%.2f' % dm + '.fft'

    cmds = ['cd %s' % work_dir]
    cmds.append(get_command('accelsearch', ACCELSEARCH_OPTIONS, [fft_file]))

    return cmds

def get_single_pulse_search_command(work_dir, basename, dm):
    '''Run PRESTO single_pulse_search.py for searching.'''

    dat_file = os.path.join(work_dir, basename + '_DM%.2f' % dm + '.dat')
    cmd = get_command('single_pulse_search.py', {'-p' : ''}, [dat_file])

    return [cmd]

def run_single_pulse_search_plotter(work_dir, basename):
    '''Run PRESTO single_pulse_search.py for plotting.'''
    # Think about dealing with the case where there are too many input files
    # (so the script needs to be run for subsets of the data). For now use
    # the -g option for single_pulse_search.py to pass the .singlepulse files.
    
    globpattern = os.path.join(work_dir, basename + '_DM*.*.singlepulse')
    status = run_command('single_pulse_search.py', {
        '-g' : '"' + globpattern + '"',
    }, [])

    return status
   
# ----------------------------------------------------------------------------

class SearchRun(object):
    def __init__(self, in_dir, work_dir, out_dir, rfi_file, zap_file):
        '''This class describes a blind pulsar search.'''
        # Set directories used in search
        self.in_dir = os.path.abspath(in_dir)
        self.work_dir = os.path.abspath(work_dir) 
        self.out_dir = os.path.abspath(out_dir)

        # Check that working directory and the output directory are empty and
        # that they are writable.
        os.access(self.work_dir, os.F_OK & os.R_OK & os.W_OK)
        files = os.listdir(self.work_dir)
        if len(files) > 0:
            raise Exception('Working directory not empty.')

        os.access(self.out_dir, os.F_OK & os.R_OK & os.W_OK)
        files = os.listdir(self.out_dir)    
        if len(files) > 0:
            raise Exception('Output directory %s not empty.' % self.out_dir)


        self.basename = self.determine_basename()
        # Read the metadata from the input directory and check that it is
        # correct (old datasets had a mismatch between the number of samples
        # in the .inf file and the size of the .subXXXX files on disk).
        self.metadata = inf_reader(os.path.join(self.in_dir, self.basename + \
            '.sub.inf')) 
       
        # Old data sets have incorrect metadata, deal with that.
        datafile_size = os.path.getsize(os.path.join(self.in_dir, 
            self.basename + '.sub0000'))
        if self.metadata.n_bins * 2 != datafile_size:
            if self.metadata.n_bins == 999 and datafile_size % 2 == 0:
                self.metadata.n_bins = datafile_size // 2
                logging.info(
                    'Overruled wrong metadata (timeseries length was 999)'
                )
            else:    
                raise Exception('Metadata incorrect')

        if rfi_file:
            self.rfi_file = os.path.abspath(rfi_file)
        else:
            try:
                lofarsoft = os.environ['LOFARSOFT'] 
            except KeyError, e:
                print 'No RFI specified. $LOFARSOFT not set.'
                raise

            self.rfi_file = os.path.join(lofarsoft, 'release', 'share', 
                'pulsar', 'data', 'master.rfi')
        self.zap_file = zap_file
        ra_str = ra2ascii(*self.metadata.j2000_ra)
        dec_str = dec2ascii(*self.metadata.j2000_dec)
        self.bary_v = get_baryv(ra_str, dec_str, self.metadata.epoch_mjd, 
            self.metadata.n_bins * self.metadata.bin_width, obs='LF') 
        print 'Calculated barycentric velocity :', self.bary_v
        logging.info('Prepared for pulsar search run.')
        logging.info('Using %s as input directory.' % self.in_dir)
        logging.info('Using %s as working directory.' % self.work_dir)
        logging.info('Using %s as output directory.' % self.out_dir)


    def get_subband_globpattern(self):
        '''
        Get a globpattern that matches the raw data (the subbands)
        
        Note : the globpattern contains the full path to the raw data.'''
        return os.path.join(self.in_dir, self.basename + '.sub[0-9]???')
    
    def determine_basename(self):
        '''Find the basename of this observation from raw data directory.'''
        files = os.listdir(self.in_dir)
        for f in files:
            if f.endswith('.sub0000'):
                basename = f[:-8]
                break
        else:
            raise Exception('No %s.sub0000 file available.')
        return basename

    def determine_channel_frequency_range_mapping(self):
        '''
        Make a mapping from channel number to frequency range.

        Note : Assumes that the observing frequency range is contiguous (like
        it is expressed in the .inf files).
        '''
        
        n_channels = self.metadata.n_channels
        c_freq_lo = self.metadata.low_channel_central_freq
        channel_bandwidth = self.metadata.channel_bandwidth       
        
        mapping = {}
        for i in range(n_channels):
            mapping[i] = (
                c_freq_lo + channel_bandwidth * (i - 0.5),
                c_freq_lo + channel_bandwidth * (i + 0.5)
            )
        return mapping
   
    def rebatch(self, ddplans, n_cores):
        '''
        Take the dedispersion plan and cut it in manageable pieces. 
        
        Note : This needs to happen because of the extra logic that deals
        with mpiprepsubband's extra constraints, and the dedispersion plans
        in Vishal's code that have ddplan.dmsperpass > 1000 DM steps per pass.
        '''
        annotated_ddplans = []
        
        for ddplan in ddplans:
            downsample = ddplan.downsamp
            if n_cores > 1:
                rebatched = mpiprepsubband_helper(ddplan, n_cores)
            else:
                rebatched = prepsubband_helper(ddplan)
            
            for chunk in rebatched: 
                new_ddplan = DedispPlan(chunk[0], ddplan.dmstep, chunk[1], 1, 
                    self.metadata.n_channels, downsample)
                annotated_ddplans.append((new_ddplan, chunk[2]))
        
        return annotated_ddplans

   
    def get_dms(self, ddplan):
        # TODO : move to DDplan class
        return [ddplan.lodm + ddplan.dmstep * i  \
            for i in range(ddplan.dmsperpass)]

    def run_search(self, ddplans, z_values, n_cores = None, 
            no_singlepulse = False):
        # The GBT drift scan survey uses dedispersion plans as units
        # of work. Because our use of mpiprepsubband (has extra constraints
        # on the number of DM trials it can write) and the fact that
        # Vishal's script contains a ddplan with more than a 1000 DM trials
        # that get created in one go we need to split the work into some
        # smaller units than specified in the dedipersion plan. We create new 
        # DDplans to that take the constraints into account.
        self.annotated_ddplans = self.rebatch(ddplans, n_cores)
        m = self.determine_channel_frequency_range_mapping()
        
        print 'frequency mapping:'
        print m
        print 'Reading the master.rfi file.'
        bad_freq_ranges = read_bad_observing_frequencies(self.rfi_file)
        print 'bad frequency ranges:'
        print bad_freq_ranges


        bad_channels  = []

        for fr in bad_freq_ranges:
            bad_channels.extend(intersect_channel_map_with_freq_range(m, fr))
        bad_channel_str = ','.join([str(idx) for idx in bad_channels]) 

        # Perform rfifind
        t_rfifind_start = time.time()
        rfifind_status = run_rfifind(self.get_subband_globpattern(), 
            self.work_dir, self.basename, bad_channel_str)
        t_rfifind_end = time.time()
        # Do some exit status checking on rfifind
        if rfifind_status != 0:
            print 'rfifind failed, exiting'
            sys.exit(1)
        print 'Running rfifind took %.2f seconds.' % \
            (t_rfifind_end - t_rfifind_start)
        # Move the rfifind output files
        for file in glob.glob(os.path.join(self.work_dir, '*_rfifind*')):
            try:
                shutil.move(file, self.out_dir)
            except IOError, e:
                pass 
        # Store the location of the rfifind mask file (used by other PRESTO
        # binaries):
        rfifind_mask_file = os.path.join(self.out_dir, self.basename + \
            '_rfifind.mask')
        
        ddplan_i = -1 
        for ddplan, n_cores_to_use in self.annotated_ddplans:
            ddplan_i += 1
            # Note : Check whether data needs downsampling (and how GBT does
            # it).

            t_prepsubband_start = time.time() 
            prepsubband_status = run_prepsubband(ddplan, n_cores_to_use,
                self.get_subband_globpattern(), self.work_dir, self.basename,
                rfifind_mask_file, determine_numout(self.metadata.n_bins))
            t_prepsubband_end = time.time()
            ddplan.prepsubband_time = t_prepsubband_end - t_prepsubband_start

            t_search_start = time.time() 
            command_list = [[] for i in range(n_cores)]
            for i, dm in enumerate(self.get_dms(ddplan)):
                # determine which core the command is for:
                core_index = i % n_cores
                # Append all the per DM processing commands to the appropriate
                # list of commands:
                # fourier transform + rednoise correction:
                command_list[core_index].extend(
                    get_fourier_and_correct_commands(
                        self.work_dir, self.basename, dm, self.bary_v, 
                        self.zap_file,
                    )
                )

                # acceleration search for each z value:
                for z_max in z_values:
                    command_list[core_index].extend(
                        get_accelsearch_command(self.work_dir, 
                            self.basename, dm, z_max)
                    )
                    # move the accelsearch output to the output directory
                    command_list[core_index].append(
                        get_command('mv', {}, [
                            os.path.join(self.work_dir, self.basename + \
                                '_DM%.2f' % dm + '_ACCEL_%s' % z_max + '*'),
                                self.out_dir
                            ]
                        )
                    )    
                if not no_singlepulse:                   
                    # Run PRESTO single pulse search (not the plotting or SSPS part):
                    command_list[core_index].extend(
                        get_single_pulse_search_command(
                            self.work_dir, self.basename, dm
                            )
                        )
                    # move the single pulse files to the output directory
                    command_list[core_index].append(
                        get_command('mv', {}, [
                            os.path.join(self.work_dir, 
                            self.basename + '_DM%.2f' % dm + '.singlepulse'),
                            self.out_dir
                            ]
                        )
                    )
               
                # move files that are no longer necessary:
                command_list[core_index].append(
                    get_command('mv', {}, [
                        os.path.join(self.work_dir, 
                        self.basename + '_DM%.2f' % dm + '.inf'),
                        self.out_dir
                        ]
                    )
                )
              
                # remove datafiles that are no longer necessary
                command_list[core_index].append(
                    get_command('rm', {}, [os.path.join(self.work_dir, 
                        self.basename + '_DM%.2f' % dm + '.fft')]
                    )
                )
                command_list[core_index].append(
                    get_command('rm', {}, [os.path.join(self.work_dir,
                        self.basename + '_DM%.2f' % dm + '.dat')]
                    )
                )

            
            # Run the per DM analysis in a paralellized fashion.
            print 'Running per DM analysis'
            popen_objects = []            

            for core_index in range(n_cores):
                if not command_list[core_index]: continue
                script_filename = os.path.join(self.work_dir, 
                    'per_dm_analysis_core%d_step%d.sh' % \
                    (core_index, ddplan_i))
                log_filename = os.path.join(self.work_dir, 
                    'per_dm_analysis_core%d_step%d.log.txt' % \
                    (core_index, ddplan_i))
                try:
                    p = run_as_script(command_list[core_index], script_filename,
                        log_filename)
                except:
                    print 'Failed to run %s' % script_filename
                    raise
                else:    
                    print script_filename
                    popen_objects.append(p) 
            
            # wait for all the scripts to finish
            for p in popen_objects:
                p.wait()
            t_search_end = time.time()
            ddplan.search_time = t_search_end - t_search_start
            
        # Run the seperate folding script (after having created a directory
        # for it to write the folds to).
        os.mkdir(os.path.join(self.out_dir, 'folds'))
        folder.main(
            folddir=os.path.join(self.out_dir, 'folds'),
            subbdir=self.in_dir,
            canddir=self.out_dir,
            basename=self.basename,
        )
        if not no_singlepulse:
            # Deal with single pulse search plotting
            d = os.getcwd()
            os.chdir(self.out_dir)
            status = run_single_pulse_search_plotter('.', self.basename)
            os.chdir(d)

def mpiprepsubband_helper(ddplan, n_cores):
    '''
    Determine mpiprepsubband options based on ddplan.

    Note : This is a helper function don't call this directly, call
    SearchRun.rebatch instead.
    
    returns a list like:
    [(<lowest DM>, <number of DM trials>, <number of CPU cores>), ...]    
    '''
    MAX_N_DMS = 1000 # Some limitation of ?prepsubband
   
    # Do the math on the number of DM trials to make per prepsubband call,
    # which needs to be a multiple of the (number of cores -1).
    max_n_dms = min(
        (MAX_N_DMS // (n_cores - 1)) * (n_cores - 1),
        (ddplan.dmsperpass // (n_cores - 1)) * (n_cores - 1)
    )
    
    # Deal with the corner case where you want fewer DM trials than there
    # are cores - 1 available:
    if ddplan.dmsperpass < (n_cores - 1):
        return [(ddplan.lodm, ddplan.dmsperpass, ddplan.dmsperpass + 1)]
 
    # This loop calculates how many times mpiprepsubband needs to be
    # called, how many DM trials it should create for each call and how 
    # many cores should be used for each call.
    out = []
    for i in range(ddplan.numpasses):
        cum_n_dms = 0         
        # read cum_n_dms as cumulative number of DM trials (this pass)
        while cum_n_dms < ddplan.dmsperpass:
            lowest_dm = cum_n_dms * ddplan.dmstep + ddplan.lodm + \
                i * ddplan.dmsperpass * ddplan.dmstep
            # Figure out how many cores to use and how many DM trials to this
            # (mpi)prepsubband call.
            if cum_n_dms + max_n_dms > ddplan.dmsperpass:
                n_dms = ddplan.dmsperpass - cum_n_dms
                # For the last few DM trials fewer cores should be used since
                # mpiprepsubband only works if:
                # number of DM trials % (number of cores - 1) == 0
                tmp = (n_dms // (n_cores - 1))
                if tmp > 0:
                    out.append((lowest_dm, tmp * (n_cores - 1), n_cores))
                    lowest_dm += tmp * (n_cores - 1) * ddplan.dmstep
                    n_dms -= tmp * (n_cores -1)
                assert n_dms < n_cores
                out.append((lowest_dm, n_dms, n_dms + 1))
            else:
                # For the normal case we use the maximum number of cores 
                # available.
                n_dms = max_n_dms
                out.append((lowest_dm, n_dms, n_cores))            
            cum_n_dms += max_n_dms            
        return out

def prepsubband_helper(ddplan):
    '''
    Determine prepsubband options based on ddplan. 
    
    Note : This is a helper function don't call this directly, call
    SearchRun.rebatch instead.

    returns a list like:
    [(<lowest DM>, <number of DM trials>, 1), ...]    
    '''
    MAX_N_DMS = 1000 # Some limitation of ?prepsubband
   
    # Do the math on the number of DM trials to make per prepsubband call,
    # which needs to be a multiple of the (number of cores -1).
    max_n_dms = min(MAX_N_DMS, ddplan.dmsperpass)
   
    # This loop calculates how many times mpiprepsubband needs to be
    # called, how many DM trials it should create for each call and how 
    # many cores should be used for each call.
    out = []

    for i in range(ddplan.numpasses):
        cum_n_dms = 0 # read cum_n_dms as cumulative number of DM trials     
        # read cum_n_dms as cumulative number of DM trials (this pass)
        while cum_n_dms < ddplan.dmsperpass:
            lowest_dm = cum_n_dms * ddplan.dmstep + ddplan.lodm + \
                i * ddplan.dmsperpass * ddplan.dmstep 
            # Figure out how many DM trials to make this prepsubband call.
            if cum_n_dms + max_n_dms > ddplan.dmsperpass:
                n_dms = ddplan.dmsperpass - cum_n_dms
                out.append((lowest_dm, n_dms, 1))
            else:
                n_dms = max_n_dms
                out.append((lowest_dm, n_dms, 1))            
            cum_n_dms += max_n_dms 
        return out


if __name__ == '__main__':
    parser = optparse.OptionParser()
    parser.add_option('--zap_file', dest='zap_file', type='string',
        default='', metavar='ZAP_LIST', 
        help='File with birdies for zapbirds.')
    parser.add_option('--i', help='Input directory for raw data.', 
        type='string', metavar='IN_DIR', dest='in_dir')
    parser.add_option('--o', help='Output directory for search data.',
        type='string', metavar='OUT_DIR', dest='out_dir') 
    parser.add_option('--w', help='Working directory.',
        type='string', metavar='WORK_DIR', dest='work_dir')
    parser.add_option('--rfi', help='RFI file', type='string', default='',
        metavar='RFI_FILE', dest='rfi_file')    
    parser.add_option('-t', help='Run in testing mode (quick).',
        action='store_true', dest='test', default=False)
    parser.add_option('--z_list', dest='z_list', type='string',
        default='[0,50]', metavar='Z_LIST', 
        help='List of integer z values, default [0,50] - don\'t use spaces.')
    parser.add_option('--ncores', dest='ncores', type='int',
        default=8, help='Number of cores to use (default 8).')
    parser.add_option('--ns', dest='ns', help='Run no single pulse search.',
        action='store_true', default=False)
    
    options, args = parser.parse_args()
    N_CORES = options.ncores

    if not (options.in_dir and options.out_dir and options.work_dir):
        print 'Provide --i, --w and --o options.'
        print parser.print_help()
        sys.exit(1)
    
    # parse list of z values:
    # TODO : refactor list parsing code
    z_values = []
    if options.z_list[0] == '[' and options.z_list[-1] == ']':
        tmp = options.z_list[1:-1].split(',')
        try:
            for z_str in tmp:
                z_values.append(int(z_str))
        except ValueError, e:
            print 'List of z values not specified correctly.'
            raise
    else:
        print 'Z values list needs to be inside of [] and contain no spaces.'
        raise Exception()
    
    # Add basic logging right here.
    logging.basicConfig(
        format='%(asctime)-15s - %(name)s - %(levelname)s - %(message)s',
        level=logging.DEBUG,
    ) 
    t_start = time.time()
    SR = SearchRun(options.in_dir, options.work_dir, options.out_dir, 
        options.rfi_file, options.zap_file)  

    ddplans = []
    # The number of subbands defaults to being the number of channels in the
    # data (according to DDplan.py which is how it is implemented here and
    # in Vishal's script as well).


    if options.test:
        # Quick very tiny dedispersion plan used for testing (whether code
        # is still ok, nothing else). Use -t flag to run with this 
        # dedispersion plan.
        ddplans.append(DedispPlan(lodm=0, dmstep=1, dmsperpass=7, numpasses=1, 
            numsub=SR.metadata.n_channels, downsamp=1))
        ddplans.append(DedispPlan(lodm=7, dmstep=1, dmsperpass=2, numpasses=1, 
            numsub=SR.metadata.n_channels, downsamp=1))
        ddplans.append(DedispPlan(lodm=9, dmstep=1, dmsperpass=7, numpasses=1, 
            numsub=SR.metadata.n_channels, downsamp=1))
        ddplans.append(DedispPlan(lodm=16, dmstep=1, dmsperpass=2, numpasses=1, 
            numsub=SR.metadata.n_channels, downsamp=1))
    else:
        # Jason's full LPPS dedispersion plan:
        ddplans.append(DedispPlan(lodm=0, dmstep=0.03, dmsperpass=1037, 
            numpasses=1, numsub=SR.metadata.n_channels, downsamp=1))
        ddplans.append(DedispPlan(lodm=31.11, dmstep=0.05, dmsperpass=378,
            numpasses=1, numsub=SR.metadata.n_channels, downsamp=2))
        ddplans.append(DedispPlan(lodm=50.01, dmstep=0.1, dmsperpass=447,
            numpasses=1, numsub=SR.metadata.n_channels, downsamp=4))
        ddplans.append(DedispPlan(lodm=94.71, dmstep=0.30, dmsperpass=434,
            numpasses=1, numsub=SR.metadata.n_channels, downsamp=8))
        ddplans.append(DedispPlan(lodm=224.91, dmstep=0.50, dmsperpass=367,
            numpasses=1, numsub=SR.metadata.n_channels, downsamp=16))
        ddplans.append(DedispPlan(lodm=408.41, dmstep=1.00, dmsperpass=92,
            numpasses=1, numsub=SR.metadata.n_channels, downsamp=32))

    SR.run_search(ddplans, z_values, N_CORES, options.ns)
    t_end = time.time()
    print '\n=== TIMINGS ==='
    search_time = 0
    dedisperse_time = 0 
    for ddplan, n_cores in SR.annotated_ddplans:
        print 'From DM %.2f to DM %.2f' %(ddplan.lodm,
             ddplan.lodm + ddplan.dmstep * ddplan.dmsperpass) 
        print 'Dedispersing cost %.2f seconds and searching cost %.2f seconds' % \
            (ddplan.prepsubband_time, ddplan.search_time)
        dedisperse_time += ddplan.prepsubband_time
        search_time += ddplan.search_time
    print '\nTotal time spent dedispersing %.2f' % dedisperse_time
    print 'Total time spent searching %.2f' % search_time
    print '\nIt took %.2f seconds to search the data.' % (t_end - t_start)
    print '(Including bookkeeping and rfifind)'
