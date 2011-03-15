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

#include <Analysis/analyseLOPESevent.h>

#define DEG2RAD (PI/180.)

//#define DEBUGGING_MESSAGES

namespace CR { // Namespace CR -- begin
  
  // ============================================================================
  //
  //  Construction
  //
  // ============================================================================
  
  // ---------------------------------------------------------- analyseLOPESevent

  analyseLOPESevent::analyseLOPESevent(){
    init();
    clear();
  };
  
  // ---------------------------------------------------------- analyseLOPESevent

  analyseLOPESevent::analyseLOPESevent (double const &remoteStart,
					double const &remoteStop,
					double const &fitStart,
					double const &fitStop,
					double const &plotStart,
					double const &plotStop) {
    init();
    
    remoteStart_p = remoteStart;
    remoteStop_p  = remoteStop;
    fitStart_p    = fitStart;
    fitStop_p     = fitStop;
    plotStart_p   = plotStart;
    plotStop_p    = plotStop;
    
    clear();
  }

  // ---------------------------------------------------------------------- init

  void analyseLOPESevent::init() {
    pipeline_p     = NULL;
    upsamplePipe_p = NULL;
    beamPipe_p     = NULL;
    lev_p          = NULL;
    upsampler_p    = NULL;
    beamformDR_p   = NULL;
    remoteStart_p  = 1./4.;
    remoteStop_p   = 4./9.;
    fitStart_p     = -2e-6;
    fitStop_p      = -1.7e-6;
    plotStart_p    = -2.05e-6;
    plotStop_p     = -1.55e-6;
    freqStart_p    = 40e6;
    freqStop_p     = 80e6;
    filterStrength_p = 3;
    remoteRange_p.resize(2);
    Polarization_p = "ANY";
  }
  
  // ---------------------------------------------------------------------- clear

  void analyseLOPESevent::clear() {
    if (pipeline_p != NULL) {
      delete pipeline_p;
      pipeline_p = NULL;
    };
    if (upsamplePipe_p != NULL) {
      delete upsamplePipe_p;
      upsamplePipe_p = NULL;
    };
    if (lev_p != NULL) {
      delete lev_p;
      lev_p = NULL;
    };
    if (upsampler_p != NULL) {
      delete upsampler_p;
      upsampler_p = NULL;
    };
  };
  
  // ============================================================================
  //
  //  Destruction
  //
  // ============================================================================
  
  analyseLOPESevent::~analyseLOPESevent ()
  {
    clear();
  }
  
  // ============================================================================
  //
  //  Parameters
  //
  // ============================================================================
  
  void analyseLOPESevent::summary (std::ostream &os)
  {
    os << "[analyseLOPESevent] Summary of object properties" << std::endl;
    os << "-- remoteStart = " << remoteStart_p << std::endl;
    os << "-- remoteStop  = " << remoteStop_p  << std::endl;
  }
   
  // ============================================================================
  //
  //  Methods
  //
  // ============================================================================

  // --------------------------------------------------------------- initPipeline
  
  Bool analyseLOPESevent::initPipeline(Record ObsRecord){
    try {
      clear();
      pipeline_p = new CRinvFFT();
      upsamplePipe_p = new CRinvFFT();
      lev_p = new LopesEventIn();
      pipeline_p->SetObsRecord(ObsRecord);
      upsamplePipe_p->SetObsRecord(ObsRecord);
      
    } catch (AipsError x) {
      cerr << "analyseLOPESevent::initPipeline: " << x.getMesg() << endl;
      return False;
    }; 
    return True;
  };

  // --------------------------------------------------------------- ProcessEvent
  Record analyseLOPESevent::ProcessEvent(String evname,
					 Double Az,
					 Double El,
					 Double distance, 
					 Double XC,
					 Double YC,
					 Bool RotatePos,
					 String PlotPrefix, 
					 Bool generatePlots,
					 Vector<Int> FlaggedAntIDs, 
					 Bool verbose,
					 Bool simplexFit,
					 Double ExtraDelay,
					 int doTVcal,
					 Double UpSamplingRate,
					 String Polarization){
    Record erg;
    try {
      //Int nsamples;
      //
      //Matrix<Double> TimeSeries;
      //

      Vector<Bool> AntennaSelection;
      Double center;
      Record fiterg;
      
#ifdef DEBUGGING_MESSAGES
      if (!verbose){
	cout <<"analyseLOPESevent::ProcessEvent: compiled with DEBUGGING_MESSAGES, so switching on verbose output."<<endl;
	verbose = True;
      };
#endif
      if (! SetupEvent(evname, doTVcal, FlaggedAntIDs, AntennaSelection, 
		       UpSamplingRate, ExtraDelay, verbose)) {
	cerr << "analyseLOPESevent::ProcessEvent: " << "Error during SetupEvent()!" << endl;
	return Record();
      };
      if (! doPositionFitting(Az, El, distance, center, XC, YC, RotatePos,
			      AntennaSelection, Polarization, simplexFit, verbose, (distance<=0.) ) ){
	cerr << "analyseLOPESevent::ProcessEvent: " << "Error during doPositionFitting()!" << endl;
	return Record();
      };
      if (! GaussFitData(Az, El, distance, center, AntennaSelection, evname, erg, fiterg, 
			 Polarization, verbose) ){
	cerr << "analyseLOPESevent::ProcessEvent: " << "Error during GaussFitData()!" << endl;
	return Record();
      };
      if (generatePlots) {
	if (! doGeneratePlots(PlotPrefix, fiterg.asArrayDouble("Cgaussian"), 
			      fiterg.asArrayDouble("Xgaussian"), AntennaSelection, Polarization) ){
	  cerr << "analyseLOPESevent::ProcessEvent: " << "Error during generatePlots()!" << endl;
	};
      };
    } catch (AipsError x) {
      cerr << "analyseLOPESevent::ProcessEvent: " << x.getMesg() << endl;
      return Record();
    }; 
    return erg;
  }

