/***************************************************************************
 *
 *   Copyright (C) 2007-2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "dsp/LoadToFoldN.h"
#include "dsp/LoadToFoldConfig.h"

#include "dsp/Input.h"
#include "dsp/InputBufferingShare.h"

#include "dsp/Dedispersion.h"
#include "dsp/Fold.h"
#include "dsp/SubFold.h"
#include "dsp/UnloaderShare.h"
#include "FTransformAgent.h"
#include "ThreadContext.h"

#include <fstream>
#include <stdlib.h>
#include <errno.h>

using namespace std;

//! Constructor
dsp::LoadToFoldN::LoadToFoldN (unsigned nthread)
{
  input_context = new ThreadContext;
  state_changes = new ThreadContext;

  if (nthread)
    set_nthread (nthread);

  if (!FTransform::Agent::context)
    FTransform::Agent::context = new ThreadContext;
}
    
//! Destructor
dsp::LoadToFoldN::~LoadToFoldN ()
{
  delete input_context;
  delete state_changes;
}

//! Set the number of thread to be used
void dsp::LoadToFoldN::set_nthread (unsigned nthread)
{
  threads.resize (nthread);
  for (unsigned i=0; i<nthread; i++)
    if (!threads[i])
      threads[i] = new_thread();

  if (configuration)
    set_configuration (configuration);

  if (input)
    set_input (input);
}

//! Set the configuration to be used in prepare and run
void dsp::LoadToFoldN::set_configuration (Config* config)
{
  configuration = config;
  configuration->nthread = threads.size();

  for (unsigned i=0; i<threads.size(); i++)
    threads[i]->set_configuration( config );
}

//! Set the Input from which data will be read
void dsp::LoadToFoldN::set_input (Input* _input)
{
  input = _input;

  if (!input)
    return;

  input->set_context( input_context );

  for (unsigned i=0; i<threads.size(); i++)
  {
    threads[i]->set_input( input );
    threads[i]->input_context = input_context;
  }
}

dsp::Input* dsp::LoadToFoldN::get_input ()
{
  return input;
}

void dsp::LoadToFoldN::prepare (LoadToFold1* fold)
{
  dsp::LoadToFoldN::signal (fold, LoadToFold1::Prepare);
  dsp::LoadToFoldN::wait (fold, LoadToFold1::Prepared);
}

//! Prepare to fold the input TimeSeries
void dsp::LoadToFoldN::prepare ()
{
  launch_threads ();

  prepare( threads[0] );

  if (threads[0]->kernel && !threads[0]->kernel->context)
    threads[0]->kernel->context = new ThreadContext;

  //
  // install InputBuffering::Share policy
  //
  typedef Transformation<TimeSeries,TimeSeries> Xform;

  for (unsigned iop=0; iop < threads[0]->operations.size(); iop++)
  {
    Xform* xform = dynamic_kast<Xform>( threads[0]->operations[iop] );

    if (!xform)
      continue;

    if (!xform->has_buffering_policy())
      continue;

    InputBuffering* ibuf;
    ibuf = dynamic_cast<InputBuffering*>( xform->get_buffering_policy() );

    if (!ibuf)
      continue;

    xform->set_buffering_policy( new InputBuffering::Share (ibuf, xform) );
  }

  bool subints = configuration->single_pulse 
    || configuration->integration_length;

  if (subints)
    prepare_subint_archival ();

  for (unsigned i=1; i<threads.size(); i++)
  {

    //
    // clone the Fold/SubFold operations
    //
    // This code satisfies two preconditions:
    // 1) the folding operation may be either a Fold or SubFold class
    // 2) the folding operations should not share outputs or predictors
    //

    unsigned nfold = threads[0]->fold.size();
    threads[i]->fold.resize( nfold );
    for (unsigned ifold = 0; ifold < nfold; ifold ++)
    {
      // the clone automatically copies the pointers to predictors ...
      threads[i]->fold[ifold] = threads[0]->fold[ifold]->clone();

      // ... but each thread should have its own
      if (threads[i]->fold[ifold]->has_folding_predictor())
        threads[i]->fold[ifold]->set_folding_predictor
           (threads[0]->fold[ifold]->get_folding_predictor()->clone());

      // ... and its own output.  New ones will be created in prepare()
      threads[i]->fold[ifold]->set_output( 0 );
    }

    //
    // share the dedispersion kernel
    //
    threads[i]->kernel = threads[0]->kernel;

    //
    // only the first thread prints updates
    //
    threads[i]->thread_id = i;

    //
    // only the first thread must manage archival
    //
    if (subints)
      threads[i]->manage_archiver = false;

    if (Operation::verbose)
      cerr << "dsp::LoadToFoldN::prepare preparing thread " << i << endl;

    threads[i]->share = threads[0];

    prepare( threads[i] );
  }
}

void dsp::LoadToFoldN::prepare_subint_archival ()
{
  unsigned nfold = threads[0]->fold.size();

  if (Operation::verbose)
    cerr << "dsp::LoadToFoldN::prepare_subint_archival nfold=" << nfold
	 << endl;

  if( threads[0]->unloader.size() != nfold )
    throw Error( InvalidState, "dsp::LoadToFoldN::prepare_subint_archival",
		 "unloader vector size=%u != fold vector size=%u",
		 threads[0]->unloader.size(), nfold );

  unloader.resize( nfold );

  for (unsigned i=1; i<threads.size(); i++)
    threads[i]->unloader.resize( nfold );

  /*
    Note that, at this point, only thread[0] has been prepared.
    Therefore, only thread[0] will have an initialized fold array
  */

  for (unsigned ifold = 0; ifold < nfold; ifold ++)
  {
    SubFold* subfold = dynamic_cast<SubFold*>( threads[0]->fold[ifold].get() );
    if (!subfold)
      throw Error( InvalidState, "dsp::LoadToFoldN::prepare_subint_archival",
		   "folder is not a SubFold" );

    unloader[ifold] = new UnloaderShare( threads.size() );
    unloader[ifold]->copy( subfold->get_divider() );
    unloader[ifold]->set_context( new ThreadContext );

    PhaseSeriesUnloader* primary_unloader = threads[0]->unloader[ifold];

    if (configuration->single_pulse_archives())
      unloader[ifold]->set_wait_all (false);
    else
      unloader[ifold]->set_unloader( primary_unloader );

    for (unsigned i=0; i<threads.size(); i++) 
    {
      UnloaderShare::Submit* submit = unloader[ifold]->new_Submit (i);

      if (configuration->single_pulse_archives())
        submit->set_unloader( primary_unloader->clone() );

      threads[i]->unloader[ifold] = submit;
    }

    subfold->set_unloader( threads[0]->unloader[ifold] );
  }

  if (Operation::verbose)
    cerr << "dsp::LoadToFoldN::prepare_subint_archival done" << endl;
}

