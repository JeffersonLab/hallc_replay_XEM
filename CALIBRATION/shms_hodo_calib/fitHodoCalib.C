/*SHMS Hodo Calibration script to determine scin. prop velocity, cable time offsets per paddle, 
  and time offsets between paddles in different planes
  Author: Carlos Yero
  Dated: June 5, 2018
  For more information, see the hodo calibration document v2 at https://hallcweb.jlab.org/doc-private/ShowDocument?docid=970 . 
*/

// December 2021 changes: 
// Velocity constraints were added since background and/or a too-small spot size were leading to unphysically high light propagation velocities.  
// January 2022 changes:
// A single paddle hit per plane requirement was added because the fitted velocity and mean time resolution were biased toward significantly larger values due to random hit backgrounds at high SHMS rates.
// As a compromise between simplicity and efficiency, no explicit matching of tracks to hit hodo paddles is done. This is referred to below as "track matching lite". 
 
// USER RESPONSIBILITIES:
// This program uses electrons for hodoscope calibrations. The user is responsible for setting PID and beta cuts. These are flagged by the comment line "//ePID CUT NOTICE" below.
// Rough cuts are applied to various times and TDC values. These need only change when there is a significant trigger change. These are flagged by the comment line "//HARD CUT NOTICE" below. 


#include <TMatrixD.h>
#include <TVectorD.h>
#include <TDecompSVD.h>
#include <TSystem.h>
#include <time.h> 
#include <TString.h>
#include "TFile.h"
#include "TTree.h"
#include <TNtuple.h>
#include "TCanvas.h"
#include <iostream>
#include <fstream>
#include "TMath.h"
#include "TH1F.h"
#include <TH2.h>
#include <TStyle.h>
#include <TGraph.h>
#include <TROOT.h>
#include <TMath.h>
#include <TLegend.h>
#include <TPaveLabel.h>
#include <TProfile.h>
#include <TPolyLine.h>
#include <TObjArray.h>
#include <TF1.h>

// C.Y. Jan 22, 2021
// Added headers necessary for parsing
// and reading existing parameter files
// (this is to be used to beta calculation within this code)
#include "parse_utils.h"
#include "hallc_parse_utils.h"