  // --------------------------------------------------------------- SetupEvent  
  Bool analyseLOPESevent::SetupEvent(String evname, 
				     int doTVcal,
				     Vector<Int> FlaggedAntIDs,
				     Vector<Bool> &AntennaSelection,
				     Double UpSamplingRate,
				     Double ExtraDelay,
				     Bool verbose,
				     Bool doGainCal,
				     Bool doDispersionCal,
				     Bool doDelayCal,
				     Bool doRFImitigation,
				     Bool doFlagNotActiveAnts,
				     Bool doAutoFlagging){
    try {
      pipeline_p->setVerbosity(verbose);
      // Generate the Data Reader
      if (! lev_p->attachFile(evname) ){
	cerr << "analyseLOPESevent::SetupEvent: " << "Failed to attach file: " << evname << endl;
	return False;
      };

      //  Enable/disable calibration steps of the FirstStagePipeline (must be done before InitEvent)
      //  parameters are initialized with 'true' by default
      pipeline_p->doGainCal(doGainCal);
      pipeline_p->doDispersionCal(doDispersionCal);
      pipeline_p->doDelayCal(doDelayCal);
      pipeline_p->doFlagNotActiveAnts(doFlagNotActiveAnts);
      pipeline_p->setFreqInterval(freqStart_p,freqStop_p);

      // initialize the Data Reader
      if (! pipeline_p->InitEvent(lev_p)){
	cerr << "analyseLOPESevent::SetupEvent: " << "Failed to initialize the DataReader!" << endl;
	return False;
      };

      //  Enable/disable doing the phase calibration as requested
      switch (doTVcal){
      case 0:
	pipeline_p->doPhaseCal(False);
	break;
      case 1:
	pipeline_p->doPhaseCal(True);
	break;
      default:
	if ( lev_p->headerRecord().isDefined("EventClass") &&
	     lev_p->headerRecord().asInt("EventClass") == LopesEventIn::Simulation ){
	  if (verbose){
	    cout << "Simulation event: ";
	  };
	  pipeline_p->doPhaseCal(False);
	} else {
	  pipeline_p->doPhaseCal(True);
	};
	break;
      };

      //  Enable/disable doing the RFI mitigation
      pipeline_p->doRFImitigation(doRFImitigation);

      // Generate the antenna selection
      Int i,j,id,nants,nselants, nflagged=FlaggedAntIDs.nelements();
      AntennaSelection.resize();
      AntennaSelection = pipeline_p->GetAntennaMask(lev_p);
      nants = AntennaSelection.nelements();
      Vector<Int> AntennaIDs,selAntIDs;
      lev_p->headerRecord().get("AntennaIDs",AntennaIDs);
      if (nflagged >0){
	for (i=0; i<nflagged; i++){
	  id = FlaggedAntIDs(i);
	  for (j=0; j<nants; j++){
	    if (AntennaIDs(j) == id) {
	      AntennaSelection(j) = False;
	      id = 0;
	      break;
	    };
	  };
	  if (verbose && (id !=0)){
	    cout << "analyseLOPESevent::SetupEvent: " << "AntennaID: " << id 
		 << " not found -> no antenna flagged." << endl;
	  };
	};
      };

      // Flag antennas (due to bad signal or because marked as not active in the CalTables)
      if (doAutoFlagging) {
        flagger.calcWeights(pipeline_p->GetTimeSeries(lev_p));
        AntennaSelection = AntennaSelection && flagger.parameters().asArrayBool("AntennaMask");
      }
      nselants = ntrue(AntennaSelection);

      //Do the upsampling (if requested)	
      Vector<Double> Times;
      if (UpSamplingRate >= lev_p->sampleFrequency()) {
	if (upsampler_p == NULL){ upsampler_p = new UpSampledDR(); };
	upsampler_p->setup(lev_p, UpSamplingRate, False, pipeline_p);
	upsamplePipe_p->InitEvent(upsampler_p,False);
	upsamplePipe_p->doPhaseCal(False);
	upsamplePipe_p->doRFImitigation(False);
	upsamplePipe_p->setVerbosity(verbose);
	
	filterStrength_p = (int)floor(3*UpSamplingRate/lev_p->sampleFrequency());
	//filterStrength_p = (int)ceil(UpSamplingRate/{bandwidth});

	Double tmpdouble;
	int upBlockSize,upBlock;
	// Calculate the blocksize for the upsampled data
	// smallest power of 2 that is larger tha 3*([fit or plot range]+1 microsec)
	tmpdouble = (max(fitStop_p,plotStop_p)-min(fitStart_p,plotStart_p)+1e-6)*UpSamplingRate;
	upBlockSize = (uint)(pow(2.0,ceil(log(tmpdouble*3)/log(2.0))));
	// Calculate where to start the block 
	upsampler_p->timeValues(Times);
	upBlock = ntrue( Times < ((fitStop_p+fitStart_p)/2.))/upBlockSize;
	
	
	// Set blocksize and shift
	upsampler_p->setBlocksize(upBlockSize);
	upsampler_p->setBlock(upBlock);
	Times.resize();
	if (verbose) {
	  upsampler_p->timeValues(Times);
	  cout << "analyseLOPESevent::SetupEvent: " << "Upsampling: upBlockSize: " << upBlockSize
	       << " upBlock: " << upBlock << " filter: " << filterStrength_p << endl;
	  cout << "analyseLOPESevent::SetupEvent: " << " min(Times): " << min(Times)*1e6 
	       << " max(Times): " << max(Times)*1e6 << endl;
	};
	beamformDR_p = upsampler_p;
	beamPipe_p = upsamplePipe_p;
      } else {
	beamformDR_p = lev_p;
	beamPipe_p =  pipeline_p;
      };
      
      beamPipe_p->setExtraDelay(ExtraDelay);
      beamformDR_p->timeValues(Times);
      int nsamples = Times.nelements();      
      //initialize the fitter
      remoteRange_p(0) = (uInt)(nsamples*remoteStart_p);
      remoteRange_p(1) = (uInt)(nsamples*remoteStop_p);
      fitObject.setTimeAxis(Times);
      fitObject.setRemoteRange(remoteRange_p);
      fitObject.setFitRangeSeconds(fitStart_p,fitStop_p);
      
    } catch (AipsError x) {
      cerr << "analyseLOPESevent::SetupEvent: " << x.getMesg() << endl;
      return False;
    }; 
    return True;
  };

  // ---------------------------------------------------------- doPositionFitting

  Bool analyseLOPESevent::doPositionFitting(Double &Az, Double &El, Double &distance, 
					    Double &center,
					    Double &XC, Double &YC, Bool RotatePos,
					    Vector<Bool> AntennaSelection, 
					    String Polarization,
					    Bool simplexFit,
					    Bool verbose,
					    Bool distanceSearch){
    try {    
      if (Polarization != ""){
	Polarization_p = Polarization;
      };
      // Set shower position
      if (! beamPipe_p->setPhaseCenter(XC, YC, RotatePos)){
	cerr << "analyseLOPESevent::doPositionFitting: " << "Error during setPhaseCenter()!" << endl;
	return False;
      };
      //perform the position fitting
      center=-1.8e-6;
      if (simplexFit) {
        if (distanceSearch) {
	  if (! findDistance(Az, El, distance, AntennaSelection, &center, True, verbose) ){
	    cerr << "analyseLOPESevent::doPositionFitting: " << "Error during findDistance()!" << endl;
	    return False;
	  };
	};
	if (verbose) { cout << "analyseLOPESevent::doPositionFitting: starting evaluateGrid()." << endl;};
	if (! evaluateGrid(Az, El, distance, AntennaSelection, &center) ){
	  cerr << "analyseLOPESevent::doPositionFitting: " << "Error during evaluateGrid()!" << endl;
	  return False;
	};
        if (distanceSearch) {
	  if (! findDistance(Az, El, distance, AntennaSelection, &center, False, verbose) ){
	    cerr << "analyseLOPESevent::doPositionFitting: " << "Error during findDistance()!" << endl;
	    return False;
	  };
	};
	if (verbose) { cout << "analyseLOPESevent::doPositionFitting: starting SimplexFit()." << endl;};

	if (distanceSearch) {
	  if (! SimplexFit(Az, El, distance, center, AntennaSelection, 1000.) ){
  	    cerr << "analyseLOPESevent::doPositionFitting: " << "Error during SimplexFit()!" << endl;
	    return False;
	  };
	} else {
	  if (! SimplexFit(Az, El, distance, center, AntennaSelection, 2000.) ){
	    cerr << "analyseLOPESevent::doPositionFitting: " << "Error during SimplexFit()!" << endl;
	    return False;
	  };
	};
	beamPipe_p->setVerbosity(verbose);
      };
    } catch (AipsError x) {
      cerr << "analyseLOPESevent::doPositionFitting: " << x.getMesg() << endl;
      return False;
    }; 
    return True;
  };


