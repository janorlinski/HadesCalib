
// base
#include "hades.h"
#include "hruntimedb.h"
#include "htask.h"
#include "hevent.h"
#include "hcategory.h"
#include "hdatasource.h"
#include "hdst.h"
#include "htime.h"
#include "hsrckeeper.h"

// tasksets
#include "hstarttaskset.h"
#include "hrich700taskset.h"
#include "hmdctaskset.h"
#include "htoftaskset.h"
#include "hrpctaskset.h"
#include "hemctaskset.h"
#include "hwalltaskset.h"
#include "hsplinetaskset.h"


//tasks
#include "hmdcdedx2maker.h"
#include "hmdctrackdset.h"
#include "hmdc12fit.h"
#include "hmdccalibrater1.h"
#include "hmetamatchF2.h"
#include "hparticlevertexfind.h"
#include "hparticlecandfiller.h"
#include "hparticletrackcleaner.h"
#include "hparticleevtinfofiller.h"
#include "hparticlestart2hitf.h"
#include "hparticlebt.h"
#include "hparticlet0reco.h"
#include "hstart2calibrater.h"
#include "hqamaker.h"
#include "hemcclusterf.h"

// defs
#include "haddef.h"
#include "richdef.h"
#include "hmdcdef.h"
#include "hmdctrackddef.h"
#include "hmdctrackgdef.h"
#include "rpcdef.h"
#include "tofdef.h"
#include "walldef.h"


// containers
#include "hmdcsetup.h"
#include "hmdclayercorrpar.h"
#include "htrbnetaddressmapping.h"
#include "hmdcdedx2scale.h"
#include "hmagnetpar.h"
#include "hemcgeompar.h"



// ROOT
#include "TSystem.h"
#include "TROOT.h"
#include "TString.h"
#include "TStopwatch.h"
#include "TDatime.h"

// standard
#include <iostream>
#include <cstdio>

using namespace std;


// macros
#include "treeFilter.h"
#include "treeFilter_rejectedEvents.h"
#include "treeFilter_acceptedEvents.h"
#include "treeFilter_noM20.h"
#include "HMoveRichSector.h"
#include "HFilterEvents.h"

// macros from JO
#include "FillRpcAndStartHistosWithTracking.cc"

