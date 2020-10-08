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
// Author VIJAY KUMAR

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
#include <TPaveText.h>

using namespace TMath;

void calibration::Begin(TTree * /*tree*/)
{
  // The Begin() function is called at the start of the query.
  // When running with PROOF Begin() is only called on the client.
  // The tree argument is deprecated (on PROOF 0 is passed).
  printf("\n\n");

  TString option = GetOption();
  TString report_option = option(0,option.Length()-79);
  Info("Begin", "Script will fail unless 'calibration.C+' is used");
  Info("Begin", "Starting calibration process with option: %s", report_option.Data());
  Info("Begin", "To see details of calibration, use option showall");
  Info("Begin", "To calibrate using TrackFired leaf, use option trackfired");
  Info("Begin", "Default is no particle cut, use option cut if desired");
  Info("Begin", "Default particle ID is electrons, use option pions if desired");
  printf("\n\n");

  //Check option 
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
  if (option.Contains("showall")) fFullShow = kTRUE;
  if (option.Contains("trackfired")) fTrack = kTRUE;
  if (option.Contains("pions") || option.Contains("pion")) fPions = kTRUE;
  if (option.Contains("cut") || fPions || option.Contains("cuts")) fCut = kTRUE;
  
  Info("SlaveBegin", "'%s' showing", (fFullShow ? "full" : "minimal"));                           
  Info("SlaveBegin", "'%s' strategy", (fTrack ? "tracking" : "quadrant"));
  Info("SlaveBegin", "cuts %s performed", (fCut ? "are" : "are not"));
  if (fCut) Info("SlaveBegin", "cutting for '%s'", (fPions ? "pions" : "electrons"));

  // Inintialize the histograms. Note they are binned per ADC channel which will be changed in the calibration analysis.
  Int_t ADC_min;
  Int_t ADC_max;
  Int_t bins;

  Int_t ADC_poiss_min;
  Int_t ADC_poiss_max;
  Int_t bins_poiss;

  ADC_min = 0;
  ADC_max = 35;
  bins = 2*(abs(ADC_min) + abs(ADC_max));

  ADC_poiss_min = 0;
  ADC_poiss_max = 200;
  bins_poiss = 2*(abs(ADC_poiss_min) + abs(ADC_poiss_max));
  
  

  fPulseInt_poiss = new TH1F*[4];
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

      fPulseInt_poiss[ipmt] = new TH1F(Form("PulseInt_poiss_PMT%d",ipmt+1),Form("Pulse Integral PMT%d; ADC Channel (pC); Counts",ipmt+1), bins_poiss, ADC_poiss_min, ADC_poiss_max);
      GetOutputList()->Add(fPulseInt_poiss[ipmt]);

      for (Int_t iquad = 0; iquad < 4; iquad++)
	{
	  fPulseInt_quad[iquad][ipmt] = new TH1F(Form("PulseInt_quad%d_PMT%d",iquad+1,ipmt+1),Form("Pulse Integral PMT%d quad%d; ADC Channel (pC); Counts",ipmt+1,iquad+1),bins,ADC_min,ADC_max);
	  GetOutputList()->Add(fPulseInt_quad[iquad][ipmt]);
     
	}
    }
  
  // Histograms for Timing info visualizations
 
  fTiming_Full = new TH1F("Timing_Full", "Full timing information for events;Time (ns);Counts", 200, -40, 50);
  GetOutputList()->Add(fTiming_Full);

  fTiming_Cut = new TH1F("Timing_Cut", "Timing cut used for 'good' hits; Time (ns);Counts", 200, -10, 50);
  GetOutputList()->Add(fTiming_Cut);

  //Histograms for timing info for each PTMs

  fTim1 = new TH1F("Timing_PMT1", "ADC TDC Diff PMT1 ; Time (ns) ;Counts", 200, -10.0, 50.0);
  GetOutputList()->Add(fTim1);
  fTim2 = new TH1F("Timing_PMT2", "ADC TDC Diff PMT2 ; Time (ns) ;Counts", 200, -10.0, 50.0);
  GetOutputList()->Add(fTim2);
  fTim3 = new TH1F("Timing_PMT3", "ADC TDC Diff PMT3 ; Time (ns) ;Counts", 200, -10.0, 50.0);
  GetOutputList()->Add(fTim3);
  fTim4 = new TH1F("Timing_PMT4", "ADC TDC Diff PMT4 ; Time (ns) ;Counts", 200, -10.0, 50.0);
  GetOutputList()->Add(fTim4);

  //Histograms for Beta  visualizations

  fBeta_Cut = new TH1F("Beta_Cut", "Beta cut used for 'good' hits;Beta;Counts", 100, -0.1, 1.5);
  GetOutputList()->Add(fBeta_Cut);  
  fBeta_Full = new TH1F("Beta_Full", "Full beta for events;Beta;Counts", 100, -0.1, 1.5);
  GetOutputList()->Add(fBeta_Full);

  //Histograms for Particle ID cut visualization

  fCut_everything = new TH2F("Cut_everything", "Visualization of no cuts; Normalized  Energy; Pre-Shower Energy (GeV)", 250, 0, 1.5, 250, 0, 1.0);
  GetOutputList()->Add(fCut_everything);
  fCut_enorm = new TH1F("Cut_enorm", "Visualization of normalized energy cuts; Normalized Energy; Counts", 200, 0, 2.0);
  GetOutputList()->Add(fCut_enorm);
  fCut_electron = new TH2F("Cut_electron", "Visualization of electron cut; Calorimeter Energy (GeV); Pre-Shower Energy (GeV)", 250, 0, 1.5, 250, 0, 1.0);
  GetOutputList()->Add(fCut_electron);
  fCut_pion = new TH2F("Cut_pion", "Visualization of pion cut; Normalized Energy; Pre-Shower Energy (GeV)", 250, 0, 1.0, 250, 0, 1.0);
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

  fReader.SetEntry(entry);
  
  //Output to verify script is working, and store the total number of events
  if (entry % 100000 == 0) printf("Processing Entry number %lld\n",entry);                          

  //Define quantities to loop over
  Int_t fpmts;
  fpmts = fhgc_pmts;   //Note HGC & NGC have the same # of PMTS

  //Require only one good track reconstruction for the event                         
  if (*Ndata_P_tr_beta != 1) return kTRUE;
  
  //Redundant, but useful if multiple tracks are eventually allowed
  for (Int_t itrack = 0; itrack < *Ndata_P_tr_beta; itrack++) 
    {
      //Require loose cut on particle velocity                                     
      fBeta_Full->Fill(P_tr_beta[itrack]);
      if (TMath::Abs(P_tr_beta[itrack] - 1.0) > 0.4) return kTRUE;
      fBeta_Cut->Fill(P_tr_beta[itrack]);

      //Filling the histograms
      for (Int_t ipmt = 0; ipmt < fpmts; ipmt++) 
	{	  
	  fTiming_Full->Fill(P_hgcer_goodAdcTdcDiffTime[ipmt]);    
   
	  //Perform a loose timing cut    

	  if(ipmt ==0){

	    if(P_hgcer_goodAdcTdcDiffTime[ipmt] >40 || P_hgcer_goodAdcTdcDiffTime[ipmt] < 33) continue;                      //13 9
	    fTim1->Fill(P_hgcer_goodAdcTdcDiffTime[ipmt]);
	  }

	  if(ipmt ==1){

	    if(P_hgcer_goodAdcTdcDiffTime[ipmt] >38 || P_hgcer_goodAdcTdcDiffTime[ipmt] < 31) continue;                          //12 7
	    fTim2->Fill(P_hgcer_goodAdcTdcDiffTime[ipmt]);
      	  }

	  if(ipmt ==2){
	    if(P_hgcer_goodAdcTdcDiffTime[ipmt] >38 || P_hgcer_goodAdcTdcDiffTime[ipmt] < 30) continue;                           //12 
	    fTim3->Fill(P_hgcer_goodAdcTdcDiffTime[ipmt]);
      
	  }
	  if(ipmt ==3){

	    if(P_hgcer_goodAdcTdcDiffTime[ipmt] >39 || P_hgcer_goodAdcTdcDiffTime[ipmt] < 31) continue;                                  //12 8
	    fTim4->Fill(P_hgcer_goodAdcTdcDiffTime[ipmt]);
      
	  }
	  // cut modified by VK, 24/05/19

	  //  if(P_hgcer_goodAdcTdcDiffTime[ipmt] >40 || P_hgcer_goodAdcTdcDiffTime[ipmt] < 30) continue;
	  // fTiming_Cut->Fill(P_hgcer_goodAdcTdcDiffTime[ipmt]);
	   
	  //Cuts to remove entries corresponding to a PMT not registering a hit    
	  if (P_hgcer_goodAdcPulseInt[ipmt] == 0.0) continue;
	 	  
	  //For quadrant cut strategy with particle ID cuts. In this case electrons are selected
	  if (!fTrack && fCut && !fPions)
	    {
	      //Retrieve particle ID information

	      //  Float_t central_p = 8.035;             // old value 6.0530


	      //  Float_t p = ((P_gtr_dp[0]/100.0)*central_p) + central_p;

	      Double_t p = *P_gtr_p;

	      //Fill histogram visualizaing the electron selection
	      fCut_everything->Fill(*P_cal_fly_earray/p, *P_cal_pr_eplane/p);                  
	      fCut_enorm->Fill(*P_cal_etotnorm);

	      //Cut on Shower vs preshower is a tilted ellipse, this requires an angle of rotation (in radians), x/y center, semimajor and semiminor axis
	      //Float_t eangle = 3.0*3.14159/4.0;
	      //Float_t ex_center = 0.66;
	      //Float_t ey_center = 0.35;
	      //Float_t esemimajor_axis = 0.28;
	      //Float_t esemiminor_axis = 0.04;

	      Float_t eangle = 3.0*3.14159/4.0;                             
	      Float_t ex_center = 0.65;                                  // Old value 0.375
	      Float_t ey_center = 0.35;                                 // old value 0.360
	      Float_t esemimajor_axis = 0.30;
	      Float_t esemiminor_axis = 0.08;
	      if (pow((*P_cal_fly_earray/p - ex_center)*cos(eangle) + (*P_cal_pr_eplane/p - ey_center)*sin(eangle),2)/pow(esemimajor_axis,2) + 
		  pow((*P_cal_fly_earray/p - ex_center)*sin(eangle) - (*P_cal_pr_eplane/p - ey_center)*cos(eangle),2)/pow(esemiminor_axis,2) < 1
		  /* P_cal_etotnorm > 0.4*/)
		{
		  //Fill histogram visualizing the electron selection
		  fCut_electron->Fill(*P_cal_fly_earray/p, *P_cal_pr_eplane/p);

		  //Fill histogram of the full PulseInt spectra for each PMT
		  fPulseInt[ipmt]->Fill(P_hgcer_goodAdcPulseInt[ipmt]);
		  fPulseInt_poiss[ipmt]->Fill(P_hgcer_goodAdcPulseInt[ipmt]);

		  //Fill histograms of what each PMT registers from each quadrant, this requires tracking the particle from the focal plane. Each quadrant is defined from the parameter files
		  Float_t y_pos = P_tr_y[0] + P_tr_ph[0]*fhgc_zpos;                              
		  Float_t x_pos = P_tr_x[0] + P_tr_th[0]*fhgc_zpos;
		  
		  //Condition for quadrant 1 mirror
		  if (y_pos >= 4.6 && x_pos >= 9.4) fPulseInt_quad[0][ipmt]->Fill(P_hgcer_goodAdcPulseInt[ipmt]);

		  //Condition for quadrant 2 mirror
		  if (y_pos < 4.6 && x_pos >= 9.4) fPulseInt_quad[1][ipmt]->Fill(P_hgcer_goodAdcPulseInt[ipmt]);

		  //Condition for quadrant 3 mirror
		  if (y_pos >= 4.6 && x_pos < 9.4) fPulseInt_quad[2][ipmt]->Fill(P_hgcer_goodAdcPulseInt[ipmt]);

		  //Condition for quadrant 4 mirror
		  if (y_pos < 4.6 && x_pos < 9.4) fPulseInt_quad[3][ipmt]->Fill(P_hgcer_goodAdcPulseInt[ipmt]);
		}
	    }//Marks end of electron selection condition


	  //For quadrant cut strategy with particle ID cuts. In this case pions are selected
	  if (!fTrack && fCut && fPions)
	    {
	      //Retrieve particle ID information
	      // Float_t central_p = 6.0530;
	      // Float_t p = ((P_gtr_dp[0]/100.0)*central_p) + central_p;  //

	      //Fill histogram visualizaing the pion selection

              Double_t p = *P_gtr_p;

	      fCut_everything->Fill(*P_cal_fly_earray/p, *P_cal_pr_eplane/p);
	      fCut_enorm->Fill(*P_cal_etotnorm);

	      //Cut on Shower vs preshower is a tilted ellipse, this requires an angle of rotation (in radians), x/y center, semimajor and semiminor axis
	      Float_t piangle = 0.0;
	      Float_t pix_center = 0.3;
	      Float_t piy_center = 0.03;
	      Float_t pisemimajor_axis = 0.3;
	      Float_t pisemiminor_axis = 0.02;
	      if (pow((*P_cal_fly_earray/p - pix_center)*cos(piangle) + (*P_cal_pr_eplane/p - piy_center)*sin(piangle),2)/pow(pisemimajor_axis,2) + 
		  pow((*P_cal_fly_earray/p - pix_center)*sin(piangle) - (*P_cal_pr_eplane/p - piy_center)*cos(piangle),2)/pow(pisemiminor_axis,2) < 1)
		{
		  //Fill histogram visualizaing the pion selection
		   fCut_pion->Fill(*P_cal_fly_earray/p, *P_cal_pr_eplane/p);

		  //Fill histogram of the full PulseInt spectra for each PMT
		  fPulseInt[ipmt]->Fill(P_hgcer_goodAdcPulseInt[ipmt]);
		  fPulseInt_poiss[ipmt]->Fill(P_hgcer_goodAdcPulseInt[ipmt]);


		  //Fill histograms of what each PMT registers from each quadrant, this requires tracking the particle from the focal plane. Each quadrant is defined from the parameter files
		  Float_t y_pos = P_tr_y[0] + P_tr_ph[0]*fhgc_zpos;                        
		  Float_t x_pos = P_tr_x[0] + P_tr_th[0]*fhgc_zpos;
		  
		
		  //Condition for quadrant 1 mirror                                                                        
		  if (y_pos >= 4.6 && x_pos >= 9.4) fPulseInt_quad[0][ipmt]->Fill(P_hgcer_goodAdcPulseInt[ipmt]);

		  //Condition for quadrant 2 mirror
		  if (y_pos < 4.6 && x_pos >= 9.4) fPulseInt_quad[1][ipmt]->Fill(P_hgcer_goodAdcPulseInt[ipmt]);

		  //Condition for quadrant 3 mirror
		  if (y_pos >= 4.6 && x_pos < 9.4) fPulseInt_quad[2][ipmt]->Fill(P_hgcer_goodAdcPulseInt[ipmt]);

		  //Condition for quadrant 4 mirror
		  if (y_pos < 4.6 && x_pos < 9.4) fPulseInt_quad[3][ipmt]->Fill(P_hgcer_goodAdcPulseInt[ipmt]);
		}
	    }   
	  //Marks end of pion selection condition
		      
	  //For quadrant cut strategy with no particle ID cut
	   if (!fTrack && !fCut)
	       {
 		 //Fill histogram of the full PulseInt spectra for each PMT
		 fPulseInt[ipmt]->Fill(P_hgcer_goodAdcPulseInt[ipmt]);
		 fPulseInt_poiss[ipmt]->Fill(P_hgcer_goodAdcPulseInt[ipmt]);


		 //Retrieve information for particle tracking from focal plane

		 //Fill histograms of what each PMT registers from each quadrant, this requires tracking the particle from the focal plane. Each quadrant is defined from the parameter files
		 Float_t y_pos = P_tr_y[0] + P_tr_ph[0]*fhgc_zpos;
		 Float_t x_pos = P_tr_x[0] + P_tr_th[0]*fhgc_zpos;
		  
		 //Condition for quadrant 1 mirror
		 if (y_pos >= 4.6 && x_pos >= 9.4) fPulseInt_quad[0][ipmt]->Fill(P_hgcer_goodAdcPulseInt[ipmt]);

		 //Condition for quadrant 2 mirror
		 if (y_pos < 4.6 && x_pos >= 9.4) fPulseInt_quad[1][ipmt]->Fill(P_hgcer_goodAdcPulseInt[ipmt]);

		 //Condition for quadrant 3 mirror
		 if (y_pos >= 4.6 && x_pos < 9.4) fPulseInt_quad[2][ipmt]->Fill(P_hgcer_goodAdcPulseInt[ipmt]);

		 //Condition for quadrant 4 mirror
		 if (y_pos < 4.6 && x_pos < 9.4) fPulseInt_quad[3][ipmt]->Fill(P_hgcer_goodAdcPulseInt[ipmt]);
		 }//Marks end of no particle ID strategy 
	  	  
	      //For TracksFired cut strategy with no particle ID cut
	       if (fTrack && !fCut)
	       {
		 //Fill histogram of the full PulseInt spectra for each PMT
		 fPulseInt[ipmt]->Fill(P_hgcer_goodAdcPulseInt[ipmt]);
		 fPulseInt_poiss[ipmt]->Fill(P_hgcer_goodAdcPulseInt[ipmt]);


		 //Fill histograms with TracksFired cut, note that quadrant cuts are included
		 for (Int_t iregion = 0; iregion < 4; iregion++)
		   {
		     if (P_hgcer_numTracksFired[iregion] == (iregion + 1)) fPulseInt_quad[iregion][ipmt]->Fill(P_hgcer_goodAdcPulseInt[ipmt]);
		   }
	       }//Marks end of tracksfired strategy with no particle ID

	     //For TracksFired cut strategy selecting electrons
	     if (fTrack && fCut && !fPions)
	       {
		 //Retrieve particle ID information
		 // Float_t central_p = 6.0530;
		 // Float_t p = ((P_gtr_dp[0]/100.0)*central_p) + central_p;

		 //Fill histogram visualizaing the electron selection

	      Double_t p = *P_gtr_p;

	      fCut_everything->Fill(*P_cal_fly_earray/p, *P_cal_pr_eplane/p);

	      //Cut on Shower vs preshower is a tilted ellipse, this requires an angle of rotation (in radians), x/y center, semimajor and semiminor axis
	      Float_t eangle = 3.0*3.14159/4;
	      Float_t ex_center = 0.375;
	      Float_t ey_center = 0.360;
	      Float_t esemimajor_axis = 0.38;
	      Float_t esemiminor_axis = 0.05;
	      if (pow((*P_cal_fly_earray/p - ex_center)*cos(eangle) + (*P_cal_pr_eplane/p - ey_center)*sin(eangle),2)/pow(esemimajor_axis,2) + 
		  pow((*P_cal_fly_earray/p - ex_center)*sin(eangle) - (*P_cal_pr_eplane/p - ey_center)*cos(eangle),2)/pow(esemiminor_axis,2) < 1)
		{
		  //Fill histogram visualizing the electron selection
		  fCut_electron->Fill(*P_cal_fly_earray/p, *P_cal_pr_eplane/p);

		  //Fill histogram of the full PulseInt spectra for each PMT
		  fPulseInt[ipmt]->Fill(P_hgcer_goodAdcPulseInt[ipmt]);
		  fPulseInt_poiss[ipmt]->Fill(P_hgcer_goodAdcPulseInt[ipmt]);


		  //Fill histograms with TracksFired cut, note that quadrant cuts are included so any off quadrant histograms will be empty
		  for (Int_t iregion = 0; iregion < 4; iregion++)
		    {
		      if (P_hgcer_numTracksFired[iregion] != 0.0) fPulseInt_quad[iregion][ipmt]->Fill(P_hgcer_goodAdcPulseInt[ipmt]);
		    }
		}
		}//Marks end of tracksfired with electrons

	  //For TracksFired cut strategy selecting pions
	    if (fTrack && fCut && fPions)
	    {
	      //Retrieve particle ID information
	      // Float_t central_p = 6.0530;
	      // Float_t p = ((P_gtr_dp[0]/100.0)*central_p) + central_p;

	      //Fill histogram visualizaing the electron selection

              Double_t p = *P_gtr_p;

	      fCut_everything->Fill(*P_cal_fly_earray/p, *P_cal_pr_eplane/p);

	      //Cut on Shower vs preshower is a tilted ellipse, this requires an angle of rotation (in radians), x/y center, semimajor and semiminor axis
	      Float_t piangle = 0.0;
	      Float_t pix_center = 0.26;
	      Float_t piy_center = 0.03;
	      Float_t pisemimajor_axis = 0.1;
	      Float_t pisemiminor_axis = 0.02;
	      if (pow((*P_cal_fly_earray/p - pix_center)*cos(piangle) + (*P_cal_pr_eplane/p - piy_center)*sin(piangle),2)/pow(pisemimajor_axis,2) + 
		  pow((*P_cal_fly_earray/p - pix_center)*sin(piangle) - (*P_cal_pr_eplane/p - piy_center)*cos(piangle),2)/pow(pisemiminor_axis,2) < 1)
		{
		  //Fill histogram visualizing the electron selection
		  fCut_pion->Fill(*P_cal_fly_earray/p, *P_cal_pr_eplane/p);

		  //Fill histogram of the full PulseInt spectra for each PMT
		  fPulseInt[ipmt]->Fill(P_hgcer_goodAdcPulseInt[ipmt]);
		  fPulseInt_poiss[ipmt]->Fill(P_hgcer_goodAdcPulseInt[ipmt]);


		  //Fill histograms with TracksFired cut, note that quadrant cuts are included
		  for (Int_t iregion = 0; iregion < 4; iregion++)
		    {
		      if (P_hgcer_numTracksFired[iregion] != 0.0) fPulseInt_quad[iregion][ipmt]->Fill(P_hgcer_goodAdcPulseInt[ipmt]);
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
  Info("Terminate", "'%s' showing", (fFullShow ? "full" : "minimal"));
  Info("Terminate", "'%s' strategy", (fTrack ? "tracking" : "quadrant"));
  Info("Terminate", "cuts %s performed", (fCut ? "are" : "are not"));
  if (fCut) Info("Terminate", "cutting for '%s'", (fPions ? "pions" : "electrons"));
  printf("\n");
  Info("Terminate", "Histograms formed, now starting calibration.\n'Peak Buffer full' is a good warning!\n");
  printf("\n");

  //gStyle->SetOptStat(1000000001);
  // Path to save the print pdf file 
  TString foutname = "/u/group/c-kaonlt/USERS/vijay/hallc_replay_lt/CALIBRATION/shms_hgcer_calib/Calibration_plots/Calibration_plots";

  //Print all plots in a single pdf 
  TString outputpdf = foutname + ".pdf";


  //Have to extract the histograms from the OutputList
  TH1F* PulseInt[4];
  TH1F* PulseInt_poiss[4];
  TH1F* PulseInt_quad[4][4];
  for (Int_t ipmt = 0; ipmt < 4; ipmt++)
    {
      PulseInt[ipmt] = dynamic_cast<TH1F*> (GetOutputList()->FindObject(Form("PulseInt_PMT%d",ipmt+1)));
      PulseInt_poiss[ipmt] = dynamic_cast<TH1F*> (GetOutputList()->FindObject(Form("PulseInt_poiss_PMT%d",ipmt+1)));

      for (Int_t iquad = 0; iquad < 4; iquad++)
	{
	  PulseInt_quad[iquad][ipmt] = dynamic_cast<TH1F*> (GetOutputList()->FindObject(Form("PulseInt_quad%d_PMT%d",iquad+1,ipmt+1)));
	} 
    }

  //Rebin the histograms, add functionality to bin HGC & NGC independently
  if (fTrack) {
    for (Int_t ipmt=0; ipmt < (fhgc_pmts); ipmt++)
      {
	for (Int_t iquad=0; iquad<4; iquad++)
	  {
	    PulseInt_quad[iquad][ipmt]->Rebin(4);
	  }
	PulseInt[ipmt]->Rebin(4);
      }
  }
  //Canvases to display cut information
  if (fFullShow)
    {
      //Canvas to show beta cut information
      TCanvas *Beta;
      Beta = new TCanvas("Beta", "Beta information for events");
      Beta->Divide(2,1);
      Beta->cd(1);
      fBeta_Full->Draw(); 
      Beta->cd(2);
      fBeta_Cut->Draw(); 

      Beta->Print(outputpdf + '(');
     
      //Canvas to show full timing  information
      TCanvas *Timing;
      Timing = new TCanvas("Timing", "Timing information for events");
      // Timing->Setlogy();
      gPad->SetLogy();
      fTiming_Full->Draw(); 

      Timing->Print(outputpdf);

      //Canvas to show timing cut info for each PMTs
      TCanvas *Timing1;
      Timing1 = new TCanvas("Timing1","time cuts for each pmts.");      
      Timing1->Divide(2,2);
      Timing1->cd(1);
      fTim1->Draw();     
      Timing1->cd(2);
      fTim2->Draw();    
      Timing1->cd(3);
      fTim3->Draw();      
      Timing1->cd(4);
      fTim4->Draw();   
      Timing1->Print(outputpdf);
    } 

  //Show the particle cuts performed in the histogram forming
  /* if (fCut)
    {
      TCanvas *cut_enorm = new TCanvas("cut_enorm", "Visualization of etotnorm");
      fCut_enorm->Draw();
      TCanvas *cut_visualization = new TCanvas("cut_visualization", "Visualization of the particle ID cuts performed");
      cut_visualization->Divide(2,1);
      cut_visualization->cd(1);
      fCut_everything->Draw("Colz");
      cut_visualization->cd(2);
      fPions ? fCut_pion->Draw("Colz") : fCut_electron->Draw("Colz");
      }*/
   //gStyle->SetOptStat(111);

  //Single Gaussian to find mean of SPE
  TF1 *Gauss1 = new TF1("Gauss1",gauss,100,3,3);
  Gauss1->SetParNames("Amplitude","Mean","Std. Dev.");

  //Sum of two Gaussians to determine SPE with minimal systematics
  TF1 *Gauss2 = new TF1("Gauss2",gauss,0, 20,6);                                               
  Gauss2->SetParNames("Amplitude 1","Mean 1","Std. Dev. 1","Amplitude 2","Mean 2","Std. Dev. 2");

  //Sum of three Gaussians to determine NPE spacing
  TF1 *Gauss3 = new TF1("Gauss3",gauss,20,9,9);
  Gauss3->SetParNames("Amplitude 1","Mean 1","Std. Dev. 1","Amplitude 2","Mean 2","Std. Dev. 2","Amplitude 3","Mean 3","Std. Dev. 3");

  //Poisson distribution to remove high NPE background
  TF1 *Poisson = new TF1("Poisson",poisson,0.0,5.0,2.0);
  Poisson->SetParNames("Mean", "Amplitude");

  // Function used for quality control of the calibration (Root does not allow to write parameter names more than 11) 

  TF1 *Function = new TF1("Function",fun_4gauss_2poisson, 0.0, 18.0 ,16.0);
  //sum_gauss_poisson->SetParNames("Amplitude 1","Mean 1","Std. Dev. 1","Amplitude 2","Mean 2","Std. Dev. 2","Amplitude 3","Mean 3","Std. Dev. 3", "Mean 4", "Amplitude 4");


  //Note about Poisson background, the mean varies between detectors/operating conditions so this quantity may require user input
  Double_t Poisson_mean;
  Poisson_mean = 5.5;  

  //Linear function used to determine goodness-of-fit for NPE spacing
  //TF1 *Linear = new TF1("Linear",linear,0,5,2);
  //  Linear->SetParNames("Slope", "Intercept");
      
  //An array is used to store the means for the SPE, and to determine NPE spacing
  Double_t mean[3];
  Double_t SD[3];
  Double_t mean_err[3];
  Double_t x_npe[3], y_npe[3], x_err[3], y_err[3];
  Double_t RChi2[3];
  Bool_t GoodFit[3];

  //Two more arrays are used to store the estimates for the calibration constants and another two to store goodness of calibration
  Double_t calibration_mk1[4], calibration_mk1Err[4], calibration_mk2[4], calibration_mk2Err[4], pmt_calib[4], pmt_calib_mk2[4];

  TPaveText *GoodFitText = new TPaveText (0.65, 0.15, 0.85, 0.2, "NDC");
  GoodFitText->SetTextColor(kGreen);
  GoodFitText->AddText("Good fit");

  /* TPaveText *BadFitText = new TPaveText (0.65, 0.15, 0.85, 0.2, "NDC");  
  BadFitText->SetTextColor(kBlack);
  BadFitText->AddText("Bad fit");*/

  //TString outputpdf = "PMT_Fits.pdf";                        //Name of the pdf file
  //Array to hold the Poisson character of the calibrations
  Double_t Pois_Chi[2];
  Pois_Chi[0] = 0.0, Pois_Chi[1] = 0.0;

  // gStyle->SetOptFit(1000000001); 
  gStyle->SetOptStat(0); 
  //Main loop for calibration
  for (Int_t ipmt=0; ipmt < (fhgc_pmts); ipmt++)
    {  
      //Initialize the various arrays (calibration arrays are explicitly filled)
      for (Int_t i=0; i<4; i++)
	{
	  mean[i] = 0.0;
	  x_npe[i] = 0, y_npe[i] = 0, x_err[i] = 0, y_err[i] = 0;
	  RChi2[i] = 0;
	} 
      //Begin strategy for quadrant cut calibration
      if (!fTrack)
	{
	  //TSpectrum class is used to find the SPE peak using the search method
	  TSpectrum *s = new TSpectrum(2);  

	  //Create Canvas to see the search result for the SPE  
	  if (fFullShow) quad_cuts[ipmt] = new TCanvas(Form("quad_cuts_%d",ipmt), Form("First Photoelectron peaks PMT%d",ipmt+1));
	  if (fFullShow) quad_cuts[ipmt]->Divide(3,1);  
	  
	  Int_t ipad = 1; //Variable to draw over pads correctly
	 
	  for (Int_t iquad=0; iquad<4; iquad++)
	    { 
	      if (iquad == ipmt) continue; //ignore a PMT looking at its own quadrant
	      if (fFullShow) quad_cuts[ipmt]->cd(ipad);

	      if (PulseInt_quad[iquad][ipmt]->GetEntries() > 0) 
		{		 
		  //Perform search for the SPE and save the peak into the array xpeaks   //0.001
		  fFullShow ? s->Search(PulseInt_quad[iquad][ipmt], 2.0, "nobackground",0.05) : s->Search(PulseInt_quad[iquad][ipmt], 2.0, "nobackground&&nodraw",0.05);
                    
		  TList *functions = PulseInt_quad[iquad][ipmt]->GetListOfFunctions(); 
		  TPolyMarker *pm = (TPolyMarker*)functions->FindObject("TPolyMarker");
		  
		  if ( pm == nullptr)               
		    {
		      cout << "pm is null!!!\n\n ";                                   
		      cout << "ipmt = " << ipmt << " and iquad = " << iquad <<endl;
		      continue;
		    }	 
		  	
		  Double_t k[3];      	 
		  Double_t * xpeaks = pm->GetX();   

		  //If amplitude of second peak then reverse the postions  
		  if (xpeaks[1] < xpeaks[0]) 
 
		    {
		      k[0] =  xpeaks[0]; xpeaks[0] = xpeaks[1]; xpeaks[1] = k[0];
		    }		
	   
		  //Use the peak to fit the SPE with a Gaussian to determine the mean
		  PulseInt_quad[iquad][ipmt]->Draw("E");
		  Gauss2->SetRange(0,16);
                  Gauss2->SetParameter(0, 200);
		  Gauss2->SetParameter(1, xpeaks[0]);
		  Gauss2->SetParameter(2, 5.25);
                  Gauss2->SetParameter(3, 40);
		  Gauss2->SetParameter(4, xpeaks[1]);	
		  Gauss2->SetParameter(5, 2.25);
		  Gauss2->SetParLimits(0, 0.0,PulseInt_quad[iquad][ipmt]->GetBinContent(PulseInt_quad[iquad][ipmt]->GetXaxis()->FindBin(xpeaks[0])));
		  Gauss2->SetParLimits(1, xpeaks[0]-2, xpeaks[0]+2);
		  Gauss2->SetParLimits(2, 0.5, 10.0);
		  Gauss2->SetParLimits(3, 0.0,PulseInt_quad[iquad][ipmt]->GetBinContent(PulseInt_quad[iquad][ipmt]->GetXaxis()->FindBin(xpeaks[1])));
		  Gauss2->SetParLimits(4, xpeaks[1]-0.5, xpeaks[1]+0.5);
		  Gauss2->SetParLimits(5, 0.5, 4.0);
		  fFullShow ? PulseInt_quad[iquad][ipmt]->Fit("Gauss2","RQN") : PulseInt_quad[iquad][ipmt]->Fit("Gauss2","RQN");		
	
		  //Again Use the peak to fit the SPE with a Gaussian to determine the mean
                  Gauss2->SetParameter(0,Gauss2->GetParameter(0));
		  Gauss2->SetParameter(1,Gauss2->GetParameter(1));
		  Gauss2->SetParameter(2,Gauss2->GetParameter(2));
                  Gauss2->SetParameter(3,Gauss2->GetParameter(3));
		  Gauss2->SetParameter(4,Gauss2->GetParameter(4));	
		  Gauss2->SetParameter(5,Gauss2->GetParameter(5));
		  fFullShow ? PulseInt_quad[iquad][ipmt]->Fit("Gauss2","RQ"): PulseInt_quad[iquad][ipmt]->Fit("Gauss2","RQ");
		 
		  // Draw individual functions from the Gauss2 function
		 TF1 *g1 = new TF1("g1","gaus",0,35);

		 if (xpeaks[1] < xpeaks[0]) 
		   {
		     g1->SetParameter(0,Gauss2->GetParameter(3));
		     g1->SetParameter(1,Gauss2->GetParameter(4));
		     g1->SetParameter(2,Gauss2->GetParameter(5));
		     g1->SetLineColor(1);
		     g1->Draw("same");
		   }

		 else 
		   {
		     g1->SetParameter(0,Gauss2->GetParameter(0));
		     g1->SetParameter(1,Gauss2->GetParameter(1));
		     g1->SetParameter(2,Gauss2->GetParameter(2));
		     g1->SetLineColor(1);
		     g1->Draw("same");
		   }

		 TF1 *g2 = new TF1("g2","gaus",0,35);
		 if (xpeaks[1] < xpeaks[0])
		   {
		     g2->SetParameter(0,Gauss2->GetParameter(0));
		     g2->SetParameter(1,Gauss2->GetParameter(1));	
		     g2->SetParameter(2,Gauss2->GetParameter(2));
		     g2->SetParLimits(2, 0.5, 10.0);
		     g2->SetLineColor(3);	      	       
		     g2->Draw("same");
		   }
    
		 else
		   {
		     g2->SetParameter(0,Gauss2->GetParameter(3));
		     g2->SetParameter(1,Gauss2->GetParameter(4));	
		     g2->SetParameter(2,Gauss2->GetParameter(5));
		     g2->SetParLimits(2, 0.5, 10.0);
		     g2->SetLineColor(3);	      	       
		     g2->Draw("same");
		   } 
		 
		 Double_t p0, p0_err, p1, p1_err, p2, p2_err, p3, p3_err, p4, p4_err, p5, p5_err, Chi, NDF;
		 TPaveText *t = new TPaveText(0.45, 0.6, 0.9, 0.9, "NDC");
		 {
		   t->SetTextColor(kBlack);
		   t->AddText(Form(" Chi/NDF     = %3.3f #/ %3.3f", Chi = Gauss2->GetChisquare(), NDF = Gauss2->GetNDF() ));
		   t->AddText(Form(" Amplitude 1     = %3.3f #pm %3.3f", p0 = Gauss2->GetParameter(0), p0_err = Gauss2->GetParError(0)));
		   t->AddText(Form(" Mean 1      = %3.3f #pm %3.3f", p1 = Gauss2->GetParameter(1), p1_err = Gauss2->GetParError(1)));
		   t->AddText(Form(" Std. 1      = %3.3f #pm %3.3f", p2 = Gauss2->GetParameter(2), p2_err = Gauss2->GetParError(2)));
		   t->AddText(Form(" Amplitude 2     = %3.3f #pm %3.3f", p3 = Gauss2->GetParameter(3), p3_err = Gauss2->GetParError(3)));
		   t->AddText(Form(" Mean 2      = %3.3f #pm %3.3f", p4 = Gauss2->GetParameter(4), p4_err = Gauss2->GetParError(4)));
		   t->AddText(Form(" Std. 2      = %3.3f #pm %3.3f", p5 = Gauss2->GetParameter(5), p5_err = Gauss2->GetParError(5)));
		   t->Draw();
		 }

		  if (xpeaks[0] > 2.0 && PulseInt_quad[iquad][ipmt]->GetBinContent(PulseInt_quad[iquad][ipmt]->GetXaxis()->FindBin(xpeaks[0])) > 40) mean[ipad-1] = Gauss2->GetParameter(1); 
		  if (xpeaks[0] > 2.0 && PulseInt_quad[iquad][ipmt]->GetBinContent(PulseInt_quad[iquad][ipmt]->GetXaxis()->FindBin(xpeaks[0])) > 40) SD[ipad-1] = Gauss2->GetParameter(2); 
		  if (xpeaks[0] > 2.0 && PulseInt_quad[iquad][ipmt]->GetBinContent(PulseInt_quad[iquad][ipmt]->GetXaxis()->FindBin(xpeaks[0])) > 40) RChi2[ipad-1] = Gauss2->GetChisquare()/Gauss2->GetNDF(); 
		  if (xpeaks[0] > 2.0 && PulseInt_quad[iquad][ipmt]->GetBinContent(PulseInt_quad[iquad][ipmt]->GetXaxis()->FindBin(xpeaks[0])) > 40) mean_err[ipad-1] = Gauss2->GetParError(1);

<<<<<<< HEAD
		  if (PulseInt_quad[iquad][ipmt]->GetBinContent(PulseInt_quad[iquad][ipmt]->GetXaxis()->FindBin(xpeaks[0])) > 2000)
		  // Set Boolean of whether fit is good or not here
		    {  
		  TPaveText *BadFitText = new TPaveText (0.65, 0.15, 0.85, 0.2, "NDC");  
		  BadFitText->SetTextColor(kRed);
		  BadFitText->AddText("Bad fit");  	  
		  if (RChi2[ipad-1] < 0.5 || RChi2[ipad-1] > 50)
=======

		  if (PulseInt_quad[iquad][ipmt]->GetBinContent(PulseInt_quad[iquad][ipmt]->GetXaxis()->FindBin(xpeaks[0])) > 2000)
	 
		    {	  // Set Boolean of whether fit is good or not here
		    
		  TPaveText *BadFitText = new TPaveText (0.65, 0.15, 0.85, 0.2, "NDC");  
		  BadFitText->SetTextColor(kRed);
		  BadFitText->AddText("Bad fit");  	  
		  if (RChi2[ipad-1] < 0.5 || RChi2[ipad-1] > 30)
>>>>>>> refs/remotes/origin/offline
		    {
		      GoodFit[ipad-1] = kFALSE; 
		      BadFitText->Draw("same");
		    } 
<<<<<<< HEAD
		  else if  (RChi2[ipad-1] > 0.5 && RChi2[ipad-1] < 50) 
		    {
		      GoodFit[ipad-1] = kTRUE;
		      GoodFitText->Draw("same");
		    } 		     	 			  	
		    }

		  else 		    
		    {  
=======
		  else if  (RChi2[ipad-1] > 0.5 && RChi2[ipad-1] < 30) 
		    {
		      GoodFit[ipad-1] = kTRUE;
		      GoodFitText->Draw("same");
		    } 
		    }

		  else
		    {

>>>>>>> refs/remotes/origin/offline
		      TPaveText *BadFitText = new TPaveText (0.65, 0.15, 0.85, 0.2, "NDC");  
		      BadFitText->SetTextColor(kRed);
		      BadFitText->AddText("Bad fit");  	  
		      if (RChi2[ipad-1] < 0.5 || RChi2[ipad-1] > 10)
			{
			  GoodFit[ipad-1] = kFALSE; 
			  BadFitText->Draw("same");
			} 
		      else if  (RChi2[ipad-1] > 0.5 && RChi2[ipad-1] < 10) 
			{
			  GoodFit[ipad-1] = kTRUE;
			  GoodFitText->Draw("same");
<<<<<<< HEAD
			} 		     	 			  	
		    } 
=======
			} 
		    }
			  	
		    
>>>>>>> refs/remotes/origin/offline
		  ipad++;
		 	 
		}
	    } 	      if(fFullShow) quad_cuts[ipmt]->Print(outputpdf);
	          
	  //Obtain the conversion from ADC to NPE by taking the average of the SPE means
	  Double_t xscale = 0.0;
	  Double_t xscaleErr = 0.0;
	  Double_t num_peaks = 0.0;
	  Double_t WeightAvgSum1 = 0.0;
	  Double_t WeightAvgSum2 = 0.0;
	  for (Int_t i=0; i<3; i++)
	    {
	     
	      if (GoodFit[i] == kFALSE) continue;                     // Take weighted avg 26/8/19 SK
	      if (mean_err[i] == 0) continue;                        
	      WeightAvgSum1 += mean[i]/(mean_err[i]*mean_err[i]);                
	      WeightAvgSum2 += 1/(mean_err[i]*mean_err[i]);
	    }

	  xscale = WeightAvgSum1/WeightAvgSum2;
	  xscaleErr = 1/TMath::Sqrt(WeightAvgSum2);    

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
	      Gauss1->SetRange(xpeaks[0]-1, xpeaks[0]+1);
	      Gauss1->SetParameter(1, xpeaks[0]);
	      Gauss1->SetParameter(2, 3);
	      Gauss1->SetParLimits(0, 0., 3000.);
	      //Gauss1->SetParLimits(1, xpeaks[0]-1, xpeaks[0]+1);
	      //Gauss1->SetParLimits(2, 0.5, 20.0);
	      PulseInt[ipmt]->GetXaxis()->SetRangeUser(0,200);
	      fFullShow ? PulseInt[ipmt]->Fit("Gauss1","RQ") : PulseInt[ipmt]->Fit("Gauss1","RQ");
	      xscale = Gauss1->GetParameter(1);
	      xscaleErr = Gauss1->GetParError(1);
	      }	  

	  //Scale full ADC spectra according to the mean of the SPE. This requires filling a new histogram with the same number of bins but scaled min/max
	  Int_t nbins;
	  nbins = (PulseInt_poiss[ipmt]->GetXaxis()->GetNbins());

 	  //With the scale of ADC to NPE create a histogram that has the conversion applied
	  fscaled[ipmt] = new TH1F(Form("fscaled_PMT%d", ipmt+1), Form("Scaled ADC spectra for PMT%d; NPE; Normalized Counts",ipmt+1), nbins,0.0,35.0);
	  //Fill this histogram bin by bin
	  for (Int_t ibin=0; ibin<nbins; ibin++)
	    {
	      Double_t y = PulseInt_poiss[ipmt]->GetBinContent(ibin);
	      Double_t x = PulseInt_poiss[ipmt]->GetXaxis()->GetBinCenter(ibin);
	      Double_t x_scaled = x/xscale;
	      Int_t bin_scaled = fscaled[ipmt]->GetXaxis()->FindBin(x_scaled); 
	      fscaled[ipmt]->SetBinContent(bin_scaled,y);
	    }

	  //Normalize the histogram for ease of fitting
	  fscaled[ipmt]->Scale(1.0/fscaled[ipmt]->Integral(), "width");                         

	  //Tcanvas for draw the  scaled histogram 
	  if (fFullShow) background_ipmt = new TCanvas(Form("backgrounf_pmt%d",ipmt), Form("Full NPE spectra for PMT%d ",ipmt+1));	  
	  if (fFullShow) background_ipmt->cd();	    
   
	  //Poisson distribution 
	  Poisson->SetRange(7,35);
	  Poisson->SetParameter(0,9.0);
	  Poisson->SetParameter(1, 0.40);
	  fFullShow ? fscaled[ipmt]->Fit("Poisson","RQBN") : fscaled[ipmt]->Fit("Poisson","RQBN");

	  // Function included 4gauss + 2poisson for the quality control check 
	  Function->SetRange(0,35.0);
	  Function->SetParameter(0,0.6);
	  Function->SetParameter(1,1.0);	
	  Function->SetParameter(2,0.39);
	  Function->SetParameter(3,0.15);
	  Function->SetParameter(4,2);
	  Function->SetParameter(5,0.24);
	  Function->SetParameter(6,0.12);
	  Function->SetParameter(7,3.0);
	  Function->SetParameter(8,0.75);
	  Function->SetParameter(9,0.12);
	  Function->SetParameter(10,4.0);
	  Function->SetParameter(11,1);
	  Function->SetParameter(12,8.0);
	  Function->SetParameter(13,0.7);
	  Function->SetParameter(14,  Poisson->GetParameter(0));  
	  Function ->SetParameter(15, Poisson->GetParameter(1));
 
	  // Constraints on mean 	 
	  Function->SetParLimits(1, 1 - 2*xscaleErr, 1 + 2*xscaleErr);	
	  Function->SetParLimits(4, 2 - 2*xscaleErr, 2 + 2*xscaleErr);
	  Function->SetParLimits(7, 3 - 2*xscaleErr, 3 + 2*xscaleErr);
	  Function->SetParLimits(10,4 - 2*xscaleErr, 4 + 2*xscaleErr);

	  // Clone the histogram before fit it	 
	  scaled_clone = (TH1F*)fscaled[ipmt]->Clone("scaled_clone");	 

	  fFullShow ? fscaled[ipmt]->Fit("Function","RQN") : fscaled[ipmt]->Fit("Function","RQN");

	  // Again fit the normalised histogram taking parameters from the first fit   
	  Function->SetParameter(0, Function->GetParameter(0));
	  Function->SetParameter(1, Function->GetParameter(1));	
	  Function->SetParameter(2, Function->GetParameter(2));
	  Function->SetParameter(3, Function->GetParameter(3));
	  Function->SetParameter(4, Function->GetParameter(4));
	  Function->SetParameter(5, Function->GetParameter(5));
	  Function->SetParameter(6, Function->GetParameter(6));
	  Function->SetParameter(7, Function->GetParameter(7));
	  Function->SetParameter(8, Function->GetParameter(8));
	  Function->SetParameter(9, Function->GetParameter(9));
	  Function->SetParameter(10,Function->GetParameter(10));
	  Function->SetParameter(11,Function->GetParameter(11));
	  Function->SetParameter(12,Function->GetParameter(12));
	  Function->SetParameter(13,Function->GetParameter(13));
	  Function->SetParameter(14,Function->GetParameter(14));
	  Function->SetParameter(15,Function->GetParameter(15));
	  Function->SetLineColor(5);

	  fFullShow ? fscaled[ipmt]->Fit("Function","RQ+") : fscaled[ipmt]->Fit("Function","RQ+");
	 
	  // Statistics box for display  parameters from the Function fit
	  Double_t  p0, p0_err, p1, p1_err, p2, p2_err, p3, p3_err, p4, p4_err, p5, p5_err, p6, p6_err, p7, p7_err, p8, p8_err,p9,p9_err,p10,p10_err,p11,p11_err,p12, p12_err, p13, p13_err, p14, p14_err, p15, p15_err, Chi, NDF;

          Chi = Function->GetChisquare();
          NDF = Function->GetNDF(); 
	  p0 = Function->GetParameter(0);
	  p0_err = Function->GetParError(0);
	  p1 = Function->GetParameter(1);
	  p1_err = Function->GetParError(1);
	  p2 = Function->GetParameter(2);
	  p2_err = Function->GetParError(2);
	  p3 = Function->GetParameter(3);
	  p3_err = Function->GetParError(3);
	  p4 = Function->GetParameter(4);
	  p4_err = Function->GetParError(4);
	  p5 = Function->GetParameter(5);
	  p5_err = Function->GetParError(5);
	  p6 = Function->GetParameter(6);
	  p6_err = Function->GetParError(6);
	  p7 = Function->GetParameter(7);
	  p7_err = Function->GetParError(7);
	  p8 = Function->GetParameter(8);
	  p8_err = Function->GetParError(8);
	  p9 = Function->GetParameter(9);
	  p9_err = Function->GetParError(9);
	  p10 = Function->GetParameter(10);
	  p10_err = Function->GetParError(10);
	  p11 = Function->GetParameter(11);
	  p11_err = Function->GetParError(11);
	  p12 = Function->GetParameter(12);
	  p12_err = Function->GetParError(12);
	  p13 = Function->GetParameter(13);
	  p13_err = Function->GetParError(13);
	  p14 = Function->GetParameter(14);
	  p14_err = Function->GetParError(14);
	  p15 = Function->GetParameter(15);
	  p15_err = Function->GetParError(15);

	  TPaveText *t1 = new TPaveText(0.70, 0.35, 0.9, 0.9, "NDC");
	  {
	    t1->SetTextColor(kBlack);
	    t1->AddText(Form(" Chi/NDF     = %3.3f #/ %3.3f", Chi, NDF ));
	    t1->AddText(Form(" Mean 1      = %3.3f #pm %3.3f ", p1, p1_err ));
	    t1->AddText(Form(" Std. Dev. 1 = %3.3f #pm %3.3f",  p2, p2_err ));
	    t1->AddText(Form(" Mean 2      = %3.3f #pm %3.3f ", p4, p4_err ));
	    t1->AddText(Form(" Std. Dev. 2 = %3.3f #pm %3.3f ", p5, p5_err ));
	    t1->AddText(Form(" Mean 3      = %3.3f #pm %3.3f ", p7, p7_err ));
	    t1->AddText(Form(" Std. Dev. 3 = %3.3f #pm %3.3f ", p8, p8_err ));
	    t1->AddText(Form(" Mean 4      = %3.3f #pm %3.3f ", p10, p10_err ));
	    t1->AddText(Form(" Std. Dev 4  = %3.3f #pm %3.3f ", p11, p11_err ));	    
	    t1->Draw();
	  }

	  // Draw individual functions from the Fuction fit  

	  TF1 *g3 = new TF1("g3","gaus",0,35.0);
	  {  
	    g3->SetParameter(0,Function->GetParameter(0));
	    g3->SetParameter(1,Function->GetParameter(1));
	    g3->SetParameter(2,Function->GetParameter(2));
	    g3->SetLineColor(1);
	    g3->Draw("same");
	  }

	  TF1 *g4 = new TF1("g4","gaus",0,35.0);
	  {
	    g4->SetParameter(0,Function->GetParameter(3));
	    g4->SetParameter(1,Function->GetParameter(4));
	    g4->SetParameter(2,Function->GetParameter(5));
	    g4->SetLineColor(3);
	    g4->Draw("same");
	  }

	  TF1 *g5 = new TF1("g5","gaus",0,35.0);
	  {
	    g5->SetParameter(0,Function->GetParameter(6));
	    g5->SetParameter(1,Function->GetParameter(7));
	    g5->SetParameter(2,Function->GetParameter(8));
	    g5->SetLineColor(4);
	    g5->Draw("same");
	  }

	  TF1 *g6 = new TF1("g6","gaus",0,35.0);
	  {
	    g6->SetParameter(0,Function->GetParameter(9));
	    g6->SetParameter(1,Function->GetParameter(10));
	    g6->SetParameter(2,Function->GetParameter(11));
	    g6->SetLineColor(2);
	    g6->Draw("same");
	  }
	  TF1 *g7 = new TF1("g7","[1]*TMath::Poisson(x,[0])",0.0,35.0);
	  {
	    g7->SetRange(0,35);
	    g7->SetParameter(0,Function->GetParameter(12));
	    g7->SetParameter(1,Function->GetParameter(13));
	    g7->SetLineColor(6);
	    g7->Draw("same"); 
	  }

	  TF1 *g8 = new TF1("g8","[1]*TMath::Poisson(x,[0])",0.0,35.0);
	  {
	    g8->SetParameter(0,Function->GetParameter(14));
	    g8->SetParameter(1,Function->GetParameter(15));
	    g8->SetLineColor(8);
	    g8->Draw("same"); 
	  }
	   if(fFullShow) background_ipmt->Print(outputpdf);

	   //Tcanvas for draw the zoomed clone histogram 

	   if (fFullShow) Full_zoom_fit_ipmt = new TCanvas(Form("Full_zoom_fit_pmt%d",ipmt), Form("Zoom in NPE spectra for PMT%d with all fits",ipmt+1));
	   if (fFullShow) scaled_clone->GetXaxis()->SetRangeUser(0,5); 
	   if (fFullShow) Full_zoom_fit_ipmt->cd();
	   if (fFullShow) scaled_clone->Draw();

	   TF1 *Function_clone = new TF1("Function_clone",fun_4gauss_2poisson, 0.0, 35.0, 16.0);
	   {
	     Function_clone->SetParameter(0,Function->GetParameter(0));
	     Function_clone->SetParameter(1,Function->GetParameter(1));	
	     Function_clone->SetParameter(2,Function->GetParameter(2));
	     Function_clone->SetParameter(3,Function->GetParameter(3));
	     Function_clone->SetParameter(4,Function->GetParameter(4));
	     Function_clone->SetParameter(5,Function->GetParameter(5));
	     Function_clone->SetParameter(6,Function->GetParameter(6));
	     Function_clone->SetParameter(7,Function->GetParameter(7));
	     Function_clone->SetParameter(8,Function->GetParameter(8));
	     Function_clone->SetParameter(9,Function->GetParameter(9));
	     Function_clone->SetParameter(10,Function->GetParameter(10));
	     Function_clone->SetParameter(11,Function->GetParameter(11));
	     Function_clone->SetParameter(12,Function->GetParameter(12));
	     Function_clone->SetParameter(13,Function->GetParameter(13));
	     Function_clone->SetParameter(14,Function->GetParameter(14));
	     Function_clone->SetParameter(15,Function->GetParameter(15));	   
	     Function_clone->SetLineColor(5);
	     Function_clone->Draw("same");
	   }
	      	  
	   TF1 *g9 = new TF1("g9","gaus",0,35.0);
	   {  
	     g9->SetParameter(0,Function->GetParameter(0));
	     g9->SetParameter(1,Function->GetParameter(1));
	     g9->SetParameter(2,Function->GetParameter(2));
	     g9->SetLineColor(1);
	     g9->Draw("same");}
	   TF1 *g10 = new TF1("g10","gaus",0,35.0);
	   {
	     g10->SetParameter(0,Function->GetParameter(3));
	     g10->SetParameter(1,Function->GetParameter(4));
	     g10->SetParameter(2,Function->GetParameter(5));
	     g10->SetLineColor(3);
	     g10->Draw("same");
	   }
	   TF1 *g11 = new TF1("g11","gaus",0,35.0);
	   {
	     g11->SetParameter(0,Function->GetParameter(6));
	     g11->SetParameter(1,Function->GetParameter(7));
	     g11->SetParameter(2,Function->GetParameter(8));
	     g11->SetLineColor(4);
	     g11->Draw("same");
	   }

	   TF1 *g12 = new TF1("g12","gaus",0,35.0);
	   {
	     g12->SetParameter(0,Function->GetParameter(9));
	     g12->SetParameter(1,Function->GetParameter(10));
	     g12->SetParameter(2,Function->GetParameter(11));
	     g12->SetLineColor(2);
	     g12->Draw("same");
	   }

	   TF1 *g13 = new TF1("g13","[1]*TMath::Poisson(x,[0])",0.0,35.0);

	   {
	     g13->SetRange(0,35);
	     g13->SetParameter(0,Function->GetParameter(12));
	     g13->SetParameter(1,Function->GetParameter(13));
	     g13->SetLineColor(6);
	     g13->Draw("same");
	   }

	   TF1 *g14 = new TF1("g14","[1]*TMath::Poisson(x,[0])",0.0,35.0);

	   {
	     g14->SetParameter(0,Function->GetParameter(14));
	     g14->SetParameter(1,Function->GetParameter(15));
	     g14->SetLineColor(8);
	     g14->Draw("same");
	   } 
	   if(fFullShow) Full_zoom_fit_ipmt->Print(outputpdf);
	
	   //TCanvas for the linear spacing of photo-electrons
	   if (fFullShow) final_spectra_ipmt = new TCanvas(Form("final_Spectra_%d",ipmt), Form("NPE spectra for PMT%d",ipmt+1));
	   y_npe[0] = Function->GetParameter(1), y_npe[1] = Function->GetParameter(4), y_npe[2] = Function->GetParameter(7);
	   y_err[0] = xscaleErr + p1_err, y_err[1] = xscaleErr + p4_err, y_err[2] = xscaleErr + p7_err;
	   x_npe[0] = 1, x_npe[1] = 2, x_npe[2] = 3;

	  TGraphErrors *gr_npe = new TGraphErrors(3, x_npe, y_npe, x_err, y_err);
          gr_npe->GetXaxis()->SetLimits(0, 5);
          gr_npe->SetMinimum(0);
          gr_npe->SetMaximum(5);
	  gr_npe->SetTitle(Form("Linear Spacing of PE for PMT%d",ipmt+1));
	  gr_npe->GetXaxis()->SetTitle("NPE number");
	  gr_npe->GetYaxis()->SetTitle("Photoelectron peak (NPE)");

	  // Fit with linear function when intercept is not zero
	  TF1 *Linear = new TF1("Linear",linear,0,5,2);
	  Linear->SetParNames("Slope", "Intercept");
	  Linear->SetParameter(0.0, 1.0);
          Linear->SetParameter(1.0, 0.0);
	  Linear->SetLineColor(1);

	  if (fFullShow) gr_npe->Draw("A*"); 

	  //Fit with another linear function when intercept is zero
	  TF1 *g15 = new TF1("g15","[0]*x", 0.0,5.0);
	  {
	    g15->SetParNames("Slope");
	    g15->SetParameter(0.0,1.0);
	    g15->SetLineColor(2);
	  }

	  fFullShow ? gr_npe->Fit("Linear","RQ") : gr_npe->Fit("Linear","RQ");
	  fFullShow ? gr_npe->Fit("g15","RQ+") : gr_npe->Fit("g15","RQ+");

	  //Get parameters from both fitting 
	  Double_t sl1, sl1_err, in1, in1_err, Chi1, NDF1, sl2, sl2_err, Chi2, NDF2 ;

          Chi1 = Linear->GetChisquare();
	  NDF1 = Linear->GetNDF(); 
	  sl1 = Linear->GetParameter(0);
	  sl1_err = Linear->GetParError(0);
	  in1 = Linear->GetParameter(1);
	  in1_err = Linear->GetParError(1);
	  
	  // For second linear fit 
          Chi2 = g15->GetChisquare();
	  NDF2 = g15->GetNDF(); 
	  sl2 = g15->GetParameter(0);
	  sl2_err = g15->GetParError(0);

	  TPaveText *t = new TPaveText(0.10, 0.77, 0.40, 0.90, "NDC");
	  {
	    t->SetTextColor(kBlack);
	    t->AddText(Form(" Chi/NDF = %03.3f #/ %3.3f", Chi1, NDF1 ));
	    t->AddText(Form(" Slope = %3.3f #pm %3.3f",sl1, sl1_err));
	    t->AddText(Form(" Intercept = %3.3f #pm %3.3f",in1, in1_err));
	    t->Draw();
	  }	  
	     
	  TPaveText *t2 = new TPaveText(0.10, 0.65, 0.40, 0.78, "NDC");
	  {
	    t2->SetTextColor(kRed);
	    t2->AddText(Form(" Chi/NDF = %3.3f #/ %3.3f", Chi2, NDF2 ));
	    t2->AddText(Form(" Slope = %3.3f #pm %3.3f",sl2, sl2_err));
	    t2->AddText(Form(" Intercept = 0 " ));
	    t2->Draw();
	  }
	  if(ipmt ==0)
	    {
	      if(fFullShow) final_spectra_ipmt->Print(outputpdf);
	    }
	  if(ipmt ==1)
	    {
	      if(fFullShow) final_spectra_ipmt->Print(outputpdf);
	    }
	  if(ipmt ==2)
	    {
	      if(fFullShow) final_spectra_ipmt->Print(outputpdf);
	    } 
	  if(ipmt ==3)
	    {
	      if(fFullShow) final_spectra_ipmt->Print(outputpdf + ')');
	    } 

 	  calibration_mk1[ipmt] = xscale;
	  calibration_mk1Err[ipmt] = xscaleErr;
	  pmt_calib[ipmt] = abs(1.0 - Function->GetParameter(1));

	  if (fCut)
	    {
	      TCanvas *cut_enorm = new TCanvas("cut_enorm", "Visualization of etotnorm");
	      fCut_enorm->Draw();

	      TCanvas *cut_visualization = new TCanvas("cut_visualization", "Visualization of the particle ID cuts performed");
	      cut_visualization->Divide(2,1);
	      cut_visualization->cd(1);
	      fCut_everything->Draw("Colz");
	      cut_visualization->cd(2);
	      fPions ? fCut_pion->Draw("Colz") : fCut_electron->Draw("Colz");
     
	    }

	   //Initial calibration constant has been obtained. Now I multiply it by the slope of the spacing of the NPE (should be approx. 1) for a second estimate

	  Double_t xscale_mk2 = xscale * p1;  

	 Double_t xscale_mk2Err = Sqrt(Power(xscaleErr*p1, 2) +  Power(xscale*p1_err,2)); 
	 // Double_t xscale_mk2Err = (Sqrt(Power(xscaleErr,2) +  Power(Linear->GetParError(0),2))); 
	
	  calibration_mk2[ipmt] = xscale_mk2;
	  calibration_mk2Err[ipmt] = xscale_mk2Err;
	  pmt_calib_mk2[ipmt] = abs(1.0 - Function->GetParameter(1));
				   
	    } // This brace marks the end of the quadrant cut strategy
               

       //Begin the TrackFired cut calibration
       if (fTrack)
	{
	  //TSpectrum class is used to find the SPE peak using the search method
	  TSpectrum *s = new TSpectrum(2); 

	  //Create Canvas to show the search result for the SPE
	  if (fFullShow) quad_cuts_ipmt = new TCanvas(Form("quad_cuts_%d",ipmt), Form("First Photoelectron peaks PMT%d",ipmt+1));
	  if (fFullShow) quad_cuts_ipmt->Divide(2,2);
	  if (fFullShow) quad_cuts_ipmt->cd(1);

	  for (Int_t iquad = 0; iquad < 4; iquad++)
	    {
	      //Perform search for the SPE and save the peak into the array xpeaks
	      if (fFullShow) quad_cuts_ipmt->cd(iquad+1);

	      PulseInt_quad[ipmt][ipmt]->GetXaxis()->SetRangeUser(0,50);
	      fFullShow ? s->Search(PulseInt_quad[iquad][ipmt], 1.0, "nobackground", 0.001) : s->Search(PulseInt_quad[iquad][ipmt], 1.5, "nobackground&&nodraw", 0.001);
	      TList *functions = PulseInt_quad[iquad][ipmt]->GetListOfFunctions();
	      TPolyMarker *pm = (TPolyMarker*)functions->FindObject("TPolyMarker");
	      Double_t *xpeaks = pm->GetX();
	      PulseInt_quad[iquad][ipmt]->GetXaxis()->SetRangeUser(-1,200);

	      //Use the peak to fit the SPE with a Gaussian to determine the mean
	      Gauss1->SetRange(xpeaks[0]-3, xpeaks[0]+3);
	      Gauss1->SetParameter(1, xpeaks[0]);
	      Gauss1->SetParameter(2, 10.);
	      Gauss1->SetParLimits(0, 0., 2000.);
	      Gauss1->SetParLimits(1, xpeaks[0]-3, xpeaks[0]+3);
	      Gauss1->SetParLimits(2, 0.5, 10.);
	      fFullShow ? PulseInt_quad[iquad][ipmt]->Fit("Gauss1","RQ") : PulseInt_quad[iquad][ipmt]->Fit("Gauss1","RQN");
	      //Store the mean of the SPE in the mean array provided it is not zero, passes a loose statistical cut, and is above a minimum channel number
	       if (xpeaks[0] != 0.0 && PulseInt_quad[iquad][ipmt]->GetBinContent(PulseInt_quad[iquad][ipmt]->GetXaxis()->FindBin(xpeaks[0])) > 10 && ipmt != iquad){	
		mean[iquad] = Gauss1->GetParameter(1);
		mean_err[iquad] = Gauss1->GetParError(1);
	      }
	      } 
	  
	  Double_t xscale = 0.0;
	  Double_t xscaleErr = 0.0;
	  Double_t WeightAvgSum1 = 0.0;
	  Double_t WeightAvgSum2 = 0.0;
	   for (Int_t i=0; i<3; i++)
	     {
	       // Need to define Boolean for this loop, earlier it's based on X2 range of fit, do similar here. Case not used for now SK 27/8/19
	       if (GoodFit[i] == kFALSE) continue;                     // Take weighted avg 26/8/19 SK
	       WeightAvgSum1 += mean[i]/(mean_err[i]*mean_err[i]);               
	       WeightAvgSum2 += 1/(mean_err[i]*mean_err[i]);
	     }
	   xscale = WeightAvgSum1/WeightAvgSum2;
	   xscaleErr = 1/TMath::Sqrt(WeightAvgSum2);
	  
	   calibration_mk1[ipmt] = xscale;
	   calibration_mk1Err[ipmt] = xscaleErr;

	   //Scale full ADC spectra according to the mean of the SPE. This requires filling a new histogram with the same number of bins but scaled min/max
	   Int_t nbins;
	   nbins = PulseInt_quad[ipmt][ipmt]->GetXaxis()->GetNbins();

	   fscaled[ipmt] = new TH1F(Form("fscaled_PMT%d", ipmt+1), Form("Scaled ADC spectra for PMT%d; NPE; Normalized Counts",ipmt+1), 210, -1, 20);

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
	  Gauss1->SetRange(0.50, 1.50);
	  Gauss1->SetParameter(0, 0.05);
	  Gauss1->SetParameter(1, 1.0);
	  Gauss1->SetParameter(2, 0.3);
	  Gauss1->SetParLimits(0, 0.0, 1.0);
	  Gauss1->SetParLimits(1, 0.5, 1.5);
	  Gauss1->SetParLimits(2, 0.1, 0.5);
	  fFullShow ? fscaled[ipmt]->Fit("Gauss1","RQ") : fscaled[ipmt]->Fit("Gauss1","RQN");
			   
	  calibration_mk2[ipmt] = calibration_mk1[ipmt]*Gauss1->GetParameter(1);
	  calibration_mk2[ipmt] = xscale * Gauss1->GetParameter(1);

	  calibration_mk2Err[ipmt] = Sqrt(Power(xscaleErr*Gauss1->GetParameter(1),2) +  Power(xscale*Gauss1->GetParError(1),2)); 
	  pmt_calib[ipmt] = abs(1.0 - Gauss1->GetParameter(1));

	  //Scale full ADC spectra according to the mean of the SPE. This requires filling a new histogram with the same number of bins but scaled min/max
	  fscaled_mk2[ipmt] = new TH1F(Form("fscaled_mk2_PMT%d", ipmt+1), Form("Scaled ADC spectra for PMT%d; NPE; Normalized Counts",ipmt+1), 210, -1, 20);
	  
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
	  Gauss1->SetRange(0.50, 1.50);
	  Gauss1->SetParameter(0, 0.05);
	  Gauss1->SetParameter(1, 1.0);
	  Gauss1->SetParameter(2, 0.3);
	  Gauss1->SetParLimits(0, 0.0, 0.1);
	  Gauss1->SetParLimits(1, 0.5, 1.5);
	  Gauss1->SetParLimits(2, 0.1, 0.5);
	  fFullShow ? fscaled_mk2[ipmt]->Fit("Gauss1","RQ") : fscaled_mk2[ipmt]->Fit("Gauss1","RQN");

	  pmt_calib_mk2[ipmt] = abs(1.0 - Gauss1->GetParameter(1));
	 

	} //This brace marks the end of TracksFired strategy   
     
    }   

  //Output the actual calibration information
  //cout << "Calibration constants are (where the '*' indicates the better value)\nPMT#: First Guess  Second Guess\n" << endl;
  cout << "Calibration constants are \nPMT#:   First Guess" << setw(25) << "Second Guess\n" << endl;
  for (Int_t i=0; i<4; i++)
    {
      cout << Form("PMT%d:", i+1) << setw(8) << Form("%3.3f", calibration_mk1[i]) << " +/- " << Form("%3.3f", calibration_mk1Err[i]) << setw(13) << Form("%3.3f", calibration_mk2[i]) << " +/- " << Form("%3.3f", calibration_mk2Err[i]) <<  "\n";
    }

  printf("\n");

  if (fCut) cout << (Pois_Chi[0] < Pois_Chi[1] ? "First Guess":"Second Guess") << " better characterizes the full Poisson character" << endl;

  //Start the process of writing the calibration information to file
 
  ofstream calibration;
  calibration.open("calibration_temp.txt", ios::out);

  if (!calibration.is_open()) cout << "Problem saving calibration constants, may have to update constants manually!" << endl;

  else
    {
      calibration <<("; phgcer_adc_to_npe are: ")<<endl; 

      for (Int_t ipmt = 0; ipmt < 4; ipmt++)
	{
	  //calibration << Form("phgcer_adc_to_npe: PMT%i = ", 1+ipmt);

	  calibration << Form("1.0/%3.3f, ",calibration_mk1[ipmt]);

	  // calibration << Form("1./%3.3f. ",calibration_mk1[ipmt],  calibration_mk1Err[ipmt])<<endl;
	  // calibration << Form("%3.3f  +/-  %3.3f ",calibration_mk1[ipmt],  calibration_mk1Err[ipmt] )<<endl;
	}
       calibration << ("")<<endl;

      for (Int_t ipmt = 0; ipmt < 4; ipmt++)
	{
	  // calibration << Form("From quality control, phgcer_adc_to_npe");
	  calibration << Form("1./%3.3f, ",calibration_mk2[ipmt]);

	  // calibration << Form("1./%3.3f. ", (pmt_calib[ipmt] < pmt_calib_mk2[ipmt]) ? calibration_mk1[ipmt] : calibration_mk2[ipmt]);
	  //calibration << Form("%3.3f  +/-  %3.3f ",calibration_mk2[ipmt],  calibration_mk2Err[ipmt] )<<endl;

	}

      calibration.close();
    }
  // Second time to save the parameters
  ofstream calibration1;
  calibration1.open("calibration_temp1.txt", ios::out);

  if (!calibration1.is_open()) cout << "Problem saving calibration constants, may have to update constants manually!" << endl;

  else
    {
      calibration1 <<("phgcer_adc_to_npe are: ")<<endl; 

      for (Int_t ipmt = 0; ipmt < 4; ipmt++)
	{
	  //calibration << Form("phgcer_adc_to_npe: PMT%i = ", 1+ipmt);

	  //calibration << Form("1./%3.3f. ",calibration_mk1[ipmt]);

	  // calibration << Form("1./%3.3f. ",calibration_mk1[ipmt],  calibration_mk1Err[ipmt])<<endl;
	  calibration1 << Form("%3i %3.3f  %3.3f",ipmt+1, calibration_mk1[ipmt],  calibration_mk1Err[ipmt] )<<endl;
	}
       calibration1 << ("From quality control")<<endl;

      for (Int_t ipmt = 0; ipmt < 4; ipmt++)
	{
	  // calibration << Form("From quality control, phgcer_adc_to_npe");
	  // calibration1 << Form("1./%3.3f. ",calibration_mk2[ipmt]);

	  // calibration << Form("1./%3.3f. ", (pmt_calib[ipmt] < pmt_calib_mk2[ipmt]) ? calibration_mk1[ipmt] : calibration_mk2[ipmt]);
	  calibration1 << Form("%3i %3.3f  %3.3f", ipmt+1, calibration_mk2[ipmt],  calibration_mk2Err[ipmt] )<<endl;

	}

      calibration1.close();
    }  
}
