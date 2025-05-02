#ifndef __FILLRPCANDSTARTHISTOSWITHTRACKING__
#define __FILLRPCANDSTARTHISTOSWITHTRACKING__

#include "HadesCalibIncludes.h"
#include "hreconstructor.h"

class FillRpcAndStartHistosWithTracking : public HReconstructor {

	protected:
	
		//evt counter
		Int_t evt;
	
	    // pointer to outputfile
	    TFile* out;
	   
		// settings
	    Bool_t imposeBetaCut = false;
	    const Double_t betaCut = 0.80;
	   
	    // histogram declarations
	    
	    // general
	    TH1F* hVertexDist;
	    TH2F* hBetaMomentum;
	    TH2F* hBetaMomentumSys0;
	    TH2F* hBetaMomentumSys1;
	    
	    // start multiplicity	    
	    TH2F* hMultiplicityPerModule;
	    TH2F* hMultiplicityPerModuleNoTimeLimit;
	    
	    //cluster number per event
	    TH1F* hFelixClusterNumberMod0;
	    TH1F* hFelixClusterNumberMod1;
	    TH1F* hFelixClusterNumberMod3;
	    TH1F* hFelixClusterNumberAllMods;
	  
		//cluster size
	    TH1F* hFelixClusterSizeMod0;
	    TH1F* hFelixClusterSizeMod1;
	    TH1F* hFelixClusterSizeMod3;
	    TH1F* hFelixClusterSizeAllMods;
	    
	    //my own clusters
	    TH1F* hJanClusterSizeMod0;
	    TH1F* hJanClusterSizeMod1;
	    TH1F* hJanClusterSizeMod3;
	    TH1F* hJanClusterSizeAllMods;
	   
	    //start efficiency
	    TH1F* hStartEfficiency;
	    TH1F* hStartEfficiencyFelix;
	    TH1F* hStartEfficiencyJan;
	    
	    //all start tot/tdiff stuff
	    TH2F* hStartVetoCorrelation;
		TH2F* hToTVsStartStrip;
		TH2F* hAbsTimeVsStartStrip;
	    TH2F* hTimeDiffVsChannel[nChannels];
	    TH2F* hTimeDiffVsChannelLocalMaximaOnly[nChannels];
	    TH2F* hTimeDiffVsChannelLocalMaximaOnlyTimeCut[nChannels];
	    TH2F* hTimeDiffVsWidth[nChannels];
	    TH2F* hTimeDiffVsWidthLocalMaximaOnly[nChannels];
	    TH2F* hRpcTimeDiffVsWidth[nChannels];
	    TH2F* hRpcTimeDiffVsWidthLocalMaximaOnly[nChannels];
	    TH2F* hTimeDiffVsWidthBigTimeRange[nChannels];
	    TH1F* hTimeDiffForTimeWalk[nChannels][nPointsForTimeWalk];
	    TH1F* hWidthForTimeWalk[nChannels][nPointsForTimeWalk];
		TH2F* hRpcTimeDifferenceVsStartStrip;
		TH2F* hRpcTimeDifferenceVsStartStripLocalMaximaOnly;
		TH2F* hRpcTimeDifferenceVsStartStripBigRange;
		
		// for rpc time/pos calibration
		TH2F* hXposDifference[6];
		TH2F* hTimeDifference[6];
		TH2F* hTimeDifferenceBigRange[6];
		TH1F* hXposOffsets[6];
		TH1F* hTimeOffsets[6];
		
		//for rpc charge calib
		TH2F* hLeftChargeVsCellNumber[6];
	    TH2F* hRightChargeVsCellNumber[6];
		
		// tot/abstime positions specifically for timingMatrix analysis
		TH1F* maxWidthPosOneFile;
		TH1F* maxAbsTimePosOneFile;
		
		// beam profile/pid quality checks specifically for timingMatrix analysis
		TH1F* hNegativeTracksOneFile[7];
		TH1F* hPionsBetaCutOneFile[7];
		TH1F* hPionsBananaCutOneFile[7];
		TH1F* hPionsBananaCutAllStripsOneFile;
		
		// timing checks specifically for timingMatrix analysis
		TH1F* fitSigmaOneFile;
		TH1F* fitMuOneFile;
		TH1F* minimumTimeOneFile;
		
		// evt counting for normalisations and trigger checks
		TH1I* evtCounter; 
		
		TString hldFileName;
		
	    // HADES stuff declarations
	    HEventHeader* eventHeader;
	    
	    // categories
		HCategory* candCat; //
	    HCategory* Start2CalCategory;
	    HCategory* rpcCalCategory;
	    
	    // objects
		HParticleCand* cand;
	    HStart2Cal* Start2CalObject;
	    HStart2Cal* Start2CalObjectRef;
	    HRpcCal* rpcCalObject;
		//HRpcHit* rpchit;
		//HRpcCluster* rpcclus;
		
		// pion graphical cuts
		TCutG* cutPositive;
		TCutG* cutNegative;

	public:
	
	    FillRpcAndStartHistosWithTracking (const Text_t*, const Text_t*, TString, TFile*);
	    virtual ~FillRpcAndStartHistosWithTracking ();
	    Bool_t init();
		Bool_t reinit();
		Int_t execute();
		Bool_t finalize();

};

#endif
