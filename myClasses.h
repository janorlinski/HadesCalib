#ifndef __MYCLASSES__
#define __MYCLASSES__

// rpc data objects  
#include "rpcdef.h"
#include "hrpcraw.h"
#include "hrpccal.h"
#include "hrpchit.h"
#include "hrpccluster.h"

#include "hstart2cal.h"

#include "myIncludes.h"

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

const Int_t refCh[4] = {9, 23, 42, 62}; //c+c
//const Int_t refCh[4] = {11, 28, 42, 62}; //au+au
 
//const Float_t refWidthLo[2] = {15.4, 12.6};
//const Float_t refWidthHi[2] = {16.4, 13.3};

const Float_t refWidthLo[2] = {widthLoLimit, widthLoLimit};
const Float_t refWidthHi[2] = {widthHiLimit, widthHiLimit};

class ChargeCalibrationRPC : public HReconstructor {

	protected:
	
	    // pointer to outputfile
	    TFile* out; 
	    
	    // settings 
	    Bool_t fillHistograms = true;
	   
	    // histogram declarations
	    TH2F* hLeftChargeVsCellNumber[6];
	    TH2F* hRightChargeVsCellNumber[6];
	    TH1F* hLeftChargeOffsets[6];
	    TH1F* hRightChargeOffsets[6];
	    
	    // HADES stuff declarations
	    HCategory* rpcCalCategory;
	    HRpcCal* rpcCalObject;
	    HEventHeader* eventHeader;
	    HCategory* rpcHit;
	    
		//HCategory* rpcClus;   //
		//HCategory* candCat; //
		
		//HParticleCand* cand;
		//HRpcHit* rpchit;
		//HRpcCluster* rpcclus;

	public:
	
		//default constructor
	    ChargeCalibrationRPC (const Text_t *name = "", const Text_t *title ="", TFile* outfile = NULL) : HReconstructor(name, title) { 
	
			out = outfile;
			//out = new TFile(outfile,"RECREATE");
	    
	    }
	
		//destructor
	    virtual ~ChargeCalibrationRPC () {
	    }
	
	    Bool_t init() {
			// this function is called once in the beginning
			// create histograms or get pointer to param containers
			// or data containers here. Tip: Create first your
			// output file and and after that your histograms/ ntuples.
			// In this way you make shure the object will end up in your
		    // root file and not any other one.
		
		//rpcCalCategory = NULL;
		rpcCalObject = NULL;
		eventHeader = NULL;

		rpcCalCategory = HCategoryManager::getCategory(catRpcCal);
	    //rpcClus = HCategoryManager::getCategory(catRpcCluster);   //
	    //candCat = HCategoryManager::getCategory(catParticleCand); //
		//rpcHit = HCategoryManager::getCategory(catRpcHit);
		
		if(out) {
				
		   out->cd();
		
		// histogram definitions
		
				   
			for (Int_t i=0; i<6; i++) { // loop over sectors
			   
				hLeftChargeVsCellNumber[i] = new TH2F(
				Form("hLeftChargeVsCellNumber_sect%i", i+1),
				Form("Left charge vs. cell number, sector %i; 32 #times module + cell; charge", i+1), 
				nCells, loLimitCells, hiLimitCells, 300, -20.0, 30.0);
				   
				hRightChargeVsCellNumber[i] = new TH2F(
				Form("hRightChargeVsCellNumber_sect%i", i+1), 
				Form("Right charge vs. cell number, sector %i; 32 #times module + cell; charge", i+1), 
				nCells, loLimitCells, hiLimitCells, 300, -20.0, 30.0);
					   			   
				hLeftChargeOffsets[i] = new TH1F(
				Form("hLeftChargeOffsets_sect%i", i+1),
				Form("Left charge offsets, sector %i; 32 #times module + cell; charge offset", i+1), 
				nCells, loLimitCells, hiLimitCells);
			   
				hRightChargeOffsets[i] = new TH1F(
				Form("hRightChargeOffsets_sect%i", i+1),
				Form("Right charge offsets, sector %i; 32 #times module + cell; charge offset", i+1), 
				nCells, loLimitCells, hiLimitCells);
			}
		}	
			return kTRUE;
	}
    
	    Bool_t reinit() {   
			// this function is called for each file
	        // after init()
			// use this place if you need already initialized
	        // containers
			rpcCalCategory = HCategoryManager::getCategory(catRpcCal);
		    //rpcClus = HCategoryManager::getCategory(catRpcCluster);   //
		    //candCat = HCategoryManager::getCategory(catParticleCand); //
			return kTRUE;
	    }
	
	    Int_t execute() {   
			
			// this function is called once per event.
			// if the function returns kSkipEvent the event
			// will be skipped a. for all following tasks
			// and b. not appear in output (hades eventLoop())
			
			// this is the part actually responsible for doing the hard work for us
			// find appropriate variables and fill the histograms with them
			
	//		cout << "This is the 'execute' function from my FillHistos : HReconstructor class \n";
			
	// 		charge calibration		
			
			for(Int_t i = 0; i<rpcCalCategory->getEntries();i++) {
	                    
				rpcCalObject = (HRpcCal*) HCategoryManager::getObject(rpcCalObject,rpcCalCategory,i);
	          
	       	    if(!rpcCalObject) continue;
	            	    
			    Int_t cell   = rpcCalObject -> getCell();
				Int_t sector = rpcCalObject -> getSector();
	            Int_t column = rpcCalObject -> getColumn();
	            Int_t index = 32*column+cell;
			            
			    Double_t leftCharge = rpcCalObject -> getLeftCharge();
			    Double_t rightCharge = rpcCalObject -> getRightCharge();
	
			    if (leftCharge>-50 && leftCharge<50)
					hLeftChargeVsCellNumber[sector] -> Fill(index,leftCharge);
				if (rightCharge>-50 && rightCharge<50)
					hRightChargeVsCellNumber[sector] -> Fill(index,rightCharge);
			
	//		    std::cout << Form("RPC charge debug: SEC%iCELL%iCOL%i Left = %f Right = %f \n", sector, cell, column, leftCharge, rightCharge);
			
			}
		
		return 0;
			
		}
	
	    Bool_t finalize() {   
		
		// this function is called once in the end of the
		// runtime of the program. Save histograms to file
		// etc. Tip: Change to your ROOT output file before
		// writing our histograms. You might not be the only
		// user of root files and the current directory could
	        // the one from another user.
		
		std::cout << "Finalizing the ChargeCalibrationRPC task..." << endl;
		
			if(out) {
				
		       out->cd();
		       
		       // write histograms
			   for (Int_t i=0; i<6; i++) {
				   
				   std::cout << "Getting offsets and saving histograms for sector " << i+1 <<endl;
				   
				    fillChargeOffsets(hLeftChargeOffsets[i], hLeftChargeVsCellNumber[i], nCells);
				    fillChargeOffsets(hRightChargeOffsets[i], hRightChargeVsCellNumber[i], nCells);
		
					hRightChargeVsCellNumber[i]->Write();
					hLeftChargeVsCellNumber[i]->Write();
					hRightChargeOffsets[i]->Write();
					hLeftChargeOffsets[i]->Write();
		
				}
				
			   //out->Save();
		       //out->Close();
		       
			}
	
		return kTRUE;
		
	    }

};

class FillRpcAndStartHistosWithTracking : public HReconstructor {

	protected:
	
