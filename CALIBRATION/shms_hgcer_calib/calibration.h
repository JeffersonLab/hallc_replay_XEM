//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Tue Jul  4 10:23:34 2017 by ROOT version 5.34/13
// from TTree T/Hall A Analyzer Output DST
// found on file: shms_replay_488.root
//////////////////////////////////////////////////////////

#ifndef calibration_h
#define calibration_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include <TSelector.h>
#include <TTreeReader.h>
#include <TTreeReaderValue.h>
#include <TTreeReaderArray.h>
#include <TH1.h>
#include <TH2.h>
#include <TNtuple.h>

const Int_t        fhgc_pmts = 4;
const Double_t     fhgc_zpos = 156.27;

// Header file for the classes stored in the TTree if any.

// Fixed size dimensions of array or collections stored in the TTree if any.

class calibration : public TSelector {
public :
   TTreeReader    fReader;
   TTree          *fChain = 0;   //!pointer to the analyzed TTree or TChain
   Bool_t          fFullRead;
   Bool_t          fFullShow;
   Bool_t          fTrack;
   Bool_t          fCut;
   Bool_t          fPions;

   // Declaration of histograms
   TH1F          **fPulseInt;
   TH1F         ***fPulseInt_quad;
   TH2F           *fCut_everything;
   TH1F           *fCut_enorm;
   TH2F           *fCut_electron;
   TH2F           *fCut_pion;
   TH1F           *fBeta_Cut;
   TH1F           *fBeta_Full;
   TH1F           *fTiming_Cut;
   TH1F           *fTiming_Full;

   // Declaration of histograms used in fitting/analysis
   TH1F *fscaled[4];
   TH1F *fscaled_nobackground[4];
   TH1F *fscaled_mk2[4];
   TH1F *fscaled_mk2_nobackground[4];
   TH1F *fscaled_temp[4];
   TH1F *fscaled_combined[4];
   TH1F *fscaled_total;
   TH1F *fscaled_temp_mk2[4];
   TH1F *fscaled_combined_mk2[4];
   TH1F *fscaled_total_mk2;

   // Declaration of canvases used in fitting/analysis
   TCanvas *quad_cuts_ipmt;
   TCanvas *low_stats_ipmt;
   TCanvas *background_ipmt;
   TCanvas *final_spectra_ipmt;
   TCanvas *background_mk2_ipmt;
   TCanvas *final_spectra_mk2_ipmt;
   TCanvas *final_spectra_combined;
   TCanvas *final_spectra_combined_mk2;
   TCanvas *scaled_poisson;
   TCanvas *scaled_total;   

   // Declaration of preprocessing quantities
   Double_t  timing_mean[4];
   Double_t  timing_std[4];

   // Readers to access the data
   TTreeReaderValue<Int_t>    Ndata_P_tr_beta            = {fReader, "Ndata.P.tr.beta"};
   TTreeReaderArray<Double_t> P_tr_beta                  = {fReader, "P.tr.beta"};
   TTreeReaderArray<Double_t> P_hgcer_goodAdcTdcDiffTime = {fReader, "P.hgcer.goodAdcTdcDiffTime"};
   TTreeReaderArray<Double_t> P_hgcer_goodAdcPulseInt    = {fReader, "P.hgcer.goodAdcPulseInt"};
   TTreeReaderArray<Double_t> P_hgcer_goodAdcPulseAmp    = {fReader, "P.hgcer.goodAdcPulseAmp"};
   TTreeReaderArray<Double_t> P_hgcer_numTracksFired     = {fReader, "P.hgcer.numTracksFired"};
   TTreeReaderValue<Double_t> P_cal_fly_earray           = {fReader, "P.cal.fly.earray"};
   TTreeReaderValue<Double_t> P_cal_pr_eplane            = {fReader, "P.cal.pr.eplane"};
   TTreeReaderValue<Double_t> P_cal_etotnorm             = {fReader, "P.cal.etotnorm"};
   TTreeReaderArray<Double_t> P_gtr_dp                   = {fReader, "P.gtr.dp"};
   TTreeReaderArray<Double_t> P_tr_x                     = {fReader, "P.tr.x"};
   TTreeReaderArray<Double_t> P_tr_ph                    = {fReader, "P.tr.ph"};
   TTreeReaderArray<Double_t> P_tr_y                     = {fReader, "P.tr.y"};
   TTreeReaderArray<Double_t> P_tr_th                    = {fReader, "P.tr.th"};
  
 calibration(TTree * /*tree*/ =0) : fChain(0) {fPulseInt = 0, fPulseInt_quad = 0, fCut_everything = 0, fCut_enorm=0, fCut_electron = 0, fCut_pion = 0, fBeta_Cut = 0, fBeta_Full = 0, fTiming_Cut = 0, fTiming_Full = 0, fFullRead = kFALSE, fFullShow = kFALSE, fTrack = kFALSE, fCut = kFALSE, fPions = kFALSE;}
   virtual ~calibration() { }
   virtual Int_t   Version() const { return 2; }
   virtual void    Begin(TTree *tree);
   virtual void    SlaveBegin(TTree *tree);
   virtual void    Init(TTree *tree);
   virtual Bool_t  Notify();
   virtual Bool_t  Process(Long64_t entry);
   virtual Int_t   GetEntry(Long64_t entry, Int_t getall = 0) { return fChain ? fChain->GetTree()->GetEntry(entry, getall) : 0; }
   virtual void    SetOption(const char *option) { fOption = option; }
   virtual void    SetObject(TObject *obj) { fObject = obj; }
   virtual void    SetInputList(TList *input) { fInput = input; }
   virtual TList  *GetOutputList() const { return fOutput; }
   virtual void    SlaveTerminate();
   virtual void    Terminate();

   ClassDef(calibration,0);
};

#endif

#ifdef calibration_cxx
void calibration::Init(TTree *tree)
{
   // The Init() function is called when the selector needs to initialize
   // a new tree or chain. Typically here the branch addresses and branch
   // pointers of the tree will be set.
   // It is normally not necessary to make changes to the generated
   // code, but the routine can be extended by the user if needed.
   // Init() will be called many times when running on PROOF
   // (once per file to be processed).

   // Set branch addresses and branch pointers

  fReader.SetTree(tree);
   
}

Bool_t calibration::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}


//Poisson distribution is used to remove background from larger values of NPE
Double_t poisson(Double_t *x, Double_t *par)
{
  Double_t result1 = (par[1]*pow(par[0],x[0])*exp(-par[0]))/(tgamma(x[0]+1));
  return result1;
}
//Gaussian distribution is used to find the mean of the SPE and determine spacing between subsequent NPE
Double_t gauss(Double_t *x, Double_t *par)
{
  Double_t result1 = par[0]*exp((-0.5)*(pow((x[0] - par[1]),2)/pow((par[2]),2)));
  Double_t result2 = par[3]*exp((-0.5)*(pow((x[0] - par[4]),2)/pow((par[5]),2)));
  Double_t result3 = par[6]*exp((-0.5)*(pow((x[0] - par[7]),2)/pow((par[8]),2)));
  Double_t result4 = par[9]*exp((-0.5)*(pow((x[0] - par[10]),2)/pow((par[11]),2)));
  Double_t result5 = par[12]*exp((-0.5)*(pow((x[0] - par[13]),2)/pow((par[14]),2)));
  return result1 + result2 + result3 + result4 + result5;
}

//A simple linear equation is used to determine how linear the means of the NPE are
Double_t linear(Double_t *x, Double_t *par)
{
  Double_t result1 = par[0]*x[0] + par[1];
  return result1;
}

#endif // #ifdef calibration_cxx
