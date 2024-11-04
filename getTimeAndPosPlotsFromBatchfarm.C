void getTimeAndPosPlotsFromBatchfarm() {

	//TFile *fin = new TFile("~/lustre2/hades/user/jorlinsk/feb24/output/rpcCalibRawFiles/061/Control08/rpcCalibRawFiles_feb24_raw_061_ALLDAY_Control08.root", "read");
	TFile *fin = new TFile("~/lustre2/hades/user/jorlinsk/feb24/output/rpcCalibRawFiles/039/Control09/rpcCalibRawFiles_feb24_raw_039_ALLDAY_Control09.root", "read");
	//TFile *fin = new TFile("~/lustre2/hades/user/jorlinsk/feb24/output/rpcCalibRawFiles/061/Control08/rpcCalibRawFiles_feb24_raw_061_00h00_Control08.root", "read");
	gStyle->SetOptStat(0);
	
	
	for (Int_t i=0; i<6; i++) {
		
		TCanvas* canvL = new TCanvas(Form("pos_s%i", i+1), "", 1500, 1000);
		TCanvas* canvR = new TCanvas(Form("time_s%i", i+1), "", 1500, 1000);
		
		canvL->SetLogz(1);
		canvR->SetLogz(1);
		
		canvL->cd();
		TH2F* histL = (TH2F*) fin->Get(Form("hXposDifference_sect%i", i+1));
		histL->Draw("colz");
		canvL->Print(Form("plotDump/pos_039_%i.png", i+1));
		
		canvR->cd();
		TH2F* histR = (TH2F*) fin->Get(Form("hTimeDifference_sect%i", i+1));
		histR->Draw("colz");
		canvR->Print(Form("plotDump/time_039_%i.png", i+1));
		
	}
	
	gSystem->Sleep(2000);
	fin->Close();

}
