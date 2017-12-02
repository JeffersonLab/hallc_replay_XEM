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
// Root > T->Process("calibration.C+")
// Root > T->Process("calibration.C+","some options")
//


#include "calibration.h"
#include <TH1.h>
#include <TH2.h>
#include <TF1.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <TPaveText.h>
#include <TSpectrum.h>
#include <TList.h>
#include <TPolyMarker.h>
#include <TGraphErrors.h>
#include <TMath.h>
#include <iostream>
#include <iomanip>
#include <fstream>

using namespace TMath;

void calibration::Begin(TTree * /*tree*/)
{
  // The Begin() function is called at the start of the query.
  // When running with PROOF Begin() is only called on the client.
  // The tree argument is deprecated (on PROOF 0 is passed).
  printf("\n\n");

  TString option = GetOption();
  Info("Begin", "Script will fail unless 'calibration.C+' is used");
  Info("Begin", "Starting calibration process with option: %s", option.Data());
  Info("Begin", "To load all branches, use option readall (warning, very slow)");
  Info("Begin", "To see details of calibration, use option showall");
  Info("Begin", "Default calibration is the HGC, for NGC use option NGC");
  Info("Begin", "To calibrate using TrackFired leaf, use option trackfired");
  Info("Begin", "Default is no particle cut, use option cut if desired");
  Info("Begin", "Default particle ID is electrons, use option pions if desired");
  printf("\n\n");

  //Check option
  if (option.Contains("readall")) fFullRead = kTRUE;
  if (option.Contains("NGC")) fNGC = kTRUE;
  if (option.Contains("showall")) fFullShow = kTRUE;
  if (option.Contains("trackfired")) fTrack = kTRUE;
  if (option.Contains("pions") || option.Contains("pion")) fPions = kTRUE;
  if (option.Contains("cut") || fPions || option.Contains("cuts")) fCut = kTRUE;
}

void calibration::SlaveBegin(TTree * /*tree*/)
{
  // The SlaveBegin() function is called after the Begin() function.
  // When running with PROOF SlaveBegin() is called on each slave server.
  // The tree argument is deprecated (on PROOF 0 is passed).

  printf("\n\n");
  TString option = GetOption();
   
  //Check option
  if (option.Contains("readall")) fFullRead = kTRUE;
  if (option.Contains("NGC")) fNGC = kTRUE;
  if (option.Contains("showall")) fFullShow = kTRUE;
  if (option.Contains("trackfired")) fTrack = kTRUE;
  if (option.Contains("pions") || option.Contains("pion")) fPions = kTRUE;
  if (option.Contains("cut") || fPions || option.Contains("cuts")) fCut = kTRUE;

  Info("SlaveBegin", "'%s' reading", (fFullRead ? "full" : "optimized"));
  Info("SlaveBegin", "calibrating '%s'", (fNGC ? "NGC" : "HGC"));
  Info("SlaveBegin", "'%s' showing", (fFullShow ? "full" : "minimal"));
  Info("SlaveBegin", "'%s' strategy", (fTrack ? "tracking" : "quadrant"));
  Info("SlaveBegin", "cuts %s performed", (fCut ? "are" : "are not"));
  if (fCut) Info("SlaveBegin", "cutting for '%s'", (fPions ? "pions" : "electrons"));

  // Inintialize the histograms. Note they are binned per ADC channel which will be changed in the calibration analysis.
  Int_t ADC_min;
  Int_t ADC_max;
  Int_t bins;

  if (fNGC) //Set up histograms for NGC
    {
      ADC_min = -10;
      ADC_max = 250;
      bins = 2*(abs(ADC_min) + abs(ADC_max));
    }

  if (!fNGC) //Set up histograms for HGC
    {
      ADC_min = -10;
      ADC_max = 200;
      bins = 2*(abs(ADC_min) + abs(ADC_max));
    }

  fPulseInt = new TH1F*[4];
  fPulseInt_quad = new TH1F**[4];
  fPulseInt_quad[0] = new TH1F*[4];
  fPulseInt_quad[1] = new TH1F*[4];
  fPulseInt_quad[2] = new TH1F*[4];
  fPulseInt_quad[3] = new TH1F*[4];
  for (Int_t ipmt = 0; ipmt < 4; ipmt++)
    {
      fPulseInt[ipmt] = new TH1F(Form("PulseInt_PMT%d",ipmt+1),Form("Pulse Integral PMT%d; ADC Channel (pC); Counts",ipmt+1), bins, ADC_min, ADC_max);
      GetOutputList()->Add(fPulseInt[ipmt]);
      for (Int_t iquad = 0; iquad < 4; iquad++)
	{
	  fPulseInt_quad[iquad][ipmt] = new TH1F(Form("PulseInt_quad%d_PMT%d",iquad+1,ipmt+1),Form("Pulse Integral PMT%d quad%d; ADC Channel (pC); Counts",ipmt+1,iquad+1),bins,ADC_min,ADC_max);
	  GetOutputList()->Add(fPulseInt_quad[iquad][ipmt]);
	}
    }

  fCut_everything = new TH2F("Cut_everything", "Visualization of no cuts; Calorimeter Energy (GeV); Pre-Shower Energy (GeV)", 200, 0, 1.0, 200, 0, 1.0);
  GetOutputList()->Add(fCut_everything);
  fCut_electron = new TH2F("Cut_electron", "Visualization of electron cut; Calorimeter Energy (GeV); Pre-Shower Energy (GeV)", 200, 0, 1.0, 200, 0, 1.0);
  GetOutputList()->Add(fCut_electron);
  fCut_pion = new TH2F("Cut_pion", "Visualization of pion cut; Calorimeter Energy (GeV); Pre-Shower Energy (GeV)", 200, 0, 1.0, 200, 0, 1.0);
  GetOutputList()->Add(fCut_pion);
  
  printf("\n\n");
}

