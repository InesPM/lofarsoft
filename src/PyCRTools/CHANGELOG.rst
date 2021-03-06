2012-05-08
==========

  :Author: Martin van den Akker <m.vandenakker@astro.ru.nl>

 - Use column based parameter tables instead of row based (for performance improvement)


2012-04-23
==========

  :Author: Martin van den Akker <m.vandenakker@astro.ru.nl>

 - Added CRDatabase support to cr_event.py pipeline script.

2012-04-04
==========

  :Author: Martin van den Akker <m.vandenakker@astro.ru.nl>

 - Added LORA parameters to database.

2012-04-04 - Rev 9991
=====================

  :Author: Heino Falcke <H.Falcke@astro.ru.nl>

 - Added function :func:`hShift` to mVector.m - this allows one to
   shift a vector left or right (with wrapping) by a fixed
   amount. This can be used for de-dispersion (using the looping
   mechanism) - an example script test_dedispersion.py has also been
   added to demonstrate that.

 - Added :func:`hPow` as a vector function. Also defined the ``**``
   operator for vectors.

2012-03-05
==========

  :Author: Pim Schellart <P.Schellart@astro.ru.nl>

 - Added PulseEnvelope task to calculate pulse envelope using Hilbert transform and find delays between maxima thereof.

2012-03-03
==========

  :Author: Pim Schellart <P.Schellart@astro.ru.nl>

 - Added :func:`hFFTWResample` to quickly and easily (up/down)sample a (periodic) signal.

2012-02-13
==========

  :Author: Heino Falcke <H.Falcke@astro.ru.nl>

 - bug fixes to beamformer.py task

2011-11-07
==========

  :Author: Pim Schellart <P.Schellart@astro.ru.nl>

- Added calibration module with C++ functions for gain calibration using
  the ASTRON provided ElementResponse library with the LOFAR antenna model.
  `hCalibratePolarizationLBA` performs on sky or x,y,z polarization gain
  calibration taking mixing between the dipoles into account.
  `hLBAGain` gives antenna gain for each of the two LOFAR antenna dipoles
  for a given direction and array of frequencies.
  Only for LBA now, will add HBA when output is verified.


2011-09-16
==========

  :Author: Martin van den Akker <m.vandenakker@astro.ru.nl>

- Added version 1.1.0 of the AERA Datareader. Now supports different
  AERA data formats.
- Updated build scripts for AERA Datareader for easier updating of
  newer versions of the AERA Datareader.


2011-09-08
==========

  :Author: Arthur Corstanje <a.corstanje@astro.ru.nl>

- Apply gain calibration normalizing the RMS noise level inside the data block to 1.0.
  This fixes a possible remaining spread of 5-10% in noise power as seen after processing some event files.
- Don't break on valid LORA file but missing entry for this event.
- Don't assume the pulse is right in the middle of the block when no sample number is given.
- Do DC offset removal before calculating baselines.
- Calculate average spectrum by taking average power (i.e. abs-square of FFT), then square root.
  The difference with taking a mean-absolute spectrum is small as long as all the added spectra
  are all close to each other.


2011-08-17
==========

  :Author: Martin van den Akker <m.vandenakker@astro.ru.nl>

- Added functionality of Endpoint radiation methods as implemented by
  Koert Poelman.


2011-08-13 - Rev 8292
=====================

  :Author: Heino Falcke <H.Falcke@astro.ru.nl>

- added a variant of hSet where one can set elements in an array to
  elements in another array (using a index list) ``array[[i_0,I1,..]]=[e_0,e_1,....]``
- plot and hArray can now accept errobars with the keywords yerr, and
  xerr

2011-08-08 - Rev 8266
=====================

  :Author: Heino Falcke <H.Falcke@astro.ru.nl>

- better treatment of direction fitting in cr_events.py - only pulses
  with enough SNR are taken into account.

- various bugfixes

=====================

  :Author: Heino Falcke <H.Falcke@astro.ru.nl>

- cr_event.py can be run in batch mode without window, if run from
  command line with the -R option

- LocatePulseTrain can now optionally search in individual antennas
  for pulses

- cr_event.py uses the above option in case no LORA information is
  present, this should improve recovery of fainter pulses

- a summary html page is written in the event and results directory,
  output and inter-linking of html pages is improved,  the time delays
  are used for a simple quality control of the reconstruction, the
  pipeline can be rerun such that existing files are skipped, some
  basic checking of basic data file integrity is possible (size)

2011-08-04 - Rev 8240
=====================

  :Author: Heino Falcke <H.Falcke@astro.ru.nl>

- added a task_logger which keeps track of execution times, use 'tlog'
  to view

- cr-event - deal with weaker pulses
   -  localize pulses firs in incoherent beam towards lora position, if present
   -  made a two-step approach for getting directions: first
      from maxima in pulses, then from cross correlations


2011-08-03 - Rev 8230
=====================

  :Author: Sander ter Veen <s.terveen@astro.ru.nl>

- cr_event.py now:
   - specify lora directory seperate from filedir (defaults to altair directories) (--loradir)

