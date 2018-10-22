#include <iostream>
#include "TFile.h"
#include "TH1D.h"

void ped_tracking_trigger(TString golden_file="",TString trigger="", TString spect=""){

  if (golden_file=="") {
    cout << "Enter golden run root file name: " << endl;
    cin >> golden_file;
  }
  if (trigger=="") {
    cout << "Enter trigger prefix (hgc, shwr, cer, rshwr (SHMS)): " << endl;
    cin >> trigger;    
  }
  if (spect=="") {
    cout << "Enter a spectrometer (p or h): " << endl;
    cin >> spect;
  }

  TString histname = Form("%s%s",spect.Data(),trigger.Data());
  if (histname.Contains("hgc")) histname = Form("%strig_%s_%s",spect.Data(),histname.Data(),"sum_pped");
  if (histname.Contains("shwr") && spect.Contains("p")) histname = Form("%strig_%s_%s",spect.Data(),histname.Data(),"sum_pped");
  if (histname.Contains("cer")) histname = Form("%strig_%s_%s",spect.Data(),histname.Data(),"adc_pped");
  if (histname.Contains("hwr") && spect.Contains("h")) histname = Form("%strig_%s_%s",spect.Data(),histname.Data(),"adc_pped");
  if (histname.Contains("Sum")) histname = Form("%strig_%s_%s",spect.Data(),histname.Data(),"adc_pped");

  //cout << histname << endl;

  TH1F* H1_ped; 
  TH1F* H2_ped; 

  TString protorootpath = Form("%s",gDirectory->GetPath());

  H2_ped = (TH1F*) gDirectory->Get(histname.Data());

  TFile* f1= new TFile(golden_file,"READ");
  if (f1->IsZombie()) {
    cout << "Cannot find : " << golden_file << endl;
    return;
  } 

  f1->GetObject(histname.Data(),H1_ped); 

  gDirectory->cd(Form("%s",protorootpath.Data()));
  
  Double_t H1_ped_peak;
  Double_t H2_ped_peak;
 
  if (H1_ped->GetEntries() > 25) {
    TSpectrum *s = new TSpectrum(1);
    gSystem->RedirectOutput("/dev/null","a");
    s->Search(H1_ped, 1.0, "nobackground&&nodraw", 0.001);
    gSystem->RedirectOutput(0);
    TList *functions = H1_ped->GetListOfFunctions();
    TPolyMarker *pm = (TPolyMarker*)functions->FindObject("TPolyMarker");
    if (pm) {
      H1_ped_peak = *pm->GetX();
    }
    else {
      H1_ped_peak = 1e+38;
    }
  }
  else {
    H1_ped_peak = 1e+38;
  }
  
  if (H2_ped->GetEntries() > 25) {
    TSpectrum *s = new TSpectrum(1);
    gSystem->RedirectOutput("/dev/null","a");
    s->Search(H2_ped, 1.0, "nobackground&&nodraw", 0.001);
    gSystem->RedirectOutput(0);
    TList *functions = H2_ped->GetListOfFunctions();
    TPolyMarker *pm = (TPolyMarker*)functions->FindObject("TPolyMarker");
    if (pm) {
      H2_ped_peak = *pm->GetX();
    }
    else {
      H2_ped_peak = 1e+38;
    }
  }
  else {
    H2_ped_peak = 1e+38;
  }
  
  
  TF1 *Gaussian =  new TF1("Gaussian","[0]*exp(-0.5*((x-[1])/[2])*((x-[1])/[2]))");
  Gaussian->SetParLimits(0,0,1000);
  Gaussian->SetParLimits(2,0,2);

  Double_t golden_mean; Double_t golden_sigma;
  Double_t proto_mean; Double_t proto_sigma;
  Gaussian->SetParameter(1,H1_ped_peak);
  gSystem->RedirectOutput("/dev/null","a");
  H1_ped->Fit(Gaussian,"QMN");
  gSystem->RedirectOutput(0);
  golden_mean = Gaussian->GetParameter(1);
  golden_sigma = Gaussian->GetParameter(2);
  Gaussian->SetParameter(1,H2_ped_peak);
  gSystem->RedirectOutput("/dev/null","a");
  H2_ped->Fit(Gaussian,"QMN");
  gSystem->RedirectOutput(0);
  proto_mean = Gaussian->GetParameter(1);
  proto_sigma = Gaussian->GetParameter(2);

  gSystem->RedirectOutput("/dev/null","a");
  TH1D* Ped_Difference = new TH1D("Ped_Difference",Form("%s Trigger Leg;%s; (Gold -Present) Ped Means (mV)",trigger.Data(),histname.Data()),1,0.5,1.5);
  gSystem->RedirectOutput(0);

  Ped_Difference->SetBinContent(1,golden_mean - proto_mean);
  Ped_Difference->SetBinError(1,sqrt(pow(golden_sigma,2)+pow(proto_sigma,2)));
  
  Double_t histmaxdiff = TMath::Abs(golden_mean - proto_mean)*1.2; 
  if (histmaxdiff < 15) histmaxdiff = 15;

  //Start Drawing
  gStyle->SetOptStat(0);
  Ped_Difference->SetAxisRange(-histmaxdiff,histmaxdiff,"Y");
  Ped_Difference->SetMarkerStyle(8);
  Ped_Difference->SetMarkerSize(1);
  Ped_Difference->DrawClone("PE1");
  gPad->Update();
  TLine *Lower_Limit = new TLine(gPad->GetUxmin(),-3.5,gPad->GetUxmax(),-3.5);
  Lower_Limit->SetLineColor(kRed); Lower_Limit->SetLineWidth(3); Lower_Limit->SetLineStyle(2); Lower_Limit->Draw();
  TLine *Upper_Limit = new TLine(gPad->GetUxmin(),+3.5,gPad->GetUxmax(),+3.5);
  Upper_Limit->SetLineColor(kRed); Upper_Limit->SetLineWidth(3); Upper_Limit->SetLineStyle(2); Upper_Limit->Draw();

  Ped_Difference->~TH1D();
}
