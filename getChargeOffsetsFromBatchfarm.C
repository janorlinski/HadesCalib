#include "myClasses.h"
//#include "../myIncludes.h"

void getChargeOffsetsFromBatchfarm () {
	
	const Bool_t manualAdjustmentsCC = kFALSE;
	const Bool_t manualAdjustmentsAuAu = kTRUE;
	
	if (manualAdjustmentsCC && manualAdjustmentsAuAu) {
		cout << "ERROR! Cannot use manual adjustments for both beamtimes at the same time!" << endl;
		return;
	}
	
    gStyle->SetPadBottomMargin(0.15);
    gStyle->SetPadRightMargin(0.15);
	gStyle->SetOptStat(0);

	//TString inFilePath = "~/lustre/hades/user/jorlinsk/feb22/output/rpcCalib/ChargeCalib/042/ChargeCalib_feb22_raw_042_ALL.root";
	//TString inFilePath = "~/lustre/hades/user/jorlinsk/feb22/output/rpcCalib/TestParameters/042/TestParameters_feb22_raw_042_ALL.root";
	//TString inFilePath = "~/lustre/hades/user/jorlinsk/feb24/output/rpcCalib/ChargeCalib/061/ChargeCalib_feb24_raw_061_ALLDAY_runA.root";
	//TString inFilePath = "~/lustre2/hades/user/jorlinsk/feb24/output/rpcCalibRawFiles/061/Control03/rpcCalibRawFiles_feb24_raw_061_ALLDAY_Control03.root";
	TString inFilePath = "~/lustre2/hades/user/jorlinsk/feb24/output/rpcCalibRawFiles/061/Control05/rpcCalibRawFiles_feb24_raw_061_ALLDAY_Control05.root";
	//TString inFilePath = "~/lustre2/hades/user/jorlinsk/feb24/output/rpcCalibRawFiles/039/Control05/rpcCalibRawFiles_feb24_raw_039_ALLDAY_Control05.root";
	//TString outFilePath = "ChargeCalib_feb22_raw_042_ALL.root";
	TString outFilePath = "ChargeCalib_feb24_raw_061_ALLDAY_Control05.root";
	//TString outFilePath = "ChargeCalib_feb24_raw_039_ALLDAY_Control05.root";
	
	cout << " \n";
	cout << " \n";
	cout << " \n";
	cout << " >>>  STATUS  <<< \n";
	cout << "  inFile: " << inFilePath << "\n";
	cout << " outFile: " << outFilePath << "\n";
	cout << " manual adjustments of offsets for CC is: " << manualAdjustmentsCC << "\n";
	cout << " manual adjustments of offsets for AuAu is: " << manualAdjustmentsAuAu << "\n";
	cout << " \n";
	gSystem->Sleep(2000);
	
	
	TFile* inFile = new TFile (inFilePath, "READ");
	TFile* outFile = new TFile (outFilePath, "RECREATE");
	
	outFile->cd();
	
	TH2F* hLeftChargeVsCellNumber[6];
	TH2F* hRightChargeVsCellNumber[6];
	TH1F* hLeftChargeOffsets[6];
	TH1F* hRightChargeOffsets[6];
	    
	for (Int_t i=0; i<6; i++) { // loop over sectors
			   			   
		hLeftChargeOffsets[i] = new TH1F(
		Form("hLeftChargeOffsets_sect%i", i+1),
		Form("Left charge offsets, sector %i; 32 #times module + cell; charge offset", i+1), 
		nCells, loLimitCells, hiLimitCells);
	   
		hRightChargeOffsets[i] = new TH1F(
		Form("hRightChargeOffsets_sect%i", i+1),
		Form("Right charge offsets, sector %i; 32 #times module + cell; charge offset", i+1), 
		nCells, loLimitCells, hiLimitCells);
	}
	
	for (Int_t i=0; i<6; i++) {
		
		//if (i!=1) continue; //select a sector to look at couts
				   
		hLeftChargeVsCellNumber[i] = (TH2F*) inFile->Get(Form("hLeftChargeVsCellNumber_sect%i", i+1));
		hRightChargeVsCellNumber[i] = (TH2F*) inFile->Get(Form("hRightChargeVsCellNumber_sect%i", i+1));
				   
		std::cout << "Getting offsets and saving histograms for sector " << i+1 <<endl;
		
		const Float_t threshFrac = 0.25;
		const Float_t diffFraction = 0.2;
		const Float_t nConsBins = 12;
		const Float_t nCycles = 2;
		
		fillChargeOffsetsAdvanced(hLeftChargeOffsets[i], hLeftChargeVsCellNumber[i], threshFrac, diffFraction, nConsBins, nCycles);
		fillChargeOffsetsAdvanced(hRightChargeOffsets[i], hRightChargeVsCellNumber[i], threshFrac, diffFraction, nConsBins, nCycles);
		
		// manual adustments for c+c
		// do them here so that they are included in control plots
		if (manualAdjustmentsCC) {
			hLeftChargeOffsets[2]->SetBinContent(44, -1.0);
			hLeftChargeOffsets[5]->SetBinContent(10, -0.0);
			hLeftChargeOffsets[5]->SetBinContent(11, -1.8);
			hRightChargeOffsets[2]->SetBinContent(19, -1.0);
			hRightChargeOffsets[2]->SetBinContent(164, 0.0);
			hRightChargeOffsets[2]->SetBinContent(165, 0.0);
			hRightChargeOffsets[2]->SetBinContent(166, -0.8);
			hRightChargeOffsets[5]->SetBinContent(42, 0.0);
			hRightChargeOffsets[5]->SetBinContent(133, 0.0);
			hRightChargeOffsets[5]->SetBinContent(134, -0.5);
			hRightChargeOffsets[5]->SetBinContent(171, 0.0);
		}
		
		
		// manual adjustments for au+au
		if (manualAdjustmentsAuAu) {
			hLeftChargeOffsets[2]->SetBinContent(83, -1.3);
		}
		hRightChargeVsCellNumber[i]->Write();
		hLeftChargeVsCellNumber[i]->Write();
		hRightChargeOffsets[i]->Write();
		hLeftChargeOffsets[i]->Write();
		
	}
	
	TCanvas* cL[6];
	TCanvas* cR[6];
	
	// printing left q to pdf
	for (Int_t i=0; i<6; i++) {
		
		cL[i] = new TCanvas("", "", 1200, 800);
		cL[i]->SetLogz(1);
		hLeftChargeVsCellNumber[i]->Draw("colz");
		hLeftChargeOffsets[i]->Draw("same");
		cL[i]->Write();
		if (i==0) cL[i]->Print("chargeRes061.pdf(");
		else cL[i]->Print("chargeRes061.pdf");
		
	}
		
	
	// printing right q to pdf
	for (Int_t i=0; i<6; i++) {
		
		cR[i] = new TCanvas("", "", 1200, 800);
		cR[i]->SetLogz(1);
		hRightChargeVsCellNumber[i]->Draw("colz");
		hRightChargeOffsets[i]->Draw("same");
		cR[i]->Write();
		if (i==5) cR[i]->Print("chargeRes061.pdf)");
		else cR[i]->Print("chargeRes061.pdf");
		
	}
		
	cout << "Saving and closing main ROOT file " << outFilePath << " ... " << endl;
		
	outFile->Save();
	outFile->Close();
}
