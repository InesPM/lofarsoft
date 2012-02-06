"""
A number of tools useful in calibrating radio data

"""

import pycrtools as cr
from pycrtools import tasks
from pycrtools.tasks import shortcuts as sc
from pycrtools import rftools as rf
import matplotlib as mpl
from mpl_toolkits.mplot3d import Axes3D
import pytmf
import math
import numpy as np

deg=math.pi/180.
pi2=math.pi/2.


class LocatePulseTrain(tasks.Task):
    """
    **Usage**

    ``LocatePulseTrain()(timeseries_data[nantennas,blocklen],timeseries_data_sum[blocklen]=None,nsigma=7,maxgap=7,minpulselen=7) -> Task.start, Task.end, Task.time_series_cut``

    **Description**

    Finds the pulse train with the highest peak in time series data,
    determine its location, and cut out the time series data around
    the pulse. If the input array has multiple dimensions (multiple
    antennnas are present), then sum the antennas first and search the
    pulse in the sum of all antennas or look for the mean pulse
    location for all antennas.

    The task calculates noise and threshold level above which to find
    pulse trains. A pulse train can have gaps of some `N=maxgaps`
    samples which are below the threshold and yet are considered part
    of the pulse train (i.e., this is the maximum separation of
    individual pulses to be considered part of the pulse train.).

    With the parameter ``search_window`` the search can be limited to
    the range of samples between those two numbers.

    If the parameter ``timeseries_data_sum`` is provided (a 1D array
    with a time series, e.g. an incoherent beam of all antennas) then
    the peak will be searched in that time series and not
    recalculated.

    If ``search_per_antenna=True`` then the peaks are searched in each
    antenna. From all antennas where a peak was found the median peak
    location and length is taken.



   **Results:**
    Returns start and end index of the strongest pulse in
    ``Task.start`` and ``Task.end``. The cut-out time series is
    returned in ``Task.timeseries_data_cut``.

    The summed time series from all data sets (if a 2D array was
    provided) is returned in ``Task.timeseries_data_sum``

    For ``search_per_antenna=True`` the list ``Task.peaks_found_list``
    contains the indices of all antennas where a peak was found. The
    number of antennas with peaks is ``self.npeaks_found``. Start and
    end of the pulses for each antenna with peak are found are in
    ``self.maxsequences``. The smallest and largest start location of
    all antennas can be found in ``Task.start_min``,
    ``Task.start_max``. See also ``Task.lengths_min``,
    ``Task.lengths_max``, and ``Task.lengths_median`` for information
    on the width of these peaks.

    See :func:`hFindSequenceGreaterThan` for a description of the other
    parameters.

    **See also:**

    :func:`hFindSequenceGreaterThan`

    **Example**

    ::

        file=open("/Users/falcke/LOFAR/usg/data/lofar/oneshot_level4_CS017_19okt_no-9.h5") #
        file["BLOCKSIZE"]=2**int(round(math.log(file["DATA_LENGTH"][0],2)))
        file["SELECTED_DIPOLES"]=["017000001", "017000002", "017000005", "017000007", "017001009", "017001010", "017001012", "017001015", "017002017", "017002019", "017002020", "017002023", "017003025", "017003026", "017003029", "017003031", "017004033", "017004035", "017004037", "017004039", "017005041", "017005043", "017005045", "017005047", "017006049", "017006051", "017006053", "017006055", "017007057", "017007059", "017007061", "017007063", "017008065", "017008066", "017008069", "017008071", "017009073", "017009075", "017009077", "017009079", "017010081", "017010083", "017010085", "017010087", "017011089", "017011091", "017011093", "017011095"]
        pulse=trun('LocatePulseTrain',file["TIMESERIES_DATA"])
        #(pulse.start,pulse.end) -> (65806L, 65934L)
        pulse.timeseries_data_sum.plot(highlight=(pulse.start,pulse.end),nhighlight=1)
        pulse.timeseries_data_cut[0].plot()

    """
    parameters=dict(
        nblocks=dict(default=16, doc="The time series data is subdivided in ``nblock`` blocks where the one with the lowest RMS is used to determine the threshold level for finding peaks."),
        nsigma=dict(default=7, doc="Pulse has to be ``nsigma`` above the noise."),
        maxgap=dict(default=7, doc="Maximum gap length allowed in a pulse where data is not above the threshold"),
        minpulselen=dict(default=7, doc="Minimum width of pulse."),
        maxlen=dict(default=1024, doc="Maximum length allowed for the pulse train to be returned."),
        minlen=dict(default=32, doc="Minimum length allowed for cutting the time time series data."),
        prepulselen=dict(default=16, doc="Safety margin to add before the start of the pulse for cutting and ``start``."),
        docut=dict(default=True, doc="Cut the time series around the pulse and return it in ``timeseries_data_cut``."),
        cut_to_power_of_two=dict(default=True, doc="Cut the time series to a length which is a power of two?"),
        maxnpeaks=dict(default=256, doc="Naximum number of train peaks to look for in data"),
        start=dict(default=0, doc="Start index of the strongest pulse train",output=True),
        end=dict(default=32, doc="End index of the strongest pulse train",output=True),
        npeaks=dict(default=0, doc="Number of peaks found in data",output=True),
        maxima=dict(default=None, doc="Values of the maxima of the pulse trains",output=True),

        indexlist=dict(default=lambda self:cr.hArray(int,[self.nantennas if self.search_per_antenna else 1,self.maxnpeaks,2]), doc="Original locations of the peaks found in the data",output=True),

        cutlen=dict(default=None, doc="Length of the cut-out data.",output=True),
        timeseries_data_sum=dict(default=None, doc="Incoherent (squared) sum of all antennas."),
        timeseries_data_cut=dict(default=lambda self:cr.hArray(float,self.timeseries_data.shape()[:-1]+[self.cutlen]), doc="Contains the time series data cut out around the pulse.",output=True),

        timeseries_length = dict(default=lambda self:self.timeseries_data.shape()[-1], doc="Length of the full input time series."),

        nantennas = dict(default=lambda self:self.timeseries_data.shape()[-2] if len(self.timeseries_data.shape())>1 else 1, doc="Number of antennas in data file."),

        search_window = dict(default=False, doc="False - if set to a tuple with two integer indices (start, end), pointing to locations in the array, only the strongest peak between those two locations are being considered. Use -1 for start=0 or end=len(array)."),

        search_per_antenna = dict(default=False, doc="Search pulses per antennas and then return the average of all peaks found."),

        doplot = dict(default=False, doc="Produce output plots."),

        plot_finish = dict(default = lambda self:cr.plotfinish(filename=self.plot_filename,doplot=self.doplot,plotpause=True),
                       doc ="Function to be called after each plot to determine whether to pause or not (see ::func::plotfinish)"),

        plot_name = dict(default="", doc="Extra name to be added to plot filename."),

        plot_filename = dict(default="", doc="Base filename to store plot in.")

        )

    def call(self,timeseries_data):
        pass

    def run(self):
        self.npeaks = 0
        #Sum all antennas, if more than one antenna is present
        if not self.timeseries_data_sum:
            if len(self.timeseries_data.shape())==1:
                self.timeseries_data_sum=self.timeseries_data
            else:
                self.timeseries_data_sum=rf.TimeBeamIncoherent(self.timeseries_data)
                self.timeseries_data_sum.sqrt()

        self.minrms=cr.Vector(float,self.nantennas)
        self.minmean=cr.Vector(float,self.nantennas)
        self.blen=cr.Vector(int,self.nantennas,fill=max(self.timeseries_length/self.nblocks,8))
        if self.search_window:
            if hasattr(self.search_window,"__getitem__") and len(self.search_window)==2 and isinstance(self.search_window[0],(long,int)) and isinstance(self.search_window[1],(long,int)):
                self.search_window_x0=max(min(len(self.timeseries_data_sum) - 1,self.search_window[0]),0) if self.search_window[0]>=0 else 0
                self.search_window_x1=max(min(len(self.timeseries_data_sum) - 1,self.search_window[1]),1) if self.search_window[1]>0 else len(self.timeseries_data_sum) - 1
            else:
                raise ValueError("ERROR: LocatePulseTrain.search_window needs to be a tuple or list of two integers.")
        else:
            self.search_window_x0=0;
            self.search_window_x1=self.timeseries_length - 1

        if self.search_per_antenna:
            self.timeseries_data[...].minstddevblock(self.blen,self.minrms,self.minmean)
            self.npeaks_list=self.indexlist[...].findsequencegreaterthan(self.timeseries_data[...,[self.search_window_x0]:[self.search_window_x1]],self.minmean+self.nsigma*self.minrms,cr.asvec(self.maxgap),cr.asvec(self.minpulselen))
            self.peaks_found_list=self.npeaks_list.Find(">",0)
            self.peaks_found_list=list(self.peaks_found_list)
            self.npeaks=len(self.peaks_found_list)
            if self.npeaks <= 0:
                print "LocatePulesTrain: No pulses found in any antenna!"
                return
            self.npeaks_greater_zero=cr.asvec(cr.hArray(self.npeaks_list)[self.peaks_found_list])
            self.indexlist[self.peaks_found_list,...,[0]:self.npeaks_greater_zero] += self.search_window_x0
            self.maxima=cr.hArray(float,[self.nantennas,max(self.npeaks_list.max(),1)],fill=-99)
            self.maxima[...].maxinsequences(self.timeseries_data[...],self.indexlist[...],self.npeaks_list) # get the maxima in each sequence
            self.maxpos=self.maxima[self.peaks_found_list,...,[0]:self.npeaks_greater_zero].maxpos() # find the strongest maximum of all sequences (per antenna), (i.e. the nth sequence contains the strongest peak)
            self.maxsequences=cr.hArray(int,[self.npeaks,2])  # get the list of the start/end indices for the strongest maxima in each antenna
            self.maxsequences[...].copy(self.indexlist[self.peaks_found_list,...,self.maxpos:self.maxpos+1])
            self.maxstarts=self.maxsequences[...].elem(0) # get the start positions
            self.sequence_lengths=cr.hArray(int,[self.npeaks,1])
            self.sequence_lengths[...].sub(self.maxsequences[...,1],self.maxsequences[...,0])
            self.start_min=self.maxstarts.min()
            self.start_max=self.maxstarts.max()
            self.lengths_min=self.sequence_lengths.min().val()
            self.lengths_max=self.sequence_lengths.max().val()
            self.lengths_median=self.sequence_lengths.median()
            self.start=self.maxstarts.median()
            self.start_rms=self.maxstarts.stddev(self.start)
            self.end=self.start+self.lengths_median
        else:
            self.timeseries_data_sum.minstddevblock(self.blen,self.minrms,self.minmean)
            self.npeaks=self.indexlist[0].findsequencegreaterthan(self.timeseries_data_sum[self.search_window_x0:self.search_window_x1],
                                                                  self.minmean[0]+self.nsigma*self.minrms[0],self.maxgap,self.minpulselen).first()
            if self.npeaks <= 0:
                print "LocatePulesTrain: No pulses found!"
                return
            self.indexlist += self.search_window_x0
            self.maxima=cr.hArray(float,[1,self.npeaks])
            self.maxima.maxinsequences(self.timeseries_data_sum,self.indexlist,self.npeaks)
            self.maxpos=self.maxima.maxpos().first()
            self.start=self.indexlist[0,self.maxpos,0]
            self.end=self.indexlist[0,self.maxpos,1]

        self.cutlen=int(2**math.ceil(math.log(min(max(self.end-self.start+self.prepulselen,self.minlen),self.maxlen),2))) if self.cut_to_power_of_two else min(max(self.end-self.start+self.prepulselen,self.minlen),self.maxlen)
        self.update()

        self.start-=self.prepulselen; self.end=self.start+self.cutlen

        if self.doplot:
            if self.search_per_antenna:
                self.timeseries_data[self.peaks_found_list,...,self.search_window_x0:self.search_window_x1].plot(nhighlight=self.npeaks_greater_zero,highlight=(self.indexlist-self.search_window_x0)[self.peaks_found_list,...,[0]:self.npeaks_greater_zero],highlightcolor="red",title="Peak search per antenna",highlightlabel="peaks found",label=self.peaks_found_list)
            else:
                if self.search_window:
                    self.timeseries_data_sum.plot(color="green",label="search window",nhighlight=self.npeaks,highlight=self.indexlist,highlightcolor="orange",title="Peak search: Incoherent sum of timeseries data",highlightlabel="peaks found")
                    cr.hArray(self.timeseries_data_sum[:self.search_window_x0]).plot(clf=False,color="blue",label="Incoherent sum")
                    cr.hArray(self.timeseries_data_sum[self.search_window_x1:]).plot(clf=False,color="blue",xvalues=cr.hArray(range(self.search_window_x1,len(self.timeseries_data_sum))))
                else:
                    self.timeseries_data_sum.plot(nhighlight=self.npeaks,highlight=self.indexlist,color="blue",highlightcolor="orange",title="Peak search: incoherent sum of timeseries data",label="incoherent sum",highlightlabel="peaks found")
                cr.hArray(self.timeseries_data_sum[self.start:self.end]).plot(clf=False,color="red",xvalues=cr.hArray(range(self.start,self.end)),label="peak region")
                cr.plt.legend(loc=2)
            self.plot_finish(name=self.__taskname__+self.plot_name)

        #Cut the data around the main pulse
        if self.docut:
            if len(self.timeseries_data.shape())>1:
                self.timeseries_data_cut[...].copy(self.timeseries_data[...,self.start:self.end])
            else:
                self.timeseries_data_cut.copy(self.timeseries_data[self.start:self.end])

        # Correct for index out of allowed range
        self.end = min(self.end, len(self.timeseries_data_sum) - 1)

