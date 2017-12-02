#define efficiencies_cxx
// The class definition in efficiencies.h has been generated automatically
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
// Root > T->Process("efficiencies.C+","some options")
// Root > T->Process("efficiencies.C+")
//

#include "efficiencies.h"
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <TPaveStats.h>
#include <iostream>
#include <iomanip>
#include <TRegexp.h>

void efficiencies::Begin(TTree * /*tree*/)
{
   // The Begin() function is called at the start of the query.
   // When running with PROOF Begin() is only called on the client.
   // The tree argument is deprecated (on PROOF 0 is passed).
  printf("\n\n");

  TString option = GetOption();
  //Regular expression to extract the specified photoelectron cutoff
  TRegexp re("[0-9]+[.][0-9]+");

  Info("Begin", "Starting efficiency process with option: %s", option.Data());
  Info("Begin", "To show details of calculation, use option showall (will open lots of windows)");
  Info("Begin", "Default detector is HGC, use option NGC if desired");
  Info("Begin", "If cut desired on other Cherenkov, use option Chercut");
  Info("Begin", "Photoelectron cut is taken as first decimal number entered. Default is 2.0");
  Info("Begin", "If cut on other Chenekov is requested, it will be taken as the second decimal number. Default is 2.0");
  printf("\n\n");

  //Check option
  
  if (option.Contains("showall")) fShowall = kTRUE;
  if (option.Contains("Chercut")) fChercut = kTRUE;
  if (option.Contains("NGC")) fNGC = kTRUE;

  //if a number is entered as %f.%f take that as the photoelectron cutoff
  if (option.Contains(re))
    {   
      TString cut_val = option.operator()(re);
      fNGC ? fNGC_cut =  cut_val.Atof() : fHGC_cut = cut_val.Atof(); 
      if (fChercut)
	{
	  TString cut_val = option.operator()(re, option.Index(re)+1);
	  fNGC ? fHGC_cut =  cut_val.Atof() : fNGC_cut = cut_val.Atof(); 
	}
    }
}

