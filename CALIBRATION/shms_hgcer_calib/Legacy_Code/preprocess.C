#define preprocess_cxx
// The class definition in preprocess.h has been generated automatically
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
// root> T->Process("preprocess.C")
// root> T->Process("preprocess.C","some options")
// root> T->Process("preprocess.C+")
//


#include "preprocess.h"
#include <TH1.h>
#include <TF1.h>
#include <TCanvas.h>
#include <TSpectrum.h>
#include <TPolyMarker.h>
#include <TStyle.h>
#include <TNtuple.h>

void preprocess::Begin(TTree * /*tree*/)
{
   // The Begin() function is called at the start of the query.
   // When running with PROOF Begin() is only called on the client.
   // The tree argument is deprecated (on PROOF 0 is passed).

  printf("\n\n");

  Info("Begin", "Starting preprocessing script to: determine correct fADC timing cuts");

  printf("\n");
  TString option = GetOption();
  if (option.Contains("NGC")) fNGC = kTRUE;
  if (option.Contains("showall")) fFullShow = kTRUE;
}

void preprocess::SlaveBegin(TTree * /*tree*/)
{
   // The SlaveBegin() function is called after the Begin() function.
   // When running with PROOF SlaveBegin() is called on each slave server.
   // The tree argument is deprecated (on PROOF 0 is passed).

   TString option = GetOption();
   
   fTiming = new TH1F*[4]; 
   for (Int_t ipmt = 0; ipmt < 4; ipmt++)
     {
       fTiming[ipmt] = new TH1F(Form("Timing_PMT%d",ipmt+1),Form("ADC TDC Diff Time PMT%d; Time (ns); Counts",ipmt+1), 400, -50, 0);
       GetOutputList()->Add(fTiming[ipmt]);
     }

}

Bool_t preprocess::Process(Long64_t entry)
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

   //Output to verify script is working, and store the total number of events
   if (entry % 100000 == 0) printf("Processing Entry number %lld\n",entry);

   //Require only one good track reconstruction for the event
   if (Ndata_P_tr_p[0] != 1) return kTRUE;

   //Redundant, but useful if multiple tracks will be allowed
   for (Int_t itrack = 0; itrack < Ndata_P_tr_p[0]; itrack++)
     {
       //Require cut on particle velocity
       if (TMath::Abs(P_tr_beta[itrack] -1.0) > 0.2) return kTRUE;

       //Fill the histograms
       for (Int_t ipmt = 0; ipmt < 4; ipmt++)
	 {
	   fTiming[ipmt]->Fill(P_hgcer_goodAdcTdcDiffTime[ipmt]);
	 }
     }

   return kTRUE;
}

void preprocess::SlaveTerminate()
{
   // The SlaveTerminate() function is called after all entries or objects
   // have been processed. When running with PROOF SlaveTerminate() is called
   // on each slave server.

}

void preprocess::Terminate()
{
   // The Terminate() function is the last function to be called during
   // a query. It always runs on the client, it can be used to present
   // the results graphically or save the results to file.
  printf("\n\n");
  Info("Terminate", "calibrating '%s'", (fNGC ? "NGC" : "HGC"));
  Info("Terminate", "'%s' showing", (fFullShow ? "full" : "minimal"));
  Info("Terminate","Histograms formed, now determining timing peaks");

  //Create temporary file to store fit information
  TFile ofile("Timing_Cuts.root","RECREATE");

  //Create arrays to store Gaussian fit information
  Float_t mean[4];
  Float_t sigma[4];

  //Create Ntuple to store timing data
  TNtuple timing_data("timing_data","Storage of Timing Information","Mean:Std"); 

  //Single Gaussian to find mean and spread of peak
  TF1 *Gauss1 = new TF1("Gauss1",pregauss,-10,200,3);
  Gauss1->SetParNames("Amplitude","Mean","Std. Dev.");

  //Have to extract histograms from OutputList
  TH1F *Timing[4];
  for (Int_t ipmt = 0; ipmt < 4; ipmt++)
    {
      Timing[ipmt] = dynamic_cast<TH1F*> (GetOutputList()->FindObject(Form("Timing_PMT%d",ipmt+1)));
    }

  //Visualize the timing information
  if (fFullShow) 
    {
      cTiming = new TCanvas("Timing", "Timing Information");
      cTiming->Divide(2,2);
    }

  TSpectrum *s = new TSpectrum(2);

  //Main loop to find timing peaks
  for (Int_t ipmt = 0; ipmt < 4; ipmt++)
    {
      //Use peak finding algorithm from TSpectrum
      
      if (fFullShow) cTiming->cd(ipmt+1);

      //Perform search for largest peak and save into array xpeaks
      fFullShow ? s->Search(Timing[ipmt],2.0,"nobackground",0.9) : s->Search(Timing[ipmt],2.0,"nobackground&&nodraw",0.9);
      TList *functions = Timing[ipmt]->GetListOfFunctions();
      TPolyMarker *pm = (TPolyMarker*)functions->FindObject("TPolyMarker");
      Double_t *xpeaks = pm->GetX();

      //Use the peak to fit with a Gaussian to determine range of cut
      Gauss1->SetRange(xpeaks[0]-3, xpeaks[0]+3);
      Gauss1->SetParameter(0, 10000.);
      Gauss1->SetParameter(1, xpeaks[0]);
      Gauss1->SetParameter(2, 0.4);
      //Gauss1->SetParLimits(0, 0., 10000.);
      Gauss1->SetParLimits(1, xpeaks[0]-3, xpeaks[0]+3);
      Gauss1->SetParLimits(2, 0.2, 1.);
      fFullShow ? Timing[ipmt]->Fit("Gauss1","RQ") : Timing[ipmt]->Fit("Gauss1","RQN");
      mean[ipmt] = Gauss1->GetParameter(1);
      sigma[ipmt] = Gauss1->GetParameter(2);
      timing_data.Fill(mean[ipmt],sigma[ipmt]);
    }

  cout << "\nTiming cut results are (mean & sigma):\n" << Form("PMT 1: %f, %f\nPMT 2: %f, %f\nPMT 3: %f, %f\nPMT 4: %f, %f\n\n",mean[0],sigma[0],mean[1],sigma[1],mean[2],sigma[2],mean[3],sigma[3]);

  timing_data.Write();
  ofile.Close();
}