class CrossCorrelateAntennas(tasks.Task):
    """
    **Usage:**

    ``Task = trun("CrossCorrelateAntennas",timeseries_data[n_dataset,size], refant=0, reference_data=None, reference_fft_data=None, fft_data=None, fft_reference_data=None, crosscorr_data=None) -> Task.crosscorr_data (i.e., strength of cross-correlation as a function of time)``

    To provide only fft data use:

    ``crosscorr_data = tload("CrossCorrelateAntennas",False)(None,fft_data=bf.fftdata).crosscorr_data``

    **Description:**

    Calculates the cross-correlation of a number of time series data
    sets with respective to a reference data set.

    The data set is a matrix of N equal-length timeseries. If not
    reference data set is provided, a reference antenna from the input
    time series matrix is specified (by default = 0, i.e. all data is
    correlated with the first data set, then the first cross
    correlation output will actually be an autocorrelation).

    Make sure N is even. This is not tested for uneven N.

    NB: This has also not yet been tested for NyquistZone 2!

    If ``timeseries_data == None``, then one needs to provide
    ``fft_data`` instead and the fft will not be (re-)calculated. Note
    that the function uses fftw convention!

    **See also:**

    :class:`CrossCorrelateAntennas`,
    :class:`LocatePulseTrain`,
    :class:`FitMaxima`

    **Example**

    ::

        file=open("/Users/falcke/LOFAR/usg/data/lofar/oneshot_level4_CS017_19okt_no-9.h5")
        file["BLOCKSIZE"]=2**int(round(math.log(file["DATA_LENGTH"][0],2)))
        file["SELECTED_DIPOLES"]=[f for f in file["DIPOLE_NAMES"] if int(f)%2==1] # select uneven antenna IDs
        timeseries_data=file["TIMESERIES_DATA"]
        (start,end)=trun("LocatePulseTrain",rf.TimeBeamIncoherent(timeseries_data),nsigma=7,maxgap=3)
        start-=16
        end=start+int(2**math.ceil(math.log(end-start,2)));
        timeseries_data_cut=hArray(float,[timeseries_data.shape()[-2],end-start])
        timeseries_data_cut[...].copy(timeseries_data[...,start:end])
        timeseries_data_cut[...]-=timeseries_data_cut[...].mean()
        timeseries_data_cut[...]/=timeseries_data_cut[...].stddev(0)

        crosscorr_data=tasks.pulsecal.CrossCorrelateAntennas()(timeseries_data_cut,signvec2=hArray(int,[2],fill=[1,1])).crosscorr_data

        crosscorr_data.abs()
        crosscorr_data[...].runningaverage(15,hWEIGHTS.GAUSSIAN)
        crosscorr_data[0:4,...].plot()

    """
    parameters=dict(
        refant = sc.p_(0,
                       "Which data set in ``data`` to use as reference data set if no ``reference_data`` provided."),

        oversamplefactor=sc.p_(1,
                               "Oversample the final cross-correlation by so many sub-steps. If equal to one then no oversampling."),

        dim=sc.p_(lambda self:self.timeseries_data.shape(),
                  "Dimension of input array.",
                  output=True),

        dimfft=sc.p_(lambda self:self.fft_data.shape(),
                     "Dimension of fft array.",
                     output=True),

        reference_data=sc.p_(None,
                             "Reference data set to cross-correlate data with. If ``None`` then use data from reference antenna ``refant``."),

        fft_data=sc.p_(lambda self:cr.hArray(complex,[self.dim[-2],self.dim[-1]/2+1]),
                       "FFT of the input timeseries data",
                       workarray=True),

        fft_reference_data=sc.p_(lambda self:cr.hArray(complex,[self.dimfft[-1]]),
                                 "FFT of the reference data, dimensions ``[N data sets, (data length)/2+1]``. If no extra reference antenna array is provided, cross correlate with reference antenna in data set",
                                 workarray=True),

        crosscorr_data_orig=sc.p_(lambda self:cr.hArray(float,[self.dimfft[-2],(self.dimfft[-1]-1)*2]),
                                  "Scratch cross correlation vector in original size to hold intermediate results",
                                  workarray=True),

        crosscorr_data=sc.p_(lambda self:cr.hArray(float,[self.dimfft[-2],(self.dimfft[-1]-1)*2*max(self.oversamplefactor,1)]),
                             "Output array of dimensions ``[N data sets, data length * oversamplefactor]`` containing the cross correlation.",
                             output=True),

        blocksize = sc.p_(lambda self:self.timeseries_data.shape()[-1],
                          "Length of the data for each antenna"),

        fftplan = sc.p_(lambda self:cr.FFTWPlanManyDftR2c(self.blocksize, 1, 1, 1, 1, 1, cr.fftw_flags.ESTIMATE),
                        "Memory and plan for FFT",
                        output=False,
                        workarray=True),

        invfftplan = sc.p_(lambda self:cr.FFTWPlanManyDftC2r(self.blocksize, 1, 1, 1, 1, 1, cr.fftw_flags.ESTIMATE),
                           "Memory and plan for inverse FFT",
                           output=False,
                           workarray=True)
        )

    def call(self,timeseries_data):
        pass

    def run(self):
        if self.timeseries_data:
            cr.hFFTWExecutePlan(self.fft_data[...],self.timeseries_data[...],self.fftplan)
            #self.fft_data[...].fftw(self.timeseries_data[...])
        if not self.reference_data:
            self.fft_reference_data.copy(self.fft_data[self.refant])
        else:
