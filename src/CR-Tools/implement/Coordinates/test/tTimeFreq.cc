/*-------------------------------------------------------------------------*
 | $Id::                                                                 $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) 2007                                                    *
 *   Lars B"ahren (lbaehren@gmail.com)                                     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include <Coordinates/CoordinateType.h>
#include <Coordinates/TimeFreq.h>

/*!
  \file tTimeFreq.cc

  \ingroup CR_Coordinates

  \brief A collection of test routines for TimeFreq
 
  \author Lars B&auml;hren
 
  \date 2007/03/06
*/

using std::cout;
using std::endl;
using CR::TimeFreq;

// -----------------------------------------------------------------------------

/*!
  \brief Show the frequency values
*/
void show_frequencies (vector<double> const &freq,
		       uint const &nyquistZone,
		       double const &incr)
{
  uint nofChannels (freq.size());

  // how many frequency channels do we have?
  cout << endl;
  cout << " Nyquist zone    = " << nyquistZone << endl;
  cout << " nof channels    = " << nofChannels << endl;
  cout << " freq. increment = " << incr             << endl;
  cout << " freq[1]-freq[0] = " << freq[1]-freq[0]  << endl;

  // print a subset of the frequency values
  cout << endl;
  cout << " [ "
	    << freq[0] << " "
	    << freq[1] << " "
	    << freq[2] << " "
	    << freq[3] << " "
	    << freq[4] << " "
	    << freq[5] << "\n   ... \n   "
	    << freq[nofChannels-3] << " "
	    << freq[nofChannels-2] << " "
	    << freq[nofChannels-1] << " ]"
	    << std:: endl;
  cout << endl;
  
}

// -----------------------------------------------------------------------------

/*!
  \brief Test constructors for a new TimeFreq object

  \return nofFailedTests -- The number of failed tests.
*/
int test_TimeFreq ()
{
  cout << "\n[test_TimeFreq]\n" << endl;

  int nofFailedTests (0);
  uint blocksize (2048);
  double sampleFrequency (80e06);
  uint nyquistZone (2);
  
  cout << "[1] Testing default constructor ..." << endl;
  try {
    TimeFreq data;
    
    data.summary();
    
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }
  
  cout << "[2] Testing argumented constructors ..." << endl;
  try {
    TimeFreq data1 (blocksize);
    data1.summary();
    //
    TimeFreq data2 (blocksize,sampleFrequency,nyquistZone);
    data2.summary();
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }
  
  cout << "[3] Testing copy constructor ..." << endl;
  try {
    TimeFreq data1 (blocksize,sampleFrequency,nyquistZone);
    TimeFreq data2 (data1);
    
    cout << "-- Properties of the original object" << endl;
    data1.summary();
    cout << "-- Properties of the copied object" << endl;
    data2.summary();
    
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }
  
  return nofFailedTests;
}

// -----------------------------------------------------------------------------

