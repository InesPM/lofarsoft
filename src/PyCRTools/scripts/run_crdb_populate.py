#! /usr/bin/env python
"""
Script to populate the CR database
"""
import os
from optparse import OptionParser

class CRDatabasePopulator(object):

    def __init__(self, db_filename="", options=None):
        """Initialisation of the CRDatabasePopulator class."""
        self.db_filename = db_filename
        self.options = options
        self.filename_list = []

        if "" == self.db_filename:
            raise ValueError("No database filename provided.")

        if not self.options:
            raise ValueError("No options provided.")

        self.dbManager = crdb.CRDatabase(filename=self.db_filename,
                                         datapath=options.datapath,
                                         resultspath=options.resultspath,
                                         lorapath=options.lorapath)

        if self.dbManager:
            self.db = self.dbManager.db
            self.settings = self.dbManager.settings
        else:
            raise ValueError("Unable to set CRDatabase.")


    def getDatabaseFileList(self):
        """Get a list of filenames that are already in the CR database."""
        result = []

        if self.db:
            sql = "SELECT filename FROM datafiles;"
            records = self.db.select(sql)
            result = [str(r[0]) for r in records]
        else:
            raise ValueError("No database available.")

        return result


    def getDiskFileList(self):
        """Get a list of filenames that need to be added to the CR database."""
        datapath = self.settings.datapath

        self.filename_list = []

        if "" != options.datafile:
            self.filename_list = [options.datafile]
        else:
            print "  Building list of files to process..."
            self.filename_list = os.listdir(datapath)

            if options.verbose:
                print "  initial list contains {0} files".format(len(self.filename_list))

            # Include: h5
            print "  filtering in h5..."
            self.filename_list = [f for f in self.filename_list if ("h5" in f)]
            if options.verbose:
                print "    list contains {0} files after filtering".format(len(self.filename_list))

            # Exclude: test
            print "  filtering out test..."
            self.filename_list = [f for f in self.filename_list if ("test" not in f)]
            if options.verbose:
                print "    list contains {0} files after filtering".format(len(self.filename_list))

            # Exclude: bkp
            print "  filtering out bkp..."
            self.filename_list = [f for f in self.filename_list if ("bkp" not in f)]
            if options.verbose:
                print "    list contains {0} files after filtering".format(len(self.filename_list))

            # Exclude: manual dumps
            print "  filtering out manual dumps..."
            self.filename_list = [f for f in self.filename_list if ("manual" not in f)]
            if options.verbose:
                print "    list contains {0} files after filtering".format(len(self.filename_list))

            # Exclude: L00000 dumps
            print "  filtering out L00000 dumps..."
            self.filename_list = [f for f in self.filename_list if ("L00000_" not in f)]
            if options.verbose:
                print "    list contains {0} files after filtering".format(len(self.filename_list))

            # Only include R000 files
            print "  Filtering in R000..."
            self.filename_list = [f for f in self.filename_list if ("R000" in f)]
            if options.verbose:
                print "    list contains {0} files after filtering".format(len(self.filename_list))

            # Exclude files already in the database
            print "  filtering out files already in database..."
            db_filename_list = self.getDatabaseFileList()
            self.filename_list = [f for f in self.filename_list if (f not in db_filename_list)]
            if options.verbose:
                print "    list contains {0} files after filtering".format(len(self.filename_list))

            print "  - Sorting filename list..."
            filename_transposed_list = [s.split('_',1)[1]+'_'+s.split('_',1)[0] for s in self.filename_list]
            filename_transposed_list.sort()
            self.filename_list = [s.rsplit('_',1)[1]+'_'+s.rsplit('_',1)[0] for s in filename_transposed_list]



def parseOptions():
    """Parse options from the command line."""
    options = None
    args = None

    usage = "usage: %prog [options] <databasefile>"
    parser = OptionParser(usage=usage)

    # parser.add_option("-a", "--archive", type="str", dest="archivepath", default="", help="directory where the archive can be found.")
    parser.add_option("-d", "--datapath", type="str", dest="datapath", default="", help="directory where the datafiles can be found.")
    parser.add_option("-r", "--resultspath", type="str", dest="resultspath", default="", help="directory where the results files are written.")
    parser.add_option("-l", "--lorapath", type="str", dest="lorapath", default="", help="directory where the lora files are written.")
    parser.add_option("-u", "--update", action="store_true", dest="update", default=False, help="Update instead of populating database.")
    parser.add_option("-s", "--hdf5", type="str", dest="datafile", default="", help="name of a single hdf5 file that needs to be registered in the database.")
    parser.add_option("-p", "--parameters", action="store_true", dest="parameters", default=False, help="Enable storing polarization parameter information if available [default=False].")
    parser.add_option("--max-threads", default=1, type=int, help="Maximum number of threads to use.")
    parser.add_option("--max-attempts", default=1, type=int, help="Maximum number of attempts to use.")
    parser.add_option("--logpath", type="str", dest="logpath", default="./log", help="directory where logged information should be written.")
    parser.add_option("-v", "--verbose", action="store_true", dest="verbose", default=False, help="Write verbose information.")

    (options, args) = parser.parse_args()

    if len(args) != 1:
        parser.error("Incorrect number of arguments.")

    options.basepath = os.path.dirname(os.path.abspath(args[0]))

    print "datapath = %s" %(options.datapath)
    print "resultspath = %s" %(options.resultspath)
    print "lorapath = %s" %(options.lorapath)
    print "datafile = %s" %(options.datafile)
    print "logpath = %s" %(options.logpath)

    print "max. threads = %d" %(options.max_threads)
    print "max. attempts = %d" %(options.max_attempts)

    return (options, args)


def call_populate_script(data_filename=""):
    """Call the script that adds *data_filename* to the database
    stored in *db_filename*."""
    db_filename = args[0]

    print "Processing file {0}...".format(data_filename)

    log_filename = options.logpath + "/" + data_filename + ".log"
    with open(log_filename, "w", buffering=1) as log_file:
        status = 1
        attempt = 1

        while ((status != 0) and (attempt <= options.max_attempts)):
            commandline = commandline_base + "{0} {1}".format(db_filename, data_filename)
            print commandline

            status = subprocess.call(commandline, stdout=log_file, stderr=subprocess.STDOUT, shell=True)

            attempt += 1


(options, args) = parseOptions()

# Create command to execute
commandline_base = "python "+os.environ["LOFARSOFT"]+"/src/PyCRTools/scripts/crdb_populate2.py "
if ("" != options.datapath): commandline_base += "-d {0} ".format(options.datapath)
if ("" != options.resultspath): commandline_base += "-r {0} ".format(options.resultspath)
if ("" != options.lorapath): commandline_base += "-l {0} ".format(options.lorapath)
if ("" != options.datafile): commandline_base += "-s {0} ".format(options.datafile)
if (options.update): commandline_base += "-u "
if (options.parameters): commandline_base += "-p "
if (options.verbose): commandline_base += "-v "


if __name__ == '__main__':
    print "Populating the CR database..."

    from pycrtools import crdatabase as crdb
    import subprocess

    db_filename = args[0]
    dbp = CRDatabasePopulator(db_filename=db_filename, options=options)

    # Create a list of all files
    dbp.getDiskFileList()

    # Populate database in a 'single thread'
    for f in dbp.filename_list:
        call_populate_script(f)