#            fftplan = cr.FFTWPlanManyDftR2c(self.refences_data.shape()[-1], 1, 1, 1, 1, 1, cr.fftw_flags.ESTIMATE)
            cr.hFFTWExecutePlan(self.fft_reference_data[...],self.reference_data[...],self.fftplan)
            #self.fft_reference_data.fftw(self.reference_data)
        self.fft_data[...].crosscorrelatecomplex(self.fft_reference_data,True)

        self.fft_data /= self.crosscorr_data.shape()[-1]

        if self.oversamplefactor>1:
            self.shift=1.0/self.oversamplefactor
            for i in range(self.oversamplefactor):
                cr.hFFTWExecutePlan(self.crosscorr_data_orig[...],self.fft_data[...],self.invfftplan)
                #self.crosscorr_data_orig[...].invfftw(self.fft_data[...])
                self.crosscorr_data[...].redistribute(self.crosscorr_data_orig[...],i,self.oversamplefactor) # distribute with gaps in between and shift by a fraction
                self.fft_data[...].shiftfft(self.fft_data[...],self.shift) # apply a sub-sample shift to the FFt data
        else:
            cr.hFFTWExecutePlan(self.crosscorr_data[...],self.fft_data[...],self.invfftplan)
            #self.crosscorr_data[...].invfftw(self.fft_data[...])

class FitMaxima(tasks.Task):
    """
    Find the maxima in multiple datsets and fit a basis spline
    polynomial to the data around their respective maximum.  Then
    determine where the maximum in the interpolated data will be. This
    allows one to determine the maximum with subsample precision.

    **Usage:**

    ``Task = FitMaxima()(data[n_datasets,n_samples], peak_width=20, nsubsamples=16, ncoeffs=5, splineorder=3, doplot=False, plotend=4, plotstart=0, sampleinterval=None, refant=None) -> Task.maxx, Task.maxy, Task.lag``

    The main result will be in ``Task.maxx`` which returns the
    (fractional) sample number of the maximum and ``Task.maxy`` the
    y-value of the maximum. ``Task.lag`` returns the relative offset of
    the peaks relative to the reference dataset (``refant``) in user
    units, specified by ``sampleinterval``.

    **See also:**

    :class:`CrossCorrelateAntennas`,
    :class:`LocatePulseTrain`

    **Example:**

    ::

        file=open("/Users/falcke/LOFAR/usg/data/lofar/oneshot_level4_CS017_19okt_no-9.h5")
        file["BLOCKSIZE"]=2**int(round(math.log(file["DATA_LENGTH"][0],2)))
        file["SELECTED_DIPOLES"]=[f for f in file["DIPOLE_NAMES"] if int(f)%2==1] # select uneven antenna IDs
        timeseries_data=file["TIMESERIES_DATA"]
        (start,end)=trun("LocatePulseTrain",rf.TimeBeamIncoherent(timeseries_data),nsigma=7,maxgap=3)
        start-=16
        end=start+int(2**math.ceil(math.log(end-start,2)));
        timeseries_data_cut=hArray(float,[timeseries_data.shape()[-2],end-start])
        timeseries_data_cut[...].copy(timeseries_data[...,start:end])
        timeseries_data_cut[...]-=timeseries_data_cut[...].mean()
        timeseries_data_cut[...]/=timeseries_data_cut[...].stddev(0)
        crosscorr_data=tasks.pulsecal.CrossCorrelateAntennas()(timeseries_data_cut).crosscorr_data
        crosscorr_data.abs()
        crosscorr_data[...].runningaverage(15,hWEIGHTS.GAUSSIAN)

        mx=trun("FitMaxima",crosscorr_data,doplot=True,refant=0,sampleinterval=5)

        print "Lags =",list(mx.lags),"nanoseconds."

    """
    parameters=dict(
        peak_width=sc.p_(20,"Width of the data over which to fit the peak."),
        nsubsamples=sc.p_(16,"Divide each original sample into that many finer pixels. Determines maximum resolution of fit."),
        ncoeffs=sc.p_(lambda self:self.peak_width/2,"Number of breakpoints to fit spline polynomial around the peak."),
        splineorder=sc.p_(3,"Order of spline to fit. 3=cubic spline."),
        refant=sc.p_(None,"Reference antenna who's maximum determines the zero point of the x-axis (e.g., to get relative delays)."),
        sampleinterval=sc.p_(None,"Separation of two subsequent samples on the x-axis in desired units - used to return lags in proper units."),
        doplot=sc.p_(False,"Plot results."),
        newfigure=sc.p_(True,"Create a new figure for plotting for each new instance of the task."),
        figure=sc.p_(None,"The matplotlib figure containing the plot",output=True),
        plotend=sc.p_(4,"Stop plotting at this data set."),
        plotstart=sc.p_(0, "Start plotting at this data set."),
        legend=sc.p_(None,"List containing labels for each antenna data set for plotting the label"),
        dim2=sc.p_(lambda self:self.data.shape()[-2]),
        coeffs=sc.p_(lambda self:cr.hArray(float,[self.dim2,self.ncoeffs],name="Spline Coefficients"),"Coefficients of spline fit.",output=True),
        fits_xdata=sc.p_(lambda self:cr.hArray(dimensions=[self.dim2,(self.peak_width-1)*(self.nsubsamples)+1],properties=self.data,name="x"),workarray=True),
        fits_ydata=sc.p_(lambda self:cr.hArray(dimensions=[self.dim2,(self.peak_width-1)*(self.nsubsamples)+1],properties=self.data,xvalues=self.fits_xdata,name="Fit"),workarray=True),
        covariance=sc.p_(lambda self:cr.hArray(float,[self.dim2,self.ncoeffs,self.ncoeffs]),workarray=True,name="covariance"),
        xpowers=sc.p_(lambda self:cr.hArray(float,[self.dim2,self.peak_width,self.ncoeffs]),workarray=True,name="Spline xpowers"),
        xdata=sc.p_(lambda self:cr.hArray(float,[self.dim2,self.peak_width]),"x-axis value for the fit"),
        xvalues=sc.p_(lambda self:cr.hArray(float,[self.data.shape()[-1]],fill=range(self.data.shape()[-1])),"Samplenumber for the time series data"),
        maxy=sc.p_(None,"Y-values of fitted maxima",output=True),
        maxx=sc.p_(None,"X-values of fitted maxima",output=True,unit="Samplenumber"),
        lags=sc.p_(None,"X-values of fitted maxima",output=True,unit="User Units")
        )

    def call(self,data):
        pass

    def run(self):
        if self.peak_width < self.splineorder*2+2:
            raise ValueError("peak_width must be >= 2*splineorder+2!")
        self.maximum=cr.asvec(self.data[...].maxpos())
        self.start=cr.Vector(int,copy=self.maximum)
        self.start-=self.peak_width/2
        self.start.max(0)
        self.end=cr.Vector(int,copy=self.start)
        self.end+=self.peak_width
        self.startf=cr.Vector(float,copy=self.start)
        self.endf=cr.Vector(float,copy=self.end)
        self.fits_xdata[...].fillrange(self.startf,cr.asvec(1.0/self.nsubsamples))
        self.xdata[...].fillrange(self.start,cr.asvec(1))
        chisquare=self.coeffs[...].bsplinefit(self.covariance[...],self.xpowers[...],self.xdata[...],self.data[...,self.start:self.end],self.startf,self.endf,cr.asvec(self.splineorder+1))
        self.fits_ydata[...].bsplinecalc(self.fits_xdata[...],self.coeffs[...],self.startf,self.endf,cr.asvec(self.splineorder+1))
        self.maxy=self.fits_ydata[...].max()
        self.maxpos=self.fits_ydata[...].maxpos()
        self.maxx=self.fits_xdata[...].elem(self.maxpos)
        if self.doplot:
            if self.newfigure and not self.figure:
                self.figure=cr.plt.figure()
            self.data[self.plotstart:self.plotend,...].plot(xvalues=self.xvalues,title="Maxima Fitting",legend=self.legend[self.plotstart:self.plotend] if self.legend else None)
            self.fits_ydata[self.plotstart:self.plotend,...].plot(clf=False)
            cr.plt.plot(self.maxx[self.plotstart:self.plotend],self.maxy[self.plotstart:self.plotend],marker="o",linestyle='None')
        self.lags=cr.Vector(copy=self.maxx)
        if self.sampleinterval:
            self.lags*=self.sampleinterval
        if not self.refant==None:
            self.lags-=self.lags[self.refant]

