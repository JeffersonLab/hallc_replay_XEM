#include "TFile.h"
#include "TH1D.h"
#include <iostream>

void run_shms_reference_time_setup(TString infile, int RunNumber, TString outfile="move_me.root", TString spec="shms") {
  gROOT->SetBatch(kTRUE);    //do not display plots
  gHcParms->Define("gen_run_number", "Run Number", RunNumber);
  if(spec.CompareTo("shms",TString::kIgnoreCase)==0) {
    gHcParms->AddString("g_ctp_database_filename", "DBASE/SHMS/standard.database");
    gHcParms->Load(gHcParms->GetString("g_ctp_database_filename"), RunNumber);
    gHcParms->Load(gHcParms->GetString("g_ctp_parm_filename"));
    gHcParms->Load(gHcParms->GetString("g_ctp_kinematics_filename"), RunNumber);
    // Load parameters for SHMS trigger configuration
    gHcParms->Load(gHcParms->GetString("g_ctp_trig_config_filename"));
  } else if (spec.CompareTo("coin",TString::kIgnoreCase)==0) {
    gHcParms->AddString("g_ctp_database_filename", "DBASE/COIN/standard.database");
    gHcParms->Load(gHcParms->GetString("g_ctp_database_filename"), RunNumber);
    gHcParms->Load(gHcParms->GetString("g_ctp_parm_filename"));
    gHcParms->Load(gHcParms->GetString("g_ctp_kinematics_filename"), RunNumber);
    // Load params for COIN trigger configuration
    gHcParms->Load(gHcParms->GetString("g_ctp_trig_config_filename"));
    // Load fadc debug parameters
    gHcParms->Load("PARAM/HMS/GEN/h_fadc_debug.param");
    gHcParms->Load("PARAM/SHMS/GEN/p_fadc_debug.param");
  } else {
    cout << "Unknown Spec: " << spec << endl;
    return;
  }

  const char* prefix = "p";

  const int nrefcut = 1;
  Int_t* fshms_tdcrefcut = new Int_t [nrefcut];
  Int_t* fdc_tdcrefcut = new Int_t [nrefcut];
  Int_t* fhodo_tdcrefcut = new Int_t [nrefcut];
  Int_t* fhodo_adcrefcut = new Int_t [nrefcut];
  Int_t* fngcer_adcrefcut = new Int_t [nrefcut];
  Int_t* fhgcer_adcrefcut = new Int_t [nrefcut];
  Int_t* faero_adcrefcut = new Int_t [nrefcut];
  Int_t* fcal_adcrefcut = new Int_t [nrefcut];

  DBRequest windowList[] = {
    {"dc_tdcrefcut", fdc_tdcrefcut, kInt, 1, 1},
    {"hodo_tdcrefcut", fhodo_tdcrefcut, kInt, 1, 1},
    {"hodo_adcrefcut", fhodo_adcrefcut, kInt, 1, 1},
    {"ngcer_adcrefcut", fngcer_adcrefcut, kInt, 1, 1},
    {"hgcer_adcrefcut", fhgcer_adcrefcut, kInt, 1, 1},
    {"aero_adcrefcut", faero_adcrefcut, kInt, 1, 1},
    {"cal_adcrefcut", fcal_adcrefcut, kInt, 1, 1},
    {0},
  };

  const char* trig_prefix = "t";
  DBRequest trigRefList[] = {
    {Form("_%s_trig_tdcrefcut",spec.Data()), fshms_tdcrefcut, kInt, 1, 1},
    {0},
  };

  gHcParms->LoadParmValues((DBRequest*)&trigRefList, trig_prefix);
  gHcParms->LoadParmValues((DBRequest*)&windowList, prefix);

  //cout << fdc_tdcrefcut[0] << endl;
  //cout << fhodo_tdcrefcut[0] << endl;
  //cout << fhodo_adcrefcut[0] << endl;
  //cout << fngcer_adcrefcut[0] << endl;
  //cout << fhgcer_adcrefcut[0] << endl;
  //cout << faero_adcrefcut[0] << endl;
  //cout << fcal_adcrefcut[0] << endl;

  gStyle->SetOptStat(0);
  gROOT->SetBatch(kTRUE);    //do not display plots
  const int n_pdc_refs = 10;
  const int n_pT_refs = 2;
  TFile* f1 = new TFile(infile, "UPDATE");
  if (f1->IsZombie()) {
    cout << "Cannot find : " << infile << endl;
    return;
  }
  TFile* f2 = new TFile(outfile,"RECREATE");
  //Canvases for SHMS Reference Times
  TCanvas *c1_pdcref = new TCanvas("c1_pdcref","c1_pdcref");
  TCanvas *c2_pdcref = new TCanvas("c2_pdcref","c2_pdcref");
  TCanvas *c3_pdcref = new TCanvas("c3_pdcref","c3_pdcref");
  TCanvas *c4_pdcref = new TCanvas("c4_pdcref","c4_pdcref");
  TCanvas *c5_pdcref = new TCanvas("c5_pdcref","c5_pdcref");
  c1_pdcref->Divide(2,2);   c2_pdcref->Divide(2,2);
  c3_pdcref->Divide(2,2);   c4_pdcref->Divide(2,2);
  c5_pdcref->Divide(2,2);
  vector<TCanvas*> c_pdc_refs = {c1_pdcref,c2_pdcref,c3_pdcref,c4_pdcref,c5_pdcref,};
  TCanvas *c6_pT = new TCanvas("c6_pT","c6_pT");
  c6_pT->Divide(2,2);
  TCanvas *c7_pFADC_ROC2 = new TCanvas("c7_pFADC_ROC2","c7_pFADC_ROC2");
  c7_pFADC_ROC2->Divide(2,1);

  vector<TH1F*> pdc_time_raw_ref;
  vector<TH1F*> pdc_tdc_mult_ref;
  vector<TH1F*> pdc_time_raw_ref_mult1;
  vector<TH1F*> pdc_time_raw_ref_mult2;
  vector<TH1F*> pdc_time_raw_ref_mult3;
  int highest_multiplicity;
  for(int i=0; i < n_pdc_refs; i++) {
    pdc_time_raw_ref.push_back((TH1F*) f1->Get(Form("pdc_time_raw_ref%d",i+1)));
    pdc_tdc_mult_ref.push_back((TH1F*) f1->Get(Form("pdc_tdc_mult_ref%d",i+1)));
    pdc_time_raw_ref_mult1.push_back((TH1F*) f1->Get(Form("pdc_time_raw_ref%d_mult1",i+1)));
    pdc_time_raw_ref_mult2.push_back((TH1F*) f1->Get(Form("pdc_time_raw_ref%d_mult2",i+1)));
    pdc_time_raw_ref_mult3.push_back((TH1F*) f1->Get(Form("pdc_time_raw_ref%d_mult3",i+1)));

    c_pdc_refs[(int) i/2]->cd(i%2+1);
    c_pdc_refs[(int) i/2]->cd(i%2+1)->SetLogy();
    pdc_time_raw_ref[i]->Draw();

    int ymax = pdc_time_raw_ref[i]->GetBinContent(pdc_time_raw_ref[i]->GetMaximumBin());
    cout << ymax << " " << fdc_tdcrefcut[0] << endl;
    TLine* cutLine = new TLine(-1*fdc_tdcrefcut[0],0,-1*fdc_tdcrefcut[0],ymax);
    cutLine->SetLineColor(kOrange);
    cutLine->Draw();
    c_pdc_refs[(int) i/2]->cd(i%2+1)->Update();

    highest_multiplicity=pdc_tdc_mult_ref[i]->GetMaximumBin();
    highest_multiplicity=pdc_tdc_mult_ref[i]->GetXaxis()->GetBinCenter(highest_multiplicity);
    if(highest_multiplicity==1) {
      pdc_time_raw_ref_mult1[i]->SetLineColor(kRed);
      pdc_time_raw_ref_mult1[i]->Draw("SAME");
    } else if(highest_multiplicity==2) {
      pdc_time_raw_ref_mult2[i]->SetLineColor(kRed);
      pdc_time_raw_ref_mult2[i]->Draw("SAME");
    } else if(highest_multiplicity==3) {
      pdc_time_raw_ref_mult3[i]->SetLineColor(kRed);
      pdc_time_raw_ref_mult3[i]->Draw("SAME");
    } else {
      cout << "No Max in Mult 1-3\n";
    }
    c_pdc_refs[(int) i/2]->cd(i%2+3);
    pdc_tdc_mult_ref[i]->Draw();
    
  }

  vector<TH1F*> pT_time_raw_ref;
  vector<TH1F*> pT_tdc_mult_ref;
  vector<TH1F*> pT_time_raw_ref_mult1;
  vector<TH1F*> pT_time_raw_ref_mult2;
  vector<TH1F*> pT_time_raw_ref_mult3;
  for(int i=0; i < n_pT_refs; i++) {
    pT_time_raw_ref.push_back((TH1F*) f1->Get(Form("pT%d_tdc_time_raw",i+1)));
    pT_tdc_mult_ref.push_back((TH1F*) f1->Get(Form("pT%d_tdc_mult",i+1)));
    pT_time_raw_ref_mult1.push_back((TH1F*) f1->Get(Form("pT%d_tdc_time_raw_mult1",i+1)));
    pT_time_raw_ref_mult2.push_back((TH1F*) f1->Get(Form("pT%d_tdc_time_raw_mult2",i+1)));
    pT_time_raw_ref_mult3.push_back((TH1F*) f1->Get(Form("pT%d_tdc_time_raw_mult3",i+1)));

    c6_pT->cd(i%2+1);
    c6_pT->cd(i%2+1)->SetLogy();
    pT_time_raw_ref[i]->Draw();

    if(i==0) {
      int ymax = pT_time_raw_ref[i]->GetBinContent(pT_time_raw_ref[i]->GetMaximumBin());
      cout << ymax << " " << fhodo_tdcrefcut[0] << endl;
      TLine* cutLineHODO = new TLine(-1*fhodo_tdcrefcut[0],0,-1*fhodo_tdcrefcut[0],ymax);
      cutLineHODO->SetLineColor(kOrange);
      cutLineHODO->Draw();
      c6_pT->cd(i%2+1)->Update();
    }
    if(i==1) {
      int ymax = pT_time_raw_ref[i]->GetBinContent(pT_time_raw_ref[i]->GetMaximumBin());
      cout << ymax << " " << fshms_tdcrefcut[0] << endl;
      TLine* cutLineHODO = new TLine(-1*fshms_tdcrefcut[0],0,-1*fshms_tdcrefcut[0],ymax);
      cutLineHODO->SetLineColor(kOrange);
      cutLineHODO->Draw();
      c6_pT->cd(i%2+1)->Update();
    }    
    highest_multiplicity=pT_tdc_mult_ref[i]->GetMaximumBin();
    highest_multiplicity=pT_tdc_mult_ref[i]->GetXaxis()->GetBinCenter(highest_multiplicity);
    if(highest_multiplicity==1) {
      pT_time_raw_ref_mult1[i]->SetLineColor(kRed);
      pT_time_raw_ref_mult1[i]->Draw("SAME");
    } else if(highest_multiplicity==2) {
      pT_time_raw_ref_mult2[i]->SetLineColor(kRed);
      pT_time_raw_ref_mult2[i]->Draw("SAME");
    } else if(highest_multiplicity==3) {
      pT_time_raw_ref_mult3[i]->SetLineColor(kRed);
      pT_time_raw_ref_mult3[i]->Draw("SAME");
    } else {
      cout << "No Max in Mult 1-3\n";
    }
    c6_pT->cd(i%2+3);
    pT_tdc_mult_ref[i]->Draw();
    
  }

  TH1F *pFADC_TREF_ROC2_raw_adc;
  TH1F *pFADC_TREF_ROC2_adc_mult;
  TH1F *pFADC_TREF_ROC2_mult1;
  TH1F *pFADC_TREF_ROC2_mult2;
  TH1F *pFADC_TREF_ROC2_mult3;
  pFADC_TREF_ROC2_raw_adc = (TH1F*) f1->Get("pFADC_TREF_ROC2_raw_adc");
  pFADC_TREF_ROC2_adc_mult = (TH1F*) f1->Get("pFADC_TREF_ROC2_adc_mult");
  pFADC_TREF_ROC2_mult1 = (TH1F*) f1->Get("pFADC_TREF_ROC2_mult1");
  pFADC_TREF_ROC2_mult2 = (TH1F*) f1->Get("pFADC_TREF_ROC2_mult2");
  pFADC_TREF_ROC2_mult3 = (TH1F*) f1->Get("pFADC_TREF_ROC2_mult3");

  c7_pFADC_ROC2->cd(1);
  c7_pFADC_ROC2->cd(1)->SetLogy();
  pFADC_TREF_ROC2_raw_adc->Draw();

  int ymax = pFADC_TREF_ROC2_raw_adc->GetBinContent(pFADC_TREF_ROC2_raw_adc->GetMaximumBin());
  cout << ymax << " " << faero_adcrefcut[0] << endl;
  cout << ymax << " " << fhodo_adcrefcut[0] << endl;
  cout << ymax << " " << fcal_adcrefcut[0] << endl;
  cout << ymax << " " << fngcer_adcrefcut[0] << endl;
  cout << ymax << " " << fhgcer_adcrefcut[0] << endl;
  TLine* cutLineAERO = new TLine(-1*faero_adcrefcut[0],0,-1*faero_adcrefcut[0],ymax);
  TLine* cutLineHODO = new TLine(-1*fhodo_adcrefcut[0],0,-1*fhodo_adcrefcut[0],ymax);
  TLine* cutLineCAL = new TLine(-1*fcal_adcrefcut[0],0,-1*fcal_adcrefcut[0],ymax);
  TLine* cutLineNGCER = new TLine(-1*fngcer_adcrefcut[0],0,-1*fngcer_adcrefcut[0],ymax);
  TLine* cutLineHGCER = new TLine(-1*fhgcer_adcrefcut[0],0,-1*fhgcer_adcrefcut[0],ymax);
  cutLineAERO->SetLineColor(kOrange);
  cutLineHODO->SetLineColor(kOrange);
  cutLineCAL->SetLineColor(kOrange);
  cutLineNGCER->SetLineColor(kOrange);
  cutLineHGCER->SetLineColor(kOrange);
  cutLineAERO->Draw();
  cutLineHODO->Draw();
  cutLineCAL->Draw();
  cutLineNGCER->Draw();
  cutLineHGCER->Draw();
  c7_pFADC_ROC2->Update();

  highest_multiplicity=pFADC_TREF_ROC2_adc_mult->GetMaximumBin();
  highest_multiplicity=pFADC_TREF_ROC2_adc_mult->GetXaxis()->GetBinCenter(highest_multiplicity);
  if(highest_multiplicity==1) {
    pFADC_TREF_ROC2_mult1->SetLineColor(kRed);
    pFADC_TREF_ROC2_mult1->Draw("SAME");
  } else if(highest_multiplicity==2) {
    pFADC_TREF_ROC2_mult2->SetLineColor(kRed);
    pFADC_TREF_ROC2_mult2->Draw("SAME");
  } else if(highest_multiplicity==3) {
    pFADC_TREF_ROC2_mult3->SetLineColor(kRed);
    pFADC_TREF_ROC2_mult3->Draw("SAME");
  } else {
    cout << "No Multiplicity Cut is better than others.. Skipping\n";
  }
  c7_pFADC_ROC2->cd(2);
  pFADC_TREF_ROC2_adc_mult->Draw();

  f2->cd();
  c1_pdcref->Write();
  c2_pdcref->Write();
  c3_pdcref->Write();
  c4_pdcref->Write();
  c5_pdcref->Write();
  c6_pT->Write();
  c7_pFADC_ROC2->Write();
  f2->Write();
  f2->Close();
  //f1->Close();
    
  return;
}