  // ---------------------------------------------------------- doPositionFitting

  Bool analyseLOPESevent::doConicalPositionFitting(Double &Az, Double &El, Double &coneAngle, 
                                                   Double &center,
                                                   Double &XC, Double &YC, Bool RotatePos,
                                                   Vector<Bool> AntennaSelection, 
                                                   String Polarization,
                                                   Bool simplexFit,
                                                   Bool verbose,
                                                   Bool coneAngleSearch){
    try {
      Double dummyDistance = 1e5; // distance does not matter for conical beamforming
      if (Polarization != ""){
        Polarization_p = Polarization;
      };
      // Set shower position
      if (! beamPipe_p->setPhaseCenter(XC, YC, RotatePos)){
        cerr << "analyseLOPESevent::doConicalPositionFitting: " << "Error during setPhaseCenter()!" << endl;
        return False;
      };
      //perform the position fitting
      center=-1.8e-6;
      if (simplexFit) {
        if (coneAngleSearch) {
          if (! findConeAngle(Az, El, coneAngle, AntennaSelection, &center, True, verbose) ){
            cerr << "analyseLOPESevent::doConicalPositionFitting: " << "Error during findDistance()!" << endl;
            return False;
          };
        };
        if (verbose) { cout << "analyseLOPESevent::doPositionFitting: starting evaluateGrid()." << endl;};
        if (! evaluateGrid(Az, El, dummyDistance, AntennaSelection, &center, coneAngle) ){
          cerr << "analyseLOPESevent::doConicalPositionFitting: " << "Error during evaluateGrid()!" << endl;
          return False;
        };
        if (coneAngleSearch) {
          if (! findConeAngle(Az, El, coneAngle, AntennaSelection, &center, False, verbose) ){
            cerr << "analyseLOPESevent::doConicalPositionFitting: " << "Error during findDistance()!" << endl;
            return False;
          };
        };
        if (verbose) { cout << "analyseLOPESevent::doConicalPositionFitting: starting SimplexFit()." << endl;};

        if (coneAngleSearch) {
          if (! SimplexFit(Az, El, dummyDistance, center, AntennaSelection, 0., coneAngle, 0.001, True) ){
            cerr << "analyseLOPESevent::doConicalPositionFitting: " << "Error during SimplexFit()!" << endl;
            return False;
          };
        } else {
          if (! SimplexFit(Az, El, dummyDistance, center, AntennaSelection, 0., coneAngle, 0.002, True) ){
            cerr << "analyseLOPESevent::doConicalPositionFitting: " << "Error during SimplexFit()!" << endl;
            return False;
          };
        };
        beamPipe_p->setVerbosity(verbose);
      };
    } catch (AipsError x) {
      cerr << "analyseLOPESevent::doConicalPositionFitting: " << x.getMesg() << endl;
      return False;
    }; 
    return True;
  };
        


