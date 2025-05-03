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
 
void generateDummyStart2CalRunPar () {
	
	
	const Bool_t skipMod1 = false;
	
	sayHello();
	
	//TFile* inFileMod0 = new TFile ("StartCalib_feb24_raw_039_ALLDAY_ITER0c.root");
	//TFile* inFileMod1 = new TFile ("StartCalib_feb24_raw_039_ALLDAY_ITER1d.root");
	//TString pathForNewAsciiFile = "Start2CalRunPar_JanOrlinski_paramTest_21022024.txt";
	//TString pathForNewAsciiFile = "Start2CalRunPar_JanOrlinski_lineFit_mod0_mod1_21022024.txt";
	//TString pathForNewAsciiFile = "Start2CalRunPar_JanOrlinski_lineFit_mod0_21022024.txt";
	TString pathForCurrentAsciiFile = "Start2CalRunPar_Dummy.txt";
	TString pathForNewAsciiFile = "Start2CalRunPar_Dummy.txt";
	vector<Int_t> listOfRunIDs;
    const Int_t runID = 509101543; // random runID from cosmics for fwall
    //Int_t firstrunId = firstRunID; // first run of day 039 in feb24
    listOfRunIDs.push_back(runID);
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
    
    cout << " >>> STATUS: Hello world! Let's generate the start2calrunpar container!" << endl;
    gSystem->Sleep(1000);
    cout << " >>> STATUS: Lubie placki" << endl;
    gSystem->Sleep(1000);
    
    Int_t nOfRuns = listOfRunIDs.size();
    cout << "In total, " << nOfRuns << " runs will be analyzed." << endl;

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
        
        for (int j = 0; j < 2; j++){
            for (int k = 0; k < 96; k++){
				//cout << "blip" << endl;
				//runID = listOfRunIDs[i];
				//cout << "j = " << j << ", k = " << k << ", runID = " << runID << endl;
				
            	Double_t offset = 0.;
            	Double_t slope = 0.;
				
	            tmpArrayOfParameters[j*96 + k] = offset; //offsets
	            tmpArrayOfParameters[192 + (j*96) + k] = slope; //walk slopes
				
            }
        }
         
		for (Int_t j=0;j<2;j++){
			for (Int_t k=0; k<96; k++){	

				myStart2CalRunParsForOneID.setVal( (j*96) + k,  tmpArrayOfParameters[ (j*96) + k] );
				myStart2CalRunParsForOneID.setVal( 192 + (j*96) + k, tmpArrayOfParameters[192 + (j*96) + k] ) ;
				
			}
		}
        
        //runID = listOfRunIDs[i]; //this shouldnt be necessary but somehow it breaks the system...
        
        cout << " >>> STATUS: Adding run " << runID << " to the container. " << i+1 << " / "<< nOfRuns << " done..." << endl;
        //cout << " >>> STATUS: Adding run " << runID << " to the container..." << endl;
        myStart2CalRunParContainer.addRun(runID, &myStart2CalRunParsForOneID, kFALSE);
        gSystem->Sleep(20);
    }
        
	//myStart2CalRunParContainer.printParams();
     
	HParAsciiFileIo* output=new HParAsciiFileIo;
	output->open(pathForNewAsciiFile, "out");
	//     rtdb->setOutput(output);
	myStart2CalRunParContainer.finishRunList();
	myStart2CalRunParContainer.write(output);
	output->close();
 
    delete myHades;
}





