#ifndef WIENERFILTER_H
#define WIENERFILTER_H

/* Standard header files */
#include <iostream>
#include <string>
#include <vector>
/* FFTW3 header files */
#include <fftw3.h> 
// RM header files
#include "rmNumUtils.h"
#include "rmCube.h"

using namespace std;

#define DEBUG 1
//! statistical limit for number of line of sights before attempting a S(i,j) iteration
#define LOSLIMIT 100

namespace RM {  //  namespace RM

    
  /*!
    \class wienerfilter  
    \ingroup RM
    \brief Wiener Filter class
    
    \author Sven Duscha (sduscha@mpa-garching.mpg.de)
    
    \date 15-06-2009 (Last change: 09-11-2009)
  */
  class wienerfilter 
  {
    
  public:
    
    //! Signal matrix (complex double)
    cmat S;
    //! System response matrix (complex double)
    cmat R;
    //! (rms) noise matrix (real double)
    mat N;
    //   cmat N;	// (rms) noise matrix (complex double)
    cvec j;	// information source vector complex double vector ITTP type
    //! Data vector
    cvec d;
    //! Signal vector
    cvec s;
    vec eigen ;
    // Intermediate products
    cmat W;	// W = R.H()*N.inv()
    cmat M;	// M = W*R
    mat NI ;  // inverse of the noisematrix
    //! Final Wiener filter matrix: WF=(inv(S) + M)*W
    cmat WF;
    //! Propagator = error of final map, intermediate result (complex double) 
    cmat D;
    //! Map error (each Faraday Depth)
    cvec maperror;
    //! Instrumental fidelity (complex double)
    cmat Q;
    //! Reconstructed map vector m
    cvec m;
    
    // Observational parameters
    
    //! Epsilon zero emissivity at frequency nu zero
  double epsilon_0;
    //! flag indicates, that Responsematrix is allready generaed 
  bool R_ready ;
   //! flag indicates, that the Propagator is ready
  bool D_ready ;
   //! flag indicates, that the complete filter matrix is ready
  bool WF_ready ;
  //! Frequency nu for which emissivity epsilon zero applies
  double nu_0;
  //! Spectral dependence power law factor alpha
  double alpha;
  double variance_s;							// standard deviation of s, complex (since both for Q and U)
  double lambda_phi;							// lambda nu for spectral dependence coherence length
  vector<double> lambdas;						// lambdas in data
  vector<double> lambdasquareds;				// lambda squareds in data
  vector<double> delta_lambdasquareds;				// delta lambda squareds distance between lambda squareds
  vector<double> frequencies;					// frequencies present in observation
  vector<double> delta_frequencies;				// delta frequencies distances between frequencies
  vector<double> bandwidths;					// if we have bandwidths given, different from delta frequencies
  vector<double> faradaydepths;					// Faraday depths to probe for
  vector<double> delta_faradaydepths;			// delta faradays distance between Faraday depths
  
  // Instrumental parameters
  vector<complex<double> > bandpass;			// bandpass factor for each frequency

  unsigned long maxiterations;					// maximum number of iterations to perform
  unsigned long number_of_iterations; 			// number of iterations in this Wiener filtering
  double rmsthreshold;							// rms threshold till iterations stop

  // String constants for memmber attributes (mainly file access names, initialized with default names)
  string Sfilename;			// filename for S matrix file
  string Rfilename;			// filename for R matrix file
  string Nfilename;			// filename for N matrix file
  string Dfilename;			// filename for D matrix file
  string Qfilename;  			// filename for Q matrix file
  string Sformula;			// formula for initial S guess
  string Stype;				// type of spectral dependency

