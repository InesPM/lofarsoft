/***************************************************************************
 *
 *   Copyright (C) 2002 by Aidan Hotan
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
using namespace std;
#include "Pulsar/Profile.h"
#include "Error.h"
#include "FTransform.h"

#include <memory>

void Pulsar::Profile::convolve (const Profile* profile)
{
  convolve (profile, -1);
}

void Pulsar::Profile::correlate (const Profile* profile)
{
  convolve (profile, 1);
}

void Pulsar::Profile::correlate_normalized (const Profile* profile)
{
  Reference::To<Pulsar::Profile> temp1, temp2;
  temp1 = this->clone();
  temp2 = profile->clone();
  //remove the baseline and subtract the mean
  double mean = -1.0, variance2=-1.0, variance1 =-1.0, varmean = -1.0;
  temp1->stats (&mean, &variance1, &varmean,0,(int)temp1->get_nbin());
  *temp1 -= mean;

  temp2->stats (&mean, &variance2, &varmean,0,(int)temp2->get_nbin());
  *temp2 -= mean;

  temp1->correlate( temp2);
  *temp1 *= 1.0 / sqrt(variance2) / sqrt(variance1) / (float)temp1->get_nbin();

  this->set_amps(temp1->get_amps());
}

void Pulsar::Profile::convolve (const Profile* profile, int dir)
{
  unsigned nbin = get_nbin();
  unsigned mbin = profile->get_nbin();

  vector<double> temp (nbin, 0.0);

  const float* nptr = get_amps();
  const float* mptr = profile->get_amps();

  for (unsigned ibin=0; ibin < nbin; ibin++) {
    for (unsigned jbin=0; jbin < mbin; jbin++) {
      unsigned n_bin = (ibin+jbin)%nbin;
      unsigned m_bin = (mbin+dir*jbin)%mbin;
      temp[ibin] += nptr[n_bin] * mptr[m_bin];
    }
  }

  set_amps (temp);
}


void Pulsar::Profile::fft_convolve (const Profile* p1)
{
  unsigned bins = get_nbin();

  if (bins != p1->get_nbin())
    throw Error (InvalidParam, "Profile::fft_convolve", 
		 "profile nbin values not equal");

  vector<float> temp1( bins+2 );
  vector<float> temp2( bins+2 );
  vector<float> resultant( bins+2 );

  FTransform::frc1d (bins, &temp1[0], get_amps());
  FTransform::frc1d (bins, &temp2[0], p1->get_amps());

  // cast the float* arrays to complex<float>*
  complex<float>* c1 = (complex<float>*) &temp1[0];
  complex<float>* c2 = (complex<float>*) &temp2[0];
  complex<float>* r = (complex<float>*) &resultant[0];

  unsigned ncomplex = bins/2+1;

  // Complex multiplication of the elements
  for (unsigned i = 0; i < ncomplex; i++)
    r[i] = c1[i]*c2[i];

  // Transform back to the time domain to get the convolved solution

  vector<float> solution (bins);

  FTransform::bcr1d (bins, &(solution[0]), &resultant[0]);

  set_amps(solution);
}

