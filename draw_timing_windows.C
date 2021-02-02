void makeplot(TString, TString, TString, TString, TCanvas*, TCanvas*, TCanvas*, TFile*, Double_t[], Double_t[], int, int[]);
void makeplotArr(TString, TString, TString, TString, TCanvas* [], TFile*, Double_t[], Double_t[], int, int[]);

void draw_timing_windows(int RunNumber) {
  const int size = 21;
  int peak_1x_pos[size] = {0};
  int peak_1x_neg[size] = {0};
  int peak_1y_pos[size] = {0};
  int peak_1y_neg[size] = {0};
  int peak_2x_pos[size] = {0};
  int peak_2x_neg[size] = {0};
  int peak_2y_pos[size] = {0};
  int peak_2y_neg[size] = {0};
  int peak_calo_pr_pos[14] = {0};
  int peak_calo_pr_neg[14] = {0};
  int peak_calo_fly[224] = {0};

  int peak_crap[size] = {0};

  TFile *outFile = new TFile(Form("ROOTfiles/TIMING/timing_windows_NEW_%d.root",RunNumber),"RECREATE");

  TString golden_file = Form("ROOTfiles/TIMING/shms_noTimingWindows_%d_1000000.root", RunNumber);

  //Load PRODUCTION Windows
  gHcParms->Define("gen_run_number", "Run Number", RunNumber);
  gHcParms->AddString("g_ctp_database_filename", "DBASE/SHMS/standard.database");
  gHcParms->Load(gHcParms->GetString("g_ctp_database_filename"), RunNumber);
  gHcParms->Load(gHcParms->GetString("g_ctp_parm_filename"));
  gHcParms->Load(gHcParms->GetString("g_ctp_kinematics_filename"), RunNumber);
  gHcParms->Load(gHcParms->GetString("g_ctp_calib_filename"));
  // Load parameters for SHMS trigger configuration
  gHcParms->Load("PARAM/TRIG/tshms.param");

  const char* prefix = "p";

  Int_t fHodoScin = 4 * 21; //2Y plane has 21 PMTs
  Double_t* fHodoPosAdcTimeWindowMin = new Double_t [fHodoScin];
  Double_t* fHodoPosAdcTimeWindowMax = new Double_t [fHodoScin];
  Double_t* fHodoNegAdcTimeWindowMin = new Double_t [fHodoScin];
  Double_t* fHodoNegAdcTimeWindowMax = new Double_t [fHodoScin];

  Int_t fPrShwr = 14;
  Double_t* fCaloPosAdcTimeWindowMin = new Double_t [fPrShwr];
  Double_t* fCaloPosAdcTimeWindowMax = new Double_t [fPrShwr];
  Double_t* fCaloNegAdcTimeWindowMin = new Double_t [fPrShwr];
  Double_t* fCaloNegAdcTimeWindowMax = new Double_t [fPrShwr];

  Int_t fFlyShwr = 224;
  Double_t* fCaloArrAdcTimeWindowMin = new Double_t [fFlyShwr];
  Double_t* fCaloArrAdcTimeWindowMax = new Double_t [fFlyShwr];

  Int_t fNGCer = 4;
  Double_t* fNGCerAdcTimeWindowMin = new Double_t [fNGCer];
  Double_t* fNGCerAdcTimeWindowMax = new Double_t [fNGCer];

  Int_t fHGCer = 4;
  Double_t* fHGCerAdcTimeWindowMin = new Double_t [fHGCer];
  Double_t* fHGCerAdcTimeWindowMax = new Double_t [fHGCer];

  DBRequest windowList[] = {
    {"hodo_PosAdcTimeWindowMin", fHodoPosAdcTimeWindowMin, kDouble,  (UInt_t) fHodoScin, 1},
    {"hodo_PosAdcTimeWindowMax", fHodoPosAdcTimeWindowMax, kDouble,  (UInt_t) fHodoScin, 1},
    {"hodo_NegAdcTimeWindowMin", fHodoNegAdcTimeWindowMin, kDouble,  (UInt_t) fHodoScin, 1},
    {"hodo_NegAdcTimeWindowMax", fHodoNegAdcTimeWindowMax, kDouble,  (UInt_t) fHodoScin, 1},
    {"cal_pos_AdcTimeWindowMin", fCaloPosAdcTimeWindowMin, kDouble,  (UInt_t) fPrShwr, 1},
    {"cal_pos_AdcTimeWindowMax", fCaloPosAdcTimeWindowMax, kDouble,  (UInt_t) fPrShwr, 1},
    {"cal_neg_AdcTimeWindowMin", fCaloNegAdcTimeWindowMin, kDouble,  (UInt_t) fPrShwr, 1},
    {"cal_neg_AdcTimeWindowMax", fCaloNegAdcTimeWindowMax, kDouble,  (UInt_t) fPrShwr, 1},
    {"cal_arr_AdcTimeWindowMin", fCaloArrAdcTimeWindowMin, kDouble,  (UInt_t) fFlyShwr, 1},
    {"cal_arr_AdcTimeWindowMax", fCaloArrAdcTimeWindowMax, kDouble,  (UInt_t) fFlyShwr, 1},
    {"ngcer_adcTimeWindowMin",  fNGCerAdcTimeWindowMin,   kDouble,  (UInt_t) fNGCer, 1},
    {"ngcer_adcTimeWindowMax",  fNGCerAdcTimeWindowMax,   kDouble,  (UInt_t) fNGCer, 1},
    {"hgcer_adcTimeWindowMin",  fHGCerAdcTimeWindowMin,   kDouble,  (UInt_t) fHGCer, 1},
    {"hgcer_adcTimeWindowMax",  fHGCerAdcTimeWindowMax,   kDouble,  (UInt_t) fHGCer, 1},
    {0},
  };

  gHcParms->LoadParmValues((DBRequest*)&windowList, prefix);
  /*
  cout << "Double Check the arrays are being read in.\n";
  cout << fHodoPosAdcTimeWindowMin[0] << endl;
  cout << fHodoPosAdcTimeWindowMax[0] << endl;
  cout << fHodoNegAdcTimeWindowMin[0] << endl;
  cout << fHodoNegAdcTimeWindowMax[0] << endl;
  cout << fCaloPosAdcTimeWindowMin[0] << endl;
  cout << fCaloPosAdcTimeWindowMax[0] << endl;
  cout << fCaloNegAdcTimeWindowMin[0] << endl;
  cout << fCaloNegAdcTimeWindowMax[0] << endl;
  cout << fCaloArrAdcTimeWindowMin[0] << endl;
  cout << fCaloArrAdcTimeWindowMax[0] << endl;
  cout << fNGCerAdcTimeWindowMin[0] << endl;
  cout << fNGCerAdcTimeWindowMax[0] << endl;
  cout << fHGCerAdcTimeWindowMin[0] << endl;
  cout << fHGCerAdcTimeWindowMax[0] << endl;
  */

  for(int i =0; i < 224; i++) {
    cout << fCaloArrAdcTimeWindowMin[i] << " " << fCaloArrAdcTimeWindowMax[i] << endl;
  }

  TFile* f1 = new TFile(golden_file, "READ");
  if (f1->IsZombie()) {
    cout << "Cannot find : " << golden_file << endl;
    return;
  }
  //1X HODO PLANE
  TCanvas *c1_hodo_pos1x = new TCanvas("phodo_1x_pos_diff_time_1","phodo_1x_pos_diff_time_1");
  TCanvas *c2_hodo_pos1x = new TCanvas("phodo_1x_pos_diff_time_2","phodo_1x_pos_diff_time_2");
  TCanvas *c1_hodo_neg1x = new TCanvas("phodo_1x_neg_diff_time_1","phodo_1x_neg_diff_time_1");
  TCanvas *c2_hodo_neg1x = new TCanvas("phodo_1x_neg_diff_time_2","phodo_1x_neg_diff_time_2");
  c1_hodo_pos1x->Divide(4,2);   c1_hodo_neg1x->Divide(4,2);
  c2_hodo_pos1x->Divide(4,2);   c2_hodo_neg1x->Divide(4,2);
  //1Y HODO PLANE
  TCanvas *c1_hodo_pos1y = new TCanvas("phodo_1y_pos_diff_time_1","phodo_1y_pos_diff_time_1");
  TCanvas *c2_hodo_pos1y = new TCanvas("phodo_1y_pos_diff_time_2","phodo_1y_pos_diff_time_2");
  TCanvas *c1_hodo_neg1y = new TCanvas("phodo_1y_neg_diff_time_1","phodo_1y_neg_diff_time_1");
  TCanvas *c2_hodo_neg1y = new TCanvas("phodo_1y_neg_diff_time_2","phodo_1y_neg_diff_time_2");
  c1_hodo_pos1y->Divide(4,2);   c1_hodo_neg1y->Divide(4,2);
  c2_hodo_pos1y->Divide(4,2);   c2_hodo_neg1y->Divide(4,2);
  //2X HODO PLANE
  TCanvas *c1_hodo_pos2x = new TCanvas("phodo_2x_pos_diff_time_1","phodo_2x_pos_diff_time_1");
  TCanvas *c2_hodo_pos2x = new TCanvas("phodo_2x_pos_diff_time_2","phodo_2x_pos_diff_time_2");
  TCanvas *c1_hodo_neg2x = new TCanvas("phodo_2x_neg_diff_time_1","phodo_2x_neg_diff_time_1");
  TCanvas *c2_hodo_neg2x = new TCanvas("phodo_2x_neg_diff_time_2","phodo_2x_neg_diff_time_2");
  c1_hodo_pos2x->Divide(4,2);   c1_hodo_neg2x->Divide(4,2);
  c2_hodo_pos2x->Divide(4,2);   c2_hodo_neg2x->Divide(4,2);
  //2Y HODO PLANE
  TCanvas *c1_hodo_pos2y = new TCanvas("phodo_2y_pos_diff_time_1","phodo_2y_pos_diff_time_1");
  TCanvas *c2_hodo_pos2y = new TCanvas("phodo_2y_pos_diff_time_2","phodo_2y_pos_diff_time_2");
  TCanvas *c3_hodo_pos2y = new TCanvas("phodo_2y_pos_diff_time_3","phodo_2y_pos_diff_time_3");
  TCanvas *c1_hodo_neg2y = new TCanvas("phodo_2y_neg_diff_time_1","phodo_2y_neg_diff_time_1");
  TCanvas *c2_hodo_neg2y = new TCanvas("phodo_2y_neg_diff_time_2","phodo_2y_neg_diff_time_2");
  TCanvas *c3_hodo_neg2y = new TCanvas("phodo_2y_neg_diff_time_3","phodo_2y_neg_diff_time_3");
  c1_hodo_pos2y->Divide(4,2);   c1_hodo_neg2y->Divide(4,2);
  c2_hodo_pos2y->Divide(4,2);   c2_hodo_neg2y->Divide(4,2);
  c3_hodo_pos2y->Divide(4,2);   c3_hodo_neg2y->Divide(4,2);

  TCanvas *junk = new TCanvas("junk","junk");
  junk->Divide(4,2);
  
  //Pre-Shower POS
  TCanvas *c1_calo_pos = new TCanvas("pcal_pos_diff_time_1","pcal_pos_diff_time_1");
  TCanvas *c2_calo_pos = new TCanvas("pcal_pos_diff_time_2","pcal_pos_diff_time_2");
  c1_calo_pos->Divide(4,2);     c2_calo_pos->Divide(4,2);
  //Pre-Shower NEG
  TCanvas *c1_calo_neg = new TCanvas("pcal_neg_diff_time_1","pcal_neg_diff_time_1");
  TCanvas *c2_calo_neg = new TCanvas("pcal_neg_diff_time_2","pcal_neg_diff_time_2");
  c1_calo_neg->Divide(4,2);     c2_calo_neg->Divide(4,2);
  //Shower ARR
  TCanvas* c_calo_arr[28];
  //Double_t* fHodoPosAdcTimeWindowMin = new Double_t [fHodoScin];
  for(int i =0; i < 28; i++) {
    c_calo_arr[i] = new TCanvas(Form("pcal_arr_diff_time_%d",i),Form("pcal_arr_diff_time_%d",i));
    c_calo_arr[i]->Divide(4,2);
  }

  //Drift Chamber 1
  TCanvas *c1_dc1 = new TCanvas("pdc1_diff_time","pdc1_diff_time");
  //Drift Chamber 1
  TCanvas *c1_dc2 = new TCanvas("pdc2_diff_time","pdc2_diff_time");
  
  //NGCER and HGCER
  TCanvas *c1_cer = new TCanvas("pcer_diff_time","pcer_diff_time");
  
  TString histname1, histname2, histname3, histname4;

  histname1 = "phodo_1x_good_adctdc_diff_time_vs_pmt_pos";
  histname2 = "phodo_1x_good_adctdc_diff_time_mult1_vs_pmt_pos";
  histname3 = "phodo_1x_good_adctdc_diff_time_mult2_vs_pmt_pos";
  histname4 = "phodo_1x_good_adctdc_diff_time_mult3_vs_pmt_pos";
  makeplot(histname1, histname2, histname3, histname4,
	   c1_hodo_pos1x, c2_hodo_pos1x, junk, 
	   f1, fHodoPosAdcTimeWindowMin, fHodoPosAdcTimeWindowMax,
	   0, peak_1x_pos);
  outFile->cd();
  c1_hodo_pos1x->Write();
  c2_hodo_pos1x->Write();
  f1->cd();

  histname1 = "phodo_1x_good_adctdc_diff_time_vs_pmt_neg";
  histname2 = "phodo_1x_good_adctdc_diff_time_mult1_vs_pmt_neg";
  histname3 = "phodo_1x_good_adctdc_diff_time_mult2_vs_pmt_neg";
  histname4 = "phodo_1x_good_adctdc_diff_time_mult3_vs_pmt_neg";
  makeplot(histname1, histname2, histname3, histname4,
	   c1_hodo_neg1x, c2_hodo_neg1x, junk, 
	   f1, fHodoNegAdcTimeWindowMin, fHodoNegAdcTimeWindowMax,
	   0, peak_1x_neg);
  outFile->cd();
  c1_hodo_neg1x->Write();
  c2_hodo_neg1x->Write();
  f1->cd();

  histname1 = "phodo_1y_good_adctdc_diff_time_vs_pmt_pos";
  histname2 = "phodo_1y_good_adctdc_diff_time_mult1_vs_pmt_pos";
  histname3 = "phodo_1y_good_adctdc_diff_time_mult2_vs_pmt_pos";
  histname4 = "phodo_1y_good_adctdc_diff_time_mult3_vs_pmt_pos";
  makeplot(histname1, histname2, histname3, histname4,
	   c1_hodo_pos1y, c2_hodo_pos1y, junk, 
	   f1, fHodoPosAdcTimeWindowMin, fHodoPosAdcTimeWindowMax,
	   1, peak_1y_pos);
  outFile->cd();
  c1_hodo_pos1y->Write();
  c2_hodo_pos1y->Write();
  f1->cd();

  histname1 = "phodo_1y_good_adctdc_diff_time_vs_pmt_neg";
  histname2 = "phodo_1y_good_adctdc_diff_time_mult1_vs_pmt_neg";
  histname3 = "phodo_1y_good_adctdc_diff_time_mult2_vs_pmt_neg";
  histname4 = "phodo_1y_good_adctdc_diff_time_mult3_vs_pmt_neg";
  makeplot(histname1, histname2, histname3, histname4,
	   c1_hodo_neg1y, c2_hodo_neg1y, junk, 
	   f1, fHodoNegAdcTimeWindowMin, fHodoNegAdcTimeWindowMax,
	   1, peak_1y_neg);
  outFile->cd();
  c1_hodo_neg1y->Write();
  c2_hodo_neg1y->Write();
  f1->cd();

  histname1 = "phodo_2x_good_adctdc_diff_time_vs_pmt_pos";
  histname2 = "phodo_2x_good_adctdc_diff_time_mult1_vs_pmt_pos";
  histname3 = "phodo_2x_good_adctdc_diff_time_mult2_vs_pmt_pos";
  histname4 = "phodo_2x_good_adctdc_diff_time_mult3_vs_pmt_pos";
  makeplot(histname1, histname2, histname3, histname4,
	   c1_hodo_pos2x, c2_hodo_pos2x, junk, 
	   f1, fHodoPosAdcTimeWindowMin, fHodoPosAdcTimeWindowMax,
	   2, peak_2x_pos);
  outFile->cd();
  c1_hodo_pos2x->Write();
  c2_hodo_pos2x->Write();
  f1->cd();

  histname1 = "phodo_2x_good_adctdc_diff_time_vs_pmt_neg";
  histname2 = "phodo_2x_good_adctdc_diff_time_mult1_vs_pmt_neg";
  histname3 = "phodo_2x_good_adctdc_diff_time_mult2_vs_pmt_neg";
  histname4 = "phodo_2x_good_adctdc_diff_time_mult3_vs_pmt_neg";
  makeplot(histname1, histname2, histname3, histname4,
	   c1_hodo_neg2x, c2_hodo_neg2x, junk, 
	   f1, fHodoNegAdcTimeWindowMin, fHodoNegAdcTimeWindowMax,
	   2, peak_2x_neg);
  outFile->cd();
  c1_hodo_neg2x->Write();
  c2_hodo_neg2x->Write();
  f1->cd();

  histname1 = "phodo_2y_good_adctdc_diff_time_vs_pmt_pos";
  histname2 = "phodo_2y_good_adctdc_diff_time_mult1_vs_pmt_pos";
  histname3 = "phodo_2y_good_adctdc_diff_time_mult2_vs_pmt_pos";
  histname4 = "phodo_2y_good_adctdc_diff_time_mult3_vs_pmt_pos";
  makeplot(histname1, histname2, histname3, histname4,
	   c1_hodo_pos2y, c2_hodo_pos2y, c3_hodo_pos2y, 
	   f1, fHodoPosAdcTimeWindowMin, fHodoPosAdcTimeWindowMax,
	   3, peak_2y_pos);
  outFile->cd();
  c1_hodo_pos2y->Write();
  c2_hodo_pos2y->Write();
  c3_hodo_pos2y->Write();
  f1->cd();

  histname1 = "phodo_2y_good_adctdc_diff_time_vs_pmt_neg";
  histname2 = "phodo_2y_good_adctdc_diff_time_mult1_vs_pmt_neg";
  histname3 = "phodo_2y_good_adctdc_diff_time_mult2_vs_pmt_neg";
  histname4 = "phodo_2y_good_adctdc_diff_time_mult3_vs_pmt_neg";
  makeplot(histname1, histname2, histname3, histname4,
	   c1_hodo_neg2y, c2_hodo_neg2y, c3_hodo_neg2y, 
	   f1, fHodoNegAdcTimeWindowMin, fHodoNegAdcTimeWindowMax,
	   3, peak_calo_pr_pos);
  outFile->cd();
  c1_hodo_neg2y->Write();
  c2_hodo_neg2y->Write();
  c3_hodo_neg2y->Write();
  f1->cd();

  histname1 = "pcal_pr_good_adctdc_diff_time_vs_pmt_pos";
  histname2 = "pcal_pr_good_adctdc_diff_time_mult1_vs_pmt_pos";
  histname3 = "pcal_pr_good_adctdc_diff_time_mult2_vs_pmt_pos";
  histname4 = "pcal_pr_good_adctdc_diff_time_mult3_vs_pmt_pos";
  makeplot(histname1, histname2, histname3, histname4,
	   c1_calo_pos,c2_calo_pos, junk, 
	   f1, fCaloPosAdcTimeWindowMin, fCaloPosAdcTimeWindowMax,
	   0, peak_calo_pr_neg);
  outFile->cd();
  c1_calo_pos->Write();
  c2_calo_pos->Write();
  f1->cd();

  
  histname1 = "pcal_pr_good_adctdc_diff_time_vs_pmt_neg";
  histname2 = "pcal_pr_good_adctdc_diff_time_mult1_vs_pmt_neg";
  histname3 = "pcal_pr_good_adctdc_diff_time_mult2_vs_pmt_neg";
  histname4 = "pcal_pr_good_adctdc_diff_time_mult3_vs_pmt_neg";
  makeplot(histname1, histname2, histname3, histname4,
	   c1_calo_neg,c2_calo_neg, junk, 
	   f1, fCaloNegAdcTimeWindowMin, fCaloNegAdcTimeWindowMax,
	   0, peak_calo_fly);
  outFile->cd();
  c1_calo_neg->Write();
  c2_calo_neg->Write();
  f1->cd();

  histname1 = "pcal_fly_good_adctdc_diff_time_vs_pmt";
  histname2 = "pcal_fly_good_adctdc_diff_time_mult1_vs_pmt";
  histname3 = "pcal_fly_good_adctdc_diff_time_mult2_vs_pmt";
  histname4 = "pcal_fly_good_adctdc_diff_time_mult3_vs_pmt";
  makeplotArr(histname1, histname2, histname3, histname4,
	   c_calo_arr, f1, fCaloArrAdcTimeWindowMin, fCaloArrAdcTimeWindowMax,
	   0, peak_crap);
  outFile->cd();
  for(int i=0; i < 28; i++) {
    c_calo_arr[i]->Write();
  }
  f1->cd();

  

  f1->Close();
  outFile->Close();

  int hm = 80 / 2.;
  cout << ";                           1x,  1y,  2x,  2y\n";
  cout << "phodo_PosAdcTimeWindowMin = ";
  bool first=true;
  for(int pmt = 0; pmt < 21; pmt++) {
    if(!first) cout << "                            ";
    cout << peak_1x_pos[pmt]-hm << ",  " << 
      peak_1y_pos[pmt]-hm << ",  " << peak_2x_pos[pmt]-hm << ",  " <<
      peak_2y_pos[pmt]-hm << ",  " << endl;
    first=false;
  }
  cout << ";                           1x,  1y,  2x,  2y\n";
  cout << "phodo_PosAdcTimeWindowMax = ";
  first=true;
  for(int pmt = 0; pmt < 21; pmt++) {
    if(!first) cout << "                            ";
    cout << peak_1x_pos[pmt]+hm << ",  " << 
      peak_1y_pos[pmt]+hm << ",  " << peak_2x_pos[pmt]+hm << ",  " <<
      peak_2y_pos[pmt]+hm << ",  " << endl;
    first=false;
  }

  cout << ";                           1x,  1y,  2x,  2y\n";
  cout << "phodo_NegAdcTimeWindowMin = ";
  first=true;
  for(int pmt = 0; pmt < 21; pmt++) {
    if(!first) cout << "                            ";
    cout << peak_1x_neg[pmt]-hm << ",  " <<
      peak_1y_neg[pmt]-hm << ",  " << peak_2x_neg[pmt]-hm << ",  " <<
      peak_2y_neg[pmt]-hm << ",  " << endl;
    first=false;
  }
  cout << ";                           1x,  1y,  2x,  2y\n";
  cout << "phodo_NegAdcTimeWindowMax = ";
  first=true;
  for(int pmt = 0; pmt < 21; pmt++) {
    if(!first) cout << "                            ";
    cout << peak_1x_neg[pmt]+hm << ",  " <<
      peak_1y_neg[pmt]+hm << ",  " << peak_2x_neg[pmt]+hm << ",  " <<
      peak_2y_neg[pmt]+hm << ",  " << endl;
    first=false;
  }

  return;
}