  //*****************************************************
  //
  // Wiener filtering algorithm functions
  //
  //*****************************************************/
  void createNoiseMatrix(double noise);		// create an instrument/simulated noise matrix, complex due to P=Q+iU and noise might differ in Q and U
  void createNoiseMatrix(vector<double> &noiseperchan);	// create a noise matrix with rms noise for each frequency channel
  void createResponseMatrix(vector<unsigned int> gaps, double eps, uint singVal );	 							// create the Response matrix frequencies as No. of columns, Faraday depths as No. of rows
  void createResponseMatrix(vector<double> intervals, double eps, uint singVal );	 							// create the Response matrix frequencies as No. of columns, Faraday depths as No. of rows
  void generateWienerFiltering(int noNoise, int flatPrior, double signal_var, double signal_corr, double noise_var, vector<unsigned int> gaps, double eps, unsigned int QR ) ;
  void generateWienerFiltering(int noNoise, double noise_var, vector<uint> gaps, cvec power, double eps, uint QR ) ;
  void generateWienerFiltering(int noNoise, double noise_var, vector<uint> gaps, cmat covMat, double eps, uint QR );
  void createResponseMatrixIntegral();	// integral form of Response matrix
  void createSMatrix(const string &);						// initial guess for the S matrix, Types: "whitesignal", "gaussian", "powerlaw", "deconvolution"
  void iterateForAll(rmCube &cubeIn, rmCube &cubeOut, double sigVar, double corLenght, double noiseVar,vector<uint> gaps, unsigned int smoothSteps, double powerEps) ;	// adjust S matrix from result of Wiener filtering
  void iterateMatForAll(rmCube &cubeIn, rmCube &cubeOut, double sigVar, double corLenght, double noiseVar,vector<uint> gaps);
  void iteratePowerSpectrum(cvec freqs, cvec faradays, cvec power0, cvec map, vector<uint> gaps, cvec epsilon, cvec delta, double aimDist);
  void iteratePowerSpectra(cvec power0, vector<cvec> maps, vector<uint> gaps, cvec epsilon, cvec delta, double aimDist, uint smoothSteps);
  void iteratePowerSpectraNaive(cvec power0, vector<cvec> datas, vector<uint> gaps, cvec epsilon, cvec delta,  double aimDist);
  void iterateCovMatrix(vector<cvec> &datas, vector<uint> &gaps, double aimDist);
  void iterateMatrixNaive(vector<cvec> &datas, vector<uint> &gaps, double aimDist);
  // Algorithm functions
  void computeVariance_s();				// compute dispersion_s (taken from data vector d)
  void computej();					// R^(dagger)N^(inverse)*d
  void computeD();					// D=R^(dagger)N^(inverse)R
  void computeD_NoSignalCov() ;
  void computeD_NoNoiseCov() ;

  // Compute intermediate product matrices
  void computeW();					// W = R^{dagger}*N^{inverse}
  void computeW_noNoise();
  void computeM();					// M = W*R

  void reconstructm();					// function to reconstruct the map m

  // Functions to free memory of matrices that are not needed anymore
  void deleteN();
  void deleteR();

  void applyWF(cvec &data, cvec &map);	// apply WF-Matrix to data vector
  vector<complex<double> > applyR(vector<complex<double> > &map); // apply response matrix to map vector
  vector<complex<double> > applyWF(vector<complex<double> >) ;
  void applyWF(std::vector<std::complex<double> > &data, std::vector<std::complex<double> > &map);

public:	
  wienerfilter();						// default constructor
  wienerfilter(int nfreqs, int nfaradays);
  wienerfilter(int nfreqs, vector<double> faradays);
  wienerfilter(int nfreqs, int nfaradays, std::string &formula);
  ~wienerfilter();							// destructor
	
  void smooth(cvec &power, uint num);
  void computeWF();	//! compute Wiener Filter operator
  
  
  // Member access functions (D and Q are results and handeled below)
//   double getVariance_s(void);					// for debugging get variance_s
  void setVariance_s(double s);					// debug: set variance_s

  double getLambdaPhi();										// get lambda_phi
  void setLambdaPhi(double lambdaphi);							// set lambda_phi
  vector<double> getLambdas();									// get lambdas vector
  void setLambdas(vector<double> &lambdas);						// set lambdas vector
  vector<double> getLambdaSquareds();							// get lambda squareds used in data
  void setLambdaSquareds(vector<double> &lambdasquareds);		// set lambda squareds used in data
  vector<double> getDeltaLambdaSquareds();		// get the delta Lambda Squareds associated with the data
  void setDeltaLambdaSquareds(vector<double> &delta_lambdasqs);	// set the delta Lambda Squareds associated with the data
  vector<double> getFrequencies();								// get frequencies vector wth observed frequencies
  void setFrequencies(vector<double> &freqs);					// set frequencies vector with to observed frequencies
  vector<double> getDeltaFrequencies();				// get delta frequencies distances between observed frequencies
  void setDeltaFrequencies(vector<double> &deltafreqs);		// set delta frequencies distances between observed frequencies
  vector<double> getFaradayDepths();				// get the Faraday depths to be probed for
  void setFaradayDepths(vector<double> &faraday_depths);	// set the Faraday depths to be probed for
  void calcSFromPowerspec(cvec power) ;
  vector<complex<double> > getBandpass();		// get bandpass for each frequency
  void setBandpass(vector<complex<double> > &);	// set bandpass for each frequency
  void createUnitBandpass() ; // create a bandpass for unit bandpass
  double getVariance_s();					// get dispersion_s (no set function only compute above)