	    // pointer to outputfile
	    TFile* out;
	   
		// settings
	    Bool_t imposeBetaCut = false;
	    const Double_t betaCut = 0.92;
	   
	    // histogram declarations
	    TH2F* hBetaMomentum;
	    TH2F* hBetaMomentumSys0;
	    TH2F* hBetaMomentumSys1;
		TH2F* hToTVsStartStrip;
		TH2F* hAbsTimeVsStartStrip;
		
		TH2F* hTimeDifferenceVsStartStrip;
		TH2F* hTimeDifferenceVsStartStripBigRange;
		
		TH2F* hXposDifference[6];
		TH2F* hTimeDifference[6];
		TH2F* hTimeDifferenceBigRange[6];
		TH1F* hXposOffsets[6];
		TH1F* hTimeOffsets[6];
		
		TH1F* maxWidthPosOneFile;
		TH1F* maxAbsTimePosOneFile;
		TH1F* hNegativeTracksOneFile;
		TH1F* hPionsOneFile;
		TH1F* fitSigmaOneFile;
		TH1F* fitMuOneFile;
		
		// other containers
		//vector <  Double_t > vSigma;
		//vector <  Double_t > vMu;
		TString hldFileName;
		
	    // HADES stuff declarations
	    HEventHeader* eventHeader;
	    
	    // categories
		HCategory* candCat; //
	    HCategory* Start2CalCategory;
	    HCategory* Start2HitCategory;
	    
	    // objects
		HParticleCand* cand;
	    HStart2Cal* Start2CalObject;
	    HStart2Hit* Start2HitObject;
		//HRpcHit* rpchit;
		//HRpcCluster* rpcclus;

	public:
	
		//default constructor
	    FillRpcAndStartHistosWithTracking (const Text_t *name = "", const Text_t *title ="", TString fn = "defaultfilename.hld", TFile *pointerToOutFile = NULL) : HReconstructor(name, title) { 
	    
			hldFileName = fn;
			out = pointerToOutFile;
	    // this function will only work on loopDST files
	    
	    }
	
		//destructor
	    virtual ~FillRpcAndStartHistosWithTracking () {
	    }
	
	    Bool_t init() {
			// this function is called once in the beginning
			// create histograms or get pointer to param containers
			// or data containers here. Tip: Create first your
			// output file and and after that your histograms/ ntuples.
			// In this way you make shure the object will end up in your
		    // root file and not any other one.
		
		eventHeader = NULL;
	    Start2CalObject = NULL;
	    Start2HitObject = NULL;
	    
	    candCat = HCategoryManager::getCategory(catParticleCand);
		Start2CalCategory = HCategoryManager::getCategory(catStart2Cal);
		Start2HitCategory = HCategoryManager::getCategory(catStart2Hit);
		
		if(out) {
				
		   out->cd();
		
		// histogram definitions
				   
		    hBetaMomentum = new TH2F("hBetaMomentum", "beta vs. mom; mom*charge; beta", 800, -2000, 2000, 420, 0, 1.4);
		    hBetaMomentumSys0 = new TH2F("hBetaMomentumSys0", "beta vs. mom Sys0; mom*charge; beta", 800, -2000, 2000, 420, 0, 1.4);
		    hBetaMomentumSys1 = new TH2F("hBetaMomentumSys1", "beta vs. mom Sys1; mom*charge; beta", 800, -2000, 2000, 420, 0, 1.4);

			//hTimeDifferenceVsStartStripAvg = new TH2F("hTimeDifferenceVsStartStripAvg", "tdiff rpc (avg) vs. start ch", 80, 0.5, 80.5, 500, -10, 10);
			hToTVsStartStrip = new TH2F("hToTVsStartStrip", "start ToT vs. start ch", 80, 0.5, 80.5, 200, 0, 50);
			hAbsTimeVsStartStrip = new TH2F("hAbsTimeVsStartStrip", "start abs time vs. start ch", 80, 0.5, 80.5, 2000, -1000, 1000);
			hTimeDifferenceVsStartStrip = new TH2F("hTimeDifferenceVsStartStrip", "tdiff rpc vs. start ch", 80, 0.5, 80.5, 800, -20, 20);
			hTimeDifferenceVsStartStripBigRange = new TH2F("hTimeDifferenceVsStartStripAllTracks", "tdiff rpc big range vs. start ch", 80, 0.5, 80.5, 1000, -50, 50);
			
			hNegativeTracksOneFile = new TH1F("hNegativeTracksOneFile", "negative tracks vs start strip", 80, -0.5, 79.5);
			hPionsOneFile = new TH1F("hPionsOneFile", "pions vs start strip", 80, -0.5, 79.5);
		
			for (Int_t i=0; i<6; i++) { // loop over sectors
			   
				hXposDifference[i] = new TH2F(
				Form("hXposDifference_sect%i",i+1),
				Form("X position difference vs. cell number, sector %i; 32 #times module + cell; x diff", i+1),
				nCells, loLimitCells, hiLimitCells, 200,-100, 100);
				
				hTimeDifference[i] = new TH2F(
				Form("hTimeDifference_sect%i",i+1),
				Form("ToA difference vs. cell number, sector %i; 32 #times module + cell; t diff [ns]", i+1),
				nCells, loLimitCells, hiLimitCells, 100,-10, 10); // originally 200, -5, 5
				
				hTimeDifferenceBigRange[i] = new TH2F(
				Form("hTimeDifferenceBigRange_sect%i",i+1),
				Form("ToA difference vs. cell number [big range], sector %i; 32 #times module + cell; t diff [ns]", i+1), 
				nCells, loLimitCells, hiLimitCells, 200, -100, 100);
					   					   			   
				hXposOffsets[i] = new TH1F(
				Form("hXposOffsets_sect%i", i+1),
				Form("X position offsets, sector %i; 32 #times module + cell", i+1), 
				nCells, loLimitCells, hiLimitCells);
			   
				hTimeOffsets[i] = new TH1F(
				Form("hTimeOffsets_sect%i", i+1),
				Form("Time offsets, sector %i; 32 #times module + cell", i+1), 
				nCells, loLimitCells, hiLimitCells);
			}
		}	
			return kTRUE;
	}
    
    
    Bool_t reinit() {   
		// this function is called for each file
        // after init()
		// use this place if you need already initialized
        // containers
		//rpcCalCategory = HCategoryManager::getCategory(catRpcCal);
	    //rpcClus = HCategoryManager::getCategory(catRpcCluster);   //
	    candCat = HCategoryManager::getCategory(catParticleCand); //
		Start2CalCategory = HCategoryManager::getCategory(catStart2Cal);
		Start2HitCategory = HCategoryManager::getCategory(catStart2Hit);
		return kTRUE;
    }

