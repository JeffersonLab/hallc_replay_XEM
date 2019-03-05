void positronCounter(int run) {

  // CUts Limits
  Double_t cut_ngcer = 2.;
  Double_t cut_edeltamin = -10.;
  Double_t cut_edeltamax = 22.;

  // Integral etottracknorm Limits
  Double_t etot_min = 0.85;
  Double_t etot_max = 1.2;

  // read ROOTfile and Get TTree
  TString filename = Form("ROOTfiles/shms_replay_production_%d_-1.root", run);
  TFile* data_file = new TFile(filename, "READ");
  TTree* T = (TTree*)data_file->Get("T");

  Double_t shms_ecal;
  Double_t shms_delta;
  Double_t shms_ngcer;
  Double_t pdc_xfp;
  Double_t pdc_yfp;

  T->SetBranchAddress("P.cal.etottracknorm", &shms_ecal);
  T->SetBranchAddress("P.gtr.dp", &shms_delta);
  T->SetBranchAddress("P.ngcer.npeSum", &shms_ngcer);
  T->SetBranchAddress("P.dc.x_fp", &pdc_xfp);
  T->SetBranchAddress("P.dc.y_fp", &pdc_yfp);

  TH1F* H_pecal = new TH1F("H_pecal", "SHMS E_{tot} Normalized Trk Energy", 100,
                           0.001, 1.7);
  TH1F* H_pdelta = new TH1F("H_pdelta", "SHMS Delta", 100, -15, 25);
  TH1F* H_pngcer =
      new TH1F("H_pngcer", "SHMS Noble Gas Cer NPE SUM", 100, 0.001, 40);

  TH1F* H_pecal_cut = new TH1F(
      "H_pecal_cut", "SHMS E_{tot} Normalized Trk Energy", 100, 0.001, 1.7);
  TH1F* H_pdelta_cut = new TH1F("H_pdelta_cut", "SHMS Delta", 100, -15, 25);
  TH1F* H_pngcer_cut =
      new TH1F("H_pngcer_cut", "SHMS Noble Gas Cer NPE SUM", 100, 0.001, 40);

  TH2F* H_pxfp_vs_pyfp = new TH2F("H_pxfp_vs_pyfp", "SHMS X_{fp} vs. Y_{fp}",
                                  100, -50, 50, 100, -50, 50);
  H_pecal->GetXaxis()->SetTitle("SHMS Cal. Normalized TRack Energy");
  H_pdelta->GetXaxis()->SetTitle("SHMS #delta [%]");
  H_pngcer->GetXaxis()->SetTitle("SHMS NGC #PhotoElectrons SUM");
  H_pxfp_vs_pyfp->GetXaxis()->SetTitle("SHMS Y_{fp} [cm]");
  H_pxfp_vs_pyfp->GetYaxis()->SetTitle("SHMS X_{fp} [cm]");

  H_pecal->GetXaxis()->CenterTitle();
  H_pdelta->GetXaxis()->CenterTitle();
  H_pngcer->GetXaxis()->CenterTitle();
  H_pxfp_vs_pyfp->GetYaxis()->CenterTitle();
  H_pxfp_vs_pyfp->GetXaxis()->CenterTitle();

  Bool_t ngc_cut;
  Bool_t edelta_cut;

  Long64_t nentries = T->GetEntries();

  // Loop over all entries
  for (Long64_t i = 0; i < nentries; i++) {

    T->GetEntry(i);

    edelta_cut = shms_delta > -10. && shms_delta < 22.;
    ngc_cut = shms_ngcer > 2.0;

    H_pecal->Fill(shms_ecal);
    H_pdelta->Fill(shms_delta);
    H_pngcer->Fill(shms_ngcer);

    if (edelta_cut && ngc_cut) {
      H_pecal_cut->Fill(shms_ecal);
      H_pdelta_cut->Fill(shms_delta);
      H_pngcer_cut->Fill(shms_ngcer);
      H_pxfp_vs_pyfp->Fill(pdc_yfp, pdc_xfp);

    } // end cut

  } // end loop over entries

  TLine* ngc_line = new TLine(cut_ngcer, 0, cut_ngcer, H_pngcer->GetMaximum());
  TLine* ecal_min = new TLine(etot_min, 0, etot_min, H_pecal->GetMaximum());
  TLine* ecal_max = new TLine(etot_max, 0, etot_max, H_pecal->GetMaximum());

  TLine* delta_min =
      new TLine(cut_edeltamin, 0, cut_edeltamin, H_pdelta->GetMaximum());
  TLine* delta_max =
      new TLine(cut_edeltamax, 0, cut_edeltamax, H_pdelta->GetMaximum());

  // DRaw Histograms
  TCanvas* c1 = new TCanvas("c1", "", 1500, 1500);
  c1->Divide(2, 2);

  c1->cd(1);
  H_pecal->SetLineWidth(2);
  H_pecal_cut->SetLineWidth(2);
  H_pecal_cut->SetLineColor(kRed);
  H_pecal->Draw();
  H_pecal_cut->Draw("sames");
  ecal_min->SetLineColor(kBlack);
  ecal_min->SetLineStyle(2);
  ecal_min->SetLineWidth(3);
  ecal_min->Draw();
  ecal_max->SetLineColor(kBlack);
  ecal_max->SetLineStyle(2);
  ecal_max->SetLineWidth(3);
  ecal_max->Draw();
  gPad->SetLogy();

  c1->cd(2);
  H_pdelta_cut->SetLineWidth(2);
  H_pdelta->SetLineWidth(2);
  H_pdelta_cut->SetLineColor(kRed);
  H_pdelta->Draw();
  H_pdelta_cut->Draw("sames");
  delta_min->SetLineColor(kBlack);
  delta_min->SetLineStyle(2);
  delta_min->SetLineWidth(3);
  delta_min->Draw();
  delta_max->SetLineColor(kBlack);
  delta_max->SetLineStyle(2);
  delta_max->SetLineWidth(3);
  delta_max->Draw();

  c1->cd(3);
  H_pngcer_cut->SetLineWidth(2);
  H_pngcer->SetLineWidth(2);
  H_pngcer_cut->SetLineColor(kRed);
  H_pngcer->Draw();
  H_pngcer_cut->Draw("sames");
  ngc_line->SetLineColor(kBlack);
  ngc_line->SetLineStyle(2);
  ngc_line->SetLineWidth(3);
  ngc_line->Draw();
  gPad->SetLogy();

  c1->cd(4);
  H_pxfp_vs_pyfp->Draw("colz");

  Int_t bin_min = H_pecal_cut->GetXaxis()->FindBin(etot_min);
  Int_t bin_max = H_pecal_cut->GetXaxis()->FindBin(etot_max);

  // Calculate the Integral of etottracknorm cut
  Double_t positron_Yield = H_pecal_cut->Integral(bin_min, bin_max);
  cout << "postiron Yield = " << positron_Yield << endl;

  c1->cd(1);

  TPaveText* pt = new TPaveText(0.15, 0.7, 0.5, 0.9, "NDC");
  pt->SetTextSize(0.04);
  pt->SetFillColor(0);
  pt->SetTextAlign(12);

  pt->AddText(Form("Run: %d\n", run));
  pt->AddText("\n");
  pt->AddText(Form("Positron Yield = %d\n", int(positron_Yield)));
  pt->AddText("\n");
  pt->AddText(Form("NGCER npeSum > %.2f\n", cut_ngcer));
  pt->AddText("\n");
  pt->AddText(Form("SHMS #delta:(%.2f, %.2f)\n", cut_edeltamin, cut_edeltamax));

  pt->Draw();
}
