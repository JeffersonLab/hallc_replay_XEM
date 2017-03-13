void shw_pshw() {
  cout << "Hello." << endl << endl;

  const Int_t nPreShowerBlocks = 14;
  const Int_t nShowerBlocks = 224;

  Double_t preShowerNeg_adc2GeV[nPreShowerBlocks] = {
    1.0/466.2, 1.0/514.7, 1.0/325.5, 1.0/247.8,
    1.0/322.0, 1.0/176.2, 1.0/161.9, 1.0/197.7,
    1.0/269.9, 1.0/241.6, 1.0/193.7, 1.0/337.5,
    1.0/237.3, 1.0/82.0
  };
  Double_t preShowerPos_adc2GeV[nPreShowerBlocks] = {
    1.0/411.2, 1.0/378.6, 1.0/357.9, 1.0/254.8,
    1.0/315.6, 1.0/331.0, 1.0/264.6, 1.0/305.4,
    1.0/149.8, 1.0/380.6, 1.0/183.1, 1.0/206.5,
    1.0/139.8, 1.0/163.1
  };
  Double_t shower_adc2GeV = 0.0005;


  TFile F("../ROOTfiles/shms_replay_488_-1.root");
  TTree* T = F.Get("T");


  Int_t nTracks;
  Double_t track_x[1000];
  Double_t track_y[1000];
  Double_t track_xp[1000];
  Double_t track_yp[1000];

  T->SetBranchAddress("Ndata.P.dc.x", &nTracks);
  T->SetBranchAddress("P.dc.x", track_x);
  T->SetBranchAddress("P.dc.y", track_y);
  T->SetBranchAddress("P.dc.xp", track_xp);
  T->SetBranchAddress("P.dc.yp", track_yp);

  Int_t nPreShower;
  Double_t preShower_posAdcC[1000];
  Double_t preShower_posAdcPI[1000];

  T->SetBranchAddress("Ndata.P.cal.pr.posAdcCounter", &nPreShower);
  T->SetBranchAddress("P.cal.pr.posAdcCounter", preShower_posAdcC);
  T->SetBranchAddress("P.cal.pr.posAdcPulseInt", preShower_posAdcPI);

  Int_t nShower;
  Double_t shower_C[1000];
  Double_t shower_PI[1000];

  T->SetBranchAddress("Ndata.P.cal.fly.adcCounter", &nShower);
  T->SetBranchAddress("P.cal.fly.adcCounter", shower_C);
  T->SetBranchAddress("P.cal.fly.adcPulseInt", shower_PI);

  Int_t nNGC;
  Double_t NGC_C[1000];
  Double_t NGC_PI[1000];

  T->SetBranchAddress("Ndata.P.ngcer.adcCounter", &nNGC);
  T->SetBranchAddress("P.ngcer.adcCounter", NGC_C);
  T->SetBranchAddress("P.ngcer.adcPulseInt", NGC_PI);


  TH2D* s_ps = new TH2D("s_ps", "s_ps", 100, -1, 30, 100, -1, 20);
  s_ps->SetTitle("");
  TH2D* s_ps_e = new TH2D("s_ps_e", "s_ps_e", 100, -1, 30, 100, -1, 20);
  TH2D* s_ps_pi = new TH2D("s_ps_pi", "s_ps_pi", 100, -1, 30, 100, -1, 20);


  Long64_t nEvents = T->GetEntries();
  //for (Long64_t iEvent; iEvent<100000; ++iEvent) {
  for (Long64_t iEvent; iEvent<nEvents; ++iEvent) {
    if ((iEvent+1)%1000 == 0) cout << "Event: " << iEvent+1 << endl;
    T->GetEntry(iEvent);

    if (nTracks!=1 || fabs(track_x[0] + track_xp[0]*330 + 9)>9 || fabs(track_y[0] + track_yp[0]*330 - 9)>9) continue;

    bool electron=false;
    for (Int_t iC=0; iC<nNGC; ++iC) {
      if (NGC_PI[iC] > 4500) {
	electron = true;
	continue;
      }
    }

    bool pion=true;
    for (Int_t iC=0; iC<nNGC; ++iC) {
      if (NGC_PI[iC] > 3500) {
	pion = false;
	continue;
      }
    }

    Double_t preShower_energy = 0.0;
    for (Int_t iPS=0; iPS<nPreShower; ++iPS) {
      Int_t psc = preShower_posAdcC[iPS];
      if (psc==6 || psc==7) {
	preShower_energy += preShower_posAdcPI[iPS] * preShowerPos_adc2GeV[psc-1];
      }
    }

    Double_t shower_energy = 0.0;
    for (Int_t iS=0; iS<nShower; ++iS) {
      Int_t sc = shower_C[iS];
      if (sc==87 || sc==88 || sc==103 || sc==104) {
	shower_energy += shower_PI[iS] * shower_adc2GeV;
      }
    }

    s_ps->Fill(preShower_energy, shower_energy);
    if (electron) s_ps_e->Fill(preShower_energy, shower_energy);
    if (pion) s_ps_pi->Fill(preShower_energy, shower_energy);

  }


  gStyle->SetOptStat(0);

  s_ps->GetXaxis()->SetTitle("pre-shower energy");
  s_ps->GetYaxis()->SetTitle("shower energy");
  s_ps->SetMarkerColor(1);

  s_ps_e->GetXaxis()->SetTitle("pre-shower energy");
  s_ps_e->GetYaxis()->SetTitle("shower energy");
  s_ps_e->SetMarkerColor(2);

  s_ps_pi->GetXaxis()->SetTitle("pre-shower energy");
  s_ps_pi->GetYaxis()->SetTitle("shower energy");
  s_ps_pi->SetMarkerColor(3);


  TCanvas* c1 = new TCanvas("c1", "c1", 800, 600);
  c1->cd();


  s_ps->Draw();
  s_ps_e->Draw("same");
  //s_ps_pi->Draw("same");
  c1->Print("shw_pshw.png");

  c1->SetLogz();

  s_ps->Draw("colz");
  c1->Print("shw_pshw_all.png");

  s_ps_e->Draw("colz");
  c1->Print("shw_pshw_e.png");

  s_ps_pi->Draw("colz");
  c1->Print("shw_pshw_pi.png");

  cout << endl << "Bye." << endl;
}