void efficiencies::SlaveBegin(TTree * /*tree*/)
{
  // The SlaveBegin() function is called after the Begin() function.
  // When running with PROOF SlaveBegin() is called on each slave server.
  // The tree argument is deprecated (on PROOF 0 is passed).

  printf("\n\n");
  TString option = GetOption();
  TRegexp re("[0-9]+[.][0-9]+");
  //Check option
  if (option.Contains("showall")) fShowall = kTRUE;
  if (option.Contains("Chercut")) fChercut = kTRUE;
  if (option.Contains("NGC")) fNGC = kTRUE;
  if (option.Contains(re))
    { 
      TString cut_val = option.operator()(re);
      fNGC ? fNGC_cut =  cut_val.Atof() : fHGC_cut = cut_val.Atof(); 
      if (fChercut)
	{
	  TString cut_val = option.operator()(re, option.Index(re)+1);
	  fNGC ? fHGC_cut =  cut_val.Atof() : fNGC_cut = cut_val.Atof(); 
	}
    }

  Info("SlaveBegin", "%s showing", fShowall ? "full" : "minimal");
  Info("SlaveBegin", "%s other Cherenkov for particle ID", fChercut ? "using" : "ignoring");
  Info("SlaveBegin", "efficiency for %s is found", fNGC ? "NGC" : "HGC");
  Info("SlaveBegin", "Number of photoelectrons to cut on is %f", fNGC ? fNGC_cut : fHGC_cut);
  if (fChercut) Info("SlaveBegin", "Number of photoelectrons to cut on in other Cherenkov %f", fNGC ? fHGC_cut : fNGC_cut);

  printf("\n");

  
  //Initialize the histograms.
  Int_t NPE_min;
  Int_t NPE_max;
  Int_t bins;

  NPE_min = 0;
  NPE_max = 20;
  bins = (NPE_min + NPE_max)*10;

  fNPE_eNoDet = new TH1F*[4];
  fNPE_eNoDet[0] = new TH1F("NPE_eNoDet_PMT1", "NPE in PMT1 No Detector Cut;NPE;Counts", bins, NPE_min, NPE_max);
  fNPE_eNoDet[1] = new TH1F("NPE_eNoDet_PMT2", "NPE in PMT2 No Detector Cut;NPE;Counts", bins, NPE_min, NPE_max);
  fNPE_eNoDet[2] = new TH1F("NPE_eNoDet_PMT3", "NPE in PMT3 No Detector Cut;NPE;Counts", bins, NPE_min, NPE_max);
  fNPE_eNoDet[3] = new TH1F("NPE_eNoDet_PMT4", "NPE in PMT4 No Detector Cut;NPE;Counts", bins, NPE_min, NPE_max);
  GetOutputList()->Add(fNPE_eNoDet[0]);
  GetOutputList()->Add(fNPE_eNoDet[1]);
  GetOutputList()->Add(fNPE_eNoDet[2]);
  GetOutputList()->Add(fNPE_eNoDet[3]);
  

  fNPE_eDet = new TH1F*[4];
  fNPE_eDet[0] = new TH1F("NPE_eDet_PMT1", "NPE in PMT1 with Detector Cut;NPE;Counts", bins, NPE_min, NPE_max);
  fNPE_eDet[1] = new TH1F("NPE_eDet_PMT2", "NPE in PMT2 with Detector Cut;NPE;Counts", bins, NPE_min, NPE_max);
  fNPE_eDet[2] = new TH1F("NPE_eDet_PMT3", "NPE in PMT3 with Detector Cut;NPE;Counts", bins, NPE_min, NPE_max);
  fNPE_eDet[3] = new TH1F("NPE_eDet_PMT4", "NPE in PMT4 with Detector Cut;NPE;Counts", bins, NPE_min, NPE_max);
  GetOutputList()->Add(fNPE_eDet[0]);
  GetOutputList()->Add(fNPE_eDet[1]);
  GetOutputList()->Add(fNPE_eDet[2]);
  GetOutputList()->Add(fNPE_eDet[3]);

  fNPE_Full_eNoDet = new TH1F("NPE_Full_eNoDet", "NPE in All PMTs with no Detector Cut;NPE;Counts", bins, NPE_min, NPE_max);
  GetOutputList()->Add(fNPE_Full_eNoDet); 
  
  fNPE_Full_eDet = new TH1F("NPE_Full_eDet", "NPE in All PMTs with Detector Cut;NPE;Counts", bins, NPE_min, NPE_max);
  GetOutputList()->Add(fNPE_Full_eDet);

  fNPE_piNoDet = new TH1F*[4];
  fNPE_piNoDet[0] = new TH1F("NPE_piNoDet_PMT1", "NPE in PMT1 No Detector Cut;NPE;Counts", bins, NPE_min, NPE_max);
  fNPE_piNoDet[1] = new TH1F("NPE_piNoDet_PMT2", "NPE in PMT2 No Detector Cut;NPE;Counts", bins, NPE_min, NPE_max);
  fNPE_piNoDet[2] = new TH1F("NPE_piNoDet_PMT3", "NPE in PMT3 No Detector Cut;NPE;Counts", bins, NPE_min, NPE_max);
  fNPE_piNoDet[3] = new TH1F("NPE_piNoDet_PMT4", "NPE in PMT4 No Detector Cut;NPE;Counts", bins, NPE_min, NPE_max);
  GetOutputList()->Add(fNPE_piNoDet[0]);
  GetOutputList()->Add(fNPE_piNoDet[1]);
  GetOutputList()->Add(fNPE_piNoDet[2]);
  GetOutputList()->Add(fNPE_piNoDet[3]);

  fNPE_piDet = new TH1F*[4];
  fNPE_piDet[0] = new TH1F("NPE_piDet_PMT1", "NPE in PMT1 with Detector Cut;NPE;Counts", bins, NPE_min, NPE_max);
  fNPE_piDet[1] = new TH1F("NPE_piDet_PMT2", "NPE in PMT2 with Detector Cut;NPE;Counts", bins, NPE_min, NPE_max);
  fNPE_piDet[2] = new TH1F("NPE_piDet_PMT3", "NPE in PMT3 with Detector Cut;NPE;Counts", bins, NPE_min, NPE_max);
  fNPE_piDet[3] = new TH1F("NPE_piDet_PMT4", "NPE in PMT4 with Detector Cut;NPE;Counts", bins, NPE_min, NPE_max);
  GetOutputList()->Add(fNPE_piDet[0]);
  GetOutputList()->Add(fNPE_piDet[1]);
  GetOutputList()->Add(fNPE_piDet[2]);
  GetOutputList()->Add(fNPE_piDet[3]);

  fNPE_Full_piNoDet = new TH1F("NPE_Full_piNoDet", "NPE in All PMTs with no Detector Cut;NPE;Counts", bins, NPE_min, NPE_max);
  GetOutputList()->Add(fNPE_Full_piNoDet); 

  fNPE_Full_piDet = new TH1F("NPE_Full_piDet", "NPE in All PMTs with Detector Cut;NPE;Counts", bins, NPE_min, NPE_max);
  GetOutputList()->Add(fNPE_Full_piDet);

  //Histograms examining various cuts for "good" hits
  fBeta_Cut = new TH1F("Beta_Cut", "Beta cut used for 'good' hits;Beta;Counts", 1000, -5, 5);
  GetOutputList()->Add(fBeta_Cut);

  fBeta_Full = new TH1F("Beta_Full", "Full beta for events;Beta;Counts", 1000, -5, 5);
  GetOutputList()->Add(fBeta_Full);

  fTiming_Cut = new TH1F("Timing_Cut", "Timing cut used for 'good' hits;Time (ns);Counts", 10000, 1, 200);
  GetOutputList()->Add(fTiming_Cut);

  fTiming_Full = new TH1F("Timing_Full", "Full timing information for events;Time (ns);Counts", 10000, 1, 200);
  GetOutputList()->Add(fTiming_Full);

  //Histograms examining particle ID cuts
  fFly_Pr_Full = new TH2F("Fly_Pr_Full", "Particle ID from calorimeter & preshower;Calorimeter (GeV);Pre-Shower (GeV)", 200, 0.0, 1.0, 200, 0.0, 1.0);
  GetOutputList()->Add(fFly_Pr_Full);
  
  fFly_Pr_eCut = new TH2F("Fly_Pr_eCut", "calorimeter & preshower electrons;Calorimeter (GeV);Pre-Shower (GeV)", 200, 0.0, 1.0, 200, 0.0, 1.0);
  GetOutputList()->Add(fFly_Pr_eCut);

  fFly_Pr_piCut = new TH2F("Fly_Pr_piCut", "calorimeter & preshower pions;Calorimeter (GeV);Pre-Shower (GeV)", 200, 0.0, 1.0, 200, 0.0, 1.0);
  GetOutputList()->Add(fFly_Pr_piCut);

  printf("\n\n");
}

