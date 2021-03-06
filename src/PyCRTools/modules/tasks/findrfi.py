"""
Module documentation
====================

.. moduleauthor:: Arthur Corstanje <a.corstanje@astro.ru.nl>
"""

from pycrtools.tasks import Task
import numpy as np
import pycrtools as cr
import matplotlib.pyplot as plt


def dirtyChannelsFromPhaseSpreads(spreads, flagwidth=3, testplots=False):
    length = len(spreads)
    sorted = np.sort(spreads)
    # Sorted plot will have many entries below the uniform-random value of sqrt(2).
    # They correspond to the RFI lines. Circumvent taking standard-deviation and peak-detection,
    # as the stddev is skewed by the very peaks we are trying to find...
    # This is done by estimating the (noise) spread using the higher half of the data (no peaks!)
    # then have a criterion of > 3x noise to make an RF-line detection.
    medianSpread = np.median(spreads)
    noise = sorted[length * 0.95] - sorted[length / 2]  # value at 95 percentile minus median

    dirtyChannels = np.where(spreads < medianSpread - 3 * noise)[0]

    # In principle, that's all! The '3' may have to be tuned a little.

    # If needed, the dirty channels have to be 'fattened' to both sides to also cut the flanks
    # of the spectral lines...
    # This holds especially when processing large blocks (high spectral resolution) in not-so-long datasets
    # e.g. 10 blocks of length 65536.
    # Currently we use a flagwidth of 1 + blocksize / 4096.

    # extend dirty channels to width 'flagwidth' (odd number needed), to also capture the flanks of
    # spectral lines
    extDirtyChannels = np.zeros(length)
    sideExtension = (flagwidth - 1) / 2
    for i in range(sideExtension, length - sideExtension):
        if i in dirtyChannels:
            extDirtyChannels[i - sideExtension:i + sideExtension + 1] = 1

    extDirtyChannels = np.where(extDirtyChannels > 0)[0]

    print 'Median spread = %f' % medianSpread
    print 'Noise = %f' % noise
    print 'There are %d dirty channels' % len(dirtyChannels)
    dirtyChannels = np.array(extDirtyChannels)
    print 'There are %d dirty channels when extended by %d channel(s) to either side' % (len(dirtyChannels), (flagwidth - 1) / 2)

    if testplots:
        plt.figure()
        plt.plot(sorted)
        plt.axvline(x=len(dirtyChannels), ymin=0, ymax=1.83)
        plt.title("Sorted phase spread values. Vertical line = cutoff for 'dirty channels'")
        plt.ylabel("Spread value [rad]")
        plt.xlabel("index")

    return dirtyChannels