uint64_t dsp::LoadToFoldN::get_minimum_samples () const
{
  if (threads.size() == 0)
    return 0;
  return threads[0]->get_minimum_samples();
}

//
// share buffering policies
//
void dsp::LoadToFoldN::share (LoadToFold1* fold, LoadToFold1* share)
{
  typedef Transformation<TimeSeries,TimeSeries> Xform;

  for (unsigned iop=0; iop < fold->operations.size(); iop++)
  {
    Xform* trans0 = dynamic_kast<Xform>( share->operations[iop] );

    if (!trans0)
      continue;

    if (!trans0->has_buffering_policy())
      continue;

    InputBuffering::Share* ibuf0;
    ibuf0 = dynamic_cast<InputBuffering::Share*>
      ( trans0->get_buffering_policy() );

    if (!ibuf0)
      continue;

    Xform* trans = dynamic_kast<Xform>( fold->operations[iop] );
    
    if (!trans)
      throw Error (InvalidState, "dsp::LoadToFoldN::share",
		   "mismatched operation type");

    if (!trans->has_buffering_policy())
      throw Error (InvalidState, "dsp::LoadToFoldN::share",
		   "mismatched buffering policy");
    
    // cerr << "Sharing buffering policy of " << trans->get_name() << endl;

    trans->set_buffering_policy( ibuf0->clone(trans) );
  }
}

void dsp::LoadToFoldN::wait (LoadToFold1* fold, LoadToFold1::State state)
{
  ThreadContext::Lock lock (fold->state_change);
  while ( fold->state != state )
    fold->state_change->wait ();
}

void dsp::LoadToFoldN::signal (LoadToFold1* fold, LoadToFold1::State state)
{
  ThreadContext::Lock lock (fold->state_change);
  fold->state = state;
  fold->state_change->broadcast();
}

void* dsp::LoadToFoldN::thread (void* context) try
{
  dsp::LoadToFold1* fold = reinterpret_cast<dsp::LoadToFold1*>( context );

  wait (fold, LoadToFold1::Prepare);

  fold->prepare ();

  if (fold->share)
    share (fold, fold->share);

  signal (fold, LoadToFold1::Prepared);

  wait (fold, LoadToFold1::Run);

  LoadToFold1::State state = LoadToFold1::Done;

  try
  {
    if (fold->log) *(fold->log) << "THREAD STARTED" << endl;

    fold->run();

    if (fold->log) *(fold->log) << "THREAD run ENDED" << endl;
  }
  catch (Error& error)
  {
    if (fold->log) *(fold->log) << "THREAD ERROR: " << error << endl;

    cerr << "THREAD ERROR: " << error << endl;

    state = LoadToFold1::Fail;
    fold->error = error;

    exit (-1);
  }

  if (fold->log) *(fold->log) << "SIGNAL end state" << endl;

  signal (fold, state);

  if (fold->log) *(fold->log) << "THREAD EXIT" << endl;

  pthread_exit (0);
}
catch (Error& error)
{
  cerr << "THREAD ERROR: " << error << endl;
  exit (-1);
}

