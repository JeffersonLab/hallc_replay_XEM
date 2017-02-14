
void UserScript() {

  UInt_t num_p1T;

  Double_t p1T_tdc[128];

  TH1F *h_p1T_tdc;

  TTree *T = (TTree*) gDirectory->Get("T");

  T->SetBranchAddress("T.shms.p1T_tdcMultiplicity", &num_p1T);
  T->SetBranchAddress("T.shms.p1T_tdcTime", &p1T_tdc);

  h_p1T_tdc = new TH1F("h_p1T_tdc", "p1T; TDC Time (ns); Counts / 1 ns", 200, 0, 200);

  for(UInt_t iev = 0, N = T->GetEntries(); iev < N; iev++) {
    T->GetEntry(iev);

    for (Uint_t ihit = 0; i < nump1T; ihit++) {

      Double_t p1T_tdc_data = p1T_tdc[ihit++];

      h_p1T_tdc->Fill(p1T_tdc);

      }

  }  // Entries loop

}


void kpp_analysis(TString histname) {

  TH1F* h;
  h = (TH1F*) gDirectory->Get(histname);
  if(!h) {
    UserScript();
    h = (TH1F*) gDirectory->Get(histname);
    if(!h) {
      cout << "User histogram " << histname << " not found" << endl;
      exit(1);
    }
  }
  h->Draw();
}