class FindRFI(Task):
    """Find RFI by creating an average spectrum and doing phase magic.
    """

    parameters = dict(
        filename=dict(default=None,
            doc="Filename, used if no file object is given."),
        f=dict(default=lambda self: cr.open(self.filename),
            doc="File object."),
        blocksize=dict(default=lambda self: self.f["BLOCKSIZE"],
            doc="Blocksize."),
        nantennas=dict(default=lambda self: self.f["NOF_SELECTED_DATASETS"],
            doc="Number of selected antennas."),
        nfreq=dict(default=lambda self: self.blocksize / 2 + 1,
            doc="Number of frequencies in FFT."),
        nofblocks=dict(default=-1, doc="Number of data blocks to process. Set to -1 for entire file."),
        startblock=dict(default=0, doc="Start processing file at this block nr."),
        refant=dict(default=None, doc="Optional parameter to set reference antenna number."),

        freq_range=dict(default=None, doc="Optional frequency range to consider; everything outside the range is flagged as 'bad'. Give as tuple, e.g. (30, 80)"),
        bandpass_filter=dict(default=None, doc="Optional bandpass filter to multiply with."),
        fft_data=dict(default=lambda self: cr.hArray(complex, dimensions=(self.nantennas, self.nfreq)),
            doc="Fourier transform of timeseries_data_resampled."),
        bad_antennas = dict(default=[],
            doc="Antennas found to be bad.", output=True),
        good_antennas = dict(default=lambda self: [name for name in self.f["SELECTED_DIPOLES"] if name not in self.bad_antennas],
            doc="Antennas found to be good.", output=True),
        flag_range_antennas = dict(default=[0.5,2.],
            doc="Multiples of median power between which an antenna is considered good."),    
        dirty_channels = dict(default=[],
            doc="List of channels found to contain RFI", output=True),
        median_average_spectrum = dict(default=None, doc="Average power spectrum, in ADC units, uncleaned (raw data). Median over antennas is taken; dimension = [nfreq].", output=True),
        median_cleaned_spectrum = dict(default=None, doc="Cleaned power spectrum. Equals average_spectrum, but with dirty_channels set to zero.", output=True),
        average_spectrum = dict(default=None, doc="Average power spectrum (ADC units) per antenna, uncleaned. Dim = [nantennas, nfreq]", output=True),
        cleaned_spectrum = dict(default=None, doc="Cleaned power spectrum per antenna.", output=True),
        phase_average = dict(default=None, doc="Average phases per antenna, per frequency. Can be passed to the RF calibration Task. Dimension = nantennas x nfreq", output=True),
        phase_RMS = dict(default=None, doc="RMS phase spread for each antenna, per frequency. Can be passed to RF calibration Task. Dimension = nantennas x nfreq", output=True),
        median_phase_spreads = dict(default=None, doc="Median over antennas, of the phase spread measure from all blocks. Dimension = [nfreq]", output=True),
        antennas_cleaned_power = dict(default=None, doc="Cleaned power (sum of squares) from cleaned spectrum, per antenna. ", output=True),
        antennas_cleaned_sum_amplitudes = dict(default=None, doc="Cleaned sqrt(power) summed from cleaned spectrum, per antenna. ", output=True),
        save_plots = dict(default=False,
            doc="Store plots"),
        plot_prefix = dict(default="",
            doc="Prefix for plots"),
        plot_type = dict(default="png",
            doc="Plot type (e.g. png, jpeg, pdf)"),
        plot_title = dict(default=True,
            doc="Plot with or without title (for publication)"),
        plotlist = dict(default=[],
            doc="List of plots"),
        plot_antennas = dict(default=lambda self: range(self.nantennas),
            doc="Antennas to create plots for."),
        verbose = dict(default=True, doc="Verbose output."),
        testplots = dict(default=False, doc="Show test plots."),
        apply_hanning_window = dict(default=True,
            doc="Apply Hanning window to data before FFT."),
        hanning_fraction = dict(default=1.0,
            doc="Fraction of the window to Hanning filter (at beginning and end of timeseries) 0.2 is 10% at both ends."),
        dataloss_threshhold = dict(default=10, doc="NOF consecutive zeros in timeseries to be considered due to dataloss."),
        blocks_with_dataloss = dict(default=0, doc="Blocks that have been found to contain dataloss.", output=True),
    )

    def run(self):
        """Run the task.
        """
        np_error_convention = np.geterr()['divide']
        np.seterr(divide='ignore') # ignore divide by zero when doing logarithms etc.

        if self.filename:
            self.f = cr.open(self.filename, blocksize=self.blocksize)  # assume blocksize has been set as well

        nblocks = (self.f["MAXIMUM_READ_LENGTH"] / self.blocksize) - self.startblock
        if self.nofblocks > 0:
            nblocks = min(nblocks, self.nofblocks)  # cap to maximum read length

        if self.verbose:
            print 'Processing %d blocks of length %d' % (nblocks, self.blocksize)
        incphasemean = cr.hArray(complex, dimensions=self.fft_data)  # this leaks memory, per Task instance (?)
        incphase = cr.hArray(complex, dimensions=incphasemean)

        avgspectrum = cr.hArray(float, dimensions=self.fft_data)
        spectrum = cr.hArray(complex, dimensions=self.fft_data)
        magspectrum = cr.hArray(complex, dimensions=self.fft_data)

        n = 0
        skippedblocks = 0
        refant = self.refant  # determine reference antenna from median power; do not rely on antenna 0 being alive...
        self.blocks_with_dataloss = []
        nyquistZone = self.f["NYQUIST_ZONE"][0]
        for i in range(nblocks):
        # accumulate list of arrays of phases, from spectrum of all antennas of every block
        # have to cut out the block with the pulse... autodetect is best?
            if self.verbose:
                print 'Doing block %d of %d' % (i, nblocks)
            self.f["BLOCK"] = i + self.startblock

            # want to discard blocks with spiky signals... see AverageSpectrum? Or a simple max/sigma test?
