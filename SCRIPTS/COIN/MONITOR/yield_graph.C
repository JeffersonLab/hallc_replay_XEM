void yield_graph() {
	TFile* yieldF = new TFile("coin_yield.root","READ");
	TTree* TY=(TTree*)yieldF->Get("Y");

	gStyle->SetOptTitle(kFALSE);
	gStyle->SetPalette(1);

	TCut hCut = "tgt==1";
	Y->Draw("run:pYpc:(sqrt(pN_trig)/pQ)",hCut,"goff");
	TGraphErrors* gHydrogen = new TGraphErrors(Y->GetEntries(),Y->GetV1(),Y->GetV2(),0,Y->GetV3());

	TCut cCut = "tgt==2";
	Y->Draw("run:pYpc:(sqrt(pN_trig)/pQ)",cCut,"goff");
	TGraphErrors* gCarbon = new TGraphErrors(Y->GetEntries(),Y->GetV1(),Y->GetV2(),0,Y->GetV3());

	TCut dCut = "tgt==3";
	Y->Draw("run:pYpc:(sqrt(pN_trig)/pQ)",dCut,"goff");
	TGraphErrors* gDummy = new TGraphErrors(Y->GetEntries(),Y->GetV1(),Y->GetV2(),0,Y->GetV3());

	gHydrogen->SetLineColor(kRed);
	gCarbon->SetLineColor(kGreen);
	gDummy->SetLineColor(kBlack);

	// Get min and max run number
	Y->Draw("run>>htemp","","goff");
	int runMin = htemp->GetXaxis()->GetXmin() - 5;
	int runMax = htemp->GetXaxis()->GetXmax() + 5;

	TCanvas* c = new TCanvas("c", "Coincidence yield per run", 800, 600);
	int maxYield = 20;
	c->DrawFrame(runMin,0,runMax,maxYield);

	TMultiGraph *mg = new TMultiGraph("mg", "Coincidence yield per run");
	mg->Add(gHydrogen);
	mg->Add(gCarbon);
	//mg->Add(gDummy);
	mg->SetTitle("Coincidence yield per run"); //doesn't work??
	mg->GetXaxis()->SetTitle("Run Number");
	mg->GetYaxis()->SetTitle("Yield per mC");

	mg->Draw("P");
	
	TLegend *leg = new TLegend(0.15, 0.7, 0.25, 0.8); 
	leg->SetFillColor(0); 
	leg->AddEntry(gHydrogen, "Hydrogen", "lp"); 
	leg->AddEntry(gCarbon, "Carbon", "lp"); 
	//leg->AddEntry(gDummy, "Dummy", "lp");
	leg->Draw();

}
