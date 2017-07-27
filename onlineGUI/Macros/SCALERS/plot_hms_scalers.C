// UserScript.C
//
// Helper macro to build additional histograms

void UserScript()
{
  //
  const UInt_t NTRIGS  = 6;
  const TString trig_names[NTRIGS]={"S1X", "S1Y", "S1XS1Y", "S2X", "S2Y", "Trig"};
  TH1F* hScalTrig[NTRIGS];
  Double_t trig_scal[NTRIGS];
  Double_t trig_ave[NTRIGS]={0,0,0,0,0,0};
  TH1F* hAveTrig;
  hAveTrig=new TH1F("AveTrig","Ave Trig Rates ",NTRIGS,0,NTRIGS);
  //
  const UInt_t NBCMS  = 3;
  const TString bcm_names[NBCMS] = {"BCM1", "BCM2","Unser"};
  TH1F* hScalBCM[NBCMS];
  TH1F* hCurBCM[NBCMS];
  TH1F* hAveCurBCM;
  hAveCurBCM=new TH1F("AveCurBCM","Ave Current ",NBCMS,0,NBCMS);
  TH1F* hAveRateBCM;
  hAveRateBCM=new TH1F("AveRateBCM","Ave BCM Rate ",NBCMS,0,NBCMS);
  Double_t bcm_scal[NBCMS];
  Double_t bcm_cur[NBCMS];
  Double_t bcm_ave_cur[NBCMS]={0.,0.,0.};
  Double_t bcm_ave_rate[NBCMS]={0.,0.,0.};
  Double_t bcm_offset[NBCMS]={250000,250000,393000};
  Double_t bcm_slope[NBCMS]={4500*1.15,4500,4000};
  //
  const UInt_t NPLANES  = 4;
  const TString plane_names[NPLANES] = {"1x", "1y", "2x", "2y"};
  const UInt_t  nbars[NPLANES] = {16, 10, 16, 10};
  const UInt_t  nbars_low[NPLANES] = {1, 1, 1, 1};
  TH1F* hScalHodNegEv[NPLANES][16];
  TH1F* hScalHodPosEv[NPLANES][16];
  TH1F* hScalHodNeg[NPLANES];
  TH1F* hScalHodPos[NPLANES];
  Double_t hod_scalneg[NPLANES][16];
  Double_t hod_scalpos[NPLANES][16];
  Double_t hod_scalneg_rate[NPLANES][16];
  Double_t hod_scalpos_rate[NPLANES][16];
  Double_t good_bcm=0;
  Double_t good_bcm_limit=265000.;
  //
  TTree *T=(TTree*)gDirectory->Get("TSH");
  Int_t totev=T->GetEntries();
  // totev=400;
  //
  TString i2dbarname;TString h2dttitle;TString h2dtname;TString list_name;
  for(UInt_t ip = 0; ip < NTRIGS; ip++) {
    h2dttitle= trig_names[ip]+"; Event Number  ; Rate ";
    h2dtname="uhScalTrig"+trig_names[ip];
    hScalTrig[ip]= new TH1F(h2dtname,h2dttitle,totev,0,totev);
    list_name ="H."+trig_names[ip]+".scalerRate";
    T->SetBranchAddress(list_name,&trig_scal[ip]);
  }
  //
  for(UInt_t ip = 0; ip < NBCMS; ip++) {
    h2dttitle= bcm_names[ip]+"; Event Number  ;  Rate ";
    h2dtname="uhScal"+bcm_names[ip];
    hScalBCM[ip]= new TH1F(h2dtname,h2dttitle,totev,0,totev);
    h2dttitle= bcm_names[ip]+"; Event Number  ; Current  ";
    h2dtname="uhCur"+bcm_names[ip];
    hCurBCM[ip]= new TH1F(h2dtname,h2dttitle,totev,0,totev);
    list_name ="H."+bcm_names[ip]+".scalerRate";
    T->SetBranchAddress(list_name,&bcm_scal[ip]);
  }
  //
  for(UInt_t ip = 0; ip < NPLANES; ip++) {
    h2dttitle= "Beam On,Neg"+plane_names[ip]+"; Scintillator Bar  ; Ave Rate ";
    h2dtname="uhScalHodNeg"+plane_names[ip];
    hScalHodNeg[ip]= new TH1F(h2dtname,h2dttitle,nbars[ip],0.5,nbars[ip]+.5);
    h2dttitle= "Beam on,Pos"+plane_names[ip]+"; Scintillator Bar  ; Ave Rate   ";
    h2dtname="uhScalHodPos"+plane_names[ip];
    hScalHodPos[ip]= new TH1F(h2dtname,h2dttitle,nbars[ip],0.5,nbars[ip]+.5);
    for(UInt_t ibar = nbars_low[ip]-1; ibar < nbars[ip]; ibar++) {
      i2dbarname = Form("%02d",ibar+1);
      h2dttitle= "Neg Hod"+plane_names[ip]+i2dbarname+"; Event Number  ; Rate ";
      h2dtname="uhScalEvHodNeg"+plane_names[ip]+"Pad"+i2dbarname;
      hScalHodNegEv[ip][ibar]= new TH1F(h2dtname,h2dttitle,totev,0,totev);
      h2dttitle= "Pos Hod"+plane_names[ip]+i2dbarname+"; Event Number  ; Rate  ";
      h2dtname="uhScalEvHodPos"+plane_names[ip]+"Pad"+i2dbarname;
      hScalHodPosEv[ip][ibar]= new TH1F(h2dtname,h2dttitle,totev,0,totev);
      hod_scalneg_rate[ip][ibar]=0;
      hod_scalpos_rate[ip][ibar]=0;
    }
  }
  //
  //
  for(UInt_t ip = 0; ip < NPLANES; ip++) {
    for(UInt_t ibar = nbars_low[ip]-1; ibar < nbars[ip]; ibar++) {
      i2dbarname = Form("%d",ibar+1);
      list_name ="H.hod."+plane_names[ip]+i2dbarname+".negScalerRate";
      T->SetBranchAddress(list_name,&hod_scalneg[ip][ibar]);
      list_name ="H.hod."+plane_names[ip]+i2dbarname+".posScalerRate";
      T->SetBranchAddress(list_name,&hod_scalpos[ip][ibar]);
    }
  }
  // Loop over the events, filling the histograms
  //  cout << " looping over data " << endl;
  for(UInt_t iev = 0; iev < totev; iev++) {
    //    cout << " iev = " << iev << endl;
    //       cout << " get entry = " << iev << endl;
    T->GetEntry(iev);
    //
    for(UInt_t ip = 0; ip < NTRIGS; ip++) {
      hScalTrig[ip]->SetBinContent(iev,trig_scal[ip]);
      if (bcm_scal[0] > good_bcm_limit) trig_ave[ip]+=trig_scal[ip];
    }
    //
    for(UInt_t ip = 0; ip < NBCMS; ip++) {
      hScalBCM[ip]->SetBinContent(iev,bcm_scal[ip]);
      bcm_cur[ip]=(bcm_scal[ip]-bcm_offset[ip])/bcm_slope[ip];
      hCurBCM[ip]->SetBinContent(iev,bcm_cur[ip]);         
      if (bcm_scal[0] > good_bcm_limit) bcm_ave_cur[ip]+=bcm_cur[ip];
      if (bcm_scal[0] > good_bcm_limit) bcm_ave_rate[ip]+=bcm_scal[ip];
    }
    if (bcm_scal[0] > good_bcm_limit) good_bcm++;
    //
    for(UInt_t ip = 0; ip < NPLANES; ip++) {
      for(UInt_t ibar = nbars_low[ip]-1; ibar < nbars[ip]; ibar++) {
	hScalHodNegEv[ip][ibar]->SetBinContent(iev,hod_scalneg[ip][ibar]);
	hScalHodPosEv[ip][ibar]->SetBinContent(iev,hod_scalpos[ip][ibar]);
	if (bcm_scal[0] > good_bcm_limit) {
	  hod_scalneg_rate[ip][ibar]+=hod_scalneg[ip][ibar];
	  hod_scalpos_rate[ip][ibar]+=hod_scalpos[ip][ibar];
	}
	// cout << ip+1 << " " << ibar+1 << " " << hod_scalneg[ip][ibar]<< " " << hod_scalpos[ip][ibar] << endl;
      }
    }    
    //     cout  << " finish event = " << iev << endl; 
  }
  cout << " Done " << endl;
  // calculate average when beam-on
  if (good_bcm >0) {
    cout << " tot ev = " << totev << " Good BCm events = " << good_bcm << endl;
    for(UInt_t ip = 0; ip < NBCMS; ip++) {
      hAveCurBCM->SetBinContent(ip,bcm_ave_cur[ip]/good_bcm);
      hAveRateBCM->SetBinContent(ip,bcm_ave_rate[ip]/good_bcm);
    }
    for(UInt_t ip = 0; ip < NTRIGS; ip++) {
      hAveTrig->SetBinContent(ip,trig_ave[ip]/good_bcm);
    }
    for(UInt_t ip = 0; ip < NPLANES; ip++) {
      for(UInt_t ibar = nbars_low[ip]-1; ibar < nbars[ip]; ibar++) {
	hScalHodNeg[ip]->SetBinContent(ibar+1,hod_scalneg_rate[ip][ibar]/good_bcm);
	hScalHodPos[ip]->SetBinContent(ibar+1,hod_scalpos_rate[ip][ibar]/good_bcm);
      }
    }
  }
  //
  return;
}