void makeplot(TString histname1, TString histname2, TString histname3, TString histname4,
	      TCanvas *c1, TCanvas *c2, TCanvas *c3, TFile *f1, Double_t *minArr, Double_t *maxArr,
	      int offset, int *peak_arr) {

  TH2F* H1_diff_time_vs_pmt, *H1_diff_time_mult1_vs_pmt,
    *H1_diff_time_mult2_vs_pmt, *H1_diff_time_mult3_vs_pmt;
  TH1D* H1_pmt, *H1_mult1_pmt, *H1_mult2_pmt, *H1_mult3_pmt;  
  TH1D* tempDraw, *tempMult1, *tempMult2, *tempMult3;
  TLine* minLine, *maxLine;
  double ymax;
  int xpeak;
  f1->GetObject(histname1.Data(), H1_diff_time_vs_pmt);  
  f1->GetObject(histname2.Data(), H1_diff_time_mult1_vs_pmt);  
  f1->GetObject(histname3.Data(), H1_diff_time_mult2_vs_pmt);  
  f1->GetObject(histname4.Data(), H1_diff_time_mult3_vs_pmt);  

  H1_pmt = (TH1D*)H1_diff_time_vs_pmt->ProjectionX("H1_pmt", 1,
                                             H1_diff_time_vs_pmt->GetSize() - 2);
  H1_mult1_pmt = (TH1D*)H1_diff_time_vs_pmt->ProjectionX("H1_mult1_pmt", 1,
                                             H1_diff_time_vs_pmt->GetSize() - 2);
  H1_mult2_pmt = (TH1D*)H1_diff_time_vs_pmt->ProjectionX("H1_mult2_pmt", 1,
                                             H1_diff_time_vs_pmt->GetSize() - 2);
  H1_mult3_pmt = (TH1D*)H1_diff_time_vs_pmt->ProjectionX("H1_mult3_pmt", 1,
					     H1_diff_time_vs_pmt->GetSize() - 2);

  for (int ipmt = 0; ipmt < (H1_pmt->GetSize() - 2); ipmt++) {
    tempDraw = (TH1D*)H1_diff_time_vs_pmt->ProjectionY(
        Form("hodo_pmt%d", ipmt + 1), ipmt + 1, ipmt + 1);
    tempMult1 = (TH1D*)H1_diff_time_mult1_vs_pmt->ProjectionY(
        Form("hodo_pmt_mult1%d", ipmt + 1), ipmt + 1, ipmt + 1);
    tempMult2 = (TH1D*)H1_diff_time_mult2_vs_pmt->ProjectionY(
        Form("hodo_pmt_mult2%d", ipmt + 1), ipmt + 1, ipmt + 1);
    tempMult3 = (TH1D*)H1_diff_time_mult3_vs_pmt->ProjectionY(
        Form("hodo_pmt_mult3%d", ipmt + 1), ipmt + 1, ipmt + 1);

    ymax = tempDraw->GetBinContent(tempDraw->GetMaximumBin());
    xpeak = (int) tempDraw->GetBinCenter(tempDraw->GetMaximumBin());
    if(abs(xpeak) < 20) {
      peak_arr[ipmt] = xpeak;
    }
    //cout << xpeak << endl;
    if(ipmt < 8){
      c1->cd(ipmt + 1);
      tempDraw->Draw();
      //tempMult1->Draw("SAME");
      tempMult1->SetLineColor(kGreen+2);
      tempMult1->Draw("SAME");
      //tempMult3->Draw("SAME");
      c1->Update();
      minLine = new TLine(minArr[ipmt*4+offset],0,
			  minArr[ipmt*4+offset],ymax);
      maxLine = new TLine(maxArr[ipmt*4+offset],0,
			  maxArr[ipmt*4+offset],ymax);
      //cout << "Min: " << minArr[ipmt*4 + offset] << " Max: " <<  maxArr[ipmt*4 + offset] << endl;
      minLine->SetLineColor(kRed);
      maxLine->SetLineColor(kRed);
      minLine->Draw();
      maxLine->Draw();
    }
    else if(ipmt >=8 && ipmt < 16) {
      c2->cd(ipmt + 1 - 8);
      tempDraw->Draw();
      //tempMult1->Draw("SAME");
      tempMult1->SetLineColor(kGreen+2);
      tempMult1->Draw("SAME");
      //tempMult3->Draw("SAME");
      c2->Update();
      minLine = new TLine(minArr[ipmt*4+offset],0,
			  minArr[ipmt*4+offset],ymax);
      maxLine = new TLine(maxArr[ipmt*4+offset],0,
			  maxArr[ipmt*4+offset],ymax);
      //cout << "Min: " << minArr[ipmt*4 + offset] << " Max: " <<  maxArr[ipmt*4 + offset] << endl;
      minLine->SetLineColor(kRed);
      maxLine->SetLineColor(kRed);
      minLine->Draw();
      maxLine->Draw();
    }
    else if(ipmt >= 16 && ipmt <= 21){
      c3->cd(ipmt + 1 - 16);
      tempDraw->Draw();
      //tempMult1->Draw("SAME");
      tempMult1->SetLineColor(kGreen+2);
      tempMult1->Draw("SAME");
      //tempMult3->Draw("SAME");
      c3->Update();
      minLine = new TLine(minArr[ipmt*4+offset],0,
			  minArr[ipmt*4+offset],ymax);
      maxLine = new TLine(maxArr[ipmt*4+offset],0,
			  maxArr[ipmt*4+offset],ymax);
      //cout << "Min: " << minArr[ipmt*4 + offset] << " Max: " <<  maxArr[ipmt*4 + offset] << endl;
      minLine->SetLineColor(kRed);
      maxLine->SetLineColor(kRed);
      minLine->Draw();
      maxLine->Draw();
    } 
    else cout << "WRONG PMT RANGE\n";
  }
  return;
}

