/***************************************************************************
 *
 *   Copyright (C) 2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "dsp/SigProcDigitizer.h"

//! Default constructor
dsp::SigProcDigitizer::SigProcDigitizer () : Digitizer ("SigProcDigitizer")
{
  nbit = 8;
}

//! Set the number of bits per sample
void dsp::SigProcDigitizer::set_nbit (int _nbit)
{
  switch(_nbit)
  {
  case 1:
  case 2:
  case 4:
  case 8:
  case -32:
    nbit=_nbit;
    break;
  default:
    throw Error (InvalidState, "dsp::SigProcDigitizer::set_nbit",
		 "nbit=%i not understood", _nbit);
    break;
  }
}

/*! 
  This method must tranpose the data from frequency major order to
  time major order.  It is assumed that ndat > 4 * nchan, and therefore
  stride in output time is smaller than stride in input frequency.

  If this condition isn't true, then the nesting of the loops should
  be inverted.
*/
void dsp::SigProcDigitizer::pack ()
{
  if (input->get_npol() != 1)
    throw Error (InvalidState, "dsp::SigProcDigitizer::pack",
		 "cannot handle npol=%d", input->get_npol());

  flip_band = input->get_bandwidth() > 0;
  if (flip_band)
    output->set_bandwidth(-input->get_bandwidth());

  if (nbit == -32)
    pack_float ();

  // the number of frequency channels
  const unsigned nchan = input->get_nchan();

  // the number of time samples
  const uint64_t ndat = input->get_ndat();

  unsigned char* outptr = output->get_rawptr();

  float digi_mean=0;
  float digi_sigma=6;
  float digi_scale=0;
  int digi_max=0;
  int digi_min=0;
  int bit_counter=0;
  int samp_per_byte = 8/nbit;

  switch (nbit){
  case 1:
    digi_mean=0.5;
    digi_scale=1;
    digi_min = 0;
    digi_max = 1;
    break;
  case 2:
    digi_mean=1.5;
    digi_scale=1;
    digi_min = 0;
    digi_max = 3;
    break;
  case 4:
    digi_mean=7.5;
    digi_scale= digi_mean / digi_sigma;
    digi_min = 0;
    digi_max = 15;
    break;
  case 8:
    digi_mean=127.5;
    digi_scale= digi_mean / digi_sigma;
    digi_min = 0;
    digi_max = 255;
    break;
  }


  /*
    TFP mode
  */

  switch (input->get_order()){

  case TimeSeries::OrderTFP:
    {
      const float* inptr = input->get_dattfp();
      outptr--; // This is important, as the program increments the pointer at the start of each byte. MJK2008.

      for(uint64_t idat=0; idat < ndat; idat++){

	for(unsigned ichan=0; ichan < nchan; ichan++){
	  unsigned inChan = ichan;
	  if (flip_band)
	    inChan = (nchan-ichan-1);

	  //printf("%f\t",(*inptr));
	  int result = (int)(((*(inptr+inChan)) * digi_scale) + digi_mean +0.5 );
	  //printf("%d\n",result);

	  // clip the result at the limits
	  //if (result < digi_min)
	  //	result = digi_min;
	  result = std::max(result,digi_min);

	  //if (result > digi_max)
	  //	result = digi_max;
	  result = std::min(result,digi_max);

	  switch (nbit){
	  case 1:
	  case 2:
	  case 4:
	    bit_counter = ichan % (samp_per_byte);

	    //	if(bit_counter==0)outptr[idat*(int)(nchan/samp_per_byte)
	    //		+ (int)(ichan/samp_per_byte)]=(unsigned char)0;
	    //	outptr[idat*(int)(nchan/samp_per_byte)
	    //		+ (int)(ichan/samp_per_byte)] += ((unsigned char) (result)) << (bit_counter*nbit);

	    if(bit_counter==0){
	      outptr++;
	      (*outptr) = (unsigned char)0;
	    }
	    //fprintf(stderr,"%d %d\n",outptr - output->get_rawptr(), idat*(int)(nchan/samp_per_byte) + (int)(ichan/samp_per_byte));
	    (*outptr) |= ((unsigned char) (result)) << (bit_counter*nbit);

	    break;
	  case 8:
	    outptr++;
	    (*outptr) = (unsigned char) result;
	    break;
	  }


	}
	inptr+=nchan;
      }

      return;
    }
  case TimeSeries::OrderFPT:
    {
      for (unsigned ichan=0; ichan < nchan; ichan++)
	{
	  const float* inptr;
	  if (flip_band)
	    inptr = input->get_datptr (nchan-ichan-1);
	  else
	    inptr = input->get_datptr (ichan);

	  for (uint64_t idat=0; idat < ndat; idat++)
	    {
	      int result = int( (inptr[idat] * digi_scale) + digi_mean +0.5 );

	      // clip the result at the limits
	      if (result < digi_min)
		result = digi_min;

	      if (result > digi_max)
		result = digi_max;



	      switch (nbit){
	      case 1:
	      case 2:
	      case 4:
		bit_counter = ichan % (samp_per_byte);


		if(bit_counter==0)outptr[idat*(int)(nchan/samp_per_byte)
					 + (int)(ichan/samp_per_byte)]=(unsigned char)0;
		outptr[idat*(int)(nchan/samp_per_byte)
		       + (int)(ichan/samp_per_byte)] += ((unsigned char) (result)) << (bit_counter*nbit);

		break;
	      case 8:
		outptr[idat*nchan + ichan] = (unsigned char) result;
		break;
	      }
	    }
	}
      return;
    }
  default:
    throw Error (InvalidState, "dsp::SigProcDigitizer::operate",
		 "Can only operate on data ordered FTP or PFT.");

  }
}




void dsp::SigProcDigitizer::pack_float () try
{
  // the number of frequency channels
  const unsigned nchan = input->get_nchan();

  // the number of time samples
  const uint64_t ndat = input->get_ndat();

  float* outptr = reinterpret_cast<float*>( output->get_rawptr() );

  switch (input->get_order())
  {
  case TimeSeries::OrderTFP:
  {
    const float* inptr = input->get_dattfp();

    for (uint64_t idat=0; idat < ndat; idat++)
    {
      if (flip_band)
      {
	unsigned in_chan = nchan-1;
	for (unsigned ichan=0; ichan < nchan; ichan++)
	{
	  outptr[ichan] = inptr[in_chan];
	  in_chan --;
	}
      }
      else
      {
	unsigned in_chan = 0;
	for (unsigned ichan=0; ichan < nchan; ichan++)
	{
	  outptr[ichan] = inptr[in_chan];
	  in_chan ++;
	}
      }

      inptr += nchan;
    }
    return;
  }
  case TimeSeries::OrderFPT:
  {
    for (unsigned ichan=0; ichan < nchan; ichan++)
    {
      const float* inptr;
      if (flip_band)
	inptr = input->get_datptr (nchan-ichan-1);
      else
	inptr = input->get_datptr (ichan);

      for (uint64_t idat=0; idat < ndat; idat++)
	outptr[idat*nchan + ichan] = inptr[idat];
    }
  }

  default:
    throw Error (InvalidState, "dsp::SigProcDigitizer::pack_float",
		 "Can only operate on data ordered FTP or PFT.");
  }
}
catch (Error& error)
{
  throw error += "dsp::SigProcDigitizer::pack_float";
}

