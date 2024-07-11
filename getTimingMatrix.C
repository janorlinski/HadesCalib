#include "myClasses.h"
//#include "../myIncludes.h"

void getTimingMatrix () {


    gStyle->SetPadBottomMargin(0.15);
    gStyle->SetPadRightMargin(0.15);
	gStyle->SetOptStat(0);

	const Int_t nDays = 1;
	Int_t nF[nDays] = {577};
	//Int_t nF[nDays] = {514};
	//Int_t nF[nDays] = {20, 577, 833};
	//Int_t nF[nDays] = {20, 577, 833, 837, 457};
	//Int_t nF[nDays] = {20};
	//Int_t nF = 160; //060
	//Int_t nF = 160; //061 00h00
	//Int_t nF = 20; //061 00h00 8 files per job
	
	//Int_t nF = 514 //039 8 files per job
	//Int_t nF = 20 //060 8 files per job
	//Int_t nF = 577; //061  8 files per job
	//Int_t nF = 833; //062  8 files per job
	//Int_t nF = 837; //063  8 files per job
	//Int_t nF = 457; //064  8 files per job
	
	
	TString day[nDays] = {"061"};
	//TString day[nDays] = {"039"};
	//TString day[nDays] = {"060", "061", "062"};
	//TString day[nDays] = {"060", "061", "062", "063", "064"};
	//TString day[nDays] = {"061"};
	TString timestamp = "ALLDAY";
	TString code = "Control05";
	
	const Int_t nEvts = 160000;
	
	Int_t nFtotal = 0;
	for (Int_t day=0; day<nDays; day++) nFtotal+=nF[day];
	cout << "Analyzing " << nDays << " dats, total files: " << nFtotal << endl;
	
	TFile* fOut = new TFile(Form("timingMatrix_%s-%s_%s_%s.root", day[0].Data(), day[nDays-1].Data(), timestamp.Data(), code.Data()), "RECREATE");
	fOut->mkdir("OneStripTiming");
	fOut->cd();
	
	TH2F* habsAllDays = new TH2F ("habsAllDays", "", nFtotal, 0.5, nFtotal+0.5, 80, 0.5, 80.5);
	TH2F* htotAllDays = new TH2F ("htotAllDays", "", nFtotal, 0.5, nFtotal+0.5, 80, 0.5, 80.5);
	TH2F* hmuAllDays = new TH2F ("hmuAllDays", "", nFtotal, 0.5, nFtotal+0.5, 80, 0.5, 80.5);
	TH2F* hsigAllDays = new TH2F ("hsigAllDays", "", nFtotal, 0.5, nFtotal+0.5, 80, 0.5, 80.5);
	TH2F* hnPionsAllDays = new TH2F ("hnPionsAllDays", "", nFtotal, 0.5, nFtotal+0.5, 80, 0.5, 80.5);
	TH2F* hnNegTracksAllDays = new TH2F ("hnNegTracksAllDays", "", nFtotal, 0.5, nFtotal+0.5, 80, 0.5, 80.5);
	TH2F* hStartMultMod0AllDays = new TH2F ("hStartMultMod0AllDays", "", nFtotal, 0.5, nFtotal+0.5, 50, 0.0, 50.0);
	TH2F* hStartMultMod1AllDays = new TH2F ("hStartMultMod1AllDays", "", nFtotal, 0.5, nFtotal+0.5, 50, 0.0, 50.0);
	TH2F* hStartMultMod3AllDays = new TH2F ("hStartMultMod3AllDays", "", nFtotal, 0.5, nFtotal+0.5, 50, 0.0, 50.0);
	TH2F* hmuDistributionAllDays = new TH2F ("hmuDistributionAllDays", "", 400, -1.0, 1.0, 80, 0.5, 80.5);
	
	TH2I* hOneStripTimingAllDays[80];
	TGraphErrors* gOneStripTimingAllDays[80];
	// WATCH OUT! Please make sure that you use identical binning with the one in the source files
	for (Int_t k = 0; k<80; k++) hOneStripTimingAllDays[k]  = new TH2I (Form("OneStripTimingAllDays_%i", k+1), Form("one strip timing, ch%i; File; #Delta #t [ns]", k+1), nFtotal, 0.5, nFtotal+0.5, 500, -10.0, 10.0);
	for (Int_t k = 0; k<80; k++) { 
		gOneStripTimingAllDays[k]  = new TGraphErrors ();
		gOneStripTimingAllDays[k]-> SetName(Form("OneStripTimingAllDays_%i", k+1));
	}
			
	habsAllDays->SetTitle(Form("Position of cal-time maximum vs. file, days %s-%s, %s, %s; File; START Strip", day[0].Data(), day[nDays-1].Data(), timestamp.Data(), code.Data()));
	htotAllDays->SetTitle(Form("Positiom of ToT maximum vs. file, days %s-%s, %s, %s; File; START Strip", day[0].Data(), day[nDays-1].Data(), timestamp.Data(), code.Data()));
	hmuAllDays->SetTitle(Form("Mu vs. file, days %s-%s, %s, %s; File; START Strip", day[0].Data(), day[nDays-1].Data(), timestamp.Data(), code.Data()));
	hsigAllDays->SetTitle(Form("Sigma vs. file, days %s-%s, %s, %s; File; START Strip", day[0].Data(), day[nDays-1].Data(), timestamp.Data(), code.Data()));
	hnPionsAllDays->SetTitle(Form("N tracks in tdiff histo ('pions') vs. file, days %s-%s, %s, %s; File; START Strip", day[0].Data(), day[nDays-1].Data(), timestamp.Data(), code.Data()));
	hnNegTracksAllDays->SetTitle(Form("N negative tracks vs. file, days %s-%s, %s, %s; File; START Strip", day[0].Data(), day[nDays-1].Data(), timestamp.Data(), code.Data()));
	hStartMultMod0AllDays->SetTitle(Form("Start multiplicity module 0 vs. file, days %s-%s, %s, %s; File; Mult per module", day[0].Data(), day[nDays-1].Data(), timestamp.Data(), code.Data()));
	hStartMultMod1AllDays->SetTitle(Form("Start multiplicity module 1 vs. file, days %s-%s, %s, %s; File; Mult per module", day[0].Data(), day[nDays-1].Data(), timestamp.Data(), code.Data()));
	hStartMultMod3AllDays->SetTitle(Form("Start multiplicity module 3 vs. file, days %s-%s, %s, %s; File; Mult per module ", day[0].Data(), day[nDays-1].Data(), timestamp.Data(), code.Data()));
	hmuDistributionAllDays->SetTitle(Form("Mu distribution, days %s-%s, %s, %s; Time difference; START Strip", day[0].Data(), day[nDays-1].Data(), timestamp.Data(), code.Data()));
		
	for (Int_t d=0; d<nDays; d++) {
		
		cout << "Starting day " << day[d] << endl;
		
		TString inFileDir = Form("~/lustre2/hades/user/jorlinsk/feb24/output/rpcCalibRawFiles/%s/%s/rpcCalibRawFiles_feb24_raw_%s_%s_%s_", 
		day[d].Data(), code.Data(), day[d].Data(), timestamp.Data(), code.Data());
		
		//vector <TH1F*> vecMu;
		//vector <TH1F*> vecSig;
		//vector <TString> vecTit;
		
		TH2F* habs = new TH2F (Form("habsDay%s", day[d].Data()), "", nF[d], 0.5, nF[d]+0.5, 80, 0.5, 80.5);
		TH2F* htot = new TH2F (Form("htotDay%s", day[d].Data()), "", nF[d], 0.5, nF[d]+0.5, 80, 0.5, 80.5);
		TH2F* hmu = new TH2F (Form("hmuDay%s", day[d].Data()), "", nF[d], 0.5, nF[d]+0.5, 80, 0.5, 80.5);
		TH2F* hsig = new TH2F (Form("hsigDay%s", day[d].Data()), "", nF[d], 0.5, nF[d]+0.5, 80, 0.5, 80.5);
		TH2F* hnNegTracks = new TH2F (Form("hnNegTracksDay%s", day[d].Data()), "", nF[d], 0.5, nF[d]+0.5, 80, 0.5, 80.5);
		TH2F* hnPions = new TH2F (Form("hnPionsDay%s", day[d].Data()), "", nF[d], 0.5, nF[d]+0.5, 80, 0.5, 80.5);
		TH2F* hStartMultMod0 = new TH2F (Form("hStartMultMod0Day%s", day[d].Data()), "", nF[d], 0.5, nF[d]+0.5, 50, 0.0, 50.0);
		TH2F* hStartMultMod1 = new TH2F (Form("hStartMultMod1Day%s", day[d].Data()), "", nF[d], 0.5, nF[d]+0.5, 50, 0.0, 50.0);
		TH2F* hStartMultMod3 = new TH2F (Form("hStartMultMod3Day%s", day[d].Data()), "", nF[d], 0.5, nF[d]+0.5, 50, 0.0, 50.0);
		TH2F* hmuDistribution = new TH2F (Form("hmuDistributionDay%s", day[d].Data()), "", 400, -1.0, 1.0, 80, 0.5, 80.5);
		
		habs->SetTitle(Form("Position of cal-time maximum vs. file, day %s, %s, %s; File; START Strip", day[d].Data(), timestamp.Data(), code.Data()));
		htot->SetTitle(Form("Positiom of ToT maximum vs. file, day %s, %s, %s; File; START Strip", day[d].Data(), timestamp.Data(), code.Data()));
		hmu->SetTitle(Form("Mu vs. file, day %s, %s, %s; File; START Strip", day[d].Data(), timestamp.Data(), code.Data()));
		hsig->SetTitle(Form("Sigma vs. file, day %s, %s, %s; File; START Strip", day[d].Data(), timestamp.Data(), code.Data()));
		hnNegTracks->SetTitle(Form("N negative tracks vs. file, day %s, %s, %s; File; START Strip", day[d].Data(), timestamp.Data(), code.Data()));
		hnPions->SetTitle(Form("N tracks in tdiff histo ('pions') vs. file, day %s, %s, %s; File; START Strip", day[d].Data(), timestamp.Data(), code.Data()));
		hStartMultMod0->SetTitle(Form("Start multiplicity module 0 vs. file, day %s, %s, %s; File; Mult per module", day[d].Data(), timestamp.Data(), code.Data()));
		hStartMultMod1->SetTitle(Form("Start multiplicity module 1 vs. file, day %s, %s, %s; File; Mult per module", day[d].Data(), timestamp.Data(), code.Data()));
		hStartMultMod3->SetTitle(Form("Start multiplicity module 3 vs. file, day %s, %s, %s; File; Mult per module", day[d].Data(), timestamp.Data(), code.Data()));
		hmuDistribution->SetTitle(Form("Mu distribution, day %s, %s, %s; Time difference; START Strip", day[d].Data(), timestamp.Data(), code.Data()));
		
		for (Int_t i=1; i<=nF[d]; i++) {
			
			TString inPath = Form(inFileDir+"%i.root", i);
			TFile* fIn = new TFile (inPath, "READ");
			Int_t iT = i; // index in alldays histograms
			//if (d>0) iT += nF[d-1];
			for (Int_t id=0; id < d; id++) iT += nF[id];
			
			if (fIn->GetNkeys()==0) {
				
				if(i==1) { // in alldays histos, only add first file
					habsAllDays->GetXaxis()->SetBinLabel(iT, "FILE HAS NO KEYS");
					htotAllDays->GetXaxis()->SetBinLabel(iT, "FILE HAS NO KEYS");
					hmuAllDays->GetXaxis()->SetBinLabel(iT, "FILE HAS NO KEYS");
					hsigAllDays->GetXaxis()->SetBinLabel(iT, "FILE HAS NO KEYS");
					hnNegTracksAllDays->GetXaxis()->SetBinLabel(iT, "FILE HAS NO KEYS");
					hnPionsAllDays->GetXaxis()->SetBinLabel(iT, "FILE HAS NO KEYS");
					hStartMultMod0AllDays->GetXaxis()->SetBinLabel(iT, "FILE HAS NO KEYS");
					hStartMultMod1AllDays->GetXaxis()->SetBinLabel(iT, "FILE HAS NO KEYS");
					hStartMultMod3AllDays->GetXaxis()->SetBinLabel(iT, "FILE HAS NO KEYS");
					//for (Int_t k = 0; k<80; k++) hOneStripTimingAllDays[k]->GetXaxis()->SetBinLabel(iT, "FILE HAS NO KEYS");
				}
				
				habs->GetXaxis()->SetBinLabel(i, "FILE HAS NO KEYS");
				htot->GetXaxis()->SetBinLabel(i, "FILE HAS NO KEYS");
				hmu->GetXaxis()->SetBinLabel(i, "FILE HAS NO KEYS");
				hsig->GetXaxis()->SetBinLabel(i, "FILE HAS NO KEYS");
				hnNegTracks->GetXaxis()->SetBinLabel(i, "FILE HAS NO KEYS");
				hnPions->GetXaxis()->SetBinLabel(i, "FILE HAS NO KEYS");
				hStartMultMod0->GetXaxis()->SetBinLabel(i, "FILE HAS NO KEYS");
				hStartMultMod1->GetXaxis()->SetBinLabel(i, "FILE HAS NO KEYS");
				hStartMultMod3->GetXaxis()->SetBinLabel(i, "FILE HAS NO KEYS");
				
				fIn->Close();
				continue;
			}
			
			//cout << "getting histograms from file " << inPath << " ..." << endl;
			TH1F* abs = (TH1F*) fIn->Get("maxAbsTimePosOneFile");
			TH1F* tot = (TH1F*) fIn->Get("maxWidthPosOneFile");
			TH1F* mu = (TH1F*) fIn->Get("fitMuOneFile");
			TH1F* sigma = (TH1F*) fIn->Get("fitSigmaOneFile");
			TH1F* negtracks = (TH1F*) fIn->Get("hNegativeTracksOneFile");
			TH1F* pions = (TH1F*) fIn->Get("hPionsOneFile");
			TH2F* mult = (TH2F*) fIn->Get("hMultiplicityPerModule");
			
			TString title = mu->GetTitle();
			TString tit = title.Remove(title.First(","));
			
			//cout << "Title: " << tit << endl;
			
			if(i==1) { // in alldays histos, only add first file, actually just day number
				habsAllDays->GetXaxis()->SetBinLabel(iT, day[d]);
				htotAllDays->GetXaxis()->SetBinLabel(iT, day[d]);
				hmuAllDays->GetXaxis()->SetBinLabel(iT, day[d]);
				hsigAllDays->GetXaxis()->SetBinLabel(iT, day[d]);
				hnNegTracksAllDays->GetXaxis()->SetBinLabel(iT, day[d]);
				hnPionsAllDays->GetXaxis()->SetBinLabel(iT, day[d]);
				hStartMultMod0AllDays->GetXaxis()->SetBinLabel(iT, day[d]);
				hStartMultMod1AllDays->GetXaxis()->SetBinLabel(iT, day[d]);
				hStartMultMod3AllDays->GetXaxis()->SetBinLabel(iT, day[d]);
				//for (Int_t k = 0; k<80; k++) hOneStripTimingAllDays[k]->GetXaxis()->SetBinLabel(iT, day[d]);
			}
			
			habs->GetXaxis()->SetBinLabel(i, tit);
			htot->GetXaxis()->SetBinLabel(i, tit);
			hmu->GetXaxis()->SetBinLabel(i, tit);
			hsig->GetXaxis()->SetBinLabel(i, tit);
			hnNegTracks->GetXaxis()->SetBinLabel(i, tit);
			hnPions->GetXaxis()->SetBinLabel(i, tit);
			hStartMultMod0->GetXaxis()->SetBinLabel(i, tit);
			hStartMultMod1->GetXaxis()->SetBinLabel(i, tit);
			hStartMultMod3->GetXaxis()->SetBinLabel(i, tit);
			
			fOut->mkdir(tit);
			
			TH1F* projMod0 = (TH1F*) mult->ProjectionY("projMod0", 1, 1);
			TH1F* projMod1 = (TH1F*) mult->ProjectionY("projMod1", 2, 2);
			TH1F* projMod3 = (TH1F*) mult->ProjectionY("projMod3", 4, 4);
			
			for (Int_t j=1; j<=50; j++) { // loop over multiplicity bins
				
				hStartMultMod0AllDays->SetBinContent(iT, j, projMod0->GetBinContent(j));
				hStartMultMod1AllDays->SetBinContent(iT, j, projMod1->GetBinContent(j));
				hStartMultMod3AllDays->SetBinContent(iT, j, projMod3->GetBinContent(j));
				
				hStartMultMod0->SetBinContent(i, j, projMod0->GetBinContent(j));
				hStartMultMod1->SetBinContent(i, j, projMod1->GetBinContent(j));
				hStartMultMod3->SetBinContent(i, j, projMod3->GetBinContent(j));
				
			}
			
			for (Int_t j=1; j<=80; j++) {
				
				//cout << "j = " << j << endl;
				// the lines below are copied from the original myClasses.h in order to move the fitting phase here
				// this way fit optimizations can be done without resending all the data to the batchfarm
				
				TH1F* projection = (TH1F*) fIn->Get(Form("projection__ch%i", j));
				Int_t nEntrOfSource = projection->GetEntries();
				//projection->ShowPeaks(0.1, "", 0.5);
				
				// set fit range
			
				Float_t fitRadius = 1.5;
				//Double_t positionOfMaximum = projection->GetBinCenter(projection->GetMaximumBin());
				Double_t positionOfMaximum = 0.0;
				Double_t heightOfMaximum = projection->GetMaximum();
				Double_t fitLoEdge = positionOfMaximum - fitRadius;
				Double_t fitHiEdge = positionOfMaximum + fitRadius;
				//std::cout << "Gaus will be fitted in range [" << fitLoEdge << ", " << fitHiEdge << "] \n";
				
				// fit and fill offset
				
				TF1* gausFit = new TF1 (Form("secondFit_%i", i), gaussianWithConstBckg, fitLoEdge, fitHiEdge, 4);
				
				gausFit->SetParameter(0, 1.0);
				gausFit->SetParLimits(0, 1.0, 1+3*heightOfMaximum);
				
				gausFit->SetParameter(1, 0.0);
				gausFit->SetParLimits(1, (-1.0)*fitRadius, fitRadius);
				
				gausFit->SetParameter(2, 0.210);
				gausFit->SetParLimits(2, 0.150, 0.270);
				
				//gausFit->FixParameter(3, 0.0);
				gausFit->SetParameter(3, 0);
				gausFit->SetParLimits(3, 0.0, 0.5*heightOfMaximum);
				
				gausFit->SetRange(fitLoEdge, fitHiEdge);
				if (nEntrOfSource>1000) projection->Fit(gausFit, "RQ");
				
				Double_t valMu = gausFit->GetParameter(1);
				//Double_t centerErr = gausFit->GetParError(1);
				Double_t valSig = gausFit->GetParameter(2);
				//Double_t sigmaErr = gausFit->GetParError(2);
				
				Double_t valAbs = abs->GetBinContent(j);
				Double_t valTot = tot->GetBinContent(j);
				//Double_t valMu = mu->GetBinContent(j);
				//Double_t valSig = sigma->GetBinContent(j);
				Double_t valnNegTracks = negtracks->GetBinContent(j);
				Double_t valnPions = pions->GetBinContent(j);
				
				gOneStripTimingAllDays[j-1]->AddPoint(iT, valMu - 2*valSig);
				gOneStripTimingAllDays[j-1]->AddPoint(iT, valMu + 2*valSig);
				//gOneStripTimingAllDays[j-1]->AddPoint(iT, valMu);
				//gOneStripTimingAllDays[j-1]->SetPointError(iT-1, 0.5, valSig);
				
				habsAllDays->SetBinContent(iT, j, valAbs);
				htotAllDays->SetBinContent(iT, j, valTot);
				
				if (valMu!=0.0 && TMath::Abs(valMu)!=fitRadius) hmuAllDays->SetBinContent(iT, j, valMu);
				else if (valMu==0.0 || TMath::Abs(valMu)==fitRadius) hmuAllDays->SetBinContent(iT, j, -999.0);
				
				if (valSig!=0.210 && valSig!=0.150 && valSig!=0.270) hsigAllDays->SetBinContent(iT, j, valSig);
				
				hnNegTracksAllDays->SetBinContent(iT, j, valnNegTracks/nEvts);
				hnPionsAllDays->SetBinContent(iT, j, valnPions/nEvts);
				
				habs->SetBinContent(i, j, valAbs);
				htot->SetBinContent(i, j, valTot);
				
				if (valMu!=0.0 && TMath::Abs(valMu)!=fitRadius) hmu->SetBinContent(i, j, valMu);
				else if (valMu==0.0 || TMath::Abs(valMu)==fitRadius) hmu->SetBinContent(i, j, -999.0);
				
				if (valMu!=0.0 && TMath::Abs(valMu)!=fitRadius) hmuDistributionAllDays->Fill(valMu, j);
				if (valMu!=0.0 && TMath::Abs(valMu)!=fitRadius) hmuDistribution->Fill(valMu, j);
			
				if (valSig!=0.210 && valSig!=0.150 && valSig!=0.270) hsig->SetBinContent(i, j, valSig);
				
				hnNegTracks->SetBinContent(i, j, valnNegTracks/nEvts);
				hnPions->SetBinContent(i, j, valnPions/nEvts);

				
				Int_t nBinsTimeDiff = projection->GetNbinsX();
				
				if (iT==1) {
					

				}
				
				if (i==1) hOneStripTimingAllDays[j-1]->GetXaxis()->SetBinLabel(iT, day[d]);
				
				for (Int_t k=0; k<nBinsTimeDiff; k++) { // loop over entries in tdiff histo for the given channel
					
					//cout << "In file " << iT << ", strip " << j << ", bin " << k+1 << ", tdiff = " << projection->GetBinContent(k+1) << endl;
					hOneStripTimingAllDays[j-1]->Fill(iT, projection->GetBinCenter(k+1), projection->GetBinContent(k+1));
					
				}
				
				fOut->cd(tit);
				projection->Write();
				
			}
			
			fIn->Close();	
			cout <<"\r" << i << " files analyzed in day " << day[d] << flush;
		}
	
		cout << "\n Done!" << endl;
		
		TString canvname = Form("timingMatrix_%s_%s_%s.pdf", day[d].Data(), timestamp.Data(), code.Data());
		
		TCanvas* cMu = new TCanvas ("" ,"", 1200, 800);
		cMu->SetGridy(1);
		//cMu->SetGridx(1);
		hmu->SetMinimum(-1.5);
		hmu->SetMaximum(1.5);
		hmu->Draw("colz");
		cMu->Print(canvname+"(");
		
		TCanvas* cSig = new TCanvas ("" ,"", 1200, 800);
		cSig->SetGridy(1);
		//cSig->SetGridx(1);
		hsig->Draw("colz");
		cSig->Print(canvname);
		
		TCanvas* cMuDist = new TCanvas ("" ,"", 1200, 800);
		cMuDist->SetGridy(1);
		//cMuDist->SetGridx(1);
		hmuDistribution->Draw("colz");
		cMuDist->Print(canvname);
		
		TCanvas* cNTracks = new TCanvas ("" ,"", 1200, 800);
		cNTracks->SetGridy(1);
		hnNegTracks->Draw("colz");
		cNTracks->Print(canvname);
		
		TCanvas* cNPions = new TCanvas ("" ,"", 1200, 800);
		cNPions->SetGridy(1);
		hnPions->Draw("colz");
		cNPions->Print(canvname);
		
		TCanvas* cAbs = new TCanvas ("" ,"", 1200, 800);
		cAbs->SetGridy(1);
		//cAbs->SetGridx(1);
		habs->Draw("colz");
		cAbs->Print(canvname);
		
		TCanvas* cTot = new TCanvas ("" ,"", 1200, 800);
		cTot->SetGridy(1);
		//cTot->SetGridx(1);
		htot->Draw("colz");
		cTot->Print(canvname+")");
		
		// save hists
		
		fOut->cd(day[d]);
		
		habs->Write();
		htot->Write();
		hmu->Write();
		hmuDistribution->Write();
		hsig->Write();
		hnNegTracks->Write();
		hnPions->Write();
		hStartMultMod0->Write();
		hStartMultMod1->Write();
		hStartMultMod3->Write();
	}
	

	TString canvname = Form("timingMatrix_%s-%s_%s_%s.pdf", day[0].Data(), day[nDays-1].Data(), timestamp.Data(), code.Data());
	TString canvnameOST = Form("timingMatrix_OST_%s-%s_%s_%s.pdf", day[0].Data(), day[nDays-1].Data(), timestamp.Data(), code.Data());
	
	TLine* line[nDays-1];
	TLine* lineMult[nDays-1];
	TLine* lineOST[nDays-1];
	Int_t cumDays = 0.0; //cumulated day index 
	for (Int_t d=0; d<nDays-1; d++) {
		
		cumDays+=nF[d];
		line[d] = new TLine (cumDays+0.5, 0.5, cumDays+0.5, 80.5);
		lineMult[d] = new TLine (cumDays+0.5, 0, cumDays+0.5, 50);
		lineOST[d] = new TLine (cumDays+0.5, -10, cumDays+0.5, 10);
		
		line[d]->SetLineStyle(2);
		line[d]->SetLineWidth(2);
		lineMult[d]->SetLineStyle(2);
		lineMult[d]->SetLineWidth(2);
		lineOST[d]->SetLineStyle(2);
		lineOST[d]->SetLineWidth(2);
		
	}
	
	TCanvas* cMuAllDays = new TCanvas ("" ,"", 1200, 800);
	cMuAllDays->SetGridy(1);
	//cMuAllDays->SetGridx(1);
	hmuAllDays->SetMinimum(-1.5);
	hmuAllDays->SetMaximum(1.5);
	hmuAllDays->Draw("colz");
	for (Int_t d=0; d<nDays-1; d++) line[d]->Draw("same");
	cMuAllDays->Print(canvname+"(");
	
	TCanvas* cSigAllDays = new TCanvas ("" ,"", 1200, 800);
	cSigAllDays->SetGridy(1);
	//cSigAllDays->SetGridx(1);
	hsigAllDays->Draw("colz");
	for (Int_t d=0; d<nDays-1; d++) line[d]->Draw("same");
	cSigAllDays->Print(canvname);
	
	TCanvas* cMuDistAllDays = new TCanvas ("" ,"", 1200, 800);
	cMuDistAllDays->SetGridy(1);
	//cMuDistAllDays->SetGridx(1);
	hmuDistributionAllDays->Draw("colz");
	cMuDistAllDays->Print(canvname);
	
	TCanvas* cAbsAllDays = new TCanvas ("" ,"", 1200, 800);
	cAbsAllDays->SetGridy(1);
	//cAbsAllDays->SetGridx(1);
	habsAllDays->Draw("colz");
	habsAllDays->SetMinimum(-20);
	habsAllDays->SetMaximum(20);
	for (Int_t d=0; d<nDays-1; d++) line[d]->Draw("same");
	cAbsAllDays->Print(canvname);
	
	TCanvas* cNTracksAllDays = new TCanvas ("" ,"", 1200, 800);
	cNTracksAllDays->SetGridy(1);
	hnNegTracksAllDays->Draw("colz");
	for (Int_t d=0; d<nDays-1; d++) line[d]->Draw("same");
	cNTracksAllDays->Print(canvname);
	
	TCanvas* cNPionsAllDays = new TCanvas ("" ,"", 1200, 800);
	cNPionsAllDays->SetGridy(1);
	hnPionsAllDays->Draw("colz");
	for (Int_t d=0; d<nDays-1; d++) line[d]->Draw("same");
	cNPionsAllDays->Print(canvname);
	
	TCanvas* cStartMultMod0 = new TCanvas ("" ,"", 1200, 800);
	cStartMultMod0->SetGridy(1);
	hStartMultMod0AllDays->Draw("colz");
	for (Int_t d=0; d<nDays-1; d++) lineMult[d]->Draw("same");
	cStartMultMod0->Print(canvname);
	
	TCanvas* cStartMultMod1 = new TCanvas ("" ,"", 1200, 800);
	cStartMultMod1->SetGridy(1);
	hStartMultMod1AllDays->Draw("colz");
	cStartMultMod1->SetLogz(kTRUE);
	for (Int_t d=0; d<nDays-1; d++) lineMult[d]->Draw("same");
	cStartMultMod1->Print(canvname);
	
	
	TCanvas* cStartMultMod3 = new TCanvas ("" ,"", 1200, 800);
	cStartMultMod3->SetGridy(1);
	hStartMultMod3AllDays->Draw("colz");
	cStartMultMod3->SetLogz(kTRUE);
	for (Int_t d=0; d<nDays-1; d++) lineMult[d]->Draw("same");
	cStartMultMod3->Print(canvname);
	
	TCanvas* cTotAllDays = new TCanvas ("" ,"", 1200, 800);
	cTotAllDays->SetGridy(1);
	//cTot->SetGridx(1);
	htotAllDays->Draw("colz");
	for (Int_t d=0; d<nDays-1; d++) line[d]->Draw("same");
	cTotAllDays->Print(canvname+")");
	
	gStyle->SetPalette(kCool);
	
	TCanvas* cOneStripTimingAllDays[80];
	
	for (Int_t k=0; k<80; k++) {
		
		cOneStripTimingAllDays[k] = new TCanvas (Form("ch%i", k+1) ,Form("ch%i", k+1), 1200, 800);
		//cTotAllDays->SetGridy(1);
		//cTot->SetGridx(1);
		hOneStripTimingAllDays[k]->Draw("colz");
		gOneStripTimingAllDays[k]->Draw("P same");
		gOneStripTimingAllDays[k]->SetMarkerStyle(20);
		gOneStripTimingAllDays[k]->SetMarkerSize(0.3);
		//gOneStripTimingAllDays[k]->SetLineColorAlpha(1, 0.2);
		//gOneStripTimingAllDays[k]->SetMarkerColorAlpha(1, 0.4);
		
		for (Int_t d=0; d<nDays-1; d++) lineOST[d]->Draw("same");
		
		if (k==0) cOneStripTimingAllDays[k]->Print(canvnameOST+"(");
		else if (k>0 && k<16) cOneStripTimingAllDays[k]->Print(canvnameOST);
		else if (k>=20 && k<36) cOneStripTimingAllDays[k]->Print(canvnameOST);
		else if (k>=60 && k<67) cOneStripTimingAllDays[k]->Print(canvnameOST);
		else if (k==67) cOneStripTimingAllDays[k]->Print(canvnameOST+")");
		else continue;
			
	}
	
	
	fOut->cd();
	
	habsAllDays->Write();
	htotAllDays->Write();
	hmuAllDays->Write();
	hmuDistributionAllDays->Write();
	hsigAllDays->Write();
	hnNegTracksAllDays->Write();
	hnPionsAllDays->Write();
	hStartMultMod0AllDays->Write();
	hStartMultMod1AllDays->Write();
	hStartMultMod3AllDays->Write();
	
	fOut->cd("OneStripTiming");
	for (Int_t j=1; j<=80; j++) cOneStripTimingAllDays[j-1]->Write();
	
	cout << "Saving and closing main ROOT file " << Form("timingMatrix_%s-%s_%s_%s.root", day[0].Data(), day[nDays-1].Data(), timestamp.Data(), code.Data()) << endl;
	
	fOut->Save();
	fOut->Close();

}
