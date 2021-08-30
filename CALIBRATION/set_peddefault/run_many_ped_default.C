#include "TFile.h"
#include "TH1D.h"
#include <iostream>
void calc_ped_default(vector<TH1I*>, TString , TString ,TString, Double_t);
void run_shms_ped_default(TString);
void run_hms_ped_default(TString);
void makeFile(TString spec);


int nPMT_pHgcer = 4,   nPMT_pNgcer   = 4, 
  nPMT_pPosAero = 7,   nPMT_pNegAero = 7,
  nPMT_pPosShwr = 14,  nPMT_pNegShwr = 14, 
  nPMT_pArrShwr = 224;

int nPMT_hCer = 2, 
  nPMT_hCalPos1 = 13, nPMT_hCalPos2 = 13, 
  nPMT_hCalPos3 = 13, nPMT_hCalPos4 = 13,
  nPMT_hCalNeg1 = 13, nPMT_hCalNeg2 = 13;

vector<TH1I*> p_hgcerHisto(nPMT_pHgcer);
vector<TH1I*> p_ngcerHisto(nPMT_pNgcer);
vector<TH1I*> p_posAeroHisto(nPMT_pPosAero);
vector<TH1I*> p_negAeroHisto(nPMT_pNegAero);
vector<TH1I*> p_posShwrHisto(nPMT_pPosShwr);
vector<TH1I*> p_negShwrHisto(nPMT_pNegShwr);
vector<TH1I*> p_arrShwrHisto(nPMT_pArrShwr);

vector<TH1I*> h_cerHisto(nPMT_hCer);
vector<TH1I*> h_calPos1Histo(nPMT_hCalPos1);
vector<TH1I*> h_calPos2Histo(nPMT_hCalPos2);
vector<TH1I*> h_calPos3Histo(nPMT_hCalPos3);
vector<TH1I*> h_calPos4Histo(nPMT_hCalPos4);
vector<TH1I*> h_calNeg1Histo(nPMT_hCalNeg1);
vector<TH1I*> h_calNeg2Histo(nPMT_hCalNeg2);


