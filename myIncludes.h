#ifndef __MYINCLUDES__
#define __MYINCLUDES__

#include "hades.h"
#include "hruntimedb.h"
#include "htask.h"

#include "htaskset.h"

#include "hevent.h"
#include "hcategory.h"
#include "hdst.h"
#include "htime.h"
#include "TH2F.h"
#include "TH2.h"
#include "TH1F.h"
#include "TH1D.h"
#include "TGaxis.h"
#include "TAxis.h"
#include "TGraph.h"
#include "TF1.h"
#include "TArrayD.h"
#include "TNtuple.h"
#include "hiterator.h"


#include "haddef.h"
#include "richdef.h"
#include "hmdcdef.h"
#include "hmdctrackddef.h"
#include "hmdctrackgdef.h"
#include "hmetamatch2.h"
#include "hsplinetrack.h"
#include "showerdef.h"
#include "rpcdef.h"
#include "tofdef.h"
#include "walldef.h"
#include "hloop.h"
#include "hcategorymanager.h"
#include "htofcluster.h"
#include "htofhit.h"
#include "hrpccal.h"
#include "hrpchit.h"
#include "hstartdef.h"
#include "hstart2cal.h"
#include "hstart2hit.h"
#include "hparticlecand.h"

#include "hrpccluster.h"
#include "hlocation.h"
#include "hphysicsconstants.h"

#include "hrpccalpar.h"
#include "hstart2calpar.h"
#include "hparasciifileio.h"

#include "TSystem.h"
#include "TROOT.h"
#include "TFile.h"
#include "TString.h"
#include "TStopwatch.h"
#include "TString.h"
#include "TList.h"
#include "TCanvas.h"
#include "TStyle.h"
#include "TGraphErrors.h"

#include "TCutG.h"
#include "hparticletracksorter.h"

#include <iostream>
#include <fstream>

void fillOffsets (TH1F* hOffsets, TH2F* hCharge, Int_t nCells) {
	
	for (Int_t i=0; i<nCells; i++) { // loop over cells
	
		// cout << "Calculating offset for cell with index " << i << endl; 
	
		// get appropriate projection
		
		TH1F* projection = (TH1F*) hCharge->ProjectionY("projection", i+1, i+2);
		Int_t nBinsOfCharge = projection->GetNbinsX();
		
		Float_t chargeMax = projection->GetMaximum();
		Float_t chargeThreshold = 0.2 * chargeMax;
		Int_t currentBin = 1;
		
		while (currentBin < 100000) { // this should never happen but it prevents an infinite loop
		
			if (projection->GetBinContent(currentBin) >= chargeThreshold &&
			projection->GetBinContent(currentBin+1) > chargeThreshold &&
			projection->GetBinContent(currentBin+2) > chargeThreshold &&
			projection->GetBinContent(currentBin+3) > chargeThreshold &&
			projection->GetBinContent(currentBin+4) > chargeThreshold &&
			projection->GetBinContent(currentBin+5) > chargeThreshold &&
			projection->GetBinContent(currentBin+6) > chargeThreshold &&
			projection->GetBinContent(currentBin+7) > chargeThreshold &&
			projection->GetBinContent(currentBin+8) > chargeThreshold) {
				
				//cout << "Found offset in cell " << i << endl;
				hOffsets->SetBinContent(i+1, projection->GetBinCenter(currentBin));
				break;
				
			}
			
			currentBin++;
			
			if (currentBin > nBinsOfCharge) {
				
				cout << "WARNING! Offset not found in cell " << i << endl;
				hOffsets->SetBinContent(i+1, 0.0);
				break;
				
			}
		}
	}
}

