void getTimeAndPosPlotsFromBatchfarm() {

	//TFile *fin = new TFile("~/lustre2/hades/user/jorlinsk/feb24/output/rpcCalibRawFiles/061/Control08/rpcCalibRawFiles_feb24_raw_061_ALLDAY_Control08.root", "read");
	//TFile *fin = new TFile("~/lustre2/hades/user/jorlinsk/feb24/output/rpcCalibRawFiles/039/Control08/rpcCalibRawFiles_feb24_raw_039_ALLDAY_Control08.root", "read");
	TFile *fin = new TFile("~/lustre2/hades/user/jorlinsk/feb24/output/rpcCalibRawFiles/061/Control08/rpcCalibRawFiles_feb24_raw_061_00h00_Control08.root", "read");
	gStyle->SetOptStat(0);
	
	
	for (Int_t i=0; i<6; i++) {
		
		TCanvas* canvL = new TCanvas(Form("leftCharge_s%i", i+1), "", 1500, 1000);
		TCanvas* canvR = new TCanvas(Form("rightCharge_s%i", i+1), "", 1500, 1000);
		
		canvL->SetLogz(1);
		canvR->SetLogz(1);
		
		canvL->cd();
		TH2F* histL = (TH2F*) fin->Get(Form("hLeftChargeVsCellNumber_sect%i", i+1));
		histL->Draw("colz");
		canvL->Print(Form("plotDump/leftCharge_061midnight_%i.png", i+1));
		
		canvR->cd();
		TH2F* histR = (TH2F*) fin->Get(Form("hRightChargeVsCellNumber_sect%i", i+1));
		histR->Draw("colz");
		canvR->Print(Form("plotDump/rightCharge_061midnight_%i.png", i+1));
		
	}
	
	gSystem->Sleep(2000);
	fin->Close();

}