  // --------------------------------------------------------------- GaussFitData
  Bool analyseLOPESevent::GaussFitData(Double &Az, Double &El, Double &distance, Double &center, 
				       Vector<Bool> AntennaSelection, String evname, 
				       Record &erg, Record &fiterg,
				       String Polarization, Bool verbose,
                                       Double coneAngle){
    try {      
      Vector<Double> ccBeam, xBeam,pBeam;
      Matrix<Double> TimeSeries;
      
      if (Polarization != ""){
	Polarization_p = Polarization;
      };
      // Get the beam-formed data
      if (! beamPipe_p->setConeAngle(coneAngle)){ //default coneAngle = 0 means spherical beamforming
	cerr << "analyseLOPESevent::GaussFitData: " << "Error during setConeAngle()!" << endl;
	return False;
      };
      if (! beamPipe_p->setDirection(Az, El, distance)){
        cerr << "analyseLOPESevent::GaussFitData: " << "Error during setDirection()!" << endl;
        return False;
      };
      if (! beamPipe_p->GetTCXP(beamformDR_p, TimeSeries, ccBeam, xBeam, pBeam, 
				AntennaSelection, Polarization_p)){
	cerr << "analyseLOPESevent::GaussFitData: " << "Error during GetTCXP()!" << endl;
	return False;
      };
      // smooth the data
      StatisticsFilter<Double> mf(filterStrength_p,FilterType::MEAN);
      ccBeam = mf.filter(ccBeam);
      xBeam = mf.filter(xBeam);
      pBeam = mf.filter(pBeam);

      Slice remoteRegion(remoteRange_p(0),(remoteRange_p(1)-remoteRange_p(0)));
      ccBeam = ccBeam - mean(ccBeam(remoteRegion));
      xBeam = xBeam - mean(xBeam(remoteRegion));
      pBeam = pBeam - mean(pBeam(remoteRegion));

      // do the fitting
      fiterg = fitObject.Fitgauss(xBeam, ccBeam, True, center);
      if ( !fiterg.isDefined("Xconverged") || !fiterg.isDefined("CCconverged") ){
	cerr << "analyseLOPESevent::GaussFitData: " << "Error during fit!" << endl;
	return False;	
      };

      if (verbose) {
	cout << "Event \"" << evname << "\" fit results:" << endl;
	if (fiterg.asBool("Xconverged")){
	  cout << " Fit to X-Beam has converged:" << endl;
	} else {
	  cout << " Fit to X-Beam has _not_ converged:" << endl;
	};
	cout << "  XHeight: " << fiterg.asDouble("Xheight") << " +/- " 
	     <<fiterg.asDouble("Xheight_error") << endl;
	cout << "  XWidth : " << fiterg.asDouble("Xwidth") << " +/- "
	     <<fiterg.asDouble("Xwidth_error") << endl;
	cout << "  XCenter: " << fiterg.asDouble("Xcenter") << " +/- "
	     <<fiterg.asDouble("Xcenter_error") << endl;
	if (fiterg.asBool("CCconverged")){
	  cout << " Fit to CC-Beam has converged:" << endl;
	} else {
	  cout << " Fit to CC-Beam has _not_ converged:" << endl;
	};
	cout << "  CCHeight: " << fiterg.asDouble("CCheight") << " +/- " 
	     <<fiterg.asDouble("CCheight_error") << endl;
	cout << "  CCWidth : " << fiterg.asDouble("CCwidth") << " +/- "
	     <<fiterg.asDouble("CCwidth_error") << endl;
	cout << "  CCCenter: " << fiterg.asDouble("CCcenter") << " +/- "
	     <<fiterg.asDouble("CCcenter_error") << endl;
      };
      
      //copy fit results to record
      erg.mergeField(fiterg,"Xconverged", RecordInterface::OverwriteDuplicates);
      erg.mergeField(fiterg,"Xheight",RecordInterface::OverwriteDuplicates);
      erg.mergeField(fiterg,"Xwidth",RecordInterface::OverwriteDuplicates);
      erg.mergeField(fiterg,"Xcenter",RecordInterface::OverwriteDuplicates);
      erg.mergeField(fiterg,"Xheight_error",RecordInterface::OverwriteDuplicates);
      erg.mergeField(fiterg,"Xwidth_error",RecordInterface::OverwriteDuplicates);
      erg.mergeField(fiterg,"Xcenter_error",RecordInterface::OverwriteDuplicates);
      erg.mergeField(fiterg,"CCconverged",RecordInterface::OverwriteDuplicates);
      erg.mergeField(fiterg,"CCheight",RecordInterface::OverwriteDuplicates);
      erg.mergeField(fiterg,"CCwidth",RecordInterface::OverwriteDuplicates);
      erg.mergeField(fiterg,"CCcenter",RecordInterface::OverwriteDuplicates);
      erg.mergeField(fiterg,"CCheight_error",RecordInterface::OverwriteDuplicates);
      erg.mergeField(fiterg,"CCwidth_error",RecordInterface::OverwriteDuplicates);
      erg.mergeField(fiterg,"CCcenter_error",RecordInterface::OverwriteDuplicates);

      // calculate other stuff
      erg.define("rmsC",stddev(ccBeam(remoteRegion)));
      erg.define("rmsX",stddev(xBeam(remoteRegion)));
      Matrix<Double> AntPos; 
      AntPos=beamPipe_p->GetAntPositions();
      AntPos = toShower(AntPos, Az, El);
      int nants = AntennaSelection.nelements();
      int nselants = ntrue(AntennaSelection);
      Vector<Double> distances(nants);
      for (int i=0; i<nants; i++) {
	distances(i) = sqrt( square(AntPos.row(i)(0)) + square(AntPos.row(i)(1)) );
      };
      erg.define("distances",distances);
      Vector<Double> tmpvec;
      tmpvec.resize(nselants);
      tmpvec = distances(AntennaSelection).getCompressedArray() ;
      erg.define("meandist",mean(tmpvec));
      erg.define("Azimuth",Az);
      erg.define("Elevation",El);
      erg.define("Distance",distance);
      erg.define("coneAngle",coneAngle);

    } catch (AipsError x) {
      cerr << "analyseLOPESevent::GaussFitData: " << x.getMesg() << endl;
      return False;
    }; 
    return True;
  }

  // --------------------------------------------------------------- doGeneratePlots
  Bool analyseLOPESevent::doGeneratePlots(String PlotPrefix, Vector<Double> ccgauss, 
					  Vector<Double> xgauss, Vector<Bool> AntennaSelection,
					  String Polarization){
    try {
      if (Polarization != ""){
	Polarization_p = Polarization;
      };
      Int nsamples=0,i=0,j=0,nants=0,nselants=0;
      Vector<Double> Times, ccBeam, xBeam, pBeam, tmpvec;
      Matrix<Double> TimeSeries;
      
      beamformDR_p->timeValues(Times);
      nsamples = Times.nelements();
      nants = AntennaSelection.nelements();
      if (! beamPipe_p->GetTCXP(beamformDR_p, TimeSeries, ccBeam, xBeam, pBeam, 
				AntennaSelection, Polarization_p)){
	cerr << "analyseLOPESevent::generatePlots: " << "Error during GetTCXP()!" << endl;
	return False;
      };
      nselants = TimeSeries.ncolumn();
      StatisticsFilter<Double> mf(filterStrength_p,FilterType::MEAN);
      ccBeam = mf.filter(ccBeam);
      xBeam = mf.filter(xBeam);
      pBeam = mf.filter(pBeam);
      Slice remoteRegion(remoteRange_p(0),(remoteRange_p(1)-remoteRange_p(0)));
      ccBeam = ccBeam - mean(ccBeam(remoteRegion));
      xBeam = xBeam - mean(xBeam(remoteRegion));
      pBeam = pBeam - mean(pBeam(remoteRegion));
      i = ntrue(Times<plotStart_p);
      j = ntrue(Times<plotStop_p);
      Slice plotRegion(i,(j-i));
      Matrix<Double> parts((j-i),nselants);
      Vector<Double> xarr, yarr, empty, gauss;
      String plotname;
      SimplePlot plotter;
      xarr = Times(plotRegion) * 1e6;
      //plot the CC-beam
      plotname = PlotPrefix;
      plotname += "-CC.ps";
      yarr = ccBeam(plotRegion) * 1e6;
      plotter.quickPlot(plotname, xarr, yarr, empty, empty,
			"Time [#gmseconds]", "CC-Beam [#gmV/m/MHz]","",9,
			True, 1, False, False, True);
      yarr = pBeam(plotRegion) *1e6;
      plotter.PlotLine(xarr,yarr,4,3);
      yarr = ccgauss(plotRegion) *1e6;
      plotter.PlotLine(xarr,yarr,8,2);
      // plot the X-Beam
      plotname = PlotPrefix;
      plotname += "-X.ps";
      yarr = xBeam(plotRegion) * 1e6;
      plotter.quickPlot(plotname, xarr, yarr, empty, empty,
			"Time [#gmseconds]", "X-Beam [#gmV/m/MHz]","",9,
			True, 1, False, False, True);
      yarr = pBeam(plotRegion) *1e6;
      plotter.PlotLine(xarr,yarr,4,3);
      yarr = xgauss(plotRegion) *1e6;
      plotter.PlotLine(xarr,yarr,8,2);
      // plot all antenna traces
      plotname = PlotPrefix;
      plotname += "-all.ps";
      Double ymax,ymin,xmax,xmin,tmpval;
      Vector<Int> AntennaIDs,selAntIDs;
      beamformDR_p->headerRecord().get("AntennaIDs",AntennaIDs);
      selAntIDs.resize(nselants);
      AntennaIDs(AntennaSelection).getCompressedArray(selAntIDs);
      tmpvec.resize(nselants);
      for (i=0; i<nselants; i++){
	  parts.column(i) = TimeSeries.column(i)(plotRegion)*1e6;
	  tmpvec(i) = max(abs(parts.column(i)));
      };
      //ymax = max(parts); ymin = min(parts);
      //ymax = 2*abs(erg.asDouble("CCheight")); ymin = -ymax;
      ymax = median(tmpvec)*1.05; ymin = -ymax;
      xmin = min(xarr); xmax = max(xarr);
      tmpval = (xmax-xmin)*0.05;
      xmin -= tmpval;
      xmax += tmpval;
      plotter.InitPlot(plotname, xmin, xmax, ymin, ymax);
      plotter.AddLabels("Time [#gmseconds]", "FieldStrength [#gmV/m/MHz]");
      Int col1,col2,col3;
      col1=col2=col3=3;
      for (i=0; i<nselants; i++){
	if (selAntIDs(i) > 70000) { 
	  plotter.PlotLine(xarr,parts.column(i),col3++,3);
	} else if (selAntIDs(i) > 40000) {
	  plotter.PlotLine(xarr,parts.column(i),col2++,2);
	} else {
	  plotter.PlotLine(xarr,parts.column(i),col1++,1);
	};
      };
    } catch (AipsError x) {
      cerr << "analyseLOPESevent::generatePlots: " << x.getMesg() << endl;
      return False;
    }; 
    return True;
  }