class DirectionFitPlaneWave(tasks.Task):
    """ Fit a plane wave through arrival times and positions.
        Taken from module srcfind.
        Parameters taken from DirectionFitTriangles.
        
        Outlier removal and iteration to be implemented.
    """
        
    parameters=dict(
        positions = dict(doc="hArray with Cartesian coordinates of the antenna positions",
                         unit="m"),

        timelags = dict(doc="hArray with the measured time lags for each event and each antenna",
                        unit="s"),

        good_positions = dict(doc="hArray with Cartesian coordinates of the antenna positions for good antennas",
                              default=lambda self:cr.hArray(properties=self.positions),
                              unit="m",output=True),

        n_timelags = dict(doc="Number of time lags provided",default=lambda self:len(self.timelags),output=True),

        n_good_antennas = dict(doc="Number of good antenna time lags",default=lambda self:len(self.timelags),output=True),

        good_timelags = dict(doc="hArray the measured time lags for each event and all good antennas",
                              default=lambda self:cr.hArray(properties=self.timelags),
                              unit="s",output=True),

        good_antennas = dict(doc="List of integer indices pointing to the good antennas in the original list",
                             default=lambda self:range(n_timelags),output=True),

        expected_timelags=sc.p_(lambda self:cr.hArray(float,[self.NAnt],name="Expected Time Lags"),
                                "Exact time lags expected for each antenna for a given source position",
                                unit="s"),

        measured_geometric_timelags=sc.p_(lambda self:cr.hArray(float,[self.NAnt],name="Geometric Time Lags"),
                                          "Time lags minus cable delay = pure geometric delay if no error",
                                          unit="s"),

        direction_guess = dict(default=False,doc="If a tuple of two numbers (azimut, elevation) then this is an initial guess for the direction (currently only for plotting).",unit="degree"),

        direction_guess_label = dict(default="direction guess",doc="A label for plotting indicating where the direction guess was coming from."),

        cabledelays=sc.p_(lambda self:cr.hArray(float,[self.NAnt],name="Cable Delays",fill=0),
                          "Know (fixed) cable delays that one needs to correct measured delays for.",
                          unit="s"),

        total_delays=sc.p_(lambda self:cr.hArray(float,[self.NAnt],name="Total Delays"),
                           "Total instrumental (residual+cable) delays needed to calibrate the array. Will be subtracted from the measured lags or added to the expected. The array will be updated during iteration",
                           unit="s"),

        residual_delays=sc.p_(lambda self:cr.hArray(float,[self.NAnt],name="Residual Delays"),
                              "Residual delays needed to calibrate the array after correction for known cable delays. Will be subtracted from the measured lags (correced for cable delays) or added to the expected. The array will be updated during iteration",
                              unit="s"),

        delta_delays=sc.p_(lambda self:cr.hArray(float,[self.NAnt],name="Delta Delays"),
                           "Additional instrumental delays needed to calibrate array will be added to timelags and will be updated during iteration",
                           unit="s"),

        delta_delays_mean_history=sc.p_([],
                                        "Mean of the difference between expected and currently calibrated measured delays for each iteration",
                                        unit="s",
                                        output=True),

        delta_delays_rms_history=sc.p_([],
                                       "RMS of the difference between expected and currently calibrated measured delays for each iteration",
                                       unit="s",
                                       output=True),

        delays_history=sc.p_(lambda self:cr.hArray(float,[self.maxiter,self.NAnt],name="Delays"),
                             "Instrumental delays for each iteration (for plotting)",
                             unit="s"),

        delays_historyT=sc.p_(lambda self:cr.hArray(float,[self.NAnt,self.maxiter],name="Delays"),
                              "Instrumental delays for each iteration (for plotting)",
                              unit="s"),

        maxiter=sc.p_(1,
                      "if >1 iterate (maximally tat many times) position and delays until solution converges."),

        delay_error=sc.p_(1e-12,
                          "Target for the RMS of the delta delays where iteration can stop.",
                          unit="s"),

        rmsfactor=sc.p_(2.0,
                        "How many sigma (times RMS) above the average can a delay deviate from the expected timelag (from latest fit iteration) before it is considered bad and removed as outlier."),

        minrmsfactor=sc.p_(1.,
                           "Minimum rmsfactor (see ``rmsfactor``) for selecting bad antennas."),

        unitscalefactor=sc.p_(1e-9,
                              "Scale factor to apply for printing and plotting."),

        unitname=sc.p_("ns",
                       "Unit corresponding to scale factor."),

        doplot=sc.p_(False,
                     "Plot results."),

        plotant_start=sc.p_(0,
                            "First antenna to plot."),

        plotant_end=sc.p_(lambda self:self.NAnt,
                          "Last antenna to plot plus one."),

        verbose=sc.p_(True,
                      "Print progress information."),

        refant=sc.p_(0,
                     "Reference antenna for which geometric delay is zero."),

        solution=sc.p_(1,
                       "Can be ``+/-1``, determine whether to take the upper or the lower ('into the ground') solution."),

        NAnt=sc.p_(lambda self: self.timelags.shape()[-1],
                   "Number of antennas and time lags. If set explicitly, take only the first NAnt antennas from ``Task.positions`` and ``Task.timelags``."),

        NTriangles=sc.p_(lambda self:self.NAnt*(self.NAnt-1)*(self.NAnt-2)/6,
                         "Number of Triangles ``= NAnt*(NAnt-1)*(NAnt-2)/6``."),

        directions=sc.p_(lambda self:cr.hArray(float,[self.NTriangles,3],name="Directions"),
                         "Cartesian direction vector for each triangle"),

        centers=sc.p_(lambda self:cr.hArray(float,[self.NTriangles,3],name="Centers of Triangles"),
                      "Cartesian coordinates of center for each triangle.",
                      unit="m"),

        errors=sc.p_(lambda self:cr.hArray(float,[self.NTriangles],name="Closure Errors"),
                     "Closure errors for each triangle (nor error if = 0)."),

        triangles_size=sc.p_(lambda self:cr.hArray(float,[self.NTriangles],name="Triangle Size"),
                             "Average size for each triangle.",
                             unit="m"),

        index=sc.p_(lambda self:cr.hArray(int,[self.NTriangles],name="Index"),
                    "Index array of good triangles.",
                    workarray=True),

        error_tolerance=sc.p_(1e-10,
                              "Level above which a closure error is considered to be non-zero (take -1 to ignore closure errors)."),

        max_delay=dict(default=15*10**-9,
                       doc="Maximum allowed delay. If a delay for an antenna is larger than this it will be flagged and igrnored"),

        ngood=sc.p_(0,
                    "Number of good triangles (i.e., without closure errors)",
                    output=True),

        ngooddelays=sc.p_(0,
                          "Number of good delays (i.e., with only minor deviation)",
                          output=True),

        delayindex=sc.p_(lambda self:cr.hArray(int,[self.NAnt],name="Delay index"),
                         "Index array of good delays.",
                         workarray=True),

        meandirection=sc.p_(lambda self:cr.hArray(float,[3]),
                            "Cartesian coordinates of mean direction from all good triangles",
                            output=True),

        meancenter=sc.p_(lambda self:cr.hArray(float,[3]),
                         "Cartesian coordinates of mean central position of all good triangles",
                         output=True),

        goodones=sc.p_(lambda self:cr.hArray(float,[self.NTriangles,3],name="Scratch array"),
                       "Scratch array to hold good directions.",
                       unit="m"),

        meandirection_spherical = sc.p_(lambda self:pytmf.cartesian2spherical(self.meandirection[0],self.meandirection[1],self.meandirection[2]),
                                        "Mean direction in spherical coordinates."),

        meandirection_azel=sc.p_(lambda self:(math.pi-(self.meandirection_spherical[2]+pi2),pi2-(self.meandirection_spherical[1])),
                                 "Mean direction as Azimuth (``N->E``), Elevation tuple."),

        meandirection_azel_deg = sc.p_(lambda self:(180-(self.meandirection_spherical[2]+pi2)/deg,90-(self.meandirection_spherical[1])/deg),
                                       "Mean direction as Azimuth (``N->E``), Elevation tuple in degrees."),

        plot_finish = dict(default=lambda self:plotfinish(dopause=False,plotpause=False),
                           doc="Function to be called after each plot to determine whether to pause or not (see :func:`plotfinish`)"),

        plot_name = dict(default="",
                         doc="Extra name to be added to plot filename.")
        )

    def call(self):
        pass

    def run(self):
        from pycrtools import srcfind 
        from numpy import cos, sin
        self.farfield=True
        c = 299792458.0 # speed of light in m/s
        rad2deg = 180.0 / math.pi

        positions = self.positions.toNumpy()
        times = self.timelags.toNumpy()
        times -= times[0]
        
        indicesOfGoodAntennas = np.arange(len(times))
        goodSubset = np.arange(len(times)) # start with all 'good'
        goodcount = len(goodSubset)
        niter = 0
        while True:
            niter += 1
            goodpositions = positions[indicesOfGoodAntennas].ravel()
            goodtimes = times[indicesOfGoodAntennas]
            (az, el) = srcfind.directionForHorizontalArray(goodpositions, goodtimes)
            if np.isnan(el) or np.isnan(az):
                print 'WARNING: plane wave fit returns NaN. Setting elevation to 0.0'
                el = 0.0 # need to propagate the warning...!
                break
            # get residuals
            expectedDelays = srcfind.timeDelaysFromDirection(goodpositions, (az, el)) 
            expectedDelays -= expectedDelays[0]
            self.delta_delays = goodtimes - expectedDelays
            # remove > k-sigma outliers and iterate
            spread = np.std(self.delta_delays)
            k = self.rmsfactor 
            goodSubset = np.where(abs(self.delta_delays - np.mean(self.delta_delays)) < k * spread)
            # gives subset of 'indicesOfGoodAntennas' that is 'good' after this iteration
            if self.verbose:
                print 'iteration # %d' %niter
                print 'az = %f, el = %f' % (az*rad2deg, el*rad2deg)
                print 'good count = %d' % goodcount
            if len(goodSubset[0]) == goodcount:
                break
            else:                
                goodcount = len(goodSubset[0])
                indicesOfGoodAntennas = indicesOfGoodAntennas[goodSubset] 
        
        cartesianDirection = [cos(el)*sin(az), cos(el)*cos(az), sin(el)]
        self.meandirection.copy(cr.hArray(cartesianDirection))
        # NB! Have to update the dependent parameters, or the previous value will come out.
        self.ws.updateParameter("meandirection_spherical",forced=True)
        self.ws.updateParameter("meandirection_azel_deg",forced=True)
        self.ws.updateParameter("meandirection_azel",forced=True)
           
        # now redo arrays for full antenna set
        expectedDelays = srcfind.timeDelaysFromDirection(positions.ravel(), (az, el)) # need positions as flat 1-D array
        expectedDelays -= expectedDelays[0] # subtract ref ant
        self.delta_delays = times - expectedDelays
        self.delta_delays_mean_history = [np.mean(self.delta_delays)] # comply with DirectionFitTriangles, return as list...
        self.delta_delays_rms_history = [np.std(self.delta_delays)]
        self.delta_delays = cr.hArray(self.delta_delays)
        
        self.residual_delays = self.delta_delays
        if self.doplot:
            import matplotlib.pyplot as plt
            plt.figure()
            plt.plot(self.delta_delays)
            plt.figure()
    #        cr.trerun("PlotAntennaLayout","Delsdays",positions = cr.hArray(goodpositions), colors=cr.hArray(self.delta_delays), sizes=100,title="Residuals for plane wave fit",plotlegend=True)            
    #        plt.figure()
    


