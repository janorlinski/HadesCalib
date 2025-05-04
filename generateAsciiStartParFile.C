#include "HadesCalibIncludes.h"


void generateAsciiStartParFile () {
	
	const TString inRootFile = "./params/allParam_feb24_gen1_22042025.root";
	const TString inAsciiFile = "Start2CalPar_JanOrlinski_03052025.txt";
		
	const TString newAsciiFile = "Start2CalPar_JanOrlinski_04052025.txt";
	
	const TString startOffsetsFile = "";

	Int_t nModulesForOffsets = 4;
	Double_t globalStartOffset = +0.16;
	Double_t globalVetoOffset = 0.0;

	//Int_t runId = 546276917; // random runid from apr25
	Int_t runId = 509556277; // last runid from feb24
	//Int_t runId = 507316668; // copied from analysisDST
	TString asciiParFile = inAsciiFile;
	TString rootParFile  = inRootFile;
	TString paramSource  = "ascii";

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

	HDst::setupSpectrometer("apr25", mdcMods, "rich,mdc,tof,rpc,emc,sts,frpc,start,tbox");
	HDst::setupParameterSources(paramSource, asciiParFile, rootParFile, "now");

	HParAsciiFileIo* output = new HParAsciiFileIo;
	output->open(newAsciiFile.Data(), "out");
	
	rtdb->setOutput(output);
	
	HStart2Calpar* pCalPar = (HStart2Calpar*) rtdb->getContainer("Start2Calpar");
	rtdb->initContainers(runId);
	
	/* TDIFF CALIBRATION */
	
	//TFile* fin = new TFile (startOffsetsFile, "read");	   
	//TH1F* hTDiffOffsets  = (TH1F*) fin->Get("hTDiffOffsets");
	
	//Float_t moduleDependentOffset = 
	
	for (Int_t m=0; m<nModulesForOffsets; m++) { // loop over modules
		for (Int_t c=0; c<nChPerModReal; c++) { // loop over channels
			
			if (m == 2) continue; //module 2 not defined
			if (m == 3 && c==8) continue; //module 3 only defined up to c+1=8
			
			Float_t oldTdcOffset   = (*pCalPar)[m][c+1].getTdcOffset();
			
			Float_t adjustment = 0.0;
			
			if (m==0 && c+1==2) adjustment = +1.15-0.16;
			if (m==0 && c+1==3) adjustment = -5.81-0.16;
			if (m==0 && c+1==4) adjustment = +0.47-0.16;
			if (m==0 && c+1==6) adjustment = +0.81-0.16;
			if (m==0 && c+1==13) adjustment = +0.28-0.16;
			if (m==0 && c+1==14) adjustment = +0.46-0.16;
			if (m==1 && c+1==1) adjustment = +3.6-0.16;
			if (m==1 && c+1==2) adjustment = +1.27-0.16;
			if (m==1 && c+1==16) adjustment = +6.21-0.16;
			
			if (adjustment > 1000 || adjustment < -1000) adjustment = 0.0;
			
			if (m==0 || m==1) adjustment+=globalStartOffset;
			else if (m==3) adjustment+=globalVetoOffset;
			
			cout << "m" << m << "c" << c+1 << " adjusting oldVal="<< oldTdcOffset<<" with adj=" << adjustment << endl;
			
			Float_t newTdcOffset  = oldTdcOffset + adjustment;
			(*pCalPar)[m][c+1].setTdcOffset(newTdcOffset);
			
		}
	}
	cout << "blip" << endl;
	//fin->Close();
	//output->close();
	delete gHades;
}
