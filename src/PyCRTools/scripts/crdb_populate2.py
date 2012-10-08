#! /usr/bin/env python
"""
Description of this script
"""
import os
import time
import re
import pickle
from optparse import OptionParser

args = None
options = None


class CRDatabasePopulator(object):
    """"""

    def __init__(self, dbFilename=None, dataFilename=None):
        """Initialization of CRDatabasePopulator object.

        **Properties**

        ==============  ==================================================
        Parameter       Description
        ==============  ==================================================
        *dbFilename*    Name of the database file.
        *dataFilename*  Name of the datafile.
        ==============  ==================================================
        """
        if not dbFilename:
            raise ValueError("No database filename provided.")
        self.dbFilename = dbFilename

        if not dataFilename:
            raise ValueError("No data filename provided.")
        self.dataFilename =  dataFilename

        self.dbManager = crdb.CRDatabase(filename=dbFilename,
                                         datapath=options.datapath,
                                         resultspath=options.resultspath,
                                         lorapath=options.lorapath)
        self.db = self.dbManager.db
        if not self.db:
            raise ValueError("Unable to open database.")

        self.newID = {}
        self.newID['event'] = -1
        self.newID['datafile'] = -1
        self.newID['station'] = -1
        self.newID['polarization'] = -1

        self.ID = {}
        self.ID['event'] = -1
        self.ID['datafile'] = -1
        self.ID['station'] = -1
        self.ID['polarization'] = -1

        self.sqlList = []


    def populate(self):
        """Populate the database with information from the selected datafile.
        """
        # Initialize data extractor
        filename_full = options.datapath + "/" + dataFilename
        dx = None
        dx = DataExtractor(filename_full)
        if not dx.isOpen():
            raise IOError("Datafile can not be opened")

        # Check if datafile is not already in database
        sql = "SELECT * FROM main.datafiles WHERE filename='{0}';".format(self.dataFilename)
        record = self.db.select(sql)
        if record:
            print "Datafile is already in the database."
            return

        # Retrieve new IDs for database entries
        for tableName in ['event', 'datafile', 'station', 'polarization']:
            sql = "SELECT max({0}ID) FROM main.{0}s;".format(tableName)
            records = self.db.select(sql)
            if records and records[0] and records[0][0]:
                self.newID[tableName] = int(records[0][0]) + 1
            else:
                self.newID[tableName] = 1
            # self.ID[tableName] = self.newID[tableName]


        # ______________________________________________________________________________
        #                                                                       Datafile

        # Write datafile info to list of SQL statements
        self.ID['datafile'] = self.newID['datafile']
        self.newID['datafile'] += 1
        sql = "INSERT INTO main.datafiles (datafileID, filename, status) VALUES ({0}, '{1}', '{2}');".format(self.ID['datafile'], self.dataFilename, 'NEW')
        self.sqlList.append(sql)

        # Write datafile parameters to list of SQL statements
        # Disabled as there are at the moment no station parameters
        # if options.parameters:
        #     datafileParameters = CRParameter(ID=self.ID['datafile'], parentname='datafile')
        #     sql = datafileParameters.writeSql()
        #     self.sqlList.append(sql)

        # ______________________________________________________________________________
        #                                                                          Event

        # Find corresponding eventID and check if this is lower than self.eventID
        timestamp = dx.timestamp
        eventIDs = self.dbManager.getEventIDs(timestamp=timestamp)
        if eventIDs:
            self.ID['event'] = eventIDs[0]
        else:
            self.ID['event'] = self.newID['event']
            self.newID['event'] += 1

            # Write event info to list of SQL statements
            sql = "INSERT INTO main.events (eventID, timestamp, status) VALUES ({0}, {1}, '{2}');".format(self.ID['event'], timestamp, 'NEW')
            self.sqlList.append(sql)

            # Add LORA data to event parameters
            eventParameters = CRParameter(ID=self.ID['event'], parentname='event')
            if options.lorapath != "":
                print "Collecting parameters..."
                process_lora_data(timestamp, eventParameters, options.lorapath)

                # Write event parameters to list of SQL statements
                sql = eventParameters.writeSql()
                self.sqlList.append(sql)

        # Write Event-Datafile table SQL
        sql = "INSERT INTO main.event_datafile (eventID, datafileID) VALUES ({0}, {1});".format(self.ID['event'], self.ID['datafile'])
        self.sqlList.append(sql)


        # ______________________________________________________________________________
        #                                                                        Station

        for stationname in dx.stationnames:
            # Write station info to list of SQL statements
            self.ID['station'] = self.newID['station']
            self.newID['station'] += 1
            sql = "INSERT INTO main.stations (stationID, stationname, status) VALUES ({0},'{1}','{2}');".format(self.ID['station'],stationname, 'NEW')
            self.sqlList.append(sql)

            # Write station parameters to list of SQL statements
            # Disabled as there are at the moment no station parameters
            # if options.parameters:
            #     stationParameters = CRParameter(ID=self.ID['station'], parentname='station')
            #     sql = stationparameters.writeSql()
            #     self.sqlList.append(sql)

            # write Datafile-Station table SQL
            sql = "INSERT INTO main.datafile_station (datafileID, stationID) VALUES ({0}, {1});".format(self.ID['datafile'], self.ID['station'])
            self.sqlList.append(sql)


            # __________________________________________________________________________
            #                                                               Polarization

            for pol_direction in ['0', '1', 'xyz']:
                self.ID['polarization'] = self.newID['polarization']
                self.newID['polarization'] += 1

                # Write polarization info to list of SQL statements
                antennaset = str(dx.antennaset)
                resultsfile = str(dx.resultsfile(pol_direction))
                sql = "INSERT INTO main.polarizations (polarizationID, antennaset, direction, status, resultsfile) VALUES ({0}, '{1}', '{2}', '{3}', '{4}');".format(self.ID['polarization'], antennaset, pol_direction, 'NEW', resultsfile)
                self.sqlList.append(sql)

                # Write polarization parameters to list of SQL statements
                if options.parameters:
                    polarizationParameters = CRParameter(ID=self.ID['polarization'], parentname='polarization')
                    results_filename = os.path.join(options.resultspath, resultsfile)
                    if os.path.isfile(results_filename):
                        parameters = xmldict.load(results_filename)
                        for key in parameters.keys():
                            polarizationParameters[key] = parameters[key]
                    sql = polarizationParameters.writeSql()
                    self.sqlList.append(sql)

                # Write Station-Polarization table SQL
                sql = "INSERT INTO main.station_polarization (stationID, polarizationID) VALUES ({0}, {1});".format(self.ID['station'], self.ID['polarization'])
                self.sqlList.append(sql)


        if options.verbose:
            self.summary()
        self.writeDatabase()


    def writeDatabase(self):
        """Write list of sql statements to the database."""
        if options.verbose:
            print "Write database (%d)" %(len(self.sqlList))

        if self.sqlList:
            if options.verbose:
                print "  Validating SQL statements..."
            for i in range(len(self.sqlList)):
                if self.sqlList[i][-1] != ';': self.sqlList[i] += ';'

            if self.db:
                if options.verbose:
                    print "Nr. of SQL lines: %d" %(len(self.sqlList))
                if options.sqlverbose:
                    for sql in self.sqlList:
                        print sql
                self.db.executelist(self.sqlList)


    def summary(self):
        """Summary of the CRDatabasePopulator object"""
        linewidth = 80

        print "="*linewidth
        print "  Summary of the CRDatabasePopulator object"
        print "="*linewidth

        print "  %38s : %s" %("database filename",self.dbFilename)
        print "  %38s : %s" %("data filename",self.dataFilename)

        print "-"*linewidth

        print "  %38s : %s" %("new EventID", self.newID['event'])
        print "  %38s : %s" %("new DatafileID", self.newID['datafile'])
        print "  %38s : %s" %("new StationID", self.newID['station'])
        print "  %38s : %s" %("new PolarizationID", self.newID['polarization'])

        print "="*linewidth