class DirectionFitTriangles(tasks.Task):
    """
    Find the direction of a source provided one has the measured delay
    of arrival and the positions of the antenna.

    Description:

    This task uses the function
    :func:`hDirectionTriangulationsCartesian` to calculate a list of
    arrival directions in Cartesian coordinates of a pulse/source from
    all possible triangles of (groups of three) antennas and the
    measured arrival times at these antennas.

    For the direction finding it is assumed that the signal arrives as
    a plane wave. If one triangle of antennas does not give a valid
    solution a solution at the horizion is returned with the complex
    part of the solution (the "closure error") returned in ``Task.error``.

    To cope with near-field sources for each triangle the coordinates
    of its center are also calculated. This allows one to later look
    for points where the dircetions from all triangles intercept.

    The inverse of the average lengths of the triangle baselines are
    returned in ``weights`` giving some indication how accurate the
    measurement in principle can be.

    From three antennas we get in general two solutions for the
    direction, unless the arrival times are out of bounds, i.e. larger
    than the light-time between two antennas.  Usually, when the three
    antennas are in a more or less horizontal plane, one of the
    solutions will appear to come from below the horizon (el < 0) and
    can be discarded. With `sign_of_solution` one can pick either the
    positive of the negative elevation.

    If :math:`N` triangles are provided then the *Directions*,
    *Origins*, and *weights* vectors have
    :math:`\\frac{N(N-1)(N-2)}{6}` results (times number of components
    of each result, i.e. times three for *origins* and times one for
    the rest.)


    The actually measured lag is assumed to consist of two parts: the
    geometric delay due to light travel times plus the cable delay in
    each antenna.

    ``measured timelag = expected geometric delay + cable delay + residual delay``

    What we do in the iteration is to use the above equation in the
    form

    ``measured timelag - (cable delay + residual delay + delta delays[i]) - expected geometric delay = 0``

    with a delta delay that is determined at each step and then subsumed into the residual delay.

    **Output:**

    The main result will be in ``Task.meandirection`` which contains
    the direction vector to the source in Cartesian coordinates. See
    also ``Task.meandirection_azel`` and
    ``Task.meandirection_azel_deg``.

    The array ``Task.delays`` will contain the final cable delays
    needed to get a coherent signal for the final position. The delays
    as a function of iteration are in
    ``Task.delays_history[iteration]``

    **Coordinate Systems**

    - Azimuth/Elevation is defined as North (0 degrees) through East
      (90 degrees) for ``Az`` and ``El`` running from 90 degree at the
      zenith to 0 degree at the horizon

    - For spherical coordindates Az/phi is defined as East (0 degree)
      through North (90 degree) and theta running from 0 degree at the
      zenith to 90 degree at the horizon

    **See also:**

    :class:`CrossCorrelateAntennas`,
    :class:`LocatePulseTrain`,
    :class:`FitMaxima`

    **Example:**

    ::

        file=open("$LOFARSOFT/data/lofar/oneshot_level4_CS017_19okt_no-9.h5")
        file["ANTENNA_SET"]="LBA_OUTER"
        file["BLOCKSIZE"]=2**17

        file["SELECTED_DIPOLES"]=["017000001","017000002","017000005","017000007","017001009","017001010","017001012","017001015","017002017","017002019","017002020","017002023","017003025","017003026","017003029","017003031","017004033","017004035","017004037","017004039","017005041","017005043","017005045","017005047","017006049","017006051","017006053","017006055","017007057","017007059","017007061","017007063","017008065","017008066","017008069","017008071","017009073","017009075","017009077","017009079","017010081","017010083","017010085","017010087","017011089","017011091","017011093","017011095"]

        timeseries_data=file["TIMESERIES_DATA"]
        positions=file["ANTENNA_POSITIONS"]

        #First determine where the pulse is in a simple incoherent sum of all time series data

        pulse=trun("LocatePulseTrain",timeseries_data,nsigma=7,maxgap=3)

        #Normalize the data which was cut around the main pulse
        pulse.timeseries_data_cut[...]-=pulse.timeseries_data_cut[...].mean()
        pulse.timeseries_data_cut[...]/=pulse.timeseries_data_cut[...].stddev(0)

        #Cross correlate all pulses with each other
        crosscorr=trun('CrossCorrelateAntennas',pulse.timeseries_data_cut,oversamplefactor=5)

        #And determine the relative offsets between them
        mx=trun('FitMaxima',crosscorr.crosscorr_data,doplot=True,refant=0,plotstart=4,plotend=5,sampleinterval=10**-9,peak_width=6,splineorder=2)

        #Now fit the direction and iterate over cable delays to get a stable solution
        direction=trun("DirectionFitTriangles",positions=positions,timelags=hArray(mx.lags),maxiter=10,verbose=True,doplot=True)

        print "========================================================================"
        print "Fit Arthur Az/El   ->  143.409 deg 81.7932 deg"
        print "Triangle Fit Az/EL -> ", direction.meandirection_azel_deg,"deg"

        # Triangle Fit Az/EL ->  (144.1118392216996, 81.84042919170588) deg for odd antennas
        # Triangle Fit Az/EL ->  (145.17844721833896, 81.973693266380721) deg for even antennas

    """
    parameters=dict(
        positions = dict(doc="hArray with Cartesian coordinates of the antenna positions",
                         unit="m"),

        timelags = dict(doc="hArray with the measured time lags for each event and each antenna",
                        unit="s"),

        good_positions = dict(doc="hArray with Cartesian coordinates of the antenna positions for good antennas",
                              default=lambda self:cr.hArray(properties=self.positions),
                              unit="m",output=True),

        n_timelags = dict(doc="Number of time lags provided",default=lambda self:len(self.timelags),output=True),

        n_good_antennas = dict(doc="Number of good antenna time lags",default=lambda self:len(self.timelags),output=True),

        good_timelags = dict(doc="hArray the measured time lags for each event and all good antennas",
                              default=lambda self:cr.hArray(properties=self.timelags),
                              unit="s",output=True),

        good_antennas = dict(doc="List of integer indices pointing to the good antennas in the original list",
                             default=lambda self:range(n_timelags),output=True),

        expected_timelags=sc.p_(lambda self:cr.hArray(float,[self.NAnt],name="Expected Time Lags"),
                                "Exact time lags expected for each antenna for a given source position",
                                unit="s"),

        measured_geometric_timelags=sc.p_(lambda self:cr.hArray(float,[self.NAnt],name="Geometric Time Lags"),
                                          "Time lags minus cable delay = pure geometric delay if no error",
                                          unit="s"),

        direction_guess = dict(default=False,doc="If a tuple of two numbers (azimut, elevation) then this is an initial guess for the direction (currently only for plotting).",unit="degree"),

        direction_guess_label = dict(default="direction guess",doc="A label for plotting indicating where the direction guess was coming from."),

        cabledelays=sc.p_(lambda self:cr.hArray(float,[self.NAnt],name="Cable Delays",fill=0),
                          "Know (fixed) cable delays that one needs to correct measured delays for.",
                          unit="s"),

        total_delays=sc.p_(lambda self:cr.hArray(float,[self.NAnt],name="Total Delays"),
                           "Total instrumental (residual+cable) delays needed to calibrate the array. Will be subtracted from the measured lags or added to the expected. The array will be updated during iteration",
                           unit="s"),

        residual_delays=sc.p_(lambda self:cr.hArray(float,[self.NAnt],name="Residual Delays"),
                              "Residual delays needed to calibrate the array after correction for known cable delays. Will be subtracted from the measured lags (correced for cable delays) or added to the expected. The array will be updated during iteration",
                              unit="s"),

        delta_delays=sc.p_(lambda self:cr.hArray(float,[self.NAnt],name="Delta Delays"),
                           "Additional instrumental delays needed to calibrate array will be added to timelags and will be updated during iteration",
                           unit="s"),

        delta_delays_mean_history=sc.p_([],
                                        "Mean of the difference between expected and currently calibrated measured delays for each iteration",
                                        unit="s",
                                        output=True),

        delta_delays_rms_history=sc.p_([],
                                       "RMS of the difference between expected and currently calibrated measured delays for each iteration",
                                       unit="s",
                                       output=True),

        delays_history=sc.p_(lambda self:cr.hArray(float,[self.maxiter,self.NAnt],name="Delays"),
                             "Instrumental delays for each iteration (for plotting)",
                             unit="s"),

        delays_historyT=sc.p_(lambda self:cr.hArray(float,[self.NAnt,self.maxiter],name="Delays"),
                              "Instrumental delays for each iteration (for plotting)",
                              unit="s"),

        maxiter=sc.p_(1,
                      "if >1 iterate (maximally tat many times) position and delays until solution converges."),

        delay_error=sc.p_(1e-12,
                          "Target for the RMS of the delta delays where iteration can stop.",
                          unit="s"),

        rmsfactor=sc.p_(3.,
                        "How many sigma (times RMS) above the average can a delay deviate from the mean before it is considered bad (will be reduced with every iteration until ``minrsmfactor``)."),

        minrmsfactor=sc.p_(1.,
                           "Minimum rmsfactor (see ``rmsfactor``) for selecting bad antennas."),

        unitscalefactor=sc.p_(1e-9,
                              "Scale factor to apply for printing and plotting."),

        unitname=sc.p_("ns",
                       "Unit corresponding to scale factor."),

        doplot=sc.p_(False,
                     "Plot results."),

        plotant_start=sc.p_(0,
                            "First antenna to plot."),

        plotant_end=sc.p_(lambda self:self.NAnt,
                          "Last antenna to plot plus one."),

        verbose=sc.p_(False,
                      "Print progress information."),

        refant=sc.p_(0,
                     "Reference antenna for which geometric delay is zero."),

        solution=sc.p_(1,
                       "Can be ``+/-1``, determine whether to take the upper or the lower ('into the ground') solution."),

        NAnt=sc.p_(lambda self: self.timelags.shape()[-1],
                   "Number of antennas and time lags. If set explicitly, take only the first NAnt antennas from ``Task.positions`` and ``Task.timelags``."),

        NTriangles=sc.p_(lambda self:self.NAnt*(self.NAnt-1)*(self.NAnt-2)/6,
                         "Number of Triangles ``= NAnt*(NAnt-1)*(NAnt-2)/6``."),

        directions=sc.p_(lambda self:cr.hArray(float,[self.NTriangles,3],name="Directions"),
                         "Cartesian direction vector for each triangle"),

        centers=sc.p_(lambda self:cr.hArray(float,[self.NTriangles,3],name="Centers of Triangles"),
                      "Cartesian coordinates of center for each triangle.",
                      unit="m"),

        errors=sc.p_(lambda self:cr.hArray(float,[self.NTriangles],name="Closure Errors"),
                     "Closure errors for each triangle (nor error if = 0)."),

        triangles_size=sc.p_(lambda self:cr.hArray(float,[self.NTriangles],name="Triangle Size"),
                             "Average size for each triangle.",
                             unit="m"),

        index=sc.p_(lambda self:cr.hArray(int,[self.NTriangles],name="Index"),
                    "Index array of good triangles.",
                    workarray=True),

        error_tolerance=sc.p_(1e-10,
                              "Level above which a closure error is considered to be non-zero (take -1 to ignore closure errors)."),

        max_delay=dict(default=15*10**-9,
                       doc="Maximum allowed delay. If a delay for an antenna is larger than this it will be flagged and igrnored"),

        ngood=sc.p_(0,
                    "Number of good triangles (i.e., without closure errors)",
                    output=True),

        ngooddelays=sc.p_(0,
                          "Number of good delays (i.e., with only minor deviation)",
                          output=True),

        delayindex=sc.p_(lambda self:cr.hArray(int,[self.NAnt],name="Delay index"),
                         "Index array of good delays.",
                         workarray=True),

        meandirection=sc.p_(lambda self:cr.hArray(float,[3]),
                            "Cartesian coordinates of mean direction from all good triangles",
                            output=True),

        meancenter=sc.p_(lambda self:cr.hArray(float,[3]),
                         "Cartesian coordinates of mean central position of all good triangles",
                         output=True),

        goodones=sc.p_(lambda self:cr.hArray(float,[self.NTriangles,3],name="Scratch array"),
                       "Scratch array to hold good directions.",
                       unit="m"),

        meandirection_spherical = sc.p_(lambda self:pytmf.cartesian2spherical(self.meandirection[0],self.meandirection[1],self.meandirection[2]),
                                        "Mean direction in spherical coordinates."),

        meandirection_azel=sc.p_(lambda self:(math.pi-(self.meandirection_spherical[2]+pi2),pi2-(self.meandirection_spherical[1])),
                                 "Mean direction as Azimuth (``N->E``), Elevation tuple."),

        meandirection_azel_deg = sc.p_(lambda self:(180-(self.meandirection_spherical[2]+pi2)/deg,90-(self.meandirection_spherical[1])/deg),
                                       "Mean direction as Azimuth (``N->E``), Elevation tuple in degrees."),

        plot_finish = dict(default=lambda self:plotfinish(dopause=False,plotpause=False),
                           doc="Function to be called after each plot to determine whether to pause or not (see :func:`plotfinish`)"),

        plot_name = dict(default="",
                         doc="Extra name to be added to plot filename.")
        )

    def call(self):
        pass

    def run(self):
        self.farfield=True
        self.delta_delays_max=0
        self.delta_delays_min=1e99
        self.delta_delays_mean_history=[]
        self.delta_delays_rms_history=[]
        self.enditer=False
        self.residual_delays.fill(0)
        if self.verbose:
            allantennas=set(range(self.NAnt))

        self.cabledelays -= self.cabledelays[self.refant]
        if self.doplot:
            cr.plt.clf()
            cr.plt.polar(0,90,marker=".",color="white") # Note, we are fooling the system, .polar plots actually phi, radius and not Az,El.
            if isinstance(self.direction_guess,(tuple,list)) and len(self.direction_guess)==2:
                cr.plt.polar((90-self.direction_guess[0])*deg,90-self.direction_guess[1],marker="o",color="blue",label=self.direction_guess_label,markersize=10)

        for it in range(self.maxiter):
            #Calculate directions from all triangles
            self.total_delays.add(self.residual_delays,self.cabledelays)
            self.measured_geometric_timelags.sub(self.timelags,self.total_delays)
            cr.hDirectionTriangulationsCartesian(self.directions,self.errors,self.centers,self.triangles_size,self.positions[:self.NAnt],self.measured_geometric_timelags[:self.NAnt],+1)

            self.delays_history[it].copy(self.total_delays)
            self.delays_historyT.redistribute(self.total_delays,it,self.maxiter)

            #Find antennas with zero closure errors, these are considered good antennas
            self.ngood=self.index.findlessthan(self.errors,self.error_tolerance).val()

            #Get mean position of good antennas
            self.goodones.copyvec(self.centers,self.index,self.ngood,3)
            if self.ngood == 0:
                print "DirectionFitTriangles: NO_GOOD_TRIANGLES_FOUND"
                return
            self.meancenter.mean(self.goodones[:self.ngood])

            #Get mean direction from good antennas
            self.goodones.copyvec(self.directions,self.index,self.ngood,3)
            self.meandirection.mean(self.goodones[:self.ngood])           # mean direction of all good antennas
            self.meandirection /= self.meandirection.vectorlength().val() #normalize direction vector

            cr.hGeometricDelays(self.expected_timelags,self.positions,self.meandirection, self.farfield)
            self.expected_timelags -= self.expected_timelags[self.refant]
            self.delta_delays.sub(self.measured_geometric_timelags,self.expected_timelags)
            self.delta_delays -= self.delta_delays[self.refant]

            self.delta_delays_mean=self.delta_delays.vec().mean()
            self.delta_delays_rms=self.delta_delays.vec().stddev(self.delta_delays_mean)
            self.delta_delays_max=max(self.delta_delays.vec().max(),self.delta_delays_max)
            self.delta_delays_min=min(self.delta_delays.vec().min(),self.delta_delays_min)

            self.delta_delays_rms_history.append(self.delta_delays_rms)
            self.delta_delays_mean_history.append(self.delta_delays_mean)

            rfac=max(self.rmsfactor*(1.0-float(it)/(self.maxiter-1)),self.minrmsfactor)
            self.ngooddelays=self.delayindex.findbetween(self.delta_delays,self.delta_delays_mean-self.delta_delays_rms*rfac,self.delta_delays_mean+self.delta_delays_rms*rfac).val()
            if self.ngooddelays>0:
                self.delta_delays.set(self.delayindex[:self.ngooddelays],0.0)
            self.residual_delays+=self.delta_delays

            self.ws.updateParameter("meandirection_spherical",forced=True)
            self.ws.updateParameter("meandirection_azel_deg",forced=True)
            self.ws.updateParameter("meandirection_azel",forced=True)

            if self.verbose:
                print "------------------------------------------------------------------------"
                badantennas=allantennas.difference(set(cr.asvec(self.delayindex[:self.ngooddelays]))) if self.ngooddelays>0 else allantennas
                if self.maxiter>1:
                    print "Iteration #",it,", rms factor =",rfac
                print "Triangle Fit Az/EL -> ", self.meandirection_azel_deg,"deg"
                print "Triangle Fit Az/EL -> ", self.meandirection
                print "Mean delta delays ["+self.unitname+"] =",self.delta_delays_mean/self.unitscalefactor,"+/-",self.delta_delays_rms/self.unitscalefactor," (number bad antennas =",self.NAnt-self.ngooddelays,")"
                print "Bad Antennas = ",badantennas

            if self.doplot:
                cr.plt.polar(self.meandirection_spherical[2],self.meandirection_spherical[1]/deg,label=str(it),marker="x")
            if self.delta_delays_rms<self.delay_error or self.enditer: #Solution has converged
                break

        #End of Iteration
        # BUGFIX: self.total_delays is recalculated only at the top of the loop
        #         therefore has to be done once more at the end.
        self.total_delays.add(self.residual_delays,self.cabledelays) # recalculates it from residual_delays
       
        self.niter=it
        if self.doplot:
            cr.plt.title("Position of Event during DirectionFitting (Spherical Coordinates, N up)")
            cr.plt.polar(self.meandirection_spherical[2],self.meandirection_spherical[1]/deg,label="final",marker="o",color="red")
            cr.plt.legend(borderaxespad=-3)
        #    cr.plt.xticklabels(['E', 'NE', 'N', 'NW', 'W', 'SW', 'S', 'SE'])

            self.plot_finish(name=self.__taskname__+self.plot_name)
        if self.verbose:
            print "------------------------------------------------------------------------"
            print "Number of iterations used: Task.niter =",self.niter
            print " "

        if self.doplot>2:
            self.delays_historyT /= self.unitscalefactor
            self.offset=cr.Vector(float,self.NAnt)
            self.offset.fillrange(0,(self.delta_delays_max-self.delta_delays_min)/self.unitscalefactor)
            self.delays_historyT[...] += self.offset
            self.delays_historyT[self.plotant_start:self.plotant_end,...,:it+1].plot(xlabel="Iteration #",ylabel="Delay (+offset)")
            self.delays_historyT[...] -= self.offset


