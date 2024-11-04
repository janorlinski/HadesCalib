//#include "myIncludes.h"
#include "myClasses.h"

const TString inRootFile = "./params/allParam_feb24_gen0_16042024.root";
//const TString inAsciiFile = "./analysisDST/feb24_dst_params_lineFit.txt";
//const TString inAsciiFile = "./params/feb24_dst_params_ofMod0_ofMod1_gRpc.txt";
//const TString inAsciiFile = "RpcCalPar_au_added_26032024_cleaned.txt";
//const TString inAsciiFile = "RpcCalPar_cc_JanOrlinski_25062024.txt";
//const TString inAsciiFile = "RpcCalPar_cc_JanOrlinski_25062024.txt";
	
//const TString newAsciiFile = "RpcCalPar_auau_JanOrlinski_25062024.txt";

//const TString chargeOffsetsFile = "ChargeCalib_feb24_raw_061_ALLDAY_Control03.root";
//const TString timeAndPosOffsetsFile = "";

const Bool_t doCharge = false; 
const Bool_t doTimeAndPos = true; 

const Double_t vProp = 177.0; // propagation velocity for time/pos conversions

void generateAsciiParFile () {
	
	TString date="27082024";
	TString beam="cc";
	
	TString inAsciiFile = "";
	TString newAsciiFile = "";
	TString chargeOffsetsFile = "";
	TString timeAndPosOffsetsFile = "";
	
	if (beam=="cc") {
		inAsciiFile = "RpcCalPar_cc_JanOrlinski_25082024.txt";
		newAsciiFile = "RpcCalPar_cc_JanOrlinski_"+date+".txt";
		chargeOffsetsFile = "ChargeCalib_feb24_raw_039_ALLDAY_Control05.root";
		timeAndPosOffsetsFile = "TimeAndPos_039_Control08.root";
	}
	
	else if (beam=="auau") {
		inAsciiFile = "RpcCalPar_auau_JanOrlinski_25082024.txt";
		newAsciiFile = "RpcCalPar_auau_JanOrlinski_"+date+".txt";
		chargeOffsetsFile = "ChargeCalib_feb24_raw_061_ALLDAY_Control05.root";
		timeAndPosOffsetsFile = "TimeAndPos_061_Control08.root";
	}

	Int_t runId = -1; //random runid from feb24 logbook, file be061234303
	if (beam=="auau") runId = 509332983; //random runid from feb24 logbook, file be061234303
	else if (beam=="cc") runId = 507428198; //random runid from feb24 logbook, file be24039223638
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
	HDst::setupParameterSources(paramSource, asciiParFile, rootParFile, "FEB24_v0g");

	HParAsciiFileIo* output = new HParAsciiFileIo;
	output->open(newAsciiFile.Data(), "out");
	
	rtdb->setOutput(output);
	
	HRpcCalPar* pCalPar = (HRpcCalPar*) rtdb->getContainer("RpcCalPar");
	rtdb->initContainers(runId);
	
	cout << "Blip! Beam is " << beam << endl;
	gSystem->Sleep(2000);
	
	TFile* fin;
	
	/* CHARGE CALIBRATION */
	
	if (doCharge) fin = new TFile (chargeOffsetsFile, "read");
	
	for (Int_t s=0; s<6 && doCharge; s++) { // loop over sectors
		
		cout << " >>> STATUS: Fetching charge offsets for sector " << s+1 << endl;
		gSystem->Sleep(1000);
		TH1F* hLeftChargeOffsets  = (TH1F*) fin->Get(Form("hLeftChargeOffsets_sect%i", s+1));
		TH1F* hRightChargeOffsets = (TH1F*) fin->Get(Form("hRightChargeOffsets_sect%i", s+1));
		
		for (Int_t m=0; m<6; m++) { // loop over modules
			for (Int_t c=0; c<32; c++) { // loop over cells
				
				Float_t oldLeftChargeOffset   = (*pCalPar)[s][m][c].getLeftTOTOffset();
				Float_t oldRightChargeOffset  = (*pCalPar)[s][m][c].getRightTOTOffset();
				
				Float_t newLeftChargeOffset  = oldLeftChargeOffset  + hLeftChargeOffsets->GetBinContent(m*32 + c + 1);
				Float_t newRightChargeOffset = oldRightChargeOffset + hRightChargeOffsets->GetBinContent(m*32 + c + 1);
					     
				if (abs(newLeftChargeOffset) > 999.0) newLeftChargeOffset = -999.0;
				if (abs(newRightChargeOffset) > 999.0) newRightChargeOffset = -999.0;
					     
				(*pCalPar)[s][m][c].setLeftTOTOffset(newLeftChargeOffset);
				(*pCalPar)[s][m][c].setRightTOTOffset(newRightChargeOffset);
				
			}
		}
	}
	
	//fin->Close();
	
	/* TIME AND POS CALIBRATION */
	
	if (doTimeAndPos) fin = new TFile (timeAndPosOffsetsFile, "read");
	
	for (Int_t s=0; s<6 && doTimeAndPos; s++) { // loop over sectors
			   			   
		cout << " >>> STATUS: Fetching time&pos offsets for sector " << s+1 << endl;
		gSystem->Sleep(1000);
		TH1F* hXposOffsets  = (TH1F*) fin->Get(Form("hXposOffsets_sect%i", s+1));
		TH1F* hTimeOffsets  = (TH1F*) fin->Get(Form("hTimeOffsets_sect%i", s+1));
		
		for (Int_t m=0; m<6; m++) { // loop over modules
			for (Int_t c=0; c<32; c++) { // loop over cells
				
				Float_t oldXposOffset   = (*pCalPar)[s][m][c].getRightTimeOffset();
				Float_t oldTimeOffset   = (*pCalPar)[s][m][c].getLeftTimeOffset();
				
				Float_t newXposOffset  = oldXposOffset  + hXposOffsets->GetBinContent(m*32 + c + 1) / vProp;
				Float_t newTimeOffset  = oldTimeOffset  + hTimeOffsets->GetBinContent(m*32 + c + 1); // bfeore feb24 was -, now set to +
					     
				(*pCalPar)[s][m][c].setLeftTimeOffset(newTimeOffset);
				(*pCalPar)[s][m][c].setRightTimeOffset(newXposOffset);
				
			}
		}
	}
	
	fin->Close();
	delete myHades;
}