- tasks/plotfootprint.py
   - runs with specification of filefilter and polarisation only
   - output plot by default in eventname/pol?/ directory

2011-08-03 - Rev 8233
=====================

  :Author: Heino Falcke <H.Falcke@astro.ru.nl>

- cr_event.py loops over polarizations as well
- additional flagging in fitbaseline and cr_event to check for unusual
  spectra
- flagging of delays implemented, new parameters pulses_strength
  (read from a fixed location in shifted and smoothed time series
  data), pulse_location, etc. in
  results.py of cr_event.py

2011-08-03 - Rev 8224
=====================

  :Author: Heino Falcke <H.Falcke@astro.ru.nl>

- cr_event.py now:
   - loops over files
   - adheres to new sub-directory structure TIMESTAMP/pol/station/
   - uses information from lora and saves it in results.py
   - has a couple of bugs fixed (e.g. baseline mutiplication)
   - new parameter nsigma for peak finding

- make_ldf2 is a slightly cleaner version for ldf plotting, making use of
  the new information in results.py and using the new directory
  structure

- loraTimestampToBlocknumber  now returns block AND samplenumber as a tuple

2011-08-02 - Rev 8218
=====================

  :Author: Heino Falcke <H.Falcke@astro.ru.nl>

- LocatePulseTrain now makes plots, accepts a narrowed search-window,
  and use the sqrt of the power (i.e. E-field) for searching for pulses
- cr_event pipeline accepts search window as parameter (-x/-X) and
  normalizes all stations to the same level.
- thelp accepts a taskname as parameter to get help on a task

2011-07-25 - Rev 8206
=====================

  :Author: Sander ter Veen <S.terVeen@astro.ru.nl>

- added lora.loraInfo(UTC_sec,datadir) to read in loradata in a dictionary

2011-07-22 - Rev 8193
=====================

  :Author: Heino Falcke <H.Falcke@astro.ru.nl>

- added tbb_spectra.py with command line options (optparse sucks ...)
  to make dynamic and average spectra


2011-07-22 - Rev 8188
=====================

  :Author: Heino Falcke <H.Falcke@astro.ru.nl>

- created a new core file: utils.py

- added a class hPrint which allows one to create hprint functions
  that can be used to reroute output also to a logfile and to an
  internal texbuffer

- averagespectrum mow produces pretty output in an html file

- Tasks have a method ``writehtml`` which allows one to produce pretty
  output automatically.

2011-07-21 - Rev 8177
=====================

  :Author: Martin van den Akker <m.vandenakker@astro.ru.nl>

- Added shortcuts to sc namespace for tasks
- Added pycrtools to cr namespace for tasks


2011-07-21 - Rev 8176
=====================

  :Author: Heino Falcke <H.Falcke@astro.ru.nl>

- Revised :func:`averagespectrum` to also produce dynamic spectra and
  run over multiple files separately (adn produce anverage and dynamic
  spectra from all files).
- Fixed a bug in :func:`hCopy`.
- Minor bug in :func:`fitbaseline` (vector size was defined with list
  as argument).

2011-7-19  - Rev 8162
=====================

  :Author: Sander ter Veen <S.terVeen@astro.ru.nl>

- Added a python version test for the ``.format`` statement. This is not available
  in python 2.5 (i.e. at Cep I cluster). Format will print a tuple instead. It's
  ugly, but then the software works.
- Fixed `averagespectrum.py` to work with import pycrtools as cr


2011-07-18 - Rev 8151
=====================

  :Author: Sander ter Veen <S.terVeen@astro.ru.nl>

- Changed import in ``grid.py`` to be :func:`gregoriandata2jd` instead
  of gregorian2jd.


2011-07-18 - Rev 8150
=====================

  :Author: Sander ter Veen <S.terVeen@astro.ru.nl>

- Added ``pow(hnumber, long int)`` in ``core.h`` to get ``hPowerLaw``
  functions on some linux systems including the CEP I cluster.


2011-07-17 - Rev 8133
=====================

  :Author: Heino Falcke <H.Falcke@astro.ru.nl>

- added :func:`trerun` to store and rerun task instances (like trun)
  without recreating them

- task averagespectrum can now also form an incoherent sum of time
  series data for all antennas, fixed some bugs

- cr_event pipeline further developed, more graphs added, fixed
  delay fitting, normalized to Galactic powerlaw shape

- added functions hPowerLawMul, hPowerLawAdd, hLinearFunctionMul,
  hLinearFunctionAdd, hLogLinearFunctionMul, hLogLinearFunctionAdd


2011-07-14 - Rev 8123
=====================

  :Author: Heino Falcke <H.Falcke@astro.ru.nl>

- writing of the parfiles of tasks can be controlled with the
  parameters tasks.task_outputdir and tasks.task_write_parfiles

- added a plotfinish class, who's instances are called after each plot
  to write figues to disk and pause for user interaction if desired.

- cr_event writes output into one common subdirectory and creates a
  summary html file


2011-07-11 - Rev 8101
=====================

  :Author: Heino Falcke <H.Falcke@astro.ru.nl>

- added testcr4.py to do an almost end-to-end radio-only CR pipeline
- AverageSpectrum revised to use new data reader

