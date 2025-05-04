#include "HadesCalibIncludes.h"
//#include "myClasses.h"
//#include "../myIncludes.h"

void getChargeOffsetsFromBatchfarm () {
	
	const Bool_t manualAdjustmentsAuAu = kFALSE;
	
    gStyle->SetPadBottomMargin(0.15);
    gStyle->SetPadRightMargin(0.15);
	gStyle->SetOptStat(0);

	TString inFilePath = "~/lustre/hades/user/jorlinsk/apr25/output/HadesCalib/116/afterStartLargeStat/HadesCalib_apr25_raw_116_allday_afterStartLargeStat.root";
	TString outFilePath = "ChargeCalib_apr25_raw_116_ALLDAY_afterStartLargeStat.root";
	
	cout << " \n";
	cout << " \n";
	cout << " \n";
	cout << " >>>  STATUS  <<< \n";
	cout << "  inFile: " << inFilePath << "\n";
	cout << " outFile: " << outFilePath << "\n";
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
		
		
		
		// manual adjustments for au+au
		if (manualAdjustmentsAuAu) {
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
		if (i==0) cL[i]->Print("chargeRes116.pdf(");
		else cL[i]->Print("chargeRes116.pdf");
		
	}
		
	
	// printing right q to pdf
	for (Int_t i=0; i<6; i++) {
		
		cR[i] = new TCanvas("", "", 1200, 800);
		cR[i]->SetLogz(1);
		hRightChargeVsCellNumber[i]->Draw("colz");
		hRightChargeOffsets[i]->Draw("same");
		cR[i]->Write();
		if (i==5) cR[i]->Print("chargeRes116.pdf)");
		else cR[i]->Print("chargeRes116.pdf");
		
	}
		
	cout << "Saving and closing main ROOT file " << outFilePath << " ... " << endl;
		
	outFile->Save();
	outFile->Close();
}
