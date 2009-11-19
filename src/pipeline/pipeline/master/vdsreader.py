# Local helpers
from pipeline.support.lofarrecipe import LOFARrecipe
import pipeline.support.utilities as utilities

class vdsreader(LOFARrecipe):
    """
    Read a GVDS file and return a list of the MS filenames referenced therein.
    """
    def __init__(self):
        super(vdsreader, self).__init__()
        self.optionparser.add_option(
            '-g', '--g(v)ds-file',
            dest="gvds",
            help="G(V)DS file describing data to be processed"
        )


    def go(self):
        self.logger.info("Starting vdsreader run")
        try:
            gvds = utilities.get_parset(self.inputs['gvds'])
        except:
            self.logger.error("Unable to read G(V)DS file")
            raise
        
        self.logger.inf("Building list of measurementsets")
        ms_names = (
            gvds["Part%d.FileName" % (part_no,)]
            for part_no in xrange(int(gvds["NParts"]))
        )
        self.logger.debug(ms_names)

        self.outputs['ms_names'] = ms_names
        return 0
