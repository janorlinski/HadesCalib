#include "checkOwnDst.h"

checkOwnDst::checkOwnDst (const Text_t *name = "", const Text_t *title = "", TFile *pointerToOutFile = NULL) : HReconstructor(name, title) { 
	    
			//hldFileName = fn;
			out = pointerToOutFile;
	    
	    }
	
checkOwnDst::~checkOwnDst () {
	    }
	
Bool_t checkOwnDst::init() {
			
		evt = 0;
			// this function is called once in the beginning
			// create histograms or get pointer to param containers
			// or data containers here. Tip: Create first your
			// output file and and after that your histograms/ ntuples.
			// In this way you make shure the object will end up in your
		    // root file and not any other one.
		
		eventHeader = NULL;
	    Start2CalObject = NULL;
		rpcCalObject = NULL;
	    
	    candCat = HCategoryManager::getCategory(catParticleCand);
		Start2CalCategory = HCategoryManager::getCategory(catStart2Cal);
		rpcCalCategory = HCategoryManager::getCategory(catRpcCal);
		
		
		if(out) {
				
		   out->cd();
		
		// histogram definitions
			
			hVertZ = new TH1F("hVertZ", "z distribution of event vertex; z_{vert} [mm]; Counts", 1800, -200, 100);
			hVertChi2 = new TH1F("hVertChi2", "vertex chi2; #chi^{2}; Counts", 1000, 0, 200);
			hPartZ = new TH1F("hPartZ", "z distribution of tracks; z_{part} [mm]", 1800, -200, 100);
			hVertZVsChi2 = new TH2F("hVertZVsChi2", "z distribution of evt vrtx vs. vrtx chi2; chi2; z_{vert} [mm]", 1000, 0, 200, 1800, -200, 100);
			hVertZ_used = new TH1F("hVertZ_used", "z distribution of event vertex; z_{vert} [mm]; Counts", 1800, -200, 100);
			hVertChi2_used = new TH1F("hVertChi2_used", "vertex chi2; #chi^{2}; Counts", 1000, 0, 200);
			hPartZ_used = new TH1F("hPartZ_used", "z distribution of tracks; z_{part} [mm]", 1800, -200, 100);
			
		    hBetaMomentum = new TH2F("hBetaMomentum", "beta vs. mom; mom*charge; beta", 800, -2000, 2000, 420, 0, 1.4);
		    hBetaMomentumSys0 = new TH2F("hBetaMomentumSys0", "beta vs. mom Sys0; mom*charge; beta", 800, -2000, 2000, 420, 0, 1.4);
		    hBetaMomentumSys1 = new TH2F("hBetaMomentumSys1", "beta vs. mom Sys1; mom*charge; beta", 800, -2000, 2000, 420, 0, 1.4);
			
			evtCounter = new TH1I("evtCounter", "event counter", 2, 0-0.5, 2-0.5);
			evtCounter->GetXaxis()->SetBinLabel(1, "all events");
			evtCounter->GetXaxis()->SetBinLabel(2, "3 fastest");
		
			for (Int_t i=0; i<6; i++) { // loop over sectors
			   
				hTimeDifference[i] = new TH2F(
				Form("hTimeDifference_sect%i",i+1),
				Form("ToA difference vs. cell number, sector %i; 32 #times module + cell; t diff [ns]", i+1),
				nCells, loLimitCells, hiLimitCells, 800,-10, 10); // originally 200, -5, 5
				 
				hTimeDifferenceOneTargetSegment[i] = new TH2F(
				Form("hTimeDifferenceOneTargetSegment_sect%i",i+1),
				Form("ToA difference vs. cell number, one target segment, sector %i; 32 #times module + cell; t diff [ns]", i+1),
				nCells, loLimitCells, hiLimitCells, 800,-10, 10); // originally 200, -5, 5
				  		
			    for (Int_t j=0; j<40; j++) {
					
					hTimeDifferenceOneStartStrip[j][i] = new TH2F(
					Form("hTimeDifferenceOneStartStrip_strip_%i_sect%i",j+1, i+1),
					Form("ToA difference vs. cell number, start strip %i, sector %i; 32 #times module + cell; t diff [ns]", j+1, i+1),
					nCells, loLimitCells, hiLimitCells, 800,-10, 10); // originally 200, -5, 5
				
					hTimeDifferenceOneStartStripOneTargetSegment[j][i] = new TH2F(
					Form("hTimeDifferenceOneStartStripOneTargetSegment_strip_%i_sect%i", j+1, i+1),
					Form("ToA difference vs. cell number, one target segment, start strip %i, sector %i; 32 #times module + cell; t diff [ns]", j+1, i+1),
					nCells, loLimitCells, hiLimitCells, 800,-10, 10); // originally 200, -5, 5
					
				}	
			}
			
		}	
			return kTRUE;
	}
    
