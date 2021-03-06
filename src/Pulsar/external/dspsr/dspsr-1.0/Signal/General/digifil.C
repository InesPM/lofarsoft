/***************************************************************************
 *
 *   Copyright (C) 2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/*
  digifil converts any file format recognized by dspsr into sigproc
  filterbank (.fil) format.
 */

#include "dsp/SigProcObservation.h"
#include "dsp/SigProcDigitizer.h"

#include "dsp/IOManager.h"
#include "dsp/Input.h"
#include "dsp/Unpacker.h"

#include "dsp/Rescale.h"
#include "dsp/PScrunch.h"
#include "dsp/FScrunch.h"
#include "dsp/TScrunch.h"

#include "dsp/DedispersionSampleDelay.h"
#include "dsp/SampleDelay.h"

#include "dsp/Filterbank.h"
#include "dsp/Detection.h"

#include "dirutil.h"
#include "Error.h"

#include <iostream>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

using namespace std;

static char* args = "b:B:D:F:f:cI:Ko:prt:hvVZ:";

void usage ()
{
  cout << "digifil - convert dspsr input to sigproc filterbank \n"
    "Usage: digifil file1 [file2 ...] \n"
    "Options:\n"
    "\n"
    "  -b bits   number of bits per sample output to file \n" 
    "  -B MB     block size in megabytes \n"
    "  -c        keep offset and scale constant \n"
    "  -I secs   number of seconds between level updates \n"
    "  -F nchan  create a filterbank (voltages only) \n"
    "  -K        remove inter-channel dispersion delays \n"
    "  -D dm     set the dispersion measure \n"
    "  -t nsamp  decimate in time \n"
    "  -f nchan  decimate in frequency \n"
    "  -I secs   rescale interval in seconds \n"
    "  -o file   output filename \n" 
    "  -r        report total Operation times \n"
    "  -p        revert to FPT order \n"
       << endl;
}

