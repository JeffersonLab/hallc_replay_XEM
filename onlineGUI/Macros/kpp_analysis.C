
void UserScript() {

  Double_t num_p1T;

  Double_t p1T_tdc[128];

  Long64_t nentries;

  TH1F *h_p1T_tdc;

  TTree *T = (TTree*) gDirectory->Get("T");

  nentries = T->GetEntries();

  T->SetBranchAddress("T.shms.p1T_tdcMultiplicity", &num_p1T);
  T->SetBranchAddress("T.shms.p1T_tdcTime", &p1T_tdc);

  h_p1T_tdc = new TH1F("h_p1T_tdc", "p1T; TDC Time (ns); Counts / 1 ns", 2000, 0, 2000);

  for(UInt_t ievent = 0; ievent < nentries; ievent++) {
    
    T->GetEntry(ievent);

    for (UInt_t ihit = 0; ihit < num_p1T; ihit++) {

      Double_t p1T_tdc_data = p1T_tdc[ihit];

      cout << ihit << "\t" << p1T_tdc_data << endl;

      h_p1T_tdc->Fill(p1T_tdc_data);

      }

  }  // Entries loop

}

void kpp_analysis(TString histname) {

  TH1F* h; h = (TH1F*) gDirectory->Get(histname);
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