//! Run through the data
void dsp::LoadToFoldN::run ()
{
  ThreadContext::Lock lock (state_changes);

  for (unsigned i=0; i<threads.size(); i++)
    threads[i]->state = LoadToFold1::Run;

  state_changes->broadcast();
}

void dsp::LoadToFoldN::launch_threads ()
{
  ids.resize( threads.size() );

  for (unsigned i=0; i<threads.size(); i++)
  {
    LoadToFold1* fold = threads[i];

    if (Operation::verbose)
    {
      string logname = "dspsr.log." + tostring (i);

      cerr << "dsp::LoadToFoldN::run spawning thread " << i 
	   << " ptr=" << fold << " log=" << logname << endl;

      fold->take_ostream( new std::ofstream (logname.c_str()) );
    }

    fold->state = LoadToFold1::Idle;
    fold->state_change = state_changes;

    errno = pthread_create (&ids[i], 0, thread, fold);

    if (errno != 0)
      throw Error (FailedSys, "psr::LoadToFoldN::run", "pthread_create");

  }

  if (Operation::verbose)
    cerr << "psr::LoadToFoldN::run all threads spawned" << endl;

}

//! Finish everything
void dsp::LoadToFoldN::finish ()
{
  Error error (InvalidState, "");

  unsigned errors = 0;
  unsigned finished = 0;

  LoadToFold1* first = 0;

  ThreadContext::Lock lock (state_changes);

  while (finished < threads.size())
  {
    for (unsigned i=0; i<threads.size(); i++)
    {
      LoadToFold1::State state = threads[i]->state;

      if (state != LoadToFold1::Done && state != LoadToFold1::Fail)
      {
        if (Operation::verbose)
        {
          cerr << "dsp::LoadToFoldN::finish thread " << i;
          if (state == LoadToFold1::Run)
            cerr << " pending" << endl;
          else if (state == LoadToFold1::Joined)
            cerr << " processed" << endl;
	  else
	    cerr << " unknown state" << endl;
        }
        continue;
      }

      try {

	if (Operation::verbose)
	  cerr << "psr::LoadToFoldN::finish joining thread " << i << endl;

	void* result = 0;
	pthread_join (ids[i], &result);

        if (Operation::verbose)
          cerr << "psr::LoadToFoldN::finish thread " << i << " joined" << endl;

	finished ++;
        threads[i]->state = LoadToFold1::Joined;

	if (state == LoadToFold1::Fail)
        {
	  errors ++;
	  error = threads[i]->error;
          cerr << "thread " << i << " aborted with error\n\t"
               << error.get_message() << endl;
          continue;
	}

	if (!first)
	{
          if (Operation::verbose)
            cerr << "psr::LoadToFoldN::finish initializing first" << endl;

	  first = threads[i];
	}
	else
        {
          if (Operation::verbose)
            cerr << "psr::LoadToFoldN::finish combining with first" << endl;

	  first->combine( threads[i] );
        }

      }
      catch (Error& error)
      {
        cerr << "psr::LoadToFoldN::finish failure on thread " 
             << i << error << endl;
      }

    }

    if (finished < threads.size())
    {
      if (Operation::verbose)
        cerr << "psr::LoadToFoldN::finish wait on condition" << endl;

      state_changes->wait();

      if (Operation::verbose)
        cerr << "psr::LoadToFoldN::finish condition wait returned" << endl;
    }

  }

  if (first)
  {
    if (Operation::verbose)
      cerr << "psr::LoadToFoldN::finish via first" << endl;

    first->finish();
  }

  for (unsigned i=0; i<unloader.size(); i++)
  {
    if (Operation::verbose)
      cerr << "psr::LoadToFoldN::finish unloader[" << i << "]" << endl;

    unloader[i]->finish();
  }

  if (errors)
  {
    error << errors << " threads aborted with an error";
    throw error += "dsp::LoadToFoldN::finish";
  }

  // add folded profiles together XXX
}

//! The creator of new LoadToFold1 threadss
dsp::LoadToFold1* dsp::LoadToFoldN::new_thread ()
{
  return new LoadToFold1;
}

