#include "myClasses.h"
#include "myIncludes.h"

const TString inRootFile = "analysisDST/allParam_Feb22_gen2_27022023.root";
const TString inAsciiFile = "analysisDST/feb24_dst_params_ofMod0.txt";
	
const TString newAsciiFile = "Start2CalPar_mod0_mod1_JanOrlinski_06032024.txt";

const TString startOffsetsFile = "StartCalib_feb24_raw_061_ALLDAY_ITER1_runB.root";

void generateAsciiStartParFile () {
	
	Int_t nModulesForOffsets = 2;
	
	Double_t globalStartOffset = 0.404;

	Int_t runId = 444571079; // copied from analysisDST
	//Int_t runId = 507316668; // copied from analysisDST
	TString asciiParFile = inAsciiFile;
	TString rootParFile  = inRootFile;
	TString paramSource  = "ascii,root";

	Hades* myHades       = new Hades;
	HSpectrometer* spec  = gHades->getSetup();
	HRuntimeDb* rtdb     = gHades->getRuntimeDb();

	Int_t mdcMods[6][4]=
	  { {1,1,1,1},
	  {1,1,1,1},
	  {1,1,1,1},
	  {1,1,1,1},
	  {1,1,1,1},
	  {1,1,1,1} };

	Int_t nLayers[6][4] = {
	  {6,6,5,6},
	  {6,6,5,6},
	  {6,6,5,6},
	  {6,6,5,6},
	  {6,6,5,6},
	  {6,6,5,6} };

	HDst::setupSpectrometer("feb24", mdcMods, "rich,mdc,tof,rpc,emc,sts,frpc,start,tbox");
	HDst::setupParameterSources(paramSource, asciiParFile, rootParFile, "now");

	HParAsciiFileIo* output = new HParAsciiFileIo;
	output->open(newAsciiFile.Data(), "out");
	
	rtdb->setOutput(output);
	
	HStart2Calpar* pCalPar = (HStart2Calpar*) rtdb->getContainer("Start2Calpar");
	rtdb->initContainers(runId);
	
	/* TDIFF CALIBRATION */
	
	TFile* fin = new TFile (startOffsetsFile, "read");	   
	TH1F* hTDiffOffsets  = (TH1F*) fin->Get("hTDiffOffsets");
	
	//Float_t moduleDependentOffset = 
	
	for (Int_t m=0; m<nModulesForOffsets; m++) { // loop over modules
		for (Int_t c=0; c<nChPerModReal; c++) { // loop over channels
			
			//if (m > 0) continue; //only calibrate module 0
			
			Float_t oldTdcOffset   = (*pCalPar)[m][c+1].getTdcOffset();
			
			Float_t adjustment = hTDiffOffsets->GetBinContent(m*nChPerMod + c + 1);
			if (adjustment > 1000 || adjustment < -1000) adjustment = 0.0;
			//if (m > 0) adjustment = 0.0; // only calibrate module 0 - comment for second step
			if (m == 0) adjustment = 0.0; // only calibrate module 1 - comment for first step
			
			adjustment+=globalStartOffset;
			
			cout << "m" << m << "c" << c+1 << " adjusting oldVal="<< oldTdcOffset<<" with adj=" << adjustment << endl;
			
			Float_t newTdcOffset  = oldTdcOffset + adjustment;
			(*pCalPar)[m][c+1].setTdcOffset(newTdcOffset);
			
		}
	}
	
	fin->Close();
	delete gHades;
}
