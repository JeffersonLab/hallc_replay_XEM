//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Wed Aug 15 17:10:26 2018 by ROOT version 6.10/02
// from TTree T/Hall A Analyzer Output DST
// found on file: ../../ROOTfiles/coin_replay_production_3424_-1.root
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

// Headers needed by this particular selector


class calibration : public TSelector {
public :
   TTreeReader     fReader;  //!the tree reader
   TTree          *fChain = 0;   //!pointer to the analyzed TTree or TChain
   Bool_t          fFullShow;
   

   //Declare histograms
   TH1F          **fPulseInt; 
   TH1F         ***fPulseInt_quad;
   TH1F           *fCut_everything;
   TH1F           *fCut_electron;
   TH1F           *fBeta_Cut;
   TH1F           *fBeta_Full;
   TH1F          **fTiming_Cut;
   TH1F          **fTiming_Full;

   // Readers to access the data (delete the ones you do not need).
   TTreeReaderArray<Double_t> H_cer_goodAdcPulseInt    = {fReader, "H.cer.goodAdcPulseInt"};
   TTreeReaderArray<Double_t> H_cer_goodAdcTdcDiffTime = {fReader, "H.cer.goodAdcTdcDiffTime"};
   TTreeReaderValue<Double_t> H_cer_xAtCer             = {fReader, "H.cer.xAtCer"};
   TTreeReaderValue<Double_t> H_cer_yAtCer             = {fReader, "H.cer.yAtCer"};

   TTreeReaderArray<Double_t> H_tr_beta                = {fReader, "H.tr.beta"};
   TTreeReaderValue<Int_t> Ndata_H_tr_beta             = {fReader, "Ndata.H.tr.beta"};

   TTreeReaderValue<Double_t> H_cal_etotnorm           = {fReader, "H.cal.etotnorm"};


   calibration(TTree * /*tree*/ =0) {fPulseInt=0,fPulseInt_quad=0,fCut_everything=0,fCut_electron=0,fBeta_Cut=0,fBeta_Full=0,fTiming_Cut=0,fTiming_Full=0,fFullShow=kFALSE;}
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
   // a new tree or chain. Typically here the reader is initialized.
   // It is normally not necessary to make changes to the generated
   // code, but the routine can be extended by the user if needed.
   // Init() will be called many times when running on PROOF
   // (once per file to be processed).

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


#endif // #ifdef calibration_cxx
