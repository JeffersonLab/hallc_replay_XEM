#include "TFile.h"
#include "TH1D.h"
#include <iostream>
void calc_ped_default(TString , TString ,TString, Double_t);
void run_shms_ped_default(TString);
void run_hms_ped_default(TString);

//
void run_shms_ped_default(TString file_name = "") {
  calc_ped_default(file_name,"hgcer","p",0);
  calc_ped_default(file_name,"ngcer","p",0);
  calc_ped_default(file_name,"aero","p",1);
  calc_ped_default(file_name,"aero","p",2);
  calc_ped_default(file_name,"cal_prshwr","p",1);
  calc_ped_default(file_name,"cal_prshwr","p",2);
  calc_ped_default(file_name,"cal_shwr","p",0);
}
//
void run_hms_ped_default(TString file_name = "") {
  calc_ped_default(file_name,"cer","h",0);
  calc_ped_default(file_name,"cal_hA","h",1);
  calc_ped_default(file_name,"cal_hB","h",1);
  calc_ped_default(file_name,"cal_hC","h",1);
  calc_ped_default(file_name,"cal_hD","h",1);
  calc_ped_default(file_name,"cal_hA","h",2);
  calc_ped_default(file_name,"cal_hB","h",2);

}


void calc_ped_default(TString golden_file = "", TString detector = "",
                           TString spect = "", Double_t polarity = -1) {

  if (golden_file == "") {
    cout << "Enter golden run root file name: " << endl;
    cin >> golden_file;
  }
  if (detector == "") {
    cout << "Enter detector prefix (hodo_1x (etc.), hgcer, aero, cal_prshwr, "
            "cal_shwr, ngcer, cal_hA (etc.)): "
         << endl;
    cin >> detector;
  }
  if (spect == "") {
    cout << "Enter a spectrometer (p or h): " << endl;
    cin >> spect;
  }
  if (polarity == -1) {
    cout << "Enter a detector polarity (pos = 1, neg = 2): " << endl;
    cin >> polarity;
  }

  TString histname = Form("%s%s", spect.Data(), detector.Data());
  if (histname.Contains("hodo") && histname.Contains("1x") && polarity == 1)
    histname = Form("%s%s", histname.Data(), "_good_pped_vs_pmt_pos");
  if (histname.Contains("hodo") && histname.Contains("1x") && polarity == 2)
    histname = Form("%s%s", histname.Data(), "_good_pped_vs_pmt_neg");
  if (histname.Contains("hodo") && histname.Contains("1y") && polarity == 1)
    histname = Form("%s%s", histname.Data(), "_good_pped_vs_pmt_pos");
  if (histname.Contains("hodo") && histname.Contains("1y") && polarity == 2)
    histname = Form("%s%s", histname.Data(), "_good_pped_vs_pmt_neg");
  if (histname.Contains("hodo") && histname.Contains("2x") && polarity == 1)
    histname = Form("%s%s", histname.Data(), "_good_pped_vs_pmt_pos");
  if (histname.Contains("hodo") && histname.Contains("2x") && polarity == 2)
    histname = Form("%s%s", histname.Data(), "_good_pped_vs_pmt_neg");
  if (histname.Contains("hodo") && histname.Contains("2y") && polarity == 1)
    histname = Form("%s%s", histname.Data(), "_good_pped_vs_pmt_pos");
  if (histname.Contains("hodo") && histname.Contains("2y") && polarity == 2)
    histname = Form("%s%s", histname.Data(), "_good_pped_vs_pmt_neg");
  if (histname.Contains("aero") && polarity == 1)
    histname = Form("%s%s", histname.Data(), "_good_pped_vs_pmt_pos");
  if (histname.Contains("aero") && polarity == 2)
    histname = Form("%s%s", histname.Data(), "_good_pped_vs_pmt_neg");
  if (histname.Contains("hcer"))
    histname = Form("%s%s", histname.Data(), "_good_pped_vs_pmt");
  if (histname.Contains("hgcer"))
    histname = Form("%s%s", histname.Data(), "_good_pped_vs_pmt");
  if (histname.Contains("ngcer"))
    histname = Form("%s%s", histname.Data(), "_good_pped_vs_pmt");
  if (histname.Contains("_prshwr") && polarity == 1)
    histname = Form("%s%s", histname.Data(), "_good_pped_vs_pmt_pos");
  if (histname.Contains("_prshwr") && polarity == 2)
    histname = Form("%s%s", histname.Data(), "_good_pped_vs_pmt_neg");
  if (histname.Contains("_shwr"))
    histname = Form("%s%s", histname.Data(), "_good_pped_vs_pmt");
  if (histname.Contains("hcal_h") && polarity == 1)
    histname = Form("%s%s", histname.Data(), "_good_pped_vs_pmt_pos");
  if (histname.Contains("hcal_h") && polarity == 2)
    histname = Form("%s%s", histname.Data(), "_good_pped_vs_pmt_neg");

  TH2F* H1_ped_vs_pmt;

  TFile* f1 = new TFile(golden_file, "READ");
  if (f1->IsZombie()) {
    cout << "Cannot find : " << golden_file << endl;
    return;
  }

  f1->GetObject(histname.Data(), H1_ped_vs_pmt);

  TH1D* H1_pmt;

  H1_pmt = (TH1D*)H1_ped_vs_pmt->ProjectionX("H1_pmt", 1,
                                             H1_ped_vs_pmt->GetSize() - 2);

  TH1D* H1_ped[H1_pmt->GetSize() - 2];

  for (Int_t ipmt = 0; ipmt < (H1_pmt->GetSize() - 2); ipmt++) {
    H1_ped[ipmt] = (TH1D*)H1_ped_vs_pmt->ProjectionY(
        Form("H1_ped_pmt%d", ipmt + 1), ipmt + 1, ipmt + 1);
  }

  Double_t H1_ped_peak[H1_pmt->GetSize() - 2];
  Double_t H1_ped_fitpeak[H1_pmt->GetSize() - 2];
  for (Int_t ipmt = 0; ipmt < (H1_pmt->GetSize() - 2); ipmt++) {

    if (H1_ped[ipmt]->GetEntries() > 25) {
      TSpectrum* s = new TSpectrum(1);
      gSystem->RedirectOutput("/dev/null", "a");
      s->Search(H1_ped[ipmt], 1.0, "nobackground&&nodraw", 0.001);
      gSystem->RedirectOutput(0);
      TList* functions = H1_ped[ipmt]->GetListOfFunctions();
      TPolyMarker* pm = (TPolyMarker*)functions->FindObject("TPolyMarker");
      if (pm) {
        H1_ped_peak[ipmt] = *pm->GetX();
      } else {
        H1_ped_peak[ipmt] = 0;
      }
    } else {
      H1_ped_peak[ipmt] = 0;
    }

  }

  TF1* Gaussian =
      new TF1("Gaussian", "[0]*exp(-0.5*((x-[1])/[2])*((x-[1])/[2]))");
  Gaussian->SetParLimits(0, 0, 1000);
  Gaussian->SetParLimits(2, 0, 2);

  for (Int_t ipmt = 0; ipmt < (H1_pmt->GetSize() - 2); ipmt++) {
    if (H1_ped_peak[ipmt] != 0) {
    Gaussian->SetParameter(1, H1_ped_peak[ipmt]);
    gSystem->RedirectOutput("/dev/null", "a");
    H1_ped[ipmt]->Fit(Gaussian, "QMN");
    gSystem->RedirectOutput(0);
    H1_ped_peak[ipmt]=int(Gaussian->GetParameter(1)*4*4.096);
    }
  }
  TString side="";
  if (polarity ==1)  side="Pos";
  if (polarity ==2)  side="Neg";
  TString PedDefName = spect+detector+"_Ped"+side+"Default= ";
  if (detector == "cal_prshwr") {
    PedDefName = spect+"cal_Ped"+side+"Default= ";
  }
  if (detector == "cal_shwr") {
    PedDefName = spect+"cal_arr_Ped"+side+"Default= ";
  }
  if (detector == "cal_hA") {
    PedDefName = spect+"cal_Ped"+side+"Default= ";
  }
  if (detector == "cal_hB" || detector == "cal_hC" || detector == "cal_hD") {
  } else {
  cout << PedDefName ;
  }
   for (Int_t ipmt = 0; ipmt < (H1_pmt->GetSize() - 2); ipmt++) {
     cout << H1_ped_peak[ipmt] ;
     if (ipmt <(H1_pmt->GetSize() - 3))  cout << ", " ;
       if (ipmt!=0 && ipmt%16 == 0 )  cout << endl;
   } 
   cout << endl;
   if (detector == "cal_hB" && polarity==2) {
    for (Int_t ipmt = 0; ipmt < (H1_pmt->GetSize() - 2); ipmt++) {
     cout << 0 ;
     if (ipmt <(H1_pmt->GetSize() - 3))  cout << ", " ;
       if (ipmt!=0 && ipmt%16 == 0 )  cout << endl;
     } 
   cout << endl;
     for (Int_t ipmt = 0; ipmt < (H1_pmt->GetSize() - 2); ipmt++) {
     cout << 0 ;
     if (ipmt <(H1_pmt->GetSize() - 3))  cout << ", " ;
       if (ipmt!=0 && ipmt%16 == 0 )  cout << endl;
     } 
    cout << endl;
    
   }

}