/*!
  \brief Test access to and manipulation of the sample frequency

  This function will we testing the various methods, by which the sample
  frequency can be received or set:
  <ul>
    <li>Retrieve \f$ \nu_{\rm Sample} \f$ in natural frequency units, i.e. in [Hz]
    \verbatim
    sample frequency [Hz] = 8e+07
    \endverbatim
    <li>Retrieve \f$ \nu_{\rm Sample} \f$ in user-defined frequency units; this
    is a simple wrapper to e.g. retrieve \f$ \nu_{\rm Sample} \f$ in units of
    [MHz], which internally utilizes casa::Quanta for conversion.
    \verbatim
    sample frequency [GHz] = 0.08
    sample frequency [MHz] = 80
    sample frequency [kHz] = 80000
    \endverbatim
    <li>Set \f$ \nu_{\rm Sample} \f$ in natural frequency units, i.e. in [Hz]
    \verbatim
    sample frequency [Hz] = 1e+08
    \endverbatim
    <li>Set \f$ \nu_{\rm Sample} \f$ in user-defined frequency units
    \verbatim
    sample frequency [Hz] = 80
    sample frequency [Hz] = 80000
    sample frequency [Hz] = 8e+07
    \endverbatim
    <li>Set \f$ \nu_{\rm Sample} \f$ using a casa::Quantity
    \verbatim
    sample frequency [Hz] = 80
    sample frequency [Hz] = 80000
    sample frequency [Hz] = 8e+07
    \endverbatim
  </ul>
    
  \return nofFailedTests -- The number of failed tests.
*/
int test_sampleFrequency ()
{
  cout << "\n[test_sampleFrequency]\n" << endl;

  int nofFailedTests (0);
  double sampleFrequency (0.0);
  TimeFreq data;

  cout << "[1] Get the natural value of sample frequency" << endl;
  try {
    sampleFrequency = data.sampleFrequency();

    cout << " sample frequency [Hz] = " << sampleFrequency << endl;
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }
  
  cout << "[2] Retrieve sample frequency in user defined unit" << endl;
  try {
    sampleFrequency = data.sampleFrequency("GHz");
    cout << " sample frequency [GHz] = " << sampleFrequency << endl;
    //
    sampleFrequency = data.sampleFrequency("MHz");
    cout << " sample frequency [MHz] = " << sampleFrequency << endl;
    //
    sampleFrequency = data.sampleFrequency("kHz");
    cout << " sample frequency [kHz] = " << sampleFrequency << endl;
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }
  
  cout << "[3] Set sample frequency in natural units" << endl;
  try {
    sampleFrequency = 1e08;

    data.setSampleFrequency (sampleFrequency);

    sampleFrequency = data.sampleFrequency();

    cout << " sample frequency [Hz] = " << sampleFrequency << endl;
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }
  
  cout << "[4] Set sample frequency user defined units" << endl;
  try {
    data.setSampleFrequency (80,"Hz");
    sampleFrequency = data.sampleFrequency();
    cout << " sample frequency [Hz] = " << sampleFrequency << endl;
    //
    data.setSampleFrequency (80,"kHz");
    sampleFrequency = data.sampleFrequency();
    cout << " sample frequency [Hz] = " << sampleFrequency << endl;
    //
    data.setSampleFrequency (80,"MHz");
    sampleFrequency = data.sampleFrequency();
    cout << " sample frequency [Hz] = " << sampleFrequency << endl;
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }
  
  cout << "[5] Set sample frequency from Quantity" << endl;
  try {
    casa::Quantity rate1 (80,"Hz");
    data.setSampleFrequency (rate1);
    sampleFrequency = data.sampleFrequency();
    cout << " sample frequency [Hz] = " << sampleFrequency << endl;
    //
    casa::Quantity rate2 (80,"kHz");
    data.setSampleFrequency (rate2);
    sampleFrequency = data.sampleFrequency();
    cout << " sample frequency [Hz] = " << sampleFrequency << endl;
    //
    casa::Quantity rate3 (80,"MHz");
    data.setSampleFrequency (rate3);
    sampleFrequency = data.sampleFrequency();
    cout << " sample frequency [Hz] = " << sampleFrequency << endl;
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }
  
  return nofFailedTests;
}

// -----------------------------------------------------------------------------

/*!
  \brief Test retrival of the values along the time axis
  
  \return nofFailedTests -- The number of failed tests.
*/
int test_timeValues ()
{
  cout << "\n[test_timeValues]\n" << endl;

  int nofFailedTests (0);
  uint blocksize (2048);
  double sampleFrequency (80e06);
  uint nyquistZone (1);
  TimeFreq data (blocksize,sampleFrequency,nyquistZone);

  cout << "-- Sample frequency [Hz] = " << sampleFrequency << endl;
  cout << "-- Sample rate     [sec] = " << 1/sampleFrequency << endl;
  cout << "-- Blocksize   [samples] = " << data.blocksize() << endl;
  cout << "-- Blocksize       [sec] = " << data.blocksize()/sampleFrequency << endl;
  cout << endl;
  
  cout << "[1] timeValues()" << endl;
  try {
#ifdef HAVE_CASA
    casa::Vector<double> values = data.timeValues();
    cout << " [" << values(0) << " " << values(1)<< " " << values(2) << " ...]" << endl;
#else 
    std::vector<double> values = data.timeValues();
    cout << " [" << values[0] << " " << values[1]<< " " << values[2] << " ...]" << endl;
#endif
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }
  
  cout << "[2] timeValues(uint,bool=false)" << endl;
  try {
#ifdef HAVE_CASA
    casa::Vector<double> values = data.timeValues(1,false);
    cout << " [" << values(0) << " " << values(1)<< " " << values(2) << " ...]" << endl;
#else 
    std::vector<double> values = data.timeValues(1,false);
    cout << " [" << values[0] << " " << values[1]<< " " << values[2] << " ...]" << endl;
#endif
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }

  cout << "[2] timeValues(uint,bool=true)" << endl;
  try {
#ifdef HAVE_CASA
    casa::Vector<double> values = data.timeValues(1,true);
    cout << " [" << values(0) << " " << values(1)<< " " << values(2) << " ...]" << endl;
#else 
    std::vector<double> values = data.timeValues(1,true);
    cout << " [" << values[0] << " " << values[1]<< " " << values[2] << " ...]" << endl;
#endif
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }

  return nofFailedTests;
}

