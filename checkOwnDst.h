#ifndef __CHECKOWNDST__
#define __CHECKOWNDST__

#include "../HadesCalibIncludes.h"
#include "hreconstructor.h"

const Float_t minVerZ = -31.0;
const Float_t maxVerZ = -27.0;
const Float_t maxVerChi2 = 20.0;

class checkOwnDst : public HReconstructor {

	protected:
	
		//evt counter
		Int_t evt;
	
	    // pointer to outputfile
	    TFile* out;
	   
		// settings
	    Bool_t imposeBetaCut = false;
	    const Double_t betaCut = 0.85;
	   
	    // histogram declarations
	    
	    // general
	    TH1F* hPartZ;
	    TH1F* hVertZ;
	    TH1F* hVertChi2;
	    TH2F* hVertZVsChi2;
	    TH1F* hPartZ_used;
	    TH1F* hVertZ_used;
	    TH1F* hVertChi2_used;
	    TH2F* hBetaMomentum;
	    TH2F* hBetaMomentumSys0;
	    TH2F* hBetaMomentumSys1;
	    
		// for rpc time/pos calibration
		TH2F* hTimeDifference[6];
		TH2F* hTimeDifferenceOneStartStrip[40][6];
		TH2F* hTimeDifferenceOneTargetSegment[6];
		TH2F* hTimeDifferenceOneStartStripOneTargetSegment[40][6];
		
		// evt counting for normalisations and trigger checks
		TH1I* evtCounter; 
		
		//TString hldFileName;
		
	    // HADES stuff declarations
	    HEventHeader* eventHeader;
	    
	    // categories
		HCategory* candCat; //
	    HCategory* Start2CalCategory;
	    HCategory* rpcCalCategory;
	    
	    // objects
		HParticleCand* cand;
	    HStart2Cal* Start2CalObject;
	    HRpcCal* rpcCalObject;
		
	public:
	
	    checkOwnDst (const Text_t*, const Text_t*, TFile*);
	    virtual ~checkOwnDst ();
	    Bool_t init();
		Bool_t reinit();
		Int_t execute();
		Bool_t finalize();

};

#endif