Bool_t efficiencies::Process(Long64_t entry)
{
   // The Process() function is called for each entry in the tree (or possibly
   // keyed object in the case of PROOF) to be processed. The entry argument
   // specifies which entry in the currently loaded tree is to be processed.
   // It can be passed to either efficiencies::GetEntry() or TBranch::GetEntry()
   // to read either all or the required parts of the data. When processing
   // keyed objects with PROOF, the object is already loaded and is available
   // via the fObject pointer.
   //
   // This function should contain the "body" of the analysis. It can contain
   // simple or elaborate selection criteria, run algorithms on the data
   // of the event and typically fill histograms.
   //
   // The processing can be stopped by calling Abort().
   //
   // Use fStatus to set the return value of TTree::Process().
   //
   // The return value is currently not used.

  //Output to verify script is working, and store the total number of events
  //if (entry % 100000 == 0) printf("Processing Entry number %lld\n",entry);

  //Get the entry to loop over 
  b_Ndata_P_tr_p->GetEntry(entry);

  //Require only one good track reconstruction for the event
  if (Ndata_P_tr_p != 1) return kTRUE;

  //Redundant requirement, but useful if multiple tracks are eventually allowed
  for (Int_t itrack = 0; itrack < Ndata_P_tr_p; itrack++)
    {
      //Require loose cut on particle velocity
      b_P_tr_beta->GetEntry(entry);
      fBeta_Full->Fill(P_tr_beta[itrack]);
      if (TMath::Abs(P_tr_beta[itrack] - 1.0) > 0.2) return kTRUE;
      fBeta_Cut->Fill(P_tr_beta[itrack]);

      //Filling the histograms
      for (Int_t ipmt = 0; ipmt < hgc_pmts; ipmt++)
	{
	  //Require the signal passes a timing cut
	  fNGC ? b_P_ngcer_goodAdcPulseTime->GetEntry(entry) : b_P_hgcer_goodAdcPulseTime->GetEntry(entry);
	  fTiming_Full->Fill(fNGC ?  P_ngcer_goodAdcPulseTime[ipmt] : P_hgcer_goodAdcPulseTime[ipmt]);
	  if (fNGC ? P_ngcer_goodAdcPulseTime[ipmt] < 50 || P_ngcer_goodAdcPulseTime[ipmt] > 125 :
	  P_hgcer_goodAdcPulseTime[ipmt] < 70 || P_hgcer_goodAdcPulseTime[ipmt] > 135) continue;
	  fTiming_Cut->Fill(fNGC ?  P_ngcer_goodAdcPulseTime[ipmt] : P_hgcer_goodAdcPulseTime[ipmt]);

	  //Require the signal passes a tracking cut, with a threshold NPE cut as well
	  fNGC ? b_P_ngcer_numTracksFired->GetEntry(entry) : b_P_hgcer_numTracksFired->GetEntry(entry);
	  if (fNGC ? P_ngcer_numTracksFired[ipmt] == 0.0 : P_hgcer_numTracksFired[ipmt] == 0.0) continue;

	  //Begin particle identification using calorimeter and NGC
	  fNGC ? b_P_ngcer_npe->GetEntry(entry) : b_P_hgcer_npe->GetEntry(entry);	  
	  fNGC ? b_P_hgcer_npeSum->GetEntry(entry) : b_P_ngcer_npeSum->GetEntry(entry);
	  b_P_cal_fly_earray->GetEntry(entry);
	  b_P_cal_pr_eplane->GetEntry(entry);
	  b_P_gtr_dp->GetEntry(entry);
	  Float_t central_p = 3.0;
	  Float_t p = ((P_gtr_dp/100.0)*central_p) + central_p;

	  //Visualize what we have to cut with
	  fFly_Pr_Full->Fill(P_cal_fly_earray/p, P_cal_pr_eplane/p);

	  //Perform cut for electrons
	  //Cut on Shower vs preshower is a tilted ellipse, this requires an angle of rotation (in radians), x/y center, semimajor and semiminor axis
	  Float_t eangle = 3.0*3.14159/4.0;
	  //Float_t eangle2 = 3.14159/4.0;
	  Float_t ex_center = 0.66;
	  //Float_t ex_center2 = -0.04;
	  Float_t ey_center = 0.35;
	  //Float_t ey_center2 = -0.03;
	  Float_t esemimajor_axis = 0.28;
	  Float_t esemiminor_axis = 0.04;
	  if (pow((P_cal_fly_earray/p - ex_center)*cos(eangle) + (P_cal_pr_eplane/p - ey_center)*sin(eangle),2)/pow(esemimajor_axis,2) + 
	      pow((P_cal_fly_earray/p - ex_center)*sin(eangle) - (P_cal_pr_eplane/p - ey_center)*cos(eangle),2)/pow(esemiminor_axis,2) < 1)
	    {
	      if (!fChercut || (fChercut && (fNGC ? P_hgcer_npeSum > fHGC_cut : P_ngcer_npeSum > fNGC_cut))) //condition if cut on other Cherenkov if desired
		{
		  fFly_Pr_eCut->Fill(P_cal_fly_earray/p, P_cal_pr_eplane/p);
		  fNPE_eNoDet[ipmt]->Fill(fNGC ? P_ngcer_npe[ipmt] : P_hgcer_npe[ipmt]);
		  fNPE_Full_eNoDet->Fill(fNGC ? P_ngcer_npe[ipmt] : P_hgcer_npe[ipmt]);
	      
		  if (fNGC ? P_ngcer_npe[ipmt] > fNGC_cut : P_hgcer_npe[ipmt] > fHGC_cut)  //Cut on detector to determine efficiency
		    {
		      fNPE_eDet[ipmt]->Fill(fNGC ? P_ngcer_npe[ipmt] : P_hgcer_npe[ipmt]);
		      fNPE_Full_eDet->Fill(fNGC ? P_ngcer_npe[ipmt] : P_hgcer_npe[ipmt]);
		    }
		}
	    }

	  //Perform cut for pions
	  //Cut on Shower vs preshower is a tilted ellipse, this requires an angle of rotation (in radians), x/y center, semimajor and semiminor axis
	  Float_t piangle = 0.0;
	  Float_t pix_center = 0.26;
	  Float_t piy_center = 0.03;
	  Float_t pisemimajor_axis = 0.1;
	  Float_t pisemiminor_axis = 0.02;
	  if (pow((P_cal_fly_earray/p - pix_center)*cos(piangle) + (P_cal_pr_eplane/p - piy_center)*sin(piangle),2)/pow(pisemimajor_axis,2) + 
	      pow((P_cal_fly_earray/p - pix_center)*sin(piangle) - (P_cal_pr_eplane/p - piy_center)*cos(piangle),2)/pow(pisemiminor_axis,2) < 1)
	    {
	      if (!fChercut || (fChercut && (fNGC ? P_hgcer_npeSum < fHGC_cut : P_ngcer_npeSum < fNGC_cut))) //condition if cut on other Cherenkov is desired
		{
		  fFly_Pr_piCut->Fill(P_cal_fly_earray/p, P_cal_pr_eplane/p);
		  fNPE_piNoDet[ipmt]->Fill(fNGC ? P_ngcer_npe[ipmt] : P_hgcer_npe[ipmt]);
		  fNPE_Full_piNoDet->Fill(fNGC ? P_ngcer_npe[ipmt] : P_hgcer_npe[ipmt]);
	      		 
		  if (fNGC ? P_ngcer_npe[ipmt] > fNGC_cut : P_hgcer_npe[ipmt] > fHGC_cut)   //Cut on detector to determine efficiency
		    {
		      fNPE_piDet[ipmt]->Fill(fNGC ? P_ngcer_npe[ipmt] : P_hgcer_npe[ipmt]);
		      fNPE_Full_piDet->Fill(fNGC ? P_ngcer_npe[ipmt] : P_hgcer_npe[ipmt]);
		    }
		}
	    }

	}//End loop on PMTs
    }//End loop over tracks

   return kTRUE;
}