# class DirectionMultipleFitTriangles(tasks.Task):
#     """
#     Find the direction of a source provided one has the measured delay
#     of arrival and the positions of the antenna. This will fit the
#     directions towards multiple sources at once, assuming the same
#     cable delay (errors).


#Needed for Task PlotDirectionTriangles

class PlotDirectionTriangles(tasks.Task):
    """
    **Description:**

    Plot the directions towards a source found by triangle
    fitting. This will average directions from triangles of antennas
    with the same center into one mean direction and then plot a mean
    direction arrow, for this subarray.

    This will als plot the underlying layout of antennas.

    **Usage:**

    **See also:**
    :class:`DirectionFitTriangles`

    **Example:**

    ::

        filename="oneshot_level4_CS017_19okt_no-9.h5"
        file=open("$LOFARSOFT/data/lofar/"+filename)
        file["ANTENNA_SET"]="LBA_OUTER"
        file["BLOCKSIZE"]=2**17

        file["SELECTED_DIPOLES"]=["017000001","017000002","017000005","017000007","017001009","017001010","017001012","017001015","017002017","017002019","017002020","017002023","017003025","017003026","017003029","017003031","017004033","017004035","017004037","017004039","017005041","017005043","017005045","017005047","017006049","017006051","017006053","017006055","017007057","017007059","017007061","017007063","017008065","017008066","017008069","017008071","017009073","017009075","017009077","017009079","017010081","017010083","017010085","017010087","017011089","017011091","017011093","017011095"]

        timeseries_data=file["TIMESERIES_DATA"]
        positions=file["ANTENNA_POSITIONS"]

        #First determine where the pulse is in a simple incoherent sum of all time series data

        pulse=trun("LocatePulseTrain",timeseries_data,nsigma=7,maxgap=3)

        #Normalize the data which was cut around the main pulse for correlation
        pulse.timeseries_data_cut[...]-=pulse.timeseries_data_cut[...].mean()
        pulse.timeseries_data_cut[...]/=pulse.timeseries_data_cut[...].stddev(0)

        #Cross correlate all pulses with each other
        crosscorr=trun('CrossCorrelateAntennas',pulse.timeseries_data_cut,oversamplefactor=5)

        #And determine the relative offsets between them
        mx=trun('FitMaxima',crosscorr.crosscorr_data,doplot=True,refant=0,plotstart=4,plotend=5,sampleinterval=10**-9,peak_width=6,splineorder=2)

        #Now fit the direction and iterate over cable delays to get a stable solution
        direction=trun("DirectionFitTriangles",positions=positions,timelags=hArray(mx.lags),maxiter=10,verbose=True,doplot=True)

        print "========================================================================"
        print "Fit Arthur Az/El   ->  143.409 deg 81.7932 deg"
        print "Triangle Fit Az/EL -> ", direction.meandirection_azel_deg,"deg"

        # Triangle Fit Az/EL ->  (144.1118392216996, 81.84042919170588) deg for odd antennas
        # Triangle Fit Az/EL ->  (145.17844721833896, 81.973693266380721) deg for even antennas

        p=trun("PlotDirectionTriangles",centers=direction.centers,positions=direction.positions,directions=direction.directions,title=filename)
    """
    parameters=dict(
        positions = dict(doc="hArray of dimension ``[NAnt,3]`` with Cartesian coordinates of the antenna positions (x0,y0,z0,...)",
                         unit="m"),

        centers = dict(doc="hArray of dimension ``[NTriangles,3]`` with Cartesian coordinates of the centers of each triangle (x0,y0,z0,...)",
                       unit="m"),

        directions = dict(doc="hArray of dimension ``[NTriangles,3]`` with Cartesian coordinates of the direction each triangle has given (x0,y0,z0,...)",
                          unit="m"),

        title = dict(default=False,
                     doc="Title for the plot (e.g., event or filename)"),

        plotlegend = dict(default=False,
                          doc="Plot a legend"),

        newfigure = sc.p_(True,
                          "Create a new figure for plotting for each new instance of the task."),

        direction_arrow_length = dict(default=10.,
                                      doc="Relative length of the direction arrows relative to the maximum size of the array"),

        positionsT = sc.p_(lambda self:cr.hArray_transpose(self.positions),
                           "hArray with transposed Cartesian coordinates of the antenna positions (x0, x1,..., y0, y1,..., z0, z1,....)",
                           unit="m",
                           workarray=True),

        NAnt = sc.p_(lambda self: self.positions.shape()[-2],
                     "Number of antennas.",
                     output=True),

        SubArrayFactor = sc.p_(lambda self:0.5,
                               "Factor used to determine the number of subarrays for which to average the direction from triangles ``NSubArrays=NAnt*SubArrayFactor``"),

        NSubArrays = sc.p_(lambda self:int(self.NAnt*self.SubArrayFactor),
                           "Number of subarrays for which to average the direction from triangles"),

        NTriangles = sc.p_(lambda self:self.NAnt*(self.NAnt-1)*(self.NAnt-2)/6,
                           "Number of Triangles = ``NAnt*(NAnt-1)*(NAnt-2)/6`` = length of directions.",
                           output=True)
        )

    def call(self):
        pass

    def run(self):
        self.meancenter=cr.hArray(float,[3])
        self.meancenter.mean(self.positions)

        self.meandirection=cr.hArray(float,[3])
        self.meandirection.mean(self.directions)

        self.triangle_distances=self.centers[...,0:3].vectorlength(self.meancenter)
        self.triangle_distances_mean=self.triangle_distances.mean()
        self.triangle_distances_stddev=self.triangle_distances.stddev()
        self.triangle_distances_max=self.triangle_distances.max()

        self.triangle_subarray_radius=(self.triangle_distances_max-self.triangle_distances_mean)/2

        self.azelr=cr.hArray(float,[self.NSubArrays,3])
        self.xyz=cr.hArray(float,[self.NSubArrays,3])
        self.azelr.fillrangevec(cr.hArray([0.0,0.0,self.triangle_distances_max]),cr.hArray([2.*math.pi/self.NSubArrays,0.0,0.]))
        cr.hCoordinateConvert(self.azelr[...],cr.CoordinateTypes.AzElRadius,self.xyz[...],cr.CoordinateTypes.Cartesian,False)
        self.xyz+=self.meancenter

        self.indx=cr.hArray(int,[self.NSubArrays,self.NTriangles])
        self.triangle_separations=cr.hArray(float,[self.NSubArrays,self.NTriangles])
        self.triangle_separations[...].vectorseparation(self.xyz[...],self.centers)

        #Find the triangles that are close to the respective grid point on the circle
        self.ntriangles_subarray=self.indx[...].findlessthan(self.triangle_separations[...],self.triangle_subarray_radius)

        self.scrt=cr.hArray(float,[self.NSubArrays,self.ntriangles_subarray.max(),3])
        self.subdirections=cr.hArray(float,[self.NSubArrays,3])
        self.suborigins=cr.hArray(float,[self.NSubArrays,3])

        self.scrt[...].copyvec(self.centers,self.indx[...],self.ntriangles_subarray,cr.Vector([3]))
        self.suborigins[...].mean(self.scrt[...,[0]:self.ntriangles_subarray])
        self.suboriginsT=self.suborigins.Transpose()

        self.scrt[...].copyvec(self.directions,self.indx[...],self.ntriangles_subarray,cr.Vector([3]))
        self.subdirections[...].mean(self.scrt[...,[0]:self.ntriangles_subarray])
        self.subdirections[...] /= self.subdirections[...].vectorlength()

        self.subpoints2=cr.hArray(copy=self.subdirections)
        self.subpoints2 *= self.triangle_distances_max*self.direction_arrow_length
        self.subpoints2 += self.suborigins

        self.meanpoint2=cr.hArray(copy=self.meandirection)
        self.meanpoint2 *= self.triangle_distances_max*self.direction_arrow_length
        self.meanpoint2 += self.meancenter

        #if self.newfigure:
        self.fig = cr.plt.figure()
        self.ax = self.fig.gca(projection='3d')
        self.ax.plot([self.meancenter[0],self.meanpoint2[0]],[self.meancenter[1],self.meanpoint2[1]],[self.meancenter[2],self.meanpoint2[2]], linewidth=5,label="Mean Direction")
        for i in range(self.NSubArrays):
            self.ax.plot([self.suborigins[i,0],self.subpoints2[i,0]],[self.suborigins[i,1],self.subpoints2[i,1]],[self.suborigins[i,2],self.subpoints2[i,2]], label="Sub-Array "+str(i),linewidth=1)
        self.ax.plot(self.positionsT[0].vec(),self.positionsT[1].vec(),self.positionsT[2].vec(),marker='x',linestyle='',label="Antennas")
        self.ax.plot(self.suboriginsT[0].vec(),self.suboriginsT[1].vec(),self.suboriginsT[2].vec(),marker='o',linestyle='',label="Sub-Arrays")
        self.ax.set_ylabel("y")
        self.ax.set_xlabel("x")
        self.ax.set_zlabel("z")
        if self.title:
            self.ax.set_title(self.title)
        if self.plotlegend:
            self.ax.legend()


