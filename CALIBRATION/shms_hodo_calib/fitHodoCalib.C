/*SHMS Hodo Calibration script to determine scin. prop velocity, cable time offsets per paddle, 
  and time offsets between paddles in different planes
  Author: Carlos Yero
  Dated: June 5, 2018
*/

#include <TMatrixD.h>
#include <TVectorD.h>
#include <TDecompSVD.h>
#include <TSystem.h>
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

void fitHodoCalib(TString filename,Int_t runNUM,Bool_t cosmic_flag=kFALSE)
{

  gStyle->SetOptFit();  
  gROOT->SetBatch(kTRUE);    //do not display plots



  Int_t evtNUM = 30000; // evtNUm is max number in the fit array
  TFile *data_file = new TFile(filename, "READ"); 
  TTree *T = (TTree*)data_file->Get("T");

  //Create output root file where histograms will be stored
  TFile *outROOT = new TFile(Form("HodoCalibPlots_%d.root", runNUM), "recreate");
  
  /******Define Fixed Quantities********/
  static const Int_t PLANES = 4;
  static const Int_t SIDES = 2;
  TString spec = "P";
  TString det = "hod";
  string pl_names[4] = {"1x", "1y", "2x", "2y"};
  string side_names[2] = {"GoodPos", "GoodNeg"};
  string nsign[2] = {"+", "-"};
  Int_t maxPMT[4] = {13, 13, 14, 21};
  Int_t refPad[4] = {0, 13, 26, 40};              //use as reference when counting bars up to 61 
  
  
  Double_t phodo_velArr[PLANES][21] = {0.0};      //store phodo velocity parameters (1/slope of the line fit)
  Double_t phodo_cableArr[PLANES][21] = {0.0};    //store phodo cableLength differences (y-int of line fit)
  Double_t phodo_sigArr[PLANES][21] = {0.0};       //store phodo sigma parameters
  Double_t vp = 30.0;                             //speed of light [cm/ns]

  if (cosmic_flag) {
    vp= -vp;
    cout << " Spedd of light set to negative number for cosmics" << endl;
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
  TString npdc_ntrack = "P.dc.ntrack";
  TString nhod_nhits = "nhits";
  TString nbeta = "P.hod.betanotrack";

  Double_t etrknrm_low_cut = 0.7;
  Double_t npngcer_npeSum_low_cut = 0.7;
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
      cout << "Plane: " << ipl << endl;
      for(int ibar = 0; ibar < maxPMT[ipl]; ibar++)
	{
	  z[ipl][ibar] = 0.0;
	  z[ipl][ibar] = (z0[ipl] + (ibar%2)*dz[ipl])*1.0;
	}

    }
                                                                                    

  /*******Define Canvas and Histograms*******/

  //----Canvas----
  TCanvas *TWAvg_canv[PLANES];
  TCanvas *TWAvg_canv_2D[PLANES];
  TCanvas *Diff_TWDistTrkPos_canv[PLANES];

  TCanvas *TWUnCorr_canv[PLANES][SIDES];
  TCanvas *TWCorr_canv[PLANES][SIDES];
  TCanvas *TWCorr_v_TrkPos_canv[PLANES];
  TCanvas *TWDiff_v_TrkPos_canv[PLANES];

  //----Histograms----
  TH1F *h1Hist_TWAvg[PLANES][21];                    // (TWCorr_Pos + TWCorr_Neg) / 2       <------- 
  TH1F *h1Hist_TWAvg_CUT[PLANES][21];                                                    //<------
  TH1F *h1Hist_TWDiffTrkPos[PLANES][21];             //1D hist of TW Corr. Dist - Track Position (width should be Hodo resolution)    <------

  TH2F *h2Hist_TW_UnCorr[PLANES][SIDES][21];         //Time-Walk Uncorrected vs. ADC Pulse Amp Hist
  TH2F *h2Hist_TW_Corr[PLANES][SIDES][21];           //Time-Walk Corrected vs. ADC Pulse Amp Hist
  TH2F *h2Hist_TW_Corr_v_TrkPos[PLANES][21];         //Time-Walk Corr Time Diff vs. Track Position @ Hod. Paddle      <-------
  TH2F *h2Hist_TWDiff_v_TrkPos[PLANES][21];          //(Time-Walk Corr Dist. Diff) vs. TrackPos)                      <-------
  TH2F *h2Hist_TWAvg_v_TrkPos[PLANES][21];           // (TWCorr_Pos + TWCorr_Neg) / 2  vs TrackPos      <------- 

  /*******Define Fit Functions and Related Variables*******/

  Double_t Mean;                                      //variable to get Mean to make a 3sig cut
  Double_t StdDev;                                    //variable to get satndar deviation to make a 3sig cut
  Double_t nSig;                                         //multiple of Sigma used for sigmaCut
  
  //Gaussian Fit for TWAvg
  TF1 *gausFit[PLANES][21];

  //1d Fit Function for fitting TW_Corr vs. TrkPos
  TF1 *fit1x = new TF1("fit1x", "[0]*x + [1]", -40., 40.);
  TF1 *fit1y = new TF1("fit1y", "[0]*x + [1]", -40., 40.);                                                                                                     
  TF1 *fit2x = new TF1("fit2x", "[0]*x + [1]", -20., 40.);                                                                                 
  TF1 *fit2y = new TF1("fit2y", "[0]*x + [1]", -20., 40.);                                                                             

  //Set Param Values/Names
  fit1x->SetParameter(0, 1.), fit1x->SetParName(0, "slope");
  fit1x->SetParameter(1, 1.), fit1x->SetParName(1, "y-int");                                                                                                                                 
  fit1y->SetParameter(0, 1.), fit1y->SetParName(0, "slope");  
  fit1y->SetParameter(1, 1.), fit1y->SetParName(1, "y-int");                                                                                                                               
  fit2x->SetParameter(0, 1.), fit2x->SetParName(0, "slope");                                                                                            
  fit2x->SetParameter(1, 1.), fit2x->SetParName(1, "y-int");  
  fit2y->SetParameter(0, 1.), fit2y->SetParName(0, "slope");           
  fit2y->SetParameter(1, 1.), fit2y->SetParName(1, "y-int"); 
  
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
	      h2Hist_TW_UnCorr[npl][side][ipmt] = new TH2F(Form("TW_UnCorr PMT %s%d%s", pl_names[npl].c_str(), ipmt+1, nsign[side].c_str()), Form("PMT %s%d%s: UnCorr. (TDC - ADC) Pulse Time vs. ADC Pulse Amplitude ",pl_names[npl].c_str(), ipmt+1, nsign[side].c_str()), 600, 0, 420, 120, -60, 60);   
	      h2Hist_TW_Corr[npl][side][ipmt] = new TH2F(Form("TW_Corr PMT %s%d%s", pl_names[npl].c_str(), ipmt+1, nsign[side].c_str()) , Form("PMT %s%d%s: Corr. (TDC - ADC) Pulse Time vs. ADC Pulse Amplitude ", pl_names[npl].c_str(), ipmt+1, nsign[side].c_str()), 600, 0, 420, 120, -60, 60);   
	      
	      h2Hist_TW_UnCorr[npl][side][ipmt]->GetYaxis()->SetTitle("Time Walk UnCorr.(TDC - ADC) Pulse Time (ns)");
	      h2Hist_TW_UnCorr[npl][side][ipmt]->GetXaxis()->SetTitle("ADC Pulse Amplitude (mV)");
	      h2Hist_TW_UnCorr[npl][side][ipmt]->GetXaxis()->CenterTitle();
	      
	      h2Hist_TW_Corr[npl][side][ipmt]->GetYaxis()->SetTitle("Time Walk Corr.(TDC - ADC) Pulse Time (ns)");;
	      h2Hist_TW_Corr[npl][side][ipmt]->GetXaxis()->SetTitle("ADC Pulse Amplitude (mV)");
	      h2Hist_TW_Corr[npl][side][ipmt]->GetXaxis()->CenterTitle();
	      
	       if (side==0) //require ONLY one side, since a time diff between two pmts at each end is taken
		{

		  h1Hist_TWAvg[npl][ipmt] = new TH1F(Form("Avg. Time: Paddle %s%d", pl_names[npl].c_str(), ipmt+1), Form("Paddle %s%d: Time-Walk Corrected Average Time", pl_names[npl].c_str(), ipmt+1), 100, 0, 100);
		  
		  h1Hist_TWAvg_CUT[npl][ipmt] = new TH1F(Form("Avg. Time CUT: Paddle %s%d", pl_names[npl].c_str(), ipmt+1), Form("Paddle %s%d: Time-Walk Corrected Average (CUT)",pl_names[npl].c_str(), ipmt+1), 100, 0, 100);
	      
		  h2Hist_TWDiff_v_TrkPos[npl][ipmt] = new TH2F(Form("DistDiff: Paddle %s%d", pl_names[npl].c_str(), ipmt+1), Form("Paddle %s%d: Time-Walk Corr. Hit Dist vs. Hod Track Position", pl_names[npl].c_str(), ipmt+1), 160, -80, 80, 200, -120, 80);
		  h2Hist_TW_Corr_v_TrkPos[npl][ipmt] = new TH2F(Form("TimeDiff: Paddle %s%d", pl_names[npl].c_str(), ipmt+1), Form("Paddle %s%d: Time-Walk Corr. TimeDiff. vs. Hod Track Position", pl_names[npl].c_str(), ipmt+1), 160, -60, 60, 200, -15, 15);
		  
		  h1Hist_TWDiffTrkPos[npl][ipmt] = new TH1F(Form("DistDiff - Track: Paddle %s%d", pl_names[npl].c_str(), ipmt+1), Form("Paddle %s%d: Time-Walk Corr. Hit Dist. - Hod Track Position",pl_names[npl].c_str(), ipmt+1), 200, -120, 80);

		  h2Hist_TWAvg_v_TrkPos[npl][ipmt] = new TH2F(Form("TimeAvg_v_Trk: Paddle %s%d", pl_names[npl].c_str(), ipmt+1), Form("Paddle %s%d: Time-Walk Corr. TimeAvg. vs. Hod Track Position", pl_names[npl].c_str(), ipmt+1), 160, -40, 40, 120, 0, 100);
  
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
	       base = spec + "." + det + "." + pl_names[npl];
	       
	       nTdcTimeUnCorr = base + "." + side_names[side] + "TdcTimeUnCorr";
	       nTdcTimeTWCorr = base + "." + side_names[side] + "TdcTimeWalkCorr";
	       nAdcPulseTime = base + "." + side_names[side] + "AdcPulseTime";
	       nAdcPulseAmp = base + "." + side_names[side] + "AdcPulseAmp";
	       nDiffTWCorr = base + "." + "DiffDisTrackCorr";
	       nTrackXPos = base + "." + "TrackXPos";
	       nTrackYPos = base + "." + "TrackYPos";
	       nhod_nhits = base + "." + "nhits";
	       
	       //------Set Branch Address-------
	       T->SetBranchAddress(nTdcTimeUnCorr,  &TdcTimeUnCorr[npl][side]);
	       T->SetBranchAddress(nTdcTimeTWCorr,  &TdcTimeTWCorr[npl][side]);
	       T->SetBranchAddress(nAdcPulseTime,  &AdcPulseTime[npl][side]);
	       T->SetBranchAddress(nAdcPulseAmp,  &AdcPulseAmp[npl][side]);
	       T->SetBranchAddress(nDiffTWCorr,  &DiffDistTWCorr[npl]);
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
  Long64_t evtCnt = 0;

  Long64_t nentries = T->GetEntries();
  
 //Loop over all entries
  for(Long64_t i=0; i<nentries; i++)
    {
      T->GetEntry(i);  

      pcal = pcal_etrkNorm>etrknrm_low_cut;
      pngcer = pngcer_npeSum>npngcer_npeSum_low_cut;
      pdctrk = pdc_ntrack>0.0;
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
		  
		  if(TdcTimeTWCorr[npl][0][ipmt] < 100. && TdcTimeTWCorr[npl][1][ipmt] < 100. && pcal_etrkNorm>0.7)
		    {
		      //Fill Average TW Corr TDC Time
		      h1Hist_TWAvg[npl][ipmt]->Fill((TdcTimeTWCorr[npl][0][ipmt] + TdcTimeTWCorr[npl][1][ipmt])/2.);
		      
		    } //end time cut
		  
		} //end pmt loop
	      
	    }// end plane loop
	  
	} //END PID ELECTRON CUT

      cout << std::setprecision(2) << double(i) / nentries * 100. << "  % " << std::flush << "\r";

    } //end loop over entries
      
      //Set cut on Sigma,
      nSig = 1;    


  //************************************//
  //    SECOND PASS OF EVENT LOOP       //
  //************************************//
  cout << "Initializing 2nd Pass of Event Loop: " << endl;

  //Loop over all entries
  for(Long64_t i=0; i<nentries; i++)
    {
      
      T->GetEntry(i);  
      
      pcal = pcal_etrkNorm>etrknrm_low_cut;
      pngcer = pngcer_npeSum>npngcer_npeSum_low_cut;
      pdctrk = pdc_ntrack>0.0;
      betaCut = beta>betanotrack_low_cut&& beta<betanotrack_hi_cut;
      pid_pelec = pcal&&pngcer&&pdctrk;
      if (cosmic_flag)  pid_pelec = betaCut&&pdctrk; 


      //-----APPLY PID CUT TO SELECT CLEAN ELECTRONS-----

      if(pid_pelec)
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
		  if(TdcTimeTWCorr[npl][0][ipmt] < 100. && TdcTimeTWCorr[npl][1][ipmt] < 100.)
		    {
		      
		      if (side==0)  //require only one side, as a time diff between the two ends of a paddle is take
			{
			  
			  
			  if (npl==0 || npl==2)
			    {
			      h2Hist_TWAvg_v_TrkPos[npl][ipmt]->Fill(TrackYPos[npl], 0.5*(TdcTimeTWCorr[npl][1][ipmt] + TdcTimeTWCorr[npl][0][ipmt])); 
			    }
			  
			  else if (npl==1 || npl==3)
			    {
			      h2Hist_TWAvg_v_TrkPos[npl][ipmt]->Fill(TrackXPos[npl], 0.5*(TdcTimeTWCorr[npl][1][ipmt] + TdcTimeTWCorr[npl][0][ipmt])); 
			      
			    }
			  if ( (((TdcTimeTWCorr[npl][0][ipmt] + TdcTimeTWCorr[npl][1][ipmt])/2.) > (Mean-nSig*StdDev)) &&  (((TdcTimeTWCorr[npl][0][ipmt] + TdcTimeTWCorr[npl][1][ipmt])/2.) < (Mean+nSig*StdDev)))
			    {
			      
			      if (npl==0 || npl==2)
				{
				  
				  h2Hist_TW_Corr_v_TrkPos[npl][ipmt]->Fill(TrackYPos[npl],  0.5*(TdcTimeTWCorr[npl][1][ipmt] - TdcTimeTWCorr[npl][0][ipmt]));			      	
				  h2Hist_TWDiff_v_TrkPos[npl][ipmt]->Fill(TrackYPos[npl],  DiffDistTWCorr[npl][ipmt]-TrackYPos[npl]);
				  h1Hist_TWDiffTrkPos[npl][ipmt]->Fill(DiffDistTWCorr[npl][ipmt] - TrackYPos[npl]);                           
				  phodo_sigArr[npl][ipmt] = h1Hist_TWDiffTrkPos[npl][ipmt]->GetStdDev();  //Get Paddle resolution stdDev to be used in sig. parameters
				}
			      else if (npl==1 || npl==3)
				{
				  
				  h2Hist_TW_Corr_v_TrkPos[npl][ipmt]->Fill(TrackXPos[npl],  0.5*(TdcTimeTWCorr[npl][1][ipmt] - TdcTimeTWCorr[npl][0][ipmt]));
				  h2Hist_TWDiff_v_TrkPos[npl][ipmt]->Fill(TrackXPos[npl],  DiffDistTWCorr[npl][ipmt]-TrackXPos[npl]);
				  h1Hist_TWDiffTrkPos[npl][ipmt]->Fill(DiffDistTWCorr[npl][ipmt] - TrackXPos[npl]);                    
				  phodo_sigArr[npl][ipmt] = h1Hist_TWDiffTrkPos[npl][ipmt]->GetStdDev();  //Get Paddle resolution stdDev to be used in sig. parameters

				  
				}
			      
			      h1Hist_TWAvg_CUT[npl][ipmt]->Fill((TdcTimeTWCorr[npl][0][ipmt] + TdcTimeTWCorr[npl][1][ipmt])/2.);
			      
			    } //end 3SIGMA CUT of TW Corr Time
			  
			}//end require ONLY single side
		      
		    } //end time cuts
		  
		}//end pmt loop
	      
	    } //end side loop
	  
	} //end plane loop
      
	} //END PID ELECTRON
          
      cout << std::setprecision(2) << double(i) / nentries * 100. << "  % " << std::flush << "\r";

    } //end entry loop

  cout << endl;

  cout << " Starting fitting slope and intercept of histograms" << endl;

  /***********DRAW HISTOGRAMS TO CANVAS***************/
 for (Int_t npl = 0; npl < PLANES; npl++ )
    {      
    
      //Create Canvas to store TW-Corr Time/Dist vs. trk position
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
	  TWUnCorr_canv[npl][side] = new TCanvas(Form("TWUnCorrCanv%d%d", npl, side), Form("plane %s_%s", pl_names[npl].c_str(), side_names[side].c_str()),  1000, 700);
	  TWCorr_canv[npl][side] = new TCanvas(Form("TWCorrCanv%d%d", npl, side), Form("plane %s_%s", pl_names[npl].c_str(), side_names[side].c_str()),  1000, 700);

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
		      fit_status = h2Hist_TW_Corr_v_TrkPos[npl][ipmt]->Fit("fit1x", "QR");  		    }
		  else if (npl==1)
		    {   
		      fit_status =  h2Hist_TW_Corr_v_TrkPos[npl][ipmt]->Fit("fit1y", "QR");     
		    } 
		  else if (npl==2)
		    {                                                                                               
		      fit_status = h2Hist_TW_Corr_v_TrkPos[npl][ipmt]->Fit("fit2x", "QR");                         
		    }
		  else if (npl==3)
		    {                                                                                            
                      fit_status = h2Hist_TW_Corr_v_TrkPos[npl][ipmt]->Fit("fit2y", "QR");     
		    } 
		  		  
		  TWDiff_v_TrkPos_canv[npl]->cd(ipmt+1);
		  h2Hist_TWDiff_v_TrkPos[npl][ipmt]->Draw("colz");
		  
		  TWAvg_canv_2D[npl]->cd(ipmt+1);
		  h2Hist_TWAvg_v_TrkPos[npl][ipmt]->Draw("colz");
		  
		  //Draw 1D TWAvg Histos
		  TWAvg_canv[npl]->cd(ipmt+1);
		  h1Hist_TWAvg[npl][ipmt]->SetLineColor(kBlack);
		  h1Hist_TWAvg_CUT[npl][ipmt]->SetLineColor(kRed);
		  h1Hist_TWAvg[npl][ipmt]->Draw();
		  h1Hist_TWAvg_CUT[npl][ipmt]->Draw("same");
		  
		  Diff_TWDistTrkPos_canv[npl]->cd(ipmt+1);
		  h1Hist_TWDiffTrkPos[npl][ipmt]->Draw();
				  	  
		  if (npl==0) {
		    phodo_velArr[0][ipmt] = 1./(fit1x->GetParameter(0));
		    if(fit_status==-1 || phodo_velArr[0][ipmt]==1 ) {
                           phodo_velArr[0][ipmt] = 15.0;
                           phodo_cableArr[0][ipmt] = 0.0;
                           phodo_sigArr[0][ipmt] = 1.0;
			   cout << " Could not fit plane = " << npl << " paddle = " << ipmt +1 << endl;
                    } else {
		           phodo_cableArr[0][ipmt] = fit1x->GetParameter(1);
		           phodo_sigArr[0][ipmt] = phodo_sigArr[0][ipmt] / (2.*phodo_velArr[0][ipmt]);
		    }
		  }
		  
		  else if (npl==1) { 
		    phodo_velArr[1][ipmt] = 1./(fit1y->GetParameter(0)); 
		    if(fit_status==-1|| phodo_velArr[1][ipmt]==1) {
                           phodo_velArr[1][ipmt] = 15.0;
                           phodo_cableArr[1][ipmt] = 0.0;
                           phodo_sigArr[1][ipmt] = 1.0;
			   cout << " Could not fit plane = " << npl << " paddle = " << ipmt +1 << endl;
                    } else {
		           phodo_cableArr[1][ipmt] = fit1y->GetParameter(1);
		           phodo_sigArr[1][ipmt] = phodo_sigArr[1][ipmt] / (2.*phodo_velArr[1][ipmt]);
		    }
		  }
		  else if (npl==2) { 
		    phodo_velArr[2][ipmt] = 1./(fit2x->GetParameter(0)); 
		    if(fit_status==-1 || phodo_velArr[2][ipmt]==1) {
                          phodo_velArr[2][ipmt] = 15.0;
                           phodo_cableArr[2][ipmt] = 0.0;
                           phodo_sigArr[2][ipmt] = 1.0;
			   cout << " Could not fit plane = " << npl << " paddle = " << ipmt +1 << endl;
                    } else {
		    phodo_cableArr[2][ipmt] = fit2x->GetParameter(1);
		    phodo_sigArr[2][ipmt] = phodo_sigArr[2][ipmt] / (2.*phodo_velArr[2][ipmt]);
		    }
		  }
		  else if (npl==3) { 
		    phodo_velArr[3][ipmt] = 1./(fit2y->GetParameter(0));
		    if(fit_status==-1 || phodo_velArr[3][ipmt]==1) {
                           phodo_velArr[3][ipmt] = 15.0;
                           phodo_cableArr[3][ipmt] = 0.0;
                           phodo_sigArr[3][ipmt] = 1.0;
			   cout << " Could not fit plane = " << npl << " paddle = " << ipmt +1 << endl;
                    } else {
		         phodo_cableArr[3][ipmt] = fit2y->GetParameter(1);
		         phodo_sigArr[3][ipmt] = phodo_sigArr[3][ipmt] / (2.*phodo_velArr[3][ipmt]);
		    }
		  }
	
		} //end single SIDE requirement
	      
	    } //end pmt loop

	} //end side loop
  
    } //end plane loop


 /************WRITE FIT RESULTS TO PARAMETER FILE***************/
 
 ofstream outPARAM;
 outPARAM.open(Form("../../PARAM/SHMS/HODO/phodo_Vpcalib_%d.param", runNUM));
 
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
 //Write Histograms to ROOT file
 outROOT->Write();
 outROOT->Close();
   
 cout << "FINISHED Getting Vp and Cable Fits . . . " << endl;
 cout << "Starting the code to fit Hodo Matrix . . . " << endl;                                                                                                                                                                                                                                                                                                                                
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                         
 /**********BEGIN CODE TO FIT HODO MATRIX**************/
 
  for(Long64_t i=0; i<nentries; i++)
    {
      T->GetEntry(i);  
      
      //Define some minimum requirements
      Bool_t x1_hit = hod_nhits[0] == 1;
      Bool_t y1_hit = hod_nhits[1] == 1;
      Bool_t x2_hit = hod_nhits[2] == 1;
      Bool_t y2_hit = hod_nhits[3] == 1;
    

      Bool_t hodTrk = TrackXPos[0]<200&&TrackYPos[0]<200&&
		      TrackXPos[1]<200&&TrackYPos[1]<200&&
		      TrackXPos[2]<200&&TrackYPos[2]<200&&
		      TrackXPos[3]<200&&TrackYPos[3]<200;
      betaCut=kTRUE;
      if (cosmic_flag)  betaCut = beta>betanotrack_low_cut&& beta<betanotrack_hi_cut;
	//require each plane to have ONLY a SINGLE HIT, and hod track coord. to be reasonable (NOT kBig)
	if (x1_hit&&y1_hit&&x2_hit&&y2_hit&&hodTrk&&betaCut)
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
		    //require good tdc hit on both ends
		    goodhit[npl] =  TdcTimeTWCorr[npl][0][bar-1]<100.&&TdcTimeTWCorr[npl][1][bar-1]<100.;
		    
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
 
    
}
