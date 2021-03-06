#ifndef __defined_util_hh
#define __defined_util_hh

#ifdef HAVE_POSTGRESQL
#include <libpq-fe.h>
#endif

//!star tool headers
#include "trigger.hh"
#include "reconstruction.hh"
#include "calibration.hh"


//!RecRadio headers
#include "RecRadioCalib.hh"
#include "RecRadioTrigger.hh"
#include "RecRadioUtil.hh"
#include "RecRadioBasic.hh"

//!std c++
#include <iostream>
#include <time.h>
#include <math.h>
#include <Riostream.h>
#ifdef HAVE_LIBGEN_H
#include <libgen.h>
#endif

//!Fast Fourier Transform of the West Version 3
#include <fftw3.h>


//!root 
#include <TROOT.h>
#include <TChain.h>
#include <TSystem.h>
#include <TGraph.h>
#include <TFile.h>
#include <TCanvas.h>
#include <TF1.h>
#include <TMath.h>
#include <TRandom.h>
#include <TChain.h>


using namespace std;
using namespace TMath;

//! RecRadio headers


/*!
  \file util.hh 
  \ingroup startools
  \brief Collection of useful functions and methods to work with LOPES^STAR data

 \date    Mar 04 2006
 \author  Thomas Asch
 
 to do
 - there is always something to do.. isn't it :-)
*/

  const unsigned int Max_window_size = 131072; 		/*!<max size of ADC data per event */
  const unsigned int Max_Time_t = 32; 			/*!<max length of timestamp */
  const unsigned int Max_Pol = 80;			/*!<max length of polarisation string*/
  const unsigned int Max_antenna_pos = 15;		/*!<max length of antenna position string*/
  const unsigned int Max_sample_id = 25;		/*!<max length of sample id string*/
  const unsigned int Max_char = 80;			/*!<max length of char arrays*/
  const unsigned int Max_notrigger = Max_window_size;	/*!<max length of selftrigger bit array*/
  const unsigned int Max_NoCh = 20;			/*!<max number of channels per antenna cluster, is not an absolute limit*/
  const unsigned int Max_SpecRes = 400000;		/*!<max number of frequency for correction*/
  const unsigned int MaxNoCross = 1000;			/*!<max number of threshold croissing points*/
  const unsigned int Max_char_Desc = 4096;		/*!<max characters for description*/

//! struct of the table events of the postgresql database
  struct event{
    unsigned int event_id;		/*!<event id*/
    char sample_id[Max_sample_id];	/*!<sample id*/
    char timestamp[Max_Time_t];		/*!<GPS timestamp*/
    char timestamp_db[Max_Time_t];	/*!<postgresql timestamp*/
    bool leap_second;			/*!<leap second*/
    unsigned int window_size;		/*!<window size of the ADC samples per event*/
    unsigned int channel_profile_id;	/*!<channel profile id*/
    short int trace[Max_window_size];	/*!<event data of one channel \sa channel_profile_id*/
    unsigned int delta_t;		/*!<ADC counter for clock ticks between events*/
  };

//! struct of the different tables of the postgresql database to create a header 
struct header{
    unsigned int run_id;		/*!<run id*/
    char start_time[Max_Time_t];	/*!<start time of the run \f$\cong\f$ timestamp of the first event*/
    char stop_time[Max_Time_t];		/*!<stop time of the run \f$\cong\f$ timestamp of the last event*/
    unsigned int profile_id;		/*!<profile id*/
    unsigned int posttrigger;		/*!<posttrigger time, is the time after the trigger*/
    unsigned int clock_frequency;	/*!<clock frequency of the ADC*/
    short int DAQ_id;			/*!<unique number of the DAQ*/
    char gps_position[Max_char];	/*!<description of the gps position*/
    char profile_name[Max_char];	/*!<name of the profile - short description*/
    
  };
  
