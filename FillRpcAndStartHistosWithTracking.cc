#include "FillRpcAndStartHistosWithTracking.h"

FillRpcAndStartHistosWithTracking::FillRpcAndStartHistosWithTracking (const Text_t *name = "", const Text_t *title ="", TString fn = "defaultfilename.hld", TFile *pointerToOutFile = NULL) : HReconstructor(name, title) { 
	    
			hldFileName = fn;
			out = pointerToOutFile;
	    
	    }
	
FillRpcAndStartHistosWithTracking::~FillRpcAndStartHistosWithTracking () {
	    }
	
Bool_t FillRpcAndStartHistosWithTracking::init() {
		
		cout << "This is FillRpcAndStartHistosWithTracking init()" << endl;
		
		evt = 0;
		// this function is called once in the beginning
		// create histograms or get pointer to param containers
		// or data containers here. Tip: Create first your
		// output file and and after that your histograms/ ntuples.
		// In this way you make shure the object will end up in your
	    // root file and not any other one.
		
		eventHeader = NULL;
	    Start2CalObject = NULL;
	    Start2CalObjectRef = NULL;
		rpcCalObject = NULL;
	    //Start2HitObject = NULL;
	    
	    candCat = HCategoryManager::getCategory(catParticleCand);
		Start2CalCategory = HCategoryManager::getCategory(catStart2Cal);
		rpcCalCategory = HCategoryManager::getCategory(catRpcCal);
		//Start2HitCategory = HCategoryManager::getCategory(catStart2Hit);
		
		// get beta momentum banana cuts for pions - if needed
		
		TFile* fCuts = new TFile("cuts.root", "READ");
		cutPositive = (TCutG*) fCuts->Get("CUTG;1");
		cutNegative = (TCutG*) fCuts->Get("CUTG;2");
		
		if(out) {
				
		   out->cd();
		
		// histogram definitions
			
			hVertexDist = new TH1F("hVertexDist", "z distribution; z [mm]", 1800, -200, 100);
		    hBetaMomentum = new TH2F("hBetaMomentum", "beta vs. mom; mom*charge; beta", 800, -2000, 2000, 420, 0, 1.4);
		    hBetaMomentumSys0 = new TH2F("hBetaMomentumSys0", "beta vs. mom Sys0; mom*charge; beta", 800, -2000, 2000, 420, 0, 1.4);
		    hBetaMomentumSys1 = new TH2F("hBetaMomentumSys1", "beta vs. mom Sys1; mom*charge; beta", 800, -2000, 2000, 420, 0, 1.4);
			
			hMultiplicityPerModule = new TH2F("hMultiplicityPerModule", "START multiplicity per module; module; mult", nModules, 0.5, nModules + 0.5, 15, 0, 15);
			hMultiplicityPerModuleNoTimeLimit = new TH2F("hMultiplicityPerModuleNoTimeLimit", "START multiplicity per module NoTimeLimit; module; mult", nModules, 0.5, nModules + 0.5, 15, 0, 15);
			
			hFelixClusterNumberMod0 = new TH1F("hFelixClusterNumberMod0", "Number of clusters per event, Felix' code, mod 0; Cluster number;", 100, -0.5, 99.5);
			hFelixClusterNumberMod1 = new TH1F("hFelixClusterNumberMod1", "Number of clusters per event, Felix' code, mod 1; Cluster number;", 100, -0.5, 99.5);
			hFelixClusterNumberMod3 = new TH1F("hFelixClusterNumberMod3", "Number of clusters per event, Felix' code, mod 3; Cluster number;", 100, -0.5, 99.5);
			hFelixClusterNumberAllMods = new TH1F("hFelixClusterNumberAllMods", "Number of clusters per event, Felix' code, all mods; Cluster number;", 200, -0.5, 199.5);
			
			hFelixClusterSizeMod0 = new TH1F("hFelixClusterSizeMod0", "Size of clusters, Felix' code, mod 0; Cluster size;", 100, -0.5, 99.5);
			hFelixClusterSizeMod1 = new TH1F("hFelixClusterSizeMod1", "Size of clusters, Felix' code, mod 1; Cluster size;", 100, -0.5, 99.5);
			hFelixClusterSizeMod3 = new TH1F("hFelixClusterSizeMod3", "Size of clusters, Felix' code, mod 3; Cluster size;", 100, -0.5, 99.5);
			hFelixClusterSizeAllMods = new TH1F("hFelixClusterSizeAllMods", "Size of clusters, Felix' code, all mods; Cluster size;", 100, -0.5, 99.5);
			
			hJanClusterSizeMod0 = new TH1F("hJanClusterSizeMod0", "Size of clusters, Jan's code, mod 0; Cluster size;", 100, -0.5, 99.5);
			hJanClusterSizeMod1 = new TH1F("hJanClusterSizeMod1", "Size of clusters, Jan's code, mod 1; Cluster size;", 100, -0.5, 99.5);
			hJanClusterSizeMod3 = new TH1F("hJanClusterSizeMod3", "Size of clusters, Jan's code, mod 3; Cluster size;", 100, -0.5, 99.5);
			hJanClusterSizeAllMods = new TH1F("hJanClusterSizeAllMods", "Size of clusters, Jan's code, all mods; Cluster size;", 100, -0.5, 99.5);
			
			hStartVetoCorrelation = new TH2F("hStartVetoCorrelation", "Correlation between START and VETO hits; VETO channel; START channel", 20, 60.5, 80.5, 40, 0.5, 40.5);
			
			hStartEfficiency = new TH1F("hStartEfficiency", "START Efficiency; Event types; Event fraction [%]", 8, 0.5, 8.5);
			hStartEfficiencyFelix = new TH1F("hStartEfficiencyFelix", "START Efficiency Felix' code; VETO channel; START channel", 4, 0.5, 4.5);
			hStartEfficiencyJan = new TH1F("hStartEfficiencyJan", "START Efficiency Jan's code; VETO channel; START channel", 4, 0.5, 4.5);
			
			hToTVsStartStrip = new TH2F("hToTVsStartStrip", "start ToT vs. start ch", nChannels, 0.5, nChannels + 0.5, 200, 0, 50);
			hAbsTimeVsStartStrip = new TH2F("hAbsTimeVsStartStrip", "start abs time vs. start ch", nChannels, 0.5, nChannels + 0.5, 2000, -1000, 1000);
			hRpcTimeDifferenceVsStartStrip = new TH2F("hRpcTimeDifferenceVsStartStrip", "tdiff rpc vs. start ch", nChannels, 0.5, nChannels + 0.5, 800, -20, 20);
			hRpcTimeDifferenceVsStartStripLocalMaximaOnly = new TH2F("hRpcTimeDifferenceVsStartStripLocalMaximaOnly", "tdiff rpc vs. start ch LocalMaximaOnly", nChannels, 0.5, nChannels + 0.5, 800, -20, 20);
			hRpcTimeDifferenceVsStartStripBigRange = new TH2F("hRpcTimeDifferenceVsStartStripAllTracks", "tdiff rpc big range vs. start ch", nChannels, 0.5, nChannels + 0.5, 1000, -50, 50);
			
			hNegativeTracksOneFile[6] = new TH1F("hNegativeTracksOneFile", "negative tracks vs start strip, all sectors", nChannels, 0.5, nChannels + 0.5);
			hPionsBetaCutOneFile[6] = new TH1F("hPionsBetaCutOneFile", "pions from beta cuts vs start strip, all sectors", nChannels, 0.5, nChannels + 0.5);
			hPionsBananaCutOneFile[6] = new TH1F("hPionsBananaCutOneFile", "pions from banana cut vs start strip, all sectors", nChannels, 0.5, nChannels + 0.5);
			hPionsBananaCutAllStripsOneFile = new TH1F("hPionsBananaCutAllStripsOneFile", "pions from banana cut vs. sectors", 6, 0.5, 6.5);
			minimumTimeOneFile = new TH1F("minimumTimeOneFile", "minimum META time", 400, 0.0, 20.0);
			
			evtCounter = new TH1I("evtCounter", "event counter", 2, 0-0.5, 2-0.5);
			evtCounter->GetXaxis()->SetBinLabel(1, "all events");
			evtCounter->GetXaxis()->SetBinLabel(2, "3 fastest");
		
			for (Int_t i=0; i<6; i++) { // loop over sectors
			   
				hNegativeTracksOneFile[i] = new TH1F(
				Form("hNegativeTracksOneFile_sect%i", i+1), Form("negative tracks vs start strip, sector %i", i+1), 
				80, -0.5, 79.5);
				
				hPionsBetaCutOneFile[i] = new TH1F(
				Form("hPionsBetaCutOneFile_sect%i", i+1), Form("pions from beta cuts vs start strip, sector %i", i+1), 
				80, -0.5, 79.5);
				
				hPionsBananaCutOneFile[i] = new TH1F(
				Form("hPionsBananaCutOneFile_sect%i", i+1), Form("pions from banana cut vs start strip, sector %i", i+1),
				80, -0.5, 79.5);
			   
				hXposDifference[i] = new TH2F(
				Form("hXposDifference_sect%i",i+1),
				Form("X position difference vs. cell number, sector %i; 32 #times module + cell; x diff", i+1),
				nCells, loLimitCells, hiLimitCells, 400,-100, 100);
				
				hTimeDifference[i] = new TH2F(
				Form("hTimeDifference_sect%i",i+1),
				Form("ToA difference vs. cell number, sector %i; 32 #times module + cell; t diff [ns]", i+1),
				nCells, loLimitCells, hiLimitCells, 800,-10, 10); // originally 200, -5, 5
				
				hTimeDifferenceBigRange[i] = new TH2F(
				Form("hTimeDifferenceBigRange_sect%i",i+1),
				Form("ToA difference vs. cell number [big range], sector %i; 32 #times module + cell; t diff [ns]", i+1), 
				nCells, loLimitCells, hiLimitCells, 2000, -100, 100);
				
				hLeftChargeVsCellNumber[i] = new TH2F(
				Form("hLeftChargeVsCellNumber_sect%i", i+1),
				Form("Left charge vs. cell number, sector %i; 32 #times module + cell; charge", i+1), 
				nCells, loLimitCells, hiLimitCells, 300, -20.0, 30.0);
				   
				hRightChargeVsCellNumber[i] = new TH2F(
				Form("hRightChargeVsCellNumber_sect%i", i+1), 
				Form("Right charge vs. cell number, sector %i; 32 #times module + cell; charge", i+1), 
				nCells, loLimitCells, hiLimitCells, 300, -20.0, 30.0);
					   		
			}
			
			for (Int_t i = 0; i<nChannels; i++) { // loop over start channels
				
				hTimeDiffVsChannel[i] = new TH2F(
				Form("hTimeDiffVsChannel_refCh%i", i+1), 
				Form("Time difference vs. channels, reference channel = %i; %i #times module + channel; t diff", i+1, nChPerMod), 
				nChannels, loLimitChannels, hiLimitChannels, nBinsForTimeDiff, tDiffLoLimit, tDiffHiLimit);
				
				hTimeDiffVsChannelLocalMaximaOnly[i] = new TH2F(
				Form("hTimeDiffVsChannelLocalMaximaOnly_refCh%i", i+1), 
				Form("Time difference vs. channels, only local maxima, reference channel = %i; %i #times module + channel; t diff", i+1, nChPerMod), 
				nChannels, loLimitChannels, hiLimitChannels, nBinsForTimeDiff, tDiffLoLimit, tDiffHiLimit);
				
				hTimeDiffVsChannelLocalMaximaOnlyTimeCut[i] = new TH2F(
				Form("hTimeDiffVsChannelLocalMaximaOnlyTimeCut_refCh%i", i+1), 
				Form("Time difference vs. channels, only local maxima, cal time cut, reference channel = %i; %i #times module + channel; t diff", i+1, nChPerMod), 
				nChannels, loLimitChannels, hiLimitChannels, nBinsForTimeDiff, tDiffLoLimit, tDiffHiLimit);
		  
				hTimeDiffVsWidth[i] = new TH2F(
				Form("hTimeDiffVsWidth_%i", i+1), 
				Form("histogram for time walk, ref%i, test%i; test width; t_{ref} - t_{test}", refCh[getRefModule(i+1)], i+1), 
				nBinsForWidth, widthLoLimit, widthHiLimit, nBinsForTimeDiff, tDiffLoLimit, tDiffHiLimit);
		   
				hTimeDiffVsWidthLocalMaximaOnly[i] = new TH2F(
				Form("hTimeDiffVsWidthLocalMaximaOnly_%i", i+1), 
				Form("histogram for time walk, only local maxima, ref%i, test%i; test width; t_{ref} - t_{test}", refCh[getRefModule(i+1)], i+1), 
				nBinsForWidth, widthLoLimit, widthHiLimit, nBinsForTimeDiff, tDiffLoLimit, tDiffHiLimit);
		   
				hRpcTimeDiffVsWidth[i] = new TH2F(
				Form("hRpcTimeDiffVsWidth_%i", i+1), 
				Form("histogram for time walk with RPC tdiffs, ref%i, test%i; test width; t_{ref} - t_{test}", refCh[getRefModule(i+1)], i+1), 
				nBinsForWidth, widthLoLimit, widthHiLimit, nBinsForTimeDiff, tDiffLoLimit, tDiffHiLimit);
		   
				hRpcTimeDiffVsWidthLocalMaximaOnly[i] = new TH2F(
				Form("hRpcTimeDiffVsWidthLocalMaximaOnly_%i", i+1), 
				Form("histogram for time walk with RPC tdiffs LocalMaximaOnly, ref%i, test%i; test width; t_{ref} - t_{test}", refCh[getRefModule(i+1)], i+1), 
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
    
Bool_t FillRpcAndStartHistosWithTracking::reinit() {   
	
		// this function is called for each file
        // after init()
		// use this place if you need already initialized
        // containers
		rpcCalCategory = HCategoryManager::getCategory(catRpcCal);
	    //rpcClus = HCategoryManager::getCategory(catRpcCluster);   //
	    candCat = HCategoryManager::getCategory(catParticleCand); //
		Start2CalCategory = HCategoryManager::getCategory(catStart2Cal);
		//Start2HitCategory = HCategoryManager::getCategory(catStart2Hit);
		return kTRUE;
    }

Int_t FillRpcAndStartHistosWithTracking::execute() {   
		
		// bools to check start efficiency
		
		Bool_t goodHitInVeto = false;
		Bool_t goodHitInMod0 = false;
		Bool_t goodHitInMod1 = false;
		//Bool_t goodHitInBoth = false;
		
		//cout << "\n \n \n NEW EVENT! evt nmbr = " << evt << endl;
		
		// this function is called once per event.
		// if the function returns kSkipEvent the event
		// will be skipped a. for all following tasks
		// and b. not appear in output (hades eventLoop())
		
		// this is the part actually responsible for doing the hard work for us
		// find appropriate variables and fill the histograms with them

		//std::cout << "execute!" << endl;

        HIterator *iterParticleCand = NULL;
        HIterator *iterStart2Cal = NULL;
        //HIterator *iterStart2Hit = NULL;
        candCat = HCategoryManager::getCategory(catParticleCand);
		Start2CalCategory = HCategoryManager::getCategory(catStart2Cal);
		//Start2HitCategory = HCategoryManager::getCategory(catStart2Hit);
        
        if (!candCat) std::cout << "no hparticlecand?! \n";
        else if (!Start2CalCategory) std::cout << "no start2cal?! \n";
        else if (!Start2CalCategory) std::cout << "no start2cal?! \n";
        //else if (!Start2HitCategory) std::cout << "no start2hit?! \n";
        
        else {
						
            iterParticleCand = (HIterator *) candCat->MakeIterator("native");
            iterStart2Cal = (HIterator *)    Start2CalCategory->MakeIterator("native");
            //iterStart2Hit = (HIterator *)    Start2HitCategory->MakeIterator("native");
            
            iterParticleCand->Reset();
            iterStart2Cal->Reset();
            //iterStart2Hit->Reset();
                  
            // function below taken from qamaker, see function body for details
			Float_t startTime  = findEvtStartTime();   
			
	        //event counter(s)
	        
	        evtCounter->Fill(0); // add every event to the first event
	        if (GetMinTofCand() >= 3) evtCounter->Fill(1); // cut on at least 3 fastest particles in tof
	        
	        // find minimum time suggested by Willy
	        
	        minimumTimeOneFile->Fill(findMinimumTime(3));
	        
	        
	        JansClusterFinder clfMod0(5.0);
	        JansClusterFinder clfMod1(5.0);
	        JansClusterFinder clfMod3(5.0);
	        
	        // START STUFF WITHOUT TRACKING
	        
			Int_t nEntr = Start2CalCategory->getEntries();
			
	        // cout << nEntr << " Start2CalCategory entries in this event ... " << endl;
			
			Int_t totalMultiplicityMod0 = 0;
			Int_t totalMultiplicityMod1 = 0;
			Int_t totalMultiplicityMod2 = 0;
			Int_t totalMultiplicityMod3 = 0;
			
			Int_t totalMultiplicityNoTimeLimitMod0 = 0;
			Int_t totalMultiplicityNoTimeLimitMod1 = 0;
			Int_t totalMultiplicityNoTimeLimitMod2 = 0;
			Int_t totalMultiplicityNoTimeLimitMod3 = 0;
			
	        //cout << "entering the loop" << endl;
	        
	        Bool_t foundAVetoHit = false;
	        Int_t scoreVeto = 0;
	        Int_t scoreMod0 = 0;
	        Int_t scoreMod1 = 0;
	        Int_t scoreModBoth = 0;
	        
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
	            
				if (moduleRef == 0) totalMultiplicityNoTimeLimitMod0 += multiplicityRef;
				if (moduleRef == 1) totalMultiplicityNoTimeLimitMod1 += multiplicityRef;
				if (moduleRef == 2) totalMultiplicityNoTimeLimitMod2 += multiplicityRef;
				if (moduleRef == 3) totalMultiplicityNoTimeLimitMod3 += multiplicityRef;
				
				for (Int_t i_mult = 0; i_mult<multiplicityRef; i_mult++) {
					
					
					Double_t widthRef = Start2CalObjectRef -> getWidth(i_mult+1);
					Double_t timeRef = Start2CalObjectRef -> getTime(i_mult+1);
							
					hToTVsStartStrip -> Fill(indexRef, widthRef);
					hAbsTimeVsStartStrip -> Fill(indexRef, timeRef);
					
					//cout << "Start hit: mod" << moduleRef << "strip" << channelRef <<", time = " << timeRef << ", width = " << widthRef << "\n";
					
					// check and update bools for efficiency
					if (moduleRef==3 && ((timeRef > -500. && timeRef < -50.) || (timeRef > 50. && timeRef < 500.))) goodHitInVeto = true;
					
					if (moduleRef == 0) clfMod0.addHit(moduleRef, channelRef, widthRef, timeRef);
					if (moduleRef == 1) clfMod1.addHit(moduleRef, channelRef, widthRef, timeRef);
					if (moduleRef == 3) clfMod3.addHit(moduleRef, channelRef, widthRef, timeRef);

					if (timeRef >= loTimeLimitForMultiplicity && timeRef <= hiTimeLimitForMultiplicity) {
						if (moduleRef == 0) totalMultiplicityMod0 += multiplicityRef;
						if (moduleRef == 1) totalMultiplicityMod1 += multiplicityRef;
						if (moduleRef == 2) totalMultiplicityMod2 += multiplicityRef;
						if (moduleRef == 3) totalMultiplicityMod3 += multiplicityRef;
					}
						
					
				}
				
				for (Int_t j = 0; j<nEntr; j++) { //another loop over all start2cal objects to get start vs. start references
					
					if (j==i) continue;
					
					//cout << "Entering j loop, j = " << j << endl;
					
					Start2CalObject = (HStart2Cal*) HCategoryManager::getObject(Start2CalObject,Start2CalCategory,j);
					if(!Start2CalObject) continue;
						
					Int_t module   = Start2CalObject -> getModule();
					Int_t channel  = Start2CalObject -> getStrip();
					Int_t index    = nChPerMod*module + channel;
					
					if (module == moduleRef) continue;
					
					Int_t multiplicity = Start2CalObject -> getMultiplicity();
						
					for (Int_t i_mult = 0; i_mult<multiplicityRef; i_mult++) {
						for (Int_t j_mult = 0; j_mult<multiplicity; j_mult++) {
							
							Double_t widthRef = Start2CalObjectRef -> getWidth(i_mult+1);	
							Double_t timeRef = Start2CalObjectRef -> getTime(i_mult+1);	
							
							Double_t width = Start2CalObject -> getWidth(j_mult+1);
							Double_t time = Start2CalObject -> getTime(j_mult+1);
							
							Double_t tDiff = timeRef - time;
							
							
							//check and update bools for efficiency
							//if ((moduleRef==3 && ((timeRef > -500. && timeRef < -50.) || (timeRef > 50. && timeRef < 500.))) && abs(tDiff)<5.0 && (module == 0 || module == 1)) goodHitInBoth = true;
							if ((moduleRef==3 && ((timeRef > -500. && timeRef < -50.) || (timeRef > 50. && timeRef < 500.))) && abs(tDiff)<5.0 && module == 0) goodHitInMod0 = true;
							if ((moduleRef==3 && ((timeRef > -500. && timeRef < -50.) || (timeRef > 50. && timeRef < 500.))) && abs(tDiff)<5.0 && module == 1) goodHitInMod1 = true;
							
							hTimeDiffVsChannel[indexRef - 1] -> Fill(index, tDiff);
							if (moduleRef == 3 && time > timeRef-2.0 && time < timeRef+2.0) hStartVetoCorrelation->Fill(indexRef, index);
							
							// time walk analysis
							
							if (indexRef == refCh[getRefModule(1)] && widthRef > refWidthLo[0] && widthRef < refWidthHi[0]) {
								
								hTimeDiffVsWidth[index - 1]->Fill(width, tDiff);
								hTimeDiffVsWidthBigTimeRange[index - 1]->Fill(width, tDiff);
								Int_t bin = getBinForTimeWalk(width);
								if (bin>=0 && bin < nPointsForTimeWalk) {
									hTimeDiffForTimeWalk[index - 1][bin]->Fill(tDiff);
									hWidthForTimeWalk[index - 1][bin]->Fill(width);
								}
								
							}
							
							if (indexRef == refCh[getRefModule(1+nChPerMod)] && widthRef > refWidthLo[1] && widthRef < refWidthHi[1]) {
								
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
				
	            hMultiplicityPerModuleNoTimeLimit->Fill(1, totalMultiplicityNoTimeLimitMod0);
	            hMultiplicityPerModuleNoTimeLimit->Fill(2, totalMultiplicityNoTimeLimitMod1);
	            hMultiplicityPerModuleNoTimeLimit->Fill(3, totalMultiplicityNoTimeLimitMod2);
	            hMultiplicityPerModuleNoTimeLimit->Fill(4, totalMultiplicityNoTimeLimitMod3);
				
			}
			
			//cout << "Resetting iterstart2cal..." << endl;
			
			iterStart2Cal->Reset(); // return to 0 for the next loop
			
			clfMod0.findClusters();
			clfMod1.findClusters();
			clfMod3.findClusters();
			
			for (Int_t i=0; i<clfMod3.getNumberOfClusters(); i++) {
				
				//Int_t clusterSize = 0;
				Float_t refToA = clfMod3.getClusterTime(i);
				Int_t refChID = 60 + clfMod3.getClusterStrip(i);
				
				//cout << "analyzing clusters with reference index " << refChID << endl;
				
				// for all local maxima in veto, get time differences to other local maxima in modules 0 and 1 
				for (Int_t j=0; j<clfMod0.getNumberOfClusters(); j++) {
					
					Float_t testToA = clfMod0.getClusterTime(j);
					Float_t testToT = clfMod0.getClusterTot(j);
					Int_t testChID = clfMod0.getClusterStrip(j);
					hTimeDiffVsChannelLocalMaximaOnly[refChID-1]->Fill(testChID, refToA-testToA);
					if (abs(refToA)>50.0) hTimeDiffVsChannelLocalMaximaOnlyTimeCut[refChID-1]->Fill(testChID, refToA-testToA);
					hTimeDiffVsWidthLocalMaximaOnly[testChID-1]->Fill(testToT, refToA-testToA);
					
				}
				for (Int_t j=0; j<clfMod1.getNumberOfClusters(); j++) {
					
					Float_t testToA = clfMod1.getClusterTime(j);
					Float_t testToT = clfMod1.getClusterTot(j);
					Int_t testChID = 20 + clfMod1.getClusterStrip(j);
					hTimeDiffVsChannelLocalMaximaOnly[refChID-1]->Fill(testChID, refToA-testToA);
					hTimeDiffVsWidthLocalMaximaOnly[testChID-1]->Fill(testToT, refToA-testToA);
					
				}
			}
				
			// cluster size analysis - jan's code
			
			for (Int_t i=0; i<clfMod0.getNumberOfClusters(); i++) {
				//cout << "JAN cluster in mod" << clfMod0.getClusterModule(i) << "strip" << clfMod0.getClusterStrip(i) << ", time = " << clfMod0.getClusterTime(i) << ", width = " <<  clfMod0.getClusterTot(i) << " and clustersize = " << clfMod0.getClusterSize(i) << endl;
				hJanClusterSizeMod0->Fill(clfMod0.getClusterSize(i));
			}
			for (Int_t i=0; i<clfMod1.getNumberOfClusters(); i++) {
				//cout << "JAN cluster in mod" << clfMod1.getClusterModule(i) << "strip" << clfMod1.getClusterStrip(i) << ", time = " << clfMod1.getClusterTime(i) << ", width = " <<  clfMod1.getClusterTot(i) << " and clustersize = " << clfMod1.getClusterSize(i) << endl;
				hJanClusterSizeMod1->Fill(clfMod1.getClusterSize(i));
			}
			for (Int_t i=0; i<clfMod3.getNumberOfClusters(); i++) {
				//cout << "JAN cluster in mod" << clfMod3.getClusterModule(i) << "strip" << clfMod3.getClusterStrip(i) << ", time = " << clfMod3.getClusterTime(i) << ", width = " <<  clfMod3.getClusterTot(i) << " and clustersize = " << clfMod3.getClusterSize(i) << endl;
				hJanClusterSizeMod3->Fill(clfMod3.getClusterSize(i));
			}
			
			
			//cout << "Beginning the rpc loop" << endl;
			
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
		
            while (NULL != (cand = static_cast<HParticleCand*>(iterParticleCand->Next()))) { //ParticleCandIter
				
	            if (cand->getChi2() > 0 && cand->getChi2() < 1000) {
					
					Float_t beta = cand->getBeta();
					Float_t mom = cand->getMomentum();
					Float_t chrg = cand->getCharge();
					Int_t sector = cand -> getSector();
					
					hVertexDist->Fill(cand->getZ());
					hBetaMomentum->Fill(mom*chrg, beta);
					if (cand->getSystemUsed()==0) hBetaMomentumSys0->Fill(mom*chrg, beta);
					else if (cand->getSystemUsed()==1) hBetaMomentumSys1->Fill(mom*chrg, beta);
										
					// GENERAL STUFF				
					
					//if (cutPositive->IsInside(mom*chrg, beta) || 
					//cutNegative->IsInside(mom*chrg, beta)) hPionsBananaCutAllStripsOneFile->Fill(sector+1);	
										
					// RPC CALIBRATION STUFF
										
					//if (imposeBetaCut && cand->getBeta() < betaCut) continue;
					
					Float_t MASS = 0;            
		            if (cand->getCharge()==-1) MASS = 139.57;
		            else if (cand->getCharge()==1) MASS = 139.57;
					else continue;
	            
// 					Xpos calibration
			  
			  
				    if (cand->getSystemUsed()==0){ 			
						
						Int_t metaCell = cand->getMetaCell(0);
						Int_t metaModule = cand->getMetaModule(0);
			            Int_t index = 32*metaModule + metaCell;
			            
						Float_t xdiff = cand->getRkMetaDx();
						hXposDifference[sector]->Fill(index, xdiff);
					
					    Float_t MIN_MOM = 50; 
		
						Float_t Tof;
		                Float_t beta_c = (mom)/TMath::Sqrt(mom*mom+MASS*MASS);
		                Float_t tof_c;
		                Float_t time_diff;
						
						if (mom > MIN_MOM) {   
							
							Tof = cand->getTof();
							tof_c  = beta / beta_c;
							time_diff =  Tof * (1.0 - tof_c);
			            
							if (beta>=betaCut && abs(time_diff)<8.0) hTimeDifference[sector]->Fill(index, time_diff);
							hTimeDifferenceBigRange[sector]->Fill(index, time_diff);
							
							// THIS SECTION IS FOR RPC TDIFF vs. START STRIPE 
							for(Int_t is = 0; is<nEntr; is++) {
				
								Start2CalObject = (HStart2Cal*) HCategoryManager::getObject(Start2CalObject,Start2CalCategory,is);
								if(!Start2CalObject) continue;
									
								Int_t nOfStartHits = Start2CalObject->getMultiplicity();
								Int_t startIndex = Start2CalObject->getModule() * 20 + Start2CalObject->getStrip();
							
								if (cand->getCharge() < 0) { //select negative tracks
								
									hNegativeTracksOneFile[sector]->Fill(startIndex, nOfStartHits);
									hNegativeTracksOneFile[6]->Fill(startIndex, nOfStartHits);
									
									if (cand->getBeta() >= betaCut && cand->getBeta() < 1.4) {
										 hPionsBetaCutOneFile[sector]->Fill(startIndex, nOfStartHits);
										 hPionsBetaCutOneFile[6]->Fill(startIndex, nOfStartHits);
									}
									
									if (cutNegative->IsInside(cand->getMomentum()*(-1.0), cand->getBeta())) {
										hPionsBananaCutOneFile[sector]->Fill(startIndex, nOfStartHits);
										hPionsBananaCutOneFile[6]->Fill(startIndex, nOfStartHits);
									}
								
									for (Int_t i=0; i<nOfStartHits; i++) { // loop over all start hits
										
										Float_t width = Start2CalObject->getWidth(i+1);
										Float_t calTime = Start2CalObject->getTime(i+1);
										Float_t time_diff_recalc = time_diff + startTime - calTime;
										
										hRpcTimeDiffVsWidth[startIndex-1]->Fill(width, time_diff_recalc);
										hRpcTimeDifferenceVsStartStrip->Fill(startIndex, time_diff_recalc);
										hRpcTimeDifferenceVsStartStripBigRange->Fill(startIndex, time_diff_recalc);
										
									} // end of nOfStartHits loop
								} // end of select negative tracks
								
								else if (cand->getCharge() > 0) { //select positive tracks
									if (cutPositive->IsInside(cand->getMomentum(), cand->getBeta())) {
										hPionsBananaCutOneFile[sector]->Fill(startIndex, nOfStartHits);
										hPionsBananaCutOneFile[6]->Fill(startIndex, nOfStartHits);
									}
								}
								
					    
							} //end of start2cal iter
							
							//now iterate over local maxima of found clusters to compare the timing
							
							for (Int_t c=0; c<clfMod0.getNumberOfClusters(); c++) {
								if (cand->getCharge() < 0) {
									
									Float_t calTime = clfMod0.getClusterTime(c);										
									Float_t width = clfMod0.getClusterTot(c);
									Int_t startIndex = clfMod0.getClusterStrip(c);
									Float_t time_diff_recalc = time_diff + startTime - calTime;
									
									hRpcTimeDiffVsWidthLocalMaximaOnly[startIndex-1]->Fill(width, time_diff_recalc);
									hRpcTimeDifferenceVsStartStripLocalMaximaOnly->Fill(startIndex, time_diff_recalc);
								}
							}
							for (Int_t c=0; c<clfMod1.getNumberOfClusters(); c++) {
								if (cand->getCharge() < 0) {
									Float_t calTime = clfMod1.getClusterTime(c);										
									Float_t width = clfMod1.getClusterTot(c);
									Int_t startIndex = 20 + clfMod1.getClusterStrip(c);
									Float_t time_diff_recalc = time_diff + startTime - calTime;
									
									hRpcTimeDiffVsWidthLocalMaximaOnly[startIndex-1]->Fill(width, time_diff_recalc);
									hRpcTimeDifferenceVsStartStripLocalMaximaOnly->Fill(startIndex, time_diff_recalc);
								}
							}
							
							if (cutPositive->IsInside(mom*chrg, beta) || 
							cutNegative->IsInside(mom*chrg, beta)) hPionsBananaCutAllStripsOneFile->Fill(sector+1);
							
	                    } // end of min mom
	                }
				}
			}
		} 
		
	evt++;
	
	hStartEfficiency->Fill(1);
	if (goodHitInVeto) hStartEfficiency->Fill(2);
	if (goodHitInMod0 || goodHitInMod1) hStartEfficiency->Fill(3);
	if (goodHitInMod0) hStartEfficiency->Fill(4);
	if (goodHitInMod1) hStartEfficiency->Fill(5);
	if (goodHitInMod0 && goodHitInMod1) hStartEfficiency->Fill(6);
	if (goodHitInMod0 && !goodHitInMod1) hStartEfficiency->Fill(7);
	if (goodHitInMod1 && !goodHitInMod0) hStartEfficiency->Fill(8);
	
	return 0;
		
	}

Bool_t FillRpcAndStartHistosWithTracking::finalize() {   
	
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
	
	fitMuOneFile = new TH1F ("fitMuOneFile", hldFileName, 80, -0.5, 79.5);

	cout << "Finalizing the FillRpcAndStartHistosWithTracking task ... " << endl;

	if(out) {
		
		out->cd();
		
		hVertexDist->Write();
		hBetaMomentum->Write();
		hBetaMomentumSys0->Write();
		hBetaMomentumSys1->Write();
		
		hToTVsStartStrip->Write();
		hAbsTimeVsStartStrip->Write();
		
		hJanClusterSizeMod0->Write();
		hJanClusterSizeMod1->Write();
		hJanClusterSizeMod3->Write();
		
		hStartEfficiency->GetXaxis()->SetBinLabel(1, "All events");
		hStartEfficiency->GetXaxis()->SetBinLabel(2, "Good VETO hit");
		hStartEfficiency->GetXaxis()->SetBinLabel(3, "Good hit in Mod0 or Mod1 (0 || 1)");
		hStartEfficiency->GetXaxis()->SetBinLabel(4, "Good hit in Mod0 (0)");
		hStartEfficiency->GetXaxis()->SetBinLabel(5, "Good hit in Mod1 (1)");
		hStartEfficiency->GetXaxis()->SetBinLabel(6, "Good hit in both modules (0 && 1)");
		hStartEfficiency->GetXaxis()->SetBinLabel(7, "Good hit in Mod0 only (0 && !1)");
		hStartEfficiency->GetXaxis()->SetBinLabel(8, "Good hit in Mod1 only (1 && !0)");
		hStartEfficiency->Scale(100.0/hStartEfficiency->GetBinContent(2));
		hStartEfficiency->Write();
		
		hStartVetoCorrelation->Write();
		
		hRpcTimeDifferenceVsStartStrip->Write();
		hRpcTimeDifferenceVsStartStripLocalMaximaOnly->Write();
		hRpcTimeDifferenceVsStartStripBigRange->Write();
		
		//maxWidthPosOneFile->Write();
		//maxAbsTimePosOneFile->Write();
		hNegativeTracksOneFile[6]->Write();
		hPionsBetaCutOneFile[6]->Write();
		hPionsBananaCutOneFile[6]->Write();
		hPionsBananaCutAllStripsOneFile->Write();
		fitMuOneFile->Write();
		evtCounter->Write();
		minimumTimeOneFile->Write();
		
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
			hTimeDiffVsChannelLocalMaximaOnly[i]->Write();
			hTimeDiffVsChannelLocalMaximaOnlyTimeCut[i]->Write();
			
		}
		
		out->mkdir("2D hists for time walk");
		out->mkdir("1D hists for time walk fitting");
			
		for (Int_t i = 0; i<nChannels; i++) { 
			
			if (skipThisChannel(i+1)) continue;
			
			out->cd("2D hists for time walk");
			hRpcTimeDiffVsWidth[i]->Write();
			hRpcTimeDiffVsWidthLocalMaximaOnly[i]->Write();
			hTimeDiffVsWidth[i]->Write();
			hTimeDiffVsWidthBigTimeRange[i]->Write();
			
			for (Int_t j = 0; j<nPointsForTimeWalk; j++) {
				
				out->cd("1D hists for time walk fitting");
				//out->cd();
				
				//uncomment these lines if you plan on correcting the time-walk effect
				
				//hTimeDiffForTimeWalk[i][j]->Write();
				//hWidthForTimeWalk[i][j]->Write();
			}
		}
		
		out->cd();
		
		for (Int_t i=0; i<6; i++) {

			hPionsBetaCutOneFile[i]->Write();
			hPionsBananaCutOneFile[i]->Write();
			hLeftChargeVsCellNumber[i]->Write();
			hRightChargeVsCellNumber[i]->Write();
			hXposDifference[i]->Write();
			hTimeDifference[i]->Write();
			hTimeDifferenceBigRange[i]->Write();
			//hXposOffsets[i]->Write();
			//hTimeOffsets[i]->Write();
				
		}
		
	   //out->Save();
      // out->Close();
       
	}

	return kTRUE;
	
}
