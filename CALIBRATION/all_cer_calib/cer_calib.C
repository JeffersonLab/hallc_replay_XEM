/*
Cameron Cotton - 10/28/2021
Used to get the calibration constant for the HMS/SHMS Cerenkov PMTs.
*/

void shms_m1(TString file_name) {
	TFile *file0=new TFile(file_name);
	TTree *tr1=(TTree*)file0->Get("T");
	
	Double_t adcPulseInt[4], adcMult[4];
	tr1->SetBranchAddress("P.hgcer.goodAdcPulseInt",&adcPulseInt);
	tr1->SetBranchAddress("P.hgcer.goodAdcMult",&adcMult);
	
	TCanvas* c1 = new TCanvas("c1", "Cerenkov Calibration Method 1", 1200, 1200);
	c1->Divide(2,2);
	TH1F *h1 = new TH1F("h1", "PMT 1 goodAdcPulseInt Distribution", 100, 0, 50);
	TH1F *h2 = new TH1F("h2", "PMT 2 goodAdcPulseInt Distribution", 100, 0, 50);
	TH1F *h3 = new TH1F("h3", "PMT 3 goodAdcPulseInt Distribution", 100, 0, 50);
	TH1F *h4 = new TH1F("h4", "PMT 4 goodAdcPulseInt Distribution", 100, 0, 50);
	
	Int_t nentries = (Int_t)tr1->GetEntries();
	for (Int_t i = 0; i < nentries; i++){
		tr1->GetEntry(i);
		if (true){ //common cuts
			if(adcMult[0] == 1 && adcPulseInt[0] > 1){ // PMT 1 Specific Cuts
				h1->Fill(adcPulseInt[0]);
			}
			if(adcMult[1] == 1 && adcPulseInt[1] > 1){ // PMT 2 Specific Cuts
				h2->Fill(adcPulseInt[1]);
			}
			if(adcMult[2] == 1 && adcPulseInt[2] > 1){ // PMT 3 Specific Cuts
				h3->Fill(adcPulseInt[2]);
			}
			if(adcMult[3] == 1 && adcPulseInt[3] > 1){ // PMT 4 Specific Cuts
				h4->Fill(adcPulseInt[3]);
			}
		}
	}
	
	c1->cd(1);
	h1->Draw();
	TF1* f1 = new TF1("f1","gaus",4,9);
	h1->Fit(f1,"R");
	double mean1 = f1->GetParameter(1); //this is the calibration constant
	h1->SetTitle("PMT1 Cerenkov Calibration; Pulse Integral");
	
	c1->cd(2);
	h2->Draw();
	TF1* f2 = new TF1("f2","gaus",4,9);
	h2->Fit(f2,"R");
	double mean2 = f2->GetParameter(1); //this is the calibration constant
	h2->SetTitle("PMT2 Cerenkov Calibration; Pulse Integral");
	
	c1->cd(3);
	h3->Draw();
	TF1* f3 = new TF1("f3","gaus",4,9);
	h3->Fit(f3,"R");
	double mean3 = f3->GetParameter(1); //this is the calibration constant
	h3->SetTitle("PMT3 Cerenkov Calibration; Pulse Integral");
	
	c1->cd(4);
	h4->Draw();
	TF1* f4 = new TF1("f4","gaus",4,9);
	h4->Fit(f4,"R");
	double mean4 = f4->GetParameter(1); //this is the calibration constant
	h4->SetTitle("PMT4 Cerenkov Calibration; Pulse Integral");
	
	cout << "1/PMT1 Calibration Constant: " << mean1 << endl;
	cout << "1/PMT2 Calibration Constant: " << mean2 << endl;
	cout << "1/PMT3 Calibration Constant: " << mean3 << endl;
	cout << "1/PMT4 Calibration Constant: " << mean4 << endl;
	
	return;
}