//! struct of the table channel_profiles of the postgresql database  
  struct channel_profiles{
    unsigned int ch_id;			/*!<channel profile id*/
    int threshold;			/*!<threshold of the first trigger level, hardware parameter*/
    int coincidence;			/*!<coincidence time of three channels, hardware parameter*/
    unsigned int board_address;		/*!<board address of the ADC module, hardware parameter*/
    char antenna_pos[Max_antenna_pos]; 	/*!<description of the antenna position*/
    unsigned int polarisation_id;	/*!<polarisation id of a channel*/
    char polarisation[Max_Pol];		/*!<description of the polarisation, \sa polarisation_id*/
    unsigned int channel_no;		/*!<number of a channel*/
    
  };

//! struct of filter data used in the hardware
  struct filter{
    int order;		/*!<order of the used filter*/
    int upper_limit;	/*!<upper limit*/
    int lower_limit;	/*!<lower limit*/
  };


//! struct of the ntuple data from KASCADE-Grande analysis
  struct h3{
    unsigned int Gt;		/*!<global trigger time, in this case \f$\cong\f$ epoch time plus leap seconds*/
    unsigned int Mmn;		/*!<sub seconds of Gt*/
    unsigned char Fanka;	/*!<bit array that describes whether GRANDE detected ANKA events or not.
    				    Fanka & 0x7 = 4 means an ANKA event is detected*/
    unsigned int Hit7;		/*!<bit array that describes which GRANDE circle has triggered
    				    Hit7 & 0x80 != 0x80 means GRANDE 17 has triggered*/
    char Nflg;			/*!<trigger flag from the Grande reconstruction*/
    float Ageg;			/*!<Age parameter of the LDF function out of the Grande reconstruction*/
    float Sizeg;		/*!<total no of electrons reconstructed by Grande*/
    float Sizmg;		/*!<total no of muons reconstructed by Grande*/
    char Idmx;			/*!<Station ID with maximum energy deposit, if max. dep. at a station on border then IDMX<0*/
    float Zeg;		    	/*!<zenith angle [rad] reconstructed by Grande*/
    float Azg;			/*!<azimuth angle [rad] reconstructed by Grande*/
    char Ngrs; 			/*!<Number of Grande stations in event*/
    unsigned short Iact;	/*!<coded bit word for active parts of this run, if array is not active, then the muon no. is incorrect*/
    float Xcg; 			/*!<X value of the core position in m (Grande)*/
    float Ycg; 			/*!<Y value of the core position in m (Grande)*/
    UChar_t Ifil;		/*!<File ID*/				    
    UInt_t Ymd;			/*!<date*/
    UInt_t Hms;			/*!<time*/
    unsigned int Ieve;		/*!<Event ID*/
    UShort_t Irun;		/*!<run ID*/
    float Eg;			/*!<reconstructed energy by Grande calculated with formula of Michael W.*/

  };