class CRParameter(object):
    """Class for objects containing extra parameter information for a
    CR information object (event, datafile, station, polarization).
    """

    def __init__(self, ID=-1, parentname="", db=None):
        """Initialize the CRParameter object"""
        self._parameters = {}
        self._db = db

        if ID < 0:
            raise ValueError("Invalid ID number  (ID<0)")

        if not parentname in ['event','datafile','station','polarization']:
            raise ValueError("Invalid parent name.")

        self._parentname = parentname
        self._tablename = parentname+"parameters"
        self._id = ID
        self._idlabel = parentname+"ID"


    def __getitem__(self, key):
        """Get the value of the parameter *key*."""
        return self._parameters[key]


    def __setitem__(self, key, value):
        """Set the value of the parameter *key* to *value*."""
        self._parameters[key] = value


    def __delitem__(self, key):
        """Empty the entry *key* from the list of parameters."""
        self._parameters[key] = None


    def pickle_parameter(self, value):
        """Return the parameter as a database friendly pickle
        representation.

        **Properties**

        ==============  ======================================================
        Parameter       Description
        ==============  ======================================================
        *value*         Python representation of the value of the parameter.
        ==============  ======================================================
        """
        return re.sub("'", '"', pickle.dumps(value))


    def unpickle_parameter(self, value):
        """Return the parameter value from parsing a database friendly
        representation of the parameter.

        **Properties**

        ==============  ================================================
        Parameter       Description
        ==============  ================================================
        *value*         Database representation of the parameter value.
        ==============  ================================================
        """
        return pickle.loads(re.sub('"', "'", str(value)))


    def write(self):
        """Write parameters to the database."""
        raise NotImplementedError("Functionality needs to be implemented.")

        if self._db:
            sql = self.writeSql()
            self._db.execute()


    def writeSql(self):
        """Construct an SQL statement to add or replace parameter information."""
        sql = ""
        sql_keys = "{0}".format(self._idlabel)
        sql_values = "{0}".format(self._id)
        for key in self._parameters.keys():
            sql_keys += ", {0}".format(key)
            sql_values += ", '{0}'".format(self.pickle_parameter(self._parameters[key]))
        sql = "INSERT OR REPLACE INTO {0} ({1}) VALUES ({2});".format(self._parentname+"parameters", sql_keys, sql_values)

        return sql


    def summary(self):
        """Summarize the CRParameter object."""
        linewidth = 80

        print "="*linewidth
        print "  Summarize the CRParameter object"
        print "="*linewidth

        print "  %-38s : %s" %("Parent name", self._parentname)
        print "  %-38s : %s" %("Table name", self._tablename)
        print "  %-38s : %s" %("ID", self._id)
        print "  %-38s : %s" %("ID label", self._idlabel)

        print "-"*linewidth

        for key in self._parameters.keys():
            print "  %-38s : %s" %(key, self._parameters[key])

        print "="*linewidth



