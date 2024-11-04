#include "myClasses.h"
//#include "../myIncludes.h"

void getTimeAndPosOffsetsFromBatchfarm () {
	
	Bool_t manualOffsetAdjustmentsCC = true;
	Bool_t manualOffsetAdjustmentsAuAu = false;

	TString inFilePath = "~/lustre2/hades/user/jorlinsk/feb24/output/rpcCalibRawFiles/061/Control08/rpcCalibRawFiles_feb24_raw_061_ALLDAY_Control08.root";
	//TString inFilePath = "~/lustre2/hades/user/jorlinsk/feb24/output/rpcCalibRawFiles/039/Control08/rpcCalibRawFiles_feb24_raw_039_ALLDAY_Control08.root";
	//TString inFilePath = "~/lustre/hades/user/jorlinsk/feb24/output/rpcCalib/TimeAndPosCalib/039/TimeAndPosCalib_feb24_dst_039_runA.root";
	//TString inFilePath = "outRpcTask_after_0k-50k.root";
	//TString outFilePath = "TimeAndPos_feb24_dst_039_runA.root";
	TString outFilePath = "TimeAndPos_061_Control08.root";
	//TString outFilePath = "TimeAndPos_061_Control08.root";
	
	TFile* inFile = new TFile (inFilePath, "READ");
	TFile* outFile = new TFile (outFilePath, "RECREATE");
	
	outFile->cd();

	TH2F* hXposDifference[6];
	TH2F* hTimeDifference[6];
	
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
		
	for (Int_t i=0; i<6; i++) {
				   
		hXposDifference[i] = (TH2F*) inFile->Get(Form("hXposDifference_sect%i",i+1));
		hTimeDifference[i] = (TH2F*) inFile->Get(Form("hTimeDifference_sect%i",i+1));
				   
		std::cout << "Getting offsets and saving histograms for sector " << i+1 <<endl;
	
	    fillPosOffsets(hXposOffsets[i], hXposFitSigma[i], hXposFitChiSquare[i], hXposDifference[i], 20.0, outFile, Form("XposFits_sect%i", i+1));
	    fillTimeOffsets(hTimeOffsets[i], hTimeFitSigma[i], hTimeFitChiSquare[i], hTimeDifference[i], 0.25, outFile, Form("TimeFits_sect%i", i+1));
	
	}
	
	if (manualOffsetAdjustmentsCC) {
		
		hTimeOffsets[0]->SetBinContent(9, 0.1);
		hTimeOffsets[0]->SetBinContent(70, 0.5);
		hTimeOffsets[0]->SetBinContent(152, 0.2);
		
		hTimeOffsets[1]->SetBinContent(26, 0.2);
		hTimeOffsets[1]->SetBinContent(78, 0.1);
		hTimeOffsets[1]->SetBinContent(107, 0.0);
		
		hTimeOffsets[2]->SetBinContent(7, -7.8);
		hTimeOffsets[2]->SetBinContent(148, 0.1);
		
		hTimeOffsets[3]->SetBinContent(25, 0.7);
		hTimeOffsets[3]->SetBinContent(156, -8.6);
		
		hTimeOffsets[4]->SetBinContent(83, 0.2);
		hTimeOffsets[4]->SetBinContent(128, 0.3);
		
		hTimeOffsets[5]->SetBinContent(76, 0.1);
		hTimeOffsets[5]->SetBinContent(156, 0.25);
		
	}
	
	for (Int_t i=0; i<6; i++) {
		
		outFile->cd();
	
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
		if (i==0) cP[i]->Print("timeAndPosRes061.pdf(");
		else cP[i]->Print("timeAndPosRes061.pdf");
		
	}
	
	// printing toa to pdf
	for (Int_t i=0; i<6; i++) {
		
		cT[i] = new TCanvas("", "", 1500, 1000);
		//cT[i]->SetLogz(1);
		hTimeDifference[i]->Draw("colz");
		hTimeOffsets[i]->SetMarkerStyle(20);
		hTimeOffsets[i]->Draw("same P");
		//cT[i]->Write();
		if (i!=5) cT[i]->Print("timeAndPosRes061.pdf");
		else if (i==5)  cT[i]->Print("timeAndPosRes061.pdf)");
		
	}
		

		
	outFile->Save();
	outFile->Close();
}