//! struct of the RunOutFlag tree
  struct AnaFlag{
     int event_id;				/*!<id of the current event*/
     int DetCh;					/*!<detected channels in this event*/
     int run_id;				/*!<id of the current run*/
     int daq_id;				/*!<id of the current daq system*/
     int channel_id[Max_NoCh];			/*!<pointer with array size of DetCh containing the channel_profile_id*/
     int ana_index;				/*!<id of the analysis flag*/
     char description[Max_char_Desc];		/*!<contains a short description the flag meaning*/
     int tot_events;				/*!<total number of events in analysed run*/
     float PulseLength[Max_NoCh];		/*!<estimate of pulse length*/
     float PulsePosition[Max_NoCh];		/*!<postion of the detected pulse*/
     float PulseMax[Max_NoCh];			/*!<max value of the found peak*/
     int NoPulses[Max_NoCh];			/*!<no of detected pulses*/
     float snr[Max_NoCh];			/*!<signal to noise ratio*/
     float snr_mean[Max_NoCh];			/*!<mean of the processed snr trace*/
     float snr_rms[Max_NoCh];			/*!<rms of the processed snr trace*/
     bool L1Trigger;				/*!<Level one trigger flag*/
     float Int_Env[Max_NoCh];			/*!<integral value of the envelope signal*/
     float Int_Pre[Max_NoCh];			/*!<integral value of the part before the pulse begins*/
     float Int_Post[Max_NoCh];			/*!<integral value of the part after the pulse*/
     float Int_Raw[Max_NoCh];			/*!<integral value of the abs(raw) signal*/
     float Int_RawSq[Max_NoCh];			/*!<integral value of the squared raw signal*/
     float Int_Pulse[Max_NoCh];			/*!<integral value of the pulse region around the pulse width*/
     float FWHM[Max_NoCh];			/*!<Full Width Half Max of the peak in the envelope signal*/
     float DynThreshold[Max_NoCh];		/*!<dynamic threshold of each channel*/
     float CoincTime;				/*!<Coincidence Time of the given event*/
     float Azimuth;				/*!<Azimuth angle derived from plane fit*/
     float Azimuth_err;				/*!<Error of azimuth angle*/
     float Zenith;				/*!<Zenith angle derived from plane fit*/
     float Zenith_err;				/*!<Error of zenith angle*/
     float BAngle;				/*!<geomagnetic angle*/
     char timestamp[Max_Time_t];		/*!<GPS timestamp*/
     int NoCross00;			      	/*!<number of saved crossing points*/
     int NoCross01;			     	/*!<number of saved crossing points*/
     int NoCross02;			     	/*!<number of saved crossing points*/
     int NoCross03;			     	/*!<number of saved crossing points*/
     int NoCross04;			     	/*!<number of saved crossing points*/
     int NoCross05;			     	/*!<number of saved crossing points*/
     int NoCross06;			     	/*!<number of saved crossing points*/
     int NoCross07;			     	/*!<number of saved crossing points*/
     int NoCross08;			     	/*!<number of saved crossing points*/
     int NoCross09;			     	/*!<number of saved crossing points*/
     float ThresCross00[MaxNoCross];		/*!<pointer with the time difference of threshold crossing points for channel 00*/
     float ThresCross01[MaxNoCross];		/*!<pointer with the time difference of threshold crossing points for channel 01*/
     float ThresCross02[MaxNoCross];		/*!<pointer with the time difference of threshold crossing points for channel 02*/
     float ThresCross03[MaxNoCross];		/*!<pointer with the time difference of threshold crossing points for channel 03*/
     float ThresCross04[MaxNoCross];		/*!<pointer with the time difference of threshold crossing points for channel 04*/
     float ThresCross05[MaxNoCross];		/*!<pointer with the time difference of threshold crossing points for channel 05*/
     float ThresCross06[MaxNoCross];		/*!<pointer with the time difference of threshold crossing points for channel 06*/
     float ThresCross07[MaxNoCross];		/*!<pointer with the time difference of threshold crossing points for channel 07*/
     float ThresCross08[MaxNoCross];		/*!<pointer with the time difference of threshold crossing points for channel 08*/
     float ThresCross09[MaxNoCross];		/*!<pointer with the time difference of threshold crossing points for channel 09*/
     float MaxCrossDelay[Max_NoCh];		/*!<maximal found delay between two threshold crossings in the trace*/
  };