2011-07-07 - Rev 8083
=====================

  :Author: Heino Falcke <H.Falcke@astro.ru.nl>

- created new tasks PlotDirectionTriangles, PlotAntennaLayout
- added ``Vec.__rmul__``, etc. to also allow vectors/arrays to be on the
  right hand  of an operand like ``*``, ``-``, ``/``, ``+``
- added a ``vec_p->reserve()`` operation in harray.cc to avoid creating
  vectors with unitialized storage (and hence null pointers ...)

2011-06-22 - Rev 7944
=====================

  :Author: Heino Falcke <H.Falcke@astro.ru.nl>

- Took out casa definitions from ``core.h`` and put it into ``casa.h``
- Started some revisions to allow for ``HNumber = long double`` (not yet
  working, due to FFTw)

2011-06-22 - Rev 7930
=====================

  :Author: Heino Falcke <H.Falcke@astro.ru.nl>

- Added :func:`hFindOdd`, :func:`hFindEven`,
  :func:`hCountOdd`, :func:`hCountEven`
- Note that those functions also accept arrays of strings as input (which then
  get converted to integers)
- adapted array.Select to take "odd" and "even" as operator
- ``tbb.py`` also accepts ``hArrays`` as input to select antennas and you can
  simply say "odd" or "even"

2011-05-27
==========

  :Author: Martin van den Akker <m.vandenakker@astro.ru.nl>

- Added pycrtools configuration to CMake.
- Moved Numpy functionality to Math and Vector modules.
- Moved AERA_IO functionality to IO module.


2011-05-14 - Rev 7703
=====================

  :Author: Heino Falcke <H.Falcke@astro.ru.nl>

- Introduced 'ERROR_RETURN' and 'ERROR_RETURN_VALUE' for C++ to throw standardized error
  messages and return from the function.
- streamlined  :func:`hDotProduct`
- Added :func:`hFillRangeVec`  to fill an array of vectors with some suitable
  numbers (start and increment values are then also vectors)
- renamed the python methods array.transpose to array.Transpose to not
  shadow the :func:`hTranspose` method
- added function :func:`hSkewLinesDistanceToClosestApproach`
- renamed :func:`hArray_find_locations` to :func:`hArray_Find`
- added function :func:`hArray_Select` to select elements in a list
- renamed :func:`hArray_transpose` to
  :func:`hArray_Transpose`. Accrodingly the method also starts with
  upper case.
- added testskewlines.py added testskewlines2.py to test distance finding
- added testpulsecal2.py to phase-calibrate on a pulse
- New task :class:`DirectionFitTriangles` which will fit the direction
  of a source and also allows one to iterate on cable delays to get a
  consistent solution.
- added :func:`hShiftFFT` to shift a data set by fractional samples in
  the FFT domain
- :class:`CrossCorrelateAntennas` uses :func:`hShiftFFT` to produced
  oversampled cross-correlations that make it easier to locate the
  peak.


2011-05-03 - Rev 7650
=====================

  :Author: Heino Falcke <H.Falcke@astro.ru.nl>

- added python method find_locations to get indexlist of values above/below/equal/between thresholds
- deleted :func:`ADC2Voltage`, :func:`hRFIDownsampling`, :func:`hRFIBaselineFitting`,
  :func:`hRFIFlagging`, :func:`hRFIMitigation` from mRF.cc which depended on the old CRTools.
- added :func:`hEndPointRadiation` (moved by Martin to a new file already)
- added :func:`hDirectionTriangulation`,
  :func:`hDirectionTriangulations` to calculate arrival directions
  from a set of triangles of antennas and arrival times (e.g. of
  pulses).
- added script testpulsecal.py to describe some calibration on pulses


2011-05-02 - Rev xxxx
=====================

  :Author: Pim Schelart <P.Schellart@astro.ru.nl>

- Moved frequency range selection out of the :mod:`tbb` datareader module for simplicity.
- Added support for skipping frequency channels to the imager.
- Added :func:`hMaskToStep` to convert mask of ones and zeros to shifts to step through array.
- Added :func:`hCountZero` to count the number of zero entries in an array.
- Added :func:`hCountNonZero` to count the number of non-zero entries in an array.


2011-05-01 - Rev xxxx
=====================

  :Author: Pim Schelart <P.Schellart@astro.ru.nl>

- Added :func:`hFFTConvert` to convert between FFTW and FFT CASA results.


2011-04-18 - Rev 7573
=====================

  :Author: Heino Falcke <h.falcke@astro.ru.nl>

- Added :func:`hElem`, :func:`hFirst`, :func:`hLast` to return
  elements of a vector (can than be used in conjunction with looping).
- Added function :func:`trun` to execute a task by name right away
- Added Tasks in ``pulsecal.py``:

  - :func:`LocatePulseTrain` in time series.
  - :func:`CrossCorrelateAntennas` - to crossorrelate a set of time series.
  - :func:`FitMaxima` - to fit the maximum of a pulse/peak precisely.



2011-04-12 - Rev 7546
=====================

  :Author: Martin van den Akker <m.vandenakker@astro.ru.nl>

