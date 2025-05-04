//#include "myIncludes.h"
//#include "myClasses.h"
#include "HadesCalibIncludes.h"

//const TString inRootFile = "./params/allParam_feb24_gen0_16042024.root";
//const TString inAsciiFile = "./analysisDST/feb24_dst_params_lineFit.txt";
//const TString inAsciiFile = "./params/feb24_dst_params_ofMod0_ofMod1_gRpc.txt";
//const TString inAsciiFile = "RpcCalPar_au_added_26032024_cleaned.txt";
//const TString inAsciiFile = "RpcCalPar_cc_JanOrlinski_25062024.txt";

const TString inAsciiFile = "./params/RpcCalPar_Oraclefeb24gen1.txt";	
const TString newAsciiFile = "RpcCalPar_JanOrlinski_04052025.txt";

const TString chargeOffsetsFile = "ChargeCalib_apr25_raw_116_ALLDAY_afterStartLargeStat.root";
const TString timeAndPosOffsetsFile = "TimeAndPos_116_afterStartLargeStat.root";

const Bool_t doCharge = true; 
const Bool_t doTimeAndPos = true; 

const Double_t vProp = 177.0; // propagation velocity for time/pos conversions

void generateAsciiParFile () {
	
	Int_t runId = 509332983; //random runid from feb24 logbook, file be061234303
	TString asciiParFile = inAsciiFile;
	TString rootParFile  = "";
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
	
	HRpcCalPar* pCalPar = (HRpcCalPar*) rtdb->getContainer("RpcCalPar");
	rtdb->initContainers(runId);
	
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
				Float_t newTimeOffset  = oldTimeOffset  - hTimeOffsets->GetBinContent(m*32 + c + 1); // bfeore feb24 was -, now set to +, 10.03.2025 setting back to - ???
					     
				if (abs(newTimeOffset) > 999.0) newTimeOffset = -999.0;
				if (abs(newXposOffset) > 999.0) newXposOffset = -999.0;
				
				(*pCalPar)[s][m][c].setLeftTimeOffset(newTimeOffset);
				(*pCalPar)[s][m][c].setRightTimeOffset(newXposOffset);
				
			}
		}
	}
	
	fin->Close();
	delete myHades;
}
