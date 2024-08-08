#include "myClasses.h"
//#include "../myIncludes.h"

void getTimeAndPosOffsetsFromBatchfarm () {

	//TString inFilePath = "~/lustre2/hades/user/jorlinsk/feb24/output/rpcCalibRawFiles/061/Control06/rpcCalibRawFiles_feb24_raw_061_ALLDAY_Control06.root";
	TString inFilePath = "~/lustre2/hades/user/jorlinsk/feb24/output/rpcCalibRawFiles/039/Control06/rpcCalibRawFiles_feb24_raw_039_ALLDAY_Control06.root";
	//TString inFilePath = "~/lustre/hades/user/jorlinsk/feb24/output/rpcCalib/TimeAndPosCalib/039/TimeAndPosCalib_feb24_dst_039_runA.root";
	//TString inFilePath = "outRpcTask_after_0k-50k.root";
	//TString outFilePath = "TimeAndPos_feb24_dst_039_runA.root";
	TString outFilePath = "TimeAndPos_039_Control06.root";
	
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
	    fillTimeOffsets(hTimeOffsets[i], hTimeFitSigma[i], hTimeFitChiSquare[i], hTimeDifference[i], 0.35, outFile, Form("TimeFits_sect%i", i+1));
	
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
		
	outFile->Save();
	outFile->Close();
}
