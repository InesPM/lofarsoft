/*-------------------------------------------------------------------------*
 | $Id::                                                                 $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) 2008                                                  *
 *   Andreas Horneffer (<mail>)                                                     *
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

#include <Data/UpSampledDR.h>

#define DEBUGGING_MESSAGES 

namespace CR { // Namespace CR -- begin
  
  // ============================================================================
  //
  //  Construction
  //
  // ============================================================================
  
  UpSampledDR::UpSampledDR ()  
    : DataReader (1){
    init();
  }
  
  void UpSampledDR::init(){
    iterator_p = NULL;
    inpDR_p = NULL;
  }
  
  // ============================================================================
  //
  //  Destruction
  //
  // ============================================================================
  
  UpSampledDR::~UpSampledDR ()
  {
    destroy();
  }
  
  void UpSampledDR::destroy() {
    if (iterator_p != NULL) { 
      delete [] iterator_p; 
      iterator_p=NULL; 
    };
    inpDR_p = NULL;
  }
  
  // ============================================================================
  //
  //  Parameters
  //
  // ============================================================================
  
  void UpSampledDR::summary (std::ostream &os)
  {;}
  
  
  // ============================================================================
  //
  //  Methods
  //
  // ============================================================================
  
  Bool UpSampledDR::setup(DataReader *inputDR, double newSampleFrequency, 
			  Bool UseCalFFT, SecondStagePipeline *inpPipeline){
    try {
      if (inputDR == NULL){
	cerr << "UpSampledDR::setup: " << "Need a valid DataReader pointer!" << endl;
	return False;
      };
      Double SampleFreqRatio = newSampleFrequency/inputDR->sampleFrequency();
      if (SampleFreqRatio < 1.) {
	cerr << "UpSampledDR::setup: " << "newSampleFrequency smaller than original!" << endl;
	return False;
      };
      inpDR_p = inputDR;

      // --------------------------------------------
      // Functionality from the "setStreams" function
      // --------------------------------------------
      DataReader::setNyquistZone(1);
      DataReader::setSampleFrequency(newSampleFrequency);
      int i,nAntennas,blocksize;
      nAntennas = inpDR_p->nofAntennas();
      blocksize = (int)floor(inpDR_p->blocksize()*SampleFreqRatio);
      // Setup the Iterators
      if (iterator_p != NULL) { delete [] iterator_p; iterator_p=NULL; };
      iterator_p = new DataIterator[nAntennas];
      for (i=0; i<nAntennas; i++){
        iterator_p[i].setDataStart(0);
        iterator_p[i].setStride(0);
        iterator_p[i].setShift(0);
	//Set the width of a step to 1, so we count in samples:
 	iterator_p[i].setStepWidthToSample();
 	iterator_p[i].setBlocksize(blocksize);
      }
      // Setup the blocksize and "Calibration" arrays
      uint fftLength(blocksize/2+1);      
      Matrix<DComplex> fft2calfft(fftLength,nAntennas,1.0);
      Vector<Double> adc2voltage(nAntennas,1.);      
      DataReader::init(blocksize,
		       adc2voltage,
		       fft2calfft);   
      setReferenceTime(inpDR_p->referenceTime());

      // --------------------------------------------
      // Generate the header record
      // --------------------------------------------
      generateHeaderRecord();

      // Set "UseCalFFT_p"
      UseCalFFT_p = UseCalFFT;
      // Set "inpPipeline_p"
      // (Will automatically set inpPipeline_p to NULL if inpPipeline is NULL...)
      inpPipeline_p = inpPipeline;
      
    } catch (AipsError x) {
      cerr << "UpSampledDR::setup: " << x.getMesg() << endl;
      return False;
    }; 
    return True;
  }


  Bool UpSampledDR::generateHeaderRecord(){
    try {
      header_p.mergeField(inpDR_p->header(),"Date", RecordInterface::OverwriteDuplicates);
      header_p.mergeField(inpDR_p->header(),"AntennaIDs", RecordInterface::OverwriteDuplicates);
      header_p.mergeField(inpDR_p->header(),"Observatory", RecordInterface::OverwriteDuplicates);
      header_p.define("SampleFreq",DataReader::sampleFrequency());
    } catch (AipsError x) {
      cerr << "UpSampledDR::generateHeaderRecord: " << x.getMesg() << endl;
      return False;
    }; 
    return True;
  };

  Matrix<Double> UpSampledDR::fx() {
    Matrix<Double> fx;
    try {
      uint nofSelectedAntennas(DataReader::nofSelectedAntennas());
      if (nofSelectedAntennas >0) {
	uint antenna, sourceAnt, startsample, origSourceBlockNo, sourceBlockNo;
	uint sourceBlockSize=inpDR_p->blocksize(), sendBlocksize=DataReader::blocksize();
	uint restoreBlocksize, restoreFFTlen, sendoffset;
	Double SampleFreqRatio = DataReader::sampleFrequency()/inpDR_p->sampleFrequency();
	Vector<Bool> origSourceSelection, SSelection;
	Vector<Double> sourceFreqs, restoreFreqs;
	Matrix<DComplex> sourceFFT;
	Vector<DComplex> sourceFFTvec, restoreFFT;
	Vector<Double> restoreFX;
	startsample = iterator_p[selectedAntennas_p(0)].position();
	origSourceSelection  = inpDR_p->antennaSelection();
	SSelection.resize(origSourceSelection.shape());
	SSelection = True;
#ifdef DEBUGGING_MESSAGES      
	cout << "UpSampledDR::fx:" << " SSelection.shape: " << SSelection.shape() 
	     << ";" << inpDR_p->antennaSelection() << endl;
#endif
	inpDR_p->setSelectedAntennas(SSelection);

	origSourceBlockNo = inpDR_p->block();
	sourceBlockNo = (uint)floor((startsample/SampleFreqRatio)/sourceBlockSize);
	// Currently the DataReader and DataIterator start to count at 1
	inpDR_p->setBlock(sourceBlockNo+1);
	restoreBlocksize  = (uint)floor(sourceBlockSize*SampleFreqRatio);
	
	sourceFreqs = inpDR_p->frequencyValues();
	restoreFFTlen = restoreBlocksize/2+1;
	restoreFreqs.resize(restoreFFTlen);
	indgen(restoreFreqs); restoreFreqs *= DataReader::sampleFrequency()/restoreBlocksize;
	sendoffset = startsample - (uint)floor((sourceBlockNo*sourceBlockSize*SampleFreqRatio));
	if (sendoffset+sendBlocksize > restoreBlocksize) {
	  cerr << "UpSampledDR::fx: " << "Inconsistent parameters!" <<endl;
	  cerr << "                 " << "Requested block on source block boundary." << endl;
	  return Matrix<Double>();	  
	};
#ifdef DEBUGGING_MESSAGES      
	cout << "UpSampledDR::fx:" << " SampleFreqRatio: " << SampleFreqRatio 
	     << " sourceBlockSize: " << sourceBlockSize 
	     << " restoreBlocksize: " << restoreBlocksize << endl;
	cout << "                " << " startsample: " << startsample
	     << " sourceBlockNo: " << sourceBlockNo 
	     << " sendoffset: " << sendoffset << endl;
#endif
	fx.resize(sendBlocksize,nofSelectedAntennas);
	if ( inpPipeline_p != NULL) {
	  sourceFFT = inpPipeline_p->GetData(inpDR_p);
	} else if (UseCalFFT_p) {
	  sourceFFT = inpDR_p->calfft();
	} else {
	  sourceFFT = inpDR_p->fft();
	}
	FFTServer<Double,DComplex> fftserv(IPosition(1,restoreBlocksize), FFTEnums::REALTOCOMPLEX);
	restoreFX.resize(restoreBlocksize);

	Double minSourceFreq=min(sourceFreqs),maxSourceFreq=max(sourceFreqs);
	for (antenna=0; antenna<nofSelectedAntennas; antenna++) {
	  sourceAnt = selectedAntennas_p(antenna);
	  sourceFFTvec = sourceFFT.column(sourceAnt);
	  //sourceFFTvec(0) = 0.; sourceFFTvec(sourceFFTvec.nelements()-1) = 0.;
	  InterpolateArray1D<Double, DComplex>::interpolate(restoreFFT,restoreFreqs,
							    sourceFreqs,sourceFFTvec,
					      InterpolateArray1D<Double, DComplex>::nearestNeighbour);
	  restoreFFT(restoreFreqs<minSourceFreq) = 0.;
	  restoreFFT(restoreFreqs>maxSourceFreq) = 0.;
	  restoreFFT = restoreFFT*SampleFreqRatio;
	  fftserv.fft(restoreFX,restoreFFT);
	  fx.column(antenna) = restoreFX(Slice(sendoffset,sendBlocksize));
	};
	
	inpDR_p->setBlock(origSourceBlockNo);
	inpDR_p->setSelectedAntennas(origSourceSelection);
      };
    } catch (AipsError x) {
      cerr << "UpSampledDR::fx: " << x.getMesg() << endl;
      return Matrix<Double>();
    }; 
    return fx;
  }
  

} // Namespace CR -- end
