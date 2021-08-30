#include "TFile.h"
#include "TH1D.h"
#include <iostream>
void calc_timing_windows(TString, TString , TString ,TString, Double_t, Double_t[], Double_t[], bool, double);

void run_shms_timing_windows(TString, TString, int, bool, double);
void run_hms_ped_timing_windows(TString);

void run_shms_timing_windows(TString file_name = "", TString out_file="", int RunNumber = 0, bool newWindows=false, double width=40.) {

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

  UInt_t nPMT = 21;   //2Y plane has 21 PMTs
  UInt_t nPlanes = 4; //1x, 1y, 2x, 2y
  UInt_t fHodoScin = nPlanes * nPMT; //This could be determined from hcana DB
  Double_t* fHodoPosAdcTimeWindowMin = new Double_t [fHodoScin];
  Double_t* fHodoPosAdcTimeWindowMax = new Double_t [fHodoScin];
  Double_t* fHodoNegAdcTimeWindowMin = new Double_t [fHodoScin];
  Double_t* fHodoNegAdcTimeWindowMax = new Double_t [fHodoScin];

  UInt_t fPrShwr = 14;
  Double_t* fCaloPosAdcTimeWindowMin = new Double_t [fPrShwr];
  Double_t* fCaloPosAdcTimeWindowMax = new Double_t [fPrShwr];
  Double_t* fCaloNegAdcTimeWindowMin = new Double_t [fPrShwr];
  Double_t* fCaloNegAdcTimeWindowMax = new Double_t [fPrShwr];

  UInt_t fFlyShwr = 224;
  Double_t* fCaloArrAdcTimeWindowMin = new Double_t [fFlyShwr];
  Double_t* fCaloArrAdcTimeWindowMax = new Double_t [fFlyShwr];

  UInt_t fNGCer = 4;
  Double_t* fNGCerAdcTimeWindowMin = new Double_t [fNGCer];
  Double_t* fNGCerAdcTimeWindowMax = new Double_t [fNGCer];

  UInt_t fHGCer = 4;
  Double_t* fHGCerAdcTimeWindowMin = new Double_t [fHGCer];
  Double_t* fHGCerAdcTimeWindowMax = new Double_t [fHGCer];

  DBRequest windowList[] = {
    {"hodo_PosAdcTimeWindowMin", fHodoPosAdcTimeWindowMin, kDouble, fHodoScin, 1},
    {"hodo_PosAdcTimeWindowMax", fHodoPosAdcTimeWindowMax, kDouble, fHodoScin, 1},
    {"hodo_NegAdcTimeWindowMin", fHodoNegAdcTimeWindowMin, kDouble, fHodoScin, 1},
    {"hodo_NegAdcTimeWindowMax", fHodoNegAdcTimeWindowMax, kDouble, fHodoScin, 1},
    {"cal_pos_AdcTimeWindowMin", fCaloPosAdcTimeWindowMin, kDouble, fPrShwr, 1},
    {"cal_pos_AdcTimeWindowMax", fCaloPosAdcTimeWindowMax, kDouble, fPrShwr, 1},
    {"cal_neg_AdcTimeWindowMin", fCaloNegAdcTimeWindowMin, kDouble, fPrShwr, 1},
    {"cal_neg_AdcTimeWindowMax", fCaloNegAdcTimeWindowMax, kDouble, fPrShwr, 1},
    {"cal_arr_AdcTimeWindowMin", fCaloArrAdcTimeWindowMin, kDouble, fFlyShwr, 1},
    {"cal_arr_AdcTimeWindowMax", fCaloArrAdcTimeWindowMax, kDouble, fFlyShwr, 1},
    {"ngcer_adcTimeWindowMin",  fNGCerAdcTimeWindowMin,   kDouble, fNGCer, 1},
    {"ngcer_adcTimeWindowMax",  fNGCerAdcTimeWindowMax,   kDouble, fNGCer, 1},
    {"hgcer_adcTimeWindowMin",  fHGCerAdcTimeWindowMin,   kDouble, fHGCer, 1},
    {"hgcer_adcTimeWindowMax",  fHGCerAdcTimeWindowMax,   kDouble, fHGCer, 1},
    {0},
  };

  gHcParms->LoadParmValues((DBRequest*)&windowList, prefix);

  calc_timing_windows(file_name,out_file,"hodo_1x","p",1,fHodoPosAdcTimeWindowMin, fHodoPosAdcTimeWindowMax,newWindows,width);
  calc_timing_windows(file_name,out_file,"hodo_1x","p",2,fHodoNegAdcTimeWindowMin, fHodoNegAdcTimeWindowMax,newWindows,width);
  calc_timing_windows(file_name,out_file,"hodo_1y","p",1,fHodoPosAdcTimeWindowMin, fHodoPosAdcTimeWindowMax,newWindows,width);
  calc_timing_windows(file_name,out_file,"hodo_1y","p",2,fHodoNegAdcTimeWindowMin, fHodoNegAdcTimeWindowMax,newWindows,width);
  calc_timing_windows(file_name,out_file,"hodo_2x","p",1,fHodoPosAdcTimeWindowMin, fHodoPosAdcTimeWindowMax,newWindows,width);
  calc_timing_windows(file_name,out_file,"hodo_2x","p",2,fHodoNegAdcTimeWindowMin, fHodoNegAdcTimeWindowMax,newWindows,width);
  calc_timing_windows(file_name,out_file,"hodo_2y","p",1,fHodoPosAdcTimeWindowMin, fHodoPosAdcTimeWindowMax,newWindows,width);
  calc_timing_windows(file_name,out_file,"hodo_2y","p",2,fHodoNegAdcTimeWindowMin, fHodoNegAdcTimeWindowMax,newWindows,width);
  calc_timing_windows(file_name,out_file,"cal_prshwr","p",1,fCaloPosAdcTimeWindowMin,fCaloPosAdcTimeWindowMax,newWindows,width);
  calc_timing_windows(file_name,out_file,"cal_prshwr","p",2,fCaloNegAdcTimeWindowMin,fCaloNegAdcTimeWindowMax,newWindows,width);
  calc_timing_windows(file_name,out_file,"cal_shwr","p",0,fCaloArrAdcTimeWindowMin,fCaloArrAdcTimeWindowMax,newWindows,width);
  calc_timing_windows(file_name,out_file,"hgcer","p",0,fHGCerAdcTimeWindowMin,fHGCerAdcTimeWindowMax,newWindows,width);
  calc_timing_windows(file_name,out_file,"ngcer","p",0,fNGCerAdcTimeWindowMin,fNGCerAdcTimeWindowMax,newWindows,width);
}
/*
void run_hms_timing_windows(TString file_name = "") {
  calc_timing_windows(file_name,"cer","h",0);
  calc_timing_windows(file_name,"cal_hA","h",1);
  calc_timing_windows(file_name,"cal_hB","h",1);
  calc_timing_windows(file_name,"cal_hC","h",1);
  calc_timing_windows(file_name,"cal_hD","h",1);
  calc_timing_windows(file_name,"cal_hA","h",2);
  calc_timing_windows(file_name,"cal_hB","h",2);

}
*/

