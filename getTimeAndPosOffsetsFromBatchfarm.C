#include "HadesCalibIncludes.h"

void getTimeAndPosOffsetsFromBatchfarm () {
	
	Bool_t manualOffsetAdjustmentsAuAu = kFALSE;

	TString inFilePath = "~/lustre/hades/user/jorlinsk/apr25/output/HadesCalib/116/afterStart/HadesCalib_apr25_raw_116_allday_afterStart.root";
	//TString inFilePath = "~/lustre/hades/user/jorlinsk/apr25/output/HadesCalib/116/afterStartLargeStat/HadesCalib_apr25_raw_116_allday_afterStartLargeStat.root";
	TString outFilePath = "TimeAndPos_116_afterStart.root";
	
	TFile* inFile = new TFile (inFilePath, "READ");
	TFile* outFile = new TFile (outFilePath, "RECREATE");
	
	outFile->cd();

	TH2F* hXposDifference[6];
	TH2F* hTimeDifference[6];
	TH2F* hRpcTimeDifferenceVsStartStripLocalMaximaOnly; // rpc time diff vs. start strip for the normalized plot 
	
	TH1F* hXposOffsets[6];
	TH1F* hTimeOffsets[6];
	
	TH1F* hXposFitSigma[6];
	TH1F* hTimeFitSigma[6];
	
	TH1F* hXposFitChiSquare[6];
	TH1F* hTimeFitChiSquare[6];
    
	for (Int_t i=0; i<6; i++) { // loop over sectors
   
		hXposOffsets[i] = new TH1F(
		Form("hXposOffsets_sect%i", i+1),
		Form("X position offsets, sector %i; 32 #times module + cell", i+1), 
		nCells, loLimitCells, hiLimitCells);
	   
		hTimeOffsets[i] = new TH1F(
		Form("hTimeOffsets_sect%i", i+1),
		Form("Time offsets, sector %i; 32 #times module + cell", i+1), 
		nCells, loLimitCells, hiLimitCells);
   
		hXposFitSigma[i] = new TH1F(
		Form("hXposFitSigma_sect%i", i+1),
		Form("X position fit sigma, sector %i; 32 #times module + cell", i+1), 
		nCells, loLimitCells, hiLimitCells);
	   
		hTimeFitSigma[i] = new TH1F(
		Form("hTimeFitSigma_sect%i", i+1),
		Form("Time fit sigma, sector %i; 32 #times module + cell", i+1), 
		nCells, loLimitCells, hiLimitCells);
   
		hXposFitChiSquare[i] = new TH1F(
		Form("hXposFitChiSquare_sect%i", i+1),
		Form("X position fit chi square, sector %i; 32 #times module + cell", i+1), 
		nCells, loLimitCells, hiLimitCells);
	   
		hTimeFitChiSquare[i] = new TH1F(
		Form("hTimeFitChiSquare_sect%i", i+1),
		Form("Time fit chi square, sector %i; 32 #times module + cell", i+1), 
		nCells, loLimitCells, hiLimitCells);
	}
	
	hRpcTimeDifferenceVsStartStripLocalMaximaOnly = (TH2F*) inFile->Get("hRpcTimeDifferenceVsStartStripLocalMaximaOnly");
	normalizeBinsX(hRpcTimeDifferenceVsStartStripLocalMaximaOnly);
		
	for (Int_t i=0; i<6; i++) {
				   
		hXposDifference[i] = (TH2F*) inFile->Get(Form("hXposDifference_sect%i",i+1));
		hTimeDifference[i] = (TH2F*) inFile->Get(Form("hTimeDifference_sect%i",i+1));
				   
		std::cout << "Getting offsets and saving histograms for sector " << i+1 <<endl;
	
	    fillPosOffsets(hXposOffsets[i], hXposFitSigma[i], hXposFitChiSquare[i], hXposDifference[i], 35.0, outFile, Form("XposFits_sect%i", i+1));
		//fillTimeOffsetsAdvanced(hTimeOffsets[i], hTimeFitSigma[i], hTimeFitChiSquare[i], hTimeDifference[i], 0.5, outFile, Form("TimeFits_sect%i", i+1));
	    fillTimeOffsetsDetailed(hTimeOffsets[i], hTimeFitSigma[i], hTimeFitChiSquare[i], hTimeDifference[i], outFile, Form("TimeFits_sect%i", i+1));
	
	}
	
	if (manualOffsetAdjustmentsAuAu) {
		
	}
	
	outFile->cd();
	hRpcTimeDifferenceVsStartStripLocalMaximaOnly->Write();
	
	for (Int_t i=0; i<6; i++) {
		
	
		hXposDifference[i]->Write();
		hTimeDifference[i]->Write();
		//hTimeDifferenceBigRange[i]->Write();
		hXposOffsets[i]->Write();
		hTimeOffsets[i]->Write();
		hXposFitSigma[i]->Write();
		hTimeFitSigma[i]->Write();
		hXposFitChiSquare[i]->Write();
		hTimeFitChiSquare[i]->Write();
		
	}
	
	TCanvas* cP[6];
	TCanvas* cT[6];
	
	// printing pos to pdf
	for (Int_t i=0; i<6; i++) {
		
		cP[i] = new TCanvas("", "", 1500, 1000);
		//cP[i]->SetLogz(1);
		hXposDifference[i]->Draw("colz");
		hXposOffsets[i]->SetMarkerStyle(20);
		hXposOffsets[i]->Draw("same P");
		//cP[i]->Write();
		if (i==0) cP[i]->Print("timeAndPosRes116.pdf(");
		else cP[i]->Print("timeAndPosRes116.pdf");
		
	}
	
	// printing toa to pdf
	for (Int_t i=0; i<6; i++) {
		
		cT[i] = new TCanvas("", "", 1500, 1000);
		//cT[i]->SetLogz(1);
		hTimeDifference[i]->Draw("colz");
		hTimeOffsets[i]->SetMarkerStyle(20);
		hTimeOffsets[i]->Draw("same P");
		//cT[i]->Write();
		if (i!=5) cT[i]->Print("timeAndPosRes116.pdf");
		else if (i==5)  cT[i]->Print("timeAndPosRes116.pdf)");
		
	}
		

		
	outFile->Save();
	outFile->Close();
}
