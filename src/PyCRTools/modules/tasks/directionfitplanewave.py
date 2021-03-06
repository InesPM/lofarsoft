"""
.. moduleauthor:: Arthur Corstanje <a.corstanje@astro.ru.nl>
"""
import pycrtools as cr
from pycrtools import tasks
from pycrtools import rftools as rf
import matplotlib as mpl
from mpl_toolkits.mplot3d import Axes3D
import pytmf
import numpy as np

deg = np.pi / 180.
pi2 = np.pi / 2.


class DirectionFitPlaneWave(tasks.Task):
    """ Fit a plane wave through arrival times and positions.
        Taken from module srcfind.
        Parameters taken from DirectionFitTriangles.

        Outlier removal and iteration to be implemented.
    """

    parameters = dict(
        # INPUT
        positions=dict(doc="hArray with Cartesian coordinates of the antenna positions",
                       unit="m"),

        timelags=dict(doc="hArray with the measured time lags for each event and each antenna",
                      unit="s"),

        good_antennas=dict(doc="List with indices of antennas to be used for the plane fit. This should already exclude stations that were flagged or do not show a reliable pulse"),

        reference_antenna=dict(doc="Index of antenna with respect to whom the timelags are given. If not given the first antenna is used as reference."),

        doplot=dict(default=False,
                    doc="Plot results."),

        verbose=dict(default=True,
                     doc="Print progress information."),

        rmsfactor=dict(default=2.0,
                       doc="How many sigma (times RMS) above the average can a delay deviate from the expected timelag (from latest fit iteration) before it is considered bad and removed as outlier."),


        # OUTPUT
        residual_delays=dict(default=lambda self: cr.hArray(float, [self.NAnt], name="Residual Delays"),
                             doc="Residual delays needed to calibrate the array after correction for known cable delays. Will be subtracted from the measured lags (correced for cable delays) or added to the expected. The array will be updated during iteration",
                             unit="s"),

        total_delays=dict(default=lambda self: cr.hArray(float, [self.NAnt], name="Total Delays"),
                          doc="Total instrumental (residual+cable) delays needed to calibrate the array. Will be subtracted from the measured lags or added to the expected. The array will be updated during iteration",
                          unit="s"),

        NAnt=dict(default=lambda self: self.timelags.shape()[-1],
                  doc="Number of antennas and time lags. If set explicitly, take only the first NAnt antennas from ``Task.positions`` and ``Task.timelags``."),

        refant=dict(default=0,
                    doc="Reference antenna for which geometric delay is zero."),

        meandirection=dict(default=lambda self: cr.hArray(float, [3]),
                           doc="Cartesian coordinates of mean direction from all good triangles",
                           output=True),

        meandirection_spherical=dict(default=lambda self: pytmf.cartesian2spherical(self.meandirection[0], self.meandirection[1], self.meandirection[2]),
                                     doc="Mean direction in spherical coordinates."),

        meandirection_azel=dict(default=lambda self: (np.pi - (self.meandirection_spherical[2] + pi2), pi2 - (self.meandirection_spherical[1])),
                                doc="Mean direction as Azimuth (``N->E``), Elevation tuple."),

        meandirection_azel_deg=dict(default=lambda self: (180 - (self.meandirection_spherical[2] + pi2) / deg, 90 - (self.meandirection_spherical[1]) / deg),
                                    doc="Mean direction as Azimuth (``N->E``), Elevation tuple in degrees."),

        fit_failed=dict(default=lambda self: False,
                        doc="Indication if fit failed",
                        output=True),

        goodcount = dict(doc="Number of good antennas.")


        #        plot_finish = dict(default=lambda self:plotfinish(dopause=False,plotpause=False),
        #                           doc="Function to be called after each plot to determine whether to pause or not (see :func:`plotfinish`)"),
        #        plot_name = dict(default="",
        #                         doc="Extra name to be added to plot filename.")
        )

    def call(self):
        pass

    def run(self):
        from pycrtools import srcfind
        from numpy import cos, sin
        print "Running PlaneWaveFit ..."
        self.farfield = True
        c = 299792458.0  # speed of light in m/s
        rad2deg = 180.0 / np.pi

        if isinstance(self.positions,np.ndarray):
            positions = self.positions
        else:
            positions = self.positions.toNumpy()
        if isinstance( self.timelags,np.ndarray):
            times = self.timelags
        else:
            times = self.timelags.toNumpy()

        # Allow for self.positions to be specified either as flat array or as 2D-array with shape (NAnt, 3)
