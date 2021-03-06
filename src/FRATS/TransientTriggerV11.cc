 /*-------------------------------------------------------------------------*
 | $Id:: TransientTriggerV7.cc                                           $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) 2009                                                    *
 *   Sander ter Veen (s.terveen@astro.ru.nl)                                              *
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

//#include <crtools.h>
//#include <Data/LOFAR_TBB.h>
//#include <Display/SimplePlot.h>
#include <iostream>
#include <fstream>
#include <cstdlib>
//#include <cmath>
#include <sstream>
#include <sys/stat.h>
#include "FRATcoincidence.h"
#include "FRATcoincidence.cc"
#include <stdio.h>
#include <math.h>

using namespace std;
using namespace FRAT::coincidence;
using namespace FRAT::analysis;
/*
 \file ttbbctlIn.cc
 
 \ingroup CR_Applications
 
 \brief A program to trigger on dispersed pulses found in LOFAR pulsar pipeline subband data. (MS files). This version handles multiple subbands as one.
 
 \author Sander ter Veen
 
 \date 2009/09/03
 */

#define CHANNELS        32
#define SAMPLES         2768
//#define STRIDE          (64/CHANNELS)^2

// 256; 768 for the old files or 3056 ipv 3072
/*
 \brief Structure to read the incoherent beam files 
 */
struct stokesdata {
	/* big endian (ppc) */
	unsigned int  sequence_number;
	//char pad[508];	
	/* little endian (intel) */
	float         samples[CHANNELS][SAMPLES|2];
};

//_______________________________________________________________________________
//                                                                      show_data



//_______________________________________________________________________________
//                                                                    swap_endian
/*
 void swap_endian( char *x )
 {
 char c;
 
 c = x[0];
 x[0] = x[3];
 x[3] = c;
 
 
 c = x[1];
 x[1] = x[2];
 x[2] = c;
 }
 */
/*
 float FloatSwap( float f )
 {
 union
 {
 float f;
 unsigned char b[4];
 } dat1, dat2;
 
 dat1.f = f;
 dat2.b[0] = dat1.b[3];
 dat2.b[1] = dat1.b[2];
 dat2.b[2] = dat1.b[1];
 dat2.b[3] = dat1.b[0];
 return dat2.f;
 }
 */

bool FileExists(string strFilename) { 
	struct stat stFileInfo; 
	bool blnReturn; 
	int intStat; 
	
	// Attempt to get the file attributes 
	intStat = stat(strFilename.c_str(),&stFileInfo); 
	if(intStat == 0) { 
		// We were able to get the file attributes 
		// so the file obviously exists. 
		blnReturn = true; 
	} else { 
		// We were not able to get the file attributes. 
		// This may mean that we don't have permission to 
		// access the folder which contains this file. If you 
		// need to do that level of checking, lookup the 
		// return values of stat which will give you 
		// more details on why stat failed. 
		blnReturn = false; 
	} 
	
	return(blnReturn); 
}

//_______________________________________________________________________________
//                                                                         main()

