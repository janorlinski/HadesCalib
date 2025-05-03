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

//#include "myClasses.h"
//#include "myIncludes.h"

using namespace std;
 
void generateStart2CalRunParFromFile () {
	
	// this is a code to generate the Start2CalRunPar containter for selected runID's (preferably 1 per hour)
	// based on TF1 object provided in a ROOT file
	// please provide all Run
	
	TFile* inFile = new TFile ("parametersFromSzymon_21012025.root", "read");
	const Int_t nOfRunIds = 86;
	const TString RunIds[nOfRunIds]  = {"509243471","509247675","509251215","509254952",
										"509258469","509262019","509268413","509269262",
										"509272845","509276404","509282413","509283660",
										"509287305","509290896","509297819","509298067",
										"509301636","509305276","509308864","509316077",
										"509319695","509323621","509329464","509330436",
										"509334817","509337612","509341268","509344873",
										"509348468","509352064","509355637","509359219",
										"509362804","509366401","509370011","509373660",
										"509377309","509381010","509382754","509384842",
										"509388018","509391695","509395201","509398805",
										"509402467","509406077","509409662","509413343",
										"509419634","509421126","509424166","509427634",
										"509431207","509434815","509438486","509442031",
										"509445668","509449272","509452830","509456482",
										"509460002","509463622","509467225","509470819",
										"509475378","509478055","509481619","509485282",
										"509488837","509492726","509496064","509499639",
										"509503228","509507113","509510478","509514084",
										"509518267","509521202","509524814","509528418",
										"509538289","509539223","509542872","509546412",
										"509550064","509553617"};
									
	TString pathForNewAsciiFile = Form("Start2CalRunPar_runID_%s-%s_generated11022025.txt", RunIds[0].Data(), RunIds[nOfRunIds-1].Data());	
    TString asciiParFile = "";
    TString rootParFile = "./analysisDST/allParam_Feb22_gen2_27022023.root";
    TString paramSource = "oracle"; // root, ascii, oracle

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

	HDst::setupParameterSources(paramSource, asciiParFile, rootParFile, "FEB24_v0g");
	//HDst::setupParameterSources("oracle",asciiParFile,rootParFile,"now"); // use to create param file
	// parsource = oracle,ascii,root (order matters)
	// if source is "ascii" a ascii param file has to provided
	// if source is "root" a root param file has to provided
	// The histDate paramter (default "now") is used wit the oracle source

    cout << " >>> STATUS: Hello world! Let's generate the start2calrunpar container!" << endl;
    inFile->ls();
    gSystem->Sleep(5000);
  
    
	HStart2CalRunPar myStart2CalRunParContainer  ("Start2CalRunPar", "Start2CalRunPar", "test");    
    
    for (Int_t i=0; i<nOfRunIds; i++) {
	
		HStart2CalRunPars myStart2CalRunParsForOneID (384);
		
        TString runID = RunIds[i];
        inFile->cd(runID.Data());
		 
        Float_t tmpArrayOfParameters [384];
        const int osmeSmake = 1;
        for (int torbaBorba = 0; torbaBorba < 384; torbaBorba+=osmeSmake) tmpArrayOfParameters[torbaBorba] = 0;
        
        for (int j = 0; j < 2; j++){
            for (int k = 0; k < 96; k++){
				
				cout << "Fetching parameter for j = " <<j<< " and k = " << k << endl;
				
            	Double_t offset = 0.;
            	Double_t slope = 0.;
				
	            tmpArrayOfParameters[j*96 + k] = offset; //offsets
	            tmpArrayOfParameters[192 + (j*96) + k] = slope; //walk slopes
				
				TF1* walkFit;
				Int_t index = j*20 + k;
				
				
				if (index>40) continue;
				
				cout << "Sourcing function for channel " << index << "..." << endl;
            
				walkFit = (TF1*) inFile->Get(Form("%s/ch_%i", runID.Data(), index));
				
				if (walkFit) cout << "Found function with name " << walkFit->GetName() << endl;
				else cout << "Function for this index was not found !!!" << endl;
				if (walkFit) {
	            	
					slope = walkFit->GetParameter(0);
					offset =  walkFit->GetParameter(1);
					
				}
					
	            tmpArrayOfParameters[j*96+k] = (-1.0) * offset; //offsets
	            tmpArrayOfParameters[192 + (j*96)+k] = (-1.0) * slope; //walk slopes
	            
				cout << "Found values for channel " << j << "/" << k << "(index "<<index<<"). offset = " << offset << ", slope = " << slope << endl;
				gSystem->Sleep(5.0);
            }
        }
        
		for (Int_t j=0;j<2;j++){
			for (Int_t k=0; k<96; k++){
				
				myStart2CalRunParsForOneID.setVal( (j*96) + k,  tmpArrayOfParameters[ (j*96) + k] );
				myStart2CalRunParsForOneID.setVal( 192 + (j*96) + k, tmpArrayOfParameters[192 + (j*96) + k] ) ;
				
			}
		}
        
        cout << " >>> STATUS: Adding run " << runID << " to the container. " << i+1 << " / "<< nOfRunIds << " done..." << endl;
        myStart2CalRunParContainer.addRun(runID.Atoi(), &myStart2CalRunParsForOneID, kFALSE);
        gSystem->Sleep(420);
    }
        
	//myStart2CalRunParContainer.printParams();
     
	HParAsciiFileIo* output=new HParAsciiFileIo;
	output->open(pathForNewAsciiFile, "out");
	myStart2CalRunParContainer.finishRunList();
	myStart2CalRunParContainer.write(output);
	output->close();
 
    delete myHades;
}





