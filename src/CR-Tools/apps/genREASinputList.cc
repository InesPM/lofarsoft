/*-------------------------------------------------------------------------*
 | $Id::                                                                 $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) 2010                                                    *
 *   Frank Schröder, Nunzia Palmieri                                       *
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
 
#include<cstdlib>
#include<iostream>
#include<fstream>
#include<string>
#include<sstream>
#include<time.h>

#include <crtools.h>
#include <Analysis/PulseProperties.h>

#include<TFile.h>
#include<TTree.h>
#include<TMath.h>
#include<TCut.h>
#include <Rtypes.h>

/*!
  \file genREASinputList.cc

  \ingroup CR_Applications

  \brief Creates ASCII list from results of call_pipeline, for input to REAS

  \author Frank Schröder, Nunzia Palmieri

  \date 2010/Apr/06

  <h3>Usage</h3>

  \verbatim
  ./genREASinputList results.root options.cfg "optional selection string"
  \endverbatim

  <h3>Prerequisites</h3>

  options_sim.cfg

  Example:
  \verbatim
  # Name of the different files that should be created ( .txt and .root )
  namebase		REASinputList
  
  # Write all cuts (e.g. time + CC beam cuts into this config file)
  TCut    ((goodReconstructed_EW)&&(EfieldMaxAbs<3000)&&((Xheight_EW/CCheight_EW)>0.8)&&((Xheight_EW/CCheight_EW)<1.5)&&(CCheight_EW/rmsCCbeam_EW>14*sqrt(NCCbeamAntennas_EW/30)))
  \endverbatim
*/

using namespace std;
using namespace TMath;

class PulseProperties;