void fillChargeOffsets (TH1F* hOffsets, TH2F* hCharge, Int_t nCells) {
	
	const Double_t defaultOffset = 5000.0; // could be also 0.0 but setting to an absurd number is recommended
	
	for (Int_t i=0; i<nCells; i++) { // loop over cells
	
		// cout << "Calculating offset for cell with index " << i << endl; 
	
		// get appropriate projection
		
		TH1F* projection = (TH1F*) hCharge->ProjectionY("projection", i+1, i+1);
		Int_t nBinsOfCharge = projection->GetNbinsX();
		
		Float_t chargeMax = projection->GetMaximum();
		Float_t chargeThreshold = 0.2 * chargeMax;
		Int_t currentBin = 1;
		
		while (currentBin < 100000) { // this should never happen but it prevents an infinite loop
		
			if (projection->GetBinContent(currentBin) >= chargeThreshold &&
			projection->GetBinContent(currentBin+1) > chargeThreshold &&
			projection->GetBinContent(currentBin+2) > chargeThreshold &&
			projection->GetBinContent(currentBin+3) > chargeThreshold &&
			projection->GetBinContent(currentBin+4) > chargeThreshold &&
			projection->GetBinContent(currentBin+5) > chargeThreshold &&
			projection->GetBinContent(currentBin+6) > chargeThreshold &&
			projection->GetBinContent(currentBin+7) > chargeThreshold &&
			projection->GetBinContent(currentBin+8) > chargeThreshold) {
				
				//cout << "Found offset in cell " << i << endl;
				hOffsets->SetBinContent(i+1, projection->GetBinLowEdge(currentBin));
				break;
				
			}
			
			currentBin++;
			
			if (currentBin > nBinsOfCharge) {
				
				cout << "WARNING! Offset not found in cell " << i << endl;
				hOffsets->SetBinContent(i+1, defaultOffset);
				break;
				
			}
		}
	}
}

void fillChargeOffsetsAdvanced (TH1F* hOffsets, TH2F* hCharge, Float_t chargeThresholdFraction, Float_t diffFraction, Int_t nConsecBins, Int_t nCycles) {
	
	const Int_t adjustOffset = 0; // adjust which bin to take, when the condition is satisfied (for low thresholds adding 1 or 2 may be necessary)
	
	for (Int_t i=0; i<hCharge->GetNbinsX(); i++) { // loop over cells
		
		//if (i!=101) continue; // select one cell to look at couts
		
		Double_t offset = 5000.0; // could be also 0.0 but setting the default to an absurd number is better
		TH1F* projection = (TH1F*) hCharge->ProjectionY("projection", i+1, i+1);
		Float_t chargeMax = projection->GetMaximum();
		
		for (Int_t c=1; c<=nCycles; c++) {
			
			//cout << "Starting cycle number " << c << endl;
			Float_t chargeThreshold = chargeThresholdFraction * chargeMax / c; // divide threshold by number of cycle
			Bool_t foundOffset = kFALSE;
			
			for (Int_t j=0; j<projection->GetNbinsX(); j++) {
				for (Int_t k=0; k<nConsecBins; k++) {
					
					// condidtions to meet: this bin is over threshold and difference to next been is less than the given fraction  
					if ( projection->GetBinContent(j+k+1) < chargeThreshold ) {
						//cout << "Bin content " << projection->GetBinContent(j+k+1) << " under threshold of " << chargeThreshold << endl;
						break;
					}
	
					if (abs(projection->GetBinContent(j+k+1) - projection->GetBinContent(j+k+2))/projection->GetBinContent(j+k+1) > diffFraction) {
						//cout << "Rel. ifference to next bin " << abs(projection->GetBinContent(j+k+1) - projection->GetBinContent(j+k+2))/projection->GetBinContent(j+k+1) << " exceeds limit of " << diffFraction << endl;
						break; 
					}
					
					if (k != nConsecBins-1) continue; //don't fill offsets until ALL k bins satisfied the threshold
					
					offset = projection->GetBinLowEdge(j+1+adjustOffset);
					cout << "Setting offset = " << offset << " in bin " << i+1 << " ... \n";
					foundOffset = kTRUE;
					j = projection->GetNbinsX(); //break the j loop
					c = nCycles; // break the c loop
				}
				
				if (j == projection->GetNbinsX()-1 && !foundOffset) cout << "WARNING! Offset not found in bin " << i+1 << " !\n";
				
			}
		}
		
		hOffsets->SetBinContent(i+1, offset);
					
	}
}

