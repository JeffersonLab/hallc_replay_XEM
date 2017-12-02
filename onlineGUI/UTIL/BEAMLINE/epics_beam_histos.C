void UserScript()
{
  const UInt_t NBPMS  = 3;
  const TString bpm_names[NBPMS]={"IPM3H07A","IPM3H07B","IPM3H07C"};
  const UInt_t NBPMS_POS  = 2;
  const TString bpm_pos[NBPMS_POS]={"X","Y"};
  const UInt_t NBPMS_TYPES  = 3;
  const TString bpm_type[NBPMS_TYPES]={"RAW","SOF","POS"};
  TH1F* hbpm[NBPMS*NBPMS_POS*NBPMS_TYPES];
  Double_t bpm[NBPMS*NBPMS_POS*NBPMS_TYPES];
  
  //
  TTree *T=(TTree*)gDirectory->Get("E");
  Int_t totev=T->GetEntries();
  //
  TString h2dttitle;TString h2dtname;TString list_name;
  Int_t icnt=0;
  for(UInt_t ip = 0; ip < NBPMS; ip++) {
  for(UInt_t is = 0; is < NBPMS_POS; is++) {
  for(UInt_t it = 0; it < NBPMS_TYPES; it++) {
     h2dttitle= ";"+bpm_names[ip]+bpm_pos[is]+" "+bpm_type[it]+" (mm)  ;  Events ";
    h2dtname="uh"+bpm_names[ip]+bpm_pos[is]+bpm_type[it];
    hbpm[icnt]= new TH1F(h2dtname,h2dttitle,100,-10,10);
    list_name =bpm_names[ip]+"."+bpm_pos[is]+bpm_type[it];
    T->SetBranchAddress(list_name,&bpm[icnt]);
    icnt++;
  }
  }
  }
  // Loop over the events, filling the histograms
  //  cout << " looping over data " << endl;
  for(UInt_t iev = 0; iev < totev; iev++) {
    //    cout << " iev = " << iev << endl;
    //       cout << " get entry = " << iev << endl;
    T->GetEntry(iev);
     for(UInt_t ip = 0; ip < NBPMS*NBPMS_POS*NBPMS_TYPES; ip++) {
       hbpm[ip]->Fill(bpm[ip]);
     }
    //
  }
}
void epics_beam_histos(TString histname) {
  TH1F* h1d;
  TH2F* h2d;
  h1d = (TH1F*) gDirectory->Get(histname);
  h2d = (TH2F*) gDirectory->Get(histname);
 if(!h1d && !h2d) {
    UserScript();
    h1d = (TH1F*) gDirectory->Get(histname);
    h2d = (TH2F*) gDirectory->Get(histname);
    if(!h1d || !h2d) {
      cout << "User histogram " << histname << " not found" << endl;
      exit(1);
    }
  }
  if (h2d) {
  h2d->SetStats(0);
  h2d->Draw("colz");
    h2d->GetXaxis()->SetTitleOffset(.6);
    h2d->GetXaxis()->SetTitleSize(0.08);
    h2d->GetYaxis()->SetTitleOffset(.6);
    h2d->GetYaxis()->SetTitleSize(0.08);
      }
  else {
    cout << "1d  drawing   " << histname << endl;
  h1d->SetStats(0);
  h1d->Draw();
    h1d->GetXaxis()->SetTitleOffset(.6);
    h1d->GetXaxis()->SetTitleSize(0.08);
    h1d->GetYaxis()->SetTitleOffset(.6);
    h1d->GetYaxis()->SetTitleSize(0.08);
  }
}