//! struct of the reconstructed event tree
  struct RecEvent{
     int event_id;			      	/*!<id of the current event*/
     int DetCh; 				/*!<detected channels in this event*/
     int run_id;				/*!<id of the current run*/
     int daq_id;				/*!<id of the current daq system*/
     int channel_id[Max_NoCh];			/*!<pointer with array size of DetCh containing the channel_profile_id*/

     float Azimuth1ThirdFit;			/*!<Azimuth angle derived from plane fit in units of degree*/
     float Azimuth1ThirdFit_err;		/*!<Error of azimuth angle in units of degree*/
     float Zenith1ThirdFit;			/*!<Zenith angle derived from plane fit in units of degree*/
     float Zenith1ThirdFit_err; 		/*!<Error of zenith angle in units of degree*/
     float AzimuthFWHMFit;			/*!<Azimuth angle derived from plane fit in units of degree*/
     float AzimuthFWHMFit_err; 			/*!<Error of azimuth angle in units of degree*/
     float ZenithFWHMFit;			/*!<Zenith angle derived from plane fit in units of degree*/
     float ZenithFWHMFit_err;  			/*!<Error of zenith angle in units of degree*/

     float Azimuth1Third;			/*!<Azimuth angle derived from plane fit in units of degree*/
     float Azimuth1Third_err; 			/*!<Error of azimuth angle in units of degree*/
     float Zenith1Third;			/*!<Zenith angle derived from plane fit in units of degree*/
     float Zenith1Third_err;  			/*!<Error of zenith angle in units of degree*/
     float AzimuthFWHM;				/*!<Azimuth angle derived from plane fit in units of degree*/
     float AzimuthFWHM_err; 			/*!<Error of azimuth angle in units of degree*/
     float ZenithFWHM;				/*!<Zenith angle derived from plane fit in units of degree*/
     float ZenithFWHM_err;  			/*!<Error of zenith angle in units of degree*/
     float AzimuthMax;				/*!<Azimuth angle derived from plane fit in units of degree*/
     float AzimuthMax_err; 			/*!<Error of azimuth angle in units of degree*/
     float ZenithMax;				/*!<Zenith angle derived from plane fit in units of degree*/
     float ZenithMax_err;  			/*!<Error of zenith angle in units of degree*/
     float BAngle;				/*!<geomagnetic angle in units of degree*/
     float EPhi[Max_NoCh];			/*!<phi component of the calibrated field strength vector (shower system) in units of field strength*/
     float ETheta[Max_NoCh];			/*!<theta component of the calibrated field strength vector (shower system) in units of field strengt*/

     char timestamp[Max_Time_t];		/*!<GPS timestamp*/

     float PulseLength1Third[Max_NoCh]; 	/*!<pulse length at 1 third of the max height in units of mus*/
     float PulseLengthFWHM[Max_NoCh];		/*!<pulse length at the half heigth in units of mus*/
     float PulsePosMax[Max_NoCh];		/*!<maximum postion of the detected pulse*/
     float PulsePos1Third[Max_NoCh];		/*!<postion of the detected pulse at 1 third of the heigth*/
     float PulsePosFWHM[Max_NoCh];		/*!<postion of the detected pulse at FWHM*/

     float PulseLength1ThirdFit[Max_NoCh]; 	/*!<pulse length at 1 third of the max height in units of mus*/
     float PulseLengthFWHMFit[Max_NoCh];	/*!<pulse length at the half heigth in units of mus*/
     float PulsePos1ThirdFit[Max_NoCh];		/*!<postion of the detected pulse at 1 third of the heigth*/
     float PulsePosFWHMFit[Max_NoCh];		/*!<postion of the detected pulse at FWHM*/

     float SNR[Max_NoCh];			/*!<signal to noise ratio*/

     float DistanceShowerAxis[Max_NoCh];	/*!<distance from the antenna to the shower axis (shower coordinate system) in m*/
     float DistanceShowerAxisErr[Max_NoCh];	/*!<error of distance from the antenna to the shower axis (shower coordinate system) in m*/

     float ShowerCoreXRadio;			/*!<reconstructed shower core position with radio in m*/
     float ShowerCoreYRadio;			/*!<reconstructed shower core position with radio in m*/

     float FieldStrengthChannel[Max_NoCh];	/*!<reconstructed field strength of the one channel in units of uV/m*/
     float FieldStrengthAntenna[Max_NoCh];	/*!<reconstructed field strength of both channels, one antenna in units of uV/m*/

     float FieldStrengthChannelFit[Max_NoCh];	/*!<reconstructed field strength of the one channel in units of uV/m*/
     float FieldStrengthAntennaFit[Max_NoCh];	/*!<reconstructed field strength of both channels, one antenna in units of uV/m*/
     
     float ChannelErrFieldStrengthFit[Max_NoCh];/*!<estimated error of field strength reconstruction of one channel in units of uV/m*/
     float ChannelErrFieldStrength[Max_NoCh];	/*!<estimated error of field strength reconstruction of one channel in units of uV/m*/
     float ChannelErrBackground[Max_NoCh];	/*!<estimated error of field strength reconstruction of one channel in units of uV/m*/
     
     char description[Max_char_Desc];		/*!<description and comments for the reconstruction*/
     float LDFSlope;				/*!<slope of the LDF distribution*/
  };