#            x = f["TIMESERIES_DATA"]
#            maxx = x.max()[0]
#            stdx = x.stddev()[0]
            self.f.getFFTData(self.fft_data, block=i + self.startblock, hanning=self.apply_hanning_window, hanning_fraction=self.hanning_fraction, datacheck=True)
            # Note: No hanning window if we want to measure power accurately from spectrum
            # in the same units as power from timeseries. Applying a window gives (at least) a scale factor
            # difference!
            # But no window makes the cleaning less effective... :(
            spectrum = self.fft_data / self.f["BLOCKSIZE"]  # normalize back to ADC units
            if nyquistZone % 2 == 1: # 1, 3 etc. is unswapped
                spectrum[..., 0] = 0.0  # reject DC component
                spectrum[..., 1] = 0.0  # reject 1st harmonic. NB! May be wrong for getting power estimates !!!
            else:
                spectrum[..., -1] = 0.0
                spectrum[..., -2] = 0.0 # reject the same DC component, but Nyquist-swapped channels...!

            magspectrum.copy(spectrum)
            magspectrum.abs()
            if i == 0 and self.refant < 0:  # in first block, determine reference antenna (which channel has median power)
                magspectrum.square()
                channel_power = 2 * cr.hArray(magspectrum[...].sum())
                even_refant = 2 * np.argsort(channel_power.toNumpy()[0::2])[self.nantennas / 4]
                odd_refant = 1 + 2 * np.argsort(channel_power.toNumpy()[1::2])[self.nantennas / 4]
                even_refant = int(even_refant)  # numpy.int64 causes problems...
                odd_refant = int(odd_refant)
                print 'Taking channel %d as reference antenna' % even_refant
                self.refant = even_refant # hack
                # the index halfway in the sorted array is the channel with median power
                magspectrum.sqrt()
#            magspectrum += 1e-9

            incphase.copy(spectrum)
            incphase /= (magspectrum + 1.0e-9)  # divide abs-magnitude out to get exp(i phi). Avoid div by 0
            evenincphaseRef = cr.hArray(incphase[even_refant].vec()) # improve? i.e. w/o memory leak. Phases of refant channel for all freqs
            oddincphaseRef = cr.hArray(incphase[odd_refant].vec())
            incphase[range(0, self.nantennas, 2), ...] /= evenincphaseRef
            incphase[range(1, self.nantennas, 2), ...] /= oddincphaseRef

            if nyquistZone % 2 == 1:
                incphase[..., 0] = 1.0  # reject DC component and first harmonic.
                incphase[..., 1] = 1.0
            else:
                incphase[..., -1] = 1.0
                incphase[..., -2] = 1.0

            incphasemean += incphase  # accumulate Sum( exp(i phi) ) for all blocks

            magspectrum.square()
#            print 'Power = %f' % (2 * (magspectrum.toNumpy().sum(axis=1))[2])
            avgspectrum += magspectrum  # accumulate average spectrum

            if cr.hCountGreaterThan(self.f.nof_consecutive_zeros, self.dataloss_threshhold).val() > 0:
                print "Warning: Found possible data loss in block {0}".format(i)
#                skippedblocks += 1
                self.blocks_with_dataloss.append([i])
                continue

            if np.isnan(incphase.sum()[0]):  # FIX
                print 'Warning: NaN found! Skipping block (should not happen anymore...!)'
                skippedblocks += 1
                continue

        nblocks -= skippedblocks
        print '# valid blocks = %d' % nblocks
        if nblocks == 0:
            print 'Error: all blocks have been skipped!'
            # may want to exit here

        freqs = self.f["FREQUENCY_DATA"].toNumpy() * 1e-6
        avgspectrum /= float(nblocks)  # normalize
        avgspectrum[..., 0] = 0.0