void run_many_ped(TString runFile, TString spec) {

  //Using f2-emc
  TString shmsReplayLoc = "ROOTfiles/TIMING/shms_replay_production_%i_-1.root";
  TString  hmsReplayLoc = "ROOTfiles/TIMING/hms_setPedDefault_%i_-1.root";

  //Using hallc_replay
  //TString shmsReplayLoc = "/volatile/hallc/xem2/cmorean/ROOTfiles/hallc_replayDefaultPed/shms_pedDefault_%i_-1.root";
  //TString  hmsReplayLoc = "/volatile/hallc/xem2/cmorean/ROOTfiles/hallc_replayDefaultPed/hms_defaultPed_%i_-1.root";

  ifstream runList(runFile);  //Should Error check
  string run;
  int currentRun;
  TFile *outFile;

  int pedestalBins = 500,
    pedestalMin = 1500, 
    pedestalMax = 3500;

  //Define each histogram in each vector
  for (int i = 0; i < nPMT_pHgcer; i++) {
    p_hgcerHisto[i] = new TH1I(Form("p_hgcerHisto_%i",i),
      Form("p_hgcerHisto_%i",i),pedestalBins,pedestalMin,pedestalMax);
  }
  for (int i = 0; i < nPMT_pNgcer; i++) {
    p_ngcerHisto[i] = new TH1I(Form("p_ngcerHisto_%i",i),
      Form("p_ngcerHisto_%i",i),pedestalBins,pedestalMin,pedestalMax);
  }
  for (int i = 0; i < nPMT_pPosAero; i++) {
    p_posAeroHisto[i] = new TH1I(Form("p_posAeroHisto_%i",i),
      Form("p_posAeroHisto_%i",i),pedestalBins,pedestalMin,pedestalMax);
  }
  for (int i = 0; i < nPMT_pNegAero; i++) {
    p_negAeroHisto[i] = new TH1I(Form("p_negAeroHisto_%i",i),
      Form("p_negAeroHisto_%i",i),pedestalBins,pedestalMin,pedestalMax);
  }
  for (int i = 0; i < nPMT_pPosShwr; i++) {
    p_posShwrHisto[i] = new TH1I(Form("p_posShwrHisto_%i",i),
      Form("p_posShwrHisto_%i",i),pedestalBins,pedestalMin,pedestalMax);
  }
  for (int i = 0; i < nPMT_pNegShwr; i++) {
    p_negShwrHisto[i] = new TH1I(Form("p_negShwrHisto_%i",i),
      Form("p_negShwrHisto_%i",i),pedestalBins,pedestalMin,pedestalMax);
  }
  for (int i = 0; i < nPMT_pArrShwr; i++) {
    p_arrShwrHisto[i] = new TH1I(Form("p_arrShwrHisto_%i",i),
      Form("p_arrShwrHisto_%i",i),pedestalBins,pedestalMin,pedestalMax);
  }

  for (int i = 0; i < nPMT_hCer; i++) {
    h_cerHisto[i] = new TH1I(Form("h_cerHisto_%i",i),
      Form("h_cerHisto_%i",i),pedestalBins,pedestalMin,pedestalMax);
  }
  for (int i = 0; i < nPMT_hCalPos1; i++) {
    h_calPos1Histo[i] = new TH1I(Form("h_calPos1Histo_%i",i),
      Form("h_calPos1Histo_%i",i),pedestalBins,pedestalMin,pedestalMax);
  }
 for (int i = 0; i < nPMT_hCalPos2; i++) {
    h_calPos2Histo[i] = new TH1I(Form("h_calPos2Histo_%i",i),
      Form("h_calPos2Histo_%i",i),pedestalBins,pedestalMin,pedestalMax);
  }
  for (int i = 0; i < nPMT_hCalPos3; i++) {
    h_calPos3Histo[i] = new TH1I(Form("h_calPos3Histo_%i",i),
      Form("h_calPos3Histo_%i",i),pedestalBins,pedestalMin,pedestalMax);
  }
  for (int i = 0; i < nPMT_hCalPos4; i++) {
    h_calPos4Histo[i] = new TH1I(Form("h_calPos4Histo_%i",i),
      Form("h_calPos4Histo_%i",i),pedestalBins,pedestalMin,pedestalMax);
  }
  for (int i = 0; i < nPMT_hCalNeg1; i++) {
    h_calNeg1Histo[i] = new TH1I(Form("h_calNeg1Histo_%i",i),
      Form("h_calNeg1Histo_%i",i),pedestalBins,pedestalMin,pedestalMax);
  }
  for (int i = 0; i < nPMT_hCalNeg2; i++) {
    h_calNeg2Histo[i] = new TH1I(Form("h_calNeg2Histo_%i",i),
      Form("h_calNeg2Histo_%i",i),pedestalBins,pedestalMin,pedestalMax);
  }

  //=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=
  //                  MAIN Loop Over runs from runfile
  //=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=
  spec.ToLower();
  if(spec == "shms") {
    cout << "Running runlist: " << runFile << " for SHMS\n";
    while(getline(runList, run)) {
      istringstream ss(run);
      ss >> currentRun;
      cout << "\nDetermining default pedestals for run: " << currentRun << endl;
      run_shms_ped_default(Form(shmsReplayLoc,currentRun));
    }
  }
  else if(spec == "hms") {
    cout << "Running runlist: " << runFile << " for HMS\n";
    while(getline(runList, run)) {
      istringstream ss(run);
      ss >> currentRun;
      cout << "\nDetermining default pedestals for run: " << currentRun << endl;
      run_hms_ped_default(Form(hmsReplayLoc,currentRun));
    }
  }
  else {
    cout << "No Matching Spectrometer: " << spec << endl;
    return;
  }


  //Output the histograms to file and find the MEAN and output to screen.

  cout << "The Mean Value of each histogram for each PMT is:\n";

  if(spec == "shms") {
    outFile  = new TFile("AllDefaultPedsSHMS.root","RECREATE");
    outFile->cd();
    cout << "\nphgcer_PedDefault= ";
    for (int i = 0; i < nPMT_pHgcer; i++) {
      cout << (int) p_hgcerHisto[i]->GetMean();
      if (i < nPMT_pHgcer)  cout << ", " ;
      p_hgcerHisto[i]->Write();
    }
    cout << "\npngcer_PedDefault= ";
    for (int i = 0; i < nPMT_pNgcer; i++) {
      cout << (int) p_ngcerHisto[i]->GetMean();
      if (i < nPMT_pNgcer)  cout << ", " ;
      p_ngcerHisto[i]->Write();
    }
    cout << "\npaero_PedPosDefault=";
    for (int i = 0; i < nPMT_pPosAero; i++) {
      cout << (int) p_posAeroHisto[i]->GetMean();
      if (i < nPMT_pPosAero)  cout << ", " ;
      p_posAeroHisto[i]->Write();
    }
    cout << "\npaero_PedNegDefault=";
    for (int i = 0; i < nPMT_pNegAero; i++) {
      cout << (int) p_negAeroHisto[i]->GetMean();
      if (i < nPMT_pNegAero)  cout << ", " ;
      p_negAeroHisto[i]->Write();
    }
    cout << "\npcal_PedPosDefault= ";
    for (int i = 0; i < nPMT_pPosShwr; i++) {
      cout << (int) p_posShwrHisto[i]->GetMean();
      if (i < nPMT_pPosShwr)  cout << ", " ;
      p_posShwrHisto[i]->Write();
    }
    cout << "\npcal_PedNegDefault= ";
    for (int i = 0; i < nPMT_pNegShwr; i++) {
      cout << (int) p_negShwrHisto[i]->GetMean();
      if (i < nPMT_pNegShwr)  cout << ", " ;
      p_negShwrHisto[i]->Write();
    }
    cout << "\npcal_arr_PedDefault= ";
    for (int i = 0; i < nPMT_pArrShwr; i++) {
      if (i!=0 && i%16 == 0 )  cout << endl;
      cout << (int) p_arrShwrHisto[i]->GetMean();
      if (i < nPMT_pArrShwr - 1)  cout << ", " ;
      p_arrShwrHisto[i]->Write();
    }
  }
  else if(spec == "hms") {
    outFile = new TFile("AllDefaultPedsHMS.root","RECREATE");
    outFile->cd();
    cout << "\nhcer_PedDefault= ";
    for (int i = 0; i < nPMT_hCer; i++) {
      if (i!=0 && i%16 == 0 )  cout << endl;
      cout << (int) h_cerHisto[i]->GetMean();
      if (i < nPMT_hCer)  cout << ", " ;
      h_cerHisto[i]->Write();
    }
    cout << "\nhcal_PedPosDefault= ";
    for (int i = 0; i < nPMT_hCalPos1; i++) {
      if (i!=0 && i%16 == 0 )  cout << endl;
      cout << (int) h_calPos1Histo[i]->GetMean();
      if (i < nPMT_hCalPos1)  cout << ", " ;
      h_calPos1Histo[i]->Write();
    }
    cout << endl; 
    for (int i = 0; i < nPMT_hCalPos2; i++) {
      if (i!=0 && i%16 == 0 )  cout << endl;
      cout << (int) h_calPos2Histo[i]->GetMean();
      if (i < nPMT_hCalPos2)  cout << ", " ;
      h_calPos2Histo[i]->Write();
    }
    cout << endl; 
    for (int i = 0; i < nPMT_hCalPos3; i++) {
      if (i!=0 && i%16 == 0 )  cout << endl;
      cout << (int) h_calPos3Histo[i]->GetMean();
      if (i < nPMT_hCalPos3)  cout << ", " ;
      h_calPos3Histo[i]->Write();
    }
    cout << endl; 
    for (int i = 0; i < nPMT_hCalPos4; i++) {
      if (i!=0 && i%16 == 0 )  cout << endl;
      cout << (int) h_calPos4Histo[i]->GetMean();
      if (i < nPMT_hCalPos4)  cout << ", " ;
      h_calPos4Histo[i]->Write();
    }
    cout << "\n hcal_PedNegDefault= ";
    for (int i = 0; i < nPMT_hCalNeg1; i++) {
      if (i!=0 && i%16 == 0 )  cout << endl;
      cout << (int) h_calNeg1Histo[i]->GetMean();
      if (i < nPMT_hCalNeg1)  cout << ", " ;
      h_calNeg1Histo[i]->Write();
    }
    cout << endl; 
    for (int i = 0; i < nPMT_hCalNeg2; i++) {
      if (i!=0 && i%16 == 0 )  cout << endl;
      cout << (int) h_calNeg2Histo[i]->GetMean();
      if (i < nPMT_hCalNeg2)  cout << ", " ;
      h_calNeg2Histo[i]->Write();
    }
  }
  else 
    return; // Should never get here..

  cout << endl;

  outFile->Write();
  outFile->Close();
  
  return;
}