//! struct of self-trigger data
  struct selftrigger{
    unsigned int thres;				/*!<self-trigger threshold*/ 
    unsigned int notrigger;			/*!<no of self-triggered time bins*/ 
    unsigned int trigger[Max_notrigger];	/*!<array of self-triggered time bins, trigger bin is only set for odd ADC channels (North/South)*/ 
    unsigned int event_id;			/*!<event id*/
    unsigned int channel_profile_id;		/*!<channel profile id*/    
  };
  
//! struct of calibration output
  struct cal{
    float CorrValue[Max_SpecRes];	/*!<frequency dependet correction values for the field strength*/
    int channel_id[Max_NoCh];		/*!<channel id*/
    float MaxHF[Max_NoCh];		/*!<maximum of HF data in the time domain of the calibration pulses*/
    float MaxEnv[Max_NoCh];		/*!<maximum of the envelope in the time domain of the calibrationi pulses*/
    bool ADCoverflow;			/*!<flag for the ADC overflow of the calibrated channel*/
    int CalCh;				/*!<channel id of the measured channel*/
    int event_id;			/*!<event id*/
  };

  
//! create the Tevent branches
/*!
After you have defined your TChain of the input files, this function creates the branches needed to access the data.
All the variables are collected in the struct event.
\param Tevent is a pointer of type  TChain to the Tevent tree
\param event is a pointer of type struct event, this struct contains all variables which belong to the tree Tevent
*/
void CreateTevent(TChain *Tevent, struct event *event);


//! create the Tevent branches
/*!
Sets up the branches of the given TTree
\param Tevent is a pointer of type  TTree to the Tevent tree
\param event is a pointer of type struct event, this struct contains all variables which belong to the tree Tevent
*/
void CreateTevent(TTree *Tevent, struct event *event);


//! create extendet Tevent branches
/*!
Info about header and channel_profile is also set to the Tevent tree
\param Tevent is a pointer of type  TChain to the Tevent tree
\param header pointer to the struct header
\param channel_profiles pointer to the struct channel_profile 
*/
void CreateTeventExtend(TChain *Tevent, struct header *header, struct channel_profiles *channel_profiles);


//! create extendet Tevent branches
/*!
stucture for TTree object
\param Tevent is a pointer of type  TChain to the Tevent tree
\param header pointer to the struct header
\param channel_profiles pointer to the struct channel_profile 
*/
void CreateTeventExtend(TTree *Tevent, struct header *header, struct channel_profiles *channel_profiles);


//! create the Theader branches
/*!
Same like CreateTevent, but for the Theader tree.
\param Theader is a pointer of type  TChain to the Theader tree
\param header is a pointer of type struct header, this struct contains all variables which belong to the tree Theader
*/
void CreateTheader(TChain *Theader, struct header *header);


//! create the Tchannel_profiles branches
/*!
Same like CreateTevent, but for the Tchannel_profiles tree.
\param Tchannel_profile is a pointer of type  TChain to the Tchannel_profile tree
\param channel_profiles is a pointer of type struct channel_profiles, this struct contains all variables which belong to the tree Tchannel_profiles
*/
void CreateTchannel_profiles(TChain *Tchannel_profile, struct channel_profiles *channel_profiles);


//! create the TAnaFlag branches
/*!
Same like CreateTevent, but for the TAnaFlag tree.
\param TFlag is a pointer of type  TChain to the TAnaFlag tree
\param AnaFlag is a pointer of type struct AnaFlag, this struct contains all variables which belong to the tree TAnaFlag
*/
void CreateTAnaFlag(TChain *TFlag, struct AnaFlag *AnaFlag);


//! create the Tselftrigger branches
/*!
Same like CreateTevent, but for the Tselftrigger tree.
\param Tselftrigger is a pointer of type  TChain to the Tselftrigger tree
\param selftrigger is a pointer of type struct selftrigger, this struct contains all variables which belong to the tree Tselftrigger
\note the self-trigger bit is only present at odd ADC channels (north/south polarisation)
*/
void CreateTselftrigger(TChain *Tselftrigger, struct selftrigger *selftrigger);


