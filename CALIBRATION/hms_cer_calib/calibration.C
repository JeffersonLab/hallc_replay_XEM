#define calibration_cxx
// The class definition in calibration.h has been generated automatically
// by the ROOT utility TTree::MakeSelector(). This class is derived
// from the ROOT class TSelector. For more information on the TSelector
// framework see $ROOTSYS/README/README.SELECTOR or the ROOT User Manual.


// The following methods are defined in this file:
//    Begin():        called every time a loop on the tree starts,
//                    a convenient place to create your histograms.
//    SlaveBegin():   called after Begin(), when on PROOF called only on the
//                    slave servers.
//    Process():      called for each event, in this function you decide what
//                    to read and fill your histograms.
//    SlaveTerminate: called at the end of the loop on the tree, when on PROOF
//                    called only on the slave servers.
//    Terminate():    called at the end of the loop on the tree,
//                    a convenient place to draw/fit your histograms.
//
// To use this file, try the following session on your Tree T:
//
// root> T->Process("calibration.C")
// root> T->Process("calibration.C","some options")
// root> T->Process("calibration.C+")
//


#include "calibration.h"
#include <TF1.h>
#include <TH1.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <TSpectrum.h>
#include <TPolyMarker.h>
#include <TPaveStats.h>

void calibration::Begin(TTree * /*tree*/)
{
   // The Begin() function is called at the start of the query.
   // When running with PROOF Begin() is only called on the client.
   // The tree argument is deprecated (on PROOF 0 is passed).

   TString option = GetOption();
   Info("Begin", "Starting calibration process with option: %s", option.Data());
   Info("Begin", "To see details of calibration, use option showall");

   if (option.Contains("showall")) fFullShow = kTRUE;
}

void calibration::SlaveBegin(TTree * /*tree*/)
{
  // The SlaveBegin() function is called after the Begin() function.
  // When running with PROOF SlaveBegin() is called on each slave server.
  // The tree argument is deprecated (on PROOF 0 is passed).

  TString option = GetOption();
  
  fPulseInt = new TH1F*[2];
  for (Int_t ipmt = 0; ipmt < 2; ipmt++) {
    fPulseInt[ipmt] = new TH1F(Form("PulseInt%d",ipmt+1), Form("Pulse integral PMT %d;ADC Channel (pC);Counts",ipmt+1), 1000, 0, 100);
    GetOutputList()->Add(fPulseInt[ipmt]);
  }
  
  fPulseInt_quad = new TH1F**[2];
  fPulseInt_quad[0] = new TH1F*[4];
  fPulseInt_quad[1] = new TH1F*[4];
  for (Int_t ipmt = 0; ipmt < 2; ipmt++) {
    for (Int_t iquad = 0; iquad < 4; iquad++) {
      fPulseInt_quad[ipmt][iquad] = new TH1F(Form("PulseInt_quad%d_PMT%d",iquad+1,ipmt+1),Form("Pulse Integral PMT%d quad%d; ADC Channel (pC); Counts",ipmt+1,iquad+1),1000,0,100);
      GetOutputList()->Add(fPulseInt_quad[ipmt][iquad]);
    }
  }
  
  //Timing and Beta cut visualizations
  fBeta_Cut = new TH1F("Beta_Cut", "Beta cut used for 'good' hits;Beta;Counts", 1000, -5, 5);
  GetOutputList()->Add(fBeta_Cut);
  fBeta_Full = new TH1F("Beta_Full", "Full beta for events;Beta;Counts", 1000, -5, 5);
  GetOutputList()->Add(fBeta_Full);

  fTiming_Cut = new TH1F*[2];
  fTiming_Full = new TH1F*[2];
  for (Int_t ipmt = 0; ipmt < 2; ipmt++) {
    fTiming_Cut[ipmt] = new TH1F(Form("Timing_Cut%d",ipmt+1), Form("Timing cut used for 'good' hits in PMT %d;Time (ns);Counts",ipmt+1), 10000, 50, 150);
    GetOutputList()->Add(fTiming_Cut[ipmt]);
    fTiming_Full[ipmt] = new TH1F(Form("Timing_Full%d",ipmt+1), Form("Full timing information for events in PMT %d;Time (ns);Counts",ipmt+1), 10000, 50, 150);
    GetOutputList()->Add(fTiming_Full[ipmt]);
  }

  //Particle ID cut visualization
  fCut_everything = new TH1F("Cut_everything", "Visualization of no cuts; Normalized Calorimeter Energy (GeV); Counts", 200, 0, 2.0);
  GetOutputList()->Add(fCut_everything);
  fCut_electron = new TH1F("Cut_electron", "Visualization of pion cut; Normalized Calorimeter Energy (GeV); Counts", 200, 0, 2.0);
  GetOutputList()->Add(fCut_electron);
}

