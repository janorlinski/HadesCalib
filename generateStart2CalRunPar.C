#include "hades.h"
#include "hruntimedb.h"
#include "hruninfo.h"
#include "hcategory.h"
#include "hdst.h"
#include "hparasciifileio.h"
#include "hstartparasciifileio.h"
#include "htime.h"

// containers to read

// start
#include "hstart2calpar.h"
#include "hstart2detector.h"
#include "hstart2trb2lookup.h"
#include "hstart2calrunpar.h"
#include "hstart2geompar.h"
#include "hstart2trb3lookup.h"
#include "hstart2trb3calpar.h"
#include "hstartparasciifileio.h"
// tof
#include "htofwalkpar.h"
#include "htofdigipar.h"
// rpc
#include "hrpccalpar.h"
#include "hrpccellstatuspar.h"
#include "hrpcchargeoffsetpar.h"
#include "hrpcslewingpar.h"
#include "hrpctimepospar.h"
// rich
#include "hrich700thresholdpar.h"
#include "hrich700digipar.h"
#include "hrich700ringfinderpar.h"
// sts
#include "hstscalpar.h"
#include "hstscalibraterpar.h"
#include "hstsdigipar.h"
#include "hstsstatuspar.h"
// frpc
#include "hfrpccalpar.h"
#include "hfrpcdigipar.h"
#include "hfrpchitfinderpar.h"
#include "hfrpcstrippar.h"
// forwardtrack
#include "hforwardcandfinderpar.h"
// emc
#include "hemccalpar.h"
#include "hemccalibraterpar.h"
#include "hemccellstatuspar.h"
#include "hemcdigipar.h"
#include "hemcsimulpar.h"

// iTOF
#include "hitofdef.h"
#include "hitofdetector.h"
#include "hitofcalpar.h"



#include "hparora2io.h"
#include "hora2info.h"

// root
#include "TSystem.h"
#include "TROOT.h"
#include "TString.h"
#include "TList.h"
#include "TFile.h"
//#include "TIter.h"

// system
#include <iostream>
#include <vector>

#include "myClasses.h"
#include "myIncludes.h"

using namespace std;
 
