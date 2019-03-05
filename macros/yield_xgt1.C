void yield_xgt1(UInt_t runNum, Int_t numEvents)
{

  // TFile *replayFile = new TFile(Form("/home/cdaq/hallc-online/hallc_replay/ROOTfiles/shms_replay_production_all_%d_%d.root", runNum, numEvents));
  TFile *replayFile = new TFile(Form("/home/cdaq/hallc-online/hallc_replay/ROOTfiles/shms_replay_production_%d_%d.root", runNum, numEvents));
  // TFile *replayFile = new TFile(Form("/net/cdaq/cdaql1data/cdaq/jpsi-007/ROOTfiles/shms_replay_production_%d_%d.root", runNum, numEvents));

  TTree *dataTree = dynamic_cast <TTree*> (replayFile->Get("T"));

  TCanvas *can = new TCanvas("can", "x > 1 Canvas", 1600, 800);
  can->Divide(2, 2);

  TCut deltaCut    = "P.gtr.dp > -15.0 && P.gtr.dp < 22.0";
  TCut cerCut      = "P.ngcer.npeSum > 2.0";
  TCut calCut      = "P.cal.etottracknorm > 0.8 && P.cal.etottracknorm < 1.3";
  TCut calZeroCut  = "P.cal.etottracknorm > 0.0";
  TCut xbjCut      = "P.kin.x_bj > 1.5 && P.kin.x_bj < 1.9";
  // TCut xbjCut      = "P.kin.x_bj > 1.517 && P.kin.x_bj < 1.917";
  TCut pidCut      = deltaCut && cerCut && calCut;
  TCut pidNoCalCut = deltaCut && cerCut && calZeroCut;
  TCut allCuts     = pidCut && xbjCut;
  
  TH1F *xbjHisto     = new TH1F("xbjHisto",     "SHMS Bjorken x; x_{Bj}; Number of Entries", 300, 0.0, 3.0);
  TH1F *q2Histo      = new TH1F("q2Histo",      "SHMS Q^{2}; Q^{2} (GeV^{2}); Number of Entries / 0.010 GeV", 1000, 0.0, 10.0);
  TH1F *w2Histo      = new TH1F("w2Histo",      "SHMS W^{2}; W^{2} (GeV^{2}); Number of Entries / 0.010 GeV", 1500, 0.0, 15.0);
  TH1F *etotHisto    = new TH1F("etotHisto",    "SHMS Normalized Track Energy; E/P; Number of Entries / 0.010 GeV", 150, 0.0, 1.5);
  TH1F *etotCutHisto = new TH1F("etotCutHisto", "SHMS Normalized Track Energy; E/P; Number of Entries / 0.010 GeV", 150, 0.0, 1.5);
  
  TH1F *numTrigsHisto = dynamic_cast <TH1F*> (replayFile->Get("ptrig_pdc_ref1"));
  Double_t numTrigs   = numTrigsHisto->Integral();
 
  dataTree->Project(xbjHisto->GetName(),     "P.kin.x_bj",          pidCut);
  dataTree->Project(q2Histo->GetName(),      "P.kin.Q2",            pidCut);
  dataTree->Project(w2Histo->GetName(),      "P.kin.W2",            pidCut);
  dataTree->Project(etotHisto->GetName(),    "P.cal.etottracknorm", pidCut);
  dataTree->Project(etotCutHisto->GetName(), "P.cal.etottracknorm", allCuts);

  Int_t    yieldRaw  = etotHisto->Integral();
  Int_t    yieldxgt1 = etotCutHisto->Integral();
  Double_t etotMax   = etotCutHisto->GetMaximum();

  gStyle->SetOptStat(0);
  etotHisto->SetLineColor(kRed);
  etotCutHisto->SetLineColor(kBlue);

  Double_t yieldPerTrig = (yieldxgt1/numTrigs)*100.;

  TLegend *leg = new TLegend(0.15, 0.6, 0.5, 0.85);
  leg->AddEntry(numTrigsHisto, Form("Number of Triggers = %.0f", numTrigs), "");
  leg->AddEntry(etotHisto, Form("e^{-} Yield = %d", yieldRaw), "");
  leg->SetTextColor(kRed);
  leg->AddEntry(etotCutHisto, Form("e^{-} Yield, 1.5 < x_{bj} < 1.9 = %d", yieldxgt1), "");
  leg->SetTextColor(kBlue);
  leg->SetLineColor(0);
  leg->AddEntry(numTrigsHisto, Form("Yield/Number of Triggers = %.4f %%", yieldPerTrig), "");

  can->cd(1);
  xbjHisto->Draw();
  can->cd(2);
  q2Histo->Draw();
  can->cd(3);
  w2Histo->Draw();
  can->cd(4);
  etotHisto->Draw();
  etotCutHisto->Draw("SAME");
  leg->Draw();
}