    Int_t execute() {   
		
		// this function is called once per event.
		// if the function returns kSkipEvent the event
		// will be skipped a. for all following tasks
		// and b. not appear in output (hades eventLoop())
		
		// this is the part actually responsible for doing the hard work for us
		// find appropriate variables and fill the histograms with them

		//std::cout << "execute!" << endl;

        HIterator *iterParticleCand = NULL;
        HIterator *iterStart2Cal = NULL;
        HIterator *iterStart2Hit = NULL;
        candCat = HCategoryManager::getCategory(catParticleCand);
		Start2CalCategory = HCategoryManager::getCategory(catStart2Cal);
		Start2HitCategory = HCategoryManager::getCategory(catStart2Hit);
        
        if (!candCat) std::cout << "no hparticlecand?! \n";
        else if (!Start2CalCategory) std::cout << "no start2cal?! \n";
        else if (!Start2HitCategory) std::cout << "no start2hit?! \n";
        
        else {
						
            iterParticleCand = (HIterator *) candCat->MakeIterator("native");
            iterStart2Cal = (HIterator *)    Start2CalCategory->MakeIterator("native");
            iterStart2Hit = (HIterator *)    Start2HitCategory->MakeIterator("native");
            iterParticleCand->Reset();
                  
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
	        
	        //extra stuff for start guys
	        
	        while (NULL != (Start2CalObject = static_cast<HStart2Cal*>(iterStart2Cal->Next()))) {
				
				
				Int_t nOfStartHits = Start2CalObject->getMultiplicity();
				
				Int_t mod = Start2CalObject->getModule();
				Int_t strip = Start2CalObject->getStrip();
				Int_t index = 20*mod + strip;
								
				for (Int_t i=0; i<nOfStartHits; i++) { // loop over all start hits
					
					Float_t width = Start2CalObject->getWidth(i+1);
					Float_t absTime = Start2CalObject->getTime(i+1);
					
					hToTVsStartStrip->Fill(index, width);
					hAbsTimeVsStartStrip->Fill(index, absTime);
					
				}

			}
			
			iterStart2Cal->Reset(); // return to 0 for the next loop
            
            while (NULL != (cand = static_cast<HParticleCand*>(iterParticleCand->Next()))) { //ParticleCandIter
				
	            if (cand->getChi2() > 0 && cand->getChi2() < 1000) {
					
					Float_t beta = cand->getBeta();
					Float_t mom = cand->getMomentum();
					Float_t chrg = cand->getCharge();
					
					hBetaMomentum->Fill(mom*chrg, beta);

					if (cand->getSystemUsed()==0) hBetaMomentumSys0->Fill(mom*chrg, beta);
					else if (cand->getSystemUsed()==1) hBetaMomentumSys1->Fill(mom*chrg, beta);
										
					if (imposeBetaCut && cand->getBeta() < betaCut) continue;
					
					Float_t MASS = 0;            
		            if (cand->getCharge()==-1) MASS = 139.57;
		            else if (cand->getCharge()==1) MASS = 139.57;
					else continue;
	            
// 					Xpos calibration
			  
				    if (cand->getSystemUsed()==0){ 			

						Float_t xdiff = cand->getRkMetaDx();
						
						Int_t indexRPCHit = cand->getRpcInd();
						Int_t metaCell = cand->getMetaCell(indexRPCHit);
						Int_t metaModule = cand->getMetaModule(indexRPCHit);
						Int_t sector = cand -> getSector();
			            Int_t index = 32*metaModule + metaCell;
			            
						hXposDifference[sector]->Fill(index, xdiff);
					
				    }
				    
// 					T diff
							
					//Float_t beta_test = (cand->getMomentum())/TMath::Sqrt((cand->getMomentum())*(cand->getMomentum())+(cand->getMass2()) );
				    Float_t MIN_MOM = 50; 
	
					Float_t Tof;
	                Float_t beta_c = (cand->getMomentum())/TMath::Sqrt((cand->getMomentum())*(cand->getMomentum())+MASS*MASS);
	                Float_t tof_c;
	                Float_t time_diff;
	                
				    if (cand->getSystemUsed() ==0 && cand->getRpcInd() > -1) {	
						
						if (cand->getMomentum() > MIN_MOM) {   
							
							Tof = cand->getTof();
							tof_c  = cand->getBeta() / beta_c;
							time_diff =  Tof * (1.0 - tof_c);
							
							Int_t indexRPCHit = cand->getRpcInd();
							Int_t metaCell = cand->getMetaCell(indexRPCHit);
							Int_t metaModule = cand->getMetaModule(indexRPCHit);
							Int_t sector = cand -> getSector();
				            Int_t index = 32*metaModule + metaCell;
			            
							if (abs(time_diff)<5) hTimeDifference[sector]->Fill(index, time_diff);
							hTimeDifferenceBigRange[sector]->Fill(index, time_diff);		
							
							// THIS SECTION IS FOR RPC TDIFF vs. START STRIPE 
							
				            while (NULL != (Start2CalObject = static_cast<HStart2Cal*>(iterStart2Cal->Next()))) { // get all the start2cal objects
							
								if (cand->getCharge() < 0) { //select negative tracks
							
									Int_t nOfStartHits = Start2CalObject->getMultiplicity();
									Int_t startIndex = Start2CalObject->getModule() * 20 + Start2CalObject->getStrip();
								
									hNegativeTracksOneFile->Fill(startIndex, nOfStartHits);
									
									if (cand->getBeta() >= betaCut && cand->getBeta() < 1.4) hPionsOneFile->Fill(startIndex, nOfStartHits);
								
									for (Int_t i=0; i<nOfStartHits; i++) { // loop over all start hits
										
										Float_t calTime = Start2CalObject->getTime(i+1);
										Float_t time_diff_recalc = time_diff + startTime - calTime;
										
										hTimeDifferenceVsStartStrip->Fill(startIndex, time_diff_recalc);
										hTimeDifferenceVsStartStripBigRange->Fill(startIndex, time_diff_recalc);
										
									} // end of nOfStartHits loop
								} // end of select negative tracks
					    
							} //end of start2cal iter
	                    }
	                }
				}
			}
		} 
	
	return 0;
		
	}