int main(int argc, char* argv[])
{
  try {
    if(argc!=4&&argc!=3) {
      cout << "\n Application to generate an ASCII list for REAS simulations, based on the ROOT file generated by call_pipeline.\n"
           << " Syntax: ./genREASinputList results.root options_sim.cfg [\"optional selection string\"] "
           << endl;
      cout<<"Wrong number of arguments!"<<endl;
      return 1;
    } else {
      cout << "\nStarting genREASinputList.\n" << endl;
     }
    

  // Processing the config-file to set standart-cuts and give a minimum and maximum geomagnetic angle 
  // and energy **************************************************************************************

  ifstream conf(argv[2]);
  string buf;
  stringstream opt;
  char tmp[1024];


  // variables to read in
  string cut_str(""), namebase="REASinputList";
  Bool_t createInfoFile=false;
  char KRETAver[1024];
  Int_t Nant_lopes30_min=10,Nant_lopesPol_min=5;
  // default cuts for CC beam
  //double CCcutA = 9.5, CCcutG = 7;

  TCut cut;

  while (conf.getline(tmp,1024)) {
    opt.clear();
    opt.str(tmp);
    opt>>buf;
    if(buf.length()==0||buf.at(0)=='#') 
      continue;
    if(buf.compare("Nant_lopes30_min")==0) 
      opt >> Nant_lopes30_min;
    if(buf.compare("Nant_lopesPol_min")==0) 
      opt >> Nant_lopesPol_min;
    if(buf.compare("TCut")==0)
      opt>>cut_str;
    //if(buf.compare("CCcutA")==0) 
    //  opt >> CCcutA;
    //if(buf.compare("CCcutG")==0) 
    //  opt >> CCcutG;

    if(buf.compare("useEWpol")==0) 
      cut = TCut("(Xheight_EW/CCheight_EW)>=-0.6&&(Xheight_EW/CCheight_EW)<=1.4");
    if(buf.compare("useNSpol")==0) 
      cut = TCut("(Xheight_NS/CCheight_NS)>=-0.6&&(Xheight_NS/CCheight_NS)<=1.4");
    if(buf.compare("useBOTHpol")==0) 
      cut = TCut("((Xheight_EW/CCheight_EW)>=-0.6&&(Xheight_EW/CCheight_EW)<=1.4)&&((Xheight_NS/CCheight_NS)>=-0.6&&(Xheight_NS/CCheight_NS)<=1.4)");

    if(buf.compare("createInfoFile")==0) 
      opt>>createInfoFile;
    if(buf.compare("namebase")==0) 
      opt>>namebase;
    buf="";
  }
  // add cut_string to cut
  cut += cut_str.c_str();


  if(argc==4)
    cut += argv[3];

  opt.clear();
  opt.str("");


  // Prepare ROOT-Files ******************************************************************************
  cout<<"Applying cut:"<<endl;
  cout<<"             "<<cut<<endl;

  TFile *f = new TFile( argv[1], "READ");
  TTree *t1 =  (TTree*)f->Get("T;1");

  TTree *t2 = t1->CopyTree(cut);

  UInt_t Gt = 0;
  Float_t Age=0,Ageg=0,Az=0,Ze=0,Xc=0,Yc=0,Xcg=0,Ycg=0,Azg=0,Zeg=0,Nmu=0,Lmuo=0,Size=0,Sizeg=0,Sizmg=0;
  //char Eventname[64];
  Int_t CutSNR_EW=0,CutSNR_NS=0;
  Int_t NlateralAntennas_EW=0,NlateralAntennas_NS=0,NCCbeamAntennas_EW=0,NCCbeamAntennas_NS=0;
  Double_t lgE=0, lgEg=0, lnA=0, lnAg=0, err_lgE=0, err_lgEg=0, err_lnA=0, err_lnAg=0;
  Double_t err_core=0, err_coreg=0, err_Az=0, err_Azg=0, err_Ze=0, err_Zeg=0;
  Double_t geomag_angle=0, geomag_angleg=0;
  Double_t chi2NDF_EW=0,chi2NDF_NS=0,eps_EW=0,eps_NS=0;
  Double_t Xheight_EW=0,Xheight_NS=0,Xheight_error_EW=0,Xheight_error_NS=0;
  Double_t CCheight_EW=0,CCheight_NS=0,CCheight_error_EW=0,CCheight_error_NS=0,R_0_EW=0,R_0_NS=0;
  Double_t rmsCCbeam_EW=0,rmsCCbeam_NS=0,sigR_0_EW=0,sigR_0_NS=0,sigeps_EW=0,sigeps_NS=0;
  Char_t reconstruction=0;

  t2->SetBranchAddress("Age",&Age);
  t2->SetBranchAddress("Ageg",&Ageg);
  t2->SetBranchAddress("Az",&Az);
  t2->SetBranchAddress("Azg",&Azg);
  //t2->SetBranchAddress("AzL_EW",&AzL_EW);
  //t2->SetBranchAddress("AzL_NS",&AzL_NS);
  //t2->SetBranchAddress("CCcenter_EW", &CCcenter_EW);
  //t2->SetBranchAddress("CCcenter_NS", &CCcenter_NS);
  //t2->SetBranchAddress("CCconverged_EW", &CCconverged_EW);
 // t2->SetBranchAddress("CCconverged_NS", &CCconverged_NS);
  t2->SetBranchAddress("CCheight_EW", &CCheight_EW);
  t2->SetBranchAddress("CCheight_NS", &CCheight_NS);
  t2->SetBranchAddress("CCheight_error_EW", &CCheight_error_EW);
  t2->SetBranchAddress("CCheight_error_NS", &CCheight_error_NS);
  //t2->SetBranchAddress("CCwidth_EW", &CCwidth_EW);
  //t2->SetBranchAddress("CCwidth_NS", &CCwidth_NS);
  //t2->SetBranchAddress("CutBadTiming_EW", &CutBadTiming_EW);
  //t2->SetBranchAddress("CutBadTiming_NS", &CutBadTiming_NS);
  //t2->SetBranchAddress("CutCloseToCore_EW", &CutCloseToCore_EW);
  //t2->SetBranchAddress("CutCloseToCore_NS", &CutCloseToCore_NS);
  t2->SetBranchAddress("CutSNR_EW", &CutSNR_EW);
  t2->SetBranchAddress("CutSNR_NS", &CutSNR_NS);
  //t2->SetBranchAddress("CutSmallSignal_EW", &CutSmallSignal_EW);
  //t2->SetBranchAddress("CutSmallSignal_NS", &CutSmallSignal_NS);
  //t2->SetBranchAddress("Distance_EW", &Distance_EW);
  //t2->SetBranchAddress("Distance_NS", &Distance_NS);
  //t2->SetBranchAddress("ElL_EW", &ElL_EW);
  //t2->SetBranchAddress("ElL_NS", &ElL_NS);
  //t2->SetBranchAddress("Eventname", &Eventname);
  t2->SetBranchAddress("Gt",&Gt);
  t2->SetBranchAddress("KRETAver",&KRETAver);
  t2->SetBranchAddress("Lmuo",&Lmuo);
  t2->SetBranchAddress("NCCbeamAntennas_EW", &NCCbeamAntennas_EW);
  t2->SetBranchAddress("NCCbeamAntennas_NS", &NCCbeamAntennas_NS);
  t2->SetBranchAddress("NlateralAntennas_EW", &NlateralAntennas_EW);
  t2->SetBranchAddress("NlateralAntennas_NS", &NlateralAntennas_NS);
  t2->SetBranchAddress("Nmu",&Nmu);
  t2->SetBranchAddress("R_0_EW", &R_0_EW);
  t2->SetBranchAddress("R_0_NS", &R_0_NS);
  t2->SetBranchAddress("Size",&Size);
  t2->SetBranchAddress("Sizeg",&Sizeg);
  t2->SetBranchAddress("Sizmg",&Sizmg);
  t2->SetBranchAddress("Xc",&Xc);
  t2->SetBranchAddress("Xcg",&Xcg);
  //t2->SetBranchAddress("Xconverged_EW",&Xconverged_EW);
  //t2->SetBranchAddress("Xconverged_NS",&Xconverged_NS);
  t2->SetBranchAddress("Xheight_EW", &Xheight_EW);
  t2->SetBranchAddress("Xheight_NS", &Xheight_NS);
  t2->SetBranchAddress("Xheight_error_EW", &Xheight_error_EW);
  t2->SetBranchAddress("Xheight_error_NS", &Xheight_error_NS);
  t2->SetBranchAddress("Yc",&Yc);
  t2->SetBranchAddress("Ycg",&Ycg);
  t2->SetBranchAddress("Ze",&Ze);
  t2->SetBranchAddress("Zeg",&Zeg);
  t2->SetBranchAddress("chi2NDF_EW", &chi2NDF_EW);
  t2->SetBranchAddress("chi2NDF_NS", &chi2NDF_NS);
  t2->SetBranchAddress("eps_EW", &eps_EW);
  t2->SetBranchAddress("eps_NS", &eps_NS);
  t2->SetBranchAddress("err_Az",&err_Az);
  t2->SetBranchAddress("err_Azg",&err_Azg);
  t2->SetBranchAddress("err_Ze",&err_Ze);
  t2->SetBranchAddress("err_Zeg",&err_Zeg);
  t2->SetBranchAddress("err_core",&err_core);
  t2->SetBranchAddress("err_coreg",&err_coreg);
  t2->SetBranchAddress("err_lgE",&err_lgE);
  t2->SetBranchAddress("err_lgEg",&err_lgEg);
  t2->SetBranchAddress("err_lnA",&err_lnA);
  t2->SetBranchAddress("err_lnAg",&err_lnAg);
  t2->SetBranchAddress("geomag_angle",&geomag_angle);
  t2->SetBranchAddress("geomag_angleg",&geomag_angleg);
  //t2->SetBranchAddress("goodReconstructed_EW", &goodReconstructed_EW);
  //t2->SetBranchAddress("goodReconstructed_NS", &goodReconstructed_NS);
  //t2->SetBranchAddress("latMeanDistCC_EW", &latMeanDistCC_EW);
  //t2->SetBranchAddress("latMeanDistCC_NS", &latMeanDistCC_NS);
  //t2->SetBranchAddress("latMeanDist_EW", &latMeanDist_EW);
  //t2->SetBranchAddress("latMeanDist_NS", &latMeanDist_NS);
  t2->SetBranchAddress("lgE",&lgE);
  t2->SetBranchAddress("lgEg",&lgEg);
  t2->SetBranchAddress("lnA",&lnA);
  t2->SetBranchAddress("lnAg",&lnAg);
  t2->SetBranchAddress("reconstruction",&reconstruction);
  t2->SetBranchAddress("rmsCCbeam_EW", &rmsCCbeam_EW);
  t2->SetBranchAddress("rmsCCbeam_NS", &rmsCCbeam_NS);
  //t2->SetBranchAddress("rmsPbeam_EW", &rmsPbeam_EW);
  //t2->SetBranchAddress("rmsPbeam_NS", &rmsPbeam_NS);
  //t2->SetBranchAddress("rmsXbeam_EW", &rmsXbeam_EW);
  //t2->SetBranchAddress("rmsXbeam_NS", &rmsXbeam_NS);
  t2->SetBranchAddress("sigR_0_EW", &sigR_0_EW);
  t2->SetBranchAddress("sigR_0_NS", &sigR_0_NS);
  t2->SetBranchAddress("sigeps_EW", &sigeps_EW);
  t2->SetBranchAddress("sigeps_NS", &sigeps_NS);



  int nentries = t2->GetEntries();
  cout<<"Number of events that passed the cut conditions: "<<nentries<<endl<<endl;

  cout <<"\nScanning for events..."<<endl
       <<"TCut = "<<cut<<endl
     //  <<"Nant_lopes30_min required for lat. distr. = " <<Nant_lopes30_min <<endl
      // <<"Nant_lopesPol_min required for lat. distr.= " <<Nant_lopesPol_min <<endl
       <<"LOPES-GT \tSource \tKRETA \taz(deg)\t errAz  \tze(deg)\terrZe  \tcoreN(m) \tcoreE(m)\terrCor \tNe_in \tNmu_in \tNmu^tr \tE_in(eV) \terrlgE_in(GeV) \tAge_in\n"
       <<"======================================================================================================================================================================================================================================================================\n";

   int count=0;
//   string gtstring;
//   bool found=0;
//   fstream fin;

  TFile *fout = new TFile( (namebase+".root").c_str(),"recreate");
  TTree *k = new TTree("k","KASCADE-Information for eventlist");
  //recreated same root file after the cuts

  k->Branch("Gt",&Gt,"Gt/i");
  k->Branch("KRETAver",&KRETAver,"KRETAver/C");

  // Additional eventlist info file *******************************************************************
  ofstream f1;

  f1.open( (namebase+".txt").c_str(), ios::out);

  // Write eventlist header, including the selection conditions ***************************************
  f1    <<"Automatic generated eventlist for the following conditions:"<<endl
        <<"TCut = "<<cut<<endl
       // <<"Nant_lopes30_min required for lat. distr. = " <<Nant_lopes30_min <<endl
       // <<"Nant_lopesPol_min required for lat. distr.= " <<Nant_lopesPol_min <<endl
        <<"LOPES-GT \tSource \tKRETA \taz(deg)\t errAz  \tze(deg)\terrZe  \tcoreN(m) \tcoreE(m)\terrCor \tNe_in \tNmu_in \tNmu^tr \tE(eV) \terrlgE_in(GeV) \tAge_in\n"
        <<"======================================================================================================================================================================================================================================================================\n";

  //some variables declaration
  float Xlopes,Ylopes,Xlopesg,Ylopesg;
  double alpha=15.25*(TMath::Pi()/180.); //in grad anlge between the 2 ref.system
  double E,Eg=0.;

  for (int i=0; i<nentries; i++) {
    t2->GetEntry(i);

    //calculate LOPES coordinate system
    Xlopes=Xc*TMath::Cos(alpha)+Yc*TMath::Sin(alpha);     //East
    //cout<<"Xlopes    "<<Xlopes<<endl;
    Ylopes=-Xc*TMath::Sin(alpha)+Yc*TMath::Cos(alpha);    //North
    Xlopesg=Xcg*TMath::Cos(alpha)+Ycg*TMath::Sin(alpha);    //East
    Ylopesg=-Xcg*TMath::Sin(alpha)+Ycg*TMath::Cos(alpha); //North



    //energy (eV)
    E=pow(10,lgE);
    Eg=pow(10,lgEg); 
    Az=Az*(180./TMath::Pi());
    Ze=Ze*(180./TMath::Pi());
    err_Az=err_Az*(180./TMath::Pi());
    err_Ze=err_Ze*(180./TMath::Pi());
    Azg=Azg*(180./TMath::Pi());
    Zeg=Zeg*(180./TMath::Pi());
    err_Azg=err_Azg*(180./TMath::Pi());
    err_Zeg=err_Zeg*(180./TMath::Pi());

    if( reconstruction=='A' ) {// KASCADE reconstruction!!!!
      f1<<Gt<<"\t"<<"A"<<"\t"<<KRETAver<<"\t"<<Az<<"\t"<<err_Az<<"\t"<<Ze<<"\t"<<err_Ze<<"\t"<<Ylopes<<"\t"<<Xlopes<<"\t"<<err_core<<"\t"<<Size<<"\t"<<Nmu<<"\t"<<Lmuo<<"\t"<<E*pow(10,9)<<"\t"<<err_lgE<<"\t"<<Age<<endl;
      cout<<Gt<<"\t"<<"A"<<"\t"<<KRETAver<<"\t"<<Az<<"\t"<<err_Az<<"\t"<<Ze<<"\t"<<err_Ze<<"\t"<<Ylopes<<"\t"<<Xlopes<<"\t"<<err_core<<"\t"<<Size<<"\t"<<Nmu<<"\t"<<Lmuo<<"\t"<<E*pow(10,9)<<"\t"<<err_lgE<<"\t"<<Age<<endl;
      ++count;
      k->Fill();
    } else if(reconstruction=='G') {
      f1<<Gt<<"\t"<<"G"<<"\t"<<KRETAver<<"\t"<<Azg<<"\t"<<err_Azg<<"\t"<<Zeg<<"\t"<<err_Zeg<<"\t"<<Ylopesg<<"\t"<<Xlopesg<<"\t"<<err_coreg<<"\t"<<Sizeg<<"\t"<<Sizmg<<"\t"<<0.<<"\t"<<Eg*pow(10,9)<<"\t"<<err_lgEg<<"\t"<<Ageg<<endl;
      cout<<Gt<<"\t"<<"G"<<"\t"<<KRETAver<<"\t"<<Azg<<"\t"<<err_Azg<<"\t"<<Zeg<<"\t"<<err_Zeg<<"\t"<<Ylopesg<<"\t"<<Xlopesg<<"\t"<<err_coreg<<"\t"<<Sizeg<<"\t"<<Sizmg<<"\t"<<0.<<"\t"<<Eg*pow(10,9)<<"\t"<<err_lgEg<<"\t"<<Ageg<<endl;
      ++count;
      k->Fill();
    } else {
      cerr << "\n\nERROR: Event reconstruction is neither Array (KASCADE) nor Grande!\n" << endl;
    }
  }//loop on events

  k->Write();
  fout->Close();
  f1.close();

  cout<<count<<" events written in REAS input list:"<<endl;
  cout<<namebase<<".txt written"<<endl;
  cout<<namebase<<".root written"<<endl;

  } catch (...) {
    cerr << "\nError in genREASinputList!" << endl;
  }
  
  return 0;
}