int main (int argc, char** argv) try 
{
  bool verbose = false;
  bool constant_offset_scale = false;

  int nbits = 2;
  unsigned filterbank_nchan = 0;
  unsigned tscrunch_factor = 0;
  unsigned fscrunch_factor = 0;

  // block size in MB
  double block_size = 2.0;
  double rescale_seconds = 10.0;

  char* output_filename = 0;

  //! Removes inter-channel dispersion delays
  Reference::To<dsp::SampleDelay> sample_delay;
  double dm = 0.0;

  string datestr_pattern = "%Y-%m-%d-%H:%M:%S";

  dsp::TimeSeries::Order order = dsp::TimeSeries::OrderTFP;

  int c;
  while ((c = getopt(argc, argv, args)) != -1)
    switch (c) {

    case 'b':
      nbits = atoi (optarg);
      break;

    case 'B':
      block_size = atof (optarg);
      break;

    case 'c':
      constant_offset_scale = true;
      break;

    case 'D':
      dm = atof (optarg);
      break;

    case 'F':
      filterbank_nchan = atoi (optarg);
      break;

    case 'K':
      if (!sample_delay)
	sample_delay = new dsp::SampleDelay;
      break;

    case 'o':
      output_filename = optarg;
      break;

    case 'p':
      order = dsp::TimeSeries::OrderFPT;
      break;

    case 'r':
      dsp::Operation::record_time = true;
      dsp::Operation::report_time = true;
      break;

    case 't':
      tscrunch_factor = atoi (optarg);
      break;

    case 'f':
      fscrunch_factor = atoi (optarg);
      break;

    case 'I':
      rescale_seconds = atof (optarg);
      break;

    case 'h':
      usage ();
      return 0;
    case 'V':
      dsp::Response::verbose = true;
      dsp::Operation::verbose = true;
      dsp::Observation::verbose = true;
    case 'v':
      verbose = true;
      break;

    case 'Z':
    {
      string lib = optarg;

      if (lib == "help")
      {
        unsigned nlib = FTransform::get_num_libraries ();
        cerr << "dspsr: " << nlib << " available FFT libraries:";
        for (unsigned ilib=0; ilib < nlib; ilib++)
          cerr << " " << FTransform::get_library_name (ilib);
  
        cerr << "\ndspsr: default FFT library " 
             << FTransform::get_library() << endl;

        exit (0);
      }
      else if (lib == "simd")
        FTransform::simd = true;
      else {
        FTransform::set_library (lib);
        cerr << "dspsr: FFT library set to " << lib << endl;
      }

      break;
    }
    default:
      cerr << "invalid param '" << c << "'" << endl;
    }

  vector <string> filenames;
  
  for (int ai=optind; ai<argc; ai++)
    dirglob (&filenames, argv[ai]);

  if (filenames.size() == 0)
  {
    cerr << "digifil: please specify a filename (-h for help)" 
	 << endl;
    return 0;
  }

  FILE* outfile = 0;

  if (output_filename)
  {
    outfile = fopen (output_filename, "w");
    if (!outfile)
    {
      fprintf (stderr, "digifil: could not open %s - %s\n",
	       output_filename, strerror (errno));
      return -1;
    }
  }

  /*
    The following lines "wire up" the signal path, using containers
    to communicate the data between operations.
  */

  if (verbose)
    cerr << "digifil: creating input timeseries container" << endl;
  Reference::To<dsp::TimeSeries> timeseries = new dsp::TimeSeries;

  if (verbose)
    cerr << "digifil: creating input/unpacker manager" << endl;
  Reference::To<dsp::IOManager> manager = new dsp::IOManager;
  manager->set_output (timeseries);

  Reference::To<dsp::Rescale> rescale;
  if (rescale_seconds)
  {
    if (verbose)
      cerr << "digifil: creating rescale transformation" << endl;
    rescale = new dsp::Rescale;
    rescale->set_input (timeseries);
    rescale->set_output (timeseries);
    rescale->set_constant (constant_offset_scale);
    rescale->set_interval_seconds (rescale_seconds);
  }

  if (verbose)
    cerr << "digifil: creating pscrunch transformation" << endl;
  Reference::To<dsp::PScrunch> pscrunch = new dsp::PScrunch;
  pscrunch->set_input (timeseries);
  pscrunch->set_output (timeseries);

  if (verbose)
    cerr << "digifil: creating output bitseries container" << endl;
  Reference::To<dsp::BitSeries> bitseries = new dsp::BitSeries;

  if (verbose)
    cerr << "digifil: creating sigproc digitizer" << endl;
  Reference::To<dsp::SigProcDigitizer> digitizer = new dsp::SigProcDigitizer;
  digitizer->set_nbit (nbits);
  digitizer->set_input (timeseries);
  digitizer->set_output (bitseries);

  bool written_header = false;

  for (unsigned ifile=0; ifile < filenames.size(); ifile++) try
  {
    Reference::To<dsp::Filterbank> filterbank;
    Reference::To<dsp::TimeSeries> filterbank_input;
    Reference::To<dsp::Detection> detection;
    Reference::To<dsp::TScrunch> tscrunch;
    Reference::To<dsp::FScrunch> fscrunch;

    if (verbose)
      cerr << "digifil: opening file " << filenames[ifile] << endl;

    manager->open (filenames[ifile]);

    dsp::Observation* obs = manager->get_info();
    const unsigned nchan = obs->get_nchan ();
    const unsigned npol = obs->get_npol ();
    const unsigned ndim = obs->get_ndim ();

    obs->set_dispersion_measure( dm );

    // the unpacked input will occupy nbytes_per_sample
    double nbytes_per_sample = sizeof(float) * nchan * npol * ndim;

    double MB = 1024.0 * 1024.0;
    uint64_t nsample = uint64_t( block_size*MB / nbytes_per_sample );

    if (verbose)
      cerr << "digifil: block_size=" << block_size << " MB "
        "(" << nsample << " samp)" << endl;

    bool do_pscrunch = obs->get_npol() > 1;

    if (!obs->get_detected())
    {

      if (filterbank_nchan)
      {
	if (verbose)
	  cerr << "digifil: creating " << filterbank_nchan 
	       << " channel filterbank" << endl;

	filterbank = new dsp::Filterbank;
	filterbank_input = new dsp::TimeSeries;

	manager->set_output (filterbank_input);

	filterbank->set_nchan( filterbank_nchan );
	filterbank->set_input( filterbank_input );
	filterbank->set_output( timeseries );

        // filterbank will do detection
        filterbank->set_output_order( dsp::TimeSeries::OrderTFP );
      }
      else
      {
	if (verbose)
	  cerr << "digifil: detecting data directly" << endl;

        detection = new dsp::Detection;
        detection->set_input( timeseries );
        detection->set_output( timeseries );

        // detection will do pscrunch
        do_pscrunch = false;
      }
    }

    if (sample_delay)
    {
      if (verbose)
	cerr << "digifil: removing dispserion delays" << endl;

      sample_delay->set_input (timeseries);
      sample_delay->set_output (timeseries);
      sample_delay->set_function (new dsp::Dedispersion::SampleDelay);
    }

    if (fscrunch_factor)
    {
      fscrunch = new dsp::FScrunch;

      fscrunch->set_factor (fscrunch_factor);
      fscrunch->set_input( timeseries );
      fscrunch->set_output( timeseries );
    }

    if (tscrunch_factor)
    {
      tscrunch = new dsp::TScrunch;

      tscrunch->set_factor (tscrunch_factor);
      tscrunch->set_input( timeseries );
      tscrunch->set_output( timeseries );
    }

    manager->set_block_size( nsample );

    dsp::Unpacker* unpacker = manager->get_unpacker();

    if (!sample_delay && unpacker->get_order_supported (order))
      unpacker->set_output_order (order);

    if (verbose)
    {
      dsp::Observation* obs = manager->get_info();

      cerr << "digifil: file " 
	   << filenames[ifile] << " opened" << endl;
      cerr << "Source = " << obs->get_source() << endl;
      cerr << "Frequency = " << obs->get_centre_frequency() << endl;
      cerr << "Bandwidth = " << obs->get_bandwidth() << endl;
      cerr << "Sampling rate = " << obs->get_rate() << endl;
    }

    dsp::SigProcObservation sigproc;

    bool start = true;

    while (!manager->get_input()->eod())
    {
      manager->operate ();

      if (filterbank)
      {
	if (verbose)
	  cerr << "digifil: filterbank" << endl;

	filterbank->operate();
      }

      if (detection)
      {
	if (verbose)
	  cerr << "digifil: detection" << endl;

	detection->operate();
      }

      if (sample_delay)
      {
	if (start)
	{
	  sample_delay->prepare ();
	  if (sample_delay->get_minimum_samples () > timeseries->get_ndat())
	    cerr << "digifil: WARNING block size nsample=" 
		 << nsample << " < dispersion delay nsample="
		 << sample_delay->get_minimum_samples () << endl;
	}

	if (verbose)
	  cerr << "digifil: dedisperse" << endl;

        sample_delay->operate();
      }

      if (fscrunch)
      {
	if (verbose)
	  cerr << "digifil: fscrunch" << endl;

        fscrunch->operate();
      }

      if (tscrunch)
      {
	if (verbose)
	  cerr << "digifil: tscrunch" << endl;

        tscrunch->operate();
      }

      if (rescale)
      {
	if (verbose)
	  cerr << "digifil: rescale" << endl;
      
	rescale->operate ();
      }

      if (do_pscrunch)
      {
	if (verbose)
	  cerr << "digifil: pscrunch" << endl;
	  
	pscrunch->operate ();
      }

      digitizer->operate ();

      if (!outfile)
      {
	MJD epoch = obs->get_start_time();

	vector<char> buffer (FILENAME_MAX);
	char* filename = &buffer[0];

	if (!epoch.datestr( filename, FILENAME_MAX, datestr_pattern.c_str() ))
	{
	  cerr << "digifil: error MJD::datestr("+datestr_pattern+")" << endl;
	  return -1;
	}

	string fname = filename + string(".fil");
	cerr << "digifil: output in " << fname << endl;

	outfile = fopen (fname.c_str(), "w");
	if (!outfile)
	{
	  cerr << "digifil: could not open "+fname+" - " << strerror (errno)
	       << endl;;
	  return -1;
	}
      }

      if (!written_header)
      {
	if (verbose)
	  cerr << "digifil: unload header" << endl;

	sigproc.copy( bitseries );
	sigproc.unload( outfile );
	written_header = true;
      }

      // output the result to stdout
      const uint64_t nbyte = bitseries->get_nbytes();
      unsigned char* data = bitseries->get_rawptr();

      if (verbose)
	cerr << "digifil: writing " << nbyte << " bytes to file" << endl;

      fwrite (data,nbyte,1,outfile);

      start = false;
    }

    if (verbose)
      cerr << "end of data file " << filenames[ifile] << endl;
  }
  catch (Error& error)
  {
    cerr << error << endl;
  }

  return 0;
}

catch (Error& error)
{
  cerr << error << endl;
  return -1;
}




