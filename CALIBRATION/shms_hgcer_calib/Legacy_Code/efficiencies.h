//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Thu Jul 20 10:25:29 2017 by ROOT version 5.34/13
// from TTree T/Hall A Analyzer Output DST
// found on file: ../../ROOTfiles/shms_replay_488_-1.root
//////////////////////////////////////////////////////////

#ifndef efficiencies_h
#define efficiencies_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include <TSelector.h>
#include <TTreeReader.h>
#include <TTreeReaderValue.h>
#include <TTreeReaderArray.h>
#include <TH1.h>
#include <TH2.h>

const Int_t hgc_pmts = 4;

// Header file for the classes stored in the TTree if any.

// Fixed size dimensions of array or collections stored in the TTree if any.

class efficiencies : public TSelector {
public :
   TTreeReader    fReader;
   TTree          *fChain = 0;   //!pointer to the analyzed TTree or TChain
   Bool_t          fShowall;
   Bool_t          fChercut;
   Bool_t          fNGC;
   Float_t         fHGC_cut;
   Float_t         fNGC_cut;

   // Declaration of histograms
   TH1F           **fNPE_eNoDet;
   TH1F           **fNPE_eDet;
   TH1F           *fNPE_Full_eNoDet;
   TH1F           *fNPE_Full_eDet;
   TH1F           **fNPE_piNoDet;
   TH1F           **fNPE_piDet;
   TH1F           *fNPE_Full_piNoDet;
   TH1F           *fNPE_Full_piDet;
   TH1F           *fBeta_Cut;
   TH1F           *fBeta_Full;
   TH1F           *fTiming_Cut;
   TH1F           *fTiming_Full;
   TH2F           *fFly_Pr_Full;
   TH2F           *fFly_Pr_eCut;
   TH2F           *fFly_Pr_piCut;

   // Readers to access the data
   TTreeReaderValue<Int_t>    Ndata_P_tr_beta            = {fReader, "Ndata.P.tr.beta"};
   TTreeReaderArray<Double_t> P_tr_beta                  = {fReader, "P.tr.beta"};
   TTreeReaderArray<Double_t> P_hgcer_goodAdcTdcDiffTime = {fReader, "P.hgcer.goodAdcTdcDiffTime"};
   TTreeReaderArray<Double_t> P_hgcer_goodAdcPulseInt    = {fReader, "P.hgcer.goodAdcPulseInt"};
   TTreeReaderArray<Double_t> P_hgcer_goodAdcPulseAmp    = {fReader, "P.hgcer.goodAdcPulseAmp"};
   TTreeReaderArray<Double_t> P_hgcer_numTracksFired     = {fReader, "P.hgcer.numTracksFired"};
   TTreeReaderArray<Double_t> P_hgcer_npe                = {fReader, "P.hgcer.npe"};
   TTreeReaderValue<Double_t> P_cal_fly_earray           = {fReader, "P.cal.fly.earray"};
   TTreeReaderValue<Double_t> P_cal_pr_eplane            = {fReader, "P.cal.pr.eplane"};
   TTreeReaderValue<Double_t> P_cal_etotnorm             = {fReader, "P.cal.etotnorm"};
   TTreeReaderArray<Double_t> P_gtr_dp                   = {fReader, "P.gtr.dp"};
   TTreeReaderArray<Double_t> P_tr_x                     = {fReader, "P.tr.x"};
   TTreeReaderArray<Double_t> P_tr_ph                    = {fReader, "P.tr.ph"};
   TTreeReaderArray<Double_t> P_tr_y                     = {fReader, "P.tr.y"};
   TTreeReaderArray<Double_t> P_tr_th                    = {fReader, "P.tr.th"};

 efficiencies(TTree * /*tree*/ =0) : fChain(0) {fNPE_eNoDet = 0, fNPE_eDet = 0, fNPE_piNoDet = 0, fNPE_piDet = 0, fNPE_Full_eNoDet = 0, fNPE_Full_eDet = 0, fNPE_Full_piNoDet = 0, fNPE_Full_piDet = 0, fBeta_Cut = 0, fBeta_Full = 0, fTiming_Cut = 0, fTiming_Full = 0, fFly_Pr_Full = 0, fFly_Pr_eCut = 0, fFly_Pr_piCut = 0, fShowall = kFALSE, fChercut = kFALSE, fNGC = kFALSE, fHGC_cut = 2.0, fNGC_cut = 2.0;}
   virtual ~efficiencies() { }
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

   ClassDef(efficiencies,0);
};

#endif

#ifdef efficiencies_cxx
void efficiencies::Init(TTree *tree)
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

Bool_t efficiencies::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

#endif // #ifdef efficiencies_cxx