void makeplotArr(TString histname1, TString histname2, TString histname3, TString histname4,
	      TCanvas *c_arr[], TFile *f1, Double_t *minArr, Double_t *maxArr,
	      int offset, int *peak_arr) {

  TH2F* H1_diff_time_vs_pmt, *H1_diff_time_mult1_vs_pmt,
    *H1_diff_time_mult2_vs_pmt, *H1_diff_time_mult3_vs_pmt;
  TH1D* H1_pmt, *H1_mult1_pmt, *H1_mult2_pmt, *H1_mult3_pmt;  
  TH1D* tempDraw, *tempMult1, *tempMult2, *tempMult3;
  TLine* minLine, *maxLine;
  double ymax;
  int xpeak;
  int canvasCnt = -1;
  int cdCnt = 0;
  f1->GetObject(histname1.Data(), H1_diff_time_vs_pmt);  
  f1->GetObject(histname2.Data(), H1_diff_time_mult1_vs_pmt);  
  f1->GetObject(histname3.Data(), H1_diff_time_mult2_vs_pmt);  
  f1->GetObject(histname4.Data(), H1_diff_time_mult3_vs_pmt);  

  H1_pmt = (TH1D*)H1_diff_time_vs_pmt->ProjectionX("H1_pmt", 1,
                                             H1_diff_time_vs_pmt->GetSize() - 2);
  H1_mult1_pmt = (TH1D*)H1_diff_time_vs_pmt->ProjectionX("H1_mult1_pmt", 1,
                                             H1_diff_time_vs_pmt->GetSize() - 2);
  H1_mult2_pmt = (TH1D*)H1_diff_time_vs_pmt->ProjectionX("H1_mult2_pmt", 1,
                                             H1_diff_time_vs_pmt->GetSize() - 2);
  H1_mult3_pmt = (TH1D*)H1_diff_time_vs_pmt->ProjectionX("H1_mult3_pmt", 1,
					     H1_diff_time_vs_pmt->GetSize() - 2);

  for (int ipmt = 0; ipmt < (H1_pmt->GetSize() - 2); ipmt++) {
    tempDraw = (TH1D*)H1_diff_time_vs_pmt->ProjectionY(
        Form("calo_fly_pmt%d", ipmt + 1), ipmt + 1, ipmt + 1);
    tempMult1 = (TH1D*)H1_diff_time_mult1_vs_pmt->ProjectionY(
        Form("calo_fly_pmt_mult1%d", ipmt + 1), ipmt + 1, ipmt + 1);
    tempMult2 = (TH1D*)H1_diff_time_mult2_vs_pmt->ProjectionY(
        Form("calo_fly_pmt_mult2%d", ipmt + 1), ipmt + 1, ipmt + 1);
    tempMult3 = (TH1D*)H1_diff_time_mult3_vs_pmt->ProjectionY(
        Form("calo_fly_pmt_mult3%d", ipmt + 1), ipmt + 1, ipmt + 1);

    ymax = tempDraw->GetBinContent(tempDraw->GetMaximumBin());
    xpeak = (int) tempDraw->GetBinCenter(tempDraw->GetMaximumBin());
    if(abs(xpeak) < 20) {
      peak_arr[ipmt] = xpeak;
    }
    //cout << xpeak << endl;

    if(ipmt % 9 == 0) {canvasCnt+=1;}
    if(cdCnt == 8) {cdCnt = 0;}
    cdCnt+=1;
    //cout << canvasCnt << " " << ipmt << endl;
    c_arr[canvasCnt]->cd(cdCnt);
    //cout << cdCnt << endl;
    tempDraw->Draw();
    tempMult1->Draw("SAME");
    tempMult2->Draw("SAME");
    tempMult3->Draw("SAME");
    c_arr[canvasCnt]->Update();
    minLine = new TLine(minArr[ipmt],0,
			minArr[ipmt],ymax);
    maxLine = new TLine(maxArr[ipmt],0,
			maxArr[ipmt],ymax);
    //cout << "Min: " << minArr[ipmt*4 + offset] << " Max: " <<  maxArr[ipmt*4 + offset] << endl;
    minLine->SetLineColor(kRed);
    maxLine->SetLineColor(kRed);
    minLine->Draw();
    maxLine->Draw();
  }
  return;
}