//! create the TRecEvent branches
/*!
\param TRecEvent TChain object
\param RecEvent struct
*/
void CreateTRecEvent(TChain *TRecEvent, struct RecEvent *RecEvent);


//! create a TChain object
/*!
Create a TChain with all files of one run for the Tevent tree.
\param Tevent tree how keeps the event data
\param FileName path and name of the first file of one run
*/
void CreateTChain(TChain *Tevent, char *FileName);


//! create the TGrande branches
/*!
\param TGrande of the Grande reconstruction values
\param h3 struct to handle the Grande variables
\param Grande_orig when h3 is original filled with Grande data, this switch must be false. If you want to open an existing TGrande Tree this switch must set to true.
*/
void CreateTGrande(TChain *TGrande, struct h3 *h3, bool Grande_orig=false);


//! create the TGrande chain
/*!
\param TGrande chain object to add all found files
\param FileName list with all files used for the TChain
*/
void CreateTGrandeChain(TChain *TGrande, char *FileName);


//! set h3 struct variables to zero
/*!
\param h3 struct of type h3
*/
void SetStruct2Zero(struct h3 *h3);


//! set AnaFlag struct variables to zero
/*!
\param AnaFlag struct of type AnaFlag
*/
void SetStruct2Zero(struct AnaFlag *AnaFlag);


//! set RecEvent struct variables to zero
/*!
\param RecEvent struct of type RecEvent
*/
void SetStruct2Zero(struct RecEvent *RecEvent);


/*!
  \brief Converts timestamps

  The function converts timestamps in string format of the Meinberg GPS unit or
  the timestamp of the postgresql database.

  Description of the parameters:
  \param Timestamp string of the timestamp
  \param Sec no. of seconds since epoch plus leap seconds
  \param Nanosec sub seconds from the timestamp  and artificial expand to nanoseconds
  \param Timeshift it is also possible to shift the timestamp in time by \f$\Delta\f$t
  \return 1 if everything is ok, 0 if wrong timestamp is converted
*/
int ConvertTimeStamp (char *Timestamp,
		      int *Sec,
		      int *Nanosec,
		      double Timeshift = 0);

#ifdef HAVE_POSTGRESQL
/*!
  \brief Checks the result of a query 
  
  Call this function with a result pointer of PGresult and the function checks
  whether the result is valid or an error occurred.
  
  \param res is a pointer of the postgresql result, that have to be checked
*/
void ResCheck (PGresult* res);
#endif

//! Fast Fourier Transform of the Trace
/*!
This function makes a fft with the given trace and calculates the amplitude and phase distribution in the frequency domain.
\param window_size is the length of the raw ADC trace
\param trace is the pointer to the ADC samples with the length of window_size
\param Amp is a filled by the function with the amplitude distribution. The memory has to be allocated before calling the function. The length of the array must be window_size/2.
\param Phase is filled by the function with the phase distribution. The memory has also to be allocated before calling the function. The length must be window_size/2.
\param RawFFT is filled with the raw fft output. The first part is filled with the real and the second with the complex part of the transformation. The length of the array must be 2 times window_size.
\param data_window is a switch to turn of the window function of the raw data, if false is set, the window function is a rectangle.
\param power switch if data are squared
\param bSubtractPedestal switch to disable pedestal subtraction, if enabled, the pedestal is subtracted form the trace array
*/
void TraceFFT (int window_size,
	       short int *trace,
	       float *Amp,
	       float *Phase,
	       float *RawFFT,
	       bool data_window=true,
	       bool power=false,
	       bool bSubtractPedestal=true);

/*!
  \brief Fast Fourier Transform of the Trace

  Same TraceFFT with float type of trace
*/
void TraceFFT (int window_size, 
	       float *trace,
	       float *Amp,
	       float *Phase,
	       float *RawFFT,
	       bool data_window=true,
	       bool pwer=false,
	       bool bSubtractPedestal=true);

//! Core of the Fast Fourier Transform
/*!
this function is called from the TraceFFT Wrapper functions. This includes the core of the fft method
*/
void TraceFFTCore(int window_size, void *trace, bool Flag, float *Amp, float *Phase, float *RawFFT, bool data_window=true, bool power=false, bool bSubtractPedestal=true);


