#include "TMath.h"

void ngc_gain() {

  static const Int_t ntubes = 4;

  Double_t tube1_mean[ntubes] = {1.68060e+03, 6.23210e+03, 3.41938e+03, 8.28936e+03};
  Double_t tube2_mean[ntubes] = {9.45180e+02, 4.93875e+03, 2.34050e+03, 7.64824e+03};
  Double_t tube3_mean[ntubes] = {1.44629e+03, 7.79568e+03, 3.51568e+03, 1.16708e+04};
  Double_t tube4_mean[ntubes] = {2.06980e+03, 1.03371e+04, 3.74562e+03, 1.49880e+04};

  Double_t tube1_mean_err[ntubes] = {1.30291e+01, 4.75684e+01, 2.56718e+01, 4.84467e+01};
  Double_t tube2_mean_err[ntubes] = {5.42784e+00, 2.24776e+01, 1.51154e+01, 3.22023e+01};
  Double_t tube3_mean_err[ntubes] = {6.76353e+00, 3.58100e+01, 1.72918e+01, 3.75558e+01};
  Double_t tube4_mean_err[ntubes] = {2.46633e+01, 8.98439e+01, 7.70273e+01, 7.62538e+01};

  Double_t tube1_hv[ntubes] = {1900.0, 2200.0, 2050.0, 2300.0}; 
  Double_t tube2_hv[ntubes] = {1900.0, 2200.0, 2050.0, 2300.0};
  Double_t tube3_hv[ntubes] = {1900.0, 2200.0, 2050.0, 2300.0};
  Double_t tube4_hv[ntubes] = {1900.0, 2200.0, 2000.0, 2300.0};

  Double_t zero_err[ntubes] = {0.0, 0.0, 0.0, 0.0};

  TF1 *tube1_fit = new TF1("tube1_fit", "pol1", 1800, 2400);
  TF1 *tube2_fit = new TF1("tube2_fit", "pol1", 1800, 2400);
  TF1 *tube3_fit = new TF1("tube3_fit", "pol1", 1800, 2400);
  TF1 *tube4_fit = new TF1("tube4_fit", "pol1", 1800, 2400);

  tube1_fit->SetLineColor(2);
  tube2_fit->SetLineColor(2);
  tube3_fit->SetLineColor(2);  
  tube4_fit->SetLineColor(2);

  TGraphErrors *tube1_gr = new TGraphErrors(ntubes, tube1_hv, tube1_mean, zero_err, tube1_mean_err);
  tube1_gr->SetMarkerSize(1);
  tube1_gr->SetMarkerColor(4);
  tube1_gr->SetMarkerStyle(21);
  tube1_gr->SetTitle("PMT 1");
  //tube1_gr->Fit(tube1_fit, "R");
  TGraphErrors *tube2_gr = new TGraphErrors(ntubes, tube2_hv, tube2_mean, zero_err, tube2_mean_err);
  tube2_gr->SetMarkerSize(1);
  tube2_gr->SetMarkerColor(4);
  tube2_gr->SetMarkerStyle(21);
  tube2_gr->SetTitle("PMT 2");
  //tube2_gr->Fit(tube2_fit, "R");
  TGraphErrors *tube3_gr = new TGraphErrors(ntubes, tube3_hv, tube3_mean, zero_err, tube3_mean_err);
  tube3_gr->SetMarkerSize(1);
  tube3_gr->SetMarkerColor(4);
  tube3_gr->SetMarkerStyle(21);
  tube3_gr->SetTitle("PMT 3");
  //tube3_gr->Fit(tube3_fit, "R");
  TGraphErrors *tube4_gr = new TGraphErrors(ntubes, tube4_hv, tube4_mean, zero_err, tube4_mean_err);
  tube4_gr->SetMarkerSize(1);
  tube4_gr->SetMarkerColor(4);
  tube4_gr->SetMarkerStyle(21);
  tube4_gr->SetTitle("PMT 4");
  //tube4_gr->Fit(tube4_fit, "R");

  TCanvas *can = new TCanvas("can", "can", 800, 800);
  can->Divide(2, 2);
  can->cd(1);
  tube1_gr->Draw("AP");
  can->cd(2);
  tube2_gr->Draw("AP");
  can->cd(3);
  tube3_gr->Draw("AP");
  can->cd(4);
  tube4_gr->Draw("AP");
  

}