class PlotAntennaLayout(tasks.Task):
    """
    **Description:**

    Plot the layout of the current dataset on the ground.

    **Usage:**

    **See also:**
    :class:`DirectionFitTriangles`

    **Example:**

    ::

        file=open("$LOFARSOFT/data/lofar/oneshot_level4_CS017_19okt_no-9.h5")
        file["ANTENNA_SET"]="LBA_OUTER"
        file["SELECTED_DIPOLES"]="odd"
        positions=file["ANTENNA_POSITIONS"]
        layout=trun("PlotAntennaLayout",positions=positions,sizes=range(48),names=range(48))
   """
    parameters=dict(
        positions = dict(doc="hArray of dimension [NAnt,3] with Cartesian coordinates of the antenna positions (x0,y0,z0,...)",
                         unit="m"),

        size = dict(default=300,
                    doc="Size of largest point."),

        sizes_min = dict(default=None,
                         doc="If set, then use this as the minimum scale for the sizes, when normalizing."),
        sizes_max = dict(default=None,
                         doc="If set, then use this as the maximum scale for the sizes, when normalizing."),

        normalize_sizes = dict(default=True,
                               doc="Normalize the sizes to run from 0-1."),

        normalize_colors = dict(default=True,
                                doc="Normalize the colors to run from 0-1."),

        sizes = dict(default=20,
                     doc="hArray of dimension [NAnt] with the values for the size of the plot"),

        colors = dict(default='b',
                      doc="hArray of dimension [NAnt] with the values for the colors of the plot"),

        names = dict(default=False,
                     doc="hArray of dimension [NAnt] with the names or IDs of the antennas"),

        title = dict(default=False,
                     doc="Title for the plot (e.g., event or filename)"),

        newfigure = sc.p_(True,
                       "Create a new figure for plotting for each new instance of the task."),

        plot_clf = dict(default=True,doc="Clean window before plotting?"),

        plot_finish = dict(default=lambda self:cr.plotfinish(doplot=True,plotpause=False),
                           doc="Function to be called after each plot to determine whether to pause or not (see ::func::plotfinish)"),

        plot_name = dict(default="",
                         doc="Extra name to be added to plot filename."),

        plotlegend = dict(default=False,
                          doc="Plot a legend"),

        positionsT = sc.p_(lambda self:cr.hArray_transpose(self.positions),
                           "hArray with transposed Cartesian coordinates of the antenna positions (x0,x1,...,y0,y1...,z0,z1,....)",
                           unit="m",
                           workarray=True),

        NAnt = sc.p_(lambda self: self.positions.shape()[-2],
                     "Number of antennas.",
                     output=True),
        )

    def call(self):
        pass

    def run(self):

        #Calculate scaled sizes
        if isinstance(self.sizes, (int, long, float)):
            self.ssizes=self.sizes
        elif isinstance(self.sizes,tuple(cr.hRealContainerTypes)):
            if self.normalize_sizes:
                self.ssizes=cr.hArray(copy=self.sizes)
                self.ssizes -= self.ssizes.min().val() if self.sizes_min==None else self.sizes_min
                self.ssizes /= self.ssizes.max().val() if self.sizes_max==None else self.sizes_max
                self.ssizes *= self.size
        else:
            raise TypeError("PlotAntennaLayout: parameter 'sizes' needs to be a number or an hArray of numbers.")

        plotlegend=True
        if isinstance(self.colors, (str, int, long, float)):
            self.scolors=self.colors
            plotlegend=False
        elif isinstance(self.colors,tuple(cr.hAllContainerTypes)):
            if self.normalize_colors:
                self.scolors=cr.hArray(copy=self.colors)
                self.scolors -= self.scolors.min().val()
                self.scolors /= self.scolors.max().val()
        else:
            raise TypeError("PlotAntennaLayout: parameter 'colos' needs to be string or an hArray thereof.")

        if self.newfigure and not hasattr(self,"figure"):
            self.figure=cr.plt.figure()
        if self.plot_clf: cr.plt.clf()
        if self.title:
            cr.plt.title(self.title)
        cr.plt.scatter(self.positionsT[0].vec(),self.positionsT[1].vec(),s=self.ssizes,c=self.colors)
        if self.names:
            for label,x,y in zip(self.names,self.positionsT[0].vec(),self.positionsT[1].vec()):
                cr.plt.annotate(str(label),xy=(x,y), xytext=(-3,3),textcoords='offset points', ha='right', va='bottom')
        if self.plotlegend and plotlegend:
            cr.plt.colorbar()
        self.plot_finish(name=self.__taskname__+self.plot_name)