void calc_timing_windows(TString golden_file = "", TString out_file = "",
			 TString detector = "", TString spect = "", Double_t polarity = -1,
			 Double_t *minArr = 0, Double_t *maxArr = 0, bool newWindows=false, double width=40.) {
  if (golden_file == "") {
    cout << "Enter golden run root file name: " << endl;
    cin >> golden_file;
  }
  if (out_file == "") {
    cout << "Enter golden run root file name: " << endl;
    cin >> out_file;
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

  UInt_t offset;
  TString histname = Form("%s%s", spect.Data(), detector.Data());
  if (histname.Contains("hodo") && histname.Contains("1x") && polarity == 1) {
    histname = Form("%s%s", histname.Data(), "_good_adctdc_diff_time_vs_pmt_pos");
    offset = 0;
  }
  if (histname.Contains("hodo") && histname.Contains("1x") && polarity == 2) {
    histname = Form("%s%s", histname.Data(), "_good_adctdc_diff_time_vs_pmt_neg");
    offset = 0;
  }
  if (histname.Contains("hodo") && histname.Contains("1y") && polarity == 1) {
    histname = Form("%s%s", histname.Data(), "_good_adctdc_diff_time_vs_pmt_pos");
    offset = 1;
  }
  if (histname.Contains("hodo") && histname.Contains("1y") && polarity == 2) {
    histname = Form("%s%s", histname.Data(), "_good_adctdc_diff_time_vs_pmt_neg");
    offset = 1;
  }
  if (histname.Contains("hodo") && histname.Contains("2x") && polarity == 1) {
    histname = Form("%s%s", histname.Data(), "_good_adctdc_diff_time_vs_pmt_pos");
    offset = 2;
  }
  if (histname.Contains("hodo") && histname.Contains("2x") && polarity == 2) {
    histname = Form("%s%s", histname.Data(), "_good_adctdc_diff_time_vs_pmt_neg");
    offset = 2;
  }
  if (histname.Contains("hodo") && histname.Contains("2y") && polarity == 1) {
    histname = Form("%s%s", histname.Data(), "_good_adctdc_diff_time_vs_pmt_pos");
    offset = 3;
  }
  if (histname.Contains("hodo") && histname.Contains("2y") && polarity == 2) {
    histname = Form("%s%s", histname.Data(), "_good_adctdc_diff_time_vs_pmt_neg");
    offset = 3;
  }
  if (histname.Contains("aero") && polarity == 1)
    histname = Form("%s%s", histname.Data(), "_good_adctdc_diff_time_vs_pmt_pos");
  if (histname.Contains("aero") && polarity == 2)
    histname = Form("%s%s", histname.Data(), "_good_adctdc_diff_time_vs_pmt_neg");
  if (histname.Contains("hcer"))
    histname = Form("%s%s", histname.Data(), "_good_adctdc_diff_time_vs_pmt");
  if (histname.Contains("hgcer"))
    histname = Form("%s%s", histname.Data(), "_good_adctdc_diff_time_vs_pmt");
  if (histname.Contains("ngcer"))
    histname = Form("%s%s", histname.Data(), "_good_adctdc_diff_time_vs_pmt");
  if (histname.Contains("_prshwr") && polarity == 1)
    histname = Form("%s%s", histname.Data(), "_good_adctdc_diff_time_vs_pmt_pos");
  if (histname.Contains("_prshwr") && polarity == 2)
    histname = Form("%s%s", histname.Data(), "_good_adctdc_diff_time_vs_pmt_neg");
  if (histname.Contains("_shwr"))
    histname = Form("%s%s", histname.Data(), "_good_adctdc_diff_time_vs_pmt");
  if (histname.Contains("hcal_h") && polarity == 1)
    histname = Form("%s%s", histname.Data(), "_good_adctdc_diff_time_vs_pmt_pos");
  if (histname.Contains("hcal_h") && polarity == 2)
    histname = Form("%s%s", histname.Data(), "_good_adctdc_diff_time_vs_pmt_neg");

  TH2F* H1_adctdc_diff_time_vs_pmt;

  TFile* f1 = new TFile(golden_file, "READ");
  if (f1->IsZombie()) {
    cout << "Cannot find : " << golden_file << endl;
    return;
  }

  f1->GetObject(histname.Data(), H1_adctdc_diff_time_vs_pmt);

  TFile* f2 = new TFile(out_file, "Update");
  if (f2->IsZombie()) {
    cout << "Cannot find : " << out_file << endl;
    return;
  }

  TH1D* H1_pmt;
  TCanvas *currentCanvas = nullptr;
  H1_pmt = (TH1D*)H1_adctdc_diff_time_vs_pmt->ProjectionX("H1_pmt", 1,
                                             H1_adctdc_diff_time_vs_pmt->GetSize() - 2);

  TH1D* H1_adctdc_diff_time[H1_pmt->GetSize() - 2];

  for (Int_t ipmt = 0; ipmt < (H1_pmt->GetSize() - 2); ipmt++) {
    H1_adctdc_diff_time[ipmt] = (TH1D*)H1_adctdc_diff_time_vs_pmt->ProjectionY(
        Form("H1_adctdc_diff_time_pmt%d", ipmt + 1), ipmt + 1, ipmt + 1);
  }

  Double_t H1_adctdc_diff_time_peak[H1_pmt->GetSize() - 2];
  Double_t H1_adctdc_diff_time_fitpeak[H1_pmt->GetSize() - 2];
  for (Int_t ipmt = 0; ipmt < (H1_pmt->GetSize() - 2); ipmt++) {

    if (H1_adctdc_diff_time[ipmt]->GetEntries() > 1000) {
      TSpectrum* s = new TSpectrum(1);
      gSystem->RedirectOutput("/dev/null", "a");
      s->Search(H1_adctdc_diff_time[ipmt], 1.0, "nobackground&&nodraw", 0.001);
      gSystem->RedirectOutput(0);
      TList* functions = H1_adctdc_diff_time[ipmt]->GetListOfFunctions();
      TPolyMarker* pm = (TPolyMarker*)functions->FindObject("TPolyMarker");
      if (pm) {
        H1_adctdc_diff_time_peak[ipmt] = *pm->GetX();
      } else {
        H1_adctdc_diff_time_peak[ipmt] = 0;
      }
    } else {
      H1_adctdc_diff_time_peak[ipmt] = 0;
    }

  TLine *minLine, *maxLine;
  double ymax, maxVal, minVal;
  Int_t nCanvas = ceil((ipmt+1) / 15.);
  TString side="";
  if (polarity ==1)  side="_pos";
  if (polarity ==2)  side="_neg";
  TString canvasName = Form("%s%s%s%s%d",spect.Data(),detector.Data(),side.Data(),"_diff_time_",nCanvas);
  if(ceil((ipmt / 15.)) < nCanvas) {
    f2->cd();
    if(currentCanvas!=NULL) {
      currentCanvas->Write();
    }
    currentCanvas = new TCanvas(canvasName,canvasName);
    f1->cd();
    currentCanvas->Divide(5,3);
  }
  int pad = 1 + (ipmt%15);
  currentCanvas->cd(pad);
  TString oldTitle = H1_adctdc_diff_time[ipmt]->GetTitle();
  TString subTitle = oldTitle(0,oldTitle.Length()-14);
  TString newTitle = Form("%s%s%d",subTitle.Data(),"PMT ",ipmt+1);
  H1_adctdc_diff_time[ipmt]->SetTitle(newTitle);
  H1_adctdc_diff_time[ipmt]->GetXaxis()->CenterTitle();
    H1_adctdc_diff_time[ipmt]->GetXaxis()->SetTitleOffset(1);
  H1_adctdc_diff_time[ipmt]->Draw();
  currentCanvas->Update();
  ymax = H1_adctdc_diff_time[ipmt]->GetBinContent(H1_adctdc_diff_time[ipmt]->GetMaximumBin());
  if(histname.Contains("hodo")) {
    minVal = minArr[ipmt*4+offset];
    maxVal = maxArr[ipmt*4+offset];
  } else {
    minVal = minArr[ipmt];
    maxVal = maxArr[ipmt];
  }
  minLine = new TLine(minVal,0,minVal,ymax);
  maxLine = new TLine(maxVal,0,maxVal,ymax);
  if(H1_adctdc_diff_time_peak[ipmt] > minVal && H1_adctdc_diff_time_peak[ipmt] < maxVal) {
    minLine->SetLineColor(kGreen);
    maxLine->SetLineColor(kGreen);
  } else {
    minLine->SetLineColor(kRed);
    maxLine->SetLineColor(kRed);      
  }
  minLine->Draw();
  maxLine->Draw();
  currentCanvas->Update();

  }
  if(newWindows==true) {
    TString bound = "";
    //hodo_PosAdcTimeWindowMin
    double halfWindow = -1*width / 2.;
    for(int i=0; i < 2; i++) {
      if(i==0) bound = "Min";
      if(i==1) bound = "Max";
      TString side="";
      if (polarity ==1)  side="Pos";
      if (polarity ==2)  side="Neg";
      TString DetectorPeakName = spect+detector+"_"+side+"AdcTimeWindow"+bound+"= ";
      if (detector == "cal_prshwr") {
	DetectorPeakName = spect+"cal_"+side+"AdcTimeWindow"+bound+"= ";
      }
      if (detector == "cal_shwr") {
	DetectorPeakName = spect+"cal_arr_"+side+"AdcTimeWindow"+bound+"= ";
      }
      if (detector == "cal_hA") {
	DetectorPeakName = spect+"cal_"+side+"AdcTimeWindow"+bound+"= ";
      }
      if (detector == "cal_hB" || detector == "cal_hC" || detector == "cal_hD") {
      } else {
	cout << DetectorPeakName ;
      }
      for (Int_t ipmt = 0; ipmt < (H1_pmt->GetSize() - 2); ipmt++) {
	cout << H1_adctdc_diff_time_peak[ipmt]+halfWindow ;
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
      halfWindow = -1 * halfWindow;
    }
  }
  f2->cd();
  currentCanvas->Write();
  f2->Close();
}