void fillTimeAndPosOffsets (TH1F* hOffsets, TH1F* hFitSigma, TH1F* hFitChiSquare, TH2F* hSource, Double_t fitRadius, TFile* out, TString dirname) {
	
	out->cd();
	out->mkdir(dirname);
	out->cd(dirname);
	
	const Int_t nCells = 192;
	
	for (Int_t i=0; i<nCells; i++) { // loop over cells
	
		// cout << "Calculating offset for cell with index " << i << endl; 
	
		// get appropriate projection
		
		TH1F* projection = (TH1F*) hSource->ProjectionY(Form("projection_%i", i+1), i+1, i+1);
		Int_t nBinsOfSource = projection->GetNbinsX();
		
		// set fit range
		
		Double_t positionOfMaximum = 3.6; // ugly hardcoded part - look for pions!!!
		//Double_t positionOfMaximum = projection->GetBinCenter(projection->GetMaximumBin());
		Double_t fitLoEdge = positionOfMaximum - fitRadius;
		Double_t fitHiEdge = positionOfMaximum + fitRadius;
		//std::cout << "Gaus will be fitted in range [" << fitLoEdge << ", " << fitHiEdge << "] \n";
		
		// fit and fill offset
		
		TF1* gausFit = new TF1 (Form("fit_%i", i), "gausn", fitLoEdge, fitHiEdge);
		gausFit->SetParLimits(1, positionOfMaximum - 0.5*fitRadius, positionOfMaximum + 0.5*fitRadius);
		gausFit->SetRange(fitLoEdge, fitHiEdge);
		projection->Fit(gausFit, "RQ");
		
		Double_t center = gausFit->GetParameter(1);
		Double_t sigma = gausFit->GetParameter(2);
		Double_t ndf = gausFit->GetNDF();
		Double_t chi = 0.0;
		if (ndf != 0.0) Double_t chi = gausFit->GetChisquare() / ndf;
		
		hOffsets->SetBinContent(i+1, center);
		hFitSigma->SetBinContent(i+1, sigma);
		hFitChiSquare->SetBinContent(i+1, chi);
		projection->Write();
		

	}
}


void sayHello () {
	
	cout << "\n \n \n" << endl;
	cout << "  #    #        #   #            #" << endl;
	cout << "  #    #        #   #            #" << endl;
	cout << "  #    #       #   #   ####     # " << endl;
	cout << " ######  ##    #   #  #    #    # " << endl;
	cout << " #    # #  #  #   #   #    #   #  " << endl;
	cout << " #    # ###   #   #   #    #   #  " << endl;
	cout << "#    #  #    # # # # ##   #       " << endl;
	cout << "#    #   ####   #   #  ###    #   " << endl;
	cout << "\n \n \n" << endl;
	gSystem->Sleep(2000);
}

/*
Bool_t skipThisChannel (Int_t channel) {
	
	Bool_t result = false;
	
	//if (channel == 1) result = true; // delete this
	if (channel == 5) result = true;
	if (channel == 6) result = true;
	//if (channel == 7) result = true; // delete this
	if (channel == 11) result = true;
	if (channel == 12) result = true;
	if (channel == 17) result = true;
	if (channel == 18) result = true;
	if (channel == 19) result = true;
	if (channel == 20) result = true;
	//if (channel == 21) result = true; //delete this
	if (channel == 25) result = true;
	if (channel == 26) result = true;
	//if (channel == 27) result = true; //delete this
	if (channel == 31) result = true;
	if (channel == 32) result = true;
	if (channel > 36) result = true;
	
	return result;
}
*/

Double_t timeWalkFuncWithSqrt (Double_t *xarg, Double_t *par) {
	
	Double_t x = xarg[0];
	Double_t result = 0.0;
	
	//result = par[0] + par[1]*pow(x, - 0.5);
	result = par[0] + par[1]*pow(x, - (1./3.));
	
	return result;
	
}

Double_t timeWalkFuncLinear (Double_t *xarg, Double_t *par) {
	
	Double_t x = xarg[0];
	Double_t result = 0.0;
	
	result = par[0] + par[1] * x;
	
	return result;
	
}


Double_t gaussianWithConstBckg (Double_t *xarg, Double_t *par) {
	
	Double_t x = xarg[0];
	Double_t result = 0.0;
	Double_t pi = TMath::Pi();
	
	result = par[0]*exp(-0.5*(((x-par[1])/par[2])*(x-par[1])/par[2])/(sqrt(2*pi)*par[2])) + par[3];
	
	return result;
	
}