  double getEpsilonZero();					// get epsilon zero value of spectral dependence
  void setEpsilonZero(double);					// set epsilon zero value of spectral dependence
  double getNuZero();						// get n� zero value of spectral dependence
  void setNuZero(double);					// set n� zero value of spectral dependence
  double getAlpha();						// get alpha power law exponent of spectral dependence
  void setAlpha(double);					// set alpha power law exponent of spectral dependence
  
  cmat getSignalMatrix();					// get S signal covariance matrix
  cmat getResponseMatrix();					// get R response matrix
  mat getNoiseMatrix();						// get N noise matrix
  cvec getInformationSourceVector();				// get j information source vector

  cmat getD();							// D
  cvec computeMapError();					// Diagonal elements of D matrix
//   cvec getMapError();						// return maperror from class attributes
  vector<complex<double> > getMapError();
  cmat getInstrumentFidelity();					// Q
  cmat getWienerFilter();					// WF final Wiener Filter matrix operator

  cvec getDataVector();							// get data in data vector d
  void setDataVector(cvec &);					// set data vector to content of cvec &data
  void setDataVector(vector<complex<double> > &);			// allows to input external data into data vector
  cvec getSignalVector();									// get the signal vector
  void setSignalVector(cvec &);								// DEBUG: this function is only for debugging!

  unsigned long getMaxIterations();				 			// get the limit of maximum iterations
  void setMaxIterations(unsigned long maxiterations);		// set the limit of maximum iterations

  unsigned long getNumberOfIterations();	// get current number of iterations

  string getSformula();						// get S matrix formula used for spectral dependence					
  string getStype();						// get type of spectral dependence

  double getRMSthreshold();					// get RMS limit threshold
  void setRMSthreshold(double rms);			// set RMS limit threshold

  // File access functions for configuration and algorithm parameters etc.
  void readConfigFile(const string &filename);					// read configuration from file
  void writeConfigFile(const string &filename);					// write configuration to file

  void readSignalMatrix(const string &filename);				// read Signal matrix from .it file
  void writeSignalMatrix(const string &filename);				// write Signal matrix to .it file
  void readNoiseMatrix(const string &filename);					// read Noise matrix from .it file
  void writeNoiseMatrix(const string &filename);				// write Noise matrix to .it file
  void readResponseMatrix(const string &filename);				// read Response matrix from .it file
  void writeResponseMatrix(const string &filename);				// write Response matrix to .it file
  void readPropagatorMatrix(const string &filename);			// read Propagator matrix from .it file
  void writePropagatorMatrix(const string &filename);			// write Propagator matrix to .it file
  void readInstrumentFidelityMatrix(const string &filename);	// read Instrument fidelity matrix from .it file
  void writeInstrumentFidelityMatrix(const string &filename);	// write Instrument fidelity matrix to .it file
  void saveAllMatrices();										// save all matrices to .it files

  //************************************************************************************
  //
  // Data I/O functions to convert to STL vector<double> and vector<complex<double> >
  //
  //************************************************************************************/
  void getData(vector<complex<double> > &cmplx);			// get data from STL complex vector
  void getData(vector<double> &re, vector<double> &im);			// get data from real and imaginary STL vectors
  void exportSignal(vector<complex<double> > &cmplx);			// export signal to STL complex vector
  void exportSignal(vector<double> &re, vector<double> &im);		// export signal to real and imaginary STL vectors

  //************************************************************************************
  //
  // Functions to read in simulated data from files
  //
  //************************************************************************************
  void readLambdaSqFromFile(const std::string &filename);		// read lambda squareds from a file

  void readLambdaSqAndDeltaLambdaSqFromFile(const std::string &filename);
  void readSimDataFromFile(const std::string &filename);		// read lambda squareds and complex intensity from file
  void read4SimDataFromFile(const std::string &filename);		// read complete set of simulated data: lambdasq, delta lambda sq and complex intensity from file
  void read4SimFreqDataFromFile(const std::string &filename);		// read complete frequency (!) set of simulated data: frequencies, delta frequencies and complex intensity
  void readSignalFromFile(const std::string &filename);			// read a real signal from file
  void readFaradayDepthsFromFile(const std::string &filename);
  void readRealDataFromFile(const std::string &filename);		// read real data from a file
  void readDataFromFile(const std::string &filename);			// read a data from a file and write it into the data vector