void fitHodoCalib(TString filename,Int_t runNUM,Bool_t cosmic_flag=kFALSE)
{

  gStyle->SetOptFit();  
  gROOT->SetBatch(kTRUE);    //do not display plots

  // Initialize the analysis clock
  clock_t t;
  t = clock();

  // changed this from 30000 N.H. 10 Sept 2021
  Int_t evtNUM = 1000000; // evtNUm is max number in the fit array
  TFile *data_file = new TFile(filename, "UPDATE"); 
  TTree *T = (TTree*)data_file->Get("T");


  //Create output root file where histograms will be stored
  TFile *outROOT = new TFile(Form("HodoCalibPlots_%d.root", runNUM), "recreate");
  
  /******Define Fixed Quantities********/
  static const Int_t PLANES = 4;
  static const Int_t SIDES = 2;
  static const TString spec = "P";
  static const TString det = "hod";
  static const string pl_names[4] = {"1x", "1y", "2x", "2y"};
  static const string side_names[2] = {"GoodPos", "GoodNeg"};
  string nsign[2] = {"+", "-"};
  Int_t maxPMT[4] = {13, 13, 14, 21};
  Int_t refPad[4] = {0, 13, 26, 40};              //use as reference when counting bars up to 61 
  
  
  Double_t phodo_velArr[PLANES][21] = {0.0};      //store phodo velocity parameters (1/slope of the line fit)
  Double_t phodo_cableArr[PLANES][21] = {0.0};    //store phodo cableLength differences (y-int of line fit)
  Double_t phodo_sigArr[PLANES][21] = {0.0};       //store phodo sigma parameters
  Double_t vp = 30.0;                             //speed of light [cm/ns]

  // C. Yero Nov 09, 2021 | Added velocity variation (phodo_velVar) about the set valued for hodoscope paddle velocity fits
  // If the fitted velocity is out of the boundary: phodo_velSet +/- phodo_velVar, the default velocity value (phodo_velSet) will be set accordingly.
  // UNITS: cm / ns
  Double_t phodo_velVar = 3.0;  // This tolerance parameter can be changed to allow larger variations in the velocities. The value 3.0 seems a reasonable compromise. 

  // velocities set (determined by averaging the velocities over all relevant scintillator paddles that had sufficient statistics for a good fit)
  //                                S1X,   S1Y,   S2X,   S2Y
  // Out-of-bounds velocities from a failed fit or low statistics will be set to phodo_velSet
  Double_t phodo_velSet[PLANES] = {15.75, 15.75, 15.75, 14.0};  //good averaged velocities to be used, in case that fit fails or is out of range

  //velocity Min/Max boundaries 
  Double_t phodo_velMin[PLANES] = {phodo_velSet[0]-phodo_velVar, phodo_velSet[1]-phodo_velVar, phodo_velSet[2]-phodo_velVar, phodo_velSet[3]-phodo_velVar};
  Double_t phodo_velMax[PLANES] = {phodo_velSet[0]+phodo_velVar, phodo_velSet[1]+phodo_velVar, phodo_velSet[2]+phodo_velVar, phodo_velSet[3]+phodo_velVar};
  

  //--------------------------------------------
  
  if (cosmic_flag) {
    vp= -vp;
    cout << " Speed of light set to negative number for cosmics" << endl;
  }

  /******Define Leafs to be read from TTree******/
 
   //---Names---
  TString base;
  TString nTdcTimeUnCorr;
  TString nTdcTimeTWCorr;
  TString nAdcPulseTime;
  TString nAdcPulseAmp;
  
  TString nTrackXPos;
  TString nTrackYPos;
  TString nDiffTWCorr;
  
  TString npcal_etrkNorm = "P.cal.etracknorm";
  TString npngcer_npeSum = "P.hgcer.npeSum";
  //TString npngcer_npeSum = "P.ngcer.npeSum";                                                                          
  TString npdc_ntrack = "P.dc.ntrack";
  TString nhod_nhits = "nhits";
  TString nbeta = "P.hod.betanotrack";

  TString npdc_xpfp = "P.dc.xp_fp";
  TString npdc_ypfp = "P.dc.yp_fp";
  
  //ePID CUT NOTICE
  Double_t etrknrm_low_cut = 0.7;
  Double_t npngcer_npeSum_low_cut = 0.5;
  Double_t betanotrack_low_cut = 0.5;
  Double_t betanotrack_hi_cut = 1.5;

  if (cosmic_flag) betanotrack_low_cut = -1.2;
  if (cosmic_flag) betanotrack_hi_cut = -.7;

  //---Variables---
  Double_t TdcTimeUnCorr[PLANES][SIDES][21];
  Double_t TdcTimeTWCorr[PLANES][SIDES][21];
  Double_t AdcPulseTime[PLANES][SIDES][21];
  Double_t AdcPulseAmp[PLANES][SIDES][21];

  Double_t TrackXPos[PLANES];
  Double_t TrackYPos[PLANES];
  Double_t DiffDistTWCorr[PLANES][21];

  Double_t pcal_etrkNorm;
  Double_t pngcer_npeSum;
  Double_t pdc_ntrack;
  Double_t hod_nhits[PLANES];
  Double_t beta;

  Double_t pdc_xpfp, pdc_ypfp;
  
  /******Define Matrices/Vectors and related *******/
  Int_t good_pad[PLANES];                   //keep track of good paddle hits
  Int_t ngood = 0;

  static const Int_t npar = 61;             //reference paddle 1x7 is fixed (so we actually have 60)
  static const Int_t ROW = 6*evtNUM;        
  static const Int_t COL = npar;
 
  Int_t row1, row2, row3, row4, row5, row6;   //keep track of ith row element
  Int_t cnt;                                  //keep track of good plane hits

  //L*x = b Linear Matrix System
  TVectorD bVec(npar);
  TMatrixD lambda(ROW,COL);
  TMatrixD Ay(npar, npar);

  //Variables that make up the b_Vector
  Double_t x[PLANES];
  Double_t y[PLANES];
  Double_t z[PLANES][21];    //z-coordinates / paddle corrected for dz offset
  Double_t zCorr[PLANES];
  Double_t x1, x2, x3, x4;   //x-coordinates of the track for planes 1, 2, 3, 4
  Double_t y1, y2, y3, y4;   //y-coordinates of the track for planes 1, 2, 3, 4
  Double_t z0[PLANES] = {52.1, 61.7, 271.4, 282.4};     //zentral z dist from hod planes to focal plane
  Double_t z1, z2, z3, z4;   // perpendicular distance from each plane to focal plane (parameters)
  Double_t dz[PLANES] = {2.12, -2.12, -2.12, -5.4};   //scintillator paddles offset from z_central


  Double_t good_TW_pos[PLANES];
  Double_t good_TW_neg[PLANES];
  
  Double_t t1, t2, t3, t4;   //time averages between two sides (i,j) of a paddle in a plane
                             //ex. t1 = 1/2 (good_TW_pos[0] + good_TW_neg[0])
  
  Double_t D12, D13, D14, D23, D24, D34;  //Distance between any two planes
  Double_t b12, b13, b14, b23, b24, b34;  //the six components of the b-vector, for each good event
  Double_t LCoeff[PLANES][21] = {0.0};   //Variables to write out LCoeff. parameter file

  //Initialize Some Variables
  bVec.Zero();
  lambda.Zero();
  Ay.Zero();

  
  //Determine Corrected z distance / paddle
  for(int ipl = 0; ipl < PLANES; ipl++)
    {
      //cout << "Plane: " << ipl << endl;
      for(int ibar = 0; ibar < maxPMT[ipl]; ibar++)
	{
	  z[ipl][ibar] = 0.0;
	  z[ipl][ibar] = (z0[ipl] + (ibar%2)*dz[ipl])*1.0;
	}

    }

  /**** Define new variables and branches to be added *****/

  //C.Y. Jan 01, 2021 | good plane hit counter variable
  Int_t good_hod_1x_nhits;
  Int_t good_hod_1y_nhits;
  Int_t good_hod_2x_nhits;
  Int_t good_hod_2y_nhits;
  Bool_t single_hit_flg;
  

  //C.Y. Jan 02, 2021 | add new branch
  TBranch *br_good_hod_1x_nhits = T->Branch("br_good_hod_1x_hit", &good_hod_1x_nhits, "good_hod_1x_nhits/I");
  TBranch *br_good_hod_1y_nhits = T->Branch("br_good_hod_1y_hit", &good_hod_1y_nhits, "good_hod_1y_nhits/I");
  TBranch *br_good_hod_2x_nhits = T->Branch("br_good_hod_2x_hit", &good_hod_2x_nhits, "good_hod_2x_nhits/I");
  TBranch *br_good_hod_2y_nhits = T->Branch("br_good_hod_2y_hit", &good_hod_2y_nhits, "good_hod_2y_nhits/I");
  TBranch *br_single_hit_flg    = T->Branch("br_single_hit_flg",  &single_hit_flg,    "single_hit_flg/b");
   
  
  /*******Define Canvas and Histograms*******/

  //----Canvas----
  TCanvas *beta_calibWeighted_canv = new TCanvas("beta_calib_weight", "SHMS Hodo Beta Calibrated (w/ Tracking + Weight)", 1000, 700);
  TCanvas *beta_calibUnweighted_canv = new TCanvas("beta_calib_noweight", "SHMS Hodo Beta Calibrated (w/ Tracking + Weight=1)", 1000, 700);
  
  TCanvas *good_hod1x_Hit_canv = new TCanvas("good_hod_1x_hits", "Good 1X Plane Hits", 1000, 700);
  TCanvas *good_hod1y_Hit_canv = new TCanvas("good_hod_1y_hits", "Good 1Y Plane Hits", 1000, 700);
  TCanvas *good_hod2x_Hit_canv = new TCanvas("good_hod_2x_hits", "Good 2X Plane Hits", 1000, 700);
  TCanvas *good_hod2y_Hit_canv = new TCanvas("good_hod_2y_hits", "Good 2Y Plane Hits", 1000, 700);
  TCanvas *good_hodSingleHit_canv = new TCanvas("good_hodSingleHit", "Good Hodo Single Hit Per Plane Flag", 1000, 700); 

  TCanvas *TWAvg_canv[PLANES];
  TCanvas *TWAvg_canv_2D[PLANES];
  TCanvas *Diff_TWDistTrkPos_canv[PLANES];

  TCanvas *TWUnCorr_canv[PLANES][SIDES];
  TCanvas *TWCorr_canv[PLANES][SIDES];
  TCanvas *TWCorr_v_TrkPos_canv[PLANES];
  TCanvas *TWDiff_v_TrkPos_canv[PLANES];

  //----Histograms----
  TH1F *H_good_hod1x_nhits = new TH1F("good_hod1x_nhits", "Good Hodo 1X Hits", 100, 0, 25);
  TH1F *H_good_hod1y_nhits = new TH1F("good_hod1y_nhits", "Good Hodo 1Y Hits", 100, 0, 25);  
  TH1F *H_good_hod2x_nhits = new TH1F("good_hod2x_nhits", "Good Hodo 2X Hits", 100, 0, 25);  
  TH1F *H_good_hod2y_nhits = new TH1F("good_hod2y_nhits", "Good Hodo 2Y Hits", 100, 0, 25);  
  TH1F *H_good_hod_nhits = new TH1F("good_hod_nhits", "Good Hodo Single Hit per Plane", 100, -1,2);

  TH1F *h1Hist_TWAvg[PLANES][21];                    // (TWCorr_Pos + TWCorr_Neg) / 2       <------- 
  TH1F *h1Hist_TWAvg_CUT[PLANES][21];                                                    //<------
  TH1F *h1Hist_TWDiffTrkPos[PLANES][21];             //1D hist of TW Corr. Dist - Track Position (width should be Hodo resolution)    <------
  TH1D *determineFitRange;                           // Used to detemine fit range of TW_CORR_v_TrkPos (proj. X axis)

  TH2F *h2Hist_TW_UnCorr[PLANES][SIDES][21];         //Time-Walk Uncorrected vs. ADC Pulse Amp Hist
  TH2F *h2Hist_TW_Corr[PLANES][SIDES][21];           //Time-Walk Corrected vs. ADC Pulse Amp Hist
  TH2F *h2Hist_TW_Corr_v_TrkPos[PLANES][21];         //Time-Walk Corr Time Diff vs. Track Position @ Hod. Paddle      <-------
  TH2F *h2Hist_TWDiff_v_TrkPos[PLANES][21];          //(Time-Walk Corr Dist. Diff) vs. TrackPos)                      <-------
  TH2F *h2Hist_TWAvg_v_TrkPos[PLANES][21];           // (TWCorr_Pos + TWCorr_Neg) / 2  vs TrackPos      <------- 

  /*******Define Fit Functions and Related Variables*******/

  Double_t Mean;                                      //variable to get Mean to make a nSig*StdDev cut
  Double_t StdDev;                                    //variable to get standard deviation to make a nSig*StdDev cut
  Double_t nSig;                                      //multiple of Sigma used for sigmaCut
  
  //Fit Ranges for TW_Corr_v_TrkPos
  int minFitBin, maxFitBin;
  double minFitVal, maxFitVal;
  int nEvts, nFitCut;

  //Gaussian Fit for TWAvg
  TF1 *gausFit[PLANES][21];

  //1d Fit Function for fitting TW_Corr vs. TrkPos
  TF1 *fit1x = new TF1("fit1x", "[0]*x + [1]", -40., 40.);
  TF1 *fit1y = new TF1("fit1y", "[0]*x + [1]", -40., 40.);                                                                                                     
  TF1 *fit2x = new TF1("fit2x", "[0]*x + [1]", -40., 40.);                                                                                 
  TF1 *fit2y = new TF1("fit2y", "[0]*x + [1]", -40., 40.);                                                                             

  /*  ---- 1D Re-Fit Function ----
     C. Yero Dec. 3 2021 | 1D Fit Function for re-fitting TW_Corr vs. TrkPos (in case fit failed or was out of velocity range)
     This new fit function would be:  Y = m_fixed * X + b,  where m_fixed = 1 / phodo_velSet, and b is the y-intercept 
     (cable time offset) which would be the ONLY fit parameter
  */

  TF1 *rfit1x = new TF1("rfit1x", Form("(1./%f) * x + [0]", phodo_velSet[0]), -40., 40.);
  TF1 *rfit1y = new TF1("rfit1y", Form("(1./%f) * x + [0]", phodo_velSet[1]), -40., 40.);                                                                                                     
  TF1 *rfit2x = new TF1("rfit2x", Form("(1./%f) * x + [0]", phodo_velSet[2]), -40., 40.);                                                                                 
  TF1 *rfit2y = new TF1("rfit2y", Form("(1./%f) * x + [0]", phodo_velSet[3]), -40., 40.); 
  


  //Set Param Values/Names
  fit1x->SetParameter(0, 1.), fit1x->SetParName(0, "slope");
  fit1x->SetParameter(1, 1.), fit1x->SetParName(1, "y-int");                                                                                                                                 
  fit1y->SetParameter(0, 1.), fit1y->SetParName(0, "slope");  
  fit1y->SetParameter(1, 1.), fit1y->SetParName(1, "y-int");                                                                                                                               
  fit2x->SetParameter(0, 1.), fit2x->SetParName(0, "slope");                                                                                            
  fit2x->SetParameter(1, 1.), fit2x->SetParName(1, "y-int");  
  fit2y->SetParameter(0, 1.), fit2y->SetParName(0, "slope");           
  fit2y->SetParameter(1, 1.), fit2y->SetParName(1, "y-int"); 

  //Set Param Values/Names for Re-Fit function
  rfit1x->SetParameter(0, 1.), rfit1x->SetParName(0, "y-int");                                                                                                           
  rfit1y->SetParameter(0, 1.), rfit1y->SetParName(0, "y-int");                                                                                                               
  rfit2x->SetParameter(0, 1.), rfit2x->SetParName(0, "y-int");                                                                                                              
  rfit2y->SetParameter(0, 1.), rfit2y->SetParName(0, "y-int");                                                                                                                                

  
  //Set PID 
  Bool_t pcal;
  Bool_t pngcer;
  Bool_t pdctrk;
  Bool_t pid_pelec; 
  Bool_t betaCut;
  
  Bool_t goodTrk;

  /********Initialize HISTOS and GET TTREE VARIABLES*********/

  T->SetBranchAddress(npcal_etrkNorm, &pcal_etrkNorm);
  T->SetBranchAddress(npngcer_npeSum, &pngcer_npeSum);
  T->SetBranchAddress(npdc_ntrack, &pdc_ntrack);
  T->SetBranchAddress(nbeta, &beta);

  // C.Y. Jan 23, 2022 | Added focal plane derivatives (dx/dz -> xp_fp, dy/dz -> yp_fp) used in beta calculation 'pathNorm' variable
  T->SetBranchAddress(npdc_xpfp, &pdc_xpfp);
  T->SetBranchAddress(npdc_ypfp, &pdc_ypfp);
 
  //Loop over hodo planes
  for (Int_t npl = 0; npl < PLANES; npl++ )
    {
      
      //Loop over hodo side
      for (Int_t side = 0; side < SIDES; side++)
	{
	  
	  //Loop over hodo PMTs
	  for (Int_t ipmt = 0; ipmt < maxPMT[npl]; ipmt++)
	    {
	    	   
	      //Initialize Histograms
	      h2Hist_TW_UnCorr[npl][side][ipmt] = new TH2F(Form("TW_UnCorr PMT %s%d%s", pl_names[npl].Data(), ipmt+1, nsign[side].Data()), Form("PMT %s%d%s: UnCorr. (TDC - ADC) Pulse Time vs. ADC Pulse Amplitude ",pl_names[npl].Data(), ipmt+1, nsign[side].Data()), 600, 0, 420, 120, -60, 60);   
	      h2Hist_TW_Corr[npl][side][ipmt] = new TH2F(Form("TW_Corr PMT %s%d%s", pl_names[npl].Data(), ipmt+1, nsign[side].Data()) , Form("PMT %s%d%s: Corr. (TDC - ADC) Pulse Time vs. ADC Pulse Amplitude ", pl_names[npl].Data(), ipmt+1, nsign[side].Data()), 600, 0, 420, 120, -60, 60);   
	      
	      h2Hist_TW_UnCorr[npl][side][ipmt]->GetYaxis()->SetTitle("Time Walk UnCorr.(TDC - ADC) Pulse Time (ns)");
	      h2Hist_TW_UnCorr[npl][side][ipmt]->GetXaxis()->SetTitle("ADC Pulse Amplitude (mV)");
	      h2Hist_TW_UnCorr[npl][side][ipmt]->GetXaxis()->CenterTitle();
	      
	      h2Hist_TW_Corr[npl][side][ipmt]->GetYaxis()->SetTitle("Time Walk Corr.(TDC - ADC) Pulse Time (ns)");;
	      h2Hist_TW_Corr[npl][side][ipmt]->GetXaxis()->SetTitle("ADC Pulse Amplitude (mV)");
	      h2Hist_TW_Corr[npl][side][ipmt]->GetXaxis()->CenterTitle();
	      
	       if (side==0) //require ONLY one side, since a time diff between two pmts at each end is taken
		{

		  h1Hist_TWAvg[npl][ipmt] = new TH1F(Form("Avg. Time: Paddle %s%d", pl_names[npl].c_str(), ipmt+1), Form("Paddle %s%d: Time-Walk Corrected Average Time", pl_names[npl].c_str(), ipmt+1), 200, 0, 200);
		  
		  h1Hist_TWAvg_CUT[npl][ipmt] = new TH1F(Form("Avg. Time CUT: Paddle %s%d", pl_names[npl].c_str(), ipmt+1), Form("Paddle %s%d: Time-Walk Corrected Average (CUT)",pl_names[npl].c_str(), ipmt+1), 200, 0, 200);
	      
		  h2Hist_TWDiff_v_TrkPos[npl][ipmt] = new TH2F(Form("DistDiff: Paddle %s%d", pl_names[npl].Data(), ipmt+1), Form("Paddle %s%d: Time-Walk Corr. Hit Dist vs. Hod Track Position", pl_names[npl].Data(), ipmt+1), 160, -80, 80, 200, -120, 80);
		  h2Hist_TW_Corr_v_TrkPos[npl][ipmt] = new TH2F(Form("TimeDiff: Paddle %s%d", pl_names[npl].Data(), ipmt+1), Form("Paddle %s%d: Time-Walk Corr. TimeDiff. vs. Hod Track Position", pl_names[npl].Data(), ipmt+1), 160, -60, 60, 200, -15, 15);
		  
		  h1Hist_TWDiffTrkPos[npl][ipmt] = new TH1F(Form("DistDiff - Track: Paddle %s%d", pl_names[npl].Data(), ipmt+1), Form("Paddle %s%d: Time-Walk Corr. Hit Dist. - Hod Track Position",pl_names[npl].Data(), ipmt+1), 200, -120, 80);

		  h2Hist_TWAvg_v_TrkPos[npl][ipmt] = new TH2F(Form("TimeAvg_v_Trk: Paddle %s%d", pl_names[npl].c_str(), ipmt+1), Form("Paddle %s%d: Time-Walk Corr. TimeAvg. vs. Hod Track Position", pl_names[npl].c_str(), ipmt+1), 160, -40, 40, 150, 0, 150);
  
		  //Set Axis Titles
		  h1Hist_TWAvg[npl][ipmt]->GetXaxis()->SetTitle("Time-Walk Corr. TDC Average Paddle Time (ns)");
		  h1Hist_TWAvg_CUT[npl][ipmt]->GetXaxis()->SetTitle("Time-Walk Corr. TDC Average Paddle Time (ns)");

		  h1Hist_TWAvg[npl][ipmt]->GetXaxis()->CenterTitle();
		  h1Hist_TWAvg_CUT[npl][ipmt]->GetXaxis()->CenterTitle();


		  h2Hist_TWDiff_v_TrkPos[npl][ipmt]->GetYaxis()->SetTitle("Hit Dist. from Paddle Center (cm)");
		  h2Hist_TWDiff_v_TrkPos[npl][ipmt]->GetXaxis()->SetTitle("Hodoscope Track Position from Center (cm)");
		   
		  h2Hist_TW_Corr_v_TrkPos[npl][ipmt]->GetYaxis()->SetTitle("Time-Walk Corrected TDC Time Difference (ns)");
		  h2Hist_TW_Corr_v_TrkPos[npl][ipmt]->GetXaxis()->SetTitle("Hodoscope Track Position from Center (cm)");

		  h2Hist_TWAvg_v_TrkPos[npl][ipmt]->GetYaxis()->SetTitle("Time-Walk Corrected Time Avgerage (ns)");
		  h2Hist_TWAvg_v_TrkPos[npl][ipmt]->GetXaxis()->SetTitle("Hodoscope Track Position from Center (cm)");
		  

		  h2Hist_TWDiff_v_TrkPos[npl][ipmt]->GetYaxis()->CenterTitle();
		  h2Hist_TWDiff_v_TrkPos[npl][ipmt]->GetXaxis()->CenterTitle();
		  h2Hist_TW_Corr_v_TrkPos[npl][ipmt]->GetYaxis()->CenterTitle();
		  h2Hist_TW_Corr_v_TrkPos[npl][ipmt]->GetXaxis()->CenterTitle();

		} //end require SINGLE side requirement
	       
	       
	       //----Define TTree Leaf Names-----
	       base = spec + "." + det + "." + pl_names[npl].c_str();
	       
	       nTdcTimeUnCorr = base + "." + side_names[side].c_str() + "TdcTimeUnCorr";
	       nTdcTimeTWCorr = base + "." + side_names[side].c_str() + "TdcTimeWalkCorr";
	       nAdcPulseTime = base + "." + side_names[side].c_str() + "AdcPulseTime";
	       nAdcPulseAmp = base + "." + side_names[side].c_str() + "AdcPulseAmp";
	       nDiffTWCorr = base + "." + "DiffDisTrackCorr";
	       nTrackXPos = base + "." + "TrackXPos";
	       nTrackYPos = base + "." + "TrackYPos";
	       nhod_nhits = base + "." + "nhits";
	       
	       //------Set Branch Address-------
	       T->SetBranchAddress(nTdcTimeUnCorr,  &TdcTimeUnCorr[npl][side]);
	       T->SetBranchAddress(nTdcTimeTWCorr,  &TdcTimeTWCorr[npl][side]);
	       T->SetBranchAddress(nAdcPulseTime,  &AdcPulseTime[npl][side]);
	       T->SetBranchAddress(nAdcPulseAmp,  &AdcPulseAmp[npl][side]);
	       //T->SetBranchAddress(nDiffTWCorr,  &DiffDistTWCorr[npl]);  //C.Y. Jan 04, 2021 | This needs to be re-calculated below using the updated hodo velocity/cable times from updated fits
	       T->SetBranchAddress(nTrackXPos,  &TrackXPos[npl]);
	       T->SetBranchAddress(nTrackYPos,  &TrackYPos[npl]);
	       T->SetBranchAddress(nhod_nhits,  &hod_nhits[npl]);

	       
	       

	    } //end loop over hodo PMTs

	} // end loop over hodo side
      
    } //end loop over hodo planes


  //**************************************************************//
  // FIRST PASS OF EVENT LOOP (Get the StdDev) of (TDC+ + TDC-)/2 //
  //**************************************************************//
  
  cout << "Initializing 1st Pass of Event Loop: " << endl;

  Long64_t nentries = T->GetEntries();
 

  // dynamically allocate 3d array
  Bool_t ***mean_time_flg = new Bool_t **[nentries]();
  
  for (int i = 0; i < nentries; i++) {
    mean_time_flg[i] = new Bool_t *[PLANES]();
    for (int j = 0; j < PLANES; j++){
      mean_time_flg[i][j] = new Bool_t[21]();
      for (int k = 0; k < 21; k++){
	mean_time_flg[i][j][k] = false;
      } // end k loop over paddles
    } // end j loop over planes
  } // end i loop over nentries


  //Loop over all entries
  for(Long64_t i=0; i<nentries; i++)
    {
      T->GetEntry(i);  
      
      pcal = pcal_etrkNorm>etrknrm_low_cut;
      pngcer = pngcer_npeSum>npngcer_npeSum_low_cut;
      pdctrk = pdc_ntrack>0.0;  // Seems to work. One could potentially improve hit filtering further by requiring a single reconstructed track, but efficiency may suffer. 
      betaCut = beta>betanotrack_low_cut&& beta<betanotrack_hi_cut;
      pid_pelec = pcal&&pngcer&&pdctrk;
      if (cosmic_flag)  pid_pelec = betaCut&&pdctrk; 
       //Apply PID ELECTRON CUT
      if(pid_pelec)
	{
	  
	  //Loop over hodo planes
	  for (Int_t npl = 0; npl < PLANES; npl++ )
	    {
	      
	      //Loop over pmt
	      for (Int_t ipmt = 0; ipmt < maxPMT[npl]; ipmt++)
		{	        
		  
		  //HARD CUT NOTICE		  
		  if(TdcTimeTWCorr[npl][0][ipmt] < 200. && TdcTimeTWCorr[npl][1][ipmt] < 200. && pcal_etrkNorm>etrknrm_low_cut)
		    {
		      //Fill Average TW Corr TDC Time
		      h1Hist_TWAvg[npl][ipmt]->Fill((TdcTimeTWCorr[npl][0][ipmt] + TdcTimeTWCorr[npl][1][ipmt])/2.);
		      
		    } //end time cut
		  
		} //end pmt loop
	      
	    }// end plane loop
	  
	} //END PID ELECTRON CUT
      
      cout << std::setprecision(2) << double(i) / nentries * 100. << "  % " << std::flush << "\r";
      
    } //end loop over entries
  
      //Set cut on StdDev. 
      // Because of the small flat background of photon hits at high rates, a 1 StdDev cut ends up being generously wider than a Gaussian 1 sigma cut would be. 
      // At very low rates (unlikely to be encountered in the SHMS) there may be some benefit in increasing nSig to 2 for higher efficiency. (This suggestion may be relevant to the HMS.)
  //HARD CUT NOTICE
  nSig = 1;    
  
  
  //************************************//
  //    SECOND PASS OF EVENT LOOP       //
  //************************************//
  cout << "Initializing 2nd Pass of Event Loop: " << endl;
  
  //Loop over all entries
  for(Long64_t i=0; i<nentries; i++)
    {
      
      T->GetEntry(i);  
      
      //initialize good paddle hit counter of hodo planes (resets per entry) 
      good_hod_1x_nhits = 0;      
      good_hod_1y_nhits = 0;
      good_hod_2x_nhits = 0;
      good_hod_2y_nhits = 0;
      single_hit_flg = false;
      
      pcal = pcal_etrkNorm>etrknrm_low_cut;
      pngcer = pngcer_npeSum>npngcer_npeSum_low_cut; //C.Y. Dec 12, 2021: put ONLY min cut requirement  |  JM 31-10-21: Added in npeSum > 0 requirement
      pdctrk = pdc_ntrack>0.0;   // Seems to work. One could potentially improve hit filtering further by requiring a single reconstructed track, but efficiency may suffer. 
      betaCut = beta>betanotrack_low_cut&& beta<betanotrack_hi_cut;
      pid_pelec = pcal&&pngcer&&pdctrk;
      if (cosmic_flag)  pid_pelec = betaCut&&pdctrk; 
      
      
      // ----------------- "LITE" HIT FILTERING (PART 1) -----------------

      if(pid_pelec) // APPLY PID CUT TO SELECT CLEAN ELECTRONS
	{
	  
	  //Loop over hodo planes
	  for (Int_t npl = 0; npl < PLANES; npl++ )
	    {
	      
	      //Loop over plane side
	      for (Int_t side = 0; side < SIDES; side++)
		{
		  
		  //Loop over pmt
		  for (Int_t ipmt = 0; ipmt < maxPMT[npl]; ipmt++)
		    {	        
		      
		      //Get Standard deviation from initial entry fill
		      StdDev =  h1Hist_TWAvg[npl][ipmt]->GetStdDev();      
		      Mean =  h1Hist_TWAvg[npl][ipmt]->GetMean();      
		  
		      //FIll Uncorrected/Corrected Time Walk Histos
		      h2Hist_TW_UnCorr[npl][side][ipmt]->Fill(AdcPulseAmp[npl][side][ipmt], TdcTimeUnCorr[npl][side][ipmt] - AdcPulseTime[npl][side][ipmt] );
		      h2Hist_TW_Corr[npl][side][ipmt]->Fill(AdcPulseAmp[npl][side][ipmt], TdcTimeTWCorr[npl][side][ipmt] - AdcPulseTime[npl][side][ipmt] );
		      
		      
		      //Add Time Cuts to get rid of kBig - kBig values, which yielded high evt density at zero
		      //HARD CUT NOTICE
		      if(TdcTimeTWCorr[npl][0][ipmt] < 200. && TdcTimeTWCorr[npl][1][ipmt] < 200.)
			{
			  
			  //require only one side, as a time diff between the two ends of a paddle is take
			  if (side==0)  
			    {
			    
			      // Apply mean tdc time +/- sig*StdDev cut to narrow window of "good hits"
			      if ( (((TdcTimeTWCorr[npl][0][ipmt] + TdcTimeTWCorr[npl][1][ipmt])/2.) > (Mean-nSig*StdDev)) &&  (((TdcTimeTWCorr[npl][0][ipmt] + TdcTimeTWCorr[npl][1][ipmt])/2.) < (Mean+nSig*StdDev)))
				{				  				  
				  mean_time_flg[i][npl][ipmt] = true;
				  
				  // increment good plane hit counter 
				  if(npl==0){				    
				    good_hod_1x_nhits++;
				   
				  }
				  if(npl==1){ 
				    good_hod_1y_nhits++;
				 
				  }			    
				  if(npl==2){              
				    good_hod_2x_nhits++;
				    
				  }			      
				  if(npl==3){            
				    good_hod_2y_nhits++;				    
				    
				  }	        		    
				  
				} //end +-nSig*StdDev CUT of TW Corr Time
			      
			    }//end single-side requirement
			  
			} //end time cuts
		      
		    }//end pmt loop
		  
		} //end side loop
	      
	    } //end plane loop
	  
	} //END PID ELECTRON
      
      cout << std::setprecision(2) << double(i) / nentries * 100. << "  % " << std::flush << "\r";
      
      
      //Fill histogram of good hits per plane
      H_good_hod1x_nhits->Fill(good_hod_1x_nhits);
      H_good_hod1y_nhits->Fill(good_hod_1y_nhits); 
      H_good_hod2x_nhits->Fill(good_hod_2x_nhits); 
      H_good_hod2y_nhits->Fill(good_hod_2y_nhits); 
     
      //Define good plane hit
      if( (good_hod_1x_nhits==1) && (good_hod_1y_nhits==1) && (good_hod_2x_nhits==1) && (good_hod_2y_nhits==1) ) { single_hit_flg = true; } 
      
      H_good_hod_nhits->Fill(single_hit_flg);
      
      //Fill branches with good hits per plane information
      br_good_hod_1x_nhits->Fill();
      br_good_hod_1y_nhits->Fill();
      br_good_hod_2x_nhits->Fill();
      br_good_hod_2y_nhits->Fill();
      br_single_hit_flg->Fill();
      
      
      // ----------------- "LITE" HIT FILTERING (PART 2) -----------------
      if(pid_pelec && single_hit_flg)
	{
	  //Loop over hodo planes
	  for (Int_t npl = 0; npl < PLANES; npl++ )
	    {	      
	      
	      //Loop over plane side
	      for (Int_t side = 0; side < SIDES; side++)
		{
		  
		  //Loop over pmt
		  for (Int_t ipmt = 0; ipmt < maxPMT[npl]; ipmt++)
		    {	        
		      if(mean_time_flg[i][npl][ipmt]==0) continue;  //C.Y. explicity add mean-time flag to eliminate possibility of multi-paddle hits per plane

		      //Get Standard deviation from initial entry fill
		      StdDev =  h1Hist_TWAvg[npl][ipmt]->GetStdDev();      
		      Mean =  h1Hist_TWAvg[npl][ipmt]->GetMean();
		      
		      //Add Time Cuts to get rid of kBig - kBig values, which yielded high evt density at zero
		      
		      //HARD CUT NOTICE
		      if(TdcTimeTWCorr[npl][0][ipmt] < 200. && TdcTimeTWCorr[npl][1][ipmt] < 200.)
			{
			  
			  //require only one side, as a time diff between the two ends of a paddle is take
			  if (side==0)  
			    {
			      

			      if (npl==0 || npl==2)
				{
				  h2Hist_TWAvg_v_TrkPos[npl][ipmt]->Fill(TrackYPos[npl], 0.5*(TdcTimeTWCorr[npl][1][ipmt] + TdcTimeTWCorr[npl][0][ipmt])); 
				}
			      
			      else if (npl==1 || npl==3)
				{
				  h2Hist_TWAvg_v_TrkPos[npl][ipmt]->Fill(TrackXPos[npl], 0.5*(TdcTimeTWCorr[npl][1][ipmt] + TdcTimeTWCorr[npl][0][ipmt])); 
				  
				}
			      
			      // Apply mean tdc time +/- sig*StdDev cut to narrow window of "good hits"
			      if ( (((TdcTimeTWCorr[npl][0][ipmt] + TdcTimeTWCorr[npl][1][ipmt])/2.) > (Mean-nSig*StdDev)) &&  (((TdcTimeTWCorr[npl][0][ipmt] + TdcTimeTWCorr[npl][1][ipmt])/2.) < (Mean+nSig*StdDev)))
				{

				  if (npl==0 || npl==2)
				    {				      
				      h2Hist_TW_Corr_v_TrkPos[npl][ipmt]->Fill(TrackYPos[npl],  0.5*(TdcTimeTWCorr[npl][1][ipmt] - TdcTimeTWCorr[npl][0][ipmt]));					     
				    }
				  
				  else if (npl==1 || npl==3)
				    {				      
				      h2Hist_TW_Corr_v_TrkPos[npl][ipmt]->Fill(TrackXPos[npl],  0.5*(TdcTimeTWCorr[npl][1][ipmt] - TdcTimeTWCorr[npl][0][ipmt]));				      				      
				    }
				  
				  h1Hist_TWAvg_CUT[npl][ipmt]->Fill((TdcTimeTWCorr[npl][0][ipmt] + TdcTimeTWCorr[npl][1][ipmt])/2.);
				  
				} //end meantime +/- nSig*StdDev CUT of TW Corr Time
			      
			    } // end single-side requirement
			  
			} //end time cuts
		      
		    } //end pmt loop
		  
		} //end side loop
	      
	    } //end plane loop
	  
	} // end pid-electron AND single plane hit CUTS (IMPORTANT ! ! ! )
      
    } //end entry loop
  
  //Draw good hits histograms per plane, and overall for 4 planes single hit.
  good_hod1x_Hit_canv->cd();
  H_good_hod1x_nhits->Draw();

  good_hod1y_Hit_canv->cd();
  H_good_hod1y_nhits->Draw();  

  good_hod2x_Hit_canv->cd();                                                                                                                                                  
  H_good_hod2x_nhits->Draw();                                                                                                                                             

  good_hod2y_Hit_canv->cd();                                                                                                                                                    
  H_good_hod2y_nhits->Draw();                                                                                                                                             

  good_hodSingleHit_canv->cd();
  H_good_hod_nhits->Draw();
  
  
  //The technique to fit the parameters phodo_velFit and phodo_cableFit is explained on page 4 and Figure 6 of the hodo calibration document v2 at https://hallcweb.jlab.org/doc-private/ShowDocument?docid=970 . 
  
  cout << " Starting fitting slope and intercept of histograms" << endl;
  
  for (Int_t npl = 0; npl < PLANES; npl++ )
    {      
      
      //--- Create Canvas to store TW-Corr Time/Dist vs. trk position ---
      
      TWCorr_v_TrkPos_canv[npl] = new TCanvas(Form("TWCorrTime_v_Pos%d", npl), Form("2DTWCorr_Time, plane %s", pl_names[npl].c_str()),  1000, 700);
      TWDiff_v_TrkPos_canv[npl] = new TCanvas(Form("TWCorrDist_v_Pos%d", npl), Form("2DTWCorr_Dist, plane %s", pl_names[npl].c_str()),  1000, 700);
      
      TWAvg_canv[npl] = new TCanvas(Form("TWAvg_%d", npl), Form("TWAvg, plane %s", pl_names[npl].c_str()),  1000, 700);
      TWAvg_canv_2D[npl] = new TCanvas(Form("TWAvg2D_%d", npl), Form("TWAvg2D, plane %s", pl_names[npl].c_str()),  1000, 700);
      Diff_TWDistTrkPos_canv[npl] = new TCanvas(Form("Diff_TrkDist_%d", npl), Form("Diff_TrkDist, plane %s", pl_names[npl].c_str()),  1000, 700);

      if (npl==0 || npl==1 || npl==2) {
	TWCorr_v_TrkPos_canv[npl]->Divide(4,4);
	TWDiff_v_TrkPos_canv[npl]->Divide(4,4);
	TWAvg_canv[npl]->Divide(4,4);
	TWAvg_canv_2D[npl]->Divide(4,4);
	Diff_TWDistTrkPos_canv[npl]->Divide(4,4);
      }
      
      else if (npl==3) {
	TWCorr_v_TrkPos_canv[npl]->Divide(7,3);
	TWDiff_v_TrkPos_canv[npl]->Divide(7,3);
	TWAvg_canv[npl]->Divide(7,3);
	TWAvg_canv_2D[npl]->Divide(7,3);
	Diff_TWDistTrkPos_canv[npl]->Divide(7,3);
      }
    
      //Loop over plane side
      for (Int_t side = 0; side < SIDES; side++)
	{	  
	  //Create Canvas
	  TWUnCorr_canv[npl][side] = new TCanvas(Form("TWUnCorrCanv%d%d", npl, side), Form("plane %s_%s", pl_names[npl].Data(), side_names[side].Data()),  1000, 700);
	  TWCorr_canv[npl][side] = new TCanvas(Form("TWCorrCanv%d%d", npl, side), Form("plane %s_%s", pl_names[npl].Data(), side_names[side].Data()),  1000, 700);

	  //Divide Canvas
	  if (npl==0 || npl==1 || npl==2) {
	    TWUnCorr_canv[npl][side]->Divide(4,4);
	    TWCorr_canv[npl][side]->Divide(4,4);
	  }
	  else if (npl==3) {
	    TWUnCorr_canv[npl][side]->Divide(7,3);
	    TWCorr_canv[npl][side]->Divide(7,3);
	  }
	  
	  Int_t fit_status;
	  
	  //Loop over pmt
	  for (Int_t ipmt = 0; ipmt < maxPMT[npl]; ipmt++)
	    {	        
	     
	      TWUnCorr_canv[npl][side]->cd(ipmt+1);
	      h2Hist_TW_UnCorr[npl][side][ipmt]->Draw("colz");
	      
	      TWCorr_canv[npl][side]->cd(ipmt+1);
	      h2Hist_TW_Corr[npl][side][ipmt]->Draw("colz");
	      fit_status=-1;
	      
	      //Require ONLY one side
	      if (side==0)
		{
		  
		  TWCorr_v_TrkPos_canv[npl]->cd(ipmt+1);
		  h2Hist_TW_Corr_v_TrkPos[npl][ipmt]->Draw("colz");		  
		   
		  //Fit TW Corr Time vs. Trk Pos
		  if (npl==0)
		    {
		      fit_status = h2Hist_TW_Corr_v_TrkPos[npl][ipmt]->Fit("fit1x", "QR");
		    }
		  else if (npl==1)
		    {   
		      determineFitRange = h2Hist_TW_Corr_v_TrkPos[npl][ipmt]->ProjectionX();
		      nEvts = determineFitRange->GetEntries();
		      if(nEvts > 2500) {nFitCut = 10;} else {nFitCut = 1;}
		      minFitBin = determineFitRange->FindFirstBinAbove(nFitCut,1,1,-1);
		      minFitVal = determineFitRange->GetBinCenter(minFitBin);
		      maxFitBin = determineFitRange->FindLastBinAbove(nFitCut,1,1,-1);
		      maxFitVal = determineFitRange->GetBinCenter(maxFitBin);
		      if(maxFitBin==-1 || minFitBin==-1) {
			fit_status =  h2Hist_TW_Corr_v_TrkPos[npl][ipmt]->Fit("fit1y", "QR");     
		      } else {
			fit_status =  h2Hist_TW_Corr_v_TrkPos[npl][ipmt]->Fit("fit1y", "","",minFitVal,maxFitVal);
		      }     
		    } 
		  else if (npl==2)
		    {                                                                                               
		      determineFitRange = h2Hist_TW_Corr_v_TrkPos[npl][ipmt]->ProjectionX();
		      nEvts = determineFitRange->GetEntries();
		      if(nEvts > 2500) {nFitCut = 10;} else {nFitCut = 1;}
		      minFitBin = determineFitRange->FindFirstBinAbove(nFitCut,1,1,-1);
		      minFitVal = determineFitRange->GetBinCenter(minFitBin);
		      maxFitBin = determineFitRange->FindLastBinAbove(nFitCut,1,1,-1);
		      maxFitVal = determineFitRange->GetBinCenter(maxFitBin);
		      if(maxFitBin==-1 || minFitBin==-1) {
			fit_status = h2Hist_TW_Corr_v_TrkPos[npl][ipmt]->Fit("fit2x", "QR");                         
		      } else {
			fit_status = h2Hist_TW_Corr_v_TrkPos[npl][ipmt]->Fit("fit2x", "", "",minFitVal,maxFitVal);                         
		      }
		    }
		  else if (npl==3)
		    {   
		      determineFitRange = h2Hist_TW_Corr_v_TrkPos[npl][ipmt]->ProjectionX();
		      nEvts = determineFitRange->GetEntries();
		      if(nEvts > 2500) {nFitCut = 10;} else {nFitCut = 1;}
		      minFitBin = determineFitRange->FindFirstBinAbove(nFitCut,1,1,-1);
		      minFitVal = determineFitRange->GetBinCenter(minFitBin);
		      maxFitBin = determineFitRange->FindLastBinAbove(nFitCut,1,1,-1);
		      maxFitVal = determineFitRange->GetBinCenter(maxFitBin);
		      if(maxFitBin==-1 || minFitBin==-1) {
			fit_status = h2Hist_TW_Corr_v_TrkPos[npl][ipmt]->Fit("fit2y", "QR");     
		      } else {
			fit_status = h2Hist_TW_Corr_v_TrkPos[npl][ipmt]->Fit("fit2y", "","",minFitVal,maxFitVal);     
		      }
		    } 

		  TWAvg_canv_2D[npl]->cd(ipmt+1);
		  h2Hist_TWAvg_v_TrkPos[npl][ipmt]->Draw("colz");
		  
		  //Draw 1D TWAvg Histos
		  TWAvg_canv[npl]->cd(ipmt+1);
		  h1Hist_TWAvg[npl][ipmt]->SetLineColor(kBlack);
		  h1Hist_TWAvg_CUT[npl][ipmt]->SetLineColor(kRed);
		  h1Hist_TWAvg[npl][ipmt]->Draw();
		  h1Hist_TWAvg_CUT[npl][ipmt]->Draw("same");
		  		  	  
		  if (npl==0) {
		    phodo_velArr[npl][ipmt] = 1./(fit1x->GetParameter(0));

		    //C. Yero Nov 09, 2021  | Added Min/Max velocity constraints
		    // (if fit failed "OR" if fit velocity is out-of-range)
		    if(fit_status==-1 || (phodo_velArr[npl][ipmt] < phodo_velMin[npl] || phodo_velArr[npl][ipmt] > phodo_velMax[npl]) ) {
		      
		      
		      if(phodo_velArr[npl][ipmt] < phodo_velMin[npl] || phodo_velArr[npl][ipmt] > phodo_velMax[npl]) { 
			cout << Form("PLANE 1X, PADDLE %d : fit velocity, %.3f cm/ns is out-of-range [%.3f, %.3f] cm/ns ", (ipmt+1), phodo_velArr[npl][ipmt], phodo_velMin[npl], phodo_velMax[npl] ) << endl;
			cout <<    "***********************" << endl;
			cout << "" << endl;
			cout << Form("---- PLANE 1X, PADDLE %d : ORIGINAL FIT RESULTS ----", ipmt+1) << endl;                                                                                                                               
			cout << Form("phodo_velocity (1/fit_slope) = %.3f ", phodo_velArr[npl][ipmt]) << endl;                                                                                                                     
			cout << Form("phodo_cable_offset (y-int.) = %.3f", fit1x->GetParameter(1)) << endl;                                                                                                                                                
			cout << "-------------------------------" << endl;   
			cout << "" << endl;
			cout << Form(" Will try re-fitting line with fit function: Y = 1 / (%.3f cm/ns) * X + b", phodo_velSet[npl] ) << endl;
			cout << "" << endl;
		      }
		      else if (fit_status==-1){
			cout << "FIT FAILED FOR . . . " << endl;
			cout << Form("PLANE 1X, PADDLE %d : fit velocity, %.3f cm/ns  ", (ipmt+1), phodo_velArr[npl][ipmt] ) << endl;
			cout << Form(" Will try re-fitting line with fit function: Y = 1 / (%.3f cm/ns) * X + b", phodo_velSet[npl] ) << endl;  
			cout << "" << endl;  
		      }
		      
		      //Re-Fit TW Corr Time vs. Trk Pos		   
		      fit_status = h2Hist_TW_Corr_v_TrkPos[npl][ipmt]->Fit("rfit1x", "QR+");  // The '+' is to overlay fit with previous original fit
		      rfit1x->SetLineColor(kBlack);
		      rfit1x->SetLineStyle(9);
		      
		      cout << Form("---- PLANE 1X, PADDLE %d : RE-FIT RESULTS ----", ipmt+1) << endl;
		      cout << Form("phodo_velocity (1/fit_slope) = %.3f ", phodo_velSet[npl]) << endl;
		      cout << Form("phodo_cable_offset (y-int.) = %.3f", rfit1x->GetParameter(0)) << endl;
		      cout << "-------------------------------" << endl;  
		      cout << "" << endl;
		      
		      TWDiff_v_TrkPos_canv[npl]->cd(ipmt+1);
		      rfit1x->Draw();
		            		      
		      //Set the re-fit results parameter values
		      phodo_velArr[npl][ipmt] = phodo_velSet[npl];
		      phodo_cableArr[npl][ipmt] = rfit1x->GetParameter(0);
		      //phodo_sigArr[npl][ipmt] = phodo_sigArr[npl][ipmt] / (2.*phodo_velSet[npl]);  Jan 04, 2021 (will be re-calculated in Hit Filtering Part 3)
		      
		      //Check fit status of re-fit 
		      if(fit_status==-1) // fit failed a second time (most likely empy data histo, or non-operational channel)
			{
			  phodo_velArr[npl][ipmt] = phodo_velSet[npl];                             
			  phodo_cableArr[npl][ipmt] = 0.0;                                                           
			  phodo_sigArr[npl][ipmt] = 1.0;  
			}
		    }		    
		    
		    // if fit is good (i.e., within range)
		    else {
		      
		      cout << "" << endl;
		      cout << Form("---- PLANE 1X, PADDLE %d : VALID FIT RESULTS ----", ipmt+1) << endl;                                                                                            
                      cout << Form("phodo_velocity (1/fit_slope) = %.3f cm/ns", phodo_velArr[0][ipmt]) << endl;    
		      cout << Form("phodo_cable_offset (y-int.) = %.3f ns", fit1x->GetParameter(1)) << endl; 
		      cout << "-------------------------------" << endl;
		      cout << "" << endl;
		      
		      phodo_cableArr[0][ipmt] = fit1x->GetParameter(1);
		      //phodo_sigArr[0][ipmt] = phodo_sigArr[0][ipmt] / (2.*phodo_velArr[0][ipmt]); Jan 04, 2021 (will be re-calculated in Hit Filtering Part 3)
		    }		  		  
		  }
		  
		  else if (npl==1) { 
		    phodo_velArr[npl][ipmt] = 1./(fit1y->GetParameter(0)); 
		    
		    if(fit_status==-1 || (phodo_velArr[npl][ipmt] < phodo_velMin[npl] || phodo_velArr[npl][ipmt] > phodo_velMax[npl]) ) {
		      
		      if (phodo_velArr[npl][ipmt] < phodo_velMin[npl] || phodo_velArr[npl][ipmt] > phodo_velMax[npl]){
			cout << Form("PLANE 1Y, PADDLE %d : fit velocity, %.3f cm/ns is out-of-range [%.3f, %.3f] cm/ns ", (ipmt+1), phodo_velArr[npl][ipmt], phodo_velMin[npl], phodo_velMax[npl] ) << endl;
			cout <<    "***********************" << endl;
			cout << "" << endl;
			cout << Form("---- PLANE 1Y, PADDLE %d : ORIGINAL FIT RESULTS ----", ipmt+1) << endl;                                                                                                                               
			cout << Form("phodo_velocity (1/fit_slope) = %.3f ", phodo_velArr[npl][ipmt]) << endl;                                                                                                                     
			cout << Form("phodo_cable_offset (y-int.) = %.3f", fit1y->GetParameter(1)) << endl;                                                                                                                                                
			cout << "-------------------------------" << endl;   
			cout << "" << endl;
			cout << Form(" Will try re-fitting line with fit function: Y = 1 / (%.3f cm/ns) * X + b", phodo_velSet[npl] ) << endl;
			cout << "" << endl;
		      }
		      else if (fit_status==-1){                                                                                                
                        cout << "FIT FAILED FOR . . . " << endl;                                                                                                          
                        cout << Form("PLANE 1Y, PADDLE %d : fit velocity, %.3f cm/ns  ", (ipmt+1), phodo_velArr[npl][ipmt] ) << endl;                                     
                        cout << Form(" Will try re-fitting line with fit function: Y = 1 / (%.3f cm/ns) * X + b", phodo_velSet[npl] ) << endl;                                                
                        cout << "" << endl;                                                                                                                                                 
                      }  

		      //Re-Fit TW Corr Time vs. Trk Pos		   
		      fit_status = h2Hist_TW_Corr_v_TrkPos[npl][ipmt]->Fit("rfit1y", "QR+");
		      rfit1y->SetLineColor(kBlack);
		      rfit1y->SetLineStyle(9);
		      
		      cout << Form("---- PLANE 1Y, PADDLE %d : RE-FIT RESULTS ----", ipmt+1) << endl;
		      cout << Form("phodo_velocity (1/fit_slope) = %.3f ", phodo_velSet[npl]) << endl;
		      cout << Form("phodo_cable_offset (y-int.) = %.3f", rfit1y->GetParameter(0)) << endl;
		      cout << "-------------------------------" << endl;  
		      cout << "" << endl;
		      
		      TWDiff_v_TrkPos_canv[npl]->cd(ipmt+1);
		      rfit1y->Draw();
		            		      
		      //Set the parameter values
		      phodo_velArr[npl][ipmt] = phodo_velSet[npl];
		      phodo_cableArr[npl][ipmt] = rfit1y->GetParameter(0) ;
		      //phodo_sigArr[npl][ipmt] = phodo_sigArr[npl][ipmt] / (2.*phodo_velSet[npl]); Jan 04, 2021 (will be re-calculated in Hit Filtering Part 3)
		      
		      //Check fit status of re-fit                                                                                
                      if(fit_status==-1) // fit failed a second time (most likely empy data histo, or non-operational channel)      
                        {                                                                                                           
                          phodo_velArr[npl][ipmt] = phodo_velSet[npl];                                                             
                          phodo_cableArr[npl][ipmt] = 0.0;                                                                          
                          phodo_sigArr[npl][ipmt] = 1.0;                                                                                 
                        } 

		    }		    
		    
		    else {

		      cout << "" << endl;
		      cout << Form("---- PLANE 1Y, PADDLE %d : VALID FIT RESULTS ----", ipmt+1) << endl;                                                                                            
                      cout << Form("phodo_velocity (1/fit_slope) = %.3f cm/ns", phodo_velArr[npl][ipmt]) << endl;    
		      cout << Form("phodo_cable_offset (y-int.) = %.3f ns", fit1y->GetParameter(1)) << endl; 
		      cout << "-------------------------------" << endl;
		      cout << "" << endl;
		      
		      phodo_cableArr[npl][ipmt] = fit1y->GetParameter(1);
		      //phodo_sigArr[npl][ipmt] = phodo_sigArr[npl][ipmt] / (2.*phodo_velArr[npl][ipmt]); Jan 04, 2021 (will be re-calculated in Hit Filtering Part 3)
		    }

		  }

		  
		  else if (npl==2) {
		    
		    phodo_velArr[npl][ipmt] = 1./(fit2x->GetParameter(0)); 

		     if(fit_status==-1 || (phodo_velArr[npl][ipmt] < phodo_velMin[npl] || phodo_velArr[npl][ipmt] > phodo_velMax[npl]) ) {
		      
		       if(phodo_velArr[npl][ipmt] < phodo_velMin[npl] || phodo_velArr[npl][ipmt] > phodo_velMax[npl]) {
			 cout << Form("PLANE 2X, PADDLE %d : fit velocity, %.3f cm/ns is out-of-range [%.3f, %.3f] cm/ns ", (ipmt+1), phodo_velArr[npl][ipmt], phodo_velMin[npl], phodo_velMax[npl] ) << endl;
			 cout <<    "***********************" << endl;
			 cout << "" << endl;
			 cout << Form("---- PLANE 2X, PADDLE %d : ORIGINAL FIT RESULTS ----", ipmt+1) << endl;                                                                                                                               
			 cout << Form("phodo_velocity (1/fit_slope) = %.3f ", phodo_velArr[npl][ipmt]) << endl;                                                                                                                     
			 cout << Form("phodo_cable_offset (y-int.) = %.3f", fit2x->GetParameter(1)) << endl;                                                                                                                                                
			 cout << "-------------------------------" << endl;   
			 cout << "" << endl;
			 cout << Form(" Will try re-fitting line with fit function: Y = 1 / (%.3f cm/ns) * X + b", phodo_velSet[npl] ) << endl;
			 cout << "" << endl;
		       }
		       else if (fit_status==-1){    
			 cout << "FIT FAILED FOR . . . " << endl;                                                              
			 cout << Form("PLANE 2X, PADDLE %d : fit velocity, %.3f cm/ns  ", (ipmt+1), phodo_velArr[npl][ipmt] ) << endl;                                 
			 cout << Form(" Will try re-fitting line with fit function: Y = 1 / (%.3f cm/ns) * X + b", phodo_velSet[npl] ) << endl;                    
			 cout << "" << endl;                                                                                                                                 
		       }  
		       
		       //Re-Fit TW Corr Time vs. Trk Pos		   
		       fit_status = h2Hist_TW_Corr_v_TrkPos[npl][ipmt]->Fit("rfit2x", "QR+");
		       rfit2x->SetLineColor(kBlack);
		       rfit2x->SetLineStyle(9);
		      
		      cout << Form("---- PLANE 2X, PADDLE %d : RE-FIT RESULTS ----", ipmt+1) << endl;
		      cout << Form("phodo_velocity (1/fit_slope) = %.3f ", phodo_velSet[npl]) << endl;
		      cout << Form("phodo_cable_offset (y-int.) = %.3f", rfit2x->GetParameter(0)) << endl;
		      cout << "-------------------------------" << endl;  
		      cout << "" << endl;
		      
		      TWDiff_v_TrkPos_canv[npl]->cd(ipmt+1);
		      rfit2x->Draw();
		            		      
		      //Set the parameter values
		      phodo_velArr[npl][ipmt] = phodo_velSet[npl];
		      phodo_cableArr[npl][ipmt] = rfit2x->GetParameter(0) ;
		      //phodo_sigArr[npl][ipmt] = phodo_sigArr[npl][ipmt] / (2.*phodo_velSet[npl]); Jan 04, 2021 (will be re-calculated in Hit Filtering Part 3)

		      //Check fit status of re-fit                                                                                     
                      if(fit_status==-1) // fit failed a second time (most likely empy data histo, or non-operational channel)     
                        {                                                                                                         
                          phodo_velArr[npl][ipmt] = phodo_velSet[npl];                                                         
                          phodo_cableArr[npl][ipmt] = 0.0;                                                                          
                          phodo_sigArr[npl][ipmt] = 1.0;                                                                              
                        } 
		      
		     }		    
		    		    
		    else {
		      cout << Form("---- PLANE 2X, PADDLE %d : VALID FIT RESULTS ----", ipmt+1) << endl;           
                      cout << Form("phodo_velocity (1/fit_slope) = %.3f cm/ns", phodo_velArr[npl][ipmt]) << endl;    
		      cout << Form("phodo_cable_offset (y-int.) = %.3f ns", fit2x->GetParameter(1)) << endl; 
		      cout << "-------------------------------" << endl;
		      cout << "" << endl;
		      
		    phodo_cableArr[npl][ipmt] = fit2x->GetParameter(1);
		    //phodo_sigArr[npl][ipmt] = phodo_sigArr[npl][ipmt] / (2.*phodo_velArr[npl][ipmt]); Jan 04, 2021 (will be re-calculated in Hit Filtering Part 3)

		    }		     

		  }
		  
		  else if (npl==3) {
		    
		    phodo_velArr[npl][ipmt] = 1./(fit2y->GetParameter(0));
		    
		    if(fit_status==-1 || (phodo_velArr[npl][ipmt] < phodo_velMin[npl] || phodo_velArr[npl][ipmt] > phodo_velMax[npl]) ) {
		      
		      if(phodo_velArr[npl][ipmt] < phodo_velMin[npl] || phodo_velArr[npl][ipmt] > phodo_velMax[npl]){
			cout << Form("PLANE 2Y, PADDLE %d : fit velocity, %.3f cm/ns is out-of-range [%.3f, %.3f] cm/ns ", (ipmt+1), phodo_velArr[npl][ipmt], phodo_velMin[npl], phodo_velMax[npl] ) << endl;
			cout <<    "***********************" << endl;
			cout << "" << endl;
			cout << Form("---- PLANE 2Y, PADDLE %d : ORIGINAL FIT RESULTS ----", ipmt+1) << endl;                                                                             
			cout << Form("phodo_velocity (1/fit_slope) = %.3f ", phodo_velArr[npl][ipmt]) << endl;                                                                                    
			cout << Form("phodo_cable_offset (y-int.) = %.3f", fit2y->GetParameter(1)) << endl;           
			cout << "-------------------------------" << endl;   
			cout << "" << endl;
			cout << Form(" Will try re-fitting line with fit function: Y = 1 / (%.3f cm/ns) * X + b", phodo_velSet[npl] ) << endl;
			cout << "" << endl;
		      }

		      else if (fit_status==-1){                                                                                                                                                              
			cout << "FIT FAILED FOR . . . " << endl;                                                                                                                                             
			cout << Form("PLANE 2Y, PADDLE %d : fit velocity, %.3f cm/ns  ", (ipmt+1), phodo_velArr[npl][ipmt] ) << endl;                                                                        
			cout << Form(" Will try re-fitting line with fit function: Y = 1 / (%.3f cm/ns) * X + b", phodo_velSet[npl] ) << endl;                                                               
			cout << "" << endl;                                                                                                                                                                  
		      } 

		      //Re-Fit TW Corr Time vs. Trk Pos		   
		      fit_status = h2Hist_TW_Corr_v_TrkPos[npl][ipmt]->Fit("rfit2y", "QR+");
		      rfit2y->SetLineColor(kBlack);
		      rfit2y->SetLineStyle(9);
		      
		      cout << Form("---- PLANE 2Y, PADDLE %d : RE-FIT RESULTS ----", ipmt+1) << endl;
		      cout << Form("phodo_velocity (1/fit_slope) = %.3f ", phodo_velSet[npl]) << endl;
		      cout << Form("phodo_cable_offset (y-int.) = %.3f", rfit2y->GetParameter(0)) << endl;
		      cout << "-------------------------------" << endl;  
		      cout << "" << endl;
		      
		      TWDiff_v_TrkPos_canv[npl]->cd(ipmt+1);
		      rfit2y->Draw();
		            		      
		      //Set the parameter values
		      phodo_velArr[npl][ipmt] = phodo_velSet[npl];
		      phodo_cableArr[npl][ipmt] = rfit2y->GetParameter(0) ;
		      //phodo_sigArr[npl][ipmt] = phodo_sigArr[npl][ipmt] / (2.*phodo_velSet[npl]); Jan 04, 2021 (will be re-calculated in Hit Filtering Part 3)
		      
		      //Check fit status of re-fit                                           
                      if(fit_status==-1) // fit failed a second time (most likely empy data histo, or non-operational channel)          
                        {                                                                                                             
                          phodo_velArr[npl][ipmt] = phodo_velSet[npl];                                   
                          phodo_cableArr[npl][ipmt] = 0.0;                                                                               
                          phodo_sigArr[npl][ipmt] = 1.0;                                                                              
                        } 
		    
		    }
		    
		    else {	
		      cout << Form("---- PLANE 2Y, PADDLE %d : VALID FIT RESULTS ----", ipmt+1) << endl;                            
		      cout << Form("phodo_velocity (1/fit_slope) = %.3f cm/ns", phodo_velArr[npl][ipmt]) << endl;    
		      cout << Form("phodo_cable_offset (y-int.) = %.3f ns", fit2y->GetParameter(1)) << endl; 
		      cout << "-------------------------------" << endl;
		      cout << "" << endl;
		      
		      phodo_cableArr[npl][ipmt] = fit2y->GetParameter(1);
		      //phodo_sigArr[npl][ipmt] = phodo_sigArr[npl][ipmt] / (2.*phodo_velArr[npl][ipmt]); Jan 04, 2021 (will be re-calculated in Hit Filtering Part 3)
		      
		    }
		    
		  }
		  
	
		} //end single SIDE requirement
	      
	    } //end pmt loop

	} //end side loop
  
    } //end plane loop
 


 //Create output root file where histograms will be stored
 TFile *outCANVAS = new TFile(Form("HodoCanvasPlots_%d.root", runNUM), "recreate");
 outCANVAS->cd();
 //Write Canvases to file
 for(int ipl = 0; ipl < PLANES; ipl++) {
   cout << "Plane: " << ipl << endl;
   //Loop over hodo side
   for (Int_t side = 0; side < SIDES; side++) {
     cout << "Side: " << side << endl;
     TWUnCorr_canv[ipl][side]->Write();
     TWCorr_canv[ipl][side]->Write();
   }
   TWAvg_canv[ipl]->Write();
   TWAvg_canv_2D[ipl]->Write();
   Diff_TWDistTrkPos_canv[ipl]->Write();
   TWCorr_v_TrkPos_canv[ipl]->Write();
   TWDiff_v_TrkPos_canv[ipl]->Write();
 }
 outCANVAS->Close();
 

 //************************************//
 //    THIRD PASS OF EVENT LOOP       //
 //************************************//
  cout << "Initializing 3rd Pass of Event Loop: " << endl;

  //Loop over all entries
  for(Long64_t i=0; i<nentries; i++)
    {
      
      T->GetEntry(i);  

      pcal = pcal_etrkNorm>etrknrm_low_cut;
      pngcer = pngcer_npeSum>npngcer_npeSum_low_cut; //C.Y. Dec 12, 2021: put ONLY min cut requirement  |  JM 31-10-21: Added in npeSum > 0 requirement
      pdctrk = pdc_ntrack>0.0;   // Seems to work. One could potentially improve hit filtering further by requiring a single reconstructed track, but efficiency may suffer. 
      betaCut = beta>betanotrack_low_cut&& beta<betanotrack_hi_cut;
      pid_pelec = pcal&&pngcer&&pdctrk;
      
      if (cosmic_flag)  pid_pelec = betaCut&&pdctrk;
      
      // ----------------- "LITE" HIT FILTERING (PART 3) -----------------

      if(pid_pelec && single_hit_flg) // per entry: apply PID cut AND single-hit requirement TO SELECT CLEAN ELECTRONS
	{
	  
	  //Loop over hodo planes
	  for (Int_t npl = 0; npl < PLANES; npl++ )
	    {
	      
	   
	      //Loop over plane side
	      for (Int_t side = 0; side < SIDES; side++)
		{
		  
		  //Loop over pmt
		  for (Int_t ipmt = 0; ipmt < maxPMT[npl]; ipmt++)
		    {	        
		      
		      if(mean_time_flg[i][npl][ipmt]==0) continue; 

		      //Get Standard deviation from initial entry fill
		      StdDev =  h1Hist_TWAvg[npl][ipmt]->GetStdDev();      
		      Mean =  h1Hist_TWAvg[npl][ipmt]->GetMean();      

		      //Add Time Cuts to get rid of kBig - kBig values, which yielded high evt density at zero
//HARD CUT NOTICE
		      if(TdcTimeTWCorr[npl][0][ipmt] < 200. && TdcTimeTWCorr[npl][1][ipmt] < 200.)
			{
			  
			  //require only one side, as a time diff between the two ends of a paddle is taken
			  if (side==0)  
			    {

			        // Apply mean tdc time +/- nsig*StdDev cut to narrow window of "good hits"
			      if ( (((TdcTimeTWCorr[npl][0][ipmt] + TdcTimeTWCorr[npl][1][ipmt])/2.) > (Mean-nSig*StdDev)) &&  (((TdcTimeTWCorr[npl][0][ipmt] + TdcTimeTWCorr[npl][1][ipmt])/2.) < (Mean+nSig*StdDev)))
				{

				  if (npl==0 || npl==2)
				    {
				      //C.Y. Jan 04, 2021 | Calculate Minty Fresh DiffDistTWCorr (using the update parameters from the velocity fits done earlier in this code) 
				      DiffDistTWCorr[npl][ipmt] = phodo_velArr[npl][ipmt] * 0.5 * (TdcTimeTWCorr[npl][1][ipmt] - 2*phodo_cableArr[npl][ipmt] - TdcTimeTWCorr[npl][0][ipmt]);
				      h2Hist_TWDiff_v_TrkPos[npl][ipmt]->Fill(TrackYPos[npl],  DiffDistTWCorr[npl][ipmt]-TrackYPos[npl]);
				      h1Hist_TWDiffTrkPos[npl][ipmt]->Fill(DiffDistTWCorr[npl][ipmt] - TrackYPos[npl]);                           

				    }
				  else if (npl==1 || npl==3)
				    {
				      //C.Y. Jan 04, 2021 | Calculate Minty Fresh DiffDistTWCorr (using the update parameters from the velocity fits done earlier in this code) 
				      DiffDistTWCorr[npl][ipmt] = phodo_velArr[npl][ipmt] * 0.5 * (TdcTimeTWCorr[npl][1][ipmt] - 2*phodo_cableArr[npl][ipmt] - TdcTimeTWCorr[npl][0][ipmt]);
				      h2Hist_TWDiff_v_TrkPos[npl][ipmt]->Fill(TrackXPos[npl],  DiffDistTWCorr[npl][ipmt]-TrackXPos[npl]);
				      h1Hist_TWDiffTrkPos[npl][ipmt]->Fill(DiffDistTWCorr[npl][ipmt] - TrackXPos[npl]);                    

				    }
				  
				} // //end meantime +/- nSig*StdDev CUT of TW Corr Time

			      
			    } //end single-side requirement

			} //end time cuts

		    } //end pmt loop

		} //end side loop

	    } //end plane loop

	} // end pid-electron AND single plane hit CUTS (IMPORTANT ! ! ! )

       cout << std::setprecision(2) << double(i) / nentries * 100. << "  % " << std::flush << "\r";
       
    } //end entry loop

  // ---- Calculate updated hodoscope sigma parameters ----

  //Loop over hodo planes
  for (Int_t npl = 0; npl < PLANES; npl++ )
    {
      //NOTE: no side requirement, as this parameter is per paddle (not pmt side)
      
      //Loop over pmt
      for (Int_t ipmt = 0; ipmt < maxPMT[npl]; ipmt++)
	{	        

	  // Calculate updated hodoscope sigma parameters
	  phodo_sigArr[npl][ipmt] = h1Hist_TWDiffTrkPos[npl][ipmt]->GetStdDev();
	  phodo_sigArr[npl][ipmt] = phodo_sigArr[npl][ipmt] / (2.*phodo_velArr[npl][ipmt]);	  

	  // Draw remaining histograms dependent on the calculated 'DiffDistTWCorr' variable
	  TWDiff_v_TrkPos_canv[npl]->cd(ipmt+1);
	  h2Hist_TWDiff_v_TrkPos[npl][ipmt]->Draw("colz");  

	  Diff_TWDistTrkPos_canv[npl]->cd(ipmt+1);
	  h1Hist_TWDiffTrkPos[npl][ipmt]->Draw();

	}
    }


  //-----------------------
 
 /************WRITE FIT RESULTS TO PARAMETER FILE***************/
 
 ofstream outPARAM;
 outPARAM.open(Form("phodo_Vpcalib_%d.param", runNUM));
 
 outPARAM << "; SHMS Hodoscope Parameter File Containing propagation velocities per paddle " << endl;
 outPARAM << "; and signal cable time diff. offsets per paddle " << endl;
 outPARAM << Form("; Run %d ", runNUM) << endl;
 outPARAM << " " << endl;
 outPARAM << " " << endl;
 
 outPARAM << ";Propagation Velocity Per Paddle" << endl;
 outPARAM << "; " << setw(20) << "1x " << setw(19) << "1y " << setw(16) << "2x " << setw(16) << "2y " << endl;
 outPARAM << "phodo_velFit = ";
   
 //--------Write Velocity Parameters to Param File-----
 for (Int_t ipmt = 0; ipmt < 21; ipmt++){
   if (ipmt==0){
     outPARAM << phodo_velArr[0][ipmt] << ", " << setw(15) << phodo_velArr[1][ipmt] << ", " << setw(15) << phodo_velArr[2][ipmt] << ", " << setw(15) << phodo_velArr[3][ipmt] << fixed << endl; 
   }
   else{
     outPARAM << setw(23) << phodo_velArr[0][ipmt] << ", " << setw(15) << phodo_velArr[1][ipmt] << ", " << setw(15) << phodo_velArr[2][ipmt] << ", " << setw(15) << phodo_velArr[3][ipmt] << fixed << endl; 
   }
 } //end pmt loop
  
 outPARAM << " " << endl;
 outPARAM << " " << endl;
 outPARAM << " " << endl;
 
 outPARAM << ";PMTs Signal Cable Time Diff. Per Paddle" << endl;
 outPARAM << "; " << setw(20) << "1x " << setw(19) << "1y " << setw(16) << "2x " << setw(16) << "2y " << endl;
 outPARAM << "phodo_cableFit = ";
 
 //Write Cable Length Time Diff. Parameters to Param File
 for (Int_t ipmt = 0; ipmt < 21; ipmt++){
   if (ipmt==0){
     outPARAM << phodo_cableArr[0][ipmt] << ", " << setw(15) << phodo_cableArr[1][ipmt] << ", " << setw(15) << phodo_cableArr[2][ipmt] << ", " << setw(15) << phodo_cableArr[3][ipmt] << fixed << endl; 
   }
   else{
     outPARAM << setw(26) << phodo_cableArr[0][ipmt] << ", " << setw(15) << phodo_cableArr[1][ipmt] << ", " << setw(15) << phodo_cableArr[2][ipmt] << ", " << setw(15) << phodo_cableArr[3][ipmt] << fixed << endl; 
   }
 }

 outPARAM << " " << endl;
 outPARAM << " " << endl;
 outPARAM << " " << endl;
 
 outPARAM << ";PMTs Time Diff. Sigma Parameters" << endl;
 outPARAM << "; " << setw(20) << "1x " << setw(19) << "1y " << setw(16) << "2x " << setw(16) << "2y " << endl;
 outPARAM << "phodo_PosSigma = ";

 //Write Sigma Parameters to file
 for (Int_t ipmt = 0; ipmt < 21; ipmt++){
   if (ipmt==0){
     outPARAM << phodo_sigArr[0][ipmt] << ", " << setw(15) << phodo_sigArr[1][ipmt] << ", " << setw(15) << phodo_sigArr[2][ipmt] << ", " << setw(15) << phodo_sigArr[3][ipmt] << fixed << endl; 
   }
   else{
     outPARAM << setw(26) << phodo_sigArr[0][ipmt] << ", " << setw(15) << phodo_sigArr[1][ipmt] << ", " << setw(15) << phodo_sigArr[2][ipmt] << ", " << setw(15) << phodo_sigArr[3][ipmt] << fixed << endl; 
   }
 }

 outPARAM << " " << endl;
 outPARAM << " " << endl;
 outPARAM << "phodo_NegSigma = ";

 //Write Sigma Parameters to file
 for (Int_t ipmt = 0; ipmt < 21; ipmt++){
   if (ipmt==0){
     outPARAM << phodo_sigArr[0][ipmt] << ", " << setw(15) << phodo_sigArr[1][ipmt] << ", " << setw(15) << phodo_sigArr[2][ipmt] << ", " << setw(15) << phodo_sigArr[3][ipmt] << fixed << endl; 
   }
   else{
     outPARAM << setw(26) << phodo_sigArr[0][ipmt] << ", " << setw(15) << phodo_sigArr[1][ipmt] << ", " << setw(15) << phodo_sigArr[2][ipmt] << ", " << setw(15) << phodo_sigArr[3][ipmt] << fixed << endl; 
   }
 }
 
   
 cout << "FINISHED Getting Vp and Cable Fits . . . " << endl;
 cout << "Starting the code to fit Hodo Matrix . . . " << endl;                                                                                                                                                                                                                                                                                                                                
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                         
 /**********BEGIN CODE TO FIT TIMING CORRECTIONS PER PADDLE, the phodo_LCoeff PARAMETERS**************/
// These are the time offsets associated with the meantime of the pos and neg TDCs of a paddle. 
// The technique to determine these parameters is explained on pages 5-7 of the hodo calibration document v2 at https://hallcweb.jlab.org/doc-private/ShowDocument?docid=970 . 
// The hodo hit filtering in this section has not been updated but is clean. However, the efficiency may be low depending on how wide the hcana window is. It can easily be improved if needed. 
 
  for(Long64_t i=0; i<nentries; i++)
    {
      T->GetEntry(i);  
      
      //C.Y. Feb 16, 2022 | added pid cuts for use in "LITE" HIT FILTERING of this section of code
      
      pcal = pcal_etrkNorm>etrknrm_low_cut;
      pngcer = pngcer_npeSum>npngcer_npeSum_low_cut;
      pdctrk = pdc_ntrack>0.0; 
      betaCut = beta>betanotrack_low_cut&& beta<betanotrack_hi_cut;
      pid_pelec = pcal&&pngcer&&pdctrk;

      //Define some minimum requirements
      Bool_t x1_hit = hod_nhits[0] == 1;
      Bool_t y1_hit = hod_nhits[1] == 1;
      Bool_t x2_hit = hod_nhits[2] == 1;
      Bool_t y2_hit = hod_nhits[3] == 1;
    

      Bool_t hodTrk = TrackXPos[0]<200&&TrackYPos[0]<200&&
		      TrackXPos[1]<200&&TrackYPos[1]<200&&
		      TrackXPos[2]<200&&TrackYPos[2]<200&&
		      TrackXPos[3]<200&&TrackYPos[3]<200;
      //betaCut=kTRUE;
      //if (cosmic_flag)  betaCut = beta>betanotrack_low_cut&& beta<betanotrack_hi_cut;
      
      //C.Y. Re-write above beta cut code, same as it is done in other parts of this code
      if (cosmic_flag) pid_pelec = betaCut&&pdctrk;

      //require each plane to have ONLY a SINGLE HIT, and hod track coord. to be reasonable (NOT kBig)
      //if (x1_hit&&y1_hit&&x2_hit&&y2_hit&&hodTrk&&betaCut) 
      
      // ----------------- "LITE HIT FILTERING (PART 4)" ----------------
      // C.Y. Feb 26, 2022 | added "LITE" hit filtering 
      // in hopes of reducing possible background that might be introduced in the LCoeff parameters extracted from the matrix fit.
      
      if(pid_pelec && single_hit_flg && hodTrk)
	  {
	    
	    //goodhit: If both ends of a paddle had a tdc hit
	    Bool_t goodhit[PLANES] = {0, 0, 0, 0};
	    
	    //Reset goodhits counter
	    cnt = 0;
	    
	    //----------------GOOD HITS Counter--------------------------------------------------------

	    //Loop over hodo planes
	    for (Int_t npl = 0; npl < PLANES; npl++ )
	      {
		
		//loop over paddles
		for (Int_t bar = 1; bar <= maxPMT[npl]; bar++ )
		  {
		    
		    
		    if (mean_time_flg[i][npl][bar-1]==0) continue; 
		    
		    //Dec. 17, 2021 C.Y.  increase the cuts, as it seems after hodo3of4 trigger alignment, this needed to be expanded to at least 125
		    // Also,  why is it we are only applying a upper cut, and NOT a lower cut?
		    //require good tdc hit on both ends 
//HARD CUT NOTICE
		    goodhit[npl] =  TdcTimeTWCorr[npl][0][bar-1]<125.&&TdcTimeTWCorr[npl][1][bar-1]<125.;
		    
		    //count if each plane had good tdc hit
		    if (goodhit[npl]) {
		      		      
		      //Define good paddle hit for each plane (varying from 1->52 paddles), if used as arry indx, do (good_pad-1)
		      good_pad[npl] = refPad[npl] + bar;

		      //Get the Good +/- TW Corr TdcTime
		      good_TW_pos[npl] = TdcTimeTWCorr[npl][0][bar-1];
		      good_TW_neg[npl] = TdcTimeTWCorr[npl][1][bar-1] - 2*phodo_cableArr[npl][bar-1]; //IMPORTANT: Apply cable time
		                                                                                   //correction obtained from fits

		      //Get the Track Coordinates
		      x[npl] = TrackXPos[npl];
		      y[npl] = TrackYPos[npl];
		      zCorr[npl] = z[npl][bar-1]; 
		      
		      //goodhit counter
		      cnt = cnt+1;
		      

		    } //end goodhit requirement
		    
		  } //end paddle loop
		
	      } //end plane loop
	    
	    //--------------------------------------------------------------------------------------------------
	    
	    //require all 4 hod planes to have a good hit
	    if (cnt==4 && ngood < evtNUM) {	     
	      
	      ngood = ngood + 1; //good event counter
              if ( ngood == evtNUM) cout << " Reach limit of filling matrix " << endl;
	      //Define all 6 ith-rows per event, each row will correspond to a linear equation
	      row1 = 6*(ngood)-6;
	      row2 = 6*(ngood)-5;
	      row3 = 6*(ngood)-4;
	      row4 = 6*(ngood)-3;
	      row5 = 6*(ngood)-2;
	      row6 = 6*(ngood)-1;

	      //Retrieve Track Coordinates
	      x1 = x[0], y1 = y[0], z1 = zCorr[0];       //Plane 1X
	      x2 = x[1], y2 = y[1], z2 = zCorr[1];       //Plane 1Y
	      x3 = x[2], y3 = y[2], z3 = zCorr[2];       //Plane 2X
	      x4 = x[3], y4 = y[3], z4 = zCorr[3];       //Plane 2Y

	      //Get Average TDC TW Corr Time between two ends of a paddle
	      t1 = 0.5 * (good_TW_pos[0] +  good_TW_neg[0]);     //Plane 1X
	      t2 = 0.5 * (good_TW_pos[1] +  good_TW_neg[1]);     //Plane 1Y
	      t3 = 0.5 * (good_TW_pos[2] +  good_TW_neg[2]);     //Plane 2X
	      t4 = 0.5 * (good_TW_pos[3] +  good_TW_neg[3]);     //Plane 2Y          
                                           	                                                                                                      	                                                                                            
	      
	      //Get all distance/velocity combinations between any two planes
	      D12 = -sqrt( pow((x2-x1),2) + pow((y2-y1),2) + pow((z2-z1),2));
	      D13 = -sqrt( pow((x3-x1),2) + pow((y3-y1),2) + pow((z3-z1),2));
	      D14 = -sqrt( pow((x4-x1),2) + pow((y4-y1),2) + pow((z4-z1),2));
	      D23 = -sqrt( pow((x3-x2),2) + pow((y3-y2),2) + pow((z3-z2),2));
	      D24 = -sqrt( pow((x4-x2),2) + pow((y4-y2),2) + pow((z4-z2),2));
	      D34 = -sqrt( pow((x4-x3),2) + pow((y4-y3),2) + pow((z4-z3),2));
	                               

	      //Define the components of the b-vector (Lambda * xVec = bVev)
	      b12 = D12/vp - (t1 - t2);
	      b13 = D13/vp - (t1 - t3);
	      b14 = D14/vp - (t1 - t4);
	      b23 = D23/vp - (t2 - t3);
	      b24 = D24/vp - (t2 - t4);
	      b34 = D34/vp - (t3 - t4);
	      	      
	      //Fill the Lambda Coefficient Matrix
	      lambda[row1][good_pad[0]-1] = 1.;
	      lambda[row2][good_pad[0]-1] = 1.;
	      lambda[row3][good_pad[0]-1] = 1.;
      
	      lambda[row4][good_pad[1]-1] = 1.;
	      lambda[row5][good_pad[1]-1] = 1.;
	      lambda[row6][good_pad[2]-1] = 1.;

	      lambda[row1][good_pad[1]-1] = -1.;
	      lambda[row2][good_pad[2]-1] = -1.;
	      lambda[row3][good_pad[3]-1] = -1.;
      
	      lambda[row4][good_pad[2]-1] = -1.;
	      lambda[row5][good_pad[3]-1] = -1.;
	      lambda[row6][good_pad[3]-1] = -1.;
	      

	      //Set Reference Paddle (1X7) lambda to ZERO
	      if (good_pad[0]==7)
		{
		  lambda[row1][6] = 0.;
		  lambda[row2][6] = 0.;
		  lambda[row3][6] = 0.;
		}
	      

	      
	      //Fill bVec components
	      //Planes 0, 1  
	      bVec[good_pad[0]-1]+= lambda[row1][good_pad[0]-1]*b12;
	      bVec[good_pad[1]-1]+= lambda[row1][good_pad[1]-1]*b12;
	      //Planes 0, 2  
	      bVec[good_pad[0]-1]+= lambda[row2][good_pad[0]-1]*b13;
	      bVec[good_pad[2]-1]+= lambda[row2][good_pad[2]-1]*b13;
	      //Planes 0, 3  
	      bVec[good_pad[0]-1]+= lambda[row3][good_pad[0]-1]*b14;
	      bVec[good_pad[3]-1]+= lambda[row3][good_pad[3]-1]*b14;
	      //Planes 1, 2  
	      bVec[good_pad[1]-1]+= lambda[row4][good_pad[1]-1]*b23;
	      bVec[good_pad[2]-1]+= lambda[row4][good_pad[2]-1]*b23;
	      //Planes 1, 3  
	      bVec[good_pad[1]-1]+= lambda[row5][good_pad[1]-1]*b24;
	      bVec[good_pad[3]-1]+= lambda[row5][good_pad[3]-1]*b24;
	      //Planes 2, 3  
	      bVec[good_pad[2]-1]+= lambda[row6][good_pad[2]-1]*b34;
	      bVec[good_pad[3]-1]+= lambda[row6][good_pad[3]-1]*b34;
	      
	    
	    } //end good event requirement

	  } //end single hit requirement
          
	cout << std::setprecision(2) << double(i) / nentries * 100. << "  % " << std::flush << "\r";

    } //end entry loop

  cout << " Number of events in fit = " << ngood << endl;
  //Fill each matrix element Ay (61rows,61cols) with the lambda coefficients
    for (Int_t ievt = 0; ievt<6*ngood; ievt++)
      {
	for(Int_t ipar=0; ipar<npar; ipar++)
	  {
	    for( Int_t jpar=0; jpar<npar; jpar++)
	      {
		Ay[ipar][jpar] += lambda[ievt][ipar] * lambda[ievt][jpar];
	      }
	  }
      }

    cout << "Starting Single Value Decomposition . . . " << endl;
    //----Use 'Single Value Decomposition' to Solve Ax = b linear system------
    TDecompSVD lamD(Ay);
    lamD.SetMatrix(Ay);                  //set matrix to be decomposed into A = USV^{T}
    bool ok; //= lamD.Decompose();                                                   
    //cout << "Decomposition OK? " << ok << endl;                                      
    ok = lamD.Solve(bVec);
    cout << "Solve OK? " << ok << endl;                                      


    //add each element to the 2d array to be written to a param file
    for (int iplane = 0; iplane<PLANES; iplane++)
      {
	for (int ipad = 1; ipad<=maxPMT[iplane]; ipad++)
	  {
	    LCoeff[iplane][ipad-1] = bVec[ipad+refPad[iplane]-1];
	  }
      }
    
    
    cout << "Writing to Parameter File . . . " << endl;
    outPARAM << "" << endl;
    outPARAM << "" << endl;
    outPARAM << "" << endl;
    outPARAM << ";Timing Corrections Per Paddle, where 1X Paddle 7 has been set as the reference paddle" << endl;
    outPARAM << "; " << setw(20) << "1x " << setw(19) << "1y " << setw(16) << "2x " << setw(16) << "2y " << endl;
    outPARAM << "phodo_LCoeff = ";

    //Write Lambda Time Coeff. Parameters to Param File
    for (Int_t ipmt = 0; ipmt < 21; ipmt++)
      {
	if (ipmt==0)
	  {
	    outPARAM << setw(8) << LCoeff[0][ipmt] << ", " << setw(15) << LCoeff[1][ipmt] << ", " << setw(15) << LCoeff[2][ipmt] << ", " << setw(15) << LCoeff[3][ipmt] << fixed << endl; 
	  }
	else
	  {
	    outPARAM << setw(23) << LCoeff[0][ipmt] << ", " << setw(15) << LCoeff[1][ipmt] << ", " << setw(15) << LCoeff[2][ipmt] << ", " << setw(15) << LCoeff[3][ipmt] << fixed << endl; 
	  }
	
      }


    //============================================================
    // C.Y. Jan 22, 2022 | CALCULATE BETA IN THE CALIBRATION CODE
    //============================================================

    //Define useful varibales for beta calculation
    Double_t scinTrnsCoord, scinLongCoord;
    Double_t pathp, pathn;
    Double_t tw_corr_pos, tw_corr_neg;
    Double_t timep, timen;
    Double_t adcamp_pos, adcamp_neg;
    Double_t scin_pos_time, scin_neg_time, scin_time;
    Double_t scin_sigma;
    Double_t zPosition;
    Double_t scinWeight;
   
    // weighted
    Double_t sumW;
    Double_t sumT;
    Double_t sumZ;
    Double_t sumZZ;
    Double_t sumTZ;

    Double_t tmp;
    Double_t tmpDenom;
    Double_t pathNorm;
    Double_t beta_calib;

    // un-weighted
    Double_t sumW_1;
    Double_t sumT_1;
    Double_t sumZ_1; 
    Double_t sumZZ_1; 
    Double_t sumTZ_1; 
    Double_t tmp_1; 
    Double_t tmpDenom_1; 
    Double_t beta_calib_1; 


    //Get hodo calibe parameters
    Double_t fTdc_Thrs =  1200.0; // mV
    Double_t fHodoPos_c2[PLANES][21]   = {0.0};  // "pc2_Pos"
    Double_t fHodoNeg_c2[PLANES][21]   = {0.0}; // "pc2_Neg"
    Double_t fHodo_LCoeff[PLANES][21]  = {0.0};
    Double_t fHodoVelFit[PLANES][21]   = {0.0};
    Double_t fHodoCableFit[PLANES][21] = {0.0};

    // Declare useful histograms to store calibrated beta
    TH1F *H_beta_calib_weighted = new TH1F("H_beta_calib_weighted", "Hodoscope Beta (w/Track + Weight) Calibrated", 100, 0., 1.5);
    TH1F *H_beta_calib_unweighted = new TH1F("H_beta_calib_unweighted", "Hodoscope Beta (w/Track + Weight=1) Calibrated", 100, 0., 1.5);

    
    for(Int_t npl=0; npl < PLANES; npl++)
      {
	for(Int_t ipad=0; ipad<21; ipad++)
	  {
	    
	    // Get TW parameters from the standard param file that gets read by HCANA
	    fHodoPos_c2[npl][ipad]   = GetParam("../../PARAM/SHMS/HODO/phodo_TWcalib.param", "pc2_Pos", npl, ipad, 21 );
	    fHodoNeg_c2[npl][ipad]   = GetParam("../../PARAM/SHMS/HODO/phodo_TWcalib.param", "pc2_Neg", npl, ipad, 21 );
	    fHodo_LCoeff[npl][ipad]  = LCoeff[npl][ipad];
	    fHodoVelFit[npl][ipad]   = phodo_velArr[npl][ipad];
	    fHodoCableFit[npl][ipad] = phodo_cableArr[npl][ipad];
	    
	  }
	
      }
    
    // NOTE: This part of the code will focus on using the hodo parameter files optained so far, and calculate the beta, exactly as it is calculated in hcana/src/THcHodoscope.cxx
    // This is in the testing phase . . . until commented otherwise

    
    cout << "Calculating Hodoscope Beta . . . " << endl;
    // loop over each entry to calculate beta
    for(Long64_t i=0; i<nentries; i++)
      {
	
	T->GetEntry(i);
	
	// --------- "HIT FILTERING LITE (PART 5) ------------"
	pcal = pcal_etrkNorm>etrknrm_low_cut;
	pngcer = pngcer_npeSum>npngcer_npeSum_low_cut;
	pdctrk = pdc_ntrack>0.0; 
	betaCut = beta>betanotrack_low_cut&& beta<betanotrack_hi_cut;
	pid_pelec = pcal&&pngcer&&pdctrk;
	
	if (cosmic_flag)  pid_pelec = betaCut&&pdctrk; 

	 //apply (single-hit + PID) per plane
	if(pid_pelec && single_hit_flg) { 
       

	//reset counters                                                                                                                                      
	sumW = 0.;                                                                                                                              
	sumT = 0.;                                                                                                                                              
	sumZ = 0.;                                                                                                                                                 
	sumZZ = 0.;                                                                                                                                           
	sumTZ = 0.; 

	sumW_1 = 0.;                                                                                                               
        sumT_1 = 0.;                                                                                                                     
        sumZ_1 = 0.;                                                                                                                            
        sumZZ_1 = 0.;                                                                                                                                 
        sumTZ_1 = 0.;  

	//Loop over hodo planes
	for (Int_t npl = 0; npl < PLANES; npl++ )
	  {
	   

	    // ------- define scin Coord. (same as in THcHodoscope.cxx) -----
	    if ( ( npl == 0 ) || ( npl == 2 ) )
	      { 
		scinTrnsCoord = TrackXPos[npl];
		scinLongCoord = TrackYPos[npl];
	      }
	    else if ( ( npl == 1 ) || ( npl == 3 ) )
	      { 
		scinTrnsCoord = TrackYPos[npl];
		scinLongCoord = TrackXPos[npl];
	      }
	    else { return -1; }
	    
	    // --------------------------------------------------------------
	    
	  
	    //loop over pmt
	    for (Int_t ipmt = 0; ipmt < maxPMT[npl]; ipmt++ )
	      {
		
		if (mean_time_flg[i][npl][ipmt]==0) continue; //guarantee tdc meantime is within +/- nSig cut


		//require finite tdc times at both ends of a paddle
	      
		//PMT fADC Pulse Amplitude (mV)
		adcamp_pos = AdcPulseAmp[npl][0][ipmt];
		adcamp_neg = AdcPulseAmp[npl][1][ipmt];

		//PMT TDC Time Time-Walk Uncorrected
		timep = TdcTimeUnCorr[npl][0][ipmt];
		timen = TdcTimeUnCorr[npl][1][ipmt];
		
		
		pathp=scinLongCoord;
		pathn=scinLongCoord;
		
		
		// apply time-walk correction
		tw_corr_pos = 1./pow(adcamp_pos/fTdc_Thrs,fHodoPos_c2[npl][ipmt]) -  1./pow(200./fTdc_Thrs, fHodoPos_c2[npl][ipmt]);
		timep += -tw_corr_pos + fHodo_LCoeff[npl][ipmt]+ pathp/fHodoVelFit[npl][ipmt];		 
		scin_pos_time = timep;
		
		
		tw_corr_neg= 1./pow(adcamp_neg/fTdc_Thrs,fHodoNeg_c2[npl][ipmt]) -  1./pow(200./fTdc_Thrs, fHodoNeg_c2[npl][ipmt]);              
		timen += -tw_corr_neg- 2*fHodoCableFit[npl][ipmt] + fHodo_LCoeff[npl][ipmt]- pathn/fHodoVelFit[npl][ipmt];
		scin_neg_time = timen;
	     
	
		//average corrected tdc time
		scin_time  = ( scin_pos_time + scin_neg_time ) / 2.;
		scin_sigma = phodo_sigArr[npl][ipmt];
		scinWeight = 1. / ( scin_sigma * scin_sigma );
		

		zPosition = z[npl][ipmt];

	     
		//sum over all paddle hits per evntry (since we currently require single-hit per plane, and 4-plane hits, we will alawys sum up to 4)
		sumW  += scinWeight;
		sumT  += scinWeight * scin_time;
		sumZ  += scinWeight * zPosition;
		sumZZ += scinWeight * ( zPosition * zPosition );
		sumTZ += scinWeight * zPosition * scin_time;
		
		tmp = sumW * sumZZ - sumZ * sumZ;
		tmpDenom = sumW * sumTZ - sumZ * sumT;
		
		sumW_1  += 1;    
                sumT_1  += 1 * scin_time;                                                                                         
                sumZ_1  += 1 * zPosition;                                                                                                                  
                sumZZ_1 += 1 * ( zPosition * zPosition );                                                                                       
                sumTZ_1 += 1 * zPosition * scin_time;                                                                                              
                                                                                                                                                          
                tmp_1 = sumW_1 * sumZZ_1 - sumZ_1 * sumZ_1;                                                                                    
                tmpDenom_1 = sumW_1 * sumTZ_1 - sumZ_1 * sumT_1;
        
  
	      } //END paddle loop

	  } // END plane loop
	
    
	if (  TMath::Abs(tmpDenom)  > ( 1 / 10000000000.0 ) ) {
	  
	  //cout << "==== TMath::Abs( tmpDenom ) > ( 1 / 10000000000.0 ) ===" << endl;
	  beta_calib = tmp/tmpDenom;
	  beta_calib_1 = tmp_1/tmpDenom_1;
	 
	  
		
	  pathNorm = TMath::Sqrt( 1. + (pdc_xpfp * pdc_xpfp) +  (pdc_ypfp * pdc_ypfp) );
	  
	  // Take angle into account
	  beta_calib = beta_calib / pathNorm;
	  beta_calib_1 = beta_calib_1 / pathNorm;
	  //cout << "beta_calib / pathNorm = " << printf("%f", (beta_calib/pathNorm)) << endl;
	  beta_calib = beta_calib / 29.979;    // velocity / c
	  beta_calib_1 = beta_calib_1 / 29.979; 
	  
	
	  //Fill weighted histograms
	  H_beta_calib_weighted->Fill(beta_calib);
	  H_beta_calib_unweighted->Fill(beta_calib_1);
	}
	
	} //end pid_pelec && single_hit_flg cut
	
	cout << std::setprecision(2) << double(i) / nentries * 100. << "  % " << std::flush << "\r";
	
      } // end entry loop
    
    beta_calibWeighted_canv->cd();
    H_beta_calib_weighted->Draw();
    
    beta_calibUnweighted_canv->cd(); 
    H_beta_calib_unweighted->Draw(); 
    
    //Write Histograms to ROOT file                                                                                                        
    outROOT->Write();                                                                                                                       
    outROOT->Close();
    
    // de-allocate memory of  3d dynamic array
    for(int i = 0; i < nentries; i++){
      
      for(int j = 0; j < PLANES; j++){ 
	
	delete[] mean_time_flg[i][j];                                                                                                 
	
      }

      delete[] mean_time_flg[i];
    }
    
    delete[] mean_time_flg;

    // Calculate the analysis rate    
    t = clock() - t;  
    printf ("The Analysis Took %.1f seconds (%.1f min.) \n", ((float) t) / CLOCKS_PER_SEC, ((float) t) / CLOCKS_PER_SEC/60.);  
   

}