    Bool_t finalize() {   
	
	// this function is called once in the end of the
	// runtime of the program. Save histograms to file
	// etc. Tip: Change to your ROOT output file before
	// writing our histograms. You might not be the only
	// user of root files and the current directory could
    // the one from another user.
        
    // fits for sigma and mu
	
	const Float_t fitRadius = 2.0;
	
	hldFileName.ReplaceAll("/lustre/hades/raw/feb24/24/", "");
	hldFileName.ReplaceAll(".root", "");
	
	maxWidthPosOneFile = new TH1F ("maxWidthPosOneFile", hldFileName, 80, -0.5, 79.5);
	maxAbsTimePosOneFile = new TH1F ("maxAbsTimePosOneFile", hldFileName, 80, -0.5, 79.5);
	fitMuOneFile = new TH1F ("fitMuOneFile", hldFileName, 80, -0.5, 79.5);
	fitSigmaOneFile = new TH1F ("fitSigmaOneFile", hldFileName, 80, -0.5, 79.5);

	for (Int_t i=0; i<=80; i++) { // loop over channels
	
		// get positiom of width maximum
		
		TH1F* projWidth = (TH1F*) hToTVsStartStrip->ProjectionY(Form("projWidth__ch%i", i), i+1, i+1);
		Float_t maxWidthPos = projWidth->GetBinCenter(projWidth->GetMaximumBin());
		maxWidthPosOneFile->SetBinContent(i+1, maxWidthPos);
		//TF1* gausFitWidth = new TF1 (Form("fit_%i", i), gaussianWithConstBckg, fitLoEdge, fitHiEdge, 4);
		
		
		// get positiom of abs time maximum
		
		TH1F* projAbsTime = (TH1F*) hAbsTimeVsStartStrip->ProjectionY(Form("projAbsTime__ch%i", i), i+1, i+1);
		Float_t maxAbsTimePos = projAbsTime->GetBinCenter(projAbsTime->GetMaximumBin());
		maxAbsTimePosOneFile->SetBinContent(i+1, maxAbsTimePos);
		
		TH1F* projection = (TH1F*) hTimeDifferenceVsStartStrip->ProjectionY(Form("projection__ch%i", i), i+1, i+1);
		Int_t nEntrOfSource = projection->GetEntries();
		
		// set fit range
	
		//Double_t positionOfMaximum = projection->GetBinCenter(projection->GetMaximumBin());
		Double_t positionOfMaximum = 0.0;
		Double_t heightOfMaximum = projection->GetMaximum();
		//Double_t positionOfMaximum = 0.0;
		Double_t fitLoEdge = positionOfMaximum - fitRadius;
		Double_t fitHiEdge = positionOfMaximum + fitRadius;
		std::cout << "Gaus will be fitted in range [" << fitLoEdge << ", " << fitHiEdge << "] \n";
		
		// fit and fill offset
		
		TF1* gausFit = new TF1 (Form("fit_%i", i), gaussianWithConstBckg, fitLoEdge, fitHiEdge, 4);
		
		gausFit->SetParameter(0, 20.0);
		gausFit->SetParLimits(0, 1.0, 1000000.0);
		
		gausFit->SetParameter(1, 0.0);
		gausFit->SetParLimits(1, fitLoEdge + 0.5*fitRadius, fitHiEdge-0.5*fitRadius);
		
		gausFit->SetParameter(2, 0.150);
		gausFit->SetParLimits(2, 0.0, fitRadius);
		
		//gausFit->FixParameter(3, 0.0);
		gausFit->SetParameter(3, 0);
		gausFit->SetParLimits(3, 0.0, 0.2*heightOfMaximum);
		
		gausFit->SetRange(fitLoEdge, fitHiEdge);
		projection->Fit(gausFit, "RQ+");
		
		Double_t center = gausFit->GetParameter(1);
		Double_t centerErr = gausFit->GetParError(1);
		Double_t sigma = gausFit->GetParameter(2);
		Double_t sigmaErr = gausFit->GetParError(2);
		
		//if (nEntrOfSource>11000) hOffsets->SetBinContent(i, center);
		//else hOffsets->SetBinContent(i, 0.0);
		out->cd();
		projection->Write();
		
		if (center != 0.0) {
			fitMuOneFile->SetBinContent(i+1, center);
			fitMuOneFile->SetBinError(i+1, centerErr);
		}
		if (sigma != 0.150) {
			fitSigmaOneFile->SetBinContent(i+1, sigma);
			fitSigmaOneFile->SetBinError(i+1, sigmaErr);
		}
	}
        
	
		if(out) {
			
			out->cd();
	       
			hBetaMomentum->Write();
			hBetaMomentumSys0->Write();
			hBetaMomentumSys1->Write();
			hToTVsStartStrip->Write();
			hAbsTimeVsStartStrip->Write();
			hTimeDifferenceVsStartStrip->Write();
			hTimeDifferenceVsStartStripBigRange->Write();
			maxWidthPosOneFile->Write();
			maxAbsTimePosOneFile->Write();
			hNegativeTracksOneFile->Write();
			hPionsOneFile->Write();
			fitMuOneFile->Write();
			fitSigmaOneFile->Write();
			
			for (Int_t i=0; i<6; i++) {
			
				//add sigma and chisquare if you wanna use this now
				//right now this is not needed since fitting can be only done after merging files from the batch farm
				
			    //fillTimeAndPosOffsets(hXposOffsets[i], hXposDifference[i], nCells, out, Form("XposFits_sect%i", i+1));
			    //fillTimeAndPosOffsets(hTimeOffsets[i], hTimeDifference[i], nCells, out, Form("TimeFits_sect%i", i+1));
			
				out->cd();
			
				hXposDifference[i]->Write();
				hTimeDifference[i]->Write();
				hTimeDifferenceBigRange[i]->Write();
				hXposOffsets[i]->Write();
				hTimeOffsets[i]->Write();
					
			}
			
		   //out->Save();
	      // out->Close();
	       
		}

	return kTRUE;
	
    }

};

class TimeAndPosCalibrationRPC : public HReconstructor {

	protected:
	
	    // pointer to outputfile
	    TFile* out;
	   
		// settings
	    Bool_t imposeBetaCut = false;
	    const Double_t betaCut = 0.92;
	   
	    // histogram declarations
	    TH2F* hBetaMomentum;
	    TH2F* hBetaMomentumSys0;
	    TH2F* hBetaMomentumSys1;
		TH2F* hToTVsStartStrip;
		TH2F* hAbsTimeVsStartStrip;
		
		TH2F* hTimeDifferenceVsStartStrip;
		TH2F* hTimeDifferenceVsStartStripBigRange;
		
		TH2F* hXposDifference[6];
		TH2F* hTimeDifference[6];
		TH2F* hTimeDifferenceBigRange[6];
		TH1F* hXposOffsets[6];
		TH1F* hTimeOffsets[6];
		
		TH1F* maxWidthPosOneFile;
		TH1F* maxAbsTimePosOneFile;
		TH1F* hNegativeTracksOneFile;
		TH1F* hPionsOneFile;
		TH1F* fitSigmaOneFile;
		TH1F* fitMuOneFile;
		
		// other containers
		//vector <  Double_t > vSigma;
		//vector <  Double_t > vMu;
		TString hldFileName;
		
	    // HADES stuff declarations
	    HEventHeader* eventHeader;
	    
	    // categories
		HCategory* candCat; //
	    HCategory* Start2CalCategory;
	    HCategory* Start2HitCategory;
	    
	    // objects
		HParticleCand* cand;
	    HStart2Cal* Start2CalObject;
	    HStart2Hit* Start2HitObject;
		//HRpcHit* rpchit;
		//HRpcCluster* rpcclus;

	public:
	
		//default constructor
	    TimeAndPosCalibrationRPC (const Text_t *name = "", const Text_t *title ="", TString fn = "defaultfilename.hld", TFile *pointerToOutFile = NULL) : HReconstructor(name, title) { 
	    
			hldFileName = fn;
			out = pointerToOutFile;
	    // this function will only work on loopDST files
	    
	    }
	
		//destructor
	    virtual ~TimeAndPosCalibrationRPC () {
	    }
	