void shms_m2(TString file_name) {
	TFile *file0=new TFile(file_name);
	TTree *tr1=(TTree*)file0->Get("T");
	
	Double_t dp, etottracknorm, eprtracknorm, adcPulseInt[4], adcMult[4], xAtCer, yAtCer;
	tr1->SetBranchAddress("P.gtr.dp",&dp);
	tr1->SetBranchAddress("P.cal.etottracknorm",&etottracknorm);
	tr1->SetBranchAddress("P.ngcer.goodAdcPulseInt",&adcPulseInt); //add hgcer option
	tr1->SetBranchAddress("P.ngcer.goodAdcMult",&adcMult);
	tr1->SetBranchAddress("P.ngcer.xAtCer",&xAtCer);
	tr1->SetBranchAddress("P.ngcer.yAtCer",&yAtCer);
	
	TH1F *h1 = new TH1F("h1", "PMT 1 goodAdcPulseInt Distribution", 80, 0, 640);
	TH1F *h2 = new TH1F("h2", "PMT 2 goodAdcPulseInt Distribution", 80, 0, 160);
	TH1F *h3 = new TH1F("h3", "PMT 3 goodAdcPulseInt Distribution", 80, 0, 160);
	TH1F *h4 = new TH1F("h4", "PMT 4 goodAdcPulseInt Distribution", 80, 0, 160);
	TH1F *h5 = new TH1F("h5","P.cal.etottracknorm Cuts on All Events",300,0,3);
	TH2F *h6 = new TH2F("h6","PMT1 Cerenkov Position Cuts",60,-30,30,60,-30,30);
	TH2F *h7 = new TH2F("h7","PMT2 Cerenkov Position Cuts",60,-30,30,60,-30,30);
	TH2F *h8 = new TH2F("h8","PMT3 Cerenkov Position Cuts",60,-30,30,60,-30,30);
	TH2F *h9 = new TH2F("h9","PMT4 Cerenkov Position Cuts",60,-30,30,60,-30,30);
	
	Int_t nentries = (Int_t)tr1->GetEntries();
	Int_t adcMultCut = 1; //cuts
	Double_t xmin = 2;
	Double_t ymin = 2;
	Double_t xmax = 25;
	Double_t ymax = 25;
	Double_t emin = 0.7;
	Double_t emax = 2.0;
	Double_t dpmin = -10;
	Double_t dpmax = 22;
	
	for (Int_t i = 0; i < nentries; i++){
		tr1->GetEntry(i);
		if (etottracknorm > 0.01){ // removing 0's
			h5->Fill(etottracknorm);
		}
		if (etottracknorm > emin && etottracknorm < emax && dp > dpmin && dp < dpmax){ //common cuts
			//if(adcMult[0] == adcMultCut && adcMult[1] == 0 && adcMult[2] == 0 && adcMult[3] == 0){ // PMT 1 Specific Cuts
			if (true){
				h6->Fill(xAtCer,yAtCer);
				if(xAtCer > xmin && xAtCer < xmax && yAtCer > ymin && yAtCer < ymax){
					h1->Fill(adcPulseInt[0]);
				}
			}
			if(adcMult[1] == adcMultCut && adcMult[0] == 0 && adcMult[2] == 0 && adcMult[3] == 0){ // PMT 2 Specific Cuts
				h7->Fill(xAtCer,yAtCer);
				if(xAtCer > xmin && xAtCer < xmax && yAtCer < -1*ymin && yAtCer > -1*ymax){
					h2->Fill(adcPulseInt[1]);
				}
			}
			if(adcMult[2] == adcMultCut && adcMult[1] == 0 && adcMult[0] == 0 && adcMult[3] == 0){ // PMT 3 Specific Cuts
				h8->Fill(xAtCer,yAtCer);
				if(xAtCer < -1*xmin && xAtCer > -1*xmax && yAtCer > ymin && yAtCer < ymax){
					h3->Fill(adcPulseInt[2]);
				}
			}
			if(adcMult[3] == adcMultCut && adcMult[1] == 0 && adcMult[2] == 0 && adcMult[0] == 0){ // PMT 4 Specific Cuts
				h9->Fill(xAtCer,yAtCer);
				if(xAtCer < -1*xmin && xAtCer > -1*xmax && yAtCer < -1*ymin && yAtCer > -1*ymax){
					h4->Fill(adcPulseInt[3]);
				}
			}
		}
	}
	
	TCanvas* c1 = new TCanvas("c1", "Cerenkov Calibration Method 2", 1200, 1200);
	c1->Divide(2,2);
	c1->cd(1);
	h1->Draw();
	TF1* f1 = new TF1("f1","[0]*TMath::Power(([1]/[2]),(x/[2]))*(TMath::Exp(-([1]/[2])))/TMath::Gamma((x/[2])+1)",100,300);
	f1->SetParameters(2000,50,3);
	h1->Fit(f1,"R");
	double yscale1 = f1->GetParameter(0);
	double mean1 = f1->GetParameter(1);
	double xscale1 = f1->GetParameter(2); //this is the calibration constant
	h1->SetTitle("PMT1 Cerenkov Calibration Poisson Fit; Pulse Integral");
	
	c1->cd(2);
	h2->Draw();
	TF1* f2 = new TF1("f2","[0]*TMath::Power(([1]/[2]),(x/[2]))*(TMath::Exp(-([1]/[2])))/TMath::Gamma((x/[2])+1)",25,75);
	f2->SetParameters(2000,50,3);
	h2->Fit(f2,"R");
	double yscale2 = f2->GetParameter(0);
	double mean2 = f2->GetParameter(1);
	double xscale2 = f2->GetParameter(2); //this is the calibration constant
	h2->SetTitle("PMT2 Cerenkov Calibration Poisson Fit; Pulse Integral");
	
	c1->cd(3);
	h3->Draw();
	TF1* f3 = new TF1("f3","[0]*TMath::Power(([1]/[2]),(x/[2]))*(TMath::Exp(-([1]/[2])))/TMath::Gamma((x/[2])+1)",25,75);
	f3->SetParameters(2000,50,3);
	h3->Fit(f3,"R");
	double yscale3 = f3->GetParameter(0);
	double mean3 = f3->GetParameter(1);
	double xscale3 = f3->GetParameter(2); //this is the calibration constant
	h3->SetTitle("PMT3 Cerenkov Calibration Poisson Fit; Pulse Integral");
	
	c1->cd(4);
	h4->Draw();
	TF1* f4 = new TF1("f4","[0]*TMath::Power(([1]/[2]),(x/[2]))*(TMath::Exp(-([1]/[2])))/TMath::Gamma((x/[2])+1)",25,75);
	f4->SetParameters(2000,50,3);
	h4->Fit(f4,"R");
	double yscale4 = f4->GetParameter(0);
	double mean4 = f4->GetParameter(1);
	double xscale4 = f4->GetParameter(2); //this is the calibration constant
	h4->SetTitle("PMT4 Cerenkov Calibration Poisson Fit; Pulse Integral");
	
	TCanvas* c2 = new TCanvas("c2", "Cerenkov Position Cuts", 1200, 1200);
	c2->Divide(2,2);
	c2->cd(1);
	h6->Draw("COLZ");
	h6->SetTitle("PMT1 Cerenkov Position Cuts; xAtCer; yAtCer");
	TLine *l1 = new TLine(xmin,ymin,xmax,ymin);
    l1->SetLineColor(kRed);
    l1->Draw();
    TLine *l2 = new TLine(xmin,ymax,xmax,ymax);
    l2->SetLineColor(kRed);
    l2->Draw();
    TLine *l3 = new TLine(xmin,ymin,xmin,ymax);
    l3->SetLineColor(kRed);
    l3->Draw();
    TLine *l4 = new TLine(xmax,ymin,xmax,ymax);
    l4->SetLineColor(kRed);
    l4->Draw();
    
	c2->cd(2);
	h7->Draw("COLZ");
	h7->SetTitle("PMT2 Cerenkov Position Cuts; xAtCer; yAtCer");
	TLine *l5 = new TLine(xmin,-ymin,xmax,-ymin);
    l5->SetLineColor(kRed);
    l5->Draw();
    TLine *l6 = new TLine(xmin,-ymax,xmax,-ymax);
    l6->SetLineColor(kRed);
    l6->Draw();
    TLine *l7 = new TLine(xmin,-ymin,xmin,-ymax);
    l7->SetLineColor(kRed);
    l7->Draw();
    TLine *l8 = new TLine(xmax,-ymin,xmax,-ymax);
    l8->SetLineColor(kRed);
    l8->Draw();
	
	c2->cd(3);
	h8->Draw("COLZ");
	h8->SetTitle("PMT3 Cerenkov Position Cuts; xAtCer; yAtCer");
	TLine *l9 = new TLine(-xmin,ymin,-xmax,ymin);
    l9->SetLineColor(kRed);
    l9->Draw();
    TLine *l10 = new TLine(-xmin,ymax,-xmax,ymax);
    l10->SetLineColor(kRed);
    l10->Draw();
    TLine *l11 = new TLine(-xmin,ymin,-xmin,ymax);
    l11->SetLineColor(kRed);
    l11->Draw();
    TLine *l12 = new TLine(-xmax,ymin,-xmax,ymax);
    l12->SetLineColor(kRed);
    l12->Draw();
	
	c2->cd(4);
	h9->Draw("COLZ");
	h9->SetTitle("PMT4 Cerenkov Position Cuts; xAtCer; yAtCer");
	TLine *l13 = new TLine(-xmin,-ymin,-xmax,-ymin);
    l13->SetLineColor(kRed);
    l13->Draw();
    TLine *l14 = new TLine(-xmin,-ymax,-xmax,-ymax);
    l14->SetLineColor(kRed);
    l14->Draw();
    TLine *l15 = new TLine(-xmin,-ymin,-xmin,-ymax);
    l15->SetLineColor(kRed);
    l15->Draw();
    TLine *l16 = new TLine(-xmax,-ymin,-xmax,-ymax);
    l16->SetLineColor(kRed);
    l16->Draw();
	
	TCanvas* c3 = new TCanvas("c3", "P.cal.etottracknorm Cuts", 1200, 1200);
	h5->Draw();
	c3->Update();
	TLine *l17 = new TLine(emin,0,emin,c3->GetUymax());
    l17->SetLineColor(kRed);
    l17->Draw();
    TLine *l18 = new TLine(emax,0,emax,c3->GetUymax());
    l18->SetLineColor(kRed);
    l18->Draw();
	
	cout << "1/PMT1 Calibration Constant: " << xscale1 << endl;
	cout << "1/PMT2 Calibration Constant: " << xscale2 << endl;
	cout << "1/PMT3 Calibration Constant: " << xscale3 << endl;
	cout << "1/PMT4 Calibration Constant: " << xscale4 << endl;
	
	return;
}


