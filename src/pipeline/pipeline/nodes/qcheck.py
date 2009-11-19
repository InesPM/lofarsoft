from __future__ import with_statement
import os, imp, logging

from pipeline.support.lofarnode import LOFARnode
from pipeline.support.utilities import log_time

class qcheck_node(LOFARnode):
    """
    Run quality check modules on an image.
    """
    def run(self, infile, pluginlist, logfile):
        with log_time(self.logger):
            self.logger.info("Processing: %s" % (infile))

            file_handler = logging.FileHandler(logfile, mode='w') 
            file_handler.setFormatter(logging.Formatter("%(message)s"))
            file_logger = logging.getLogger('main')
            file_logger.addHandler(file_handler)
            file_logger.setLevel(logging.INFO)
            pipeline_logger = logging.getLogger(self.logger.name + ".qcheck")
            pipeline_logger.setLevel(logging.WARN)

            loggers = {'main': file_logger, 'warn': pipeline_logger}

            for plugin in pluginlist:
                try:
                    qcheck = imp.load_source('qcheck', plugin)
                    print qcheck
                except ImportError:
                    self.logger.warn("Quality check module (%s) not found" % (plugin))
                    break
                try:
                    qcheck.run(infile, loggers=loggers)
                except Exception, e:
                    self.logger.warn("Quality check failed on %s" % (infile))
                    self.logger.exception(str(e))

        return 0
