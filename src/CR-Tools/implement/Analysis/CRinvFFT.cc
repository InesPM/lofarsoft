/*-------------------------------------------------------------------------*
 | $Id::                                                                 $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) 2007                                                    *
 *   Andreas Horneffer (<mail>)                                            *
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

// CASA
#include <scimath/Mathematics/FFTServer.h>
// CR-Tools
#include <Analysis/CRinvFFT.h>
#include <Analysis/CRdelayPlugin.h>
#include <Imaging/Beamforming.h>

namespace CR { // Namespace CR -- begin
  
  // ============================================================================
  //
  //  Construction
  //
  // ============================================================================
  
  //_____________________________________________________________________________
  //                                                                     CRinvFFT
  
  CRinvFFT::CRinvFFT ()
    : CR::SecondStagePipeline()
  {
    init();
  }
  
  //_____________________________________________________________________________
  //                                                                         init
  
  void CRinvFFT::init()
  {
    AntPosValid_p       = False;
    ploAntSelValid_p    = False;
    AntGainInterpInit_p = False;
    
    InterAntGain_p = NULL;
    
    Double tmpval=0.;
    DirParams_p.define("Xpos",tmpval);
    DirParams_p.define("Ypos",tmpval);
    DirParams_p.define("Curvature",tmpval);
    DirParams_p.define("coneAngle",tmpval); // default coneAngle = 0 means that spherical beamforming is used
    DirParams_p.define("Az",tmpval);
    tmpval=90.;
    DirParams_p.define("El",tmpval);
    ExtraDelay_p = 0.;
    GeomDelays_p.resize();
  };
  
  // ============================================================================
  //
  //  Destruction
  //
  // ============================================================================
  
  CRinvFFT::~CRinvFFT ()
  {
    destroy();
  }
  
  void CRinvFFT::destroy () {
    AntGainInterpInit_p = False;
    if (InterAntGain_p != NULL) {
      delete InterAntGain_p;
    }
  }
  
  // ============================================================================
  //
  //  Parameters
  //
  // ============================================================================
  
  void CRinvFFT::summary ()
  {;}
  
  //_____________________________________________________________________________
  //                                                               setPhaseCenter

  Bool CRinvFFT::setPhaseCenter (Double XC,
				 Double YC,
				 Bool rotate)
  {
    try {
      if (rotate) {
	Double XCn,YCn;
	// Rotation by 15.25 degrees
	// (GPS measurement of angle between KASCADE and LOPES coordinate system)
	// remark: the value in the Gauss-Kürger coordinate system is 15.7°
	// which would correspond to 15.23° to geographic north
	// Value used in the KRETA-evaluation is 15.0°.
	// cos 15.25° = 0.964787323  ;  sin 15.25° = 0.263031214
	XCn = XC*0.964787323+YC*0.263031214;
	YCn = XC*-0.263031214+YC*0.964787323;
	DirParams_p.define("Xpos",XCn);
	DirParams_p.define("Ypos",YCn);
      } else {
	DirParams_p.define("Xpos",XC);
	DirParams_p.define("Ypos",YC);
      };
    } catch (AipsError x) {
      cerr << "CRinvFFT::setPhaseCenter: " << x.getMesg() << endl;
      return False;
    }; 
    return True;    
  };

  //_____________________________________________________________________________
  //                                                                 setDirection

  Bool CRinvFFT::setDirection (Double AZ,
			       Double EL,
			       Double Curvature)
  {
    try {
      DirParams_p.define("Az",AZ);
      DirParams_p.define("El",EL);
      DirParams_p.define("Curvature",Curvature);	
    } catch (AipsError x) {
      cerr << "CRinvFFT::setDirection: " << x.getMesg() << endl;
      return False;
    }; 
    return True;    
  }


  //_____________________________________________________________________________
  //                                                                 setConeAngle

  Bool CRinvFFT::setConeAngle (Double coneAngle)
  {
    try {
      DirParams_p.define("coneAngle",coneAngle);
    } catch (AipsError x) {
      cerr << "CRinvFFT::setConeAngle: " << x.getMesg() << endl;
      return False;
    }; 
    return True;    
  }
  
  
  //_____________________________________________________________________________
  //                                                              GetAntPositions

  Matrix<Double> CRinvFFT::GetAntPositions (DataReader *dr,
					    bool substractReference)
  {
    Matrix<Double> NewAntPos;
    try {
      if (!AntPosValid_p && dr==NULL){
	cerr << "CRinvFFT::GetAntPositions: Called without DataReader while position cache is not valid!" 
	     << endl;
	return Matrix<Double>();
      };
      if (dr!=NULL && posCachedDate_p != dr->headerRecord().asuInt("Date") ){
	AntPosValid_p = False;
      };
      if (!AntPosValid_p ){
	Vector<Int> AntennaIDs;
	uInt i,date;
	Vector<Double> tmpvec;
	dr->headerRecord().get("Date",date);
	dr->headerRecord().get("AntennaIDs",AntennaIDs);
	tmpvec.resize(3);
	AntPositions_p.resize(3,AntennaIDs.nelements());
	for (i=0;i<AntennaIDs.nelements();i++){
	  CTRead->GetData(date, AntennaIDs(i), "Position", &tmpvec);
	  AntPositions_p.column(i) = tmpvec;
	};
	AntPosValid_p = True;
	posCachedDate_p = dr->headerRecord().asuInt("Date");
      };
      Int i,nants=AntPositions_p.ncolumn();
      Vector<Double> ReferencePos(3,0.);
      ReferencePos(0) = DirParams_p.asDouble("Ypos");
      ReferencePos(1) = DirParams_p.asDouble("Xpos");
      NewAntPos.resize(nants,3);
      for (i=0;i<nants;i++){
        if (substractReference)
          NewAntPos.row(i) = AntPositions_p.column(i)-ReferencePos;
        else
          NewAntPos.row(i) = AntPositions_p.column(i);
      };
    } catch (AipsError x) {
      cerr << "CRinvFFT::GetAntPositions: " << x.getMesg() << endl;
      return Matrix<Double>();
    }; 
    return NewAntPos;    
    
  };
  
  
  // ============================================================================
  //
  //  Protected Methods
  //
  // ============================================================================

  //_____________________________________________________________________________
  //                                                               initGainInterp

  Bool CRinvFFT::initGainInterp (DataReader *dr){
    Bool success=True;
    try {
      if (InterAntGain_p != NULL) {delete InterAntGain_p;};
      InterAntGain_p = new CalTableInterpolater<Double>;

      success = success && InterAntGain_p->doCacheInput();
      success = success && InterAntGain_p->AttatchReader(CTRead);
      success = success && InterAntGain_p->SetField("AntennaGainFaktor");
      success = success && InterAntGain_p->SetAxis("AntennaGainFaktFreq");
      success = success && InterAntGain_p->SetAxis("AntennaGainFaktAz");
      success = success && InterAntGain_p->SetAxis("AntennaGainFaktEl");
      success = success && InterAntGain_p->SetAxisValue(1,dr->frequencyValues());
     
      AntGainInterpInit_p = success;      
#ifdef DEBUGGING_MESSAGES      
      cout << "CRinvFFT::initGainInterp() successfully finished! "  << endl;
#endif
    } catch (AipsError x) {
      cerr << "CRinvFFT::initGainInterp: " << x.getMesg() << endl;
      return False;
    }; 
    return success;
  };

  //_____________________________________________________________________________
  //                                                                GetShiftedFFT

  Matrix<DComplex> CRinvFFT::GetShiftedFFT (DataReader *dr)
  {
    Matrix<DComplex> FFTdata;
    try {
      Vector<Int> AntennaIDs;
      uInt i,date;
      Vector<Double> tmpvec;
      dr->headerRecord().get("Date",date);
      dr->headerRecord().get("AntennaIDs",AntennaIDs);
      
      // ***** getting the phase gradients
      // update the antenna positions (if needed)
      if (!AntPosValid_p || (posCachedDate_p != dr->headerRecord().asuInt("Date")) ){
	tmpvec.resize(3);
	AntPositions_p.resize(3,AntennaIDs.nelements());
	for (i=0;i<AntennaIDs.nelements();i++){
	  CTRead->GetData(date, AntennaIDs(i), "Position", &tmpvec);
	  AntPositions_p.column(i) = tmpvec;
	};
	tmpvec.resize(2);
	AntPosValid_p = True;
	posCachedDate_p = dr->headerRecord().asuInt("Date");
      };
      // calculate the positions relative to the phase center
      Vector<Double> ReferencePos(3,0.);
      ReferencePos(0) = DirParams_p.asDouble("Ypos");
      ReferencePos(1) = DirParams_p.asDouble("Xpos");
      Matrix<Double> tmpAntPos(AntennaIDs.nelements(),3);
      for (i=0;i<AntennaIDs.nelements();i++){
	tmpAntPos.row(i) = AntPositions_p.column(i)-ReferencePos;
      };
      // get the direction
      tmpvec.resize(3);
      tmpvec(0) = DirParams_p.asDouble("Az");
      tmpvec(1) = DirParams_p.asDouble("El");
      tmpvec(2) = DirParams_p.asDouble("Curvature"); 
      
      // Container for the phase gradients
      Matrix<DComplex> phaseGradients;
      CR::GeomWeight geomWeight;
      
      // for conical beamforming is used, set curvature to large value
      // although the curvature has no influence on the conical beamforming,
      // it must not be 0 for technical reasons (normalization of the direction vector)
      if (DirParams_p.asDouble("coneAngle")>0) {
        tmpvec(2) = 1e5;
        geomWeight.setConeAngle(DirParams_p.asDouble("coneAngle"));
      } else {
        geomWeight.setConeAngle(0);
      }      
//       geomWeight.showProgress(verbose); 
      geomWeight.setAntPositions(tmpAntPos,
				 CR::CoordinateType::NorthEastHeight); 
      geomWeight.setSkyPositions(tmpvec,
				 CR::CoordinateType::AzElRadius,
				 True);
      geomWeight.setFrequencies(dr->frequencyValues());
      // set angle between shower plane and conical wavefront (if 0, spherical beamforming is used instead of conical beamforming)

#ifdef DEBUGGING_MESSAGES      
      geomWeight.summary();
#endif
      Cube<DComplex> tmpCcube;
      tmpCcube = geomWeight.weights();
      // store geometrical delays for later access
      //This also changes the shape of GeomDelays_p
      GeomDelays_p.assign(geomWeight.delays());
      //I think this is not really needed, but it'll ease some peoples mind.
      GeomDelays_p.unique();
#ifdef DEBUGGING_MESSAGES      
      cout << "CRinvFFT::GetShiftedFFT: delays: " << GeomDelays_p << endl;
#endif

      phaseGradients = Matrix<DComplex>(geomWeight.weights().nonDegenerate());

      // ***** getting the antenna gain calibration
      // initialize the caltable interpolater (if needed)      
      if (!AntGainInterpInit_p) { //AntGainInterpInit_p is defined in FirstStagePipeline.h and reset in InitEvent()
	if (!initGainInterp(dr)){
	  cerr << "CRinvFFT::GetShiftedFFT: " << "Error while initializing the CalTableInterpolater." << endl;
	  return Matrix<DComplex>();
	};
      }
            // Set the direction in the caltable interpolater
      tmpvec.resize(1);
      tmpvec(0) = DirParams_p.asDouble("Az");
      InterAntGain_p->SetAxisValue(2,tmpvec);
      tmpvec(0) = DirParams_p.asDouble("El");
      InterAntGain_p->SetAxisValue(3,tmpvec);
      // get the interpolated gains
      Matrix<DComplex> AntGainFactors(phaseGradients.shape());
      Vector<DComplex> tmpCvec;
      Array<Double> tmparr;
      DComplex tmpCval;
      tmpvec.resize(2);
      tmpCvec.resize(dr->fftLength());
      tmparr.resize(tmpCvec.shape());
      // If the calibration for the e-field should be done,
      // then take the gain simulation into accout.
      // Otherwise set the gain to 1 + 0i
      if (DoGainCal_p)
        for (i=0;i<AntennaIDs.nelements();i++){
	  InterAntGain_p->GetValues(date, AntennaIDs(i), &tmparr);
	  //CTRead->GetData(date, AntennaIDs(i), "FrequencyBand", &tmpvec);
	  //cout << "CRinvFFT::GetShiftedFFT: tmpCvec:" << tmpCvec.shape() << " tmparr:"<<tmparr.shape()
	  //     << " phaseGradients:"<< phaseGradients.shape() << endl;
#ifdef DEBUGGING_MESSAGES      
	  cout << "CRinvFFT::GetShiftedFFT: AntGain-shape: " << tmparr.shape() 
	       << " tmpCvec.shape(): "  << tmpCvec.shape() 
	       << " Freq-shape: " << dr->frequencyValues().shape() << endl;
#endif
	  convertArray(tmpCvec,Vector<Double>(tmparr));
	  // compute 1.947/delta_nu/sqrt(Gain)
	  //   1.947        = constant (at least for LOPES)
	  //   delta_nu     = (stopFreq_p-startFreq_p)/1e6 [converted to MHz]
	  //   1/sqrt(Gain) = tmpCvec
	  tmpCval = 1.947*1e6/(stopFreq_p-startFreq_p);

	  AntGainFactors.column(i) = tmpCvec*tmpCval;
        }
      else {
        AntGainFactors.set(DComplex (1.,0.));
      };
      //cout << "CRinvFFT::GetShiftedFFT: AntGainFactors:" << AntGainFactors.shape()
      //     << " phaseGradients:"<< phaseGradients.shape() << endl;
      FFTdata = phaseGradients*AntGainFactors*GetData(dr);

// Dummy until the GeometricalWeight class works as it is supposed to...
//      FFTdata = GetData(dr);

      // add extra delay to compensate for trigger etc.
      CRdelayPlugin crdel;
      crdel.parameters().define("Az",DirParams_p.asDouble("Az"));
      crdel.parameters().define("El",DirParams_p.asDouble("El"));
      crdel.parameters().define("ReferencePos",ReferencePos);
      crdel.parameters().define("frequencyValues",dr->frequencyValues());
      crdel.parameters().define("ExtraDelay",ExtraDelay_p);
      crdel.apply(FFTdata);
    } catch (AipsError x) {
      cerr << "CRinvFFT::GetShiftedFFT: " << x.getMesg() << endl;
      return Matrix<DComplex>();
    }; 
    return FFTdata;
  }

  // ============================================================================
  //
  //  Public Methods
  //
  // ============================================================================

  Matrix<Double> CRinvFFT::GetTimeSeries(DataReader *dr,
					 Vector<Bool> antennaSelection,
					 String Polarization){
    Matrix<Double> timeSeries;
    try {
      Matrix<DComplex> FFTData;
      FFTData = GetShiftedFFT(dr);
      uInt i,j,nants=FFTData.ncolumn(),nselants,blocksize=dr->blocksize();
      if (antennaSelection.nelements() != nants){
	antennaSelection = Vector<Bool>(nants,True);
      };
      // Select Antennas according to Polarization
      if (Polarization != "ANY"){
	uInt date;
	dr->headerRecord().get("Date",date);
	if (!ploAntSelValid_p || 
	    (ploAntSelPol_p != Polarization) ||
	    (ploAntSelDate_p != date)) {
	  ploAntSel_p.resize(nants);
	  ploAntSel_p = True;
	  Vector<Int> AntennaIDs;
	  String tempstring;
	  dr->headerRecord().get("AntennaIDs",AntennaIDs);
#ifdef DEBUGGING_MESSAGES      
	  cout << "CRinvFFT::GetTimeSeries: Polarization check, flagging Antennas: ";
#endif
	  for (i=0;i<nants;i++){
	    CTRead->GetData(date, AntennaIDs(i), "Polarization", &tempstring);
	    if (tempstring != Polarization) {
#ifdef DEBUGGING_MESSAGES      
	      cout << AntennaIDs(i) << " ";
#endif
	      ploAntSel_p(i) = False;
	    };
	  };//for
#ifdef DEBUGGING_MESSAGES      
	  cout << endl ;
#endif
	  ploAntSelPol_p   = Polarization;
	  ploAntSelDate_p  = date;
	  ploAntSelValid_p = True;
	};// if (!ploAntSelValid_p ...
	// Apply polarization-based selection
	antennaSelection = antennaSelection && ploAntSel_p;
      };
      nselants=ntrue(antennaSelection);
      if (nselants == 0) {
	cerr << "CRinvFFT::GetTimeSeries: " << "No antennas selected/all antennas flagged!" << endl;
      };
      timeSeries.resize(blocksize,nselants);
      j=0;
      for (i=0;i<nants;i++){
	if (antennaSelection(i)){
	  timeSeries.column(j) = dr->invfft(FFTData.column(i));
	  j++;
	};
      };      
    } catch (AipsError x) {
      cerr << "CRinvFFT::GetTimeSeries: " << x.getMesg() << endl;
      return Matrix<Double>();
    }; 
    return timeSeries;
  }

  //_____________________________________________________________________________
  //                                                                    GetCCBeam
  
  Vector<Double> CRinvFFT::GetCCBeam (DataReader *dr,
				      Vector<Bool> antennaSelection,
				      String Polarization)
  {
    Vector<Double> ccBeamData;
    try {
      Matrix<Double> TimeSeries;
      TimeSeries = GetTimeSeries(dr, antennaSelection, Polarization);
      if (TimeSeries.ncolumn() > 1) {
	ccBeamData = CR::calc_ccbeam<Double,DComplex> (TimeSeries);
      } else if (TimeSeries.ncolumn() == 1) {
	ccBeamData = abs(TimeSeries.column(0));
      } else {
	ccBeamData = Vector<Double>();
      };      
    } catch (AipsError x) {
      cerr << "CRinvFFT::GetCCBeam: " << x.getMesg() << endl;
      return Vector<Double>();
    }; 
    return ccBeamData;
  }

  //_____________________________________________________________________________
  //                                                                     GetXBeam
  
  Vector<Double> CRinvFFT::GetXBeam (DataReader *dr,
				     Vector<Bool> antennaSelection,
				     String Polarization)
  {
    Vector<Double> xBeamData;
    try {
      Matrix<Double> TimeSeries;
      TimeSeries = GetTimeSeries(dr, antennaSelection, Polarization);
      if (TimeSeries.ncolumn() > 1) {
	xBeamData = CR::calc_xbeam<Double,DComplex> (TimeSeries);
      } else if (TimeSeries.ncolumn() == 1) {
	xBeamData = abs(TimeSeries.column(0));
      } else {
	xBeamData = casa::Vector<Double>();
      };						 
    } catch (AipsError x) {
      cerr << "CRinvFFT::GetXBeam: " << x.getMesg() << endl;
      return Vector<Double>();
    }; 
    return xBeamData;
  }

  //_____________________________________________________________________________
  //                                                                     GetPBeam
  
  Vector<Double> CRinvFFT::GetPBeam (DataReader *dr,
				     Vector<Bool> antennaSelection,
				     String Polarization)
  {
    Vector<Double> pBeamData;
    try {
      Matrix<Double> TimeSeries;
      TimeSeries = GetTimeSeries(dr, antennaSelection, Polarization);
      Int i,nants=TimeSeries.ncolumn();

      if (TimeSeries.ncolumn() > 0) {
	pBeamData = square(TimeSeries.column(nants-1));
	for (i=0; i<(nants-1); i++){
	  pBeamData += square(TimeSeries.column(i));
	};
	pBeamData /= (Double)nants;
	pBeamData = sqrt(pBeamData);
      } else {
	pBeamData = Vector<Double>();
      }; 
    } catch (AipsError x) {
      cerr << "CRinvFFT::GetXBeam: " << x.getMesg() << endl;
      return Vector<Double>();
    }; 
    return pBeamData;
  }
  
  //_____________________________________________________________________________
  //                                                                      GetTCXP
  
  Bool CRinvFFT::GetTCXP (DataReader *dr,
			  Matrix<Double> & TimeSeries, 
			  Vector<Double> & ccBeamData,
			  Vector<Double> & xBeamData, 
			  Vector<Double> & pBeamData,
			  Vector<Bool> antennaSelection,
			  String Polarization) {
    try {
      TimeSeries = GetTimeSeries(dr, antennaSelection, Polarization);
      if (TimeSeries.ncolumn() > 1) {
	ccBeamData = CR::calc_ccbeam<Double,DComplex>(TimeSeries);
	xBeamData  = CR::calc_xbeam<Double,DComplex> (TimeSeries);
	
	Int i,nants=TimeSeries.ncolumn();
	pBeamData = square(TimeSeries.column(nants-1));
	for (i=0; i<(nants-1); i++){
	  pBeamData += square(TimeSeries.column(i));
	};
	pBeamData /= (Double)nants;
	pBeamData = sqrt(pBeamData);
      } else if (TimeSeries.ncolumn() == 1) {
	ccBeamData = xBeamData = pBeamData = abs(TimeSeries.column(0));
      } else {
	ccBeamData = xBeamData = pBeamData = Vector<Double>();
      };						 
    } catch (AipsError x) {
      cerr << "CRinvFFT::GetTCX: " << x.getMesg() << endl;
      return False;
    }; 
    return True;
  }

} // Namespace CR -- end