def process_lora_data(timestamp, parameters, lorapath):
    """Add LORA specific information to the event information in the database.

    **Properties**

    ============  =======================================================
    Parameter     Description
    ============  =======================================================
    *event*       event object in which to store the LORA information.
    *lorapath*    Path where the LORA information files are found.
    ============  =======================================================
    """
    if options.verbose:
        print "Adding LORA parameters..."

    lora_data = lora.loraInfo(timestamp, lorapath)
    if lora_data:
        for key in lora_data.keys():
            lora_key = "lora_" + key
            parameters[lora_key] = lora_data[key]
    else:
        print("Empty lora_data set...")



class DataExtractor(object):
    """Class to extract specific data from an hdf5 cr datafile."""

    def __init__(self, filename=""):
        """Initialisation of the DataExtractor object.

        **Properties**

        ==========  ==========================================
        Parameter   Description
        ==========  ==========================================
        *filename*  filename of the VHECR data file.
        ==========  ==========================================
        """
        if options.verbose:
            print "  Initializing DataExtractor object..."

        self._filename = filename
        self._datafile = None

        self.open()


    def open(self):
        """Open a VHECR data file."""
        if options.verbose:
            print "  Opening VHECR datafile '{0}'...".format(self._filename)

        if not self._datafile:
            if "" != self._filename:
                try:
                    self._datafile = cr.open(self._filename)
                except:
                    self._datafile = None
                    print "WARNING: problem when opening file {0}".format(self._filename)
            else:
                raise ValueError("No filename provided!")


    def isOpen(self):
        """Check if the datafile is open."""
        if self._datafile:
            result = True
        else:
            result = False

        return result


    @property
    def timestamp(self):
        """Timestamp of the event."""
        result = 0

        # if options.verbose:
        #     print "  Retrieving timestamp of the event..."

        if self.isOpen():
            seconds = self._datafile["TIME"][0]
            subsec  = self._datafile["SAMPLE_NUMBER"][0] * self._datafile["SAMPLE_INTERVAL"][0]
            result = int(seconds) # + round(subsec + 0.5))
        else:
            raise ValueError("No open datafile")

        return result


    @property
    def stationnames(self):
        """List of station names."""
        result = []

        # if options.verbose:
        #     print "  Retrieving station names..."

        if self.isOpen():
            result = list(set(self._datafile["STATION_NAME"]))

        return result


    @property
    def antennaset(self):
        """Name of the used antennaset in the datafile."""
        result = ""

        # if options.verbose:
        #     print "  Retrieving antennaset information..."

        if self.isOpen():
            result = self._datafile["ANTENNA_SET"]

        return result


    def resultsfile(self, pol='0'):
        """Name of the resultsfile where the pipeline results are stored.

        This filename is derived from metadata in the datafile.

        **Properties**

        =========  ===================================================
        Parameter  Datafile
        =========  ===================================================
        *pol*      polarization direction.
        =========  ===================================================
        """
        result = ""

        # if options.verbose:
        #     print "  Retrieving polarization information..."

        if self.isOpen():
            # The filename of the resultsfile is created/derived in the same way as in cr_event.py
            root_pathname = "VHECR_LORA-"

            # Timestamp
            time_s      = self._datafile["TIME"][0]
            time_s_str  = time.strftime("%Y%m%dT%H%M%S",time.gmtime(time_s))
            time_ms     = int(self._datafile["SAMPLE_INTERVAL"][0]*self._datafile["SAMPLE_NUMBER"][0]*1000)
            time_ms_str = str(time_ms).zfill(3)
            time_stamp  = time_s_str+"."+time_ms_str+"Z"

            polname = "pol%s" %(pol)

            stationname = self.stationnames[0]

            rel_pathname = os.path.join(root_pathname+time_stamp, polname, stationname)
            rel_filename = os.path.join(rel_pathname, "results.xml")

            result = rel_filename

        return result


    def summary(self):
        """Summary of the DataExtractor object."""
        linewidth = 80

        print "="*linewidth
        print "  Summary of the DataExtractor object."
        print "="*linewidth

        print "  %-40s : %s" %("Filename", self._filename)

        print "="*linewidth



