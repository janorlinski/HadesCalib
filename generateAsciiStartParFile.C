#include "HadesCalibIncludes.h"


void generateAsciiStartParFile () {
	
	const TString inRootFile = "./params/allParam_feb24_gen1_22042025.root";
	const TString inAsciiFile = "./params/apr25_dst_params.txt";
		
	const TString newAsciiFile = "Start2CalPar_JanOrlinski_03052025.txt";
	
	const TString startOffsetsFile = "";

	Int_t nModulesForOffsets = 4;
	Double_t globalStartOffset = 0.0;
	Double_t globalVetoOffset = -0.19;

	Int_t runId = 546276917; // random runid from apr25
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
			
			if (m==0 && c+1==4) adjustment = -0.78;
			else if (m==0 && c+1==5) adjustment = -0.04;
			else if (m==0 && c+1==6) adjustment = -0.89;
			else if (m==0 && c+1==7) adjustment = +0.07;
			else if (m==0 && c+1==8) adjustment = -0.20;
			else if (m==0 && c+1==9) adjustment = +0.19;
			else if (m==0 && c+1==10) adjustment = -0.19;
			else if (m==0 && c+1==11) adjustment = +0.11;
			else if (m==0 && c+1==12) adjustment = +0.07;
			else if (m==0 && c+1==14) adjustment = -0.93;
			
			else if (m==1 && c+1==3) adjustment = +0.20;
			else if (m==1 && c+1==4) adjustment = +0.09;
			else if (m==1 && c+1==5) adjustment = +0.08;
			else if (m==1 && c+1==6) adjustment = -0.07;
			else if (m==1 && c+1==7) adjustment = +0.08;
			else if (m==1 && c+1==8) adjustment = -0.02;
			else if (m==1 && c+1==9) adjustment = +0.08;
			else if (m==1 && c+1==10) adjustment = 0.00;
			else if (m==1 && c+1==11) adjustment = +0.11;
			else if (m==1 && c+1==12) adjustment = +1.67;
			else if (m==1 && c+1==13) adjustment = +0.18;
			else if (m==1 && c+1==14) adjustment = +4.08;
			
			else if (m==3 && c+1==1) adjustment = +0.18;
			else if (m==3 && c+1==2) adjustment = +0.18;
			else if (m==3 && c+1==3) adjustment = +0.12;
			else if (m==3 && c+1==4) adjustment = +0.18;
			else if (m==3 && c+1==6) adjustment = +0.02;
			else if (m==3 && c+1==7) adjustment = -0.04;
			else if (m==3 && c+1==8) adjustment = -0.57;
			
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
	output->close();
	delete gHades;
}