- Added tutorial of how to write C++ code for python wrappers.
- Added tutorial of how to write documentation for the C++ code.


2011-04-13 - Rev 7548
=====================

  :Author: Heino Falcke <h.falcke@astro.ru.nl>

- Added :func:`hSquareAdd` to calculate the power of a real
  (timeseries) array and add it to a vector (like spectralpower for
  complex data).
- Added :func:`hMulAddSum` to add two larger vectors and sum them onto
  a smaller output vector.
- Added :func:`hMaxInSequences`, :func:`hMinInSequences`,
  :func:`hSumInSequences`, :func:`hMeanInSequences`,
  :func:`hStdDevInSequences` to work with the sequences provided by
  :func:`hFindSequence`.
- Started a new module :mod:`rftools` to contain some simple and
  common operations needed in radio frequency interferometry,
  calibration and pulse detection.
- Functions added to :mod:`rftools`: :func:`TimeBeamIncoherent`,
  :func:`LocatePulseTrain`.
- Added :func:`SpectralPower2` to give the actual spectral power (squared).
- Fixed ``testcr3`` and ``testrfi4``.
- Fixed a bug for ``hArray_write`` - stringarrays in dicts will be
  replaced by lists.


2011-04-09 - Rev 7518
=====================

  :Author: Heino Falcke <h.falcke@astro.ru.nl>

- ``testcr3.py`` now works on a LOPES and a LOFAR dataset (not yet
  including RFI, baselinefitting etc.).
- Changed ``beamformer.py`` and ``qualitycheck.py`` to accept new keywords
- ``tbb.py`` and ``datareader.py`` synchronized to treat keywords in a
  similar way.
- Made sure that wrappers for :func:`hFill`, :func:`Set`,
  :func:`Find`, :func:`Flip`, :func:`Copy`, :func:`hSort`,
  :func:`hZipper`, :func:`ReadFileBinary`, :func:`WriteFileBinary`,
  :func:`hWriteFileBinaryAppend`, :func:`Redistribute`,
  :func:`PPrint`, :func:`hPrettyString`, :func:`ReadFileText`,
  :func:`hWriteFileText`, :func:`Transpose` are also generated for
  string and boolean vectors.


2011-04-06 - Rev 7508
=====================

  :Author: Heino Falcke <h.falcke@astro.ru.nl>

- FindPython first finds Enthought python (EDP) version also for
  libraries and includes.
- Put in a flag to plot semilog graphs also with buggy EDP64 version
  on new Macs ...  If your name is Heino and you have a MacBook more
  recent then March 2011 with OS10.6.7 and you use the Enthought 64 bit
  (EDP64) Python version 6.3 then set ``plt.EDP64bug=True`` to avoid
  problems with semilog axes. Semilog will then work only with reduced
  functionality.
- ``io/tbb.py`` object - made keyword access a little more elegant.
- By default tasks now return themselves rather than the workspace
  object (uness an explicit return value is given in ``.run``)!!
- :func:`hRandomizePhase`.
- Finished ``testcr3.py`` to demonstrate RFI excision and beamforming.


2011-04-05
==========

  :Author: Martin van den Akker <m.vandenakker@astro.ru.nl>

- Update of pycrtools documentation generation: Added description,
  references and examples in the correct formatting.


2011-03-30 - Rev 7432
=====================

  :Author: Heino Falcke <h.falcke@astro.ru.nl>

- Changed behaviour of :func:`hMulAdd2` / :func:`hMulDiv2` /
  :func:`hMulSub2`, so that 1st operand is wrapped if shorter than the
  2nd.
- A new method to hArrays: ``ary[0,0,etc.].array()`` will return a
  copy of the slice of the original hArray.
- Added a new version of :func:`hRunningAverage` that can operate on
  the same vector.
- Checked and bugfixed the different modes of the beamformer task
  seems to work now.
- Also related bugfixes in :mod:`averagespectrum`,
  :mod:`dynamicspectrum`.


2011-03-30 - Rev 7410
=====================

  :Author: Heino Falcke <h.falcke@astro.ru.nl>

- Fixed ``testcr2`` to work again.
- Uploaded ``/data/lopes/2004.01.12.00:28:11.577.event`` as testfile.
- First working version of a BeamFormer task - works with LOPES data.
- A draft and completely incomplete version of a conversion routine
  ``convert`` for coordinates.


2011-03-29 - Rev 7387
=====================

  :Author: Heino Falcke <h.falcke@astro.ru.nl>

- :func:`harray.plot` now better handles sliced *yvalues* in
  combination with *xvalues* of different dimensions. I.e. you can
  have a 2D array with 1D xvalues.
- Introduced :func:`hBSplineCalcAssign` with :func:`hBSplineCalc` as
  wrapper function for compatibility with the documentation generation
- Changed parameter ``maxchunk`` to ``maxnchunks`` in
  :mod:`averagespectrum`,
- Added a new task to calculate a (incohrent) dynamic spectrum from a
  number of files and to plot


2011-03-26 - Rev 7372
=====================

  :Author: Heino Falcke <h.falcke@astro.ru.nl>

