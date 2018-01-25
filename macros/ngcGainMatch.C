#define nruns 2
#define ntubes 4
{

 
  //Int_t nruns  = 1;
  //Int_t ntubes = 3;
  Double_t tube1_mean[nruns] = {249.3, 305.4};
  Double_t tube2_mean[nruns] = {197.6, 257.7};
  Double_t tube3_mean[nruns] = {198.4, 220.2};
  Double_t tube4_mean[nruns] = {206.0, 295.1};

  Double_t tube1_mean_err[nruns] = {1.4, 4.1};
  Double_t tube2_mean_err[nruns] = {0.9, 1.6};
  Double_t tube3_mean_err[nruns] = {0.6, 0.9};
  Double_t tube4_mean_err[nruns] = {1.0, 1.9};

  Double_t tube1_hv[nruns] = {1950, 2000}; 
  Double_t tube2_hv[nruns] = {2150, 2200};
  Double_t tube3_hv[nruns] = {1965, 1990};
  Double_t tube4_hv[nruns] = {1887, 1917};

  Double_t zero_err[ntubes] = {0.0, 0.0, 0.0, 0.0};

  TF1 *tube1_fit = new TF1("tube1_fit", "pol1", 2000, 2400);
  TF1 *tube2_fit = new TF1("tube2_fit", "pol1", 2000, 2400);
  TF1 *tube3_fit = new TF1("tube3_fit", "pol1", 2000, 2400);
  TF1 *tube4_fit = new TF1("tube4_fit", "pol1", 2000, 2400);

  tube1_fit->SetLineColor(2);
  tube2_fit->SetLineColor(2);
  tube3_fit->SetLineColor(2);  
  tube4_fit->SetLineColor(2);

  TGraphErrors *tube1_gr = new TGraphErrors(nruns, tube1_hv, tube1_mean, zero_err, tube1_mean_err);
  tube1_gr->SetMarkerSize(1);
  tube1_gr->SetMarkerColor(4);
  tube1_gr->SetMarkerStyle(21);
  tube1_gr->SetTitle("PMT 1");
  //tube1_gr->Fit(tube1_fit, "R");
  TGraphErrors *tube2_gr = new TGraphErrors(nruns, tube2_hv, tube2_mean, zero_err, tube2_mean_err);
  tube2_gr->SetMarkerSize(1);
  tube2_gr->SetMarkerColor(4);
  tube2_gr->SetMarkerStyle(21);
  tube2_gr->SetTitle("PMT 2");
  //tube2_gr->Fit(tube2_fit, "R");
  TGraphErrors *tube3_gr = new TGraphErrors(nruns, tube3_hv, tube3_mean, zero_err, tube3_mean_err);
  tube3_gr->SetMarkerSize(1);
  tube3_gr->SetMarkerColor(4);
  tube3_gr->SetMarkerStyle(21);
  tube3_gr->SetTitle("PMT 3");
  //tube3_gr->Fit(tube3_fit, "R");
  TGraphErrors *tube4_gr = new TGraphErrors(nruns, tube4_hv, tube4_mean, zero_err, tube4_mean_err);
  tube4_gr->SetMarkerSize(1);
  tube4_gr->SetMarkerColor(4);
  tube4_gr->SetMarkerStyle(21);
  tube4_gr->SetTitle("PMT 4");
  //tube4_gr->Fit(tube4_fit, "R");

  TCanvas *can = new TCanvas("can", "can", 1800, 600);
  can->Divide(2, 2);
  can->cd(1);
  //tube1_gr->Fit(tube1_fit, "R");
  tube1_gr->Draw("AP");
  can->cd(2);
  tube2_gr->Draw("AP");
  //tube2_gr->Fit(tube2_fit, "R");
  can->cd(3);
  tube3_gr->Draw("AP");
  //tube3_gr->Fit(tube3_fit, "R");
  can->cd(4);
  tube4_gr->Draw("AP");
  //tube4_gr->Fit(tube4_fit, "R");


}
