#include <TH1.h>
#include <TFile.h>

void kpp_plots() {

  TFile *run_487_kpp_df = new TFile("./hallc-online_HISTOGRAMS/kpp_487.root");

  TH1D *h_p1X_plTime = new TH1D();
  TH1D *h_p1Y_plTime = new TH1D();
  TH1D *h_p2X_plTime = new TH1D();
  TH1D *h_p2Y_plTime = new TH1D();

  TH2F *h2_hgc_pulseTime = new TH2F();
  TH1D *h_hgc_pulseTime = new TH1D();

  h_p1X_plTime = dynamic_cast <TH1D*> (run_487_kpp_df->Get("h_p1X_plTime"));
  h_p1Y_plTime = dynamic_cast <TH1D*> (run_487_kpp_df->Get("h_p1Y_plTime"));
  h_p2X_plTime = dynamic_cast <TH1D*> (run_487_kpp_df->Get("h_p2X_plTime"));
  h_p2Y_plTime = dynamic_cast <TH1D*> (run_487_kpp_df->Get("h_p2Y_plTime"));

  h2_hgc_pulseTime = dynamic_cast <TH2F*> (run_487_kpp_df->Get("h2_pngc_pulseTime_pT1_diff"));
  h_hgc_pulseTime = h2_hgc_pulseTime->ProjectionY("h_hgc_pulseTime", 3, 3);
  
  h_p1X_plTime->SetLineColor(8);
  h_p1X_plTime->SetFillColor(8);
  h_p1X_plTime->Rebin(2);
  h_p1X_plTime->GetXaxis()->SetRangeUser(20,60);
  h_p1X_plTime->GetYaxis()->SetTitle("Counts / 2 ns");
  		     
  h_p1Y_plTime->SetLineColor(50);
  h_p1Y_plTime->SetFillColor(50);
  h_p1Y_plTime->Rebin(2);
  h_p1Y_plTime->GetXaxis()->SetRangeUser(20,60);
  h_p1Y_plTime->GetYaxis()->SetTitle("Counts / 2 ns");
  
  h_p2X_plTime->SetLineColor(9);
  h_p2X_plTime->SetFillColor(9);
  h_p2X_plTime->Rebin(2);
  h_p2X_plTime->GetXaxis()->SetRangeUser(20,60);
  h_p2X_plTime->GetYaxis()->SetTitle("Counts / 2 ns");
  
  h_p2Y_plTime->SetLineColor(52);
  h_p2Y_plTime->SetFillColor(52);
  h_p2Y_plTime->Rebin(2);
  h_p2Y_plTime->GetXaxis()->SetRangeUser(20,60);
  h_p2Y_plTime->GetYaxis()->SetTitle("Counts / 2 ns");
  
  TCanvas *c_hodo_plTime = new TCanvas("c_hodo_plTime", "", 800, 800);
  c_hodo_plTime->Divide(2, 2);
  c_hodo_plTime->cd(1);
  h_p1X_plTime->Draw();
  c_hodo_plTime->cd(2);
  h_p1Y_plTime->Draw();
  c_hodo_plTime->cd(3);
  h_p2X_plTime->Draw();
  c_hodo_plTime->cd(4);
  h_p2Y_plTime->Draw();

}