- Added :func:`hInverse` to calculate inverse of a vector.
- :func:`hPolynomialMul` / :func:`hPolynomialAdd` and
  :func:`hBSplineCalcAdd` / :func:`hBSplineCalcMul` to add or multiply
  polynomial/spline to output vector - useful for iteratively
  calculating a baseline
- Took out the bspline functions without providing an order.
- BSpline functions now also excplicitly need ``xmin``, ``xmax``
  parameter (to avoid scaling errors).
- BSplineCalc made faster - had to copy GSL routines (e.g.,
  gsl_bspline_eval_nonzero) from more recent version (1.14) into
  source code - I have 1.11.
- ``tasks/fitbaseline.py`` debugged and improved.


2011-03-24 - Rev 7365
=====================

  :Author: Pim Schellart <p.schellart@astro.ru.nl>

- Added FFTW module with wrappers for the fftw advanced routines
  including plan storage.


2011-03-18 - Rev 7336
=====================

  :Author: Heino Falcke <h.falcke@astro.ru.nl>

- Fixed some indentation errors.
- Renamed ``listfiles`` to ``listFiles``.
- Added :func:`readParfiles` to ``datareader.py`` - to return a dict from a
  parameter file.
- WorkSpace class definition now accepts a python parameterfile to set
  variables in a workspace. Useful to control a pipeline script, eg::

    ws=tasks.WorkSpace("MyPipeline",parfile="~/LOFAR/work/parameters.par")()

  and the ``ws.x``, ``ws.y= ...``
- In workspace explicitly named paramers in call to workspace take
  precedence over parfile parameters.
- Introduced ``pardict=keyword`` for :mod:`WorkSpaces` and :mod:`Tasks`
  which can contain parameters of multiple tasks.



2011-03-16 - Rev 7331
=====================

  :Author: Heino Falcke <h.falcke@astro.ru.nl>

- Further fix of :func:`hArray_setitem` bug.


2011-03-16 - Rev 7321
=====================

  :Author: Heino Falcke <h.falcke@astro.ru.nl>

- Fixed :func:`hArray_setitem` which had indentation errors.
- Added compile script again to compile hftools.
- Added :func:`hPrettyString` to process a slice and and to print begin and
  end of a vector. Replaces internals of Python :func:`VecToString`
  function. Old version was making copy of vector and crashed if memory
  was scarce.


2011-03-15 - Rev 7308
=====================

  :Author: Martin van den Akker <m.vandenakker@astro.ru.nl>

- Added autogenerated documentation from hftools to python
  documentation.


2011-03-14 - Rev 7302
=====================

  :Author: Heino Falcke <h.falcke@astro.ru.nl>

