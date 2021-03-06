#! /usr/bin/env python

"""This script runs the automatic pipeline (cr_event) on the LORA triggered LOFAR VHECR data.
"""

import os
import multiprocessing
import subprocess
from pycrtools import crdatabase as crdb

from optparse import OptionParser

# Parse commandline options
parser = OptionParser()
parser.add_option("-d", "--database", default="cr.db", help = "filename of database")
parser.add_option("--max-threads", default = 1, type = int, help = "maximum number of threads to use.")
parser.add_option("--log-dir", default = "./log", help = "directory to store logs.")
parser.add_option("--output-dir", default = "./results", help = "directory to store pipeline output.")

(options, args) = parser.parse_args()

db_filename = options.database
dbManager = crdb.CRDatabase(db_filename)
db = dbManager.db

event_ids = dbManager.getEventIDs(status = "CR_FOUND")

def call_pipeline(event_id):
    """Function that actually calls the subprocess for each event.
    """

    print "processing event", event_id

    with open(options.log_dir+"/"+"cr_physics-"+str(event_id)+".log", "w", buffering = 1) as f:

        command = "python "+os.environ["LOFARSOFT"]+"/src/PyCRTools/pipelines/cr_physics.py --id="+str(event_id)+" --database="+options.database + " --output-dir="+options.output_dir

        print  command

        status = subprocess.call(command, stdout=f, stderr=subprocess.STDOUT, shell=True)

# Check how many CPU's we have
count = multiprocessing.cpu_count()

print "Using", min(options.max_threads, count), "out of", count, "available CPU's."

# Create a thread pool
pool = multiprocessing.Pool(processes=min(options.max_threads,count))

# Execute the pipeline for each file
pool.map(call_pipeline, event_ids)