void fillTimeOffsets (TH1F* hOffsets, TH2F* hSource, Int_t firstBinToFill, Int_t lastBinToFill, TFile* out, TString dirname) {
	
	out->cd();
	out->mkdir(dirname);
	out->cd(dirname);
	
	const Float_t fitRadius = 0.5;
	
	for (Int_t i=firstBinToFill; i<=lastBinToFill; i++) { // loop over channels
	
		// cout << "Calculating offset for cell with index " << i << endl; 
		// get appropriate projection
		
		TH1F* projection = (TH1F*) hSource->ProjectionY(Form("projection_"+dirname+"_ch%i", i), i, i);
		Int_t nEntrOfSource = projection->GetEntries();
		
		// set fit range
		
		Double_t positionOfMaximum = projection->GetBinCenter(projection->GetMaximumBin());
		Double_t heightOfMaximum = projection->GetMaximum();
		//Double_t positionOfMaximum = 0.0;
		Double_t fitLoEdge = positionOfMaximum - fitRadius;
		Double_t fitHiEdge = positionOfMaximum + fitRadius;
		std::cout << "Gaus will be fitted in range [" << fitLoEdge << ", " << fitHiEdge << "] \n";
		
		// fit and fill offset
		
		TF1* gausFit = new TF1 (Form("fit_%i", i), gaussianWithConstBckg, fitLoEdge, fitHiEdge, 4);
		
		gausFit->SetParameter(0, 1000);
		//gausFit->SetParLimits(0, 0.1*heightOfMaximum, 1000000.0);
		
		gausFit->SetParameter(1, 0.0);
		gausFit->SetParLimits(1, fitLoEdge + 0.25*fitRadius, fitHiEdge-0.25*fitRadius);
		
		gausFit->SetParameter(2, 1.0);
		gausFit->SetParLimits(2, 0.0, 0.5*fitRadius);
		
		
		gausFit->FixParameter(3, 0.0);
		//gausFit->SetParameter(3, 0.0);
		//gausFit->SetParLimits(3, 0.0, 0.05*heightOfMaximum);
		
		
		gausFit->SetRange(fitLoEdge, fitHiEdge);
		projection->Fit(gausFit, "WR");
		
		Double_t center = gausFit->GetParameter(1);
		if (nEntrOfSource>11000) hOffsets->SetBinContent(i, center);
		else hOffsets->SetBinContent(i, 0.0);
		
		projection->Write();
		

	}
}

Float_t GetMinTofCand() {
	
	// from Gosia
	
    HCategory* hcParticleCand = HCategoryManager::getCategory(catParticleCand);

    if (!hcParticleCand) {
    cerr << "Could not read the required category ParticleCand" << endl;
    return -10000.;
    }

    //---------------------------------------------------------------------------------
    map<Int_t, Float_t> tMaps[2];

    //---------------------------------------------------------------------------------
    for (Int_t p = 0; p < hcParticleCand->getEntries(); p++) {
    HParticleCand* hpc = (HParticleCand*) hcParticleCand->getObject(p);

    if (hpc->getSystemUsed() < 0 || hpc->getSystemUsed() > 1)
        continue;

    Int_t i = hpc->getSystemUsed() ? hpc->getTofHitInd() : hpc->getRpcInd();
    Float_t d = hpc->getDistanceToMetaHit(), t = hpc->getTof();

    if (t < 0. || t > 300. || d > 3000.)
        continue;

    t *= 2100. / d;

    if (tMaps[hpc->getSystemUsed()].count(i) == 0 || (tMaps[hpc->getSystemUsed()].count(i) == 1 && tMaps[hpc->getSystemUsed()][i] > t))
        tMaps[hpc->getSystemUsed()][i] = t;
    }

    //---------------------------------------------------------------------------------
    vector<Float_t> tList;

    //---------------------------------------------------------------------------------
    for (Int_t i = 0; i < 2; i++)
    for (auto it = tMaps[i].begin(); it != tMaps[i].end(); it++)
        tList.push_back(it->second);

    //---------------------------------------------------------------------------------
    if (tList.size() == 0)
    return -10000.;

    std::sort(tList.begin(), tList.end());

    //---------------------------------------------------------------------------------
    Float_t sum = 0.;
    for (Int_t i = 0; i < TMath::Min(3, (Int_t) tList.size()); i++)
    sum += tList[i];

    //---------------------------------------------------------------------------------
    return (sum / TMath::Min(3, (Int_t) tList.size()));
}


#endif