  // ------------------------------------------------------------------- toShower

  Matrix<Double> analyseLOPESevent::toShower (const Matrix<Double> & pos,
					      Double Az,
					      Double El){
    Matrix<Double> erg;
    try {
      if (pos.ncolumn() != 3) {
	cerr << "analyseLOPESevent:toShower: " << "invalis parameters: pos.ncolumn() != 3 " << endl;
	return Matrix<Double>();
      };
      erg.resize(pos.shape());

      Double azrad,elrad;
      azrad = Az*DEG2RAD;
      elrad = El*DEG2RAD;

      Double l1, l2, l3, m1, m2, m3, n1, n2, n3;
      
      l1 = -sin(azrad);
      l2 = cos(azrad);
      l3 = 0.;

      m1 = sin(elrad)*cos(azrad);
      m2 = sin(elrad)*sin(azrad);
      m3 = -cos(elrad);

      n1 = cos(elrad)*cos(azrad);
      n2 = cos(elrad)*sin(azrad);
      n3 = sin(elrad);
      
      erg.column(0) = pos.column(0)*l1 + pos.column(1)*l2;
      erg.column(1) = pos.column(0)*m1 + pos.column(1)*m2 + pos.column(2)*m3;
      erg.column(2) = pos.column(0)*n1 + pos.column(1)*n2 + pos.column(2)*n3;

    } catch (AipsError x) {
      cerr << "analyseLOPESevent:toShower: " << x.getMesg() << endl;
      return Matrix<Double>();
    }; 
    return erg;
  };


  // overload SimplexFit, to allow optional coneAngle argurment
  // to switch between spherical and conical beamforming
  
  // for spherical beamforming (default):
  Bool analyseLOPESevent::SimplexFit (Double &Az,
                                      Double &El,
                                      Double &distance,
                                      Double &center, 
                                      Vector<Bool> AntennaSelection,
                                      Double distanceStep){
    Double dummyConeAngle=0, dummyAngleStep=0;                                      
    return SimplexFit(Az,El,distance,center,AntennaSelection,distanceStep,dummyConeAngle,dummyAngleStep, False);                                      
  }                                      

