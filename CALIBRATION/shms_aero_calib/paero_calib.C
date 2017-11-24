#include <TFile.h>
#include <TTree.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <TH1.h>
#include <TF1.h>
#include <iostream>
#include <fstream>
#include <iomanip>

//
// Calibrate SHMS aerogel detector by localizing SPE peaks.
// The SPE peak positions are found from Gaussian fits to the peaks.
// The fit ranges can be adjusted by changing fit range limits
// flo_* and fhi_* in the body of code.

// The code works on root output from hcana, the name of which is the
// single input parameter of the program.

#define NPMT 7
#define MaxAdc 40.   //pC
#define NBin   1000

void paero_calib(string fname) {

  TFile *f = new TFile(fname.c_str());
  TTree* tree;
  f->GetObject("T",tree);

  int nentries = tree->GetEntries();
  cout << "nentries= " << nentries << endl;

  double adc_pos[NPMT];
  double adc_neg[NPMT];

  TBranch* b_adc_pos;
  TBranch* b_adc_neg;
 
  tree->SetBranchAddress("P.aero.goodPosAdcPulseInt",adc_pos,&b_adc_pos);
  tree->SetBranchAddress("P.aero.goodNegAdcPulseInt",adc_neg,&b_adc_neg);

  TH1D* hpos[NPMT];
  TH1D* hneg[NPMT];

  for (int i=0; i<NPMT; i++) {
    hpos[i] = new TH1D(Form("hpos%d",i+1), Form("ADC+ %i",i+1), NBin, 0.001, MaxAdc);
    hneg[i] = new TH1D(Form("hneg%d",i+1), Form("ADC- %i",i+1), NBin, 0.001, MaxAdc);
  }

  for (int ientry=0; ientry<nentries; ientry++) {

    tree->GetEntry(ientry);
    
    for (int i=0; i<NPMT; i++) {
      hpos[i]->Fill(adc_pos[i], 1.);
      hneg[i]->Fill(adc_neg[i], 1.);
    }

  }

  TCanvas* c1 = new TCanvas("adc_spec", "fADC spectra", 600, 800);
  c1->Divide(2,NPMT);

  double flo_neg[NPMT] { 8., 8.,10., 8., 7., 5., 8.};
  double fhi_neg[NPMT] {16.,13.,18.,14.,15.,10.,18.};

  double flo_pos[NPMT] { 5., 6., 8.,10., 5., 6., 5.};
  double fhi_pos[NPMT] {10.,12.,14.,17.,10.,12.,11.};

  float gain_pos[NPMT] {NPMT*0.};
  float gain_neg[NPMT] {NPMT*0.};

  int ip=0;
  for (int i=0; i<NPMT; i++) {
    c1->cd(++ip);
    if (hpos[i]->GetSumOfWeights() > 0) {
      hpos[i]->Fit("gaus","","",flo_pos[i],fhi_pos[i]);
      hpos[i]->GetFunction("gaus")->SetLineColor(2);
      hpos[i]->GetFunction("gaus")->SetLineWidth(2);
      gain_pos[i] = hpos[i]->GetFunction("gaus")->GetParameter(1);
    }
    else
      hpos[i]->Draw();
    c1->cd(++ip);
    if (hneg[i]->GetSumOfWeights() > 0) {
      hneg[i]->Fit("gaus","","",flo_neg[i],fhi_neg[i]);
      hneg[i]->GetFunction("gaus")->SetLineColor(2);
      hneg[i]->GetFunction("gaus")->SetLineWidth(2);
      gain_neg[i] = hneg[i]->GetFunction("gaus")->GetParameter(1);
    }
    else
      hneg[i]->Draw();
  }

  ofstream of;
  of.open("gain.r",ios::out);
  of << "paero_neg_gain = ";
  for (int i=0; i<NPMT; i++)
    of << "1./" << gain_neg[i] << ", ";
  of << endl;
  of << "paero_pos_gain = ";
  for (int i=0; i<NPMT; i++)
    of << "1./" << gain_pos[i] << ", ";
  of << endl;
  of.close();

}