Bool_t checkOwnDst::reinit() {
	
		// this function is called for each file
        // after init()
		// use this place if you need already initialized
        // containers
        
		rpcCalCategory = HCategoryManager::getCategory(catRpcCal);
	    candCat = HCategoryManager::getCategory(catParticleCand); //
		Start2CalCategory = HCategoryManager::getCategory(catStart2Cal);
		return kTRUE;
    }

Int_t checkOwnDst::execute() {
		
		
		//cout << "\n NEW EVENT! evt nmbr = " << evt << endl;
		
		// this function is called once per event.
		// if the function returns kSkipEvent the event
		// will be skipped a. for all following tasks
		// and b. not appear in output (hades eventLoop())
		
		// this is the part actually responsible for doing the hard work for us
		// find appropriate variables and fill the histograms with them

		//std::cout << "execute!" << endl;

        HIterator *iterParticleCand = NULL;
        HIterator *iterStart2Cal = NULL;
        candCat = HCategoryManager::getCategory(catParticleCand);
		Start2CalCategory = HCategoryManager::getCategory(catStart2Cal);
        
		eventHeader  = gHades->getCurrentEvent()->getHeader();
        HVertex vertex      = eventHeader->getVertexReco();
		Float_t eventVerZ = vertex.getZ();
		Float_t eventVerChi2 = vertex.getChi2();
		
		hVertChi2->Fill(eventVerChi2);
		hVertZ->Fill(eventVerZ);
		hVertZVsChi2->Fill(eventVerChi2, eventVerZ);
		
		if (eventVerZ >= minVerZ && eventVerZ <= maxVerZ && eventVerChi2 <= maxVerChi2) {
			
			hVertChi2_used->Fill(eventVerChi2);
			hVertZ_used->Fill(eventVerZ);
			
		}
        
		if (!candCat) std::cout << "no hparticlecand?! \n";
        else if (!Start2CalCategory) std::cout << "no start2cal?! \n";
        else if (!Start2CalCategory) std::cout << "no start2cal?! \n";
        
        else {
						
            iterParticleCand = (HIterator *) candCat->MakeIterator("native");
            iterStart2Cal = (HIterator *)    Start2CalCategory->MakeIterator("native");
            
            iterParticleCand->Reset();
            iterStart2Cal->Reset();
			Float_t startTime  = findEvtStartTime();   
			
	        //event counter(s)
	        
	        evtCounter->Fill(0); // add every event to the first event
	        if (GetMinTofCand() >= 3) evtCounter->Fill(1); // cut on at least 3 fastest particles in tof
	        
            while (NULL != (cand = static_cast<HParticleCand*>(iterParticleCand->Next()))) { //ParticleCandIter
				
	            if (cand->getChi2() > 0 && cand->getChi2() < 1000) {
					
					Float_t beta = cand->getBeta();
					Float_t mom = cand->getMomentum();
					Float_t chrg = cand->getCharge();
					Int_t sector = cand -> getSector();
					
					hPartZ->Fill(cand->getZ());
					hBetaMomentum->Fill(mom*chrg, beta);
					if (cand->getSystemUsed()==0) hBetaMomentumSys0->Fill(mom*chrg, beta);
					else if (cand->getSystemUsed()==1) hBetaMomentumSys1->Fill(mom*chrg, beta);
					
					Float_t MASS = 0;            
		            if (cand->getCharge()==-1) MASS = 139.57;
		            else if (cand->getCharge()==1) MASS = 139.57;
					else continue;
			  
				    if (cand->getSystemUsed()==0){ 			
						
						//Int_t indexRPCHit = cand->getRpcInd();
						//Int_t indexMetaHit = cand->getMetaInd();
						Int_t metaCell = cand->getMetaCell(0);
						Int_t metaModule = cand->getMetaModule(0);
			            Int_t index = 32*metaModule + metaCell;
			            
					    Float_t MIN_MOM = 50; 
		
						Float_t Tof;
		                Float_t beta_c = (mom)/TMath::Sqrt(mom*mom+MASS*MASS);
		                Float_t tof_c;
		                Float_t time_diff;
						
						if (mom > MIN_MOM && cand->getCharge() < 0) {   
							
							Tof = cand->getTof();
							tof_c  = beta / beta_c;
							time_diff =  Tof * (1.0 - tof_c);
			            
									if (beta>=betaCut) {
										hTimeDifference[sector]->Fill(index, time_diff);
										if (eventVerZ >= minVerZ && eventVerZ <= maxVerZ) { //impose beta cuts and vertex cuts 
											
											//cout << "verz = " << eventVerZ << ", verchi2 = " << eventVerChi2 << endl;
											if (eventVerChi2 > maxVerChi2) continue;
											
												//cout << "passed chi2 test! verz = " << eventVerZ << ", verchi2 = " << eventVerChi2 << endl;
												hTimeDifferenceOneTargetSegment[sector]->Fill(index, time_diff);									
												hPartZ_used->Fill(cand->getZ());
										}
									}
								}
									
							// THIS SECTION IS FOR RPC TDIFF vs. START STRIPE 
							
				            while (NULL != (Start2CalObject = static_cast<HStart2Cal*>(iterStart2Cal->Next()))) { // get all the start2cal objects
							
								Int_t nOfStartHits = Start2CalObject->getMultiplicity();
								Int_t startIndex = Start2CalObject->getModule() * 20 + Start2CalObject->getStrip();
							
								if (cand->getCharge() < 0) { //select negative tracks
							
									for (Int_t i=0; i<nOfStartHits; i++) { // loop over all start hits
										
										Float_t width = Start2CalObject->getWidth(i+1);
										Float_t calTime = Start2CalObject->getTime(i+1);
										Float_t time_diff_recalc = time_diff + startTime - calTime;
										
										hTimeDifferenceOneStartStrip[startIndex][sector]->Fill(index, time_diff_recalc);
										
										if (beta>=betaCut && eventVerZ >= minVerZ && eventVerZ <= maxVerZ && eventVerChi2 <= maxVerChi2) { //impose beta cuts and vertex cuts 
											hTimeDifferenceOneStartStripOneTargetSegment[startIndex][sector]->Fill(index, time_diff);
										}
										
									} // end of nOfStartHits loop
								} // end of select negative tracks
					    
							} //end of start2cal iter
							
	                    
	                }
				}
			}
		} 
		
	evt++;
	
	return 0;
		
	}