  //************************************************************************************ 
  //
  // ASCII (GNUplot) format writing functions
  //
  //************************************************************************************/

  void writeASCII(vector<double> v, const std::string &filename);	// output a STL vector
  void writeASCII(vector<double> v1, vector<double> v2, const std::string &filename);	// output a STL
  void writeASCII(vector<double> v, vector<complex<double> > cv, const std::string &filename);	// output a STL vector
  void writeASCII(vector<double>, cvec v, const std::string &filename);	// STL vector and a complex ITPP vector
  void writeASCII(vec v, const std::string &filename);			// output a vector to an ASCII file
  void writeASCII(cvec v, const std::string &filename);			// output a complex vector to an ASCII file

  // ASCII (GNUplot) with coordinates
  void writeASCII(vec coord, vec v, const std::string &filename);	// output a coordinate vector and a vector to an ASCII file
  void writeASCII(vec coord, cvec v, const std::string &filename);	// output a coordinate vector and a vector to an ASCII file

  void writeASCII(mat M, const std::string &filename);		// output a real matrix M to GNUplot ASCII format
  void writeASCII(cmat M, const std::string &filename);		// output a complex matrix M to GNUplot ASCII format
  void writeASCII(cmat M, const std::string &part, const std::string &filename);	// output the real or imaginary part of a complex matrix to GNUplot ASCII format

  // === Mathematica text format output routines ================================

  void writeMathematica (mat M,
			 const string &filename);
  void writeMathematica (cmat M,
			 const std::string &part,
			 const std::string &filename);

  // High-level output functions working directly on class attributes: data map etc.

  //! Write data vector d to an ASCII file
  void writeDataToFile(const std::string &filename);
  //! Write signal vector s to an ASCII file
  void writeSignalToFile(const std::string &filename);
  //! Write map vector m to an ASCII file
  void writeMapToFile(const std::string &filename);

  //! Create simple data
  void createData (double max,
		   double stretch,
		   double shift=0);
  //! Create a simple signal
  void createSignal (double max,
		     double stretch, 
		     double shift=0);
  //! Simple Response Matrix for testing
  void createSimpleResponseMatrix (int shift,
				   double scale);
  
  // === Helper functions =======================================================

  //! Compute P=sqrt(Q+iU)
  void complexPower( vector<complex<double> > &signal,
		    vector<double> &power);
  //! Compute P=sqrt(Q+iU)
  void complexPower( cvec &signal,
		    vector<double> &power);	
  complex<double> integrand(double x,double phi_a, double phi_b,double alpha) ;
  complex<double> integral(double nu_a, double nu_b, double eps,double alpha, double phi_a, double phi_b, int level ) ;
  complex<double> integral_approx(double nu_a, double nu_b, double eps,double alpha, double phi_a, double phi_b, int level ) ;
  void prepare(vector<double> &freqsC, vector<double> &freqsI, vector<double> &faras, double nu_0, double alpha, double epsilon, int method) ;
  void anaNoise(vector<double> &freqsC, vector<double> &freqsI, rmCube &cube, vector<double> &faras, double epsilon,double alpha) ;
  // === Noise functions for signal creation ====================================
  
/*   //! Add noise to data */
/*   void addNoise(double max); */
  
  };
  
  /*! class for constructor of an cvec object, which uses an own cvec object, and
      generates the new vector from it (not a copy, but a function of the vector)
      The Function is in this case newVek[i] = 1/(arg1*vek[i]+arg2)+arg3*vek[i] */
  class oneOverEps : public cVecFunk {
    cvec vek ;
    complex<double> arg1 ;
    complex<double> arg2 ;
    complex<double> arg3 ;
    public :
    oneOverEps(cvec vector, complex<double> ar1, complex<double> ar2, complex<double> ar3) {
      vek = vector ;
      arg1= ar1 ;
      arg2= ar2 ;
      arg3= ar3 ;
    }
    complex<double> function(uint index) {
      return 1.0/(arg1*vek[index]+arg2)+arg3*vek[index] ;
    }
  } ;
  
}  //  end namespace RM

#endif
