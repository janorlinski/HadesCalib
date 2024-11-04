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

#include "htofhitsim.h"
#include "hrpchitsim.h"
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
#include "hparticlestart2hitf.h"
#include "hstart2clusterfinder.h"
#include "rpcdef.h"
#include "hrpcraw.h"
#include "hrpccal.h"
#include "hrpchit.h"
#include "hrpccluster.h"
#include "hstart2cal.h"
//#include "hstart2clusterfinder.h"

#include <iostream>
#include <fstream>


//global RPC variables
Int_t nCells = 192;
Float_t loLimitCells = -0.5;
Float_t hiLimitCells = nCells + loLimitCells;

//global constants for START setup variables
const Int_t nModules = 4;
const Int_t nChPerMod = 20;
const Int_t nChPerModReal = 16;
const Int_t nChannels = nModules*nChPerMod;
const Float_t loLimitChannels = 0.5;
const Float_t hiLimitChannels = nChannels + loLimitChannels;

const Int_t nHistForTimeWalk = 2;

const Float_t tDiffEntriesThreshold = 10000;

const Float_t loTimeLimitForMultiplicity = -5.0; // ns
const Float_t hiTimeLimitForMultiplicity = 5.0; // ns

const Int_t nPointsForTimeWalk = 17;
const Float_t widthLoLimitTW = 2.0;
const Float_t widthHiLimitTW = 19.0;
const Float_t widthBinWidth = (widthHiLimitTW - widthLoLimitTW)/nPointsForTimeWalk;

const Float_t widthLoLimit = 0.0;
const Float_t widthHiLimit = 15.0;

const Float_t tDiffLoLimit = -10.0;
const Float_t tDiffHiLimit =  10.0;

const Int_t nBinsForWidth = 200;
const Int_t nBinsForTimeDiff = 400;

//const Int_t refCh[4] = {9, 23, 42, 62}; //c+c
const Int_t refCh[4] = {11, 28, 42, 67}; //au+au
 
//const Float_t refWidthLo[2] = {15.4, 12.6};
//const Float_t refWidthHi[2] = {16.4, 13.3};

const Float_t refWidthLo[2] = {widthLoLimit, widthLoLimit};
const Float_t refWidthHi[2] = {widthHiLimit, widthHiLimit};

const Float_t clusteringTimeWindow = 5.0; // ns
const Float_t clusteringStripWindow = 16; // 16 strips difference

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