void hms_m1(TString file_name) {
	TFile *file0=new TFile(file_name);
	TTree *tr1=(TTree*)file0->Get("T");
	
	Double_t adcPulseInt[4], adcMult[4];
	tr1->SetBranchAddress("H.cer.goodAdcPulseInt",&adcPulseInt);
	tr1->SetBranchAddress("H.cer.goodAdcMult",&adcMult);
	
	TCanvas* c1 = new TCanvas("c1", "Cerenkov Calibration Method 1", 1200, 1200);
	c1->Divide(2,1);
	TH1F *h1 = new TH1F("h1", "PMT 1 goodAdcPulseInt Distribution", 50, 0, 25);
	TH1F *h2 = new TH1F("h2", "PMT 2 goodAdcPulseInt Distribution", 50, 0, 25);
	
	Int_t nentries = (Int_t)tr1->GetEntries();
	for (Int_t i = 0; i < nentries; i++){
		tr1->GetEntry(i);
		if (true){ //common cuts
			if(adcMult[0] == 1 && adcPulseInt[0] > 1){ // PMT 1 Specific Cuts
				h1->Fill(adcPulseInt[0]);
			}
			if(adcMult[1] == 1 && adcPulseInt[1] > 1){ // PMT 2 Specific Cuts
				h2->Fill(adcPulseInt[1]);
			}
		}
	}
	
	c1->cd(1);
	h1->Draw();
	TF1* f1 = new TF1("f1","gaus",2,8);
	h1->Fit(f1,"R");
	double mean1 = f1->GetParameter(1); //this is the calibration constant
	h1->SetTitle("PMT1 Cerenkov Calibration; Pulse Integral");
	
	c1->cd(2);
	h2->Draw();
	TF1* f2 = new TF1("f2","gaus",2,8);
	h2->Fit(f2,"R");
	double mean2 = f2->GetParameter(1); //this is the calibration constant
	h2->SetTitle("PMT2 Cerenkov Calibration; Pulse Integral");
	
	cout << "1/PMT1 Calibration Constant: " << mean1 << endl;
	cout << "1/PMT2 Calibration Constant: " << mean2 << endl;
	
	return;
}


int run_shms_cer_calib(Int_t method = 2, TString file_name = "/u/group/c-xem2/ccotton/practice/hallc_replay_XEM/ROOTfiles/shms_replay_production_all_2788_1000000.root"){
	switch (method) {
		case 1:
			cout << "Running SHMS HGCER Calibration Using Method 1" << endl; //for the HGCER
			shms_m1(file_name);
			break;
		case 2:
			cout << "Running SHMS NGCER Calibration Using Method 2" << endl; //for the NGCER, may need to play with fit ranges depending on kinematic setting (mean npe)
			shms_m2(file_name);
			break;
		default:
			cout << "Please Enter A Valid Method Number: 1 for HGCER Calibration or 2 for NGCER Calibration" << endl;
			return -1;
	}
	return 0;
}


int run_hms_cer_calib(TString file_name = "/u/group/c-xem2/ccotton/practice/hallc_replay_XEM/ROOTfiles/hms_replay_production_all_1885_-1.root"){
	cout << "Running HMS Cerenkov Calibration Using Method 1" << endl;
	hms_m1(file_name);
	return 0;
}
