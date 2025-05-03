#include "myClasses.h"
#include "myIncludes.h"


void getStartOffsetsFromBatchfarm () {

	const Bool_t isSecondIter = false;

	//TString inFilePath = "~/lustre/hades/user/jorlinsk/feb24/output/startCalib/061/startCalib_feb24_raw_061_ALLDAY_ITER0_runB.root";
	TString inFilePath = "~/lustre/hades/user/jorlinsk/feb24/output/startCalib/061/startCalib_feb24_raw_061_ALLDAY_ITER1_runB.root";
	//TString inFilePath = "~/lustre/hades/user/jorlinsk/feb24/output/startCalib/040/startCalib_feb24_raw_040_ALLDAY_ITER0.root";
	//TString inFilePath = "~/lustre/hades/user/jorlinsk/feb24/output/startCalib/039/startCalib_feb24_raw_039_ALLDAY_ITER0.root";
	//TString inFilePath = "~/lustre/hades/user/jorlinsk/feb24/output/startCalib/039/startCalib_feb24_raw_039_ALLDAY_ITER1d.root";
	//TString inFilePath = "~/lustre/hades/user/jorlinsk/feb24/output/startCalib/039/startCalib_feb24_raw_039_ALLDAY_ITER3.root";
	//TString inFilePath = "~/lustre/hades/user/jorlinsk/feb24/output/startCalib/039/startCalib_feb24_raw_039_ALLDAY_TWTEST.root";
	//TString outFilePath = "StartCalib_feb24_raw_061_ALLDAY_ITER0_runB.root";
	TString outFilePath = "StartCalib_feb24_raw_061_ALLDAY_ITER1_runB.root";
	//TString outFilePath = "StartCalib_feb24_raw_039_ALLDAY_ITER0.root";
	//TString outFilePath = "StartCalib_feb24_raw_038_16h03_TEST14h35.root";
	
	
	
	TFile* inFile = new TFile (inFilePath, "READ");
	TFile* outFile = new TFile (outFilePath, "RECREATE");
	
	outFile->cd();
	
	TH2F* hTDiff_ref1;
	TH2F* hTDiff_ref2;
	TH2F* hTDiff_ref3;
	TH2F* hTDiff_ref4;
	TH1F* hTDiffOffsets;
    TH1F* hTimeDiffForTimeWalk[nChannels][nPointsForTimeWalk];
    TH1F* hWidthForTimeWalk[nChannels][nPointsForTimeWalk];
    
    TGraphErrors* gTimeWalk_sqrt[nChannels];
    TGraphErrors* gTimeWalk_line[nChannels];
    
    TH2F* hTimeDiffVsWidth[nChannels];
    
    TCanvas* cVerifyTimeWalkFunctions[nChannels];
    
	hTDiffOffsets = new TH1F("hTDiffOffsets", "hTDiffOffsets", nChannels, loLimitChannels, hiLimitChannels);
			   
	hTDiff_ref1 = (TH2F*) inFile->Get(Form("time diffs module 0/hTimeDiffVsChannel_refCh%i", refCh[0]));
	hTDiff_ref2 = (TH2F*) inFile->Get(Form("time diffs module 1/hTimeDiffVsChannel_refCh%i", refCh[1]));
	hTDiff_ref3 = (TH2F*) inFile->Get(Form("time diffs module 2/hTimeDiffVsChannel_refCh%i", refCh[2]));
	hTDiff_ref4 = (TH2F*) inFile->Get(Form("time diffs module 3/hTimeDiffVsChannel_refCh%i", refCh[3]));
	
	for (Int_t i = 0; i<nChannels; i++) {
		
		cVerifyTimeWalkFunctions[i] = new TCanvas(Form("cVerifyTimeWalkFunctions_ch%i", i+1), Form("cVerifyTimeWalkFunctions_ch%i", i+1));
		hTimeDiffVsWidth[i] = (TH2F*) inFile->Get(Form("2D hists for time walk/hTimeDiffVsWidth_%i", i+1));
		
		gTimeWalk_sqrt[i] = new TGraphErrors();
		gTimeWalk_line[i] = new TGraphErrors();
		
		gTimeWalk_sqrt[i] -> SetTitle(Form("Time walk graph, fit sqrt, channel %i; ToT; T_{diff} [ns]", i+1));
		gTimeWalk_line[i] -> SetTitle(Form("Time walk graph, fit line, channel %i; ToT; T_{diff} [ns]", i+1));
		
		gTimeWalk_sqrt[i] -> SetName(Form("TWgraph_sqrt_ch%i", i+1));
		gTimeWalk_line[i] -> SetName(Form("TWgraph_line_ch%i", i+1));
			
		for (Int_t j = 0; j<nPointsForTimeWalk; j++) {
	
			hTimeDiffForTimeWalk[i][j] = (TH1F*) inFile->Get(
			Form("hTimeDiffForTimeWalk_refCh%i_testCh%i_widthBin%i", refCh[getRefModule(i+1)], i+1, j));
			
			hWidthForTimeWalk[i][j] = (TH1F*) inFile->Get(
			Form("hWidthForTimeWalk_refCh%i_testCh%i_widthBin%i", refCh[getRefModule(i+1)], i+1, j));
			
		}
	}
	

	fillTimeOffsets(hTDiffOffsets, hTDiff_ref1, 1*nChPerMod+1, 2*nChPerMod, outFile, "ref0");
	fillTimeOffsets(hTDiffOffsets, hTDiff_ref2, 0*nChPerMod+1, 1*nChPerMod, outFile, "ref1");
	//fillTimeOffsets(hTDiffOffsets, hTDiff_ref3, 49, 96, outFile, "ref3"); 
	//fillTimeOffsets(hTDiffOffsets, hTDiff_ref4, 1, 48, outFile, "ref4");
	
	// time walk analysis
	
	outFile->mkdir("TW fits");
	
	const Float_t fitRadius = 3.0;
	for (Int_t i = 0; i<nChannels; i++) {
		
		outFile->cd("TW fits");
		outFile->mkdir(Form("TW fits/ch%i", i+1));
		outFile->cd(Form("TW fits/ch%i", i+1));
		
		if (skipThisChannel(i+1)) continue;
		
		Int_t pointIterator = 0;
		
		cout << " >>> STATUS: beginning the j loop for channel i = " << i << endl;
		
		for (Int_t j = 0; j<nPointsForTimeWalk; j++) {
			
			cout << "entering time walk analysis, i = " << i << ", j = " << j << endl;
			
			Double_t positionOfMaximum = hTimeDiffForTimeWalk[i][j]->GetBinCenter(hTimeDiffForTimeWalk[i][j]->GetMaximumBin());
			Double_t heightOfMaximum = hTimeDiffForTimeWalk[i][j]->GetMaximum();
			//Double_t positionOfMaximum = 0.0;
			Double_t fitLoEdge = positionOfMaximum - fitRadius;
			Double_t fitHiEdge = positionOfMaximum + fitRadius;
			//std::cout << "Gaus will be fitted in range [" << fitLoEdge << ", " << fitHiEdge << "] \n";
			
			// fit and fill offset
			
			TF1* gausFit = new TF1 (Form("fit_%i", i), gaussianWithConstBckg, fitLoEdge, fitHiEdge, 4);
			gausFit->SetParameter(0, 1000);
			gausFit->SetParLimits(0, 0.1*heightOfMaximum, 1000000.0);
			
			gausFit->SetParameter(1, positionOfMaximum);
			gausFit->SetParLimits(1, fitLoEdge + 0.25*fitRadius, fitHiEdge - 0.25*fitRadius);
			
			gausFit->SetParameter(2, 1.0);
			gausFit->SetParLimits(2, 0.0, 2.0);
			
			gausFit->SetParameter(3, 0.0);
			gausFit->SetParLimits(3, 0.0, 0.5*heightOfMaximum);
			
			gausFit->SetRange(fitLoEdge, fitHiEdge);
			hTimeDiffForTimeWalk[i][j]->Fit(gausFit, "WQ");
			hTimeDiffForTimeWalk[i][j]->Write();
			Double_t center = gausFit->GetParameter(1);
			Double_t sigma = gausFit->GetParameter(2);
			Double_t centerError = gausFit->GetParError(1);
			Double_t nEntr = hTimeDiffForTimeWalk[i][j]->GetEntries();
			Double_t centerTotError = sqrt(nEntr + centerError*centerError); // 
			
			if (centerError > 2.0) {
				cout << "skipping due to centerError > 2.0" << endl;
				continue; //dont include points with a large fit error (this is mostly for cosmetics)
			}
			if (nEntr < tDiffEntriesThreshold) {
				cout << "skipping due to nEntr below threshold" << endl;
				continue; //dont include points where the tdiff hist has not enough entries
			}
			if (sigma < 0.075) {
				cout << "skipping due to sigma < 0.075" << endl;
				continue; //dont include points where sigma < 0.075, this cannot be a physical result
			}
			
			gTimeWalk_sqrt[i]->AddPoint(hWidthForTimeWalk[i][j]->GetMean(), center);
			gTimeWalk_line[i]->AddPoint(hWidthForTimeWalk[i][j]->GetMean(), center);
			
			gTimeWalk_sqrt[i]->SetPointError(pointIterator, hWidthForTimeWalk[i][j]->GetStdDev(), sigma);
			gTimeWalk_line[i]->SetPointError(pointIterator, hWidthForTimeWalk[i][j]->GetStdDev(), sigma);
			
			pointIterator++;
			
		}
		
		TF1* timeWalkFitSqrt = new TF1 (Form("timeWalkSqrtFit_%i", i+1), timeWalkFuncWithSqrt, widthLoLimitTW, widthHiLimitTW, 2);
		gTimeWalk_sqrt[i]->Fit(timeWalkFitSqrt, "R+");
		
		TF1* timeWalkFitLine = new TF1 (Form("timeWalkLineFit_%i", i+1), timeWalkFuncLinear, widthLoLimitTW, widthHiLimitTW, 2);
		
		//we can already form some expectations about the line:
		
		timeWalkFitLine->SetParameter(0, -0.5);
		timeWalkFitLine->SetParameter(1, 0.2);
		//timeWalkFitLine->SetParLimits(1, 0.0, 10.0);
		
		gTimeWalk_line[i]->Fit(timeWalkFitLine, "R+");
		
	}
		
	outFile->cd();
		
	hTDiff_ref1->Write();
	hTDiff_ref2->Write();
	hTDiff_ref3->Write();
	hTDiff_ref4->Write();
	hTDiffOffsets->Write();
	
	outFile->mkdir("TW 2d hists");
	outFile->mkdir("TW sqrt graphs");
	outFile->mkdir("TW line graphs");
	
	for (Int_t i=0; i<nChannels; i++) {
		
		if (skipThisChannel(i+1)) continue;
		
		outFile->cd();
		cVerifyTimeWalkFunctions[i]->cd();
		hTimeDiffVsWidth[i]->Draw("colz");
		gTimeWalk_line[i]->Draw("P same");
		cVerifyTimeWalkFunctions[i]->Write();
		
		outFile->cd("TW 2d hists");
		hTimeDiffVsWidth[i]->Write();
		
		outFile->cd("TW sqrt graphs");
		gTimeWalk_sqrt[i]->Write();
		
		outFile->cd("TW line graphs");
		gTimeWalk_line[i]->Write();
	}
	
	outFile->Save();
	outFile->Close();
}