// -----------------------------------------------------------------------------

/*!
  \brief Test retrival of the values along the frequency axis

  \verbatim
  freq[0]   = 4e+07
  freq[1]   = 4.00391e+07
  freq[2]   = 4.00781e+07
  freq[N-2] = 7.99609e+07
  freq[N-1] = 8e+07
  
  freq. increment = 39062.5
  freq[1]-freq[0] = 39062.5
  \endverbatim
  
  \return nofFailedTests -- The number of failed tests.
*/
#ifndef HAVE_CASA
int test_frequencyValues ()
{
  cout << "\n[test_frequencyValues]\n" << endl;

  int nofFailedTests (0);
  uint blocksize (2048);
  double sampleFrequency (80e06);
  uint nyquistZone (2);
  TimeFreq data (blocksize,sampleFrequency,nyquistZone);
  vector<double> freq;

  cout << "[1] Frequency values for different Nyquist zones..." << endl;
  try {
    freq = data.frequencyValues();
    show_frequencies (freq,
		      data.nyquistZone(),
		      data.increment()[1]);
    
    data.setNyquistZone (3);
    
    freq = data.frequencyValues();
    show_frequencies (freq,
		      data.nyquistZone(),
		      data.increment()(1));
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }
  
  cout << "[2] Frequency channels selection by mask..." << endl;
  try {
    vector<bool> selection (data.fftLength(),true);

    selection[0] = selection[1] = selection[selection.size()-1] = false;

    freq = data.frequencyValues(selection);

    show_frequencies (freq,
		      data.nyquistZone(),
		      data.increment()(1));
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }
  
  cout << "[3] Frequency channels selection by range..." << endl;
  try {
    vector<double> range (2);

    range[0] = 85e+06;
    range[1] = 95e+06;

    // provide range as vector

    freq = data.frequencyValues(range);

    show_frequencies (freq,
		      data.nyquistZone(),
		      data.increment()(1));

    // provide range as limits

    freq = data.frequencyValues(range[1],range[0]);
    
    show_frequencies (freq,
		      data.nyquistZone(),
		      data.increment()(1));

  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }
  
  return nofFailedTests;
}
#endif

// -----------------------------------------------------------------------------

/*!
  \brief Test creation go coordinate axes from the time-frequency parameters

  \return nofFailedTests -- The number of failed tests.
*/
int test_coordinateAxes ()
{
  cout << "\n[test_coordinateAxes]\n" << endl;

  int nofFailedTests (0);

  uint blocksize (2048);
  double sampleFrequency (80e06);
  uint nyquistZone (2);
  TimeFreq data (blocksize,sampleFrequency,nyquistZone);

  cout << "[1] Coordinates axes using the default methods" << endl;
  try {
    LinearCoordinate timeAxis   = data.timeAxis();
    SpectralCoordinate freqAxis = data.frequencyAxis();
    /* Show properties of the time axis */
    cout << "Time axis" << endl;
    CR::CoordinateType::summary(std::cout,timeAxis);
    /* Show properties of the frequency axis */
    cout << "Frequency axis" << endl;
    CR::CoordinateType::summary(std::cout,freqAxis);
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }
  
  cout << "[2] Coordinates axes using the default methods" << endl;
  try {
    double refValue (100);
    double increment (0.1);
    double refPixel (1);

    LinearCoordinate timeAxis   = data.timeAxis(refValue,
						increment,
						refPixel);

    cout << "Time axis" << endl;
    CR::CoordinateType::summary(std::cout,timeAxis);

    SpectralCoordinate freqAxis = data.frequencyAxis(refValue,
						     increment,
						     refPixel);
    
    cout << "Frequency axis" << endl;
    CR::CoordinateType::summary(std::cout,freqAxis);
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }
  
  return nofFailedTests;
}

// -----------------------------------------------------------------------------

int main ()
{
  int nofFailedTests (0);
  
  // Test for the constructor(s)
  nofFailedTests += test_TimeFreq ();
  // Test accesss to the sample frequency in the ADC
  nofFailedTests += test_sampleFrequency ();
  // Test retrival of the time values
  nofFailedTests += test_timeValues ();
  // Test retrival of the frequency values
#ifndef HAVE_CASA
  nofFailedTests += test_frequencyValues ();
#endif
  // Test conversion of parameters to coordinate axes
  nofFailedTests += test_coordinateAxes ();
  
  return nofFailedTests;
}