int main (int argc,
		  char *argv[])
{
	bool failsafe=0;
	cout<<"This file outputs histograms of pulsar data to /Users/STV/Documents/GiantPulse/."<<endl;
	int integrationlength = 1; //average length
	float triggerlevel = 141/3;
 	unsigned int nofFailedTests=0;
	float average;
	std::string inputdir, configfilename;
	vector<std::string> inputdirs;
	int ninputdirs;
	bool multipledirs=false;
	vector<int> inputstartSBs;
	int firstSB, lastSB, nrblocks=5, startpos=0;
	int CoinNr=10;
	int CoinTime=80;
	float DM=56.8;
	int nDMs=1;
    int nFreqs=0;
    vector<float> DMvalues;
    vector<float> FREQvalues;
	//	std::string infile="/Users/STV/Astro/data/pulsars/L2009_13298/SB0.MS", outfile="/Users/STV/Documents/GiantPulse/";
	std::string logfilename = "/Users/STV/Documents/GiantPulse/excesslog_pdt.txt";
	std::string triggerlogfilename = "/Users/STV/Astro/Programming/pulsar/trigger.log";
	int channels=CHANNELS;
	int samples=SAMPLES;
	int startsubbandnumber=321;
	bool DoPadding=0, verbose=false;
	std::string extension=".stokes";
	int timeintegration=1;
	int nrCombinedSBs = 1;
	int HBAmode = 1;
	string pulsedir = "pulses";
    int sleeptime = 0;
	
	/*
	 Check if filename of the dataset is provided on the command line; if only
	 a fraction of the possible tests can be carried out.
	 */
	
	
	for(int argcounter(1); argcounter < argc; argcounter++){
	    std::string topic = string(argv[argcounter]); 
		if(topic == "help" || argc < 2){
			cout << "Options:\n"
			"-c <config file>\n"
			"-i <inputdir>\n"
			"-FSB <first subband>\n"
			"-LSB <last subband>\n"
			"-il <integration length\n"
			"-tl <triggerlevel>\n"
			"-log <logfile>\n"
			"-tlog <triggerlogfile>\n"
			"-n <nrblocks>\n"
			"-Cnr <coincidence nr subbands\n"
			"-Ctime <coincidence time windows\n"
			"-Tnr <number of stored triggers\n"
			"-ch <number of channels, TO BE ADDED>\n"
			"-sa <number of samples, TO BE ADDED>\n"
			"-verbose, display histograms\n"
			"-ext <file extenstion after .MS\n"
			"- pad use padding for newer data files\n"
			"-startSB real subband to set the startfrequency right\n"
			"-DM dispersion measure\n"
			"-nrCSB number of subbands to combine as one"
			"-tInt <time integration>"
			"-LBA LBA mode"
            "-sleep <sleeptime (sec)>"
			"Examples:\n"
			"./TransientTrigger -i ~/Astro/data/pulsars/obs/ -FSB 0 -LSB 10 -il 20 -tl 50 -n 51 \n"
			"./TransientTrigger -i ~/Astro/data/pulsars/L2009_13298/ -FSB 0 -LSB 10 -il 20 -tl 50 -n 3595 \n; open /Users/STV/Documents/GiantPulse/excesslog_pdt.txt\n"
			"./TransientTrigger -i ~/Astro/data/pulsars/obs/ -FSB 0 -LSB 10 -il 20 -tl 50 -n 51 -log TransientTrigger.log -tlog trigger.log\n";
			"./TransientTrigger -c TransientTrigger.conf\n";
			return 1;
		} else if(topic == "-c"){
			argcounter++;
			configfilename = string(argv[argcounter]);
			cout << "configfile: " << configfilename << endl;
		} else if(topic == "-i"){
			argcounter++;
			inputdir = string(argv[argcounter]);
			cout << "input directory: " << inputdir << endl;
		} else if(topic == "-indirs"){
			multipledirs=true;
			argcounter++;
			ninputdirs = atoi(argv[argcounter]);
			inputdirs.resize(ninputdirs,"");
			inputstartSBs.resize(ninputdirs,0);
			for(int i=0;i<ninputdirs;i++){
				argcounter++;
				inputdirs[i] = string(argv[argcounter]);
				argcounter++;
				inputstartSBs[i]=atoi(argv[argcounter]);
			}
			for(int i=0;i<ninputdirs;i++){
				cout << "input directory: " << inputdirs[i] << " starting at " << inputstartSBs[i] << endl;
			}
		} else if(topic == "-FSB"){
			argcounter++;
			firstSB = atoi(argv[argcounter]);
			cout << "First subband: " << firstSB << endl;
		} else if(topic == "-LSB"){
			argcounter++;
			lastSB = atoi(argv[argcounter]);
		    cout << "Last subband: " << lastSB << endl;
		} else if(topic == "-il"){
			argcounter++;
			integrationlength = atoi(argv[argcounter]);
			cout << "integration length: " << integrationlength << endl;
		} else if(topic == "-tl"){
			argcounter++;
			triggerlevel = atof(argv[argcounter]);
			cout << "triggerlevel " << triggerlevel << endl;
		} else if(topic == "-log"){
			argcounter++;
			logfilename = string(argv[argcounter]);
			cout << "log file " << logfilename << endl;
		} else if(topic == "-pulsedir"){
			argcounter++;
			pulsedir = string(argv[argcounter]);
			cout << "pulse plots directory " << pulsedir << endl;
		} else if(topic == "-tlog"){
			argcounter++;
			triggerlogfilename = string(argv[argcounter]);
			cout << "trigger log file " << triggerlogfilename << endl;
		} else if(topic == "-n"){
			argcounter++;
			nrblocks = atoi(argv[argcounter]);
			cout << "nr blocks " << nrblocks << endl;
		} else if(topic == "-Cnr"){
			argcounter++;
			CoinNr = atoi(argv[argcounter]);
			cout << "Coincidence subbands required " << CoinNr << endl;	
		} else if(topic == "-sleep"){
			argcounter++;
            sleeptime=(int) 1000000*atof(argv[argcounter]);
			cout << "Sleep for " << sleeptime << "us" << endl; 
		} else if(topic == "-Ctime"){
			argcounter++;
			CoinTime = atoi(argv[argcounter]);
			cout << "Coincidence timewindow " << CoinTime << endl;
		} else if(topic == "-stb"){
			argcounter++;
			startpos = atol(argv[argcounter]);
			cout << "First block " << startpos << endl;	
		} else if(topic == "-ch"){
			argcounter++;
			channels = atoi(argv[argcounter]);
			cout << "nr of channels " << channels << endl;
		} else if(topic == "-sa"){
			argcounter++;
			samples = atoi(argv[argcounter]);
			cout << "nr of samples " << samples << endl;
		} else if(topic == "-LBA"){
			HBAmode = 0;
			cout << "using LBA data" << endl;
		} else if(topic == "-verbose"){
			verbose = true;
			cout << "using verbose output" << endl;
		} else if(topic == "-pad"){
			DoPadding = true;
			cout << "using padding output" << endl;
		} else if(topic == "-ext"){
			argcounter++;
			extension = argv[argcounter];
			cout << "file extensions: .MS" << extension << endl;
		} else if(topic == "-startSB"){
			argcounter++;
			startsubbandnumber = atoi(argv[argcounter]);
			cout << "startsubband number: " << startsubbandnumber << endl;
		} else if(topic == "-DM"){
			argcounter++;
			nDMs = atoi(argv[argcounter]);
			cout << "dispersion measure: " ;
			DMvalues.resize(nDMs);
			for(int i=0;i<nDMs;i++){
				argcounter++;
			    DMvalues[i]=atof(argv[argcounter]);
                cout << " " << DMvalues[i];
			}		
            cout << endl;	
		} else if(topic == "-freq"){
			argcounter++;
			nFreqs = atoi(argv[argcounter]);
			cout << "frequency values: " ;
			DMvalues.resize(nFreqs);
			for(int i=0;i<nFreqs;i++){
				argcounter++;
			    FREQvalues[i]=atof(argv[argcounter]);
                cout << " " << FREQvalues[i];
			}		
            cout << endl;	
		} else if(topic == "-tInt"){
			argcounter++;
			timeintegration = atoi(argv[argcounter]);
			cout << "timeintegration number: " << timeintegration << endl;
		} else if(topic == "-nrCSB"){
			argcounter++;
			nrCombinedSBs = atoi(argv[argcounter]);
			cout << "number of subbands to take together: " << nrCombinedSBs << endl;
		} else {
			cout << "specify at least an config file with -c <config file>";
		}
	}
	int samplesOr2 = samples|2;
	
	
	
	cout << logfilename << endl << triggerlogfilename << endl;
	
	//============================= NEW I/O code ======================================================
	
	
	
	//SubbandTrigger SBT1 SubbandTrigger(int NrChannels, int NrSamples, float DM, float TriggerLevel, float ReferenceFreq, float StartFreq, float FreqResolution, float TimeResolution, int startBlock, int IntegrationLength, bool InDoPadding, bool InUseSamplesOr2)
	//bool SubbandTrigger::processData(float* data, unsigned int sequenceNumber, FRAT::coincidence::CoinCheck cc, int CoinNr, int CoinTime){
	
	
	//
	//SubbandTrigger SBT1 SubbandTrigger(int NrChannels, int NrSamples, float DM, float TriggerLevel, float ReferenceFreq, float StartFreq, float FreqResolution, float TimeResolution, int startBlock, int IntegrationLength, bool InDoPadding, bool InUseSamplesOr2)
	
	float ReferenceFreq=SubbandToFreq(startsubbandnumber,HBAmode);
	float FreqResolution = CalcFreqResolution(channels);
	float TimeResolution = CalcTimeResolution(channels,timeintegration);
	int ninputfiles=lastSB-firstSB+1;
	int nstreams = ninputfiles/nrCombinedSBs;
	if(ninputfiles%nrCombinedSBs!=0){
		cout << "Not all subbands could be processed in blocks of " << nrCombinedSBs << endl;
		ninputfiles = nstreams*nrCombinedSBs;
		lastSB=firstSB+ninputfiles-1;
		cout << "Last subband: " << lastSB << endl;
		cout << "ninputfiles: " << ninputfiles << endl;
	}
    int NrChPerSB = channels;
	int NrChannels=nrCombinedSBs*channels;
	cout << "Nr of input files: " << ninputfiles << "; Nr of streams: " << nstreams << endl;
	
	
	// Initialize SBtriggers and coincidence mechanism
	
	float DMval=DM;
	SubbandTrigger* SBTs[nstreams][nDMs];
	CoinCheck cc[nDMs];
	
	
	
	int StreamCounter=0;
	for(int SB=firstSB;SB<=lastSB;SB+=nrCombinedSBs){
		for(int DMcounter=0;DMcounter<nDMs;DMcounter++){
			DMval=DMvalues[DMcounter];
			cout << "StreamNr " << StreamCounter << " DMcounter " << DMcounter << " DM " << DMval << endl;
			float SBFreq = SubbandToFreq(SB+startsubbandnumber,HBAmode);
	        SBTs[StreamCounter][DMcounter] = new SubbandTrigger(StreamCounter, NrChannels, samples,DMval,triggerlevel,ReferenceFreq, SBFreq, FreqResolution, TimeResolution, startpos, integrationlength, DoPadding, DoPadding);
			//SBTs[StreamCounter][DMcounter]->summary();
		}
		StreamCounter++;
	}
	
	
	// Initialize coincidence
	
	
	//--------------READ IN THE FILES----------------------------------	
	FILE ** pFile;
	pFile = new FILE*[ninputfiles];	
	
	std::string* infile;
	infile=new std::string[ninputfiles];
	
	
	
	std::string * shortinfile;
	shortinfile = new std::string[ninputfiles];
	if(multipledirs){
		int inputdircounter=0;
	    for(int i=firstSB; i<=lastSB; i++){
			while(inputdircounter!=(inputstartSBs.size()-1) && i>=inputstartSBs[inputdircounter+1]){
				inputdircounter++;
			}
			std::stringstream ss;
		    std::string number;
			ss << i;
			ss >> number;
		        if(i < 10){ 	infile[i-firstSB]=inputdirs[inputdircounter] + "SB00" + number + extension; 
                        } else if( i < 100) { infile[i-firstSB]=inputdirs[inputdircounter] + "SB0" + number + extension;
                        } else {
		          infile[i-firstSB]=inputdirs[inputdircounter] + "SB" + number + extension;
			}
                        cout << "using file " << infile[i-firstSB] << endl;
			shortinfile[i-firstSB] = "SB" + number;
		}
	} else {
		for(int i=firstSB; i<=lastSB; i++){
			std::stringstream ss;
			std::string number;
            
            //if i < 10:
            //    ss << '0'
            //if i < 100:
            //    ss << '0'
			ss << i;
			ss >> number;
			infile[i-firstSB]=inputdir + "SB" + number + ".MS" + extension;
			cout << "using file " << infile[i-firstSB] << endl;
			shortinfile[i-firstSB] = "SB" + number;
		}
	}
	
	for(int i=0;i<ninputfiles;i++){
		
		pFile[i] = fopen(infile[i].c_str(),"rb");
		while (pFile[i]==NULL) {
			cout << "File not there yet" << endl;
			usleep(500000);
			pFile[i] = fopen(infile[i].c_str(),"rb");
		} 		
	}
	
	ofstream pulselogfile;
	int pulsenr=1;
	bool foundpulse;
	ofstream datamonitor[nstreams][nDMs];
	stringstream datamonitorfilename;//[nstreams][nDMs];
	string datamonitorfilen;
	for(int sc=0;sc < nstreams; sc++){
		for(int DMcounter=0; DMcounter<nDMs; DMcounter++){
			stringstream datamonitorfilename;//[nstreams][nDMs];
			string datamonitorfilen;
			datamonitorfilename << pulsedir << "/monitor_DM_" << DMvalues[DMcounter] << "_s_" << sc <<".log";
			datamonitorfilename >> datamonitorfilen;
			datamonitor[sc][DMcounter].open(datamonitorfilen.c_str());
		}
	}
	string alltriggerlogfilename;
	alltriggerlogfilename=pulsedir+"/found_triggers.log";
	
	ofstream triggerlogfile;
	ofstream alltriggerlogfile;
	//logfile.open(logfilename.c_str());
	triggerlogfile.open(triggerlogfilename.c_str());
	alltriggerlogfile.open(alltriggerlogfilename.c_str(),ios::out | ios::app);
	
	unsigned int sequence_nr[ninputfiles];
	unsigned int prev_sequence_nr[ninputfiles];
	for(int i=0; i < ninputfiles; i++){
		prev_sequence_nr[i]=0;
	}
	char pad[508];
	unsigned num;
	fpos_t pos;
	int blockdatasize=channels*samplesOr2*sizeof(float);
	int stokesdatasize=blockdatasize+sizeof(unsigned int);
	if(DoPadding){stokesdatasize+=sizeof(pad);}
	float *data=(float*)malloc(nrCombinedSBs*blockdatasize);
	if (data==NULL)
	{
		cerr << "Memory could not be allocated\n";
		return 1;
	}
	
	cout << "stokesdatasize " << stokesdatasize << endl;
	
	for(int j=0; j<ninputfiles; j++){fseek(pFile[j], startpos*(stokesdatasize), SEEK_SET); }
	for(int blockNr = 0; blockNr< nrblocks; blockNr++){
		for(int sc=0; sc < nstreams; sc++){
			
			for(int fc=0; fc < nrCombinedSBs; fc++){
				//cout << "fc = " << fc << endl;
				int i=sc*nrCombinedSBs+fc; //file identifier
				fgetpos(pFile[i],&pos);
				
				num = fread( &(sequence_nr[i]), sizeof(sequence_nr[i]), 1, pFile[i]); //read sequence number
				if( !num) {
					//	cout << "Error, EOF reached.\n";
				}
				

				
				if(DoPadding){
					num = fread( &(pad),sizeof(pad), 1, pFile[i]); //read padding
					if( !num) {
						//		cout << "Error, EOF reached.\n";
					}
				}
				//cout << "Data read start " << fc*channels*samplesOr2 << endl;
				num = fread( &(data[fc*channels*samplesOr2]), blockdatasize, 1, pFile[i]); //read data
				if( !num) {
					//	cout << "Error, EOF reached.\n";
				}
				if( !num) {
					cout << "*";
					usleep(50000);
					fsetpos(pFile[i],&pos);
					fc--;
					
					//i--; //1 file k--
					continue;
				}
				else {		  
					swap_endian( (char*)&(sequence_nr[i]) );
					if(i==0){  
						cerr << "reading sequence number: " << sequence_nr[i] << endl;
					}		
					//	if (i%nrCombinedSBs==0){
					//		blockvalidsamples[fc]=0; blocksum[fc]=0;
					// 	}//  }
				}
				if(prev_sequence_nr[i]!=(sequence_nr[i]-1) && blockNr !=0)
				{
				    cout << "Data missing for block " << blockNr;
					if(failsafe){
						cout << "\n Failsafe is on, stopping the trigger ";
						cout << "numbers: " << prev_sequence_nr[i] << " " << sequence_nr[i];
						return 43;
					}
					cout << endl;
				}
				prev_sequence_nr[i]=sequence_nr[i];

			} // for files 
			for(int DMcounter=0; DMcounter<nDMs; DMcounter++){	//analyse data of one stream for all DMs
				
				cout << "Processing " << sc << " " << DMcounter << endl;
				foundpulse=SBTs[sc][DMcounter]->processData(data, sequence_nr[sc*nrCombinedSBs], &cc[DMcounter], CoinNr, CoinTime);

				datamonitor[sc][DMcounter] << SBTs[sc][DMcounter]->blockAnalysisSummary() << "\n";
				if(foundpulse){ 
					triggerlogfile << "Found pulse " << pulsenr << " " << SBTs[sc][DMcounter]->FoundTriggers();
                    alltriggerlogfile << "pulse " << pulsenr << " " <<SBTs[sc][DMcounter]->FoundTriggers();
                    alltriggerlogfile.close();
                    triggerlogfile.close();
					usleep(2000);
					triggerlogfile.open(triggerlogfilename.c_str());
			    		alltriggerlogfile.open(alltriggerlogfilename.c_str(),ios::out | ios::app);
					for(int fc2=0;fc2<nstreams;fc2++){
						
						stringstream pulselogfn;
						pulselogfn << pulsedir << "/pulse" << pulsenr << "_" << fc2 << ".log";
						string pulselogfilename;
						pulselogfn >> pulselogfilename;
						
						SBTs[fc2][DMcounter]->makeplotBuffer(pulselogfilename);

					}
					pulsenr++;
				}
			}
			

		}
        usleep(sleeptime);
	}
	return nofFailedTests;
}