	    Bool_t init() {
			// this function is called once in the beginning
			// create histograms or get pointer to param containers
			// or data containers here. Tip: Create first your
			// output file and and after that your histograms/ ntuples.
			// In this way you make shure the object will end up in your
		    // root file and not any other one.
		
		eventHeader = NULL;
	    Start2CalObject = NULL;
	    Start2HitObject = NULL;
	    
	    candCat = HCategoryManager::getCategory(catParticleCand);
		Start2CalCategory = HCategoryManager::getCategory(catStart2Cal);
		Start2HitCategory = HCategoryManager::getCategory(catStart2Hit);
		
		if(out) {
				
		   out->cd();
		
		// histogram definitions
				   
		    hBetaMomentum = new TH2F("hBetaMomentum", "beta vs. mom; mom*charge; beta", 800, -2000, 2000, 420, 0, 1.4);
		    hBetaMomentumSys0 = new TH2F("hBetaMomentumSys0", "beta vs. mom Sys0; mom*charge; beta", 800, -2000, 2000, 420, 0, 1.4);
		    hBetaMomentumSys1 = new TH2F("hBetaMomentumSys1", "beta vs. mom Sys1; mom*charge; beta", 800, -2000, 2000, 420, 0, 1.4);

			//hTimeDifferenceVsStartStripAvg = new TH2F("hTimeDifferenceVsStartStripAvg", "tdiff rpc (avg) vs. start ch", 80, 0.5, 80.5, 500, -10, 10);
			hToTVsStartStrip = new TH2F("hToTVsStartStrip", "start ToT vs. start ch", 80, 0.5, 80.5, 200, 0, 50);
			hAbsTimeVsStartStrip = new TH2F("hAbsTimeVsStartStrip", "start abs time vs. start ch", 80, 0.5, 80.5, 2000, -1000, 1000);
			hTimeDifferenceVsStartStrip = new TH2F("hTimeDifferenceVsStartStrip", "tdiff rpc vs. start ch", 80, 0.5, 80.5, 800, -20, 20);
			hTimeDifferenceVsStartStripBigRange = new TH2F("hTimeDifferenceVsStartStripAllTracks", "tdiff rpc big range vs. start ch", 80, 0.5, 80.5, 1000, -50, 50);
			
			hNegativeTracksOneFile = new TH1F("hNegativeTracksOneFile", "negative tracks vs start strip", 80, -0.5, 79.5);
			hPionsOneFile = new TH1F("hPionsOneFile", "pions vs start strip", 80, -0.5, 79.5);
		
			for (Int_t i=0; i<6; i++) { // loop over sectors
			   
				hXposDifference[i] = new TH2F(
				Form("hXposDifference_sect%i",i+1),
				Form("X position difference vs. cell number, sector %i; 32 #times module + cell; x diff", i+1),
				nCells, loLimitCells, hiLimitCells, 200,-100, 100);
				
				hTimeDifference[i] = new TH2F(
				Form("hTimeDifference_sect%i",i+1),
				Form("ToA difference vs. cell number, sector %i; 32 #times module + cell; t diff [ns]", i+1),
				nCells, loLimitCells, hiLimitCells, 100,-10, 10); // originally 200, -5, 5
				
				hTimeDifferenceBigRange[i] = new TH2F(
				Form("hTimeDifferenceBigRange_sect%i",i+1),
				Form("ToA difference vs. cell number [big range], sector %i; 32 #times module + cell; t diff [ns]", i+1), 
				nCells, loLimitCells, hiLimitCells, 200, -100, 100);
					   					   			   
				hXposOffsets[i] = new TH1F(
				Form("hXposOffsets_sect%i", i+1),
				Form("X position offsets, sector %i; 32 #times module + cell", i+1), 
				nCells, loLimitCells, hiLimitCells);
			   
				hTimeOffsets[i] = new TH1F(
				Form("hTimeOffsets_sect%i", i+1),
				Form("Time offsets, sector %i; 32 #times module + cell", i+1), 
				nCells, loLimitCells, hiLimitCells);
			}
		}	
			return kTRUE;
	}
    
    
    Bool_t reinit() {   
		// this function is called for each file
        // after init()
		// use this place if you need already initialized
        // containers
		//rpcCalCategory = HCategoryManager::getCategory(catRpcCal);
	    //rpcClus = HCategoryManager::getCategory(catRpcCluster);   //
	    candCat = HCategoryManager::getCategory(catParticleCand); //
		Start2CalCategory = HCategoryManager::getCategory(catStart2Cal);
		Start2HitCategory = HCategoryManager::getCategory(catStart2Hit);
		return kTRUE;
    }

    Int_t execute() {   
		
		// this function is called once per event.
		// if the function returns kSkipEvent the event
		// will be skipped a. for all following tasks
		// and b. not appear in output (hades eventLoop())
		
		// this is the part actually responsible for doing the hard work for us
		// find appropriate variables and fill the histograms with them

		//std::cout << "execute!" << endl;

        HIterator *iterParticleCand = NULL;
        HIterator *iterStart2Cal = NULL;
        HIterator *iterStart2Hit = NULL;
        candCat = HCategoryManager::getCategory(catParticleCand);
		Start2CalCategory = HCategoryManager::getCategory(catStart2Cal);
		Start2HitCategory = HCategoryManager::getCategory(catStart2Hit);
        
        if (!candCat) std::cout << "no hparticlecand?! \n";
        else if (!Start2CalCategory) std::cout << "no start2cal?! \n";
        else if (!Start2HitCategory) std::cout << "no start2hit?! \n";
        
        else {
						
            iterParticleCand = (HIterator *) candCat->MakeIterator("native");
            iterStart2Cal = (HIterator *)    Start2CalCategory->MakeIterator("native");
            iterStart2Hit = (HIterator *)    Start2HitCategory->MakeIterator("native");
            iterParticleCand->Reset();
                  
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
	        
	        //extra stuff for start guys
	        
	        while (NULL != (Start2CalObject = static_cast<HStart2Cal*>(iterStart2Cal->Next()))) {
				
				
				Int_t nOfStartHits = Start2CalObject->getMultiplicity();
				
				Int_t mod = Start2CalObject->getModule();
				Int_t strip = Start2CalObject->getStrip();
				Int_t index = 20*mod + strip;
								
				for (Int_t i=0; i<nOfStartHits; i++) { // loop over all start hits
					
					Float_t width = Start2CalObject->getWidth(i+1);
					Float_t absTime = Start2CalObject->getTime(i+1);
					
					hToTVsStartStrip->Fill(index, width);
					hAbsTimeVsStartStrip->Fill(index, absTime);
					
				}

			}
			
			iterStart2Cal->Reset(); // return to 0 for the next loop
            
            while (NULL != (cand = static_cast<HParticleCand*>(iterParticleCand->Next()))) { //ParticleCandIter
				
	            if (cand->getChi2() > 0 && cand->getChi2() < 1000) {
					
					Float_t beta = cand->getBeta();
					Float_t mom = cand->getMomentum();
					Float_t chrg = cand->getCharge();
					
					hBetaMomentum->Fill(mom*chrg, beta);

					if (cand->getSystemUsed()==0) hBetaMomentumSys0->Fill(mom*chrg, beta);
					else if (cand->getSystemUsed()==1) hBetaMomentumSys1->Fill(mom*chrg, beta);
										
					if (imposeBetaCut && cand->getBeta() < betaCut) continue;
					
					Float_t MASS = 0;            
		            if (cand->getCharge()==-1) MASS = 139.57;
		            else if (cand->getCharge()==1) MASS = 139.57;
					else continue;
	            
// 					Xpos calibration
			  
				    if (cand->getSystemUsed()==0){ 			

						Float_t xdiff = cand->getRkMetaDx();
						
						Int_t indexRPCHit = cand->getRpcInd();
						Int_t metaCell = cand->getMetaCell(indexRPCHit);
						Int_t metaModule = cand->getMetaModule(indexRPCHit);
						Int_t sector = cand -> getSector();
			            Int_t index = 32*metaModule + metaCell;
			            
						hXposDifference[sector]->Fill(index, xdiff);
					
				    }
				    
// 					T diff
							
					//Float_t beta_test = (cand->getMomentum())/TMath::Sqrt((cand->getMomentum())*(cand->getMomentum())+(cand->getMass2()) );
				    Float_t MIN_MOM = 50; 
	
					Float_t Tof;
	                Float_t beta_c = (cand->getMomentum())/TMath::Sqrt((cand->getMomentum())*(cand->getMomentum())+MASS*MASS);
	                Float_t tof_c;
	                Float_t time_diff;
	                
				    if (cand->getSystemUsed() ==0 && cand->getRpcInd() > -1) {	
						
						if (cand->getMomentum() > MIN_MOM) {   
							
							Tof = cand->getTof();
							tof_c  = cand->getBeta() / beta_c;
							time_diff =  Tof * (1.0 - tof_c);
							
							Int_t indexRPCHit = cand->getRpcInd();
							Int_t metaCell = cand->getMetaCell(indexRPCHit);
							Int_t metaModule = cand->getMetaModule(indexRPCHit);
							Int_t sector = cand -> getSector();
				            Int_t index = 32*metaModule + metaCell;
			            
							if (abs(time_diff)<5) hTimeDifference[sector]->Fill(index, time_diff);
							hTimeDifferenceBigRange[sector]->Fill(index, time_diff);		
							
							// THIS SECTION IS FOR RPC TDIFF vs. START STRIPE 
							
				            while (NULL != (Start2CalObject = static_cast<HStart2Cal*>(iterStart2Cal->Next()))) { // get all the start2cal objects
							
								if (cand->getCharge() < 0) { //select negative tracks
							
									Int_t nOfStartHits = Start2CalObject->getMultiplicity();
									Int_t startIndex = Start2CalObject->getModule() * 20 + Start2CalObject->getStrip();
								
									hNegativeTracksOneFile->Fill(startIndex, nOfStartHits);
									
									if (cand->getBeta() >= betaCut && cand->getBeta() < 1.4) hPionsOneFile->Fill(startIndex, nOfStartHits);
								
									for (Int_t i=0; i<nOfStartHits; i++) { // loop over all start hits
										
										Float_t calTime = Start2CalObject->getTime(i+1);
										Float_t time_diff_recalc = time_diff + startTime - calTime;
										
										hTimeDifferenceVsStartStrip->Fill(startIndex, time_diff_recalc);
										hTimeDifferenceVsStartStripBigRange->Fill(startIndex, time_diff_recalc);
										
									} // end of nOfStartHits loop
								} // end of select negative tracks
					    
							} //end of start2cal iter
	                    }
	                }
				}
			}
		} 
	
	return 0;
		
	}