Bool_t calibration::Process(Long64_t entry)
{
   // The Process() function is called for each entry in the tree (or possibly
   // keyed object in the case of PROOF) to be processed. The entry argument
   // specifies which entry in the currently loaded tree is to be processed.
   // When processing keyed objects with PROOF, the object is already loaded
   // and is available via the fObject pointer.
   //
   // This function should contain the \"body\" of the analysis. It can contain
   // simple or elaborate selection criteria, run algorithms on the data
   // of the event and typically fill histograms.
   //
   // The processing can be stopped by calling Abort().
   //
   // Use fStatus to set the return value of TTree::Process().
   //
   // The return value is currently not used.

   fReader.SetEntry(entry);
   
   //Only one track
   if (*Ndata_H_tr_beta != 1) return kTRUE;

   for (Int_t itrack = 0; itrack < *Ndata_H_tr_beta; itrack++) {
     //Beta Cut
     fBeta_Full->Fill(H_tr_beta[itrack]);
     if (TMath::Abs(H_tr_beta[itrack] -1.0) > 0.2) return kTRUE;
     fBeta_Cut->Fill(H_tr_beta[itrack]);

     for (Int_t ipmt = 0; ipmt < 2; ipmt++) {
       //Timing Cut
       fTiming_Full[ipmt]->Fill(H_cer_goodAdcTdcDiffTime[ipmt]);
       if (H_cer_goodAdcTdcDiffTime[ipmt] > 110 || H_cer_goodAdcTdcDiffTime[ipmt] < 90) continue;
       fTiming_Cut[ipmt]->Fill(H_cer_goodAdcTdcDiffTime[ipmt]);

       if (ipmt == 0) fCut_everything->Fill(*H_cal_etotnorm);
       //Electron Cut
       if (*H_cal_etotnorm < 0.5) {
	 if (ipmt == 0) fCut_electron->Fill(*H_cal_etotnorm);
	 fPulseInt[ipmt]->Fill(H_cer_goodAdcPulseInt[ipmt]);

	 //Quadrant Cuts
	 if (*H_cer_xAtCer >= 0.0 && *H_cer_yAtCer >= 0.0) {
	   fPulseInt_quad[ipmt][0]->Fill(H_cer_goodAdcPulseInt[ipmt]);
	 }
	 if (*H_cer_xAtCer >= 0.0 && *H_cer_yAtCer < 0.0) {
	   fPulseInt_quad[ipmt][1]->Fill(H_cer_goodAdcPulseInt[ipmt]);
	 }
	 if (*H_cer_xAtCer < 0.0 && *H_cer_yAtCer >= 0.0) {
	   fPulseInt_quad[ipmt][2]->Fill(H_cer_goodAdcPulseInt[ipmt]);
	 }
	 if (*H_cer_xAtCer < 0.0 && *H_cer_yAtCer < 0.0) {
	   fPulseInt_quad[ipmt][3]->Fill(H_cer_goodAdcPulseInt[ipmt]);
	 }
       }//End of electron cut
     }//End of PMT loop
   }//End of tracking loop
   
   return kTRUE;
}