#        avgspectrum[..., 1] = 0.0 # zero out DC and 1st harmonic (why is 1st harmonic so strong?)
        # No 1st harmonic cut, probably a Hanning window artifact... preserve power.

        self.average_spectrum = cr.hArray(avgspectrum)  # hArray to output param

        incPhaseRMS = cr.hArray(float, dimensions=incphasemean)
        self.phase_average = cr.hArray(float, dimensions=incphasemean)

        cr.hComplexToPhase(self.phase_average, incphasemean)
        incphasemean.abs()
        incPhaseRMS.copy(incphasemean)
        incPhaseRMS *= -1 / float(nblocks)
        incPhaseRMS += 1
        incPhaseRMS.sqrt()
#        incPhaseRMS *= np.sqrt(2.0)  # check...???
        phaseRMS = incPhaseRMS
        self.phase_RMS = phaseRMS
        x = phaseRMS.toNumpy()
        #medians = np.median(x, axis=0)
        evenmedians = np.median(x[np.arange(0, x.shape[0], 2)], axis=0)
        oddmedians = np.median(x[np.arange(1, x.shape[0], 2)], axis=0)
#        medians = oddmedians.copy()

#        for i in range(len(medians)):
#            medians[i] = min(evenmedians[i], oddmedians[i])

#        import pdb; pdb.set_trace()
        evenmedians[0] = 1.0
        oddmedians[0] = 1.0
        evenmedians[1] = 1.0
        oddmedians[1] = 1.0
        self.median_phase_spreads = cr.hArray(evenmedians) # change!
        # print ' There are %d medians' % len(medians)

        # Get average spectrum, median over antennas for every freq channel
        y = np.median(avgspectrum.toNumpy(), axis=0)
        self.median_average_spectrum = cr.hArray(y)
        logspectrum = np.log(y)

        # Get 'dirty channels' for output, and to show in plot
        # Extend dirty channels to both sides, especially when having large blocksizes
        flagwidth = 1 + self.blocksize / 4096
        even_dirty_channels = dirtyChannelsFromPhaseSpreads(evenmedians, flagwidth=flagwidth, testplots=self.testplots)
        odd_dirty_channels = dirtyChannelsFromPhaseSpreads(oddmedians, flagwidth=flagwidth, testplots=self.testplots)

        dirty_channels = []
        for i in range(self.blocksize/2):
            if i in even_dirty_channels or i in odd_dirty_channels:
                dirty_channels.append(i)
#            dirty_channels = [x for x in even_dirty_channels or x in odd_dirty_channels]
#            dirty_channels.append(

        # if a frequency range was given, flag everything outside the range as 'dirty'

        if self.freq_range:  # flagged as dirty or outside range
            alldirtychannels = [int(ch) for ch in range(len(freqs)) if (ch in dirty_channels) or not (self.freq_range[0] < freqs[ch] < self.freq_range[1])]
        else:
            alldirtychannels = [int(ch) for ch in dirty_channels]

        self.dirty_channels = alldirtychannels
        # Get cleaned spectrum per antenna
        cleanedspectrum = cr.hArray(copy=avgspectrum)
        cleanedspectrum[..., cr.hArray(self.dirty_channels)] = 0.0  # seems not to work with hArrays as cleanedspectrum[..., cr.hArray(self.dirty_channels)] = 0.0
#        plt.figure()
#        plt.plot(np.log(cleanedspectrum.toNumpy()[0]))
#        plt.plot(np.log(cleanedspectrum.toNumpy()[20]))
        self.cleaned_spectrum = cleanedspectrum

        # cleanedspectrum[self.dirty_channels] = np.float('nan')

        # Get median cleaned spectrum (over all antennas)
        median_cleaned_spectrum = np.copy(y)
        median_cleaned_spectrum[self.dirty_channels] = 0.0

        if self.bandpass_filter:
            print "Applying bandpass"
            median_cleaned_spectrum *= self.bandpass_filter.toNumpy()

        self.median_cleaned_spectrum = cr.hArray(median_cleaned_spectrum)

        self.amplitude_spectrum = self.cleaned_spectrum.new()
        self.amplitude_spectrum.copy(self.cleaned_spectrum)
        self.amplitude_spectrum.sqrt()

        self.antennas_cleaned_sum_amplitudes =  2 * cr.hArray(self.amplitude_spectrum[...].sum())
        # Sum up power for avg spectrum
        # Factor 2 because of real-FFT.
        self.antennas_cleaned_power = 2 * cr.hArray(self.cleaned_spectrum[...].sum())