/*


  histname1 = "pdc_1u1_rawtdc";
  histname2 = "pdc_1u1_rawtdc_nhit1";
  histname3 = "pdc_1u1_rawtdc_nhit2";
  histname4 = "pdc_1u1_rawtdc_nhit3";

  histname1 = "pdc_1u2_rawtdc";
  histname2 = "pdc_1u2_rawtdc_nhit1";
  histname3 = "pdc_1u2_rawtdc_nhit2";
  histname4 = "pdc_1u2_rawtdc_nhit3";

  histname1 = "pdc_1x1_rawtdc";
  histname2 = "pdc_1x1_rawtdc_nhit1";
  histname3 = "pdc_1x1_rawtdc_nhit2";
  histname4 = "pdc_1x1_rawtdc_nhit3";

  histname1 = "pdc_1x2_rawtdc";
  histname2 = "pdc_1x2_rawtdc_nhit1";
  histname3 = "pdc_1x2_rawtdc_nhit2";
  histname4 = "pdc_1x2_rawtdc_nhit3";

  histname1 = "pdc_1v1_rawtdc";
  histname2 = "pdc_1v1_rawtdc_nhit1";
  histname3 = "pdc_1v1_rawtdc_nhit2";
  histname4 = "pdc_1v1_rawtdc_nhit3";

  histname1 = "pdc_1v2_rawtdc";
  histname2 = "pdc_1v2_rawtdc_nhit1";
  histname3 = "pdc_1v2_rawtdc_nhit2";
  histname4 = "pdc_1v2_rawtdc_nhit3";

  histname1 = "pdc_2u1_rawtdc";
  histname2 = "pdc_2u1_rawtdc_nhit1";
  histname3 = "pdc_2u1_rawtdc_nhit2";
  histname4 = "pdc_2u1_rawtdc_nhit3";
  
  histname1 = "pdc_2u2_rawtdc";
  histname2 = "pdc_2u2_rawtdc_nhit1";
  histname3 = "pdc_2u2_rawtdc_nhit2";
  histname4 = "pdc_2u2_rawtdc_nhit3";

  histname1 = "pdc_2x1_rawtdc";
  histname2 = "pdc_2x1_rawtdc_nhit1";
  histname3 = "pdc_2x1_rawtdc_nhit2";
  histname4 = "pdc_2x1_rawtdc_nhit3";

  histname1 = "pdc_2x2_rawtdc";
  histname2 = "pdc_2x2_rawtdc_nhit1";
  histname3 = "pdc_2x2_rawtdc_nhit2";
  histname4 = "pdc_2x2_rawtdc_nhit3";

  histname1 = "pdc_2v1_rawtdc";
  histname2 = "pdc_2v1_rawtdc_nhit1";
  histname3 = "pdc_2v1_rawtdc_nhit2";
  histname4 = "pdc_2v1_rawtdc_nhit3";

  histname1 = "pdc_2v2_rawtdc";
  histname2 = "pdc_2v2_rawtdc_nhit1";
  histname3 = "pdc_2v2_rawtdc_nhit2";
  histname4 = "pdc_2v2_rawtdc_nhit3";

  histname1 = "pngcer_good_adctdc_diff_time_vs_pmt";
  histname1 = "pngcer_good_adctdc_diff_time_mult1_vs_pmt";
  histname1 = "pngcer_good_adctdc_diff_time_mult2_vs_pmt";
  histname1 = "pngcer_good_adctdc_diff_time_mult3_vs_pmt";

  histname1 = "phgcer_good_adctdc_diff_time_vs_pmt";
  histname1 = "phgcer_good_adctdc_diff_time_mult1_vs_pmt";
  histname1 = "phgcer_good_adctdc_diff_time_mult2_vs_pmt";
  histname1 = "phgcer_good_adctdc_diff_time_mult3_vs_pmt";
*/