Bool_t calibration::Process(Long64_t entry)
{
  // The Process() function is called for each entry in the tree (or possibly
  // keyed object in the case of PROOF) to be processed. The entry argument
  // specifies which entry in the currently loaded tree is to be processed.
  // It can be passed to either calibration::GetEntry() or TBranch::GetEntry()
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
  if (entry % 100000 == 0) printf("Processing Entry number %lld\n",entry);

  //Define quantities to loop over
  Int_t fpmts;
  fpmts = fNGC ? fngc_pmts : fhgc_pmts;   //Note HGC & NGC have the same # of PMTS

  //Get the entry to loop over
  if (fFullRead) fChain->GetTree()->GetEntry(entry);
  else b_Ndata_P_tr_p->GetEntry(entry);
  
  //Require only one good track reconstruction for the event
  if (Ndata_P_tr_p != 1) return kTRUE;
  
  //Redundant, but useful if multiple tracks are eventually allowed
  for (Int_t itrack = 0; itrack < Ndata_P_tr_p; itrack++) 
    {
      if (!fFullRead) b_P_tr_beta->GetEntry(entry);
      //Require loose cut on particle velocity
      if (TMath::Abs(P_tr_beta[itrack] -1.) > 0.2) return kTRUE;

      //Filling the histograms
      for (Int_t ipmt = 0; ipmt < fpmts; ipmt++) 
	{	  
	  //Perform a loose timing cut
	  if (!fFullRead) fNGC ? b_P_ngcer_goodAdcPulseTime->GetEntry(entry) : b_P_hgcer_goodAdcPulseTime->GetEntry(entry);
	  if (fNGC ? P_ngcer_goodAdcPulseTime[ipmt] < 50 || P_ngcer_goodAdcPulseTime[ipmt] > 125 :
	             P_hgcer_goodAdcPulseTime[ipmt] < 70 || P_hgcer_goodAdcPulseTime[ipmt] > 135) continue;

	  //Cuts to remove entries corresponding to a PMT not registering a hit	  
	  if (!fFullRead) fNGC ? b_P_ngcer_goodAdcPulseInt->GetEntry(entry) : b_P_hgcer_goodAdcPulseInt->GetEntry(entry);
	  if (fNGC ? P_ngcer_goodAdcPulseInt[ipmt] == 0.0 : P_hgcer_goodAdcPulseInt[ipmt] == 0.0) continue;
	 	  
	  //For quadrant cut strategy with particle ID cuts. In this case electrons are selected
	  if (!fTrack && fCut && !fPions)
	    {
	      //Retrieve particle ID information
	      if (!fFullRead) b_P_cal_fly_earray->GetEntry(entry);
	      if (!fFullRead) b_P_cal_pr_eplane->GetEntry(entry);
	      if (!fFullRead) b_P_gtr_dp->GetEntry(entry);
	      Float_t central_p = 3.0;
	      Float_t p = ((P_gtr_dp/100.0)*central_p) + central_p;

	      //Fill histogram visualizaing the electron selection
	      fCut_everything->Fill(P_cal_fly_earray/p, P_cal_pr_eplane/p);

	      //Cut on Shower vs preshower is a tilted ellipse, this requires an angle of rotation (in radians), x/y center, semimajor and semiminor axis
	      Float_t eangle = 3.0*3.14159/4.0;
	      Float_t ex_center = 0.66;
	      Float_t ey_center = 0.35;
	      Float_t esemimajor_axis = 0.28;
	      Float_t esemiminor_axis = 0.04;
	      if (pow((P_cal_fly_earray/p - ex_center)*cos(eangle) + (P_cal_pr_eplane/p - ey_center)*sin(eangle),2)/pow(esemimajor_axis,2) + 
		  pow((P_cal_fly_earray/p - ex_center)*sin(eangle) - (P_cal_pr_eplane/p - ey_center)*cos(eangle),2)/pow(esemiminor_axis,2) < 1)
		{
		  //Fill histogram visualizing the electron selection
		  fCut_electron->Fill(P_cal_fly_earray/p, P_cal_pr_eplane/p);

		  //Retrieve information for particle tracking from focal plane
		  if (!fFullRead) b_P_tr_y->GetEntry(entry), b_P_tr_ph->GetEntry(entry);
		  if (!fFullRead) b_P_tr_x->GetEntry(entry), b_P_tr_th->GetEntry(entry);

		  //Fill histogram of the full PulseInt spectra for each PMT
		  fNGC ? fPulseInt[ipmt]->Fill(P_ngcer_goodAdcPulseInt[ipmt]) : fPulseInt[ipmt]->Fill(P_hgcer_goodAdcPulseInt[ipmt]);

		  //Fill histograms of what each PMT registers from each quadrant, this requires tracking the particle from the focal plane. Each quadrant is defined from the parameter files
		  Float_t y_pos = fNGC ? P_tr_y[0] + P_tr_ph[0]*fngc_zpos : P_tr_y[0] + P_tr_ph[0]*fhgc_zpos;
		  Float_t x_pos = fNGC ? P_tr_x[0] + P_tr_th[0]*fngc_zpos : P_tr_x[0] + P_tr_th[0]*fhgc_zpos;
		  
		  //Condition for quadrant 1 mirror
		  if (fNGC ? y_pos >= 0.0 && x_pos >= 0.0 : y_pos >= 4.6 && x_pos >= 9.4)
		    fNGC ? fPulseInt_quad[0][ipmt]->Fill(P_ngcer_goodAdcPulseInt[ipmt]) : fPulseInt_quad[0][ipmt]->Fill(P_hgcer_goodAdcPulseInt[ipmt]);

		  //Condition for quadrant 2 mirror
		  if (fNGC ? y_pos < 0.0 && x_pos >= 0.0 : y_pos < 4.6 && x_pos >= 9.4)
		    fNGC ? fPulseInt_quad[1][ipmt]->Fill(P_ngcer_goodAdcPulseInt[ipmt]) : fPulseInt_quad[1][ipmt]->Fill(P_hgcer_goodAdcPulseInt[ipmt]);

		  //Condition for quadrant 3 mirror
		  if (fNGC ? y_pos >= 0.0 && x_pos < 0.0 : y_pos >= 4.6 && x_pos < 9.4)
		    fNGC ? fPulseInt_quad[2][ipmt]->Fill(P_ngcer_goodAdcPulseInt[ipmt]) : fPulseInt_quad[2][ipmt]->Fill(P_hgcer_goodAdcPulseInt[ipmt]);

		  //Condition for quadrant 4 mirror
		  if (fNGC ? y_pos < 0.0 && x_pos < 0.0 : y_pos < 4.6 && x_pos < 9.4)
		    fNGC ? fPulseInt_quad[3][ipmt]->Fill(P_ngcer_goodAdcPulseInt[ipmt]) : fPulseInt_quad[3][ipmt]->Fill(P_hgcer_goodAdcPulseInt[ipmt]);
		}
	    }//Marks end of electron selection condition


	  //For quadrant cut strategy with particle ID cuts. In this case pions are selected
	  if (!fTrack && fCut && fPions)
	    {
	      //Retrieve particle ID information
	      if (!fFullRead) b_P_cal_fly_earray->GetEntry(entry);
	      if (!fFullRead) b_P_cal_pr_eplane->GetEntry(entry);
	      if (!fFullRead) b_P_gtr_dp->GetEntry(entry);
	      Float_t central_p = 3.0;
	      Float_t p = ((P_gtr_dp/100.0)*central_p) + central_p;

	      //Fill histogram visualizaing the pion selection
	      fCut_everything->Fill(P_cal_fly_earray/p, P_cal_pr_eplane/p);

	      //Cut on Shower vs preshower is a tilted ellipse, this requires an angle of rotation (in radians), x/y center, semimajor and semiminor axis
	      Float_t piangle = 0.0;
	      Float_t pix_center = 0.26;
	      Float_t piy_center = 0.03;
	      Float_t pisemimajor_axis = 0.1;
	      Float_t pisemiminor_axis = 0.02;
	      if (pow((P_cal_fly_earray/p - pix_center)*cos(piangle) + (P_cal_pr_eplane/p - piy_center)*sin(piangle),2)/pow(pisemimajor_axis,2) + 
		  pow((P_cal_fly_earray/p - pix_center)*sin(piangle) - (P_cal_pr_eplane/p - piy_center)*cos(piangle),2)/pow(pisemiminor_axis,2) < 1)
		{
		  //Fill histogram visualizaing the pion selection
		  fCut_pion->Fill(P_cal_fly_earray/p, P_cal_pr_eplane/p);

		  //Retrieve information for particle tracking from focal plane
		  if (!fFullRead) b_P_tr_y->GetEntry(entry), b_P_tr_ph->GetEntry(entry);
		  if (!fFullRead) b_P_tr_x->GetEntry(entry), b_P_tr_th->GetEntry(entry);

		  //Fill histogram of the full PulseInt spectra for each PMT
		  fNGC ? fPulseInt[ipmt]->Fill(P_ngcer_goodAdcPulseInt[ipmt]) : fPulseInt[ipmt]->Fill(P_hgcer_goodAdcPulseInt[ipmt]);

		  //Fill histograms of what each PMT registers from each quadrant, this requires tracking the particle from the focal plane. Each quadrant is defined from the parameter files
		  Float_t y_pos = fNGC ? P_tr_y[0] + P_tr_ph[0]*fngc_zpos : P_tr_y[0] + P_tr_ph[0]*fhgc_zpos;
		  Float_t x_pos = fNGC ? P_tr_x[0] + P_tr_th[0]*fngc_zpos : P_tr_x[0] + P_tr_th[0]*fhgc_zpos;
		  
		  //Condition for quadrant 1 mirror
		  if (fNGC ? y_pos >= 0.0 && x_pos >= 0.0 : y_pos >= 4.6 && x_pos >= 9.4) 
		    fNGC ? fPulseInt_quad[0][ipmt]->Fill(P_ngcer_goodAdcPulseInt[ipmt]) : fPulseInt_quad[0][ipmt]->Fill(P_hgcer_goodAdcPulseInt[ipmt]);

		  //Condition for quadrant 2 mirror
		  if (fNGC ? y_pos < 0.0 && x_pos >= 0.0 : y_pos < 4.6 && x_pos >= 9.4)
		    fNGC ? fPulseInt_quad[1][ipmt]->Fill(P_ngcer_goodAdcPulseInt[ipmt]) : fPulseInt_quad[1][ipmt]->Fill(P_hgcer_goodAdcPulseInt[ipmt]);

		  //Condition for quadrant 3 mirror
		  if (fNGC ? y_pos >= 0.0 && x_pos < 0.0 : y_pos >= 4.6 && x_pos < 9.4)
		    fNGC ? fPulseInt_quad[2][ipmt]->Fill(P_ngcer_goodAdcPulseInt[ipmt]) : fPulseInt_quad[2][ipmt]->Fill(P_hgcer_goodAdcPulseInt[ipmt]);

		  //Condition for quadrant 4 mirror
		  if (fNGC ? y_pos < 0.0 && x_pos < 0.0 : y_pos < 4.6 && x_pos < 9.4)
		    fNGC ? fPulseInt_quad[3][ipmt]->Fill(P_ngcer_goodAdcPulseInt[ipmt]) : fPulseInt_quad[3][ipmt]->Fill(P_hgcer_goodAdcPulseInt[ipmt]);
		}
	    }//Marks end of pion selection condition
		      
	  //For quadrant cut strategy with no particle ID cut
	  if (!fTrack && !fCut)
	    {
	      //Fill histogram of the full PulseInt spectra for each PMT
	      fNGC ? fPulseInt[ipmt]->Fill(P_ngcer_goodAdcPulseInt[ipmt]) : fPulseInt[ipmt]->Fill(P_hgcer_goodAdcPulseInt[ipmt]);

	      //Retrieve information for particle tracking from focal plane
	      if (!fFullRead) b_P_tr_y->GetEntry(entry), b_P_tr_ph->GetEntry(entry);
	      if (!fFullRead) b_P_tr_x->GetEntry(entry), b_P_tr_th->GetEntry(entry);

	      //Fill histograms of what each PMT registers from each quadrant, this requires tracking the particle from the focal plane. Each quadrant is defined from the parameter files
	      Float_t y_pos = fNGC ? P_tr_y[0] + P_tr_ph[0]*fngc_zpos : P_tr_y[0] + P_tr_ph[0]*fhgc_zpos;
	      Float_t x_pos = fNGC ? P_tr_x[0] + P_tr_th[0]*fngc_zpos : P_tr_x[0] + P_tr_th[0]*fhgc_zpos;
		  
	      //Condition for quadrant 1 mirror
	      if (fNGC ? y_pos >= 0.0 && x_pos >= 0.0 : y_pos >= 4.6 && x_pos >= 9.4)
		fNGC ? fPulseInt_quad[0][ipmt]->Fill(P_ngcer_goodAdcPulseInt[ipmt]) : fPulseInt_quad[0][ipmt]->Fill(P_hgcer_goodAdcPulseInt[ipmt]);

	      //Condition for quadrant 2 mirror
	      if (fNGC ? y_pos < 0.0 && x_pos >= 0.0 : y_pos < 4.6 && x_pos >= 9.4)
		fNGC ? fPulseInt_quad[1][ipmt]->Fill(P_ngcer_goodAdcPulseInt[ipmt]) : fPulseInt_quad[1][ipmt]->Fill(P_hgcer_goodAdcPulseInt[ipmt]);

	      //Condition for quadrant 3 mirror
	      if (fNGC ? y_pos >= 0.0 && x_pos < 0.0 : y_pos >= 4.6 && x_pos < 9.4)
		fNGC ? fPulseInt_quad[2][ipmt]->Fill(P_ngcer_goodAdcPulseInt[ipmt]) : fPulseInt_quad[2][ipmt]->Fill(P_hgcer_goodAdcPulseInt[ipmt]);

	      //Condition for quadrant 4 mirror
	      if (fNGC ? y_pos < 0.0 && x_pos < 0.0 : y_pos < 4.6 && x_pos < 9.4)
		fNGC ? fPulseInt_quad[3][ipmt]->Fill(P_ngcer_goodAdcPulseInt[ipmt]) : fPulseInt_quad[3][ipmt]->Fill(P_hgcer_goodAdcPulseInt[ipmt]);
	    }//Marks end of no particle ID strategy 
		  
	  //For TracksFired cut strategy with no particle ID cut
	  if (fTrack && !fCut)
	    {
	      //Fill histogram of the full PulseInt spectra for each PMT
	      fNGC ? fPulseInt[ipmt]->Fill(P_ngcer_goodAdcPulseInt[ipmt]) : fPulseInt[ipmt]->Fill(P_hgcer_goodAdcPulseInt[ipmt]);

	      //Fill histograms with TracksFired cut, note that quadrant cuts are included
	      if (!fFullRead) fNGC ? b_P_ngcer_numTracksFired->GetEntry(entry) : b_P_hgcer_numTracksFired->GetEntry(entry);
	      for (Int_t iregion = 0; iregion < 4; iregion++)
		{
		  if (fNGC ? P_ngcer_numTracksFired[iregion] == (iregion + 1) : P_hgcer_numTracksFired[iregion] == (iregion + 1))
		    fNGC ? fPulseInt_quad[iregion][ipmt]->Fill(P_ngcer_goodAdcPulseInt[ipmt]) : fPulseInt_quad[iregion][ipmt]->Fill(P_hgcer_goodAdcPulseInt[ipmt]);
		}
	    }//Marks end of tracksfired strategy with no particle ID

	  //For TracksFired cut strategy selecting electrons
	  if (fTrack && fCut && !fPions)
	    {
	      //Retrieve particle ID information
	      if (!fFullRead) b_P_cal_fly_earray->GetEntry(entry);
	      if (!fFullRead) b_P_cal_pr_eplane->GetEntry(entry);
	      if (!fFullRead) b_P_gtr_dp->GetEntry(entry);
	      Float_t central_p = 3.0;
	      Float_t p = ((P_gtr_dp/100.0)*central_p) + central_p;

	      //Fill histogram visualizaing the electron selection
	      fCut_everything->Fill(P_cal_fly_earray/p, P_cal_pr_eplane/p);

	      //Cut on Shower vs preshower is a tilted ellipse, this requires an angle of rotation (in radians), x/y center, semimajor and semiminor axis
	      Float_t eangle = 3.0*3.14159/4;
	      Float_t ex_center = 0.66;
	      Float_t ey_center = 0.35;
	      Float_t esemimajor_axis = 0.28;
	      Float_t esemiminor_axis = 0.04;
	      if (pow((P_cal_fly_earray/p - ex_center)*cos(eangle) + (P_cal_pr_eplane/p - ey_center)*sin(eangle),2)/pow(esemimajor_axis,2) + 
		  pow((P_cal_fly_earray/p - ex_center)*sin(eangle) - (P_cal_pr_eplane/p - ey_center)*cos(eangle),2)/pow(esemiminor_axis,2) < 1)
		{
		  //Fill histogram visualizing the electron selection
		  fCut_electron->Fill(P_cal_fly_earray/p, P_cal_pr_eplane/p);

		  //Fill histogram of the full PulseInt spectra for each PMT
		  fNGC ? fPulseInt[ipmt]->Fill(P_ngcer_goodAdcPulseInt[ipmt]) : fPulseInt[ipmt]->Fill(P_hgcer_goodAdcPulseInt[ipmt]);

		  //Fill histograms with TracksFired cut, note that quadrant cuts are included so any off quadrant histograms will be empty
		  if (!fFullRead) fNGC ? b_P_ngcer_numTracksFired->GetEntry(entry) : b_P_hgcer_numTracksFired->GetEntry(entry);
		  for (Int_t iregion = 0; iregion < 4; iregion++)
		    {
		      if (fNGC ? P_ngcer_numTracksFired[iregion] != 0.0 : P_hgcer_numTracksFired[iregion] != 0.0)
			fNGC ? fPulseInt_quad[iregion][ipmt]->Fill(P_ngcer_goodAdcPulseInt[ipmt]) : fPulseInt_quad[iregion][ipmt]->Fill(P_hgcer_goodAdcPulseInt[ipmt]);
		    }
		}
	    }//Marks end of tracksfired with electrons

	  //For TracksFired cut strategy selecting pions
	  if (fTrack && fCut && fPions)
	    {
	      //Retrieve particle ID information
	      if (!fFullRead) b_P_cal_fly_earray->GetEntry(entry);
	      if (!fFullRead) b_P_cal_pr_eplane->GetEntry(entry);
	      if (!fFullRead) b_P_gtr_dp->GetEntry(entry);
	      Float_t central_p = 3.0;
	      Float_t p = ((P_gtr_dp/100.0)*central_p) + central_p;

	      //Fill histogram visualizaing the electron selection
	      fCut_everything->Fill(P_cal_fly_earray/p, P_cal_pr_eplane/p);

	      //Cut on Shower vs preshower is a tilted ellipse, this requires an angle of rotation (in radians), x/y center, semimajor and semiminor axis
	      Float_t piangle = 0.0;
	      Float_t pix_center = 0.26;
	      Float_t piy_center = 0.03;
	      Float_t pisemimajor_axis = 0.1;
	      Float_t pisemiminor_axis = 0.02;
	      if (pow((P_cal_fly_earray/p - pix_center)*cos(piangle) + (P_cal_pr_eplane/p - piy_center)*sin(piangle),2)/pow(pisemimajor_axis,2) + 
		  pow((P_cal_fly_earray/p - pix_center)*sin(piangle) - (P_cal_pr_eplane/p - piy_center)*cos(piangle),2)/pow(pisemiminor_axis,2) < 1)
		{
		  //Fill histogram visualizing the electron selection
		  fCut_pion->Fill(P_cal_fly_earray/p, P_cal_pr_eplane/p);

		  //Fill histogram of the full PulseInt spectra for each PMT
		  fNGC ? fPulseInt[ipmt]->Fill(P_ngcer_goodAdcPulseInt[ipmt]) : fPulseInt[ipmt]->Fill(P_hgcer_goodAdcPulseInt[ipmt]);

		  //Fill histograms with TracksFired cut, note that quadrant cuts are included
		  if (!fFullRead) fNGC ? b_P_ngcer_numTracksFired->GetEntry(entry) : b_P_hgcer_numTracksFired->GetEntry(entry);
		  for (Int_t iregion = 0; iregion < 4; iregion++)
		    {
		      if (fNGC ? P_ngcer_numTracksFired[iregion] != 0.0 : P_hgcer_numTracksFired[iregion] != 0.0)
			fNGC ? fPulseInt_quad[iregion][ipmt]->Fill(P_ngcer_goodAdcPulseInt[ipmt]) : fPulseInt_quad[iregion][ipmt]->Fill(P_hgcer_goodAdcPulseInt[ipmt]);
		    }
		}
	    }//Marks end of tracksfired with electrons

	}//Marks end of loop over PMTs  

    }//Marks end of loop over tracks
	      
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
  printf("\n");
  Info("Terminate", "'%s' reading", (fFullRead ? "full" : "optimized"));
  Info("Terminate", "calibrating '%s'", (fNGC ? "NGC" : "HGC"));
  Info("Terminate", "'%s' showing", (fFullShow ? "full" : "minimal"));
  Info("Terminate", "'%s' strategy", (fTrack ? "tracking" : "quadrant"));
  Info("Terminate", "cuts %s performed", (fCut ? "are" : "are not"));
  if (fCut) Info("Terminate", "cutting for '%s'", (fPions ? "pions" : "electrons"));
  printf("\n");
  Info("Terminate", "Histograms formed, now starting calibration.\n'Peak Buffer full' is a good warning!\n");
  printf("\n");

  gStyle->SetOptStat(1000000001);

  //Have to extract the histograms from the OutputList
  TH1F* PulseInt[4];
  TH1F* PulseInt_quad[4][4];
  for (Int_t ipmt = 0; ipmt < 4; ipmt++)
    {
      PulseInt[ipmt] = dynamic_cast<TH1F*> (GetOutputList()->FindObject(Form("PulseInt_PMT%d",ipmt+1)));
      for (Int_t iquad = 0; iquad < 4; iquad++)
	{
	  PulseInt_quad[iquad][ipmt] = dynamic_cast<TH1F*> (GetOutputList()->FindObject(Form("PulseInt_quad%d_PMT%d",iquad+1,ipmt+1)));
	}
    }

  //Rebin the histograms, add functionality to bin HGC & NGC independently
  //Not needed since unit conversion into SI, but available in the future
  /*
  for (Int_t ipmt=0; ipmt < (fNGC ? fngc_pmts : fhgc_pmts); ipmt++)
    {
      for (Int_t iquad=0; iquad<4; iquad++)
	{
	  fNGC ? PulseInt_quad[iquad][ipmt]->Rebin(20) : PulseInt_quad[iquad][ipmt]->Rebin(20);
	}
      fNGC ? PulseInt[ipmt]->Rebin(20) : PulseInt[ipmt]->Rebin(20);
    }
*/

  //Show the particle cuts performed in the histogram forming
  if (fCut)
    {
      TCanvas *cut_visualization = new TCanvas("cut_visualization", "Visualization of the particle ID cuts performed");
      cut_visualization->Divide(2,1);
      cut_visualization->cd(1);
      fCut_everything->Draw("Colz");
      cut_visualization->cd(2);
      fPions ? fCut_pion->Draw("Colz") : fCut_electron->Draw("Colz");
    }
 
  gStyle->SetOptFit(111);

  //Single Gaussian to find mean of SPE
  TF1 *Gauss1 = new TF1("Gauss1",gauss,-10,200,3);
  Gauss1->SetParNames("Amplitude","Mean","Std. Dev.");

  //Sum of three Gaussians to determine NPE spacing
  TF1 *Gauss3 = new TF1("Gauss3",gauss,0.5,3.5,9);
  Gauss3->SetParNames("Amplitude 1","Mean 1","Std. Dev. 1","Amplitude 2","Mean 2","Std. Dev. 2","Amplitude 3","Mean 3","Std. Dev. 3");

  //Poisson distribution to remove high NPE background
  TF1 *Poisson = new TF1("Poisson",poisson,0,20,2);
  Poisson->SetParNames("Mean", "Amplitude");

  //Note about Poisson background, the mean varies between detectors/operating conditions so this quantity may require user input
  Double_t Poisson_mean = 0;
  fNGC ? Poisson_mean = 16.0 : Poisson_mean = 5.5;  

  //Linear function used to determine goodness-of-fit for NPE spacing
  TF1 *Linear = new TF1("Linear",linear,0,4,2);
  Linear->SetParNames("Slope", "Intercept");
      
  //An array is used to store the means for the SPE, and to determine NPE spacing
  Double_t mean[3];
  Double_t x_npe[3], y_npe[3], x_err[3], y_err[3];
      
  //Two more arrays are used to store the estimates for the calibration constants and another two to store goodness of calibration
  Double_t calibration_mk1[4], calibration_mk2[4], pmt_calib[4], pmt_calib_mk2[4];

  //Array to hold the Poisson character of the calibrations
  Double_t Pois_Chi[2];
  Pois_Chi[0] = 0.0, Pois_Chi[1] = 0.0;

  gStyle->SetOptFit(111);

  //Main loop for calibration
  for (Int_t ipmt=0; ipmt < (fNGC ? fngc_pmts : fhgc_pmts); ipmt++)
    {
      //Initialize the various arrays (calibration arrays are explicitly filled)
      for (Int_t i=0; i<3; i++)
	{
	  mean[i] = 0.0;
	  x_npe[i] = 0, y_npe[i] = 0, x_err[i] = 0, y_err[i] = 0;
	}

      //Begin strategy for quadrant cut calibration
      if (!fTrack)
	{
	  //TSpectrum class is used to find the SPE peak using the search method
	  TSpectrum *s = new TSpectrum(2);  

	  //Create Canvas to see the search result for the SPE  
	  if (fFullShow) quad_cuts_ipmt = new TCanvas(Form("quad_cuts_%d",ipmt), Form("First Photoelectron peaks PMT%d",ipmt+1));
	  if (fFullShow) quad_cuts_ipmt->Divide(3,1);  
	  
	  Int_t ipad = 1; //Variable to draw over pads correctly
	  for (Int_t iquad=0; iquad<4; iquad++)
	    {
	      if (iquad == ipmt) continue; //ignore a PMT looking at its own quadrant
	      if (fFullShow) quad_cuts_ipmt->cd(ipad);

	      //Perform search for the SPE and save the peak into the array xpeaks
	      fFullShow ? s->Search(PulseInt_quad[iquad][ipmt], 2.0, "nobackground", 0.001) : s->Search(PulseInt_quad[iquad][ipmt], 2.5, "nobackground&&nodraw", 0.001);
	      TList *functions = PulseInt_quad[iquad][ipmt]->GetListOfFunctions();
	      TPolyMarker *pm = (TPolyMarker*)functions->FindObject("TPolyMarker");
	      Double_t *xpeaks = pm->GetX();
	      if (xpeaks[1] < xpeaks[0]) xpeaks[1] = xpeaks[0];

	      //Use the peak to fit the SPE with a Gaussian to determine the mean
	      Gauss1->SetRange(xpeaks[1]-5, xpeaks[1]+5);
	      Gauss1->SetParameter(1, xpeaks[1]);
	      Gauss1->SetParameter(2, 10.);
	      Gauss1->SetParLimits(0, 0., 2000.);
	      Gauss1->SetParLimits(1, xpeaks[1]-10, xpeaks[1]+10);
	      Gauss1->SetParLimits(2, 0.5, 10.);
	      fFullShow ? PulseInt_quad[iquad][ipmt]->Fit("Gauss1","RQ") : PulseInt_quad[iquad][ipmt]->Fit("Gauss1","RQN");

	      //Store the mean of the SPE in the mean array provided it is not zero and passes a loose statistical cut. Note that indexing by ipad-1 is for convienience 
	      if (xpeaks[1] > 2.0 && PulseInt_quad[iquad][ipmt]->GetBinContent(PulseInt_quad[iquad][ipmt]->GetXaxis()->FindBin(xpeaks[1])) > 90) mean[ipad-1] = Gauss1->GetParameter(1); 
	      ipad++;
	    }
	  
	  //Obtain the conversion from ADC to NPE by taking the average of the SPE means
	  Double_t xscale = 0.0;
	  Double_t num_peaks = 0.0;
	  for (Int_t i=0; i<3; i++)
	    {
	      if (mean[i] == 0.0) continue;
	      xscale += mean[i];
	      num_peaks += 1.0;
	    }
	  if (num_peaks != 0.0) xscale = xscale/num_peaks;

	  //Perform check if the statistics were too low to get a good estimate of the SPE mean
	  if (xscale < 1.0)
	    {
	      //Repeat the exact same procedure for the SPE of each quadrant, except now its for the full PMT spectra
	      if(fFullShow) low_stats_ipmt = new TCanvas(Form("low_stats_%d",ipmt),Form("Low stats analysis for PMT%d",ipmt+1));
	      if(fFullShow) low_stats_ipmt->cd(1);
	      PulseInt[ipmt]->GetXaxis()->SetRangeUser(0,20);
	      fFullShow ? s->Search(PulseInt[ipmt], 3.5, "nobackground", 0.001) : s->Search(PulseInt[ipmt], 2.0, "nobackground&&nodraw", 0.001);
	      TList *functions = PulseInt[ipmt]->GetListOfFunctions();
	      TPolyMarker *pm = (TPolyMarker*)functions->FindObject("TPolyMarker");
	      Double_t *xpeaks = pm->GetX();
	      Gauss1->SetRange(xpeaks[1]-5, xpeaks[1]+5);
	      Gauss1->SetParameter(1, xpeaks[1]);
	      Gauss1->SetParameter(2, 10.);
	      Gauss1->SetParLimits(0, 0., 2000.);
	      Gauss1->SetParLimits(1, xpeaks[1]-5, xpeaks[1]+5);
	      Gauss1->SetParLimits(2, 0.5, 20.);
	      PulseInt[ipmt]->GetXaxis()->SetRangeUser(-10,200);
	      fFullShow ? PulseInt[ipmt]->Fit("Gauss1","RQ") : PulseInt[ipmt]->Fit("Gauss1","RQN");
	      xscale = Gauss1->GetParameter(1);
	    }	  
	  //Scale full ADC spectra according to the mean of the SPE. This requires filling a new histogram with the same number of bins but scaled min/max
	  Int_t nbins;
	  nbins = (PulseInt[ipmt]->GetXaxis()->GetNbins());

	  //With the scale of ADC to NPE create a histogram that has the conversion applied
	  fscaled[ipmt] = new TH1F(Form("fscaled_PMT%d", ipmt+1), Form("Scaled ADC spectra for PMT%d; NPE; Normalized Counts",ipmt+1), fNGC ? 210 : 210, -1, fNGC ? 20 : 20);
	  
	  //Fill this histogram bin by bin
	  for (Int_t ibin=0; ibin<nbins; ibin++)
	    {
	      Double_t y = PulseInt[ipmt]->GetBinContent(ibin);
	      Double_t x = PulseInt[ipmt]->GetXaxis()->GetBinCenter(ibin);
	      Double_t x_scaled = x/xscale;
	      Int_t bin_scaled = fscaled[ipmt]->GetXaxis()->FindBin(x_scaled); 
	      fscaled[ipmt]->SetBinContent(bin_scaled,y);
	    }

	  //Normalize the histogram for ease of fitting
	  fscaled[ipmt]->Scale(1.0/fscaled[ipmt]->Integral(), "width");

	  //Begin the removal of the Poisson-like background
	  if (fFullShow) background_ipmt = new TCanvas(Form("backgrounf_pmt%d",ipmt), Form("NPE spectra for PMT%d with Poisson-like background",ipmt+1));
	  if (fFullShow) background_ipmt->cd(1);
	  Poisson->SetParameter(0, 6.5);
	  Poisson->SetParameter(1, 0.25);
	  Poisson->SetParLimits(0, 6.0, 8.0);
	  fFullShow ? fscaled[ipmt]->Fit("Poisson","RQ") : fscaled[ipmt]->Fit("Poisson","RQN");

	  //Make and fill histogram with the background removed
	  fscaled_nobackground[ipmt] = new TH1F(Form("fscaled_nobackground_pmt%d", ipmt+1), Form("NPE spectra background removed for PMT%d; NPE; Normalized Counts",ipmt+1), fNGC ? 210 : 210, -1, fNGC ? 20 : 20);

	  for (Int_t ibin=0; ibin<nbins; ibin++)
	    {
	      Double_t y = Poisson->Eval(fscaled[ipmt]->GetXaxis()->GetBinCenter(ibin));
	      Double_t y2 = fscaled[ipmt]->GetBinContent(ibin) - y;
	      fscaled_nobackground[ipmt]->SetBinContent(ibin,y2);
	    }

	  if (fFullShow) final_spectra_ipmt = new TCanvas(Form("final_Spectra_%d",ipmt), Form("NPE spectra Background Removed for PMT%d",ipmt+1));
	  if (fFullShow) final_spectra_ipmt->Divide(2,1);
	  if (fFullShow) final_spectra_ipmt->cd(1);
	  Gauss3->SetParameters(0.08, 1.0, 0.22, 0.029, 2, 0.5, 0.15, 3, 0.26);
	  Gauss3->SetParLimits(1, 0.5, 1.5);
	  Gauss3->SetParLimits(2, 0.0, 1.0);
	  Gauss3->SetParLimits(3, 0.0, 1.0);
	  Gauss3->SetParLimits(4, 1.5, 2.5);
	  Gauss3->SetParLimits(5, 0.2, 0.6);
	  Gauss3->SetParLimits(6, 0.0, 1.0);
	  Gauss3->SetParLimits(7, 2.5, 3.5);
	  Gauss3->SetParLimits(8, 0.2, 0.5);
	  fFullShow ? fscaled_nobackground[ipmt]->Fit("Gauss3","RQ") : fscaled_nobackground[ipmt]->Fit("Gauss3","RQN");
	  if (fFullShow) fscaled_nobackground[ipmt]->GetXaxis()->SetRangeUser(0,5);
	  if (fFullShow) fscaled_nobackground[ipmt]->GetYaxis()->SetRangeUser(0,0.2);

	  //Create a TGraphErrors to determine the spacing of the NPE
	  y_npe[0] = Gauss3->GetParameter(1), y_npe[1] = Gauss3->GetParameter(4), y_npe[2] = Gauss3->GetParameter(7);
	  y_err[0] = Gauss3->GetParError(1), y_err[1] = Gauss3->GetParError(4), y_err[2] = Gauss3->GetParError(7);
	  x_npe[0] = 1, x_npe[1] = 2, x_npe[2] = 3;
	  TGraphErrors *gr_npe = new TGraphErrors(3, x_npe, y_npe, x_err, y_err);
	  gr_npe->SetTitle(Form("Linear Spacing of PE for PMT%d",ipmt+1));
	  gr_npe->GetXaxis()->SetTitle("NPE number");
	  gr_npe->GetYaxis()->SetTitle("Photoelectron peak (NPE)");

	  //Plot this graph with the NPE spectra
	  if (fFullShow) final_spectra_ipmt->cd(2);
	  Linear->SetParameters(1.0, 0.0);
	  fFullShow ? gr_npe->Fit("Linear","RQ") : gr_npe->Fit("Linear","RQN");
	  if (fFullShow) gr_npe->Draw("A*");
	  calibration_mk1[ipmt] = xscale;
	  pmt_calib[ipmt] = abs(1.0 - Gauss3->GetParameter(1));
      
	  //Initial calibration constant has been obtained. Now I multiply it by the slope of the spacing of the NPE (should be approx. 1) for a second estimate

	  Double_t xscale_mk2 = xscale * Gauss3->GetParameter(1);

	  //Take this new xscale and repeat the exact same procedure as before
	  fscaled_mk2[ipmt] = new TH1F(Form("fhgc_scaled_mk2_PMT%d", ipmt+1), Form("Scaled ADC spectra for PMT%d; NPE; Normalized Counts",ipmt+1), fNGC ? 210 : 210, -1, fNGC ? 20 : 20);
	  
	  //Fill this histogram bin by bin
	  for (Int_t ibin=0; ibin<nbins; ibin++)
	    {
	      Double_t y = PulseInt[ipmt]->GetBinContent(ibin);
	      Double_t x = PulseInt[ipmt]->GetXaxis()->GetBinCenter(ibin);
	      Double_t x_scaled = x/xscale_mk2;
	      Int_t bin_scaled = fscaled_mk2[ipmt]->GetXaxis()->FindBin(x_scaled); 
	      fscaled_mk2[ipmt]->SetBinContent(bin_scaled,y);
	    }

	  //Normalize the histogram for ease of fitting
	  fscaled_mk2[ipmt]->Scale(1.0/fscaled_mk2[ipmt]->Integral(), "width");

	  //Begin the removal of the Poisson-like background
	  if (fFullShow) background_mk2_ipmt = new TCanvas(Form("background_mk2_pmt%d",ipmt), Form("NPE spectra for PMT%d with Poisson-like background",ipmt+1));
	  if (fFullShow) background_mk2_ipmt->cd(1);
	  Poisson->SetParameter(0, 6.5);
	  Poisson->SetParameter(1, 0.25);
	  Poisson->SetParLimits(0, 6.0, 8.0);
	  fFullShow ? fscaled_mk2[ipmt]->Fit("Poisson","RQ"):fscaled_mk2[ipmt]->Fit("Poisson","RQN");

	  //Make and fill histogram with the background removed
	  fscaled_mk2_nobackground[ipmt] = new TH1F(Form("fscaled_mk2_nobackground_pmt%d", ipmt+1), Form("NPE spectra background removed for PMT%d; NPE; Normalized Counts",ipmt+1), fNGC ? 210 : 210, -1, fNGC ? 20 : 20);

	  for (Int_t ibin=0; ibin<nbins; ibin++)
	    {
	      Double_t y = Poisson->Eval(fscaled_mk2[ipmt]->GetXaxis()->GetBinCenter(ibin));
	      Double_t y2 = fscaled_mk2[ipmt]->GetBinContent(ibin) - y;
	      fscaled_mk2_nobackground[ipmt]->SetBinContent(ibin,y2);
	    }

	  if (fFullShow) final_spectra_mk2_ipmt = new TCanvas(Form("final_Spectra_mk2_%d",ipmt), Form("NPE spectra Background Removed for PMT%d",ipmt+1));
	  if (fFullShow) final_spectra_mk2_ipmt->Divide(2,1);
	  if (fFullShow) final_spectra_mk2_ipmt->cd(1);
	  Gauss3->SetParameters(0.08, 1.0, 0.22, 0.029, 2, 0.5, 0.15, 3, 0.26);
	  Gauss3->SetParLimits(1, 0.5, 1.5);
	  Gauss3->SetParLimits(2, 0.0, 1.0);
	  Gauss3->SetParLimits(3, 0.0, 1.0);
	  Gauss3->SetParLimits(4, 1.5, 2.5);
	  Gauss3->SetParLimits(5, 0.2, 0.6);
	  Gauss3->SetParLimits(6, 0.0, 1.0);
	  Gauss3->SetParLimits(7, 2.5, 3.5);
	  Gauss3->SetParLimits(8, 0.2, 0.5);
	  fFullShow ? fscaled_mk2_nobackground[ipmt]->Fit("Gauss3","RQ") : fscaled_mk2_nobackground[ipmt]->Fit("Gauss3","RQN");
	  if (fFullShow) fscaled_mk2_nobackground[ipmt]->GetXaxis()->SetRangeUser(0,5);
	  if (fFullShow) fscaled_mk2_nobackground[ipmt]->GetYaxis()->SetRangeUser(0,0.2);

	  //Create a TGraphErrors to determine the spacing of the NPE
	  y_npe[0] = Gauss3->GetParameter(1), y_npe[1] = Gauss3->GetParameter(4), y_npe[2] = Gauss3->GetParameter(7);
	  y_err[0] = Gauss3->GetParError(1), y_err[1] = Gauss3->GetParError(4), y_err[2] = Gauss3->GetParError(7);
	  x_npe[0] = 1, x_npe[1] = 2, x_npe[2] = 3;
	  TGraphErrors *gr_npe_mk2 = new TGraphErrors(3, x_npe, y_npe, x_err, y_err);
	  gr_npe_mk2->GetXaxis()->SetTitle("NPE number");
	  gr_npe_mk2->GetYaxis()->SetTitle("Photoelectron peak (NPE)");

	  //Plot this graph with the NPE spectra
	  if (fFullShow) final_spectra_mk2_ipmt->cd(2);
	  Linear->SetParameters(1.0, 0.0);
	  fFullShow ? gr_npe_mk2->Fit("Linear","RQ") : gr_npe_mk2->Fit("Linear","RQN");
	  if (fFullShow) gr_npe_mk2->Draw("A*");
	  calibration_mk2[ipmt] = xscale_mk2;
	  pmt_calib_mk2[ipmt] = abs(1.0 - Gauss3->GetParameter(1));
	} // This brance marks the end of the quadrant cut strategy


      

      //Begin the TrackFired cut calibration
      if (fTrack)
	{
	  //TSpectrum class is used to find the SPE peak using the search method
	  TSpectrum *s = new TSpectrum(2); 

	  //Create Canvas to show the search result for the SPE
	  if (fFullShow) quad_cuts_ipmt = new TCanvas(Form("quad_cuts_%d",ipmt), Form("First Photoelectron peaks PMT%d",ipmt+1));
	  if (fFullShow) quad_cuts_ipmt->Divide(2,2);
	  //if (fFullShow) quad_cuts_ipmt->cd(1);

	  for (Int_t iquad = 0; iquad < 4; iquad++)
	    {
	      //Perform search for the SPE and save the peak into the array xpeaks
	      if (fFullShow) quad_cuts_ipmt->cd(iquad+1);

	      //fNGC ? PulseInt_quad[iquad][ipmt]->GetXaxis()->SetRangeUser(150,2000) : PulseInt_quad[ipmt][ipmt]->GetXaxis()->SetRangeUser(5,20);
	      fFullShow ? s->Search(PulseInt_quad[iquad][ipmt], 2.0, "nobackground", 0.001) : s->Search(PulseInt_quad[iquad][ipmt], 2.0, "nobackground&&nodraw", 0.001);
	      TList *functions = PulseInt_quad[iquad][ipmt]->GetListOfFunctions();
	      TPolyMarker *pm = (TPolyMarker*)functions->FindObject("TPolyMarker");
	      Double_t *xpeaks = pm->GetX();

	      //Use the peak to fit the SPE with a Gaussian to determine the mean
	      Gauss1->SetRange(xpeaks[1]-3, xpeaks[1]+3);
	      Gauss1->SetParameter(1, xpeaks[1]);
	      Gauss1->SetParameter(2, 10.);
	      Gauss1->SetParLimits(0, 0., 2000.);
	      Gauss1->SetParLimits(1, xpeaks[1]-3, xpeaks[1]+3);
	      Gauss1->SetParLimits(2, 0.5, 10.);
	      //PulseInt_quad[iquad][ipmt]->GetXaxis()->SetRangeUser(5,20);
	      fFullShow ? PulseInt_quad[iquad][ipmt]->Fit("Gauss1","RQ") : PulseInt_quad[iquad][ipmt]->Fit("Gauss1","RQN");

	      //Store the mean of the SPE in the mean array provided it is not zero, passes a loose statistical cut, and is above a minimum channel number
	      //Added condition that iquad != ipmt since spectrum is quite different
	      if (xpeaks[1] != 0.0 && PulseInt_quad[iquad][ipmt]->GetBinContent(PulseInt_quad[iquad][ipmt]->GetXaxis()->FindBin(xpeaks[1])) > 50 && iquad != ipmt) mean[iquad] = Gauss1->GetParameter(1);
	    }
	  
	  Double_t xscale = 0.0;
	  Double_t num_peaks = 0.0;
	  for (Int_t i = 0; i < 3; i++)
	    {
	      if (mean[i] == 0.0) continue;
	      xscale += mean[i];
	      num_peaks += 1.0;
	    }

	  calibration_mk1[ipmt] = xscale/num_peaks;

	  //Scale full ADC spectra according to the mean of the SPE. This requires filling a new histogram with the same number of bins but scaled min/max
	  Int_t nbins;
	  nbins = PulseInt_quad[ipmt][ipmt]->GetXaxis()->GetNbins();

	  fscaled[ipmt] = new TH1F(Form("fscaled_PMT%d", ipmt+1), Form("Scaled ADC spectra for PMT%d; NPE; Normalized Counts",ipmt+1), 210, -1, fNGC ? 20 : 20);

	  //Fill this histogram bin by bin
	  for (Int_t ibin=0; ibin<nbins; ibin++)
	    {
	      Double_t y = PulseInt_quad[ipmt][ipmt]->GetBinContent(ibin);
	      Double_t x = PulseInt_quad[ipmt][ipmt]->GetXaxis()->GetBinCenter(ibin);
	      Double_t x_scaled = x/calibration_mk1[ipmt];
	      Int_t bin_scaled = fscaled[ipmt]->GetXaxis()->FindBin(x_scaled); 
	      fscaled[ipmt]->SetBinContent(bin_scaled,y);
	    }

	  //Normalize the histogram for ease of fitting
	  fscaled[ipmt]->Scale(1.0/fscaled[ipmt]->Integral(), "width");
	  
	  if (fFullShow) final_spectra_ipmt = new TCanvas(Form("final_Spectra_%d",ipmt), Form("Calibrated spectra for PMT%d; NPE; Normalized Counts",ipmt+1));
	  if (fFullShow) final_spectra_ipmt->cd(1);

	  //Find the location of the SPE and subtract from 1.0 to determine accuracy of calibration
	  Gauss1->SetRange(0.50, 2.0);
	  Gauss1->SetParameter(0, 0.05);
	  Gauss1->SetParameter(1, 1.0);
	  Gauss1->SetParameter(2, 0.3);
	  Gauss1->SetParLimits(0, 0.0, 1.0);
	  Gauss1->SetParLimits(1, 0.5, 1.5);
	  Gauss1->SetParLimits(2, 0.1, 0.5);
	  fFullShow ? fscaled[ipmt]->Fit("Gauss1","RQ") : fscaled[ipmt]->Fit("Gauss1","RQN");
			   
	  calibration_mk2[ipmt] = calibration_mk1[ipmt]*Gauss1->GetParameter(1);
	  pmt_calib[ipmt] = abs(1.0 - Gauss1->GetParameter(1));

	  //Scale full ADC spectra according to the mean of the SPE. This requires filling a new histogram with the same number of bins but scaled min/max
	  fscaled_mk2[ipmt] = new TH1F(Form("fscaled_mk2_PMT%d", ipmt+1), Form("Scaled ADC spectra for PMT%d; NPE; Normalized Counts",ipmt+1), 210, -1, fNGC ? 20 : 20);

	  //Fill this histogram bin by bin
	  for (Int_t ibin=0; ibin<nbins; ibin++)
	    {
	      Double_t y = PulseInt_quad[ipmt][ipmt]->GetBinContent(ibin);
	      Double_t x = PulseInt_quad[ipmt][ipmt]->GetXaxis()->GetBinCenter(ibin);
	      Double_t x_scaled = x/calibration_mk2[ipmt];
	      Int_t bin_scaled = fscaled_mk2[ipmt]->GetXaxis()->FindBin(x_scaled); 
	      fscaled_mk2[ipmt]->SetBinContent(bin_scaled,y);
	    }

	  //Normalize the histogram for ease of fitting
	  fscaled_mk2[ipmt]->Scale(1.0/fscaled_mk2[ipmt]->Integral(), "width");
	  
	  if (fFullShow) final_spectra_mk2_ipmt = new TCanvas(Form("final_Spectra_mk2_%d",ipmt), Form("Calibrated spectra for PMT%d; NPE; Normalized Counts",ipmt+1));
	  if (fFullShow) final_spectra_mk2_ipmt->cd(1);

	  //Find the location of the SPE and subtract from 1.0 to determine accuracy of calibration
	  Gauss1->SetRange(0.50,2.0);
	  Gauss1->SetParameter(0, 0.05);
	  Gauss1->SetParameter(1, 1.0);
	  Gauss1->SetParameter(2, 0.3);
	  Gauss1->SetParLimits(0, 0.0, 0.1);
	  Gauss1->SetParLimits(1, 0.5, 1.5);
	  Gauss1->SetParLimits(2, 0.1, 0.5);
	  fFullShow ? fscaled_mk2[ipmt]->Fit("Gauss1","RQ") : fscaled_mk2[ipmt]->Fit("Gauss1","RQN");

	  pmt_calib_mk2[ipmt] = abs(1.0 - Gauss1->GetParameter(1));

	} //This brace marks the end of TracksFired strategy


      //Begin investigation of Poisson-like behaviour of calibrated spectra..only valid if particle ID is applied
      if (fCut)
	{
	  fscaled_combined[ipmt] = new TH1F(Form("fscaled_combined%d",ipmt+1), Form("Scaled ADC spectra for PMT %d", ipmt+1), 300, 0, fNGC ? 30 : 20);

	  fscaled_combined_mk2[ipmt] = new TH1F(Form("fscaled_combined_mk2%d",ipmt+1), Form("Scaled ADC spectra with Second Calibration for PMT %d", ipmt+1), 300, 0, fNGC ? 30 : 20);
  
	  Int_t nbins = PulseInt[ipmt]->GetXaxis()->GetNbins();
	  Double_t xmean = calibration_mk1[ipmt];
	  Double_t xmean_mk2 = calibration_mk2[ipmt];

	  fscaled_temp[ipmt] = new TH1F(Form("fscaled_temp_pmt%d",ipmt+1), Form("Scaled ADC spectra for PMT %d", ipmt+1), 300, 0, fNGC ? 30 : 20);
	  fscaled_temp_mk2[ipmt] = new TH1F(Form("fscaled_temp_mk2_pmt%d",ipmt+1), Form("Scaled ADC spectra for PMT %d", ipmt+1), 300, 0, fNGC ? 30 : 20);

	  //Fill this histogram bin by bin
	  for (Int_t ibin=0; ibin < nbins; ibin++)
	    {
	      Double_t y = PulseInt[ipmt]->GetBinContent(ibin);
	      Double_t x = PulseInt[ipmt]->GetXaxis()->GetBinCenter(ibin);
	      Double_t x_scaled_mk1 = x/xmean;
	      Double_t x_scaled_mk2 = x/xmean_mk2;
	      Int_t bin_scaled_mk1 = fscaled_temp[ipmt]->GetXaxis()->FindBin(x_scaled_mk1); 
	      Int_t bin_scaled_mk2 = fscaled_temp_mk2[ipmt]->GetXaxis()->FindBin(x_scaled_mk2);
	      fscaled_temp[ipmt]->SetBinContent(bin_scaled_mk1,y);
	      fscaled_temp_mk2[ipmt]->SetBinContent(bin_scaled_mk2,y);
	    }
	  fscaled_combined[ipmt]->Add(fscaled_temp[ipmt]);
	  fscaled_combined_mk2[ipmt]->Add(fscaled_temp_mk2[ipmt]);	

	  //Normalize the histogram for ease of fitting
	  fscaled_combined[ipmt]->Scale(1.0/fscaled_combined[ipmt]->Integral(), "width");
	  fscaled_combined_mk2[ipmt]->Scale(1.0/fscaled_combined[ipmt]->Integral(), "width");
	}

    } // This brace marks the end of the loop over PMTs

  //Combine each PMT into one final histogram
  if (fCut)
    {
      fscaled_total = new TH1F("fscaled_total", "Scaled ADC spectra for all PMTs;NPE;Normalized Counts", 300, 0, fNGC ? 30 : 20);
      fscaled_total_mk2 = new TH1F("fscaled_total_mk2", "Scaled ADC spectra for all PMTs;NPE;Normalized Counts", 300, 0, fNGC ? 30 : 20);
      for (Int_t i=0; i<4; i++)
	{
	  fscaled_total->Add(fscaled_combined[i]);
	  fscaled_total_mk2->Add(fscaled_combined_mk2[i]);
	}

      fscaled_total->Scale(1.0/fscaled_total->Integral(), "width");
      fscaled_total_mk2->Scale(1.0/fscaled_total_mk2->Integral(), "width");

      //Display the Poisson characteristics of the ADC spectra
      if (fFullShow) scaled_total = new TCanvas("scaled_total", "Scaled ADC of all PMTs showing Poisson Fit");
      if (fFullShow) scaled_total->Divide(2,1);
      if (fFullShow) scaled_total->cd(1);
      Poisson->SetRange(0, fNGC ? 30 : 20);
      Poisson->SetParameter(0, Poisson_mean);
      Poisson->SetParameter(1, 0.8);
      Poisson->SetParLimits(0, Poisson_mean - 1.0, Poisson_mean + 3.0);
      fFullShow ? fscaled_total->Fit("Poisson","RQ") : fscaled_total->Fit("Poisson","RQN");
      Pois_Chi[0] = Poisson->GetChisquare();
      if (fFullShow) scaled_total->cd(2);
      fFullShow ? fscaled_total_mk2->Fit("Poisson","RQ") : fscaled_total_mk2->Fit("Poisson","RQN");
      Pois_Chi[1] = Poisson->GetChisquare();
    }

  printf("\n\n");

  //Output the actual calibration information
  cout << "Calibration constants are (where the '*' indicates the better value)\nPMT#: First Guess  Second Guess\n" << endl;
  for (Int_t i=0; i<4; i++)
    {
      cout << Form("PMT%d:", i+1) << setw(8) << Form("%3.3f", calibration_mk1[i]) << (pmt_calib[i] < pmt_calib_mk2[i] ? "*" : " ") << setw(13) << Form("%3.3f", calibration_mk2[i]) << (pmt_calib[i] > pmt_calib_mk2[i] ? "*\n" : "\n");
    }

  printf("\n");

  if (fCut) cout << (Pois_Chi[0] < Pois_Chi[1] ? "First Guess":"Second Guess") << " better characterizes the full Poisson character" << endl;

  //Start the process of writing the calibration information to file
 
  ofstream calibration;
  calibration.open("calibration_temp.txt", ios::out);

  if (!calibration.is_open()) cout << "Problem saving calibration constants, may have to update constants manually!" << endl;

  else
    {
      calibration << "phgcer_adc_to_npe = "; 
      for (Int_t ipmt = 0; ipmt < 4; ipmt++)
	{
	  calibration << Form("1./%3.3f. ", (pmt_calib[ipmt] < pmt_calib_mk2[ipmt]) ? calibration_mk1[ipmt] : calibration_mk2[ipmt]);
	}

      calibration.close();
    }
}