void efficiencies::SlaveTerminate()
{
   // The SlaveTerminate() function is called after all entries or objects
   // have been processed. When running with PROOF SlaveTerminate() is called
   // on each slave server.
}

void efficiencies::Terminate()
{
   // The Terminate() function is the last function to be called during
   // a query. It always runs on the client, it can be used to present
   // the results graphically or save the results to file.
  printf("\n\n");
  Info("Terminate", "%s showing", fShowall ? "full" : "minimal");
  Info("Terminate", "%s other Cherenkov for particle ID", fChercut ? "using" : "ignoring");
  Info("Terminate", "efficiency for %s is found", fNGC ? "NGC" : "HGC");
  Info("Terminate", "Number of photoelectrons to cut on is %f", fNGC ? fNGC_cut : fHGC_cut);
  if (fChercut) Info("Terminate", "Number of photoelectrons to cut on in other Cherenkov %f", fNGC ? fHGC_cut : fNGC_cut);

  //Need to exact the histograms from the OutputList
  TH1F* NPE_eNoDet[4];
  TH1F* NPE_eDet[4];
  TH1F* NPE_piNoDet[4];
  TH1F* NPE_piDet[4];

  for (Int_t ipmt = 0; ipmt < 4; ipmt++)
    {
      NPE_eNoDet[ipmt] = dynamic_cast<TH1F*> (GetOutputList()->FindObject(Form("NPE_eNoDet_PMT%d",ipmt+1)));
      NPE_eDet[ipmt] = dynamic_cast<TH1F*> (GetOutputList()->FindObject(Form("NPE_eDet_PMT%d",ipmt+1)));
      NPE_piNoDet[ipmt] = dynamic_cast<TH1F*> (GetOutputList()->FindObject(Form("NPE_piNoDet_PMT%d",ipmt+1)));
      NPE_piDet[ipmt] = dynamic_cast<TH1F*> (GetOutputList()->FindObject(Form("NPE_piDet_PMT%d",ipmt+1)));
    }
  


  //Canvases to display efficiency information
  if (fShowall)
    {
      //Canvas to show beta cut information
      TCanvas *Beta;
      Beta = new TCanvas("Beta", "Beta information for events");
      Beta->Divide(2,1);
      Beta->cd(1);
      fBeta_Full->Draw();
      Beta->cd(2);
      fBeta_Cut->Draw();

      //Canvas to show timing cut information
      TCanvas *Timing;
      Timing = new TCanvas("Timing", "Timing information for events");
      Timing->Divide(2,1);
      Timing->cd(1);
      fTiming_Full->Draw();
      Timing->cd(2);

      fTiming_Cut->Draw();

      //Canvas to show Particle ID cut information
      TCanvas *Fly_Pr;
      Fly_Pr = new TCanvas("Fly_Pr", "Particle ID info from calorimeter & Preshower");
      Fly_Pr->Divide(3,1);
      Fly_Pr->cd(1);
      fFly_Pr_Full->Draw("Colz");
      Fly_Pr->cd(2);
      fFly_Pr_eCut->Draw("Colz");
      Fly_Pr->cd(3);
      fFly_Pr_piCut->Draw("Colz");

      //Canvases to show Effects of HGC cut
      //Start with electrons
      gStyle->SetOptStat(11);
      TCanvas *Det_eCut;
      Det_eCut = new TCanvas("Det_eCut","Effect of performing cut for electrons per PMT");
      Det_eCut->Divide(2,4);
      for (Int_t ipad = 0; ipad < 8; ipad += 2)
	{
	  Det_eCut->cd(ipad+1);
	  NPE_eNoDet[ipad/2]->Draw();
	  gPad->Update();
	  TPaveStats *s1 = (TPaveStats*) gPad->GetPrimitive("stats");
	  s1->SetTextSize(0.1), s1->SetX1NDC(0.7), s1->SetY1NDC(0.5);

	  Det_eCut->cd(ipad+2);
	  NPE_eDet[ipad/2]->Draw();
	  gPad->Update();
	  TPaveStats *s2 = (TPaveStats*) gPad->GetPrimitive("stats");
	  s2->SetTextSize(0.1), s2->SetX1NDC(0.7), s2->SetY1NDC(0.5);
	}

      TCanvas *Det_eCut_Full;
      Det_eCut_Full = new TCanvas("Det_eCut_Full","Effect of performing cut for electrons");
      Det_eCut_Full->Divide(2,1);
      Det_eCut_Full->cd(1);
      fNPE_Full_eNoDet->Draw();
      Det_eCut_Full->cd(2);
      fNPE_Full_eDet->Draw();

      //End with Pions
      gStyle->SetOptStat(11);
      TCanvas *Det_piCut;
      Det_piCut = new TCanvas("Det_piCut","Effect of performing cut for pions per PMT");
      Det_piCut->Divide(2,4);
      for (Int_t ipad = 0; ipad < 8; ipad += 2)
	{
	  Det_piCut->cd(ipad+1);
	  NPE_piNoDet[ipad/2]->Draw();
	  gPad->Update();
	  TPaveStats *s3 = (TPaveStats*) gPad->GetPrimitive("stats");
	  s3->SetTextSize(0.1), s3->SetX1NDC(0.7), s3->SetY1NDC(0.5);

	  Det_piCut->cd(ipad+2);
	  NPE_piDet[ipad/2]->Draw();
	  gPad->Update();
	  TPaveStats *s4 = (TPaveStats*) gPad->GetPrimitive("stats");
	  s4->SetTextSize(0.1), s4->SetX1NDC(0.7), s4->SetY1NDC(0.5);
	}
      	
      TCanvas *Det_piCut_Full;
      Det_piCut_Full = new TCanvas("Det_piCut_Full","Effect of performing cut for pions");
      Det_piCut_Full->Divide(2,1);
      Det_piCut_Full->cd(1);
      fNPE_Full_piNoDet->Draw();
      Det_piCut_Full->cd(2);
      fNPE_Full_piDet->Draw();
    }
  
  //Output the actual efficiency information i.e. ratio of detected particles
  cout << Form("\nEfficiencies for the %s with a cut at %.1f are:\nPMT#  electrons  pions:electron", fNGC ? "NGC" : "HGC", fNGC ? fNGC_cut : fHGC_cut) << endl;
  for (Int_t ipmt = 0; ipmt < 4; ipmt++)
    {
      printf("PMT%d: %2.2f%%         1:%.f\n", ipmt+1, (NPE_eDet[ipmt]->GetEntries()/NPE_eNoDet[ipmt]->GetEntries())*100.0, NPE_eDet[ipmt]->GetEntries()/NPE_piDet[ipmt]->GetEntries());
    }  
  printf("Full: %2.2f%%         1:%.f\n\n\n",(fNPE_Full_eDet->GetEntries()/fNPE_Full_eNoDet->GetEntries())*100.0, fNPE_Full_eDet->GetEntries()/fNPE_Full_piDet->GetEntries());
}