Int_t analysisDST(TString inFile, TString outFile,Int_t nEvents=10000, Int_t startEvt=0)
{
	//10k event jobs start finishing around 21 minutes, at 15 filesPerJob the total completion time was c.a. 28 minutes
	
	TString outdir = "/lustre/hades/user/jorlinsk/apr25/dst/";
	
    new Hades;
    gHades->setOutputLimit(20000000000); // 20GB
    gHades->setEmbeddingMode(0);
    TStopwatch timer;
    gHades->setTreeBufferSize(8000);
    gHades->makeCounter(100);
    HRuntimeDb *rtdb = gHades -> getRuntimeDb();
    gHades->setBeamTimeID(HADES::kApr25);
    gHades->getSrcKeeper()->addSourceFile("analysisDST.cc");
    gHades->getSrcKeeper()->addSourceFile("treeFilter.h");
    gHades->getSrcKeeper()->addSourceFile("treeFilter_acceptedEvents.h");
    gHades->getSrcKeeper()->addSourceFile("treeFilter_rejectedEvents.h");
    gHades->getSrcKeeper()->addSourceFile("HMoveRichSector.h");
    gHades->getSrcKeeper()->addSourceFile("sendScript_SL.sh");
    gHades->getSrcKeeper()->addSourceFile("HFilterEvents.h");
    gHades->getSrcKeeper()->addSourceFile("FillRpcAndStartHistosWithTracking.h");
    gHades->getSrcKeeper()->addSourceFile("FillRpcAndStartHistosWithTracking.cc");



    //-------------- Default Settings for the File names -----------------
    TString baseDir = outdir;
    if(!baseDir.EndsWith("/")) baseDir+="/";

    TString fileWoPath = gSystem->BaseName(inFile.Data());
    fileWoPath.ReplaceAll(".hld","");
    Int_t day      = HTime::getDayFileName(fileWoPath,kFALSE);
    Int_t evtBuild = HTime::getEvtBuilderFileName(fileWoPath,kFALSE);
    //--------------------------------------------------------------------




    //####################################################################
    //######################## CONFIGURATION #############################
    printf("Setting configuration...+++\n");

    TString asciiParFile     = "./params/apr25_dst_params.txt";  // HMdcDeDx2 + HMdcDeDx2Scale

    //TString rootParFile      = "./allParam_feb24_gen0_11042024.root";  // oracle no HMdcDeDx2Scale
    //TString rootParFile      = "./allParam_feb24_gen0_14042024.root";  // oracle no HMdcDeDx2Scale
    //TString rootParFile      = "./allParam_feb24_gen0_16042024.root";  // oracle no HMdcDeDx2Scale
    TString rootParFile      = "./params/allParam_feb24_gen1_22042025.root";  // oracle no HMdcDeDx2Scale
    //TString paramSource      = "oracle,ascii"; // root, ascii, oracle
    TString paramSource      = "ascii,root"; // root, ascii, oracle

    TString outFileSuffix    = "_dst_apr25.root";

    TString beamtime         = "apr25";
    TString paramrelease     = "FEB24_v0g";  // now,
    //TString paramrelease     = "now";  // now,

    //Int_t  refId             = 44457107fff9; //first in root par
    //Int_t  refId             = -1; //first in root par
    Int_t  refId             = 509170365;//444571079;
    Bool_t kParamFile        = kFALSE;
    Bool_t doExtendedFit     = kFALSE; // switch on/off fit for initial params of segment fitter (10 x slower!)
    Bool_t doStartCorrection = kTRUE;  // kTRUE (default)=  use run by run start corrections
    Bool_t doRotateRich      = kFALSE;
    Bool_t doMetaMatch       = kFALSE;  // default : kTRUE, kFALSE switch off metamatch in clusterfinder
    Bool_t useOffVertex      = kTRUE;  // default : kTRUE,  kTRUE=use off vertex procedure  (apr12 gen8:kFALSE, gen9:kTRUE)
    Bool_t doMetaMatchScale  = kTRUE;
    Bool_t useWireStat       = kFALSE; // online!
    Float_t metaScale        = 2; // apr12: 1.5
    Bool_t doTree            = kTRUE;
    //####################################################################
    //####################################################################


    //-------------- Default Settings for the File names -----------------
    TString outDir   = Form("%s",baseDir.Data());
    TString outDirQA = outDir+"qa/";

    if(gSystem->AccessPathName(outDir.Data()) != 0){
	cout<<"Creating output dir :"<<outDir.Data()<<endl;
	gSystem->Exec(Form("mkdir -p %s",outDir.Data()));
    }
    if(gSystem->AccessPathName(outDirQA.Data()) != 0){
	cout<<"Creating QA output dir :"<<outDirQA.Data()<<endl;
	gSystem->Exec(Form("mkdir -p %s",outDirQA.Data()));
    }

    TString hldSuffix =".hld";
    TString hldFile   = gSystem->BaseName(inFile.Data());
    if (hldFile.EndsWith(hldSuffix)) hldFile.ReplaceAll(hldSuffix,"");

    TString hld      = hldFile+hldSuffix;
    TString outFileDst  = outDir+hld+outFileSuffix;
    outFileDst.ReplaceAll("//", "/");
    //--------------------------------------------------------------------


    //------ extended output for eventbuilder 1------------------------------------//
    //----------------------------------------------------------------------------//
    // cosmic run check (Pavel) : WallRaw, EmcCal, TofRaw,TofHit,RpcHit,RpcCluster

    Cat_t notPersistentCatAll[] =
    {
	//catRich, catMdc, catShower, catTof, catTracks,
	catRich700Raw,
        //catRichDirClus, catRichHitHdr, 
        //catRichHit, catRichCal,
	//catMdcRaw,
	//catMdcCal1, //changed
	catMdcCal2,catMdcClusInf,catMdcHit,
	catMdcSeg,  //changed
	catMdcTrkCand,catMdcRawEventHeader,
	catEmcRaw,catEmcCalQA,
	//catEmcCal,catEmcCluster,
	//catTofRaw,
	//catTofHit, catTofCluster,  // changed
	//catRpcRaw,
	//catRpcCal,                // changed
	//catRpcHit, catRpcCluster, // changed
	catRKTrackB, catSplineTrack,
	catMetaMatch,             // changed
	//catParticleCandidate, catParticleEvtInfo, catParticleMdc,
	//catWallRaw, Pavel
	catWallOneHit,
	catWallCal,               // changed
	catStart2Raw //catStart2Cal, catStart2Hit,
	// catTBoxChan
    };
    //--------------------------------------------------------------------

    //------standard output for dst production------------------------------------//
    //----------------------------------------------------------------------------//
    Cat_t notPersistentCat[] =
    {
	//catRich, catMdc, catShower, catTof, catTracks,
	catRich700Raw, //catRichDirClus,
	catRichHitHdr,
	//catRichHit, catRichCal,
	//catMdcRaw, catMdcCal1,
	catMdcCal2,catMdcClusInf, catMdcHit,
	catMdcSeg, catMdcTrkCand, catMdcRawEventHeader,
	catEmcRaw,  catEmcCalQA,
        //catEmcCal, catEmcCluster,
	//catTofRaw,
	//catTofHit, catTofCluster,
	//catRpcRaw, catRpcCal, catRpcHit, catRpcCluster,
	catRKTrackB, catSplineTrack,
	catMetaMatch,
	//catParticleCandidate, catParticleEvtInfo, catParticleMdc,
	catWallRaw,
	catWallOneHit, catWallCal,
	catStart2Raw //catStart2Cal, catStart2Hit,
	// catTBoxChan
    };
    //--------------------------------------------------------------------

    //####################################################################
    //####################################################################



    Int_t mdcMods[6][4]=
    { {1,1,1,1},
    {1,1,1,1},
    {1,1,1,1},
    {1,1,1,1},
    {1,1,1,1},
    {1,1,1,1} };

    // recommendations from Vladimir+Olga
    // according to params from 28.04.2011
    Int_t nLayers[6][4] = {
	{6,6,5,6},
	{6,6,5,6},
	{6,6,5,6},
	{6,6,5,6},
	{6,6,5,6},
	{6,6,5,6} };
    Int_t nLevel[4] = {10,50000,10,5000};

    HDst::setupSpectrometer(beamtime,mdcMods,"rich,mdc,tof,rpc,emc,wall,start,tbox");
    // beamtime mdcMods_apr12, mdcMods_full
    // Int_t mdcset[6][4] setup mdc. If not used put NULL (default).
    // if not NULL it will overwrite settings given by beamtime
    // detectors (default)= rich,mdc,tof,rpc,shower,wall,tbox,start

    HDst::setupParameterSources(paramSource,asciiParFile,rootParFile,paramrelease);  // now, APR12_gen2_dst
    //HDst::setupParameterSources("oracle",asciiParFile,rootParFile,"now"); // use to create param file
    // parsource = oracle,ascii,root (order matters)
    // if source is "ascii" a ascii param file has to provided
    // if source is "root" a root param file has to provided
    // The histDate paramter (default "now") is used wit the oracle source

    HDst::setDataSource(0,"",inFile,refId); // Int_t sourceType,TString inDir,TString inFile,Int_t refId, TString eventbuilder"
    // datasource 0 = hld, 1 = hldgrep 2 = hldremote, 3 root
    // like "lxhadeb02.gsi.de"  needed by dataosoure = 2
    // inputDir needed by dataosoure = 1,2
    // inputFile needed by dataosoure = 1,3


    HDst::setupUnpackers(beamtime,"rich,mdc,tof,rpc,emc,tbox,wall,latch,start");
    // beamtime mar19
    // detectors (default)= rich,mdc,tof,rpc,emc,wall,tbox,latch,start

    if(kParamFile) {
        TDatime time;
        TString paramfilename= Form("allParam_feb24_gen0_%02i%02i%i",time.GetDay(),time.GetMonth(),time.GetYear());  // without .root

	if(gSystem->AccessPathName(Form("%s.root",paramfilename.Data())) == 0){
	    gSystem->Exec(Form("rm -f %s.root",paramfilename.Data()));
	}
	if(gSystem->AccessPathName(Form("%s.log",paramfilename.Data())) == 0){
	    gSystem->Exec(Form("rm -f %s.log" ,paramfilename.Data()));
	}

	if (!rtdb->makeParamFile(Form("%s.root",paramfilename.Data()),beamtime.Data(),"24-JAN-2024","20-MAR-2024")) {
	    delete gHades;
	    exit(1);
	}
    }
    if(refId<0){
	refId = gHades->getDataSource()->getCurrentRunId();
    }
    //--------------------------------------------------------------------
    // ----------- Build TASK SETS (using H***TaskSet::make) -------------
    HStartTaskSet        *startTaskSet        = new HStartTaskSet();
    HRich700TaskSet      *richTaskSet         = new HRich700TaskSet();
    HRpcTaskSet          *rpcTaskSet          = new HRpcTaskSet();
    HEmcTaskSet          *emcTaskSet          = new HEmcTaskSet();

    HTofTaskSet          *tofTaskSet          = new HTofTaskSet();
    HWallTaskSet         *wallTaskSet         = new HWallTaskSet();
    HMdcTaskSet          *mdcTaskSet          = new HMdcTaskSet();
    //    mdcTaskSet->setVersionDeDx(1); // 0 = no dEdx, 1 = HMdcDeDx2


    HMdcSetup*   mysetup    = (HMdcSetup*)  rtdb->getContainer("MdcSetup");
    //HEmcGeomPar* emcgeompar = (HEmcGeomPar*)rtdb->getContainer("EmcGeomPar");
    //HMagnetPar*  magnetpar  = (HMagnetPar*) rtdb->getContainer("MagnetPar");


    HMdcDeDx2Scale* mdcdedxscale = (HMdcDeDx2Scale*)rtdb->getContainer("MdcDeDx2Scale");
    rtdb->initContainers(refId);

    mysetup     ->setStatic();
    //emcgeompar  ->setStatic();
    mdcdedxscale->setStatic();
    //magnetpar   ->setStatic();


    //magnetpar->setCurrent(0);  // FIELD OFF
    //magnetpar->setCurrent(1850);  // C+C
    //magnetpar->printParams();

    mysetup->getMdcCommonSet()->setIsSimulation(0);                 // sim=1, real =0
    mysetup->getMdcCommonSet()->setAnalysisLevel(4);                // fit=4
    mysetup->getMdcCalibrater1Set()->setMdcCalibrater1Set(2, 1);    // 1 = NoStartandCal, 2 = StartandCal, 3 = NoStartandNoCal ::  0 = noTimeCut, 1 = TimeCut
    mysetup->getMdcTrackFinderSet()->setIsCoilOff(kFALSE);          // field is on
    mysetup->getMdcTrackFinderSet()->setNLayers(nLayers[0]);
    mysetup->getMdcTrackFinderSet()->setNLevel(nLevel);
    mysetup->getMdc12FitSet()->setMdc12FitSet(2,1,0,kFALSE,kFALSE); // tuned fitter, seg


    //HTask *startTasks         = startTaskSet       ->make("real","nostarthit"); // changed
    HTask *startTasks         = startTaskSet       ->make("real","");
    HTask *richTasks          = richTaskSet        ->make("real","");
    HTask *tofTasks           = tofTaskSet         ->make("real","");
    HTask *wallTasks          = wallTaskSet        ->make("real");
    HTask *rpcTasks           = rpcTaskSet         ->make("real");
    HTask *emcTasks           = emcTaskSet         ->make("real","");
    /*
    //----------------------------------------------------------------------
    // online dsts mar19
    // this should in future go to params container
    HEmcClusterF* clFinder = (HEmcClusterF*) emcTasks->getTask("emc.clusf");

    clFinder->setRpcMatchingParDThDPh(5);
    clFinder->setRpcMatchingParDTheta(0,5);
    clFinder->setRpcMatchingParDTime(0,10);
    clFinder->setClFnParamDTimeWindow(-5,5);
    //----------------------------------------------------------------------
    */

    //----------------------------------------------------------------------
    // GEN3
    // this should in future go to params container
    HEmcClusterF* clFinder = (HEmcClusterF*) emcTasks->getTask("emc.clusf");
    clFinder->setRpcMatchingParDThDPh(3.6);      // it is default value
    clFinder->setRpcMatchingParDTheta(0.2,1.1);
    clFinder->setRpcMatchParDTimeInNs(0.,2.);     // 2ns
    clFinder->setClFnParamDTimeWindow(-2.,2.);   // 2 ns
    clFinder->setCellEnergyCut(10.);
    //----------------------------------------------------------------------


    HTask *mdcTasks           = mdcTaskSet         ->make("rtdb","");
    HMdcCalibrater1* calibrater =  mdcTaskSet->getCalibrater1();

    //----------------SPLINE and RUNGE TACKING----------------------------------------
    HSplineTaskSet         *splineTaskSet       = new HSplineTaskSet("","");
    HTask *splineTasks     = splineTaskSet      ->make("","spline,runge");

    HParticleStart2HitF    *pParticleStart2HitF = new HParticleStart2HitF   ("particlehitf"      ,"particlehitf","");
    pParticleStart2HitF->setTimeCut(5,9);

    HParticleCandFiller    *pParticleCandFiller = new HParticleCandFiller   ("particlecandfiller","particlecandfiller");
									     //,"NOMOMENTUMCORR,NOPATHLENGTHCORR");
									     //,"NOMETAQANORM,NOMOMENTUMCORR,NOPATHLENGTHCORR");  // online dst
    HParticleTrackCleaner  *pParticleCleaner    = new HParticleTrackCleaner ("particlecleaner"   ,"particlecleaner");
    HParticleVertexFind    *pParticleVertexFind = new HParticleVertexFind   ("particlevertexfind","particlevertexfind","");
    HParticleEvtInfoFiller *pParticleEvtInfo    = new HParticleEvtInfoFiller("particleevtinfo"   ,"particleevtinfo",beamtime);
    //HParticleTrackCleaner  *pParticleCleanerSecond = new HParticleTrackCleaner ("particlecleanerSecond","particlecleanerSecond"); // feb22: uses iTof


    //----------------------- Quality Assessment -------------------------
    HQAMaker *qaMaker =0;
    if (!outDirQA.IsNull())
    {
	qaMaker = new HQAMaker("qamaker","qamaker");
	qaMaker->setOutputDir((Text_t *)outDirQA.Data());
	//qaMaker->setPSFileName((Text_t *)hldFile.Data());
	qaMaker->setUseSlowPar(kFALSE);
	//qaMaker->setUseSlowPar(kTRUE);
	qaMaker->setSamplingRate(1);
	qaMaker->setIntervalSize(50);
    }



    //------------------------ Master task set ---------------------------
    HTaskSet *masterTaskSet = gHades->getTaskSet("all");
    masterTaskSet->add(startTasks);

    masterTaskSet->add(tofTasks);
    masterTaskSet->add(rpcTasks);
    //masterTaskSet->add(pParticleStart2HitF); // run before wall,mdc,sline,candfiller
    masterTaskSet->add(richTasks);
    if(doRotateRich)masterTaskSet->add(new HMoveRichSector("moveRichSector","moveRichSector"));

    masterTaskSet->add(wallTasks);
    masterTaskSet->add(emcTasks);

    masterTaskSet->add(mdcTasks);

    masterTaskSet->add(splineTasks);
    masterTaskSet->add(pParticleCandFiller);
    masterTaskSet->add(pParticleCleaner);
    masterTaskSet->add(pParticleVertexFind); // run after track cleaning
    masterTaskSet->add(pParticleEvtInfo);

    TFile *out = new TFile(outFile, "RECREATE");	
    //TFile *out = new TFile("/lustre/hades/user/jorlinsk/feb24/rpcCalibRawFiles/outRpcAndStartTasks_cc_calTime0again.root", "RECREATE");	
    
    FillRpcAndStartHistosWithTracking* JanHadesCalibTask = new FillRpcAndStartHistosWithTracking("tcalibrpc", "tcalibrpc", inFile, out);
    
    masterTaskSet->add(JanHadesCalibTask);	
    //masterTaskSet->add(new HParticleT0Reco("T0","T0",beamtime));

    //masterTaskSet->add(pParticleCleanerSecond); // feb22: uses iTof


    //--------------------------------------------------------------------
    // Filtering part

    //masterTaskSet->add(new HFilterEvents);

    //addFilterAccepted(masterTaskSet,inFile,outDir) ;  // treeFilter.h
    //addFilterRejected(masterTaskSet,inFile,outDir) ;  // treeFilter.h
    ////addFilterNoM20   (masterTaskSet,inFile,outDir) ;  // treeFilter.h
    //addFilter        (masterTaskSet,inFile,outDir) ;  // treeFilter.h
    //--------------------------------------------------------------------

    if (qaMaker) masterTaskSet->add(qaMaker);

    //--------------------------------------------------------------------
    //  special settings
    HMdcTrackDSet::setTrackParam(beamtime);
    //HMdcTrackDSet::setFindOffVertTrkFlag(useOffVertex); //???


    if(!doMetaMatch)    HMdcTrackDSet::  setMetaMatchFlag(kFALSE,kFALSE);  //do not user meta match in clusterfinder
    if(doMetaMatchScale)HMetaMatchF2::   setScaleCut(metaScale,metaScale,metaScale); // (tof,rpc,shower) increase matching window, but do not change normalization of MetaQA
    if(useWireStat)     HMdcCalibrater1::setUseWireStat(kTRUE);
    
    //mdcTaskSet->getCalibrater1()->setUseMultCut(500,kTRUE,kTRUE,kTRUE); // nwires, docut, useTot, skip
    //mdcTaskSet->getCalibrater1()->setDoPrint(kTRUE);


    //--------------------------------------------------------------------
    // find best initial params for segment fit (takes long!)
    if(doExtendedFit) {
	HMdcTrackDSet::setCalcInitialValue(1);  // -  1 : for all clusters 2 : for not fitted clusters
    }
    //--------------------------------------------------------------------

    //HMdcDeDx2Maker::setFillCase(2);                      // 0 =combined, 1=combined+seg, 2=combined+seg+mod (default)
    HStart2Calibrater::setCorrection(doStartCorrection); // kTRUE (default)=  use

    //--------------------------------------------------------------------

    if (!gHades->init()){
	Error("init()","Hades did not initialize ... once again");
	exit(1);
    }

    //--------------------------------------------------------------------
    //----------------- Set not persistent categories --------------------
    HEvent *event = gHades->getCurrentEvent();

    HCategory *cat = ((HCategory *)event->getCategory(catRichCal));
    if(cat) cat->setPersistency(kTRUE);


    if(evtBuild == 1) {
	for(UInt_t i=0;i<sizeof(notPersistentCatAll)/sizeof(Cat_t);i++){
	    cat = ((HCategory *)event->getCategory(notPersistentCatAll[i]));
	    if(cat)cat->setPersistency(kFALSE);
	}
    } else {
	for(UInt_t i=0;i<sizeof(notPersistentCat)/sizeof(Cat_t);i++){
	    cat = ((HCategory *)event->getCategory(notPersistentCat[i]));
	    if(cat)cat->setPersistency(kFALSE);
	}
    }
    //--------------------------------------------------------------------
    if(doTree){
	// output file
	gHades->setOutputFile((Text_t*)outFileDst.Data(),"RECREATE","Test",2);
	gHades->makeTree();
    }
    Int_t nProcessed = gHades->eventLoop(nEvents,startEvt);
    printf("Events processed: %i\n",nProcessed);

    cout<<"--Input file      : "<<inFile  <<endl;
    cout<<"--QA directory is : "<<outDirQA<<endl;
    cout<<"--DST Output file is  : "<<outFileDst <<endl;
    cout<<"--Calib Output file is  : "<<outFile <<endl;

    printf("Real time: %f\n",timer.RealTime());
    printf("Cpu time: %f\n",timer.CpuTime());
    if (nProcessed) printf("Performance: %f s/ev\n",timer.CpuTime()/nProcessed);

    if(kParamFile) rtdb->saveOutput();

    delete gHades;
    timer.Stop();

    return 0;

}

#ifndef __CINT__
int main(int argc, char **argv)
{
    TROOT AnalysisDST("AnalysisDST","compiled analysisDST macros");


    TString nevents,startevent;
    switch (argc)
    {
    case 3:
	return analysisDST(TString(argv[1]),TString(argv[2])); // inputfile + outdir
	break;
    case 4:  // inputfile + outdir + nevents
	nevents=argv[3];

	return analysisDST(TString(argv[1]),TString(argv[2]),nevents.Atoi());
	break;
	// inputfile + nevents + startevent
    case 5: // inputfile + outdir + nevents + startevent
	nevents   =argv[3];
	startevent=argv[4];
	return analysisDST(TString(argv[1]),TString(argv[2]),nevents.Atoi(),startevent.Atoi());
	break;
    default:
	cout<<"usage : "<<argv[0]<<" inputfile outputdir [nevents] [startevent]"<<endl;
	return 0;
    }
}
#endif

