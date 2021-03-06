#!/usr/bin/env python
'''
Placeholder for a module that has miscellaneous helper functions that now sit
in the LPPS search, folding and singlepulse scripts.
'''
import subprocess
import os
import stat
import sys
import shutil
import re
from os.path import join

# ----------------------------------------------------------------------------
# -- Some useful Exception classes                                          --
# ----------------------------------------------------------------------------

class MissingOptionException(Exception):
    def __init__(self, option_name):
        self.option_name = option_name

    def __str__(self):
        return 'The %s option was not supplied.' % self.option_name

class MissingCommandLineOptionException(MissingOptionException):
    def __str__(self):
        return 'The --%s option was not supplied.' % self.option_name

class DirectoryNotEmpty(Exception):
    def __init__(self, path):
        self.path = path

    def __str__(self):
        return 'The path \'%s\' is not empty.' % self.path

class WrongPermissions(Exception):
    def __init__(self, path):
        self.path = path

    def __str__(self):
        return 'The path \'%s\' has the wrong permissions' % self.path

# ----------------------------------------------------------------------------
# -- Helper functions for making system calls.                              --
# ----------------------------------------------------------------------------

def get_command(program, options_dict, parameters):
    '''
    Construct system calls (for commandlines).
    
    >>> get_command('prepfold', {'-o' : 'outfile'}, ['infile1', 'infile2'])
    'prepfold -o outfile infile1 infile2'
    '''
    cmd = [program]
    for option, value in options_dict.iteritems():
        if len(value) > 0:
            cmd.append(option)
            cmd.append(value)
        else:
            cmd.append(option)
    cmd.extend(parameters)
    cmd = ' '.join(cmd)
    return cmd

def run_command(program, options_dict, parameters, *args, **kwargs):
    '''
    Construct and run system calls using subprocess module.

    Note: calls to subprocess.Popen use shell=True (as the commandlines can
    then easily be logged).    
    '''
    stdout = kwargs.get('stdout', sys.stdout)
    stderr = kwargs.get('stderr', sys.stderr)

    cmd_ln = get_command(program, options_dict, parameters)
    # TODO : consider doing something to the return codes
    print '-' * 70
    print 'Running command :', cmd_ln
    stdout.flush()
    p = subprocess.Popen(cmd_ln, shell=True, stderr=stderr, stdout=stdout)
    output, error = p.communicate()
    print '-' * 70
    return p.returncode

# ----------------------------------------------------------------------------
# -- Helper functions for making shell scripts                              --
# ----------------------------------------------------------------------------

def run_as_script(command_list, script_filename, log_filename):
    '''
    Run a list of commands as script, return subprocess.Popen instance.
    '''
    create_script(command_list, script_filename)
    p = subprocess.Popen(script_filename + '>' + log_filename + ' 2>&1',
        shell=True)

    return p


def create_script(command_list, filename):
    '''
    Create a shell script given a list of commands and filename.
    '''
    # Add echo statements to show what's getting run in the logs as well.
    tmp = []
    for cmd in command_list:
        tmp.append('date')
        tmp.append('echo \'%s\'' % cmd)
        tmp.append(cmd)
    tmp.append('date')
    # Open the script for writing and write the contents:
    try:
        f = open(filename, 'w')
        try:
            f.write('\n'.join(tmp))
        finally:
            f.close()
    except IOError, e:
        print 'Failed to open script file %s .' % filename
        raise

    # 'chmod' u+rwx
    try:
        os.chmod(filename,
            stat.S_IXUSR | stat.S_IWUSR | stat.S_IRUSR)
    except Exception, e:
        print 'Failed to make script executable.'
        raise

# ----------------------------------------------------------------------------
# -- Helper functions for copying/symlinking/moving/removing files          --
# ----------------------------------------------------------------------------

def symlink_matching(from_dir, to_dir, file_regexp):
    file_pattern = re.compile(file_regexp)

    files = os.listdir(from_dir)
    print 'Symlink matching %s' % file_regexp
    n_matched = 0
    n_not_matched = 0
    for f in files:
        if file_pattern.match(f):
            n_matched += 1
            os.symlink(join(from_dir, f), join(to_dir, f))
        else:
            n_not_matched += 1
    print 'Symlinked %d files (%d non-matching files).' % (n_matched, n_not_matched)
    return  

def move_matching(from_dir, to_dir, file_regexp):
    file_pattern = re.compile(file_regexp)

    files = os.listdir(from_dir)
    print 'Move matching %s' % file_regexp
    n_matched = 0
    n_not_matched = 0
    for f in files:
        if file_pattern.match(f):
            n_matched += 1
            shutil.move(join(from_dir, f), join(to_dir, f))
        else:
            n_not_matched += 1
    print 'Moved %d files (%d non-matching files).' % (n_matched, n_not_matched)
    return  

def copy_matching(from_dir, to_dir, file_regexp):
    file_pattern = re.compile(file_regexp)

    files = os.listdir(from_dir)
    print 'Copy matching %s' % file_regexp
    n_matched = 0
    n_not_matched = 0
    for f in files:
        if file_pattern.match(f):
            n_matched += 1
            shutil.copy(join(from_dir, f), join(to_dir, f))
        else:
            n_not_matched += 1
    print 'Copied %d files (%d non-matching files).' % (n_matched, n_not_matched)
    return  

def remove_matching(target_dir, file_regexp):
    file_pattern = re.compile(file_regexp)
    files = os.listdir(target_dir)
    print 'Remove matching %s' % file_regexp
    n_matched = 0
    n_not_matched = 0
    for f in files:
        if file_pattern.match(f):
            n_matched += 1
            os.remove(join(target_dir, f))
        else:
            n_not_matched += 1
    print 'Removed %d files (%d non-matching files).' % (n_matched, n_not_matched)
    return  



if __name__ == '__main__':
    print 'Running tests.'
    import doctest
    doctest.testmod()