    Bool_t finalize() {   
	
	// this function is called once in the end of the
	// runtime of the program. Save histograms to file
	// etc. Tip: Change to your ROOT output file before
	// writing our histograms. You might not be the only
	// user of root files and the current directory could
    // the one from another user.
        
    // fits for sigma and mu
	
	const Float_t fitRadius = 2.0;
	
	hldFileName.ReplaceAll("/lustre/hades/raw/feb24/24/", "");
	hldFileName.ReplaceAll(".root", "");
	
	maxWidthPosOneFile = new TH1F ("maxWidthPosOneFile", hldFileName, 80, -0.5, 79.5);
	maxAbsTimePosOneFile = new TH1F ("maxAbsTimePosOneFile", hldFileName, 80, -0.5, 79.5);
	fitMuOneFile = new TH1F ("fitMuOneFile", hldFileName, 80, -0.5, 79.5);
	fitSigmaOneFile = new TH1F ("fitSigmaOneFile", hldFileName, 80, -0.5, 79.5);

	for (Int_t i=0; i<=80; i++) { // loop over channels
	
		// get positiom of width maximum
		
		TH1F* projWidth = (TH1F*) hToTVsStartStrip->ProjectionY(Form("projWidth__ch%i", i), i+1, i+1);
		Float_t maxWidthPos = projWidth->GetBinCenter(projWidth->GetMaximumBin());
		maxWidthPosOneFile->SetBinContent(i+1, maxWidthPos);
		//TF1* gausFitWidth = new TF1 (Form("fit_%i", i), gaussianWithConstBckg, fitLoEdge, fitHiEdge, 4);
		
		
		// get positiom of abs time maximum
		
		TH1F* projAbsTime = (TH1F*) hAbsTimeVsStartStrip->ProjectionY(Form("projAbsTime__ch%i", i), i+1, i+1);
		Float_t maxAbsTimePos = projAbsTime->GetBinCenter(projAbsTime->GetMaximumBin());
		maxAbsTimePosOneFile->SetBinContent(i+1, maxAbsTimePos);
		
		TH1F* projection = (TH1F*) hTimeDifferenceVsStartStrip->ProjectionY(Form("projection__ch%i", i), i+1, i+1);
		Int_t nEntrOfSource = projection->GetEntries();
		
		// set fit range
	
		//Double_t positionOfMaximum = projection->GetBinCenter(projection->GetMaximumBin());
		Double_t positionOfMaximum = 0.0;
		Double_t heightOfMaximum = projection->GetMaximum();
		//Double_t positionOfMaximum = 0.0;
		Double_t fitLoEdge = positionOfMaximum - fitRadius;
		Double_t fitHiEdge = positionOfMaximum + fitRadius;
		std::cout << "Gaus will be fitted in range [" << fitLoEdge << ", " << fitHiEdge << "] \n";
		
		// fit and fill offset
		
		TF1* gausFit = new TF1 (Form("fit_%i", i), gaussianWithConstBckg, fitLoEdge, fitHiEdge, 4);
		
		gausFit->SetParameter(0, 20.0);
		gausFit->SetParLimits(0, 1.0, 1000000.0);
		
		gausFit->SetParameter(1, 0.0);
		gausFit->SetParLimits(1, fitLoEdge + 0.5*fitRadius, fitHiEdge-0.5*fitRadius);
		
		gausFit->SetParameter(2, 0.150);
		gausFit->SetParLimits(2, 0.0, fitRadius);
		
		//gausFit->FixParameter(3, 0.0);
		gausFit->SetParameter(3, 0);
		gausFit->SetParLimits(3, 0.0, 0.2*heightOfMaximum);
		
		gausFit->SetRange(fitLoEdge, fitHiEdge);
		projection->Fit(gausFit, "RQ+");
		
		Double_t center = gausFit->GetParameter(1);
		Double_t centerErr = gausFit->GetParError(1);
		Double_t sigma = gausFit->GetParameter(2);
		Double_t sigmaErr = gausFit->GetParError(2);
		
		//if (nEntrOfSource>11000) hOffsets->SetBinContent(i, center);
		//else hOffsets->SetBinContent(i, 0.0);
		out->cd();
		projection->Write();
		
		if (center != 0.0) {
			fitMuOneFile->SetBinContent(i+1, center);
			fitMuOneFile->SetBinError(i+1, centerErr);
		}
		if (sigma != 0.150) {
			fitSigmaOneFile->SetBinContent(i+1, sigma);
			fitSigmaOneFile->SetBinError(i+1, sigmaErr);
		}
	}
        
	
		if(out) {
			
			out->cd();
	       
			hBetaMomentum->Write();
			hBetaMomentumSys0->Write();
			hBetaMomentumSys1->Write();
			hToTVsStartStrip->Write();
			hAbsTimeVsStartStrip->Write();
			hTimeDifferenceVsStartStrip->Write();
			hTimeDifferenceVsStartStripBigRange->Write();
			maxWidthPosOneFile->Write();
			maxAbsTimePosOneFile->Write();
			hNegativeTracksOneFile->Write();
			hPionsOneFile->Write();
			fitMuOneFile->Write();
			fitSigmaOneFile->Write();
			
			for (Int_t i=0; i<6; i++) {
			
				//add sigma and chisquare if you wanna use this now
				//right now this is not needed since fitting can be only done after merging files from the batch farm
				
			    //fillTimeAndPosOffsets(hXposOffsets[i], hXposDifference[i], nCells, out, Form("XposFits_sect%i", i+1));
			    //fillTimeAndPosOffsets(hTimeOffsets[i], hTimeDifference[i], nCells, out, Form("TimeFits_sect%i", i+1));
			
				out->cd();
			
				hXposDifference[i]->Write();
				hTimeDifference[i]->Write();
				hTimeDifferenceBigRange[i]->Write();
				hXposOffsets[i]->Write();
				hTimeOffsets[i]->Write();
					
			}
			
		   //out->Save();
	      // out->Close();
	       
		}

	return kTRUE;
	
    }

};

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
	Int_t module = -1;
	if (channel >=1 && channel <= nChPerMod) module = 1;
	else if (channel >=nChPerMod+1 && channel <= 2*nChPerMod) module = 0;
	//cout << "getRefModule returning " << module << " for channel = " << channel << endl;
	return module;
}

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