#        dirty_power = 2 * cr.hArray(avgspectrum[...].sum() ) - cleaned_power
#        total_power = 2 * cr.hArray(avgspectrum[...].sum() )

        # Get bad antennas from outliers in power
        # Instead of the non-robust k-sigma method, take the range to be from 1/2 to 2 times median power.
        # This has the advantage that it will always catch 0-power channels
        # and that it is robust for up to 50% - 1 outliers!
        median_power = self.antennas_cleaned_power.median()[0]
        channel_ids = self.f["SELECTED_DIPOLES"]
        self.good_antennas = [channel_ids[i] for i in range(self.nantennas) if (self.flag_range_antennas[0] * median_power < self.antennas_cleaned_power[i] < self.flag_range_antennas[1] * median_power)]
        self.bad_antennas = [id for id in channel_ids if id not in self.good_antennas]
        print 'There are %d bad antennas (outliers in cleaned-power): ' % len(self.bad_antennas)
        print self.bad_antennas

        if self.save_plots:

            # Average spectrum (uncleaned)
            plt.clf()
            # Plot only the uncleaned spectrum
#            plt.figure()
            plt.plot(freqs, logspectrum, c='b')
            ax = plt.gca()
            if self.plot_title:
                plt.title('Median-average spectrum of all antennas')
            plt.xlabel('Frequency [MHz]')
            plt.ylabel('Log-Spectral Power [ADU]')
#            self.plot_finish(filename=self.plot_name + "-avgspectrum_withflags",filetype=self.filetype)

            p = self.plot_prefix + "average_spectrum.{0}".format(self.plot_type)
            plt.savefig(p)
            self.plotlist.append(p)


            # Average spectrum (uncleaned)
            plt.figure()
            # plot dirty channels into spectrum plot, in red
            plt.plot(freqs, logspectrum, c='b')
            dirtyspectrum = np.zeros(len(logspectrum))
            dirtyspectrum += np.float('nan')  # min(logspectrum)
            dirtyspectrum[self.dirty_channels] = logspectrum[self.dirty_channels]
            plt.plot(freqs, dirtyspectrum, 'x', c='r', markersize=8)
            if self.plot_title:
                plt.title('Median-average spectrum of all antennas, with flagging')
            plt.ylim(ax.get_ylim())
            plt.xlabel('Frequency [MHz]')
            plt.ylabel('Log-Spectral Power [ADU]')
#            self.plot_finish(filename=self.plot_name + "-avgspectrum_withflags",filetype=self.filetype)

            p = self.plot_prefix + "average_spectrum_flagged.{0}".format(self.plot_type)
            plt.savefig(p)
            self.plotlist.append(p)

            fig = plt.figure()
            ax1 = fig.add_subplot(111)
            ax1.plot(freqs, logspectrum, c='b')
            ax1.set_xlabel('Frequency [MHz]')
            # Make the y-axis label and tick labels match the line color.
            ax1.set_ylabel('Log-Spectral Power [ADU]')

            ax2 = ax1.twinx()
            ax2.plot(freqs, evenmedians, c='r'  ) # HACK! Need some combination of even and odd.
            ax2.set_ylabel('Phase Variance')

            # move to testplots?
            p = self.plot_prefix + "average_spectrum_phasevariance.{0}".format(self.plot_type)
            self.plotlist.append(p)
            plt.savefig(p)

            # Average spectrum (cleaned)
            plt.figure()
            log_cleanedspectrum = np.log(median_cleaned_spectrum)
            log_cleanedspectrum[log_cleanedspectrum <= ax.get_ylim()[0]] = np.nan
            plt.plot(freqs, log_cleanedspectrum, c='b')
            if self.plot_title:
                plt.title('Median-average spectrum of all antennas, cleaned')
            plt.ylim(ax.get_ylim()[0], plt.gca().get_ylim()[1])
            plt.xlabel('Frequency [MHz]')
            plt.ylabel('Log-Spectral Power [ADU]')
            p = self.plot_prefix + "average_spectrum_cleaned.{0}".format(self.plot_type)

            plt.savefig(p)

            self.plotlist.append(p)

        np.seterr(divide=np_error_convention)