void generateStart2CalRunPar () {
	
	
	const Bool_t skipMod1 = false;
	const Bool_t addDummy = true;
	
	sayHello();
	
	TFile* inFileMod0 = new TFile ("StartCalib_feb24_raw_039_ALLDAY_ITER0c.root");
	TFile* inFileMod1 = new TFile ("StartCalib_feb24_raw_039_ALLDAY_ITER1d.root");
	//TString pathForNewAsciiFile = "Start2CalRunPar_JanOrlinski_paramTest_21022024.txt";
	//TString pathForNewAsciiFile = "Start2CalRunPar_JanOrlinski_lineFit_mod0_mod1_21022024.txt";
	//TString pathForNewAsciiFile = "Start2CalRunPar_JanOrlinski_lineFit_mod0_21022024.txt";
	TString pathForNewAsciiFile = "Start2CalRunPar_JanOrlinski_28022024.txt";
	vector<Int_t> listOfRunIDs;
    const Int_t firstRunID = 507346805; // all be files
    const Int_t lastRunID = 507460334; // all be files
    const Int_t dummyRunID = 509101543; // all be files
    //const Int_t firstRunID = 507346805; // good 039 data
    //const Int_t lastRunID = 507432952; // good 039 data
    Int_t firstrunId = firstRunID; // first run of day 039 in feb24
    //   TString asciiParFile = "./feb22_dst_params_default.txt";
    
    //TString asciiParFile = "Start2CalRunPar_JanOrlinski_15022024.txt";
    TString asciiParFile = "";
    TString rootParFile = "./analysisDST/allParam_Feb22_gen2_27022023.root";
    TString paramSource = "root"; // root, ascii, oracle

    Hades* myHades      = new Hades;
   
    HSpectrometer* spec = gHades->getSetup();
    HRuntimeDb* rtdb    = gHades->getRuntimeDb();

    Int_t mdcMods[6][4] =
        { {1,1,1,1},
        {1,1,1,1},
        {1,1,1,1},
        {1,1,1,1},
        {1,1,1,1},
        {1,1,1,1} };

	HDst::setupSpectrometer("feb24", mdcMods, "rich,mdc,tof,rpc,emc,wall,start,sts,frpc,itof");
	// beamtime mdcMods_apr12, mdcMods_full
	// Int_t mdcset[6][4] setup mdc. If not used put NULL (default).
	// if not NULL it will overwrite settings given by beamtime
	// detectors (default)= rich,mdc,tof,rpc,shower,wall,tbox,start

	HDst::setupParameterSources(paramSource, asciiParFile, rootParFile, "now");
	//HDst::setupParameterSources("oracle",asciiParFile,rootParFile,"now"); // use to create param file
	// parsource = oracle,ascii,root (order matters)
	// if source is "ascii" a ascii param file has to provided
	// if source is "root" a root param file has to provided
	// The histDate paramter (default "now") is used wit the oracle source

	// this stage will obtain a TList of "HRunInfo" classes
    HParOra2Io* oracleIO = new HParOra2Io;
    oracleIO->open();
    rtdb->setSecondInput(oracleIO);
    HOra2Info* oracleInfo = oracleIO -> getOra2Info();
    TList* listOfRuns = oracleInfo -> getListOfRuns("feb24");
    
    cout << " >>> STATUS: Hello world! Let's generate the start2calrunpar container!" << endl;
    gSystem->Sleep(500);
    
    TIter next (listOfRuns);
    Bool_t startCollectingRuns = false;
    
    while( HRunInfo* runInfo = (HRunInfo*) next() ) {
		
		Int_t run = int(runInfo->getRunId());
		
		
		if (run == firstRunID) {
			startCollectingRuns = true;
			cout << "FIRST RUN will be runID = " << runInfo->getRunId() << ", times " << runInfo->getStartTime() << " - " << runInfo->getEndTime() << endl;
			gSystem->Sleep(500);
		}
		
		//startCollectingRuns=true;
		
		if (startCollectingRuns) {
			listOfRunIDs.push_back(run);
			cout << " >>> STATUS: Adding run " << run << " to the analysis." << " Times " << runInfo->getStartTime() << " - " << runInfo->getEndTime() << endl;
			gSystem->Sleep(5);
		}
		
		if (run == lastRunID) {
			cout << "LAST RUN will be runID = " << runInfo->getRunId() << ", times " << runInfo->getStartTime() << " - " << runInfo->getEndTime() << endl;
			break;
			gSystem->Sleep(500);
		}
		
	}
    
    if (addDummy) listOfRunIDs.push_back(dummyRunID);
    
    Int_t nOfRuns = listOfRunIDs.size();
    cout << "In total, " << nOfRuns << " (incl. dummy) runs will be analyzed ()." << endl;

	HStart2CalRunPar myStart2CalRunParContainer  ("Start2CalRunPar", "Start2CalRunPar", "test");    
	
     for (int i = 0; i < nOfRuns; i++) {
		 
		
		HStart2CalRunPars myStart2CalRunParsForOneID (384);
        Int_t runID = listOfRunIDs[i];
        
        //cout << " >>> STATUS: Analyzing run " << runID << " ..." << endl;
        //myStart2CalRunParContainer.getRun(runID, &myStart2CalRunParsForOneID, kFALSE);
        
        //Float_t earlytest = myStart2CalRunParsForOneID.getVal(0);
        //cout << "earlytest = " << earlytest << endl;
        
        //myStart2CalRunParsForOneID.clear();
       // cout << "size of myStart2CalRunParsForOneID = " << myStart2CalRunParsForOneID->getNVals() << endl;
       
        Float_t tmpArrayOfParameters [384];
        const int osmeSmake = 1;
        for (int torbaBorba = 0; torbaBorba < 384; torbaBorba+=osmeSmake) tmpArrayOfParameters[torbaBorba] = 0;
        
        
        //cout << " >>> STATUS: Looking for offsets and slopes in the input file..." << endl;
        
		if (listOfRunIDs[i] == dummyRunID) cout << " >>> STATUS: setting all 0's for the dummy runID = " << dummyRunID << " !" << endl;
        
        for (int j = 0; j < 2; j++){
            for (int k = 0; k < 96; k++){
				//cout << "blip" << endl;
				//runID = listOfRunIDs[i];
				//cout << "j = " << j << ", k = " << k << ", runID = " << runID << endl;
				
            	Double_t offset = 0.;
            	Double_t slope = 0.;
				
	            tmpArrayOfParameters[j*96 + k] = offset; //offsets
	            tmpArrayOfParameters[192 + (j*96) + k] = slope; //walk slopes
				
				//cout << "Sstarting channel " << j << "/" << k << " with offset = " << offset << ", slope = " << slope << endl;
				/*
				if (j==0 && k==7) {
					
					offset = 21.37;
					slope = 0.;
		            tmpArrayOfParameters[j*96+k] = (-1.0) * offset; //offsets
		            tmpArrayOfParameters[192 + (j*96)+k] = (-1.0) * slope; //walk slopes
		            
				}
				*/
				
				
				
				//continue;
				
				TGraphErrors* graphMod0;
				TGraphErrors* graphMod1;
				TF1* walkFitMod0;
				TF1* walkFitMod1;
				
				if (listOfRunIDs[i] == dummyRunID) continue;
					//gSystem->Sleep(1000);
					//cout << " >>> STATUS: setting all 0's for the dummy runID = " << dummyRunID << " !" << endl;

				if (skipMod1 && j>0) continue; 
				if (k>15) continue;
				
				Int_t index = j*20 + k + 1;
				if (skipThisChannel(index)) continue;
				
				//cout << " >>> STATUS: Sourcing offsets and slopes for channel " << index << "..." << endl;
            
				//TGraphErrors* graph = (TGraphErrors*) inFile->Get(Form("TW sqrt graphs/TWgraph_sqrt_ch%i", index));
				graphMod0 = (TGraphErrors*) inFileMod0->Get(Form("TW line graphs/TWgraph_line_ch%i", index));
				if (!skipMod1) graphMod1 = (TGraphErrors*) inFileMod1->Get(Form("TW line graphs/TWgraph_line_ch%i", index));
				//TF1* sqrtFit = graph->GetFunction(Form("timeWalkSqrtFit_%i", index));
				walkFitMod0 = graphMod0->GetFunction(Form("timeWalkLineFit_%i", index));
				if (!skipMod1) walkFitMod1 = graphMod1->GetFunction(Form("timeWalkLineFit_%i", index));
            	
            	if (walkFitMod0 && j==0) {
					//cout << "getting mod0 params j = " << j << endl;
					offset = walkFitMod0->GetParameter(0);
					slope =  walkFitMod0->GetParameter(1);
				}
				
				else if (walkFitMod1 && j==1) {
					//cout << "getting mod1 params j = " << j << endl;
					offset = walkFitMod1->GetParameter(0);
					slope =  walkFitMod1->GetParameter(1);
				}
            	
	            tmpArrayOfParameters[j*96+k] = (-1.0) * offset; //offsets
	            tmpArrayOfParameters[192 + (j*96)+k] = (-1.0) * slope; //walk slopes
	            
				//cout << "Found values for channel " << j << "/" << k << ". offset = " << offset << ", slope = " << slope << endl;
				//gSystem->Sleep(5.0);
            }
        }
        
        //cout << " >>> STATUS: Done loading offsets and slopes from the source file!" << endl;
        
        
        //runID = listOfRunIDs[i]; //this shouldnt be necessary but somehow it breaks the system...
        
       // for (Int_t j=0; j<384; j++) {
			
			//cout << "j = " << j << ", temp param = " << tmpArrayOfParameters[j] << endl; 
         
	 //}
         
		for (Int_t j=0;j<2;j++){
			for (Int_t k=0; k<96; k++){
				
				//Double_t test1 = (Float_t) floor(  tmpArrayOfParameters[ (j*96) + k]*10000.) / 10000.;
				//Double_t test2 = (Float_t) floor(  tmpArrayOfParameters[192+ (j*96) + k]*10000.)/10000.;
				
				//cout << " j = " << j << ", k = " << k <<", test1 = " << test1 << endl;
				//cout << " j = " << j << ", k = " << k <<", test2 = " << test2 << endl;
			
			    //Float_t latetest = myStart2CalRunParsForOneID.getVal(0);
				//cout << "latetest = " << latetest << endl;
			
				//cout << "In channel " << j << "/" << k << " offset = " << myStart2CalRunParsForOneID.getVal(j*96 + k) << ", slope = " << myStart2CalRunParsForOneID.getVal(j*96 + k + 192) << endl;
			
				//myStart2CalRunParsForOneID.setVal(1, 540.0);
				//myStart2CalRunParsForOneID.setVal(1, 0.0  ) ;
				
				//myStart2CalRunParsForOneID.setVal( (j*96) + k, (Float_t) ( floor(  tmpArrayOfParameters[ (j*96) + k]*10000.) / 10000.) );
				//myStart2CalRunParsForOneID.setVal( 192 + (j*96) + k, (Float_t)  (floor( tmpArrayOfParameters[192+ (j*96) + k]*10000.)/10000.    )  ) ;
				
				myStart2CalRunParsForOneID.setVal( (j*96) + k,  tmpArrayOfParameters[ (j*96) + k] );
				myStart2CalRunParsForOneID.setVal( 192 + (j*96) + k, tmpArrayOfParameters[192 + (j*96) + k] ) ;
				
				
			}
		}
        
        //runID = listOfRunIDs[i]; //this shouldnt be necessary but somehow it breaks the system...
        
        cout << " >>> STATUS: Adding run " << runID << " to the container. " << i+1 << " / "<< nOfRuns << " done..." << endl;
        //cout << " >>> STATUS: Adding run " << runID << " to the container..." << endl;
        myStart2CalRunParContainer.addRun(runID, &myStart2CalRunParsForOneID, kFALSE);
        gSystem->Sleep(12);
    }
        
    
	//myStart2CalRunParContainer.printParams();
     
	HParAsciiFileIo* output=new HParAsciiFileIo;
	output->open(pathForNewAsciiFile, "out");
	//     rtdb->setOutput(output);
	myStart2CalRunParContainer.finishRunList();
	myStart2CalRunParContainer.write(output);
	output->close();
   
     
   /*  
    fstream myOutputFile;
    myOutputFile.open("testCalRunPar_root5_test.txt",ios::out);
   HStartParAsciiFileIo myIo(&myOutputFile);
  
//   HParRootFile myRootFile("myTestRootFile.root","RECREATE","myTestRootFile.root");
//   HStartParRootFileIo myIo(&myRootFile);
  
  
//   myIo.write(&myStart2CalRunParContainer);
    myStart2CalRunParContainer.write(myOutputFile); */
 
    delete myHades;
}