def parseOptions():
    """Parse options from the command line."""

    usage = "usage: %prog [options] <databasefile> <datafile>"
    parser = OptionParser(usage=usage)

    # parser.add_option("-a", "--archive", type="str", dest="archivepath", default="", help="directory where the archive can be found.")
    parser.add_option("-d", "--datapath", type="str", dest="datapath", default="", help="directory where the datafiles can be found.")
    parser.add_option("-r", "--resultspath", type="str", dest="resultspath", default="", help="directory where the results files are written.")
    parser.add_option("-l", "--lorapath", type="str", dest="lorapath", default="", help="directory where the lora files are written.")
    # parser.add_option("-u", "--update", action="store_true", dest="update", default=False, help="Update instead of populating database.")
    # parser.add_option("-s", "--hdf5", type="str", dest="datafile", default="", help="name of a single hdf5 file that needs to be registered in the database.")
    parser.add_option("-p", "--parameters", action="store_true", dest="parameters", default=False, help="Enable storing polarization parameter information if available [default=False].")

    parser.add_option("-v", "--verbose", action="store_true", dest="verbose", default=False, help="Show verbose information [default=False].")
    parser.add_option("--sqlverbose", action="store_true", dest="sqlverbose", default=False, help="Show verbose SQL information [default=False].")
    parser.add_option("-f", "--force", action="store_true", dest="force", default=False, help="Force writing to the database resetting the write-lock [default=False].")

    (options, args) = parser.parse_args()

    if len(args) != 2:
        parser.error("Incorrect number of arguments.")

    options.basepath = os.path.dirname(os.path.abspath(args[0]))

    print "datapath = %s" %(options.datapath)
    print "resultspath = %s" %(options.resultspath)
    print "lorapath = %s" %(options.lorapath)
    print "parameters = ", options.parameters

    return (options, args)



if __name__ == '__main__':
    print "Adding information from datafile to the CR database..."

    (options, args) = parseOptions()

    import pycrtools as cr
    from pycrtools import crdatabase as crdb
    from pycrtools import xmldict
    from pycrtools import lora

    dbFilename = args[0]
    dataFilename = args[1]

    populator = CRDatabasePopulator(dbFilename=dbFilename, dataFilename=dataFilename)

    populator.populate()

    pass