//! Inverse Fast Fourier Transform of the Trace
/*!
Same an TraceFFT but the inverse order
*/
void TraceiFFT(int window_size,  float *RawFFT, float *trace, int NoZeros=0);	      


//! Amplitude and Phase converted to a trace
/*!
You have to give this function just the amplitude and phase distribution and you will get back
the trace (not interpolated)
The window_size is the length of the original data. 
This function is not tested with interpolated data!!!
\param window_size defines the length of the ADC trace.
\param Amp Amplitude distribution with half of the window_size
\param Phase Phase distribution with half of the window_size
\param trace is the time signal with length of window_size
\param fft_data RawFFT data pointer to recalculate the inverse FFT
\param NoZeros number of used zeros
*/
void AmpPhase2Trace(int window_size,  float *Amp, float *Phase, float *trace, float* fft_data, int NoZeros = 0);


//! Band-path Filter 
/*!
This function calculates a band-path filter of the given trace. Two FFTs are necessary.
\param window_size defines the length of the ADC trace.
\param trace is the time signal with length of window_size
\param start_freq is the start frequency of the filter in MHz
\param stop_freq is the stop frequency of the filter in MHz
\param trace_out is a pointer of length window_size with the signal after filtering
*/
void BandFilterFFT(int window_size, float *trace, float start_freq, float stop_freq, float *trace_out);


//! Band-path Filter 
/*!
This function filters one given frequency out of the time signal.
\param window_size defines the length of the ADC trace.
\param trace is the time signal with length of window_size
\param freq is the frequency to filter out
\param trace_out is a pointer of length window_size with the signal after filtering
*/
void FrequencyFilter(int window_size, float *trace, float freq, float *trace_out);


//! Trace checker for ADC error
/*!
The given ADC trace is checked for each sample if during sampling an error occurred. If yes, the function returns false, in any other case true.
\param event is the pointer of the struct event to the trace
\return true if no sample is bigger than 4095, or false if one sample is bigger
*/
bool TraceCheck(struct event *event);


//! another fft algorithm for the trace
/*!
The alternative fft function seems to be not as acruate as the other one.
\param window_size is the length of the trace
\param trace pointer to the data
\param Amp float pointer of half of the length of window_size is filled with the amplitude distribution
\param Phase is filled with the phase information. Length is also half window_size
\param RawFFT must be a pointer with length of two window_sizes
\param data_window is switch to enable or disable a window function for the trace
*/
void TraceFFTReal(int window_size, short int *trace, float *Amp, float *Phase, float *RawFFT, bool data_window=true);


//! inverse fft of the alternative transform
/*
Variables are the same as in the TraceFFTReal function.
\param NoZeros is not important normal transform. This is used by zero padding
*/
void TraceiFFTReal(int window_size,  float *RawFFT, float *trace, int NoZeros=0);


//! alternative Zero Padding function
/*
  The used fft algorithm seems to make this zero padding very inaccurate.
  This function has to be checked better. Don't use it.
*/
void ZeroPaddingFFTReal (int window_size,
			 short int *trace,
			 int NoZeros,
			 float* ZPTrace);

/*!
  \brief correct the ADC clipping
  
  If the input voltage is higher than plus minus 0.5 volts, then the ADC will set
  the 13th bit. If the value is 4095 the clipping was to the lower side, if the
  value is 8192 the clipping was at the upper limit of the ADC range. This
  function should be used for Zero padding! Otherwise the interpolated values are
  more incorrect than without.
  
  \param window_size length of the trace array
  \param trace pointer to the raw samples without used the SubstractPedestal function!
*/
void CorrectADCClipping (unsigned int window_size,
			 short int *trace);


//! calculation of sigma
/*
\param window_size length of array trace
\param trace array with the data
*/
float GetSigma(int window_size, float *trace);


//! calculation of mean
/*
  \param window_size length of array trace
  \param trace array with the data
*/
float GetMean (int window_size,
	       float *trace);


//! calculation of RMS
/*
  \param window_size length of array trace
  \param trace array with the data
*/
float GetRMS (int window_size,
	      float *trace);

#endif
