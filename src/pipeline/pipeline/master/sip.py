import sys, datetime, logging
from pipeline.support.lofarrecipe import LOFARrecipe
from pipeline.support.lofaringredient import LOFARinput, LOFARoutput
import pipeline.support.utilities as utilities

class sip(LOFARrecipe):
    """
    The LOFAR Standard Imaging Pipeline.
    """
    def go(self):
        super(sip, self).go()

        # Set up logging to file
        handler = logging.FileHandler('%s/pipeline.log.%s' % (
                self.config.get("layout", "log_directory"),
                str(datetime.datetime.now())
            )
        )
        formatter = logging.Formatter(
            "%(asctime)s - %(levelname)s - %(name)s:  %(message)s",
            "%Y-%m-%d %H:%M:%S"
        )
        handler.setFormatter(formatter)
        self.logger.addHandler(handler)

        self.logger.info("Standard Imaging Pipeline starting.")

        self.logger.info("Reading VDS file")
        inputs = LOFARinput(self.inputs)
        inputs['gvds'] = self.config.get("layout", "gvds")
        outputs = LOFARoutput()
        if self.cook_recipe('vdsreader', inputs, outputs):
            self.logger.warn("vdsreader reports failure")
            return 1
        ms_names = outputs['ms_names']

        self.logger.info("Copying data to compute nodes")
        inputs = LOFARinput(self.inputs)
        inputs['args'] = ms_names
        outputs = LOFARoutput()
        if self.cook_recipe('copier', inputs, outputs):
            self.logger.warn("copier reports failure")
            return 1
        ms_names = self.outputs['ms_names']

        self.logger.info("Calling DPPP")
        inputs = LOFARinput(self.inputs)
        inputs['args'] = ms_names
        outputs = LOFARoutput()
        if self.cook_recipe('dppp', inputs, outputs):
            self.logger.warn("DPPP reports failure")
            return 1
        processed_filenames = outputs['filenames']

        self.logger.info("Calling BBS")
        inputs = LOFARinput(self.inputs)
        inputs['force'] = True
        outputs = LOFARoutput()
        if self.cook_recipe('bbs', inputs, outputs):
            self.logger.warn("BBS reports failure")
            return 1

        # Trim off any bad section of the data
        self.logger.info("Calling local flagger")
        inputs = LOFARinput(self.inputs)
        inputs['args'] = processed_filenames
        inoputs['n_factor'] = 2.0
        inputs['suffix'] = ".flagged"
        outputs = LOFARoutput()
        if self.cook_recipe('flagger', inputs, outputs):
            self.logger.warn("flagger reports failure")
            return 1
        flagged_outnames = outputs['data']

        self.logger.info("Calling vdsmaker")
        inputs = LOFARinput(self.inputs)
        inputs['directory'] = self.config.get('layout', 'vds_directory')
        inputs['gds'] = "flagged.gds"
        inputs['args'] = flagged_outnames
        outputs = LOFARoutput()
        if self.cook_recipe('vdsmaker', inputs, outputs):
            self.logger.warn("vdsmaker reports failure")
            return 1
        else:
            self.outputs['gds'] = outputs['gds']
        flagged_gds = outputs['gds']

        self.logger.info("Calling MWImager")
        inputs = LOFARinput(self.inputs)
        inputs['gvds'] = flagged_gds
        outputs = LOFARoutput()
        if self.cook_recipe('mwimager', inputs, outputs):
            self.logger.warn("MWImager reports failure")
            return 1
        # The SIP will return the names of the generated images
        self.outputs['images'] = outputs['images']

        self.logger.info("Calling source extractor")
        inputs = LOFARinput(self.inputs)
        inputs['args'] = outputs['images']
        outputs = LOFARoutput()
        if self.cook_recipe('sextractor', inputs, outputs):
            self.logger.warn("sextractor reports failure")
            return 1

        return 0

if __name__ == '__main__':
    sys.exit(sip().main())