//
void run_shms_ped_default(TString file_name = "") {
  calc_ped_default(p_hgcerHisto,file_name,"hgcer","p",0);
  calc_ped_default(p_ngcerHisto,file_name,"ngcer","p",0);
  calc_ped_default(p_posAeroHisto,file_name,"aero","p",1);
  calc_ped_default(p_negAeroHisto,file_name,"aero","p",2);
  calc_ped_default(p_posShwrHisto,file_name,"cal_prshwr","p",1);
  calc_ped_default(p_negShwrHisto,file_name,"cal_prshwr","p",2);
  calc_ped_default(p_arrShwrHisto,file_name,"cal_shwr","p",0);
}
//
void run_hms_ped_default(TString file_name = "") {
  calc_ped_default(h_cerHisto,file_name,"cer","h",0);
  calc_ped_default(h_calPos1Histo,file_name,"cal_hA","h",1);
  calc_ped_default(h_calPos2Histo,file_name,"cal_hB","h",1);
  calc_ped_default(h_calPos3Histo,file_name,"cal_hC","h",1);
  calc_ped_default(h_calPos4Histo,file_name,"cal_hD","h",1);
  calc_ped_default(h_calNeg1Histo,file_name,"cal_hA","h",2);
  calc_ped_default(h_calNeg2Histo,file_name,"cal_hB","h",2);

}


void calc_ped_default(vector<TH1I*> allPeaks, TString golden_file = "", TString detector = "",
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

  H1_pmt = (TH1D*)H1_ped_vs_pmt->ProjectionX("H1_pmt", 1, H1_ped_vs_pmt->GetSize() - 2);

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
     (allPeaks[ipmt])->Fill(H1_ped_peak[ipmt]);
     if (ipmt <(H1_pmt->GetSize() - 3))  cout << ", " ;
     if (ipmt!=0 && ipmt%16 == 0 )  cout << endl;
   } 
   cout << endl;
   /*
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
*/
   f1->Close();
}
