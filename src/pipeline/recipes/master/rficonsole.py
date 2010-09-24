#                                                         LOFAR IMAGING PIPELINE
#
#                                                  rficonsole (AOflagger) recipe
#                                                            John Swinbank, 2010
#                                                      swinbank@transientskp.org
# ------------------------------------------------------------------------------

from __future__ import with_statement

from contextlib import nested
from collections import defaultdict

import sys

import lofarpipe.support.utilities as utilities
import lofarpipe.support.lofaringredient as ingredient
from lofarpipe.support.baserecipe import BaseRecipe
from lofarpipe.support.remotecommand import RemoteCommandRecipeMixIn
from lofarpipe.support.remotecommand import ProcessLimiter
from lofarpipe.support.group_data import load_data_map

class rficonsole(BaseRecipe, RemoteCommandRecipeMixIn):
    inputs = {
        'executable': ingredient.ExecField(
            '--executable',
            default="/opt/LofIm/daily/lofar/bin/rficonsole",
            help="rficonsole executable"
        ),
        'nproc': ingredient.IntField(
            '--nproc',
            default=1,
            help="Maximum number of simultaneous processes per compute node"
        ),
        'nthreads': ingredient.IntField(
            '--nthreads',
            default=1,
            help="Number of threads per rficonsole process"
        ),
    }

    def go(self):
        self.logger.info("Starting rficonsole run")
        super(rficonsole, self).go()

        #                           Load file <-> compute node mapping from disk
        # ----------------------------------------------------------------------
        self.logger.debug("Loading map from %s" % self.inputs['args'])
        data = load_data_map(self.inputs['args'][0])

        #      We'll dispatch a single rficonsole job to each node, asking it to
        #                                    process all the data for that node.
        # ----------------------------------------------------------------------
        hostlist = defaultdict(list)
        for host, file in data:
            hostlist[host].append(file)

        #                               Limit number of process per compute node
        # ----------------------------------------------------------------------
        self.logger.debug("Limit to %s processes/node" % self.inputs['nproc'])
        compute_nodes_lock = ProcessLimiter(self.inputs['nproc'])

        command = "python %s" % (self.__file__.replace('master', 'nodes'))
        job_args = []
        for host, files in hostlist.iteritems():
            args = [
                host, command, compute_nodes_lock[host],
                self.inputs['executable'], self.inputs['nthreads']
            ]
            args.extend(hostlist[host])
            job_args.append(args)
        self._schedule_jobs(job_args)

        if self.error.isSet():
            self.logger.warn("Failed rficonsole process detected")
            return 1
        else:
            return 0

if __name__ == '__main__':
    sys.exit(rficonsole().main())