class StartCalibration : public HReconstructor {

	protected:
	
	    // pointer to outputfile
	    TFile* out; 
	    
	    // settings 
	    Bool_t fillHistograms = true;
	   
	    // histogram declarations
	    TH1F* hMultiplicity;
	    TH2F* hMultiplicityPerModule;
	    TH2F* hWidthVsChannel;
	    TH2F* hAbsTimeVsChannel;
	    TH2F* hTimeDiffVsChannel[nChannels];
	    TH2F* hTimeDiffVsWidth[nChannels];
	    TH2F* hTimeDiffVsWidthBigTimeRange[nChannels];
	    TH1F* hTimeDiffForTimeWalk[nChannels][nPointsForTimeWalk];
	    TH1F* hWidthForTimeWalk[nChannels][nPointsForTimeWalk];
	    
	    // HADES stuff declarations
	    HCategory* Start2CalCategory;
	    HStart2Cal* Start2CalObjectRef;
	    HStart2Cal* Start2CalObject;
	    HEventHeader* eventHeader;

	public:
	
		//default constructor
	    StartCalibration (const Text_t *name = "", const Text_t *title ="", TFile* fout = NULL) : HReconstructor(name, title) { 
	
			out = fout;
	    
	    }
	
		//destructor
	    virtual ~StartCalibration () {
	    }
	
	    Bool_t init() {
			
			cout << ">>> DEBUG <<< Initializing the StartCalibration task..." <<endl;
			
			// this function is called once in the beginning
			// create histograms or get pointer to param containers
			// or data containers here. Tip: Create first your
			// output file and and after that your histograms/ ntuples.
			// In this way you make shure the object will end up in your
		    // root file and not any other one.
		
		//rpcCalCategory = NULL;
		Start2CalObjectRef = NULL;
		Start2CalObject = NULL;
		eventHeader = NULL;

		Start2CalCategory = HCategoryManager::getCategory(catStart2Cal);
	    //rpcClus = HCategoryManager::getCategory(catRpcCluster);   //
	    //candCat = HCategoryManager::getCategory(catParticleCand); //
		//rpcHit = HCategoryManager::getCategory(catRpcHit);
		
		if(out) {
				
		   out->cd();
		
		// histogram definitions

			hWidthVsChannel = new TH2F(
			"hWidthVsChannel", Form("START width vs. channel; %i #times module + channel; START width [a.u.]", nChPerMod), 
			nChannels, loLimitChannels, hiLimitChannels, nBinsForWidth, widthLoLimit, widthHiLimit);

			hMultiplicity = new TH1F(
			"hMultiplicity", "START multiplicity; mult; counts", 
			10, 0, 10);

			hMultiplicityPerModule = new TH2F(
			"hMultiplicityPerModule", "START multiplicity per module; module; mult", 
			nModules, 0.5, nModules + 0.5, 50, 0, 50);
			   
			hAbsTimeVsChannel = new TH2F(
			"hAbsTimeVsChannel", "START absolute time in channels; channel; time", 
			nChannels, 0.5, nChannels + 0.5, 2000, -10, 10);
			   
			for (Int_t i = 0; i<nChannels; i++) {
				
				hTimeDiffVsChannel[i] = new TH2F(
				Form("hTimeDiffVsChannel_refCh%i", i+1), 
				Form("Time difference vs. channels, reference channel = %i; %i #times module + channel; t diff", i+1, nChPerMod), 
				nChannels, loLimitChannels, hiLimitChannels, nBinsForTimeDiff, tDiffLoLimit, tDiffHiLimit);
		  
				hTimeDiffVsWidth[i] = new TH2F(
				Form("hTimeDiffVsWidth_%i", i+1), 
				Form("histogram for time walk, ref%i, test%i; test width; t_{ref} - t_{test}", refCh[getRefModule(i+1)], i+1), 
				nBinsForWidth, widthLoLimit, widthHiLimit, nBinsForTimeDiff, tDiffLoLimit, tDiffHiLimit);
		   
				hTimeDiffVsWidthBigTimeRange[i] = new TH2F(
				Form("hTimeDiffVsWidthBigTimeRange_%i", i+1), 
				Form("histogram for time walk BigTimeRange, ref%i, test%i; test width; t_{ref} - t_{test}", refCh[getRefModule(i+1)], i+1), 
				nBinsForWidth, widthLoLimit, widthHiLimit, nBinsForTimeDiff, 5*tDiffLoLimit, 5*tDiffHiLimit);
		   
				for (Int_t j = 0; j<nPointsForTimeWalk; j++) {
			
					hTimeDiffForTimeWalk[i][j] = new TH1F(
					Form("hTimeDiffForTimeWalk_refCh%i_testCh%i_widthBin%i", refCh[getRefModule(i+1)], i+1, j), 
					Form("Time difference histogram, ref = ch%i, test = ch%i, widthBin%i, refwidth in range %f-%f; t_{diff} [ns]", 
					refCh[getRefModule(i+1)], i+1, j, refWidthLo[getRefModule(i+1)], refWidthHi[getRefModule(i+1)]), 
					nBinsForTimeDiff, tDiffLoLimit, tDiffHiLimit);
					
					hWidthForTimeWalk[i][j] = new TH1F(
					Form("hWidthForTimeWalk_refCh%i_testCh%i_widthBin%i", refCh[getRefModule(i+1)], i+1, j), 
					"", 
					100, widthLoLimitTW, widthHiLimitTW);
					
				}
			}
			
		}	
			return kTRUE;
	}
    
	    Bool_t reinit() {   
			// this function is called for each file
	        // after init()
			// use this place if you need already initialized
	        // containers
			Start2CalCategory = HCategoryManager::getCategory(catStart2Cal);
		    //rpcClus = HCategoryManager::getCategory(catRpcCluster);   //
		    //candCat = HCategoryManager::getCategory(catParticleCand); //
			return kTRUE;
	    }
	