  // general call for conical or spherical beamforming
  Bool analyseLOPESevent::SimplexFit (Double &Az,
				      Double &El,
				      Double &distance,
				      Double &center, 
				      Vector<Bool> AntennaSelection,
				      Double distanceStep,
                                      Double &coneAngle,
                                      Double angleStep,
                                      Bool conicalBeamforming){    
    try {
      Int i=0,minpos=0,maxpos=0,niteration=0,oldminpos=0,nsameiter=0;
      Record erg;
      Vector<Double> azs(4), els(4), dists(4), height(4,0.), cents(4), coneAngles(4);
      Double newaz=0., newel=0., newdist=0., newheight=0., nnewheight=0., newcent=0., nnewcent=0., newconeAngle=0.;
      Double meanaz=0., meanel=0., meandist=0., vecaz=0., vecel=0., vecdist=0., meanconeAngle=0., vecconeAngle=0.;
      Bool running=True;
      Double dummyDistance=1e5; // dummy distance for conical beamforming, since distance has no effect, but must be != 0
#define Xstop 3.
#define CCstop 0.5

      // starting fit with xbeam
      SkymapQuantity::Type beamtype = SkymapQuantity::TIME_X;

      if (beamPipe_p == NULL){
	cerr << "analyseLOPESevent:SimplexFit: " << "Error: beamPipe_p == NULL " << endl;
	return False;	
      };
      if (beamformDR_p == NULL){
	cerr << "analyseLOPESevent:SimplexFit: " << "Error: beamformDR_p == NULL " << endl;
	return False;	
      };
      beamPipe_p->setVerbosity(False);

      // set start values
      azs = Az; els = El; dists = distance; coneAngles = coneAngle;
      azs(1) += 1; els(2) += 1; 
      azs(3) -= 1; els(3) -= 1;      
      dists(2) -= distanceStep;dists(1) += distanceStep;dists(3) += distanceStep;
      coneAngles(2) += angleStep; coneAngles(1) -= angleStep; coneAngles(3) -= angleStep;
      cents = center;
      for (i=0; i<4; i++){
        if (conicalBeamforming)
          getBeamHeight(azs(i), els(i), dummyDistance, AntennaSelection, &height(i), beamtype, &cents(i), coneAngles(i));
        else
	  getBeamHeight(azs(i), els(i), dists(i), AntennaSelection, &height(i), beamtype, &cents(i));
      };
      minpos=0; nsameiter=0;oldminpos=0;
      while(running){ // the big loop
	// find worst point
	for (i=0; i<4; i++){
	  if ((i!=minpos) && (height(i)<height(minpos))){
	    minpos = i;
	  };
	};
	// find the vector from minpoint to centerpoint
	meanaz = meanel = meandist = 0.;
	for (i=0; i<4; i++){
	  if (i!=minpos){
	    meanaz += azs(i); meanel += els(i); meandist += dists(i); meanconeAngle += coneAngles(i);
	  };
	};
	meanaz /= 3.; meanel /= 3.; meandist /= 3.; meanconeAngle /= 3.;
	vecaz = meanaz-azs(minpos); vecel = meanel-els(minpos); vecdist = meandist-dists(minpos);
        vecconeAngle = meanconeAngle-coneAngles(minpos);
	if (minpos == oldminpos){
	  nsameiter++;
	} else {
	  nsameiter=0;
	  oldminpos = minpos;
	};
	if ( (nsameiter==8) || ((abs(vecaz)<0.03) && (abs(vecel)<0.03) && 
             ((!conicalBeamforming&&(abs(vecdist)<1)) || (conicalBeamforming&&(abs(vecconeAngle)<0.00001))) ) ) {
	  // restart from close to best position
	  maxpos=0;
	  for (i=1; i<4; i++){
	    if (height(i)>height(maxpos)){
	      maxpos = i;
	    };
	  };
	  azs(minpos) = azs(maxpos)+0.2;
	  els(minpos) = els(maxpos)+0.2;
	  dists(minpos) = dists(maxpos)+50;
          coneAngles(minpos) = coneAngles(maxpos)+0.0001;
	  cents(minpos) = cents(maxpos);
          if (conicalBeamforming)
	    getBeamHeight(azs(minpos),els(minpos),dummyDistance, AntennaSelection, &height(minpos), beamtype, &cents(minpos), coneAngles(minpos));
          else  
            getBeamHeight(azs(minpos),els(minpos),dists(minpos), AntennaSelection, &height(minpos), beamtype, &cents(minpos));
	  continue;
	};
	if (nsameiter==16) {
	  // We are seriously stuck. Just give up...
	  running = False;
	}
	//new point
	newaz = azs(minpos) + 2.*vecaz;	newel = els(minpos) + 2.*vecel;
	newdist = dists(minpos) + 2.*vecdist;
        newconeAngle = coneAngles(minpos) + 2.*vecconeAngle;
	newcent = cents(minpos);
        if (conicalBeamforming)       
	  getBeamHeight(newaz, newel, dummyDistance, AntennaSelection,&newheight, beamtype, &newcent, newconeAngle);
        else
          getBeamHeight(newaz, newel, newdist, AntennaSelection,&newheight, beamtype, &newcent);
        
	if (newheight > max(height)) { //expand the simplex
	  cout << " SimplexFit: expanding simplex"<<endl;
	  newaz = azs(minpos) + 4.*vecaz; newel = els(minpos) + 4.*vecel;
	  newdist = dists(minpos) + 4.*vecdist;
          newconeAngle = coneAngles(minpos) + 4.*vecconeAngle;       
	  nnewcent = cents(minpos);
          if (conicalBeamforming)       
            getBeamHeight(newaz, newel, dummyDistance, AntennaSelection, &nnewheight, beamtype, &nnewcent, newconeAngle);
          else
            getBeamHeight(newaz, newel, newdist, AntennaSelection, &nnewheight, beamtype, &nnewcent);
	  if (newheight > nnewheight) {
	    newaz = azs(minpos) + 2.*vecaz; newel = els(minpos) + 2.*vecel;
	    newdist = dists(minpos) + 2.*vecdist;
            newconeAngle = coneAngles(minpos) + 2.*vecconeAngle;
	  } else {
	    newheight = nnewheight;
	    newcent = nnewcent;
	  };
	} else {
	  height(minpos) = max(height);
	  if (newheight < min(height)) { 
	    cout << " SimplexFit: reducing simplex"<<endl;
	    newaz = azs(minpos) + 1.5*vecaz; newel = els(minpos) + 1.5*vecel;
	    newdist = dists(minpos) + 1.5*vecdist;
            newconeAngle = coneAngles(minpos) + 1.5*vecconeAngle;
	    nnewcent = cents(minpos);
            if (conicalBeamforming)       
              getBeamHeight(newaz, newel, dummyDistance, AntennaSelection, &nnewheight, beamtype, &nnewcent, newconeAngle);
            else
              getBeamHeight(newaz, newel, newdist, AntennaSelection, &nnewheight, beamtype, &nnewcent);
	    if (newheight > nnewheight) {
	      newaz = azs(minpos) + 2.*vecaz; newel = els(minpos) + 2.*vecel;
	      newdist = dists(minpos) + 2.*vecdist;
              newconeAngle = coneAngles(minpos) + 2.*vecconeAngle;
	    } else {
	      newheight = nnewheight;
	      newcent = nnewcent;
	    };
	  };
	};
	// save the new position
	Bool clipping=True;
	while (clipping){
	  if (newel>90.) { newel=180.-newel; continue;};
	  if (newel<0.) { newel=-newel; continue;};
	  if (newdist<0.) { newdist=-newdist; continue;};
          if (newconeAngle<0.) { newconeAngle=-newconeAngle; continue;};
	  clipping = False;
	};
	azs(minpos) = newaz; els(minpos) = newel; dists(minpos) = newdist; cents(minpos) = newcent;
        coneAngles(minpos) = newconeAngle;
	height(minpos)=newheight;
	// generate output
	printf("## %i ------------------------------- \n",niteration);
	printf("  Azimuth:   %6.2f; %6.2f; %6.2f; %6.2f \n",azs(0),azs(1),azs(2),azs(3));
	printf("  Elevation: %6.2f; %6.2f; %6.2f; %6.2f \n",els(0),els(1),els(2),els(3));
        if (conicalBeamforming)
	  printf("  Rho[rad]:  %6.4f; %6.4f; %6.4f; %6.4f \n",coneAngles(0),coneAngles(1),coneAngles(2),coneAngles(3));
        else
          printf("  Distance:  %6.1f; %6.1f; %6.1f; %6.1f \n",dists(0),dists(1),dists(2),dists(3));
	printf("  Center:    %6.3f; %6.3f; %6.3f; %6.3f \n",cents(0)*1e6,cents(1)*1e6,cents(2)*1e6,cents(3)*1e6);
	printf("  Height:    %6.3f; %6.3f; %6.3f; %6.3f \n",height(0),height(1),height(2),height(3));
	// test convergence
	meanaz = max(azs)-min(azs); meanel = max(els)-min(els);
	meanaz *= cos(mean(els)*DEG2RAD);
	meanaz = abs(meanaz*meanel);
	meandist = max(dists)-min(dists);
        meanconeAngle = max(coneAngles)-min(coneAngles);
	
	
	// switching to ccbeam if convergence gets better
	if ((beamtype == SkymapQuantity::TIME_X) && (meanaz<0.01*Xstop) &&
            ((!conicalBeamforming&&((meandist/mean(dists))<0.025*Xstop)) ||  
             (conicalBeamforming&&((meanconeAngle/mean(coneAngles))<0.025*Xstop))) ) {
	  cout << "analyseLOPESevent:SimplexFit: Switching to fit to ccbeam." << endl;
	  beamtype = SkymapQuantity::TIME_CC;
	  // replacing xbeam fitresults with ccbeam fitresults
	  for (i=0; i<4; i++){
            if (conicalBeamforming)       
              getBeamHeight(azs(i),els(i),dummyDistance, AntennaSelection, &height(i), beamtype, &cents(i), coneAngles(i) );
            else
	      getBeamHeight(azs(i),els(i),dists(i), AntennaSelection, &height(i), beamtype, &cents(i) );
	  };
	};

	// Stop fit if converged
	if ((meanaz<0.01*CCstop) && 
            ((!conicalBeamforming&&((meandist/mean(dists))<0.025*CCstop)) ||
             (conicalBeamforming &&((meanconeAngle/mean(coneAngles))<0.025*CCstop))) ) {
	  running = False;
	};
	niteration++;
	if (niteration > 100) {
	  running = False;
	};
      }; // end of the big loop
      // calculate mean position
      meanaz = meanel = meandist = meanconeAngle = 0.;
      for (i=0; i<4; i++){
	meanaz += azs(i); meanel += els(i); meandist += dists(i); meanconeAngle += coneAngles(i);
      };
      meanaz /= 4.; meanel /= 4.; meandist /= 4.; meanconeAngle /= 4.;
      Bool clipping=True;
      while (clipping){
	if (meanaz>360.) { meanaz-=360.; continue;};
	if (meanaz<0.) { meanaz+=360.; continue;};
	if (meanel>90.) { meanel=180.-meanel; continue;};
	if (meanel<0.) { meanel=-meanel; continue;};
	if (meandist<0.) { meandist=-meandist; continue;};
	clipping = False;
      };
      Az = meanaz; El = meanel; distance = meandist; center = mean(cents); coneAngle = meanconeAngle;

    } catch (AipsError x) {
      cerr << "analyseLOPESevent:SimplexFit: " << x.getMesg() << endl;
      return False;
    }; 
    return True;
  };




  
  Bool analyseLOPESevent::evaluateGrid (Double &Az,
					Double &El,
					Double &distance,
					Vector<Bool> AntennaSelection,
					Double *centerp,
                                        Double coneAngle){
    try {
      int estep=0,astep=0,arange=0,erange=3;
      Double maxaz=Az,maxel=El,maxheight=0.,maxcenter=-1.8e-6;
      Double az_=0.,el_=0.,height_=0.,center_=0.;
      Double stepfactor=1.;

      if (beamPipe_p == NULL){
	cerr << "analyseLOPESevent:evaluateGrid: " << "Error: beamPipe_p == NULL " << endl;
	return False;	
      };
      if (beamformDR_p == NULL){
	cerr << "analyseLOPESevent:evaluateGrid: " << "Error: beamformDR_p == NULL " << endl;
	return False;	
      };

      beamPipe_p->setVerbosity(False);

      for (estep=-erange; estep<=erange; estep++){
	el_ = El + stepfactor*estep;
	arange = erange-abs(estep);
	for (astep=-arange; astep<=arange; astep++){
	  az_ = Az+astep*stepfactor/(cos(el_*DEG2RAD)+1e-6);
	  if (el_>90.){ az_+= 180.; };
	  center_ = 2e99;
	  // use x-beam for evaluateGrid
	  if ( getBeamHeight(az_, el_, distance, AntennaSelection, &height_, SkymapQuantity::TIME_X, &center_, coneAngle)) {
	    if (height_ > maxheight){
	      maxaz = az_; maxel = el_; maxheight=height_; maxcenter=center_;
	    };
	  };
	};
      };
      Bool clipping=True;
      while (clipping){
	if (maxaz>360.) { maxaz-=360.; continue;};
	if (maxaz<0.) { maxaz+=360.; continue;};
	if (maxel>90.) { maxel=180.-maxel; continue;};
	if (maxel<0.) { maxel=-maxel; continue;};
	clipping = False;
      };
      Az = maxaz; El = maxel;
      if (centerp != NULL) {
	*centerp = maxcenter;
      };
    } catch (AipsError x) {
      cerr << "analyseLOPESevent:evaluateGrid: " << x.getMesg() << endl;
      return False;
    }; 
    return True;
  };
  
  
  Double analyseLOPESevent::getHeight (Double az,
				       Double el,
				       Double dist, 
				       Vector<Bool> AntennaSelection,
				       Double *centerp,
                                       Double coneAngle){
    Double erg;
    try {
      Vector<Double> ccb,xb,pb;
      Matrix<Double> ts;
      Record ergrec;
      Bool clipping=True, goodfit=True;
      while (clipping){
	if (az>360.) { az-=360.; continue;};
	if (az<0.) { az+=360.; continue;};
	if (el>90.) { el=180.-el; continue;};
	if (el<0.) { el=-el; continue;};
	if (dist<0.) { dist=-dist; continue;};
	clipping = False;
      };
      beamPipe_p->setDirection(az, el, dist);
      beamPipe_p->setConeAngle(coneAngle);
      beamPipe_p->GetTCXP(beamformDR_p, ts, ccb, xb, pb, AntennaSelection, Polarization_p);
      StatisticsFilter<Double> mf(filterStrength_p,FilterType::MEAN);
      ccb = mf.filter(ccb);
      xb = mf.filter(xb);
      if (centerp != NULL) {
	ergrec = fitObject.Fitgauss(xb, ccb, False, *centerp);
      } else {
	ergrec = fitObject.Fitgauss(xb, ccb, False);
      };
      erg = ergrec.asDouble("CCheight")*1e6;
      if (centerp != NULL) {
	*centerp = ergrec.asDouble("CCcenter");
      };
      if (!ergrec.asBool("CCconverged") || 
	  (ergrec.asDouble("CCcenter") < fitStart_p) || (ergrec.asDouble("CCcenter") > fitStop_p) ||
	  (ergrec.asDouble("CCwidth")>1e-7) || (ergrec.asDouble("CCwidth")<1e-9) ||
	  (ergrec.asDouble("CCcenter_error")>(fitStop_p-fitStart_p))||
	  (ergrec.asDouble("CCwidth_error")>1e-7)||
	  ((ergrec.asDouble("CCheight_error")/ergrec.asDouble("CCheight"))>10) ) { 
	goodfit=False;
	beamformDR_p->timeValues(pb);
	erg = mean(ccb(Slice(ntrue(pb<-1.83e-6),5)))*1e6;
	if (centerp != NULL) {
	  *centerp = -1.8e-6;
	};
      };
#ifdef DEBUGGING_MESSAGES      
      cout << "getHeight: Az:"<<az<<" El:"<<el<<" Dist:"<<dist<<" Height:"<< erg << " conv:"<<
	ergrec.asBool("Xconverged")<<ergrec.asBool("CCconverged")<<goodfit<< " Center:"<< ergrec.asDouble("CCcenter")<<endl;
#endif
    } catch (AipsError x) {
      cerr << "analyseLOPESevent:getHeight: " << x.getMesg() << endl;
      return 0.;
    }; 
    return erg;
  };