void run_hms_reference_time_setup(TString infile, int RunNumber, TString outfile="move_me.root", TString spec="hms") {
  gROOT->SetBatch(kTRUE);    //do not display plots
  // Load Global parameters
  // Add variables to global list.
  gHcParms->Define("gen_run_number", "Run Number", RunNumber);
  if(spec.CompareTo("hms",TString::kIgnoreCase)==0) {
    gHcParms->AddString("g_ctp_database_filename", "DBASE/HMS/standard.database");
    gHcParms->Load(gHcParms->GetString("g_ctp_database_filename"), RunNumber);
    gHcParms->Load(gHcParms->GetString("g_ctp_parm_filename"));
    gHcParms->Load(gHcParms->GetString("g_ctp_kinematics_filename"), RunNumber);
    // Load params for HMS trigger configuration
    gHcParms->Load(gHcParms->GetString("g_ctp_trig_config_filename"));
    // Load fadc debug parameters
    gHcParms->Load("PARAM/HMS/GEN/h_fadc_debug.param");
  } else if (spec.CompareTo("coin",TString::kIgnoreCase)==0) {
    gHcParms->AddString("g_ctp_database_filename", "DBASE/COIN/standard.database");
    gHcParms->Load(gHcParms->GetString("g_ctp_database_filename"), RunNumber);
    gHcParms->Load(gHcParms->GetString("g_ctp_parm_filename"));
    gHcParms->Load(gHcParms->GetString("g_ctp_kinematics_filename"), RunNumber);
    // Load params for COIN trigger configuration
    gHcParms->Load(gHcParms->GetString("g_ctp_trig_config_filename"));
    // Load fadc debug parameters
    gHcParms->Load("PARAM/HMS/GEN/h_fadc_debug.param");
    gHcParms->Load("PARAM/SHMS/GEN/p_fadc_debug.param");
  } else {
    cout << "Unknown Spec: " << spec << endl;
    return;
  }

  const char* prefix = "h";

  const int nrefcut = 1;
  Int_t* fhms_tdcrefcut = new Int_t [nrefcut];
  Int_t* fdc_tdcrefcut = new Int_t [nrefcut];
  Int_t* fhodo_tdcrefcut = new Int_t [nrefcut];
  Int_t* fhodo_adcrefcut = new Int_t [nrefcut];
  Int_t* fcer_adcrefcut = new Int_t [nrefcut];
  Int_t* fcal_adcrefcut = new Int_t [nrefcut];

  DBRequest windowList[] = {
    {"dc_tdcrefcut", fdc_tdcrefcut, kInt, 1, 1},
    {"hodo_tdcrefcut", fhodo_tdcrefcut, kInt, 1, 1},
    {"hodo_adcrefcut", fhodo_adcrefcut, kInt, 1, 1},
    {"cer_adcrefcut", fcer_adcrefcut, kInt, 1, 1},
    {"cal_adcrefcut", fcal_adcrefcut, kInt, 1, 1},
    {0},
  };
  const char* trig_prefix = "t";
  DBRequest trigRefList[] = {
    {Form("_%s_trig_tdcrefcut",spec.Data()), fhms_tdcrefcut, kInt, 1, 1},
    {0},
  };

  gHcParms->LoadParmValues((DBRequest*)&trigRefList, trig_prefix);
  gHcParms->LoadParmValues((DBRequest*)&windowList, prefix);

  //cout << fdc_tdcrefcut[0] << endl;
  //cout << fhodo_tdcrefcut[0] << endl;
  //cout << fhodo_adcrefcut[0] << endl;
  //cout << fcer_adcrefcut[0] << endl;
  //cout << fcal_adcrefcut[0] << endl;
  //cout << fhms_tdcrefcut[0] << endl;

  gStyle->SetOptStat(0);
  gROOT->SetBatch(kTRUE);    //do not display plots
  int n_hdc_refs = 5;
  const int n_hT_refs = 2;
  TFile* f1 = new TFile(infile, "UPDATE");
  if (f1->IsZombie()) {
    cout << "Cannot find : " << infile << endl;
    return;
  }
  TFile* f2;
  if(spec.CompareTo("hms",TString::kIgnoreCase)==0) {
    f2 = new TFile(outfile,"RECREATE");
  } else if (spec.CompareTo("coin",TString::kIgnoreCase)==0) {
    f2 = new TFile(outfile,"UPDATE");
  }

  //Canvases for HMS Reference Times
  TCanvas *c1_hdcref = new TCanvas("c1_hdcref","c1_hdcref");
  TCanvas *c2_hdcref = new TCanvas("c2_hdcref","c2_hdcref");
  TCanvas *c3_hdcref = new TCanvas("c3_hdcref","c3_hdcref");
  c1_hdcref->Divide(2,2);   c2_hdcref->Divide(2,2);
  c3_hdcref->Divide(2,2);
  vector<TCanvas*> c_hdc_refs = {c1_hdcref,c2_hdcref,c3_hdcref};
  TCanvas *c6_hT = new TCanvas("c6_hT","c6_hT");
  c6_hT->Divide(2,2);
  TCanvas *c7_hFADC_ROC1 = new TCanvas("c7_hFADC_ROC1","c7_hFADC_ROC1");
  c7_hFADC_ROC1->Divide(2,1);

  vector<TH1F*> hdc_time_raw_ref;
  vector<TH1F*> hdc_tdc_mult_ref;
  vector<TH1F*> hdc_time_raw_ref_mult1;
  vector<TH1F*> hdc_time_raw_ref_mult2;
  vector<TH1F*> hdc_time_raw_ref_mult3;
  int highest_multiplicity;
  bool histoFound=true;
  for(int i=0; i < n_hdc_refs; i++) {
    histoFound = f1->Get(Form("hdc_time_raw_ref%d",i+1));
    if(!histoFound&&i==4) {cout <<"No DCREF5 in sp18 running\n";continue;}
    hdc_time_raw_ref.push_back((TH1F*) f1->Get(Form("hdc_time_raw_ref%d",i+1)));
    hdc_tdc_mult_ref.push_back((TH1F*) f1->Get(Form("hdc_tdc_mult_ref%d",i+1)));
    hdc_time_raw_ref_mult1.push_back((TH1F*) f1->Get(Form("hdc_time_raw_ref%d_mult1",i+1)));
    hdc_time_raw_ref_mult2.push_back((TH1F*) f1->Get(Form("hdc_time_raw_ref%d_mult2",i+1)));
    hdc_time_raw_ref_mult3.push_back((TH1F*) f1->Get(Form("hdc_time_raw_ref%d_mult3",i+1)));

    c_hdc_refs[(int) i/2]->cd(i%2+1);
    c_hdc_refs[(int) i/2]->cd(i%2+1)->SetLogy();
    hdc_time_raw_ref[i]->Draw();

    int ymax = hdc_time_raw_ref[i]->GetBinContent(hdc_time_raw_ref[i]->GetMaximumBin());
    cout << ymax << " " << fdc_tdcrefcut[0] << endl;
    TLine* cutLine = new TLine(-1*fdc_tdcrefcut[0],0,-1*fdc_tdcrefcut[0],ymax);
    cutLine->SetLineColor(kOrange);
    cutLine->Draw();
    c_hdc_refs[(int) i/2]->cd(i%2+1)->Update();

    highest_multiplicity=hdc_tdc_mult_ref[i]->GetMaximumBin();
    highest_multiplicity=hdc_tdc_mult_ref[i]->GetXaxis()->GetBinCenter(highest_multiplicity);
    if(highest_multiplicity==1) {
      hdc_time_raw_ref_mult1[i]->SetLineColor(kRed);
      hdc_time_raw_ref_mult1[i]->Draw("SAME");
    } else if(highest_multiplicity==2) {
      hdc_time_raw_ref_mult2[i]->SetLineColor(kRed);
      hdc_time_raw_ref_mult2[i]->Draw("SAME");
    } else if(highest_multiplicity==3) {
      hdc_time_raw_ref_mult3[i]->SetLineColor(kRed);
      hdc_time_raw_ref_mult3[i]->Draw("SAME");
    } else {
      cout << "No Max in Mult 1-3\n";
    }
    c_hdc_refs[(int) i/2]->cd(i%2+3);
    hdc_tdc_mult_ref[i]->Draw();
    
  }
  
  vector<TH1F*> hT_time_raw_ref;
  vector<TH1F*> hT_tdc_mult_ref;
  vector<TH1F*> hT_time_raw_ref_mult1;
  vector<TH1F*> hT_time_raw_ref_mult2;
  vector<TH1F*> hT_time_raw_ref_mult3;
  for(int i=0; i < n_hT_refs; i++) {
    hT_time_raw_ref.push_back((TH1F*) f1->Get(Form("hT%d_tdc_time_raw",i+1)));
    hT_tdc_mult_ref.push_back((TH1F*) f1->Get(Form("hT%d_tdc_mult",i+1)));
    hT_time_raw_ref_mult1.push_back((TH1F*) f1->Get(Form("hT%d_tdc_time_raw_mult1",i+1)));
    hT_time_raw_ref_mult2.push_back((TH1F*) f1->Get(Form("hT%d_tdc_time_raw_mult2",i+1)));
    hT_time_raw_ref_mult3.push_back((TH1F*) f1->Get(Form("hT%d_tdc_time_raw_mult3",i+1)));

    c6_hT->cd(i%2+1);
    c6_hT->cd(i%2+1)->SetLogy();
    hT_time_raw_ref[i]->Draw();
    if(i==0) {
      int ymax = hT_time_raw_ref[i]->GetBinContent(hT_time_raw_ref[i]->GetMaximumBin());
      cout << ymax << " " << fhms_tdcrefcut[0] << endl;
      TLine* cutLine = new TLine(-1*fhms_tdcrefcut[0],0,-1*fhms_tdcrefcut[0],ymax);
      cutLine->SetLineColor(kOrange);
      cutLine->Draw();
      c6_hT->cd(i%2+1)->Update();
    }
    if(i==1) {
      int ymax = hT_time_raw_ref[i]->GetBinContent(hT_time_raw_ref[i]->GetMaximumBin());
      cout << ymax << " " << fhodo_tdcrefcut[0] << endl;
      TLine* cutLine = new TLine(-1*fhodo_tdcrefcut[0],0,-1*fhodo_tdcrefcut[0],ymax);
      cutLine->SetLineColor(kOrange);
      cutLine->Draw();
      c6_hT->cd(i%2+1)->Update();
    }

    highest_multiplicity=hT_tdc_mult_ref[i]->GetMaximumBin();
    highest_multiplicity=hT_tdc_mult_ref[i]->GetXaxis()->GetBinCenter(highest_multiplicity);
    if(highest_multiplicity==1) {
      hT_time_raw_ref_mult1[i]->SetLineColor(kRed);
      hT_time_raw_ref_mult1[i]->Draw("SAME");
    } else if(highest_multiplicity==2) {
      hT_time_raw_ref_mult2[i]->SetLineColor(kRed);
      hT_time_raw_ref_mult2[i]->Draw("SAME");
    } else if(highest_multiplicity==3) {
      hT_time_raw_ref_mult3[i]->SetLineColor(kRed);
      hT_time_raw_ref_mult3[i]->Draw("SAME");
    } else {
      cout << "No Max in Mult 1-3\n";
    }
    c6_hT->cd(i%2+3);
    hT_tdc_mult_ref[i]->Draw();
    
  }

  TH1F *hFADC_TREF_ROC1_raw_adc;
  TH1F *hFADC_TREF_ROC1_adc_mult;
  TH1F *hFADC_TREF_ROC1_mult1;
  TH1F *hFADC_TREF_ROC1_mult2;
  TH1F *hFADC_TREF_ROC1_mult3;
  hFADC_TREF_ROC1_raw_adc = (TH1F*) f1->Get("hFADC_TREF_ROC1_raw_tdc");
  hFADC_TREF_ROC1_adc_mult = (TH1F*) f1->Get("hFADC_TREF_ROC1_adc_mult");
  hFADC_TREF_ROC1_mult1 = (TH1F*) f1->Get("hFADC_TREF_ROC1_mult1");
  hFADC_TREF_ROC1_mult2 = (TH1F*) f1->Get("hFADC_TREF_ROC1_mult2");
  hFADC_TREF_ROC1_mult3 = (TH1F*) f1->Get("hFADC_TREF_ROC1_mult3");

  c7_hFADC_ROC1->cd(1);
  c7_hFADC_ROC1->cd(1)->SetLogy();
  hFADC_TREF_ROC1_raw_adc->Draw();

  int ymax = hFADC_TREF_ROC1_raw_adc->GetBinContent(hFADC_TREF_ROC1_raw_adc->GetMaximumBin());
  cout << ymax << " " << fhodo_adcrefcut[0] << endl;
  cout << ymax << " " << fcal_adcrefcut[0] << endl;
  cout << ymax << " " << fcer_adcrefcut[0] << endl;
  TLine* cutLineHODO = new TLine(-1*fhodo_adcrefcut[0],0,-1*fhodo_adcrefcut[0],ymax);
  TLine* cutLineCAL = new TLine(-1*fcal_adcrefcut[0],0,-1*fcal_adcrefcut[0],ymax);
  TLine* cutLineCER = new TLine(-1*fcer_adcrefcut[0],0,-1*fcer_adcrefcut[0],ymax);
  cutLineHODO->SetLineColor(kOrange);
  cutLineCAL->SetLineColor(kOrange);
  cutLineCER->SetLineColor(kOrange);
  cutLineHODO->Draw();
  cutLineCAL->Draw();
  cutLineCER->Draw();
  c7_hFADC_ROC1->Update();


  highest_multiplicity=hFADC_TREF_ROC1_adc_mult->GetMaximumBin();
  highest_multiplicity=hFADC_TREF_ROC1_adc_mult->GetXaxis()->GetBinCenter(highest_multiplicity);
  if(highest_multiplicity==1) {
    hFADC_TREF_ROC1_mult1->SetLineColor(kRed);
    hFADC_TREF_ROC1_mult1->Draw("SAME");
  } else if(highest_multiplicity==2) {
    hFADC_TREF_ROC1_mult2->SetLineColor(kRed);
    hFADC_TREF_ROC1_mult2->Draw("SAME");
  } else if(highest_multiplicity==3) {
    hFADC_TREF_ROC1_mult3->SetLineColor(kRed);
    hFADC_TREF_ROC1_mult3->Draw("SAME");
  } else {
    cout << "No Multiplicity Cut is better than others.. Skipping\n";
  }
  c7_hFADC_ROC1->cd(2);
  hFADC_TREF_ROC1_adc_mult->Draw();

  f2->cd();
  c1_hdcref->Write();
  c2_hdcref->Write();
  if(histoFound) {
    c3_hdcref->Write();
  }
  c6_hT->Write();
  c7_hFADC_ROC1->Write();
  f2->Write();
  f2->Close();
  //f1->Close();
    
  return;
}

void run_coin_reference_time_setup(TString infile, int RunNumber, TString outfile="move_me.root") {
  
  run_shms_reference_time_setup(infile, RunNumber, outfile,"coin");
  run_hms_reference_time_setup(  infile, RunNumber, outfile,"coin");
  return;
}
