#include "checkOwnDst.cc"

using namespace std;

Int_t loopDST_task(TString infileList="/lustre/hades/dst/feb22/gen2/042/02/root/be2204200012502.hld_dst_feb22.root",
		   TString outfile="test.root",
		   Int_t nEvents = -1
		  )
{
    //  infileList : comma seprated file list "file1.root,file2.root" or "something*.root"
    //  outfile    : optional (not used here) , used to store hists in root file
    //  nEvents    : number of events to processed. if  nEvents < entries or < 0 the chain will be processed

    //-------------------------------------------------
    // create loop obejct and hades
    HLoop loop(kTRUE);
    //-------------------------------------------------

    //-------------------------------------------------
    // setup the spectrometer for filter tasks
    // this part is needed if your task needs
    // detectors or parameter container
    if(0){
	TString beamtime="feb22";

	Int_t mdcMods[6][4]=
	{ {1,1,1,1},
	{1,1,1,1},
	{1,1,1,1},
	{1,1,1,1},
	{1,1,1,1},
	{1,1,1,1} };
	
	TString asciiParFile     = "";
    TString rootParFile      = "./allParam_Feb22_gen2_27022023.root";  // gen2 (ora+ itof calpar+trb3lookup from ascii)
    TString paramSource      = "root"; // root, ascii, oracle
	TString paramrelease     = "FEB22_v2a";
	
	HDst::setupSpectrometer(beamtime,mdcMods,"rich,mdc,tof,rpc,emc,wall,start,tbox");
	HDst::setupParameterSources(paramSource,asciiParFile,rootParFile,paramrelease);
    }
    //-------------------------------------------------


    //-------------------------------------------------
    // reading input files and decalring containers
    Bool_t ret =kFALSE;
    if(infileList.Contains(",")){
	ret = loop.addMultFiles(infileList);      // file1,file2,file3
    } else if(infileList.Contains(".txt") || infileList.Contains(".list")){
        ret = loop.addFilesList(infileList); //in case of file-list (1 file per line with full path)
    } else{
	ret = loop.addFiles(infileList); // myroot*.root
    }

    if(ret == 0) {
	cout<<"READBACK: ERROR : cannot find inputfiles : "<<infileList.Data()<<endl;
	return 1;
    }


    if(!loop.setInput("-*,+HParticleCand,+HParticleEvtInfo,+HWallHit")) { //select some cotegories
	cout<<"READBACK: ERROR : cannot read input !"<<endl;
	exit(1);
    }


    //-------------------------------------------------
    // Fix lustre many seek/read issue by reading larger
    // file junks into cache. Cache all branches and
    // switch of the learning phase
    loop.getChain()->SetCacheSize(-1); // 8Mb - 8000000
    //loop.getChain()->AddBranchToCache("-*,+HParticleCand,+HParticleEvtInfo,+HRpcCluster,+HTofCluster,+HStart2Hit,+HStart2Cal,+HWallHit,+HWallEventPlane",kTRUE);
    loop.getChain()->AddBranchToCache("*",kTRUE);
    loop.getChain()->StopCacheLearningPhase();
    //-------------------------------------------------

    loop.printCategories();
    loop.printChain();
    //-------------------------------------------------

    //-------------------------------------------------
    //parameters

    TFile *out = new TFile(outfile, "RECREATE");
//    out = TFile::Open(TFile::AsyncOpen(outfile,"UPDATE"));  //TODO: to be used if more than 1 task set is used!!!

    //-------------------------------------------------
    // added for the task
    
    HTaskSet *masterTaskSet = gHades->getTaskSet("all");
    
    checkOwnDst* checkOwnDst_task = new checkOwnDst("cod", "cod", out);
    
    masterTaskSet->add(checkOwnDst_task);	

    //-------------------------------------------------

    //-------------------------------------------------
    // event loop starts here
    Int_t entries = loop.getEntries();
    
	cout << "Start loopDST. Available entries  = "<< entries << endl;
	if(nEvents < entries && nEvents >= 0 ) entries = nEvents;	
	
	cout << "nEvents = " << nEvents <<", " << entries <<" events will be processed. \n ";

    for (Int_t i = 0; i < entries; i++) {
	Int_t nbytes =  loop.nextEvent(i);             // get next event. categories will be cleared before

	if(nbytes <= 0) { cout<<nbytes<<endl; break; } // last or broken event reached -> end

	if(i%10000 == 0) cout<<"Event "<< i << " / " << entries << Form(" (%2.0f%% done)\n", 100*float(i)/float(entries));

//	cout << "Event " << i << ":  ";

    } // end eventloop


    //-------------------------------------------------
    // added for the task
    if(gHades) gHades->finalizeTasks();
    //-------------------------------------------------

    out->Save();
    out->Close();

    delete gHades;
    return 0;
}