  Bool analyseLOPESevent::getBeamHeight (Double az,
                                         Double el,
                                         Double dist,
                                         Vector<Bool> AntennaSelection,
				         Double *beamheightp,
					 SkymapQuantity::Type beamtype,
                                         Double *centerp,
                                         Double coneAngle){
    Bool status = True;
    try {
      Vector<Double> ccb,xb,pb;
      Matrix<Double> ts;
      Record ergrec;
      Bool clipping=True, goodfit=True;
      while (clipping){
        if (az>360.) { az-=360.; continue;};
        if (az<0.) { az+=360.; continue;};
        if (el>90.) { el=180.-el; continue;};
        if (el<0.) { el=-el; continue;};
        if (dist<0.) { dist=-dist; continue;};
        clipping = False;
      };
      beamPipe_p->setDirection(az, el, dist);
      beamPipe_p->setConeAngle(coneAngle);
      beamPipe_p->GetTCXP(beamformDR_p, ts, ccb, xb, pb, AntennaSelection, Polarization_p);

      // Apply mean filters to cc- and x-beam
      StatisticsFilter<Double> mf(filterStrength_p,FilterType::MEAN);
      xb = mf.filter(xb);
      if (beamtype==SkymapQuantity::TIME_CC) {
	ccb = mf.filter(ccb);
      } else {
	ccb.resize(0);
      };

      // Defining record for fit results
      if (centerp != NULL) {
        ergrec = fitObject.Fitgauss(xb, ccb, False, *centerp);
      } else {
        ergrec = fitObject.Fitgauss(xb, ccb, False);
      };

      // if desired beamtype is xbeam
      if ( beamtype==SkymapQuantity::TIME_X) {
	if (!ergrec.asBool("Xconverged") ||
           (ergrec.asDouble("Xcenter") < fitStart_p) || (ergrec.asDouble("Xcenter") > fitStop_p) ||
           (ergrec.asDouble("Xwidth")>1e-7) || (ergrec.asDouble("Xwidth")<1e-9) ||
           (ergrec.asDouble("Xcenter_error")>(fitStop_p-fitStart_p))||
           (ergrec.asDouble("Xwidth_error")>1e-7)||
           ((ergrec.asDouble("Xheight_error")/ergrec.asDouble("Xheight"))>10) ) {
 	  beamformDR_p->timeValues(pb);
          // return mean of xb in timewindow of 62.5 ns starting at 1.83e-6
          *beamheightp=  mean(xb(Slice(ntrue(pb<-1.83e-6),int(beamformDR_p->sampleFrequency()/GetDataReader()->sampleFrequency())*5)))*1e6;
	  goodfit=False;
          if (centerp != NULL) {
            *centerp = -1.8e-6;
          };
	}
	else {
	  *beamheightp = ergrec.asDouble("Xheight")*1e6;
          if (centerp != NULL) {
            *centerp = ergrec.asDouble("Xcenter");
	  };
	};

      }
      else if ( beamtype==SkymapQuantity::TIME_CC) {
	// if beamtype is "CC" or any other unkown string
        if (!ergrec.asBool("CCconverged") ||
            (ergrec.asDouble("CCcenter") < fitStart_p) || (ergrec.asDouble("CCcenter") > fitStop_p) ||
            (ergrec.asDouble("CCwidth")>1e-7) || (ergrec.asDouble("CCwidth")<1e-9) ||
            (ergrec.asDouble("CCcenter_error")>(fitStop_p-fitStart_p))||
            (ergrec.asDouble("CCwidth_error")>1e-7)||
            ((ergrec.asDouble("CCheight_error")/ergrec.asDouble("CCheight"))>10) ) {
          beamformDR_p->timeValues(pb);
	  // return mean of ccb in timewindow of 62.5 ns around 1.83e-6
          *beamheightp=  mean(ccb(Slice(ntrue(pb<-1.83e-6),int(beamformDR_p->sampleFrequency()/GetDataReader()->sampleFrequency())*5)))*1e6;
          goodfit=False;
          if (centerp != NULL) {
            *centerp = -1.8e-6;
          };
        }
        else {
	  *beamheightp = ergrec.asDouble("CCheight")*1e6;
          if (centerp != NULL) {
            *centerp = ergrec.asDouble("CCcenter");
          };
	};
      } else {
	cerr << "getBeamHeight: Unknown beamtype, only SkymapQuantity::TIME_CC"
	     << " and SkymapQuantity::TIME_X supported!" << endl;
      };
      status = goodfit;

#ifdef DEBUGGING_MESSAGES      
          cout << "getBeamHeight: Beamtype: "<<beamtype<<" Az:"<<az<<" El:"<<el<<" Dist:"<<dist<<" Height:"<< beamheightp << " conv:"
	       << ergrec.asBool("Xconverged")<<ergrec.asBool("CCconverged")<<goodfit<< " Center:"<< ergrec.asDouble("CCcenter")<<endl;
#endif

    } catch (AipsError x) {
      cerr << "analyseLOPESevent:getBeamHeight: " << x.getMesg() << endl;
      return False;
    };
    return status;
  };





