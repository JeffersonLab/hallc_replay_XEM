void hodo_efficiency(TString hist1name, TString hist2name){
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
    Hratio->SetMinimum(0.95);
    Hratio->SetMaximum(1.05);
    Hratio->Draw("EP");
    Hratio->GetXaxis()->SetTitleOffset(.6);
    Hratio->GetXaxis()->SetTitleSize(0.08);
    Hratio->GetYaxis()->SetTitleOffset(.6);
    Hratio->GetYaxis()->SetTitleSize(0.08);
  } else {
    if (!H1) cout << " Histogram " << hist1name << " does not exist" << endl;
    if (!H2) cout << " Histogram " << hist2name << " does not exist" << endl;
  }
}
