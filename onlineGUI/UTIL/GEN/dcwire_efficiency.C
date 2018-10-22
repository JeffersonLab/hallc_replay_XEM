void dcwire_efficiency(TString hist1name, TString hist2name){
  TH1F* H1;
  TH1F* H2;

  H1 = (TH1F*) gDirectory->Get(hist1name);
  H2 = (TH1F*) gDirectory->Get(hist2name);
  if (H1 && H2) {
    H1->Sumw2();
    H2->Sumw2();
    TH1F* Hratio=(TH1F*)H1->Clone();
    Hratio->Divide(H1,H2,1,1,"B");
    Hratio->SetStats(0);
    Hratio->SetMinimum(0.5);
    Hratio->SetMaximum(1.05);
    Hratio->SetMarkerStyle(21);
    Hratio->SetMarkerSize(0.75);
    Hratio->DrawClone("EP1");
    Hratio->GetXaxis()->SetTitleOffset(.6);
    Hratio->GetXaxis()->SetTitleSize(0.15);
    Hratio->GetYaxis()->SetTitle("Efficiency (%)");
    Hratio->GetYaxis()->SetTitleOffset(.6);
    Hratio->GetYaxis()->SetTitleSize(0.15);
    gPad->Update();
    TLine *line = new TLine(0.0, 1.0, gPad->GetUxmax(), 1.0);
    line->SetLineColor(kBlue); line->SetLineWidth(3); line->SetLineStyle(2); line->Draw();
  } else {
    if (!H1) cout << " Histogram " << hist1name << " does not exist" << endl;
    if (!H2) cout << " Histogram " << hist2name << " does not exist" << endl;
  }
}