- Added :func:`hMin`/:func:`hMax`: will perform a
  ``min`/``max(e_i,val)`` for all elements ``e_i``.
- Added :func:`hRandomizePhase` to randomize phase of selected complex
  numbers and set amplitude to a certain value.
- :func:`hMeanAbs` now also works for complex vectors.
- Added :func:`hRandomizePeaks` to replace peaks in a time series data
  set by random values.
- Tasks will automatically return its own workspace (i.e. with all
  parameters) - that gives best access to all input and output values.
- Added ``testrfi4.py`` to demonstrate RFI cleaning of LOFAR station
  data.
- ``calcbaseline``: will now create the inverse of the baseline (so
  you can just multiply, which in principe is the faster operation).


2011-03-11 - Rev 7300
=====================

  :Author: Heino Falcke <h.falcke@astro.ru.nl>

- Added the filename to the header dicts of hArrays read with
  :func:`crfile`.
- :func:`writeheader` will take parameter ``nbands`` from the array
  ``par``.
- ``array(0,3,3,[0,2,3])=value`` is now possple, i.e. setting elements
  with an index list as last index.
- :func:`fitbaseline` tasks can save results to file.
- Added ``root_filename`` to ``datareader.py`` to get filename without
  ending ``pcr``.


2011-03-11 - Rev 7272
=====================

  :Author: Heino Falcke <h.falcke@astro.ru.nl>

- :mod:`averagespectrum` contains a ``qplot`` method which allows one
  to quickly view blocks that were flagged.
- More quality information is returned and printed,
  e.g. ``Task.homogeneity_factor`` should tell one quickly whether
  there was some problem with the data.
- Added more input and output parameters to control quality checking.


2011-03-11 - Rev 7266
=====================

  :Author: Heino Falcke <h.falcke@astro.ru.nl>

- Further tweaks to :mod:`averagespectrum`:

  - More efficient read-in for smaller files
  - Function ``qplot`` (method to :mod:`AverageSpectrum`) to quickly
    plot flagged blocks.

- New parameters antenna_star and stride to, e.g., read odd/even
  antennas only.
- Quality check to use median instead of mean to estimate the best RMS
  limit.
- Support of 0.10 ipython version to access global variable ``Task``.


2011-03-08 - Rev 7231
=====================

  :Author: Heino Falcke <h.falcke@astro.ru.nl>

- Added a method to update the (output) header in an array from within
  a task in a definde way, applied it to :mod:`averagespectrum`.


2011-03-08 - Rev 7230
=====================

  :Author: Heino Falcke <h.falcke@astro.ru.nl>

- Various bugfixes for :mod:`averagespectrum`, :mod:`tasks`, and
  :mod:`plot`.


2011-03-07 - Rev 7226
=====================

  :Author: Heino Falcke <h.falcke@astro.ru.nl>

- Plot can now deal with plotting sliced arrays where ``xvalues`` are
  being sliced in the same way.
- :mod:`averagespectrum` can now also do an ordinary average spectrum
  (not using a double fft).


2011-03-07 - Rev 7223
=====================

  :Author: Heino Falcke <h.falcke@astro.ru.nl>

- Moved :func:`tshortuts` to tasks/shortcuts and adapted imports.
- :func:`tget`/:func:`tput` now accept an additional name under which
  to store the parameters.
- Added the ``fitbaseline.py`` task.


2011-03-05 - Rev 7217

  :Author: Heino Falcke <h.falcke@astro.ru.nl>

- Allowed tasks to return a value (just make ``.run`` return a value).
- Implemented :func:`hMinStdDev` to calculate the minimum standard
  deviation of a vector within blocks of a certain length. Used to find
  the RMS in the cleanest part of a spiky data set.
- Implemented an improved version of ``fitbaseline`` as a task.
- Added :func:`hBSplineCalc` to calculate a spline without having to
  provide powers of `x`.
- Moved ``core/types.py`` to ``core/htypes.py``.
- Move task commands from config to ``modules/__init__``.
- Further rearrangements to straighten out importing.
- Defined ``tasks.set_globals`` which lets one manipulate variables in the
  (interactive) global namespace.
- The currently loaded task can now be accessed through the variable
  :mod:`task`.
- Added :func:`setHeader` and :func:`getHeader` functions to hArrays.
- Moved :func:`FitBaseline` and :func:`CalcBaseline` to separate file
  ``fitbaseline.py``.
- :mod:`averagespectrum` task is named AverageSpectrum now.
- ``hArray``:

  + Changed the parameter ``par`` in ``hArray(par=)`` to accepte dicts.
  + Changed data format of ``hArray`` files to be written to a directory and
    to include vectors and arrays in "par" as binary files

    .. note:: This means you can store an entire set of hArrays in one
       file - e.g., if you write ary to disk, just store another array
       in ``ary.par`` or store it with :func:`ary.setHeader`.


2011-03-04
==========

  :Author: Martin van den Akker <m.vandenakker@astro.ru.nl>

- Replaced awk parser scripts by a python script.
- The python script generates:

  a. The wrapper code for the python bindings.
  b. Documentation in both doxygen and sphinx format.


2011-02-20 - Rev 7128
=====================

  :Author: Heino Falcke <h.falcke@astro.ru.nl>

- Minor updates and bugfixes to tasks.
- Added ``listfiles`` to get a list of files, using Unix-style file
  patterns and evironment variables.
- Made :mod:`averagespectrum` to work on multiple files.
- Added :func:`spikeexcess` to qualitycheck routine (the one in
  ``qualitycheck.py``), checks only for too many spikes.
- Allowed variables ``nsigma``, such that sigma is fixed to give a
  fixed number (e.g. one) of peaks for the given blocklength.


2011-02-17 - Rev 7108
=====================

  :Author: Heino Falcke <h.falcke@astro.ru.nl>

- Finalized tasks, various bug fixes, added positional parameters.
- Added documentation to tasks.


2011-02-15 - Rev 7103
=====================

  :Author: Heino Falcke <h.falcke@astro.ru.nl>

- Major rewrite of the tasks module, now also include workspaces.
- Added the module ``averagespectrum`` which calculates an average
  station spectrum and is implemented as a new task -configure.py
  contains a number of shortcut.
- Added function :func:`IO.getFilenames` to retrieve a list of files
  in a directory with a certain start and/or ending.
- Changed :func:`hArray.__repr__` to produce something one can read back
  in.


2011-02-07 - Rev 7011
=====================

  :Author: Sander ter Veen <s.terveen@astro.ru.nl>

- Changed calibration data path to ``/data/lofar/StaticMetadata/``.
- Cron job running on own machine to keep this synchronised with the
  LOFAR repository.
- Updated ``metadata.py`` to include the new path.
- Phase calibration for most stations now also available.


2011-02-03 - Rev 6977
=====================

  :Author: Heino Falcke <h.falcke@astro.ru.nl>

- Minor bug fixes, writeheader (take sliced arrays into account, allow
  specifying dimensions explicitly).
- Updated and tested :func:`stationspectrum`.


2011-02-03 - Rev 6957
=====================

  :Author: Heino Falcke <h.falcke@astro.ru.nl>

- Added error messages to read/write functions in ``mIO.cc``
- Fixed a bug in :func:`hWriteFileBinary` which would not write a
  block if the file did not exist yet.
- hArrays now contain a header dict, which can be assigned at creation
  or is inherited upon reading from a datafile and can be extended at
  will.
- The header information is stored (and retrieved) with
  :func:`hArray_writeheader` etc.


2011-02-03 - Rev 6952
=====================

  :Author: Heino Falcke <h.falcke@astro.ru.nl>

- Added :func:`hArrayRead`, :func:`hArray_write`, and
  :func:`hArray_writeheader` this allows one to write an hArray with a
  header file (``filename.hdr``) to disk that contains dimensions and
  other info. It can be read in with :func:`hArrayRead`.


2011-02-01 - Rev 6914
=====================

  :Author: Heino Falcke <h.falcke@astro.ru.nl>

- Renamed ``Write/ReadDump`` to ``Write/ReadFileBinary``.
- Renamed ``Write/ReadTextTable`` to ``Write/ReadFileText``
- Added ``WriteFileBinaryAppend`` to append a block of data.
- ``Write/ReadFileBinary`` have a changed parameter ``start`` rather
  than ``block``, which allows one to specify the starting position in
  units of the elements in the vector rather than in unist of the
  vector size. This allows simple random access read and write.


2011-01-27 - Rev 6860
=====================

  :Author: Heino Falcke <h.falcke@astro.ru.nl>

- Added :func:`hWriteTextTable` to write data to a text file.
- Changed parameters in :func:`hReadTextTable`.


2011-01-26 - Rev 6832
=====================

  :Author: Heino Falcke <h.falcke@astro.ru.nl>

- Added :func:`hReadTextTable` to read data from a textfile.
- added :func:`hSplitString` (not in Python) to split a string into
  words.


2010-12-22
==========

  :Author: Martin van den Akker <m.vandenakker@astro.ru.nl>

- Added support for AERA datareader.


2010-12-14
==========

  :Author: Martin van den Akker <m.vandenakker@astro.ru.nl>

- Added :func:`writeRaw` and :func:`readRaw` method for hArrays.
  These create a raw string which is a memcopy of the data in the
  (flat) hArray vector. This can be used to write and read hArray data
  with pickle in Python.


2010-12-03
==========

  :Author: Martin van den Akker <m.vandenakker@astro.ru.nl>

- Removed ``using namespace`` from headers.
- Added :func:`stringToUpper` and :func:`stringToLower` methods to
  core functionality.


2010-11-22 - Rev 6442
=====================

  :Author: Heino Falcke <h.falcke@astro.ru.nl>

- Work around so that the data reader works again and can read ``FX``
  with selected antennas.
- Removed a bug in :func:`hMul2` that came in due to an accidentaly
  copied line.
- Added ``Pymacs`` to the extras, which allows one to run Python under
  (aqua)emacs with tab completion (etc).
- Start to add highlighting to plot (not yet tested).


2010-11-22 - Rev 6350
=====================

  :Author: Heino Falcke <h.falcke@astro.ru.nl>

- Added function :func:`hFindSequenceGreaterThan` etc. to find
  sequences of data that is above (or below) a certain value - useful
  for identifying peaks in the data.
- Added documentation about :func:`hFindSequence` to tutorial.
- Fixed a few inconsistencies in :func:`hFileRead` to now allow using
  looping.
- Fixed bugs in the tutorial (no idea how they got there).
- ``hArray.plot()`` now also passes normal scipy plotting keywords to
  ``plt.plot()`` like ``color="red"``.
- Added philosophy and history of pycrtool presentation to doc
  directory for reading.


2010-11-17 - Rev 6340
=====================

  :Author: Heino Falcke <h.falcke@astro.ru.nl>

- Found a number of bugs.
- Added short doc string to python wrapper.
- Rearranged some parts of the software (moving code from ``.h`` to ``.cc``
  files).
- Added ``testseti7.py``.


2010-11-17 - Rev 6332
=====================

  :Author: Heino Falcke <h.falcke@astro.ru.nl>

- Modified preprocessor to produce forward declarations of functions
  (this assumes all non pass-as-reference parameters are defined as
  constant), this should allow moving fucntions from ``.h`` file back
  to the ``.cc`` file.
- Removed the obsolete ``SLICED`` types from ``hfppnew.h``.
- Deleted a number of warnings in ``mMath``.
- Fixed a few bugs here and there (e.g., inconsistent
  :func:`hApplyFilter` declaration).
- Introduced ``HFPP_FUNC_IS_INLINE`` to indicate that a function is
  defined as inline.
- Introduced ``HFPP_CLASS_STDIT`` as a type of wrapper to be
  generated. This is used for forward declarations and essentially now
  assumes that by default all vector functions are defined using
  templated iterators (which they de facto already are by convention
  so far).


2010-11-15 - Rev 6304
=====================

  :Author: Sander ter Veen <s.terveen@astro.ru.nl>

- Added ``AntennaPositions`` keyword to ``IO.py``. This gives the
  antenna positions in the local coordinate relative to the CS002
  core.


2010-11-08 - Rev 6257
=====================

  :Author: Heino Falcke <h.falcke@astro.ru.nl>

- ``.read`` method allows so specify a block *and* a single antenna
  selection.
- Changed :func:`DoubleFFT` to allow for blocks to be stored on disk
  (example not yet finished!).
- Several bug fixes.
- Got ``testrfi3.py`` to work again (``testrfi2.py`` doesn't).
- Reinserted import plot.
- Modified readdump to allow reading in a chunk of data only.


2010-11-05
==========

  :Author: Martin van den Akker <m.vandenakker@astro.ru.nl>

- Module gets rebuild if *any* of the header files changes.


2010-11-04 - Rev 6231
=====================

  :Author: Heino Falcke <h.falcke@astro.ru.nl>

- Changed the ``.read`` method to allow providing a block or even a
  list of block numbers that are to be read sequentially into a
  matrix.
- ``.read`` can now also read into array of different type (e.g. read
  ``Fx`` data into complex arrays right away).
- Fixed bugs that became obvious when running the tutorial (which is
  slightly updated - figureless version only).
- Deleted some unnecessary error messages in the ``hftools`` (if it
  doesn't crash or hang it is not an error but a feature ...!).
- Updated the preprocessor and added a keyword
  ``HFPP_PAR_IS_UNMUTABLE_SCALAR`` in the wrapper definition which
  ensures that a parameter is not turned into a vector when called in
  looping mode.


2010-11-04 - Rev 6230
=====================

  :Author: Heino Falcke <h.falcke@astro.ru.nl>

- Interchanged the arguments ``blocklen`` and ``nblocks`` for
  :func:`doubleFFT` to be in the same order as used for defining the
  corresponding matrix.
- Added :func:`hChiSquared` and :func:`hMeanChiSquared` functions.


2010-11-03 - Rev 6228
=====================

  :Author: Heino Falcke <h.falcke@astro.ru.nl>

- Added :func:`hTranspose` - to transpose a vector on the C++ level.
- Adapted :func:`hArray_tranpose` to use :func:`hTranspose` with
  hArrays.
- Added Functions :func:`hDiffSum` and :func:`hDiffSquaredSum` to
  caluclate the (sum of the (squared)) differences of two vectors.
- Changed :func:`hFill` to allow filling one vector with a second
  vector of different type.
- hArrays can now be created with a copy from another vector of
  different type (use the ``copy=other_array`` or ``fill=other_array``
  keywords)
- Added :func:`hDoubleFFT`, :func:`hDoubleFFTPhase`,
  :func:`hDoubleFFTPhaseMul` to do a double FFT based on Arthur's
  numpy example.
- ``testDoubleFFT.py`` shows an example testing the double FFT.
- Created a separate ``plot.py`` module in core, as this was not working
  properly in the new structure.
- Moved the list of hftools function to be added as methods to harrays
  and vectors to ``type.py``.



2010-08-26 - Rev 5836 +
=======================

  :Author: Sander ter Veen <s.terveen@astro.ru.nl>

- Corrected :func:`hMulAdd2`.
- Added ``IO`` and ``calibration`` module.
- Added ``pipeline_start.py`` script for the first basic steps using these
  modules.


2010-08-26 - Rev 5765
=====================

  :Author: Heino Falcke <h.falcke@astro.ru.nl>

- Added :func:`hFlip` to reverse the order of elements in a vector.


2010-08-26 - Rev 5751
=====================

- Added :func:`hMulAdd2` which will loop over the 2nd rather than
  the 1st parameter in looping mode


2010-08-25
==========

  :Author: Heino Falcke <h.falcke@astro.ru.nl>

- Changed :func:`hFileRead` to accept iterate input and hence to read
  data into the array starting at any location.


2010-06-07 - Rev 5064
=====================

  :Author: Heino Falcke <h.falcke@astro.ru.nl>

- Added global function :func:`trackHistory` - to switch history
  tracking in arrays on or off.


2010-06-07 - Rev 5057
=====================

  :Author: Heino Falcke <h.falcke@astro.ru.nl>

- Changed downsample routines to work with fractional block sizes.
- Changed RFI routines to check for too large number of bins (so
  smaller blocksizes will also work).


2010-06-01
==========

  :Author: Heino Falcke <h.falcke@astro.ru.nl>

- Added convenience function :func:`hInvFFTwSave` to do a save fft.
- Added :func:`hMinPos`, :func:`hMaxPos` - returning position of
  maxium or minimum.
- Added calculation of time delays with cross-correlation from CR
  data.
- Remaining: some not-understood behaviour of the inverse fftw.


Todo
====

- Change :func:`hAllTypes`, :func:`hArrayTypes`, etc. in
  ``core/htypes.py`` to sets - that is factor 2-3 faster in
  comparisons using 'in' ....
- Revisit hArrays sharing the same vector. If one has a different
  shape the other one is also affected and lengths and slices become
  inconsistent!!
- Make :func:`hArray_set` accept a list of an array as last dimension.
- :mod:`averagespectrum` need to take ``nquistzone`` into account (use
  :func:`nyquistflip`).
- Use ``shutils rmtree`` for :func:`hArray_write` to remove old file
  directory.
- :func:`hCoordinateConvert` can only do AZELR -> XYZ and not vice versa. Use
  pytmf to do that (and check other conversions as well), also,
  autmatically loop over all 3-tuples in vector.

