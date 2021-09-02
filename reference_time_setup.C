#include "TFile.h"
#include "TH1D.h"
#include <iostream>

void run_shms_reference_time_setup(TString infile);
void run_hms_reference_time_setup(TString infile);
void run_coin_reference_time_setup(TString infile);

void run_shms_reference_time_setup(TString infile) {
  gStyle->SetOptStat(0);
  const int n_pdc_refs = 10;
  const int n_pT_refs = 2;
  TFile* f1 = new TFile(infile, "UPDATE");
  if (f1->IsZombie()) {
    cout << "Cannot find : " << infile << endl;
    return;
  }
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

  c1_pdcref->Write();
  c2_pdcref->Write();
  c3_pdcref->Write();
  c4_pdcref->Write();
  c5_pdcref->Write();
  c6_pT->Write();
  c7_pFADC_ROC2->Write();

  //f1->Close();
    
  return;
}

void run_hms_reference_time_setup(TString infile) {
  gStyle->SetOptStat(0);
  int n_hdc_refs = 5;
  const int n_hT_refs = 2;
  TFile* f1 = new TFile(infile, "UPDATE");
  if (f1->IsZombie()) {
    cout << "Cannot find : " << infile << endl;
    return;
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

  c1_hdcref->Write();
  c2_hdcref->Write();
  if(!histoFound) {
    c3_hdcref->Write();
  }
  c6_hT->Write();
  c7_hFADC_ROC1->Write();

  //f1->Close();
    
  return;
}

void run_coin_reference_time_setup(TString infile) {
  cout << "This script is not working yet...\n";
  return;
  TFile* f1 = new TFile(infile, "UPDATE");
  if (f1->IsZombie()) {
    cout << "Cannot find : " << infile << endl;
    return;
  }

  return;
}