void fillPosOffsets (TH1F* hOffsets, TH1F* hFitSigma, TH1F* hFitChiSquare, TH2F* hSource, Double_t fitRadius, TFile* out, TString dirname) {
	
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
		
		Double_t positionOfMaximum = 0.0; // ugly hardcoded part - look for pions!!!
		//Double_t positionOfMaximum = projection->GetBinCenter(projection->GetMaximumBin());
		Double_t fitLoEdge = positionOfMaximum - fitRadius;
		Double_t fitHiEdge = positionOfMaximum + fitRadius;
		//std::cout << "Gaus will be fitted in range [" << fitLoEdge << ", " << fitHiEdge << "] \n";
		
		// fit and fill offset
		
		TF1* gausFit = new TF1 (Form("fit_%i", i), "gausn", fitLoEdge, fitHiEdge);
		gausFit->SetParLimits(1, positionOfMaximum - 0.5*fitRadius, positionOfMaximum + 0.5*fitRadius);
		gausFit->SetParLimits(2, 0.1, 0.3); //sigma must be rather narrow, to not fit in the bckground region
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

void fillTimeOffsets (TH1F* hOffsets, TH1F* hFitSigma, TH1F* hFitChiSquare, TH2F* hSource, Double_t fitRadius, TFile* out, TString dirname) {
	
	out->cd();
	out->mkdir(dirname);
	out->cd(dirname);
	
	const Int_t nCells = 192;
	
	for (Int_t i=0; i<nCells; i++) { // loop over cells
	
		// cout << "Calculating offset for cell with index " << i << endl; 
	
		// get appropriate projection
		
		TH1F* projection = (TH1F*) hSource->ProjectionY(Form("projection_%i", i+1), i+1, i+1);
		Int_t nBinsOfSource = projection->GetNbinsX();
		
		Bool_t firstMaxFound = false;
		Double_t positionOfMaximum = 0.0; 
		Double_t positionOfFirstMaximum = 0.0; 
		Double_t maxContent = 0.0;
		for (Int_t bin=1; bin<=nBinsOfSource; bin++) {
			
			Double_t currentContent = projection->GetBinContent(bin);
			
			if (currentContent >=  maxContent) { // standard max searching scan
				maxContent = currentContent;
				positionOfMaximum = projection->GetBinCenter(bin);
			}
			
			//else if (currentContent/maxContent < 0.60 && currentContent > 40.0) break; //cc
			else if (currentContent/maxContent < 0.75 && currentContent > 0.05*projection->GetMaximum() && positionOfMaximum!=positionOfFirstMaximum) { //auau, complicated mostly due to double structure - could be removed by clustering
																																						//there is a change in philosophy: now we want to ALWAYS find the false peak, to ignore it :)
				if (firstMaxFound) bin=100000; //alternative to breaking, since it will only break this if
				else {
					firstMaxFound = true;
					positionOfFirstMaximum = positionOfMaximum;
				}
			}
			
			//cout << "Current content = " << currentContent << " and is " << currentContent/maxContent << " of maximum" << endl;
			
			/*
			if (bin==nBinsOfSource) { // if the break failed, return to some safe conditions
				cout << "No matching maximum found! Setting radius to -2 : 2" << endl;
				positionOfMaximum=0.0;
				fitRadius=2.0;
			}
			*/
			
		}
		
		Double_t fitLoEdge = positionOfMaximum - fitRadius;
		Double_t fitHiEdge = positionOfMaximum + fitRadius;
		std::cout << "Gaus will be fitted in range [" << fitLoEdge << ", " << fitHiEdge << "] \n";
		
		// fit and fill offset
		
		TF1* gausFit = new TF1 (Form("fit_%i", i), "gausn", fitLoEdge, fitHiEdge);
		
		gausFit->SetParameter(0, 1000);
		gausFit->SetParameter(1, positionOfMaximum);
		gausFit->SetParameter(0, 0.160);
		
		gausFit->SetParLimits(0, 20, 200000000);
		gausFit->SetParLimits(1, -2, 2);
		gausFit->SetParLimits(2, 0.1, 0.35); //sigma must be rather narrow, to not fit in the bckground region
		gausFit->SetRange(fitLoEdge, fitHiEdge);
		
		projection->Fit(gausFit, "WRQ"); // all weights = 1 option on this time
		
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

Float_t findMinimumTime (Int_t numOfMetaHits) {
	// copied from hydra 2.6-8 and adapted to work as a standalone function
	
    // Function which loops inside tof and rpc hits, find the 3 hits
    // with minimum time-of-flight, and returns the average value -> fastest signal
    
	HCategory*     fCatTof;      //! pointer to the tof hit data
	HCategory*     fCatRpc;      //! pointer to the rpc hit data
    HTofHit* pTofHit = NULL;
    HRpcHit* pRpcHit = NULL;
    
    fCatTof = gHades->getCurrentEvent()->getCategory(catTofHit);
    if (NULL == fCatTof) {
	Warning("init", "HTofHit category not available!");
    }

    fCatRpc = gHades->getCurrentEvent()->getCategory(catRpcHit);
    if (NULL == fCatRpc) {
	Warning("init", "HRpcHit category not available!");
    }

    vector<Float_t> tofList, rpcList, allList; // lists of time-of-flight

    //--------------------------------------------------------------------
    // loop in HTofHit category
    if(fCatTof) {
	for (Int_t tofEntry = 0; tofEntry < fCatTof->getEntries(); ++tofEntry)
	{
	    if (NULL == (pTofHit = static_cast<HTofHit*>(fCatTof->getObject(tofEntry)))) {
		Error("findMinimumTime", "Pointer to HTofHit object == NULL, returning");
		return -1;
	    }

	    //--------------------------------------------------------------------
            // skipp the embedded sim tracks
	    if(gHades->getEmbeddingMode()>0){
		HTofHitSim* pTofHitSim = 0;
		pTofHitSim = dynamic_cast<HTofHitSim*>(pTofHit);
		if(pTofHitSim){
                    if(pTofHitSim->getNTrack1() > 0 || pTofHitSim->getNTrack2() > 0) continue;
		}
	    }
	    //--------------------------------------------------------------------


	    Float_t tofTof = pTofHit->getTof();
	    if ( (tofTof < 0) || (tofTof > 300) ) continue; // cuts too low or high tof values

	    Float_t distTof = 0;
	    pTofHit->getDistance(distTof);
	    if (distTof > 3000) continue; // cuts not feasible distances

	    tofList.push_back(2100. * tofTof / distTof);  // fill with normalized tof to 2100 mm
	    allList.push_back(2100. * tofTof / distTof);  // fill with normalized tof to 2100 mm
	    
	    //if (bDebug) nt1->Fill(gHades->getEventCounter(), 1, tofTof, distTof, 2100. * tofTof / distTof); commented bc not necessary (?) and would require further definitions
	    
	} // end of HTofHit loop
    }
    //--------------------------------------------------------------------

    //--------------------------------------------------------------------
    // loop in RpcHit category
    Int_t tracksL[10];
    Int_t tracksR[10];

    if(fCatRpc){
	for (Int_t rpcEntry = 0; rpcEntry < fCatRpc->getEntries(); ++rpcEntry)
	{
	    if (NULL == (pRpcHit = static_cast<HRpcHit*>(fCatRpc->getObject(rpcEntry)))) {
		Error("execute", "Pointer to HRpcHit object == NULL, returning");
		return -1;
	    }

	    //--------------------------------------------------------------------
            // skipp the embedded sim tracks
	    if(gHades->getEmbeddingMode()>0){
		HRpcHitSim* pRpcHitSim = 0;
		pRpcHitSim = dynamic_cast<HRpcHitSim*>(pRpcHit);
		if(pRpcHitSim){
		    pRpcHitSim->getTrackLArray(tracksL);
                    pRpcHitSim->getTrackRArray(tracksR);
                    Bool_t simTrack=kFALSE;
		    for(Int_t i = 0; i < 10; i++){
			if(tracksL[i] > 0 || tracksR[i] > 0) { simTrack = kTRUE; break;}
		    }
                    if(simTrack) continue;
		}
	    }
	    //--------------------------------------------------------------------

	    Float_t tofRpc = pRpcHit->getTof();
	    if ( (tofRpc < 0) || (tofRpc > 300) ) continue; // cuts too low or high tof values

	    Float_t xRpc = 0., yRpc = 0., zRpc = 0.;
	    pRpcHit->getXYZLab(xRpc, yRpc, zRpc);
	    Float_t distRpc = TMath::Sqrt(xRpc * xRpc + yRpc * yRpc + zRpc * zRpc);
	    if (distRpc > 3000) continue; // cuts not feasible distances

	    rpcList.push_back(2100. * tofRpc / distRpc);  // fill with normalized tof to 2100 mm;
	    allList.push_back(2100. * tofRpc / distRpc);  // fill with normalized tof to 2100 mm;
	    
	    // if (bDebug) nt1->Fill(gHades->getEventCounter(), 0, tofRpc, distRpc, 2100. * tofRpc / distRpc); commented, see above
	    
	} // end of HRpcHit loop
    }
    //--------------------------------------------------------------------

    if (allList.size() == 0) return -1; // if no good time, returns a negative value

    //--------------------------------------------------------------------
    // sort tof lists from minimum to maximum value
    std::sort(tofList.begin(), tofList.end());
    std::sort(rpcList.begin(), rpcList.end());
    std::sort(allList.begin(), allList.end());
    //--------------------------------------------------------------------


    //--------------------------------------------------------------------
    // select the three tof values with minimum time,
    // and calculate the average, for tof rpc and both
    Float_t tofSum   = 0, rpcSum   = 0, allSum   = 0;
    Float_t tofMean  = 0, rpcMean  = 0, allMean  = 0;
    Int_t   tofCount = 0, rpcCount = 0, allCount = 0;
    for (vector<Float_t>::iterator it = tofList.begin(); it != tofList.end(); ++it) {
	if (tofCount < numOfMetaHits) {
	    tofSum += *it;
	    tofCount++;
	}
    }
    if (tofCount > 0) tofMean = tofSum / tofCount;

    for (vector<Float_t>::iterator it = rpcList.begin(); it != rpcList.end(); ++it) {
	if (rpcCount < numOfMetaHits) {
	    rpcSum += *it;
	    rpcCount++;
	}
    }
    if (rpcCount > 0) rpcMean = rpcSum/rpcCount;

    for (vector<Float_t>::iterator it = allList.begin(); it != allList.end(); ++it) {
	if (allCount < numOfMetaHits) {
	    allSum += *it;
	    allCount++;
	}
    }
    if (allCount > 0) allMean = allSum / allCount;
    //--------------------------------------------------------------------

    //if (bDebug) nt2->Fill(gHades->getEventCounter(), rpcList.size(), tofList.size(), tofMean, rpcMean, allMean, tofCount, rpcCount, allCount);

    return allMean; // returns the minimum time from tof+rpc
}

Float_t findEvtStartTime () {
	
	
    HCategory* Start2HitCategory;
    HStart2Hit* Start2HitObject;
    
	Start2HitCategory = HCategoryManager::getCategory(catStart2Hit);
	
	
	HIterator* iterStart2Hit = (HIterator *) Start2HitCategory->MakeIterator("native");
	
	// from qamaker
	
	// first step is to get global timing information
	// there is only one "time 0" and is located in
	// Start2Hit category
	Float_t startTime  = 0.0;   // put here 0.0 as default, means no start HIT 
	Int_t   startStrip = -1;
	Int_t   startStrip1 = -1;
	
	if (iterStart2Hit) {
	  iterStart2Hit->Reset();
	  while (NULL != (Start2HitObject = static_cast<HStart2Hit*>(iterStart2Hit->Next()))) {
		 if (Start2HitObject->getModule() == 0 || Start2HitObject->getModule() == 1) {
			startStrip = Start2HitObject->getStrip();
			startStrip1 = Start2HitObject->getModule()*20+Start2HitObject->getStrip();
			startTime  = Start2HitObject->getTime();
			//cout << "getting startTime = " << startTime << endl;
		 }
	  }  //end of start iteration
	} // if iterStart
	
	// end of from qamaker
	
	return startTime;
	
}

Int_t getBinForTimeWalk (Float_t width) {
	
	// this function will give you the bin corresponding to the given width
	// it needs to know global constants: npoints, lo and hi limit for width
	
	/*
const Int_t nPointsForTimeWalk = 15;
const Float_t widthLoLimitTW = 0.0;
const Float_t widthHiLimitTW = 15.0;
const Float_t widthBinWidth = (widthHiLimitTW - widthLoLimitTW)/nPointsForTimeWalk;
	*/
	
	if (width < widthLoLimitTW || width > widthHiLimitTW) return -1;
	Int_t bin = Int_t((width - widthLoLimitTW)/widthBinWidth);
	//cout << "this is the getBinForTimeWalk function returning bin "<< bin << " for width = " << width << endl;
	return bin;
	
}

Int_t getRefModule(Int_t channel) {
	
	//ugly hardcoded function i know
	
	//Int_t module = -1;
	//if (channel >=1 && channel <= nChPerMod) module = 1;
	//else if (channel >=nChPerMod+1 && channel <= 2*nChPerMod) module = 0;
	//cout << "getRefModule returning " << module << " for channel = " << channel << endl;
	
	if (channel >= 60) return 0;
	else return 3;
	
	//return module;
}

Bool_t skipThisChannel (Int_t channel) {
	
	return false; // for now the function is overridden with a false, it is safer to have empty histograms, than to blindly skip them...
	
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
	if (channel == 21) result = true; //delete this //uncommented on 18.03
	if (channel == 22) result = true; //added on 18.03
	if (channel == 25) result = true;
	if (channel == 26) result = true;
	//if (channel == 27) result = true; //delete this
	if (channel == 31) result = true;
	if (channel == 32) result = true;
	if (channel > 36) result = true;
	
	return result;
}

Bool_t checkVectorDiffs (vector<Float_t> vec, Float_t test, Float_t window) {
	
	Bool_t result = false;
	Int_t n = vec.size();
	if(n=0) {
		cout << "Size 0 in checkVectorDiffs! Returning false! \n";
		return false;
	}
	
	for(Int_t i=0; i<n; i++) {
		
		if (fabs(vec[i]-test)<=window) result = true;
		
	}
	
	return result;
	
}

Int_t findMax (vector<Float_t> vec) {
	
		Int_t imax = 0;
		Int_t n = vec.size();
		Float_t max = 0.0;
		
		for (Int_t i=0; i<n; i++) {
			
			if (vec[i]>max) {
				
				imax=i;
				max=vec[i];
				
			}
			
		}
		
		return imax;
	
}

class JansClusterFinder {
	
	protected:
	
		Float_t timeWindow = 5.0;
	
		vector<Int_t> hitModules;
		vector<Int_t> hitStrips;
		vector<Float_t> hitTots;
		vector<Float_t> hitTimes;
		
		vector<Int_t> clusterModules;
		vector<Int_t> clusterStrips;
		vector<Float_t> clusterTots;
		vector<Float_t> clusterTimes;
		vector<Float_t> clusterSizes;
		
	public:

		JansClusterFinder (Float_t tw = 5.0) {
			
			JansClusterFinder::timeWindow = tw;
			
		}
		
		~JansClusterFinder() {}
		
		void addHit(Int_t module, Int_t strip, Float_t ToT, Float_t time) {
			
			hitModules.push_back(module);
			hitStrips.push_back(strip);
			hitTots.push_back(ToT);
			hitTimes.push_back(time);
			
		}
		
		void findClusters() {
			
			Int_t nHits = hitModules.size();
			
			if (nHits<1) {
				cout << "not enough hits to look for clusters !!!" << endl;
				return;
			}
			
			for (Int_t i = 0; i<nHits; i++) {
			
				// get empty vectors for a cluster candidate
				vector<Int_t> clusterCandidateModules;
				vector<Int_t> clusterCandidateStrips;
				vector<Float_t> clusterCandidateTots;
				vector<Float_t> clusterCandidateTimes;
				Int_t clusterCandidateSize = 0;
				
				// choose a time ref and iterate over all hits, add matching ones to the cluster candidate vector
				Float_t refTime = 0.0;
				if (hitTimes[i]!=-9999.0) refTime = hitTimes[i];
				else continue;
				
				while(true) { // actually - keep iterating over all the hits, until you find a match
					
					Int_t addedHits = 0;
					
					for (Int_t j = 0; j<nHits; j++) {
						
						if ((fabs(refTime-hitTimes[j])<timeWindow || 
						checkVectorDiffs(clusterCandidateTimes, hitTimes[j], timeWindow)) && 
						hitTimes[j]!=-9999.0) { // include a hit that simply fits with our ref time, or that matches with any of the hits in cluster candidate, reject hits already included in a cluster candidate
							
							clusterCandidateModules.push_back(hitModules[j]);
							clusterCandidateStrips.push_back(hitStrips[j]);
							clusterCandidateTots.push_back(hitTots[j]);
							clusterCandidateTimes.push_back(hitTimes[j]);
							clusterCandidateSize++;
							addedHits++;
							
							// if a hit was added to a cluster - delete it from available hits
							hitModules[j]=-9999.0;
							hitStrips[j]=-9999.0;
							hitTots[j]=-9999.0;
							hitTimes[j]=-9999.0;
							
						}
						
					}
					
					if (addedHits == 0) break;
					
				}
				
				Int_t imax = findMax(clusterCandidateTots); // find the hit with max tot within this cluster
				
				clusterModules.push_back(clusterCandidateModules[imax]);
				clusterStrips.push_back(clusterCandidateStrips[imax]);
				clusterTots.push_back(clusterCandidateTots[imax]);
				clusterTimes.push_back(clusterCandidateTimes[imax]);
				clusterSizes.push_back(clusterCandidateSize);
				
				
				//cout << "Found cluster with size " << clusterCandidateSize << ", tot=" << clusterCandidateTots[imax] << ", time=" << clusterCandidateTimes[imax] << endl;
				
			}
			
		}
		
		Int_t getNumberOfClusters () {return clusterModules.size();}
		Int_t getClusterSize (Int_t i) {if (clusterModules.size()>0) return clusterSizes[i]; else return -9999;}
		Float_t getClusterModule (Int_t i) {if (clusterModules.size()>0) return clusterModules[i]; else return -9999.0;}
		Float_t getClusterStrip (Int_t i) {if (clusterModules.size()>0) return clusterStrips[i]; else return -9999.0;}
		Float_t getClusterTime (Int_t i) {if (clusterModules.size()>0) return clusterTimes[i]; else return -9999.0;}
		Float_t getClusterTot (Int_t i) {if (clusterModules.size()>0) return clusterTots[i]; else return -9999.0;}
		
};

#endif