Bool_t checkOwnDst::finalize() {
	
	// this function is called once in the end of the
	// runtime of the program. Save histograms to file
	// etc. Tip: Change to your ROOT output file before
	// writing our histograms. You might not be the only
	// user of root files and the current directory could
    // the one from another user.

	cout << "Finalizing the checkOwnDst task ... " << endl;

	if(out) {
		
		out->cd();
		
	    hPartZ->Write();
	    hVertZ->Write();
	    hVertChi2->Write();
	    hVertZVsChi2->Write();
	    hPartZ_used->Write();
	    hVertZ_used->Write();
	    hVertChi2_used->Write();
		hBetaMomentum->Write();
		hBetaMomentumSys0->Write();
		hBetaMomentumSys1->Write();
		evtCounter->Write();
		
		out->mkdir("OneStartStrip");
		for (Int_t i=0; i<6; i++) {
			out->cd();
			hTimeDifference[i]->Write();
			hTimeDifferenceOneTargetSegment[i]->Write();
			
			for (Int_t j=0; j<40; j++) {
				
				out->cd("OneStartStrip");
				hTimeDifferenceOneStartStrip[j][i]->Write();
				hTimeDifferenceOneStartStripOneTargetSegment[j][i]->Write();
			}
		}
    }

	return kTRUE;
	
}