void plot_hms_scalers(TString histname,TString histname2) {
  const UInt_t NTRIGS  = 6;
  const TString trig_names[NTRIGS]={"h1X","h1Y","h1Xh1y","h2X","h2Y","hTrig"};
  const UInt_t NPLANES  = 4;
  const TString plane_names[NPLANES] = {"1x", "1y", "2x", "2y"};
  const TString bar_names[16] = {"Pad01", "Pad02", "Pad03", "Pad04","Pad05", "Pad06", "Pad07", "Pad08","Pad09","Pad10","Pad11", "Pad12", "Pad13", "Pad14","Pad15", "Pad16"};
  const UInt_t  nbars[NPLANES] = {16, 10, 16, 10};
  const UInt_t  nbars_low[NPLANES] = {1, 1, 1, 1};
  TH1F* h;
  TH1F* hh;
  TH1F* htrig;
  TH1F* hhtrig;
  TText* htext;
  TText* hhtext;
  Double_t aveh=0,avehh=0;
  h = (TH1F*) gDirectory->Get(histname);
  hh = (TH1F*) gDirectory->Get(histname2);
  if(!h) {
    UserScript();
    h = (TH1F*) gDirectory->Get(histname);
    if(!h) {
      cout << "User histogram " << histname << " not found" << endl;
      exit(1);
    }
    if (histname2!="none") {
      hh = (TH1F*) gDirectory->Get(histname2);
      if(!hh) {
	cout << "User histogram " << histname2 << " not found" << endl;
	exit(1);
      } 
    }
  }
  if (histname.Contains("Trig")) {
    htrig = (TH1F*) gDirectory->Get("AveTrig");
  }
  if (histname2.Contains("Trig")) {
    hhtrig = (TH1F*) gDirectory->Get("AveTrig");
  }
  for (Int_t ip=0;ip<NTRIGS;ip++) {
    if (histname.Contains(trig_names[ip])) aveh=htrig->GetBinContent(ip);
    if (histname2.Contains(trig_names[ip])) avehh=hhtrig->GetBinContent(ip);
  }
  // cout << histname.Contains("adcpeak") << endl;
  Double_t maxf=0,max1=0,max2=0;
  h->SetStats(0);
  h->GetXaxis()->SetTitleOffset(.6);
  h->GetXaxis()->SetTitleSize(0.08);
  h->GetYaxis()->SetTitleOffset(.6);
  h->GetYaxis()->SetTitleSize(0.08);
  h->SetMinimum(0);
  max1=h->GetBinContent(h->GetMaximumBin());
  if (histname.Contains("uhScalHod")) {
    TH1F* h1hod;
    TH1F* h2hod;
    TString h1title;
    TString h2title;
    UInt_t ipl_f1=0;
    UInt_t ipl_f2=0;
    for(UInt_t ipl = 0; ipl < NPLANES; ipl++) {
      if (histname.Contains(plane_names[ipl]))ipl_f1 = ipl;
      if (histname2.Contains(plane_names[ipl])) ipl_f2=ipl;
    }
    if (histname.Contains("Neg")) h1title="uhScalHodNeg"+plane_names[ipl_f1];
    if (histname.Contains("Pos")) h1title="uhScalHodPos"+plane_names[ipl_f1];
    if (histname2.Contains("Neg")) h2title="uhScalHodNeg"+plane_names[ipl_f2];
    if (histname2.Contains("Pos")) h2title="uhScalHodPos"+plane_names[ipl_f2];
    h1hod = (TH1F*) gDirectory->Get(h1title);
    h2hod = (TH1F*) gDirectory->Get(h2title);
    for(UInt_t ibar = nbars_low[ipl_f1]-1; ibar < nbars[ipl_f1]; ibar++) {
      if (h1hod) aveh+=h1hod->GetBinContent(ibar+1);
    }
    for(UInt_t ibar = nbars_low[ipl_f2]-1; ibar < nbars[ipl_f2]; ibar++) {
      if (h2hod) avehh+=h2hod->GetBinContent(ibar+1);
    }
  }
  if (histname.Contains("EvHod")) {
    TH1F* h1hod;
    TH1F* h2hod;
    TString h1title;
    TString h2title;
    TString i2barname;
    for(UInt_t ipl = 0; ipl < NPLANES; ipl++) {
      for(UInt_t ibar = nbars_low[ipl]-1; ibar < nbars[ipl]; ibar++) {
	if (histname.Contains(plane_names[ipl]) && histname.Contains(bar_names[ibar])) {
	  if (histname.Contains("Neg")) h1title="uhScalHodNeg"+plane_names[ipl];
	  if (histname.Contains("Pos")) h1title="uhScalHodPos"+plane_names[ipl];
	  h1hod = (TH1F*) gDirectory->Get(h1title);
	  if (h1hod) aveh=h1hod->GetBinContent(ibar+1);
	}
	if (histname2.Contains(plane_names[ipl]) && histname2.Contains(bar_names[ibar])) {
	  if (histname2.Contains("Neg")) h2title="uhScalHodNeg"+plane_names[ipl];
	  if (histname2.Contains("Pos")) h2title="uhScalHodPos"+plane_names[ipl];
	  h2hod = (TH1F*) gDirectory->Get(h2title);
	  if (h2hod) avehh=h2hod->GetBinContent(ibar+1);
	}
      }
    }
  }
  if (histname.Contains("BCM") || histname.Contains("Unser")) {
    TH1F* hbcm;
    if (histname.Contains("Cur") ) hbcm = (TH1F*) gDirectory->Get("AveCurBCM");
    if (histname.Contains("Scal") ) hbcm = (TH1F*) gDirectory->Get("AveRateBCM");
    if (histname.Contains("BCM1")) aveh=hbcm->GetBinContent(0);
    if (histname.Contains("BCM2")) aveh=hbcm->GetBinContent(1);
    if (histname.Contains("Unser")) aveh=hbcm->GetBinContent(2);
    if (histname2!="none") {
      if (histname2.Contains("BCM1")) avehh=hbcm->GetBinContent(0);
      if (histname2.Contains("BCM2")) avehh=hbcm->GetBinContent(1);
      if (histname2.Contains("Unser")) avehh=hbcm->GetBinContent(2);
    }
  }
  if (hh)  max2=hh->GetBinContent(hh->GetMaximumBin());
  maxf=max1*1.2;
  if (hh && max2>max1) maxf=max2*1.2;
  if (h && !histname.Contains("uhScalHod"))  htext = new TText(2,maxf*.95,Form("%s Ave = %7.2f",h->GetTitle(),aveh));
  if (hh && !histname.Contains("uhScalHod")) hhtext = new TText(2,maxf*.90,Form("%s Ave = %7.2f",hh->GetTitle(),avehh));
  if (h && histname.Contains("uhScalHod"))  htext = new TText(2,maxf*.95,Form("%s Integral = %7.2f",h->GetTitle(),aveh));
  if (hh && histname.Contains("uhScalHod")) hhtext = new TText(2,maxf*.90,Form("%s Integral = %7.2f",hh->GetTitle(),avehh));
  h->SetMaximum(maxf);
  h->Draw("p");
  h->SetMarkerStyle(7);
  h->SetMarkerColor(1);
  htext->Draw();
  if (hh) {
    hh->Draw("p same");
    hh->SetMarkerColor(2);
    hh->SetMarkerStyle(7);
    hhtext->Draw();
    hhtext->SetTextColor(2);
  }
}