	    Int_t execute() {   
			
			
			//cout << ">>> DEBUG <<< Executing the StartCalibration task..." <<endl;
			
			// this function is called once per event.
			// if the function returns kSkipEvent the event
			// will be skipped a. for all following tasks
			// and b. not appear in output (hades eventLoop())
			
			// this is the part actually responsible for doing the hard work for us
			// find appropriate variables and fill the histograms with them	
			
			Int_t nEntr = Start2CalCategory->getEntries();
			Int_t totalMultiplicityMod0 = 0;
			Int_t totalMultiplicityMod1 = 0;
			Int_t totalMultiplicityMod2 = 0;
			Int_t totalMultiplicityMod3 = 0;
			
			for(Int_t i = 0; i<nEntr; i++) {
				
				//cout << ">>> DEBUG <<< entering i loop, i = " << i << endl;
	                    
				Start2CalObjectRef = (HStart2Cal*) HCategoryManager::getObject(Start2CalObjectRef,Start2CalCategory,i);
	       	    if(!Start2CalObjectRef) continue;
	            	    
			    Int_t moduleRef   = Start2CalObjectRef -> getModule();
	            Int_t channelRef  = Start2CalObjectRef -> getStrip();
	            Int_t indexRef    = nChPerMod*moduleRef + channelRef;
	            if (moduleRef > nModules - 1) continue;
	            
	            //cout << "debug channel, moduleRef = " << moduleRef << " and channelRef = " << channelRef << endl;
	            
	            Int_t multiplicityRef = Start2CalObjectRef -> getMultiplicity();
	            
	            hMultiplicity->Fill(multiplicityRef);
	           if (moduleRef == 0) totalMultiplicityMod0 += multiplicityRef;
	           if (moduleRef == 1) totalMultiplicityMod1 += multiplicityRef;
	           if (moduleRef == 2) totalMultiplicityMod2 += multiplicityRef;
	           if (moduleRef == 3) totalMultiplicityMod3 += multiplicityRef;
				
				// small loop over all entries 
				
				for (Int_t i_mult = 0; i_mult<multiplicityRef; i_mult++) {
					
					
					Double_t widthRef = Start2CalObjectRef -> getWidth(i_mult+1);
					Double_t timeRef = Start2CalObjectRef -> getTime(i_mult+1);		
					hWidthVsChannel -> Fill(indexRef, widthRef);
					hAbsTimeVsChannel -> Fill (indexRef, timeRef);
						
					
				}
				
				for (Int_t j = 0; j<nEntr; j++) {
					
					if (j==i) continue;
					
					//cout << "entering big j loop, j = " << j << endl;
					
					Start2CalObject = (HStart2Cal*) HCategoryManager::getObject(Start2CalObject,Start2CalCategory,j);
					if(!Start2CalObject) continue;
						
					Int_t module   = Start2CalObject -> getModule();
					Int_t channel  = Start2CalObject -> getStrip();
					Int_t index    = nChPerMod*module + channel;
					
					if (module == moduleRef) continue;
					
					Int_t multiplicity = Start2CalObject -> getMultiplicity();
						
					for (Int_t i_mult = 0; i_mult<multiplicityRef; i_mult++) {
							
						//cout << "entering i_mult loop, i_mult = " << i_mult << endl;
						for (Int_t j_mult = 0; j_mult<multiplicity; j_mult++) {
							
							//cout << "entering j_mult loop, j_mult = " << j_mult << endl;
							
							Double_t widthRef = Start2CalObjectRef -> getWidth(i_mult+1);	
							Double_t timeRef = Start2CalObjectRef -> getTime(i_mult+1);	
							
							Double_t width = Start2CalObject -> getWidth(j_mult+1);
							Double_t time = Start2CalObject -> getTime(j_mult+1);
							
							Double_t tDiff = timeRef - time;
							//cout << " about to fill hist, indexRef = " << indexRef << endl;
							hTimeDiffVsChannel[indexRef - 1] -> Fill(index, tDiff);
							
							// time walk analysis
							
							if (indexRef == refCh[0] && widthRef > refWidthLo[0] && widthRef < refWidthHi[0]) {
								
								hTimeDiffVsWidth[index - 1]->Fill(width, tDiff);
								hTimeDiffVsWidthBigTimeRange[index - 1]->Fill(width, tDiff);
								Int_t bin = getBinForTimeWalk(width);
								if (bin>=0 && bin < nPointsForTimeWalk) {
									hTimeDiffForTimeWalk[index - 1][bin]->Fill(tDiff);
									hWidthForTimeWalk[index - 1][bin]->Fill(width);
								}
								
							}
							
							if (indexRef == refCh[1] && widthRef > refWidthLo[1] && widthRef < refWidthHi[1]) {
								
								hTimeDiffVsWidth[index - 1]->Fill(width, tDiff);
								hTimeDiffVsWidthBigTimeRange[index - 1]->Fill(width, tDiff);
								Int_t bin = getBinForTimeWalk(width);
								if (bin>=0 && bin < nPointsForTimeWalk) {
									hTimeDiffForTimeWalk[index - 1][bin]->Fill(tDiff);
									hWidthForTimeWalk[index - 1][bin]->Fill(width);
								}
							}
						}
					}
				}
				
				
	            hMultiplicityPerModule->Fill(1, totalMultiplicityMod0);
	            hMultiplicityPerModule->Fill(2, totalMultiplicityMod1);
	            hMultiplicityPerModule->Fill(3, totalMultiplicityMod2);
	            hMultiplicityPerModule->Fill(4, totalMultiplicityMod3);
				
			}
		
		return 0;
			
		}
	
	    Bool_t finalize() {   
		
		// this function is called once in the end of the
		// runtime of the program. Save histograms to file
		// etc. Tip: Change to your ROOT output file before
		// writing our histograms. You might not be the only
		// user of root files and the current directory could
	        // the one from another user.
		
		std::cout << "Finalizing the StartCalibration task..." << endl;
		
			if(out) {
				
		        out->cd();
		       
		       // write histograms

				hMultiplicity->Write();
				hMultiplicityPerModule->Write();
				hWidthVsChannel->Write();
				hAbsTimeVsChannel->Write();
				
				for (Int_t i = 0; i<nChannels; i++) { 
					
					if (i >= 0*nChPerMod && i < 1*nChPerMod) {
						out->mkdir("time diffs module 0");
						out->cd("time diffs module 0");
					} 
					
					
					else if (i >= 1*nChPerMod && i < 2*nChPerMod) {
						out->mkdir("time diffs module 1");
						out->cd("time diffs module 1");
					} 
					
					
					else if (i >= 2*nChPerMod && i < 3*nChPerMod) {
						out->mkdir("time diffs module 2");
						out->cd("time diffs module 2");
					} 
					
					
					else if (i >= 3*nChPerMod && i < 4*nChPerMod) {
						out->mkdir("time diffs module 3");
						out->cd("time diffs module 3");
					} 
					
					hTimeDiffVsChannel[i]->Write();
					
				}
				
				
				out->mkdir("2D hists for time walk");
				//out->mkdir("1D hists for time walk fitting");
					
				for (Int_t i = 0; i<nChannels; i++) { 
					
					if (skipThisChannel(i+1)) continue;
					
					out->cd("2D hists for time walk");
					hTimeDiffVsWidth[i]->Write();
					hTimeDiffVsWidthBigTimeRange[i]->Write();
					
					for (Int_t j = 0; j<nPointsForTimeWalk; j++) {
						
						//out->cd("1D hists for time walk fitting");
						out->cd();
						hTimeDiffForTimeWalk[i][j]->Write();
						hWidthForTimeWalk[i][j]->Write();
					}
				}
					
				//out->Save();
			    //out->Close(); // don't close the file since the file is handled outside of the task
		       
			}
	
		return kTRUE;
		
	    }

};


#endif
