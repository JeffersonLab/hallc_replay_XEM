void coin_monitor_graph(int runMin = -1, int runMax = -1) {
	TFile* yieldF = new TFile("SCRIPTS/COIN/MONITOR/coin_monitor.root","READ");
	TTree* TY=(TTree*)yieldF->Get("Y");

	//gStyle->SetOptTitle(kFALSE);
	gStyle->SetPalette(1);
	TVirtualPad *pad;

	TCanvas* c = new TCanvas("c", "Coincidence monitoring", 1024, 640);
	c->Divide(2,2);

	// Get min and max run number
	// If user specifies -1 for min||max, we use the min||max in coin_monitor.root
	// These are also the defaults
	Y->Draw("run>>htemp","","goff");
	if (runMin==-1) {
		runMin = htemp->GetXaxis()->GetXmin() - 5;
	}
	if (runMax==-1) {
		runMax = htemp->GetXaxis()->GetXmax() + 5;
	}

	// =============================================
	// YIELD
	pad = c->cd(1);

	TCut hCut = "tgt==1";
	Y->Draw("run:pYpc:(sqrt(pN_trig)/pQ)",hCut,"goff");
	TGraphErrors* gHydrogen = new TGraphErrors(Y->GetEntries(),Y->GetV1(),Y->GetV2(),0,Y->GetV3());

	TCut cCut = "tgt==2";
	Y->Draw("run:pYpc:(sqrt(pN_trig)/pQ)",cCut,"goff");
	TGraphErrors* gCarbon = new TGraphErrors(Y->GetEntries(),Y->GetV1(),Y->GetV2(),0,Y->GetV3());

	//TCut dCut = "tgt==3";
	//Y->Draw("run:pYpc:(sqrt(pN_trig)/pQ)",dCut,"goff");
	//TGraphErrors* gDummy = new TGraphErrors(Y->GetEntries(),Y->GetV1(),Y->GetV2(),0,Y->GetV3());

	gHydrogen->SetLineColor(46);
	gCarbon->SetLineColor(38);
	//gDummy->SetLineColor(41);


	TMultiGraph *mgYield = new TMultiGraph("mgYield", "Coincidence yield per run");

	mgYield->Add(gHydrogen);
	mgYield->Add(gCarbon);
	//mgYield->Add(gDummy);

	mgYield->Draw("AP");
	mgYield->GetXaxis()->SetLimits(runMin, runMax);
	mgYield->GetXaxis()->SetTitle("Run Number");
	mgYield->GetYaxis()->SetTitle("Yield per mC");

	TLegend *legYield = new TLegend(0.15, 0.3, 0.35, 0.2); 
	legYield->SetFillColor(0); 
	legYield->AddEntry(gHydrogen, "Hydrogen", "lp"); 
	legYield->AddEntry(gCarbon, "Carbon", "lp"); 
	//legYield->AddEntry(gDummy, "Dummy", "lp");
	legYield->Draw();

	
	// =============================================
	// LIVE TIME
	pad = c->cd(2);

	Y->Draw("run:roc2PS6liveTime","","goff");
	TGraph* gPLive = new TGraph(Y->GetEntries(), Y->GetV1(), Y->GetV2());

	gPLive->SetTitle("ROC2 PS6 Live Time");

	gPLive->SetMarkerStyle(22);
	gPLive->SetMarkerColor(30);
	gPLive->SetMinimum(48);
	gPLive->SetMaximum(102);
	gPLive->Draw("AP");

	gPLive->GetXaxis()->SetLimits(runMin, runMax);
	gPLive->GetXaxis()->SetTitle("Run Number");
	gPLive->GetYaxis()->SetTitle("Live time");

	// =============================================
	// 3 of 4 efficiency
	pad = c->cd(3);

	Y->Draw("run:p3of4Eff","","goff");
	TGraph* gP3of4 = new TGraph(Y->GetEntries(), Y->GetV1(), Y->GetV2());
	
	Y->Draw("run:h3of4Eff","","goff");
	TGraph* gH3of4 = new TGraph(Y->GetEntries(), Y->GetV1(), Y->GetV2());

	gP3of4->SetMarkerColor(46);
	gP3of4->SetMarkerStyle(26);
	gH3of4->SetMarkerColor(30);
	gH3of4->SetMarkerStyle(32);

	TMultiGraph *mg3of4 = new TMultiGraph("mg3of4", "3 of 4 Efficiency");

	mg3of4->Add(gP3of4);
	mg3of4->Add(gH3of4);

	mg3of4->Draw("AP");
	mg3of4->SetMinimum(0.48);
	mg3of4->SetMaximum(1.02);
	mg3of4->GetXaxis()->SetLimits(runMin, runMax);
	mg3of4->GetXaxis()->SetTitle("Run Number");
	mg3of4->GetYaxis()->SetTitle("3/4 Efficiency");

	TLegend *leg3of4 = new TLegend(0.15, 0.3, 0.35, 0.2);
        leg3of4->SetFillColor(0);
        leg3of4->AddEntry(gP3of4, "SHMS 3/4", "p");
        leg3of4->AddEntry(gH3of4, "HMS 3/4", "p");
        leg3of4->Draw();

	// =============================================
	// PID track efficiency
	pad = c->cd(4);

	Y->Draw("run:pHadTrackEff:pHadTrackEffErr","","goff");
	TGraphErrors* gPTrack = new TGraphErrors(Y->GetEntries(),Y->GetV1(),Y->GetV2(),0,Y->GetV3());

	Y->Draw("run:hElecTrackEff:hElecTrackEffErr","","goff");
	TGraphErrors* gHTrack = new TGraphErrors(Y->GetEntries(),Y->GetV1(),Y->GetV2(),0,Y->GetV3());

	gPTrack->SetMarkerColor(46);
	gPTrack->SetLineColor(46);
	gPTrack->SetMarkerStyle(26);
	gHTrack->SetMarkerColor(30);
	gHTrack->SetLineColor(30);
	gHTrack->SetMarkerStyle(32);

	TMultiGraph *mgTrack = new TMultiGraph("mgTrack", "PID Tracking Efficiency");
	mgTrack->Add(gPTrack);
	mgTrack->Add(gHTrack);

	mgTrack->Draw("AP");
	mgTrack->SetMinimum(0.48);
	mgTrack->SetMaximum(1.02);
	mgTrack->GetXaxis()->SetLimits(runMin, runMax);
	mgTrack->GetXaxis()->SetTitle("Run Number");
	mgTrack->GetYaxis()->SetTitle("PID Tracking Efficiency");

	TLegend *legTrack = new TLegend(0.15, 0.3, 0.35, 0.2);
        legTrack->SetFillColor(0);
        legTrack->AddEntry(gPTrack, "SHMS Hadron", "p");
        legTrack->AddEntry(gHTrack, "HMS Electron", "p");
        legTrack->Draw();

}	