  Bool analyseLOPESevent::findDistance( Double &Az,
		      			Double &El,
				        Double &distance,
				        Vector<Bool> AntennaSelection, 
				        Double *centerp,
					Bool rough,
					Bool verbose){
    try {
      Double height_=0., maxheight=0., maxdist=2500., center_=-1.8e-6, maxcenter=0.;
      for(int d=2000; d<=15000; ){
	if (getBeamHeight(Az, El, d, AntennaSelection, &height_, SkymapQuantity::TIME_X, &center_)) {
	  if (height_ > maxheight) {
  	    maxheight = height_;
  	    maxdist = d;
	    maxcenter = center_;
	  };
        };

        if (rough) d+=1000;
        else d+=200;
      };
      distance = maxdist;
      if (centerp != NULL) {
        *centerp = maxcenter;
      };
      if (verbose) { cout << "findDistance: best Distance (xbeam): "<<distance<<endl; };
    } catch (AipsError x) {
	cerr << "analyseLOPESevent:findDistance: "<< x.getMesg() <<endl;
	return false;
    };
    return true;
  };
  

  Bool analyseLOPESevent::findConeAngle( Double &Az,
                                         Double &El,
                                         Double &coneAngle,
                                         Vector<Bool> AntennaSelection, 
                                         Double *centerp,
                                         Bool rough,
                                         Bool verbose){
    try {
      Double height_=0., maxheight=0., maxconeAngle=0., center_=-1.8e-6, maxcenter=0.;
      Double dummyDistance = 1e5; // distance has no effect for conical beamforming
      for(double rho=0; rho<=0.1; ){
        if (getBeamHeight(Az, El, dummyDistance, AntennaSelection, &height_, SkymapQuantity::TIME_X, &center_, rho)) {
          if (height_ > maxheight) {
            maxheight = height_;
            maxconeAngle = rho;
            maxcenter = center_;
          }
        }

        if (rough) rho+=0.005;
        else rho+=0.0005;
      }
      coneAngle = maxconeAngle;
      if (centerp != NULL) {
        *centerp = maxcenter;
      }
      if (verbose)
        cout << "findConeAngle: best coneAngle rho (xbeam): "<<coneAngle<<endl;
    } catch (AipsError x) {
        cerr << "analyseLOPESevent:findConeAngle: "<< x.getMesg() <<endl;
        return false;
    };
    return true;
  };
  

} // Namespace CR -- end