#        import pdb; pdb.set_trace()
        if len(positions.shape) == 1:
            positions = positions.reshape(len(positions) / 3, 3)

        if not self.reference_antenna:
            times -= times[0]

        if self.good_antennas:
            indicesOfGoodAntennas = np.array(self.good_antennas)
            goodSubset = np.array(self.good_antennas)  # start with all 'good'

        else:
            indicesOfGoodAntennas = np.arange(len(times))
            goodSubset = np.arange(len(times))  # start with all 'good'

        self.goodcount = len(goodSubset)

        niter = 0
        while True:
            niter += 1
            # if fit only remains with three antennas (or less) it should not be trusted as it always has a solution (fails)
            if self.goodcount < 3:
                print "ERROR: too few good antennas for direction fit."
                self.meandirection = cr.hArray((0., 0., 1.))
                self.fit_failed = True
                break

            goodpositions = positions[indicesOfGoodAntennas].ravel()
            goodtimes = times[indicesOfGoodAntennas]
            (az, el) = srcfind.directionForHorizontalArray(goodpositions, goodtimes)
            if np.isnan(el) or np.isnan(az):
                print 'WARNING: plane wave fit returns NaN. Setting elevation to 0.0'
                el = np.deg2rad(40)  # need to propagate the warning...!
                self.fit_failed = True
            else:
                self.fit_failed = False

            # get residuals
            expectedDelays = srcfind.timeDelaysFromDirection(goodpositions, (az, el))
            expectedDelays -= expectedDelays[0]
            self.residual_delays = goodtimes - expectedDelays

            if self.fit_failed:
                bins = int((self.residual_delays.max()-self.residual_delays.min())*c/(positions[:,0].max()-positions[:,0].min()))
                if bins < 1:
                    bins = 1
                hist, edges = np.histogram(self.residual_delays,bins=bins)

                max_time = np.argmax(hist)
                print "histogram filled", hist
                print "edges", edges
                # fix for first and last bin
                print "maximum at", max_time
                try:
                    upper = edges[max_time+2]
                except:
                    upper = edges[edges.shape[0]-1]
                    print "upper exception"
                try:
                    lower = edges[max_time]

                except:
                    print "lower exception"
                    lower = edges[0]

                print "selecting between lower ",lower, " and upper", upper
                goodSubset = np.where((self.residual_delays > lower)&(self.residual_delays < upper))
            else:
                # remove > k-sigma outliers and iterate
                spread = np.std(self.residual_delays)
                k = self.rmsfactor
                goodSubset = np.where(abs(self.residual_delays - np.mean(self.residual_delays)) < k * spread)
            # gives subset of 'indicesOfGoodAntennas' that is 'good' after this iteration
            if self.verbose:
                print 'iteration # %d' % niter
                print 'az = %f, el = %f' % (az * rad2deg, el * rad2deg)
                print 'good count = %d' % self.goodcount
            # if the next iteration has the same number of good antenneas the while loop will be terminated
            if len(goodSubset[0]) == self.goodcount:
                break
            else:
                self.goodcount = len(goodSubset[0])
                indicesOfGoodAntennas = indicesOfGoodAntennas[goodSubset]

        cartesianDirection = [cos(el) * sin(az), cos(el) * cos(az), sin(el)]
        self.meandirection.copy(cr.hArray(cartesianDirection))

        if self.fit_failed != True:
            # NB! Have to update the dependent parameters, or the previous value will come out.
            self.ws.updateParameter("meandirection_spherical", forced=True)
            self.ws.updateParameter("meandirection_azel_deg", forced=True)
            self.ws.updateParameter("meandirection_azel", forced=True)

        # now redo arrays for full antenna set
        expectedDelays = srcfind.timeDelaysFromDirection(positions.ravel(), (az, el))  # need positions as flat 1-D array
        expectedDelays -= expectedDelays[0]  # subtract ref ant
        self.residual_delays = times - expectedDelays
        self.residual_delays -= np.median(self.residual_delays)
        self.delta_delays_mean_history = [np.mean(self.residual_delays)]  # comply with DirectionFitTriangles, return as list...
        self.delta_delays_rms_history = [np.std(self.residual_delays)]
        self.residual_delays = cr.hArray(self.residual_delays)

        if self.doplot:
            import matplotlib.pyplot as plt
            plt.figure()
            plt.plot(self.residual_delays)
            plt.figure()
    #        cr.trerun("PlotAntennaLayout","Delsdays",positions = cr.hArray(goodpositions), colors=cr.hArray(self.residual_delays), sizes=100,title="Residuals for plane wave fit",plotlegend=True)
    #        plt.figure()