void calibration::SlaveTerminate()
{
   // The SlaveTerminate() function is called after all entries or objects
   // have been processed. When running with PROOF SlaveTerminate() is called
   // on each slave server.

}

void calibration::Terminate()
{
   // The Terminate() function is the last function to be called during
   // a query. It always runs on the client, it can be used to present
   // the results graphically or save the results to file.

  Info("Terminate", "'%s' showing", (fFullShow ? "full" : "minimal"));
  Info("Terminate", "Histograms formed, now starting calibration.\n'Peak Buffer full' is a good warning!\n");
  printf("\n");

  //Have to extract the histograms from the OutputList 
  TH1F* PulseInt[2];
  TH1F* PulseInt_quad[2][4];
  TH1F* Timing_Cut[2];
  TH1F* Timing_Full[2];
  for (Int_t ipmt = 0; ipmt < 2; ipmt++) {
    Timing_Cut[ipmt] = dynamic_cast<TH1F*> (GetOutputList()->FindObject(Form("Timing_Cut%d",ipmt+1)));
    Timing_Full[ipmt] = dynamic_cast<TH1F*> (GetOutputList()->FindObject(Form("Timing_Full%d",ipmt+1)));
    PulseInt[ipmt] = dynamic_cast<TH1F*> (GetOutputList()->FindObject(Form("PulseInt%d",ipmt+1)));
    for (Int_t iquad = 0; iquad < 4; iquad++) {
      PulseInt_quad[ipmt][iquad] = dynamic_cast<TH1F*> (GetOutputList()->FindObject(Form("PulseInt_quad%d_PMT%d",iquad+1,ipmt+1)));
    }
  }

  Double_t Cer_Peak[2];
  //Begin peak Finding
  for (Int_t ipmt = 0; ipmt < 2; ipmt++) {
    PulseInt[ipmt]->GetXaxis()->SetRangeUser(0,8);
    TSpectrum *s = new TSpectrum(1);
    s->Search(PulseInt[ipmt], 1.0, "nobackground&&nodraw", 0.001);
    TList *functions = PulseInt[ipmt]->GetListOfFunctions();
    TPolyMarker *pm = (TPolyMarker*)functions->FindObject("TPolyMarker");
    Cer_Peak[ipmt] = *pm->GetX();
    PulseInt[ipmt]->GetXaxis()->SetRangeUser(0,100);
  }

  //Begin Fitting of SPE
  /*TF1 *poisson =  new TF1("poisson","[1]*((pow([0],x)*exp(-[0]))/(tgamma(x+1)))",0,20);
  poisson->SetParName(0,"#mu");
  poisson->SetParName(1,"Amplitude");
  poisson->SetParameters(5.0,100);
  poisson->SetParLimits(0,0.0,10.0);
  poisson->SetParLimits(1,0.0,9999.0);
  poisson->SetRange(0,Cer_Peak[0]+2); PulseInt[0]->Fit("poisson","RQM");
  TF1 *PulseIntPMT1Fit = PulseInt[0]->GetFunction("poisson");
  poisson->SetRange(0,Cer_Peak[1]+2); PulseInt[1]->Fit("poisson","RQM");
  TF1 *PulseIntPMT2Fit = PulseInt[1]->GetFunction("poisson");*/
 
  TF1 *gaussian =  new TF1("gaussian","[1]*exp(-0.5*((x-[0])/[2])*((x-[0])/[2]))",0,20);
  gaussian->SetParName(0,"#mu");
  gaussian->SetParName(1,"Amplitude");
  gaussian->SetParName(2,"#sigma");
  gaussian->SetParameters(5.0,100,1.0);
  gaussian->SetParLimits(0,0.0,10.0);
  gaussian->SetParLimits(1,0.0,9999.0);
  gaussian->SetParLimits(2,0.0,10.0);
  gaussian->SetRange(0,Cer_Peak[0]+2); PulseInt[0]->Fit("gaussian","RQM0");
  TF1 *PulseIntPMT1Fit = PulseInt[0]->GetFunction("gaussian");
  gaussian->SetRange(0,Cer_Peak[1]+2); PulseInt[1]->Fit("gaussian","RQM0");
  TF1 *PulseIntPMT2Fit = PulseInt[1]->GetFunction("gaussian");
  
  if (fFullShow) {
    TCanvas *Beta = new TCanvas("Beta","Beta cuts used");
    Beta->Divide(2,1);
    Beta->cd(1); fBeta_Full->Draw();
    Beta->cd(2); fBeta_Cut->Draw();
   
    TCanvas *Cal = new TCanvas("Cal","Calorimeter cuts used");
    Cal->Divide(2,1);
    Cal->cd(1); fCut_everything->Draw();
    Cal->cd(2); fCut_electron->Draw();

    TCanvas *Timing = new TCanvas("Timing","Timing cuts used");
    Timing->Divide(2,2);
    Timing->cd(1); Timing_Full[0]->Draw();
    Timing->cd(2); Timing_Cut[0]->Draw();
    Timing->cd(3); Timing_Full[1]->Draw();
    Timing->cd(4); Timing_Cut[1]->Draw();
    /*
    TCanvas *PulseIntPMT1 = new TCanvas("PulseIntPMT1","Good Pulse Integral from Cherenkov PMT 1");
    PulseIntPMT1->Divide(2,2);
    PulseIntPMT1->cd(1); PulseInt_quad[0][0]->Draw();
    PulseIntPMT1->cd(2); PulseInt_quad[0][1]->Draw();
    PulseIntPMT1->cd(3); PulseInt_quad[0][2]->Draw();
    PulseIntPMT1->cd(4); PulseInt_quad[0][3]->Draw();

    TCanvas *PulseIntPMT2 = new TCanvas("PulseIntPMT2","Good Pulse Integral from Cherenkov PMT 2");
    PulseIntPMT2->Divide(2,2);
    PulseIntPMT2->cd(1); PulseInt_quad[1][0]->Draw();
    PulseIntPMT2->cd(2); PulseInt_quad[1][1]->Draw();
    PulseIntPMT2->cd(3); PulseInt_quad[1][2]->Draw();
    PulseIntPMT2->cd(4); PulseInt_quad[1][3]->Draw();
    */
    gStyle->SetOptStat(111);
    gStyle->SetOptFit(0111);
    TCanvas *cPulseInt = new TCanvas("cPulseInt","Good Pulse Integral in both Cherenkov PMTs");
    cPulseInt->Divide(2,1);
    cPulseInt->cd(1); PulseInt[0]->Draw(); PulseIntPMT1Fit->Draw("same");
    cPulseInt->cd(2); PulseInt[1]->Draw(); PulseIntPMT2Fit->Draw("same");
  }

  //Output the actual calibration information
  cout << "Calibration constants are \nPMT#: ADC-to-NPE Value\n" << endl;
  cout << "PMT 1:" << setw(8) << Form("%3.3f\n",PulseIntPMT1Fit->GetParameter(0));
  cout << "PMT 2:" << setw(8) << Form("%3.3f\n",PulseIntPMT2Fit->GetParameter(0));

  //Start the process of writing the calibration information to file
  ofstream calibration;
  calibration.open("calibration_temp.txt", ios::out);

  if (!calibration.is_open()) cout << "Problem saving calibration constants, may have to update constants manually!" << endl;

  else {
    calibration << "hcer_adc_to_npe = "; 
    calibration << Form("1./%3.3f, 1./%3.3f", PulseIntPMT1Fit->GetParameter(0), PulseIntPMT2Fit->GetParameter(0));
    calibration.close();
  }
}
