//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Fri Mar  2 15:04:26 2018 by ROOT version 6.10/02
// from TTree T/Hall A Analyzer Output DST
// found on file: /home/rambrose/hallc_replay/ROOTfiles/shms_replay_production_all_2732_-1.root
//////////////////////////////////////////////////////////

#ifndef preprocess_h
#define preprocess_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include <TSelector.h>
#include <TTreeReader.h>
#include <TTreeReaderValue.h>
#include <TTreeReaderArray.h>

// Headers needed by this particular selector


class preprocess : public TSelector {
public :
   TTreeReader     fReader;  //!the tree reader
   TTree          *fChain = 0;   //!pointer to the analyzed TTree or TChain

   Bool_t          fNGC;
   Bool_t          fFullShow;

   // Declaration of Histograms
   TH1F          **fTiming;

   // Declaration of Canvases
   TCanvas        *cTiming;

   // Readers to access the data (delete the ones you do not need).
   TTreeReaderArray<Double_t> P_hgcer_goodAdcTdcDiffTime = {fReader, "P.hgcer.goodAdcTdcDiffTime"};
   TTreeReaderArray<Int_t> Ndata_P_tr_p = {fReader, "Ndata.P.tr.p"};
   TTreeReaderArray<Double_t> P_tr_beta = {fReader, "P.tr.beta"};
   

   preprocess(TTree * /*tree*/ =0) {fNGC = kFALSE, fFullShow = kFALSE; }
   virtual ~preprocess() { }
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

   ClassDef(preprocess,0);

};

#endif

#ifdef preprocess_cxx
void preprocess::Init(TTree *tree)
{
   // The Init() function is called when the selector needs to initialize
   // a new tree or chain. Typically here the reader is initialized.
   // It is normally not necessary to make changes to the generated
   // code, but the routine can be extended by the user if needed.
   // Init() will be called many times when running on PROOF
   // (once per file to be processed).

   fReader.SetTree(tree);
}

Bool_t preprocess::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

//Gaussian distribution is used to find the mean of the SPE and determine spacing between subsequent NPE
Double_t pregauss(Double_t *x, Double_t *par)
{
  Double_t result1 = par[0]*exp((-0.5)*(pow((x[0] - par[1]),2)/pow((par[2]),2)));
  Double_t result2 = par[3]*exp((-0.5)*(pow((x[0] - par[4]),2)/pow((par[5]),2)));
  Double_t result3 = par[6]*exp((-0.5)*(pow((x[0] - par[7]),2)/pow((par[8]),2)));
  Double_t result4 = par[9]*exp((-0.5)*(pow((x[0] - par[10]),2)/pow((par[11]),2)));
  Double_t result5 = par[12]*exp((-0.5)*(pow((x[0] - par[13]),2)/pow((par[14]),2)));
  return result1 + result2 + result3 + result4 + result5;
}

#endif // #ifdef preprocess_cxx
