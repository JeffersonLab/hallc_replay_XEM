#define calibration_cxx
// Vijay Kumar, Univerity of Regina - 24/07/20
// vijay36361@gmail.com
// For more info on processing with TProof, see https://root.cern.ch/doc/v608/classTProof.html

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
{ printf("\n\n");
  TString option = GetOption();
  Info("Begin", "Starting calibration process");
  printf("\n\n");
}

void calibration::SlaveBegin(TTree * /*tree*/)
{
  printf("\n\n");
  TString option = GetOption();
  // Initialize the histograms. Note they are binned per ADC channel which will be changed in the calibration analysis.
  Int_t ADC_min;
  Int_t ADC_max;
  Int_t bins;
  ADC_max = 35;
  // Initialize the histograms, they used in the quality control work. Note they are binned per ADC channel which will be changed in the calibration analysis
  Int_t ADC_poiss_min;
  Int_t ADC_poiss_max;
  Int_t bins_poiss;
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
    {  // Separate ADC channel for each quadrant histogram
      if(ipmt == 0)
	{
	  ADC_min = 4;
	  bins = 2*(abs(ADC_min) + abs(ADC_max));
	  fPulseInt[ipmt] = new TH1F(Form("PulseInt_PMT%d",ipmt+1),Form("Pulse Integral PMT%d; ADC Channel (pC); Counts",ipmt+1), bins, ADC_min, ADC_max);
	  GetOutputList()->Add(fPulseInt[ipmt]); 
	  for (Int_t iquad = 0; iquad < 4; iquad++)
	    {
	      fPulseInt_quad[iquad][ipmt] = new TH1F(Form("PulseInt_quad%d_PMT%d",iquad+1,ipmt+1),Form("Pulse Integral PMT%d quad%d; ADC Channel (pC); Counts",ipmt+1,iquad+1),bins, ADC_min,ADC_max);
	      GetOutputList()->Add(fPulseInt_quad[iquad][ipmt]);
	    }      
	  fPulseInt_poiss[ipmt] = new TH1F(Form("PulseInt_poiss_PMT%d",ipmt+1),Form("Pulse Integral PMT%d; ADC Channel (pC); Counts",ipmt+1), bins_poiss, ADC_poiss_min, ADC_poiss_max);
	  GetOutputList()->Add(fPulseInt_poiss[ipmt]);
	}
      if(ipmt == 1)
	{
	  ADC_min = 4;
	  bins = 2*(abs(ADC_min) + abs(ADC_max));	  
	  fPulseInt[ipmt] = new TH1F(Form("PulseInt_PMT%d",ipmt+1),Form("Pulse Integral PMT%d; ADC Channel (pC); Counts",ipmt+1), bins, ADC_min, ADC_max);
	  GetOutputList()->Add(fPulseInt[ipmt]);
	  for (Int_t iquad = 0; iquad < 4; iquad++)
	    {
	      fPulseInt_quad[iquad][ipmt] = new TH1F(Form("PulseInt_quad%d_PMT%d",iquad+1,ipmt+1),Form("Pulse Integral PMT%d quad%d; ADC Channel (pC); Counts",ipmt+1,iquad+1),bins, ADC_min,ADC_max);
	      GetOutputList()->Add(fPulseInt_quad[iquad][ipmt]);
	    }      
	  fPulseInt_poiss[ipmt] = new TH1F(Form("PulseInt_poiss_PMT%d",ipmt+1),Form("Pulse Integral PMT%d; ADC Channel (pC); Counts",ipmt+1), bins_poiss, ADC_poiss_min, ADC_poiss_max);
	  GetOutputList()->Add(fPulseInt_poiss[ipmt]);
	} 
      if(ipmt == 2)
	{
	  ADC_min = 4;
	  bins = 2*(abs(ADC_min) + abs(ADC_max));
	  fPulseInt[ipmt] = new TH1F(Form("PulseInt_PMT%d",ipmt+1),Form("Pulse Integral PMT%d; ADC Channel (pC); Counts",ipmt+1), bins, ADC_min, ADC_max);
	  GetOutputList()->Add(fPulseInt[ipmt]);
	  for (Int_t iquad = 0; iquad < 4; iquad++)
	    {
	      fPulseInt_quad[iquad][ipmt] = new TH1F(Form("PulseInt_quad%d_PMT%d",iquad+1,ipmt+1),Form("Pulse Integral PMT%d quad%d; ADC Channel (pC); Counts",ipmt+1,iquad+1), bins, ADC_min, ADC_max);
	      GetOutputList()->Add(fPulseInt_quad[iquad][ipmt]);
	    }      
	  fPulseInt_poiss[ipmt] = new TH1F(Form("PulseInt_poiss_PMT%d",ipmt+1),Form("Pulse Integral PMT%d; ADC Channel (pC); Counts",ipmt+1), bins_poiss, ADC_poiss_min, ADC_poiss_max);
	  GetOutputList()->Add(fPulseInt_poiss[ipmt]);
	} 
      if(ipmt == 3)
	{
	  ADC_min = 4;
	  bins = 2*(abs(ADC_min) + abs(ADC_max));
	  fPulseInt[ipmt] = new TH1F(Form("PulseInt_PMT%d",ipmt+1),Form("Pulse Integral PMT%d; ADC Channel (pC); Counts",ipmt+1), bins, ADC_min, ADC_max);
	  GetOutputList()->Add(fPulseInt[ipmt]);
	  for (Int_t iquad = 0; iquad < 4; iquad++)
	    {
	      fPulseInt_quad[iquad][ipmt] = new TH1F(Form("PulseInt_quad%d_PMT%d",iquad+1,ipmt+1),Form("Pulse Integral PMT%d quad%d; ADC Channel (pC); Counts",ipmt+1,iquad+1),bins, ADC_min,ADC_max);
	      GetOutputList()->Add(fPulseInt_quad[iquad][ipmt]);
	    }
	  fPulseInt_poiss[ipmt] = new TH1F(Form("PulseInt_poiss_PMT%d",ipmt+1),Form("Pulse Integral PMT%d; ADC Channel (pC); Counts",ipmt+1), bins_poiss, ADC_poiss_min, ADC_poiss_max);
	  GetOutputList()->Add(fPulseInt_poiss[ipmt]);
	}
    }
  
  // Histograms for Timing info visualizations 
  fTiming_Full = new TH1F("fTiming_full", "Full timing information for events;Time (ns);Counts", 200, -40 , 50);
  GetOutputList()->Add(fTiming_Full);
  //Individual Histogram for timing info for each PTM
  fTim1 = new TH1F("Timing_PMT1", "ADC TDC Diff PMT1 ; Time (ns) ;Counts", 200, -40.0, 50.0);
  GetOutputList()->Add(fTim1);
  fTim1_full = new TH1F("Timing_full_PMT1", "ADC TDC Diff PMT1 ; Time (ns) ;Counts", 200, -40.0, 50.0);
  GetOutputList()->Add(fTim1_full);
  fTim2 = new TH1F("Timing_PMT2", "ADC TDC Diff PMT2 ; Time (ns) ;Counts", 200, -40.0, 50.0);
  GetOutputList()->Add(fTim2);
  fTim2_full = new TH1F("Timing_full_PMT2", "ADC TDC Diff PMT1 ; Time (ns) ;Counts", 200, -40.0, 50.0);
  GetOutputList()->Add(fTim2_full);
  fTim3 = new TH1F("Timing_PMT3", "ADC TDC Diff PMT3 ; Time (ns) ;Counts", 200, -40.0, 50.0);
  GetOutputList()->Add(fTim3);
  fTim3_full = new TH1F("Timing_full_PMT3", "ADC TDC Diff PMT1 ; Time (ns) ;Counts", 200, -40.0, 50.0);
  GetOutputList()->Add(fTim3_full);
  fTim4 = new TH1F("Timing_PMT4", "ADC TDC Diff PMT4 ; Time (ns) ;Counts", 200, -40.0, 50.0);
  GetOutputList()->Add(fTim4);
  fTim4_full = new TH1F("Timing_full_PMT4", "ADC TDC Diff PMT1 ; Time (ns) ;Counts", 200, -40.0, 50.0);
  GetOutputList()->Add(fTim4_full);
  //Histograms for Beta visualization
  fBeta_Cut = new TH1F("Beta_Cut", "Beta cut used for 'good' hits;Beta;Counts", 100, -0.1, 1.5);
  GetOutputList()->Add(fBeta_Cut);  
  fBeta_Full = new TH1F("Beta_Full", "Full beta for events;Beta;Counts", 100, -0.1, 1.5);
  GetOutputList()->Add(fBeta_Full);
  printf("\n\n");
}

Bool_t calibration::Process(Long64_t entry) 
{
  fReader.SetEntry(entry);
  //Output to verify script is working, and store the total number of events
  if (entry % 100000 == 0) printf("Processing Entry number %lld\n",entry);                          
  //Define quantities to loop over
  Int_t fpmts;
  fpmts = fhgc_pmts;   
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
	  //Filling timing info before cut
	  fTiming_Full->Fill(P_hgcer_goodAdcTdcDiffTime[ipmt]);
	  //Perform a loose timing cut on each PMT
	  if(ipmt ==0)
	    {
	      fTim1_full->Fill(P_hgcer_goodAdcTdcDiffTime[ipmt]);
	      if(P_hgcer_goodAdcTdcDiffTime[ipmt] >40 || P_hgcer_goodAdcTdcDiffTime[ipmt] < 30) continue;                      
	      fTim1->Fill(P_hgcer_goodAdcTdcDiffTime[ipmt]);
	    }
	  if(ipmt ==1)
	    {
	      fTim2_full->Fill(P_hgcer_goodAdcTdcDiffTime[ipmt]);
	      if(P_hgcer_goodAdcTdcDiffTime[ipmt] >40 || P_hgcer_goodAdcTdcDiffTime[ipmt] < 30) continue;                          
	      fTim2->Fill(P_hgcer_goodAdcTdcDiffTime[ipmt]);
	    }
	  if(ipmt ==2)
	    {
	      fTim3_full->Fill(P_hgcer_goodAdcTdcDiffTime[ipmt]);
	      if(P_hgcer_goodAdcTdcDiffTime[ipmt] >40 || P_hgcer_goodAdcTdcDiffTime[ipmt] < 30) continue;                           
	      fTim3->Fill(P_hgcer_goodAdcTdcDiffTime[ipmt]);
	    }
	  if(ipmt ==3)
	    {
	      fTim4_full->Fill(P_hgcer_goodAdcTdcDiffTime[ipmt]);
	      if(P_hgcer_goodAdcTdcDiffTime[ipmt] >40 || P_hgcer_goodAdcTdcDiffTime[ipmt] < 30) continue;                                  
	      fTim4->Fill(P_hgcer_goodAdcTdcDiffTime[ipmt]);
	    }
	  //Cuts to remove entries corresponding to a PMT not registering a hit    
	  if (P_hgcer_goodAdcPulseInt[ipmt] == 0.0) continue;
	  //For quadrant cut strategy with no particle ID cut
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
	}//Marks end of loop over PMTs
    }//Marks end of loop over tracks  
  return kTRUE;
}

void calibration::SlaveTerminate()
{
}

void calibration::Terminate()
{  
  TString option = GetOption();
  TString RunNumStartStr = option(0,4);
  TString RunNumEndStr = option(5,20);
  Int_t RunNumStart = (RunNumStartStr.Atoi());
  Int_t RunNumEnd = (RunNumEndStr.Atoi());
  printf("\n");
  Info("Terminate", "Histograms formed, now starting calibration.\n'Peak Buffer full' is a good warning!\n");
  printf("\n");
  // Default name of the output pdf
  TString foutname;
  if ( RunNumStart == RunNumEnd){
    foutname = Form("Calibration_plots/Calibration_plots_%i", RunNumStart);
  }
  else {
    foutname = Form("Calibration_plots/Calibration_plots_%i-%i", RunNumStart, RunNumEnd);
  }
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
  
  //Canvases to display cut information
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
  //Canvas to show timing cuts info for each PMT
  TCanvas *Timing1;
  Timing1 = new TCanvas("Timing1","time cuts for each pmts");      
  Timing1->Divide(2,2);
  Timing1->cd(1);
  fTim1->SetFillColor(kRed);
  fTim1_full->SetFillColor(kBlue);
  fTim1_full->Draw();
  fTim1->Draw("same");     
  Timing1->cd(2);
  fTim2->SetFillColor(kRed);
  fTim2_full->SetFillColor(kBlue);
  fTim2_full->Draw();
  fTim2->Draw("same");    
  Timing1->cd(3);
  fTim3->SetFillColor(kRed);
  fTim3_full->SetFillColor(kBlue);
  fTim3_full->Draw(); 
  fTim3->Draw("same");      
  Timing1->cd(4);
  fTim4->SetFillColor(kRed);
  fTim4_full->SetFillColor(kBlue);
  fTim4_full->Draw();
  fTim4->Draw("same");   
  Timing1->Print(outputpdf);  
  //Single Gaussian to find mean of SPE
  TF1 *Gauss1 = new TF1("Gauss1",gauss,100,3,3);
  Gauss1->SetParNames("Amplitude","Mean","Std. Dev.");
  //Sum of two Gaussians to determine SPE with minimal systematics
  TF1 *Gauss2 = new TF1("Gauss2",gauss,0, 20,6);                                               
  Gauss2->SetParNames("Amplitude 1","Mean 1","Std. Dev. 1","Amplitude 2","Mean 2","Std. Dev. 2");
  //Poisson distribution to remove high NPE background
  TF1 *Poisson = new TF1("Poisson",poisson,0.0,5.0,2.0);
  Poisson->SetParNames("Mean", "Amplitude");
  // The fit used in the quality control of the calibration. Sum of four gaussian and two poisson distribtions
  TF1 *Gauss4Poiss2 = new TF1("Gauss4Poiss2",fun_4gauss_2poisson, 0.0, 18.0 ,16.0);
  //Note about Poisson background, the mean varies between detectors/operating conditions so this quantity may require user input
  Double_t Poisson_mean;
  Poisson_mean = 5.5;  
  //Linear function used to determine goodness-of-fit for NPE spacing
  TF1 *Linear = new TF1("Linear",linear,0,5,2);
  Linear->SetParNames("Slope", "Intercept");     
  //An array is used to store the means for the SPE, and to determine NPE spacing
  Double_t mean[3];
  Double_t SD[3];
  Double_t mean_err[3];
  Double_t x_npe[3], y_npe[3], x_err[3], y_err[3];
  Double_t RChi2[3];
  Bool_t GoodFit[3];
  //Two more arrays are used to store the estimates for the calibration constants and another two to store goodness of calibration
  Double_t calibration_mk1[4], calibration_mk1Err[4], calibration_mk2[4], calibration_mk2Err[4], pmt_calib[4], pmt_calib_mk2[4], calibration_weighted_ave[4], calibration_weighted_ave_err[4] ;
  TPaveText *GoodFitText = new TPaveText (0.65, 0.15, 0.85, 0.2, "NDC");
  GoodFitText->SetTextColor(kGreen);
  GoodFitText->AddText("Good fit");
  //TString outputpdf = "PMT_Fits.pdf";                        //Name of the pdf file
  //Array to hold the Poisson character of the calibrations
  Double_t Pois_Chi[2];
  Pois_Chi[0] = 0.0, Pois_Chi[1] = 0.0;
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
      {
	//TSpectrum class is used to find the SPE peak using the search method
	TSpectrum *s = new TSpectrum(2);  
	//Create Canvas to see the search result for the SPE  
	quad_cuts[ipmt] = new TCanvas(Form("quad_cuts_%d",ipmt), Form("First Photoelectron peaks PMT%d",ipmt+1));
	quad_cuts[ipmt]->Divide(3,1);  	  
	Int_t ipad = 1; //Variable to draw over pads correctly      
	for (Int_t iquad=0; iquad<4; iquad++)
	  { 
	    if (iquad == ipmt) continue; //ignore a PMT looking at its own quadrant
	    quad_cuts[ipmt]->cd(ipad);	  
	    if (PulseInt_quad[iquad][ipmt]->GetEntries() > 0)
	      {		 
		//Perform search for the SPE and save the peak into the array xpeaks   //0.00	      
		s->Search(PulseInt_quad[iquad][ipmt], 2.0, "" ,0.05);	      
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
	      
		//If amplitude of second peak greater than first peak then switch the order around  
		if (xpeaks[1] < xpeaks[0])
		  {
		    k[0] =  xpeaks[0]; xpeaks[0] = xpeaks[1]; xpeaks[1] = k[0];
		  }			    	   
		//Use the peak to fit the SPE with a Gaussian to determine the mean	    
		if(ipmt == 0)
		  {  
		    PulseInt_quad[iquad][ipmt]->Draw("E");
		    Gauss2->SetRange(0,16);
		    Gauss2->SetParameter(0, 2000);
		    Gauss2->SetParameter(1, 6.0);
		    Gauss2->SetParameter(2, 2);
		    Gauss2->SetParameter(3, 1000);
		    Gauss2->SetParameter(4, 12);	
		    Gauss2->SetParameter(5, 3.0);
		    Gauss2->SetParLimits(0, 0.0,PulseInt_quad[iquad][ipmt]->GetBinContent(PulseInt_quad[iquad][ipmt]->GetXaxis()->FindBin(xpeaks[0])));
		    Gauss2->SetParLimits(1, 5.0, 8.0); 
		    Gauss2->SetParLimits(2, 0.5 , 4.0);
		    Gauss2->SetParLimits(3, 0.0, PulseInt_quad[iquad][ipmt]->GetBinContent(PulseInt_quad[iquad][ipmt]->GetXaxis()->FindBin(xpeaks[1])));
		    Gauss2->SetParLimits(4, 10, 17);   
		    Gauss2->SetParLimits(5, 2.0, 4.0);
		    PulseInt_quad[iquad][ipmt]->Fit("Gauss2","RQN");	           
		    double x1 = Gauss2->GetParameter(1);
		    double x2 = Gauss2->GetParameter(4);
		    xpeaks[0] = x1;
		    xpeaks[1] = x2;    
		  }	

		if(ipmt == 1)
		  {  
		    PulseInt_quad[iquad][ipmt]->Draw("E");
		    Gauss2->SetRange(0,16);
		    Gauss2->SetParameter(0, 2000);
		    Gauss2->SetParameter(1, 6.0);
		    Gauss2->SetParameter(2, 2);
		    Gauss2->SetParameter(3, 1000);
		    Gauss2->SetParameter(4, 12);	
		    Gauss2->SetParameter(5, 3.0);
		    Gauss2->SetParLimits(0, 0.0,PulseInt_quad[iquad][ipmt]->GetBinContent(PulseInt_quad[iquad][ipmt]->GetXaxis()->FindBin(xpeaks[0])));
		    Gauss2->SetParLimits(1, 5.0, 8.0);
		    Gauss2->SetParLimits(2, 0.5 , 4.0);
		    Gauss2->SetParLimits(3, 0.0, PulseInt_quad[iquad][ipmt]->GetBinContent(PulseInt_quad[iquad][ipmt]->GetXaxis()->FindBin(xpeaks[1])));
		    Gauss2->SetParLimits(4, 10, 17);   
		    Gauss2->SetParLimits(5, 2.0, 4.0);
		    PulseInt_quad[iquad][ipmt]->Fit("Gauss2","RQN");
		    double x1 = Gauss2->GetParameter(1);
		    double x2 = Gauss2->GetParameter(4);	
		    xpeaks[0] = x1;
		    xpeaks[1] = x2;
		  }	

		if(ipmt == 2)	      
		  { 
		    PulseInt_quad[iquad][ipmt]->Draw("E");
		    Gauss2->SetRange(0,16);
		    Gauss2->SetParameter(0, 2000);
		    Gauss2->SetParameter(1, 6.0);
		    Gauss2->SetParameter(2, 2);
		    Gauss2->SetParameter(3, 1000);
		    Gauss2->SetParameter(4, 12);	
		    Gauss2->SetParameter(5, 3.0);
		    Gauss2->SetParLimits(0, 0.0,PulseInt_quad[iquad][ipmt]->GetBinContent(PulseInt_quad[iquad][ipmt]->GetXaxis()->FindBin(xpeaks[0])));
		    Gauss2->SetParLimits(1, 5.0, 7.0); //2.0
		    Gauss2->SetParLimits(2, 0.5 , 4.0);
		    Gauss2->SetParLimits(3, 0.0, PulseInt_quad[iquad][ipmt]->GetBinContent(PulseInt_quad[iquad][ipmt]->GetXaxis()->FindBin(xpeaks[1])));
		    Gauss2->SetParLimits(4, 10, 17);   //2.0
		    Gauss2->SetParLimits(5, 2.0, 4.0);
		    PulseInt_quad[iquad][ipmt]->Fit("Gauss2","RQN");
	
		    double x1 = Gauss2->GetParameter(1);
		    double x2 = Gauss2->GetParameter(4);	
		    xpeaks[0] = x1;
		    xpeaks[1] = x2;
		  }	

		if(ipmt == 3)
		  { 
		    PulseInt_quad[iquad][ipmt]->Draw("E");
		    Gauss2->SetRange(0,16);
		    Gauss2->SetParameter(0, 2000);
		    Gauss2->SetParameter(1, 6.0);
		    Gauss2->SetParameter(2, 2);
		    Gauss2->SetParameter(3, 1000);
		    Gauss2->SetParameter(4, 12);	
		    Gauss2->SetParameter(5, 3.0);
		    Gauss2->SetParLimits(0, 0.0,PulseInt_quad[iquad][ipmt]->GetBinContent(PulseInt_quad[iquad][ipmt]->GetXaxis()->FindBin(xpeaks[0])));
		    Gauss2->SetParLimits(1, 5.0, 8.0); //2.0
		    Gauss2->SetParLimits(2, 0.5 , 4.0);
		    Gauss2->SetParLimits(3, 0.0, PulseInt_quad[iquad][ipmt]->GetBinContent(PulseInt_quad[iquad][ipmt]->GetXaxis()->FindBin(xpeaks[1])));
		    Gauss2->SetParLimits(4, 10, 17);   //2.0
		    Gauss2->SetParLimits(5, 2.0, 4.0);
		    PulseInt_quad[iquad][ipmt]->Fit("Gauss2","RQN");       
		    double x1 = Gauss2->GetParameter(1);
		    double x2 = Gauss2->GetParameter(4);	
		    xpeaks[0] = x1;
		    xpeaks[1] = x2;
		  }

		//Again Use the peak to fit the SPE with a Gaussian to determine the mean
		Gauss2->SetParameter(0,Gauss2->GetParameter(0));
		Gauss2->SetParameter(1,Gauss2->GetParameter(1));
		Gauss2->SetParameter(2,Gauss2->GetParameter(2));
		Gauss2->SetParameter(3,Gauss2->GetParameter(3));
		Gauss2->SetParameter(4,Gauss2->GetParameter(4));	
		Gauss2->SetParameter(5,Gauss2->GetParameter(5));
		PulseInt_quad[iquad][ipmt]->Fit("Gauss2","RQ");		 
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
	  
		// Get values ONLY if number of events in first peak is over 40 at its maximum. Values are all 0 if this is NOT true
		if (xpeaks[0] > 4.0 && PulseInt_quad[iquad][ipmt]->GetBinContent(PulseInt_quad[iquad][ipmt]->GetXaxis()->FindBin(xpeaks[0])) > 40) mean[ipad-1] = Gauss2->GetParameter(1); 
		if (xpeaks[0] > 4.0 && PulseInt_quad[iquad][ipmt]->GetBinContent(PulseInt_quad[iquad][ipmt]->GetXaxis()->FindBin(xpeaks[0])) > 40) SD[ipad-1] = Gauss2->GetParameter(2); 
		if (xpeaks[0] > 4.0 && PulseInt_quad[iquad][ipmt]->GetBinContent(PulseInt_quad[iquad][ipmt]->GetXaxis()->FindBin(xpeaks[0])) > 40) RChi2[ipad-1] = Gauss2->GetChisquare()/Gauss2->GetNDF(); 
		if (xpeaks[0] > 4.0 && PulseInt_quad[iquad][ipmt]->GetBinContent(PulseInt_quad[iquad][ipmt]->GetXaxis()->FindBin(xpeaks[0])) > 40) mean_err[ipad-1] = Gauss2->GetParError(1);
	  
		if (PulseInt_quad[iquad][ipmt]->GetBinContent(PulseInt_quad[iquad][ipmt]->GetXaxis()->FindBin(xpeaks[0])) > 2000){
		  // Set Boolean of whether fit is good or not here, based upon reduced Chi2 of the fit
		  TPaveText *BadFitText = new TPaveText (0.65, 0.15, 0.85, 0.2, "NDC");  
		  BadFitText->SetTextColor(kRed);
		  BadFitText->AddText("Bad fit");
		  if (RChi2[ipad-1] < 0.5 || RChi2[ipad-1] > 30)
		    {	
		      GoodFit[ipad-1] = kFALSE; 
		      BadFitText->Draw("same");
		    } 
		  else if  (RChi2[ipad-1] > 0.5 && RChi2[ipad-1] < 30)
		    {
		      GoodFit[ipad-1] = kTRUE;
		      GoodFitText->Draw("same");
		    } 
		}

		else
		  {
		    TPaveText *BadFitText = new TPaveText (0.65, 0.15, 0.85, 0.2, "NDC");  
		    BadFitText->SetTextColor(kRed);
		    BadFitText->AddText("Bad fit");  	  
		    if (RChi2[ipad-1] < 0.5 || RChi2[ipad-1] > 20)
		      {
			GoodFit[ipad-1] = kFALSE; 
			BadFitText->Draw("same");
		      } 
		    else if  (RChi2[ipad-1] > 0.5 && RChi2[ipad-1] < 20)
		      {
			GoodFit[ipad-1] = kTRUE;
			GoodFitText->Draw("same");
		      } 
		  }
		ipad++;
	      }
	  }
	  
	quad_cuts[ipmt]->Print(outputpdf);        
	//Obtain the conversion from ADC to NPE by taking the error weighted average of the SPE means
	Double_t xscale = 0.0;
	Double_t xscaleErr = 0.0;
	Double_t num_peaks = 0.0;
	Double_t WeightAvgSum1 = 0.0;
	Double_t WeightAvgSum2 = 0.0;
	// Take error weighted avg of calibration constants 26/8/19 SK
	// ONLY consider quadrants which returned acceptable fits in the weighted average
	for (Int_t i=0; i<3; i++)
	  { 
	    if (GoodFit[i] == kFALSE) continue;                     
	    if (mean_err[i] == 0) continue;                        
	    WeightAvgSum1 += mean[i]/(mean_err[i]*mean_err[i]);                
	    WeightAvgSum2 += 1/(mean_err[i]*mean_err[i]);
	  }
	// xscale -> First guess of the calibration constant
	xscale = WeightAvgSum1/WeightAvgSum2;
	xscaleErr = 1/TMath::Sqrt(WeightAvgSum2);    

	//Perform check if the statistics were too low to get a good estimate of the SPE mean	  
	//Scale full ADC spectra according to the mean of the SPE. This requires filling a new histogram with the same number of bins but scaled min/max
	Int_t nbins;
	nbins = (PulseInt_poiss[ipmt]->GetXaxis()->GetNbins());

	//With the scale of ADC to NPE create a histogram that has the conversion applied
	fscaled[ipmt] = new TH1F(Form("fscaled_PMT%d", ipmt+1), Form("Scaled ADC spectra for PMT%d; NPE; Normalized Counts",ipmt+1), nbins, 0.0 ,30.0);
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
	background_ipmt = new TCanvas(Form("backgrounf_pmt%d",ipmt), Form("Full NPE spectra for PMT%d ",ipmt+1));	  
	background_ipmt->cd();	    
   
	//Poisson distribution 
	Poisson->SetRange(7,35);
	Poisson->SetParameter(0,9.0);
	Poisson->SetParameter(1, 0.40);
	fscaled[ipmt]->Fit("Poisson","RQBN");

	// Gauss4Poiss2 included 4gauss + 2poisson for the quality control check 
	Gauss4Poiss2->SetRange(0.0, 30.0);
	Gauss4Poiss2->SetParameter(0, 0.6);  
	Gauss4Poiss2->SetParameter(1, 1.0);	
	Gauss4Poiss2->SetParameter(2, 0.5);  
	Gauss4Poiss2->SetParameter(3, 0.15);   
	Gauss4Poiss2->SetParameter(4, 2.0);
	Gauss4Poiss2->SetParameter(5, 0.6);  
	Gauss4Poiss2->SetParameter(6, 0.12);
	Gauss4Poiss2->SetParameter(7, 3.0);
	Gauss4Poiss2->SetParameter(8, 0.7);                   
	Gauss4Poiss2->SetParameter(9, 0.12);
	Gauss4Poiss2->SetParameter(10, 4.0);
	Gauss4Poiss2->SetParameter(11, 0.7);                 
	Gauss4Poiss2->SetParameter(12, 8.0);
	Gauss4Poiss2->SetParameter(13, 0.7);
	Gauss4Poiss2->SetParameter(14, Poisson->GetParameter(0));  
	Gauss4Poiss2 ->SetParameter(15, Poisson->GetParameter(1));
      
	// Constraints on mean 	
	Gauss4Poiss2->SetParLimits(1, 1 - 3*xscaleErr, 1 + 3*xscaleErr);	
	Gauss4Poiss2->SetParLimits(4, 2 - 3*xscaleErr, 2 + 3*xscaleErr);
	Gauss4Poiss2->SetParLimits(7, 3 - 3*xscaleErr, 3 + 3*xscaleErr);
	Gauss4Poiss2->SetParLimits(10, 4 - 3*xscaleErr, 4 + 3*xscaleErr);
	if(ipmt == 3) 
	  {
	    Gauss4Poiss2->SetParLimits(1, 1 - 2*xscaleErr, 1 + 2*xscaleErr);
	    Gauss4Poiss2->SetParLimits(4, 2 - 2*xscaleErr, 4 + 2*xscaleErr);
	  }
      
	//Constraints of sigma
	Gauss4Poiss2->SetParLimits(2, 0.02, 0.6);
	Gauss4Poiss2->SetParLimits(5, 0.04, 0.7);
	Gauss4Poiss2->SetParLimits(8, 0.05, 0.9);
	Gauss4Poiss2->SetParLimits(11, 0.07, 2.0 );
      
	if(ipmt == 2)
	  {  Gauss4Poiss2->SetParLimits(2, 0.02, 1.0);
	  }
	if(ipmt == 3)
	  {
	  
	    Gauss4Poiss2->SetParLimits(2, 0.02, 0.9);
	    Gauss4Poiss2->SetParLimits(5, 0.04, 1.0);
	    Gauss4Poiss2->SetParLimits(11, 0.04, 3.0);
	  }
	// Clone the histogram before fitting it	 
	scaled_clone = (TH1F*)fscaled[ipmt]->Clone("scaled_clone");	 
	fscaled[ipmt]->Fit("Gauss4Poiss2","RQN");
      
	// Again fit the normalised histogram taking parameters from the first fit   
	Gauss4Poiss2->SetParameter(0, Gauss4Poiss2->GetParameter(0));
	Gauss4Poiss2->SetParameter(1, Gauss4Poiss2->GetParameter(1));	
	Gauss4Poiss2->SetParameter(2, Gauss4Poiss2->GetParameter(2));
	Gauss4Poiss2->SetParameter(3, Gauss4Poiss2->GetParameter(3));
	Gauss4Poiss2->SetParameter(4, Gauss4Poiss2->GetParameter(4));
	Gauss4Poiss2->SetParameter(5, Gauss4Poiss2->GetParameter(5));
	Gauss4Poiss2->SetParameter(6, Gauss4Poiss2->GetParameter(6));
	Gauss4Poiss2->SetParameter(7, Gauss4Poiss2->GetParameter(7));
	Gauss4Poiss2->SetParameter(8, Gauss4Poiss2->GetParameter(8));
	Gauss4Poiss2->SetParameter(9, Gauss4Poiss2->GetParameter(9));
	Gauss4Poiss2->SetParameter(10,Gauss4Poiss2->GetParameter(10));
	Gauss4Poiss2->SetParameter(11,Gauss4Poiss2->GetParameter(11));
	Gauss4Poiss2->SetParameter(12,Gauss4Poiss2->GetParameter(12));
	Gauss4Poiss2->SetParameter(13,Gauss4Poiss2->GetParameter(13));
	Gauss4Poiss2->SetParameter(14,Gauss4Poiss2->GetParameter(14));
	Gauss4Poiss2->SetParameter(15,Gauss4Poiss2->GetParameter(15));
	Gauss4Poiss2->SetLineColor(5);

	fscaled[ipmt]->Fit("Gauss4Poiss2","RQ+");
	 
	// Statistics box for display  parameters from the Gauss4Poiss2 fit
	// p0 = Offset Gauss1,  p1 = Mean Gauss1, p2 = stdDev Gauss1, p3 = Offset Gauss2,  p4 = Mean Gauss2, p5 = stdDev Gauss2, p6 = Offset Gauss3,  p7 = Mean Gauss3, p8 = stdDev Gauss3,
	// p9 = Offset Gauss4,  p10 = Mean Gauss4, p11 = stdDev Gauss4, p12 = mean  Poiss1, p13 = amplitude Poiss1, p14 = mean Poiss2, p15 = amplitude Poiss2
	Double_t  p0, p0_err, p1, p1_err, p2, p2_err, p3, p3_err, p4, p4_err, p5, p5_err, p6, p6_err, p7, p7_err, p8, p8_err,p9,p9_err,p10,p10_err,p11,p11_err,p12, p12_err, p13, p13_err, p14, p14_err, p15, p15_err, Chi, NDF;

	Chi = Gauss4Poiss2->GetChisquare();    NDF = Gauss4Poiss2->GetNDF(); 
	p0 = Gauss4Poiss2->GetParameter(0);    p0_err = Gauss4Poiss2->GetParError(0);
	p1 = Gauss4Poiss2->GetParameter(1);    p1_err = Gauss4Poiss2->GetParError(1);	  
	p2 = Gauss4Poiss2->GetParameter(2);    p2_err = Gauss4Poiss2->GetParError(2);	  
	p3 = Gauss4Poiss2->GetParameter(3);    p3_err = Gauss4Poiss2->GetParError(3);	  
	p4 = Gauss4Poiss2->GetParameter(4);    p4_err = Gauss4Poiss2->GetParError(4);	  
	p5 = Gauss4Poiss2->GetParameter(5);    p5_err = Gauss4Poiss2->GetParError(5);	  
	p6 = Gauss4Poiss2->GetParameter(6);    p6_err = Gauss4Poiss2->GetParError(6);	  
	p7 = Gauss4Poiss2->GetParameter(7);    p7_err = Gauss4Poiss2->GetParError(7);	  
	p8 = Gauss4Poiss2->GetParameter(8);    p8_err = Gauss4Poiss2->GetParError(8);	  
	p9 = Gauss4Poiss2->GetParameter(9);    p9_err = Gauss4Poiss2->GetParError(9);
	p10 = Gauss4Poiss2->GetParameter(10);  p10_err = Gauss4Poiss2->GetParError(10);	  
	p11 = Gauss4Poiss2->GetParameter(11);  p11_err = Gauss4Poiss2->GetParError(11);	  
	p12 = Gauss4Poiss2->GetParameter(12);  p12_err = Gauss4Poiss2->GetParError(12);
	p13 = Gauss4Poiss2->GetParameter(13);  p13_err = Gauss4Poiss2->GetParError(13);
	p14 = Gauss4Poiss2->GetParameter(14);  p14_err = Gauss4Poiss2->GetParError(14);
	p15 = Gauss4Poiss2->GetParameter(15);  p15_err = Gauss4Poiss2->GetParError(15);
      
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
      
	// Draw individual function from the Fuction fit
	TF1 *g3 = new TF1("g3","gaus",0,35.0);
	{	
	  g3->SetParameter(0,Gauss4Poiss2->GetParameter(0));
	  g3->SetParameter(1,Gauss4Poiss2->GetParameter(1));
	  g3->SetParameter(2,Gauss4Poiss2->GetParameter(2));
	  g3->SetLineColor(1);
	  g3->Draw("same");
	}

	TF1 *g4 = new TF1("g4","gaus",0,35.0);
	{
	  g4->SetParameter(0,Gauss4Poiss2->GetParameter(3));
	  g4->SetParameter(1,Gauss4Poiss2->GetParameter(4));
	  g4->SetParameter(2,Gauss4Poiss2->GetParameter(5));
	  g4->SetLineColor(3);
	  g4->Draw("same");
	}

	TF1 *g5 = new TF1("g5","gaus",0,35.0);
	{
	  g5->SetParameter(0,Gauss4Poiss2->GetParameter(6));
	  g5->SetParameter(1,Gauss4Poiss2->GetParameter(7));
	  g5->SetParameter(2,Gauss4Poiss2->GetParameter(8));
	  g5->SetLineColor(4);
	  g5->Draw("same");
	}

	TF1 *g6 = new TF1("g6","gaus",0,35.0);
	{
	  g6->SetParameter(0,Gauss4Poiss2->GetParameter(9));
	  g6->SetParameter(1,Gauss4Poiss2->GetParameter(10));
	  g6->SetParameter(2,Gauss4Poiss2->GetParameter(11));
	  g6->SetLineColor(2);
	  g6->Draw("same");
	}
	TF1 *g7 = new TF1("g7","[1]*TMath::Poisson(x,[0])",0.0,35.0);
	{
	  g7->SetRange(0,35);
	  g7->SetParameter(0,Gauss4Poiss2->GetParameter(12));
	  g7->SetParameter(1,Gauss4Poiss2->GetParameter(13));
	  g7->SetLineColor(6);
	  g7->Draw("same"); 
	}

	TF1 *g8 = new TF1("g8","[1]*TMath::Poisson(x,[0])",0.0,35.0);
	{
	  g8->SetParameter(0,Gauss4Poiss2->GetParameter(14));
	  g8->SetParameter(1,Gauss4Poiss2->GetParameter(15));
	  g8->SetLineColor(802);
	  g8->Draw("same"); 
	}
	background_ipmt->Print(outputpdf);

	//Tcanvas for draw the zoomed clone histogram 
	Full_zoom_fit_ipmt = new TCanvas(Form("Full_zoom_fit_pmt%d",ipmt), Form("Zoom in NPE spectra for PMT%d with all fits",ipmt+1));
	scaled_clone->GetXaxis()->SetRangeUser(0,5); 
	Full_zoom_fit_ipmt->cd();
	scaled_clone->Draw();

	TF1 *Gauss4Poiss2_clone = new TF1("Gauss4Poiss2_clone",fun_4gauss_2poisson, 0.0, 35.0, 16.0);
	{
	  Gauss4Poiss2_clone->SetParameter(0,Gauss4Poiss2->GetParameter(0));
	  Gauss4Poiss2_clone->SetParameter(1,Gauss4Poiss2->GetParameter(1));	
	  Gauss4Poiss2_clone->SetParameter(2,Gauss4Poiss2->GetParameter(2));
	  Gauss4Poiss2_clone->SetParameter(3,Gauss4Poiss2->GetParameter(3));
	  Gauss4Poiss2_clone->SetParameter(4,Gauss4Poiss2->GetParameter(4));
	  Gauss4Poiss2_clone->SetParameter(5,Gauss4Poiss2->GetParameter(5));
	  Gauss4Poiss2_clone->SetParameter(6,Gauss4Poiss2->GetParameter(6));
	  Gauss4Poiss2_clone->SetParameter(7,Gauss4Poiss2->GetParameter(7));
	  Gauss4Poiss2_clone->SetParameter(8,Gauss4Poiss2->GetParameter(8));
	  Gauss4Poiss2_clone->SetParameter(9,Gauss4Poiss2->GetParameter(9));
	  Gauss4Poiss2_clone->SetParameter(10,Gauss4Poiss2->GetParameter(10));
	  Gauss4Poiss2_clone->SetParameter(11,Gauss4Poiss2->GetParameter(11));
	  Gauss4Poiss2_clone->SetParameter(12,Gauss4Poiss2->GetParameter(12));
	  Gauss4Poiss2_clone->SetParameter(13,Gauss4Poiss2->GetParameter(13));
	  Gauss4Poiss2_clone->SetParameter(14,Gauss4Poiss2->GetParameter(14));
	  Gauss4Poiss2_clone->SetParameter(15,Gauss4Poiss2->GetParameter(15));	   
	  Gauss4Poiss2_clone->SetLineColor(5);
	  Gauss4Poiss2_clone->Draw("same");
	}
	      	  
	TF1 *g9 = new TF1("g9","gaus",0,35.0);
	{  
	  g9->SetParameter(0,Gauss4Poiss2->GetParameter(0));
	  g9->SetParameter(1,Gauss4Poiss2->GetParameter(1));
	  g9->SetParameter(2,Gauss4Poiss2->GetParameter(2));
	  g9->SetLineColor(1);
	  g9->Draw("same");
	}
	TF1 *g10 = new TF1("g10","gaus",0,35.0);
	{
	  g10->SetParameter(0,Gauss4Poiss2->GetParameter(3));
	  g10->SetParameter(1,Gauss4Poiss2->GetParameter(4));
	  g10->SetParameter(2,Gauss4Poiss2->GetParameter(5));
	  g10->SetLineColor(3);
	  g10->Draw("same");
	}
	TF1 *g11 = new TF1("g11","gaus",0,35.0);
	{
	  g11->SetParameter(0,Gauss4Poiss2->GetParameter(6));
	  g11->SetParameter(1,Gauss4Poiss2->GetParameter(7));
	  g11->SetParameter(2,Gauss4Poiss2->GetParameter(8));
	  g11->SetLineColor(4);
	  g11->Draw("same");
	}

	TF1 *g12 = new TF1("g12","gaus",0,35.0);
	{
	  g12->SetParameter(0,Gauss4Poiss2->GetParameter(9));
	  g12->SetParameter(1,Gauss4Poiss2->GetParameter(10));
	  g12->SetParameter(2,Gauss4Poiss2->GetParameter(11));
	  g12->SetLineColor(2);
	  g12->Draw("same");
	}

	TF1 *g13 = new TF1("g13","[1]*TMath::Poisson(x,[0])",0.0,35.0);
	{
	  g13->SetRange(0,35);
	  g13->SetParameter(0,Gauss4Poiss2->GetParameter(12));
	  g13->SetParameter(1,Gauss4Poiss2->GetParameter(13));
	  g13->SetLineColor(6);
	  g13->Draw("same");
	}

	TF1 *g14 = new TF1("g14","[1]*TMath::Poisson(x,[0])",0.0,35.0);
	{
	  g14->SetParameter(0,Gauss4Poiss2->GetParameter(14));
	  g14->SetParameter(1,Gauss4Poiss2->GetParameter(15));
	  g14->SetLineColor(802);
	  g14->Draw("same");
	} 
	Full_zoom_fit_ipmt->Print(outputpdf);
	
	//TCanvas for the linear spacing of photo-electrons
	final_spectra_ipmt = new TCanvas(Form("final_Spectra_%d",ipmt), Form("NPE spectra for PMT%d",ipmt+1));
	y_npe[0] = Gauss4Poiss2->GetParameter(1), y_npe[1] = Gauss4Poiss2->GetParameter(4), y_npe[2] = Gauss4Poiss2->GetParameter(7);
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

	gr_npe->Draw("A*"); 

	//Fit with another linear function when intercept is zero
	TF1 *g15 = new TF1("g15","[0]*x", 0.0,5.0);
	{
	  g15->SetParNames("Slope");
	  g15->SetParameter(0.0,1.0);
	  g15->SetLineColor(2);
	}

	gr_npe->Fit("Linear","RQ");
	gr_npe->Fit("g15","RQ+");

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
	if(ipmt ==0){
	  final_spectra_ipmt->Print(outputpdf);
	}
	if(ipmt ==1){
	  final_spectra_ipmt->Print(outputpdf);
	}
	if(ipmt ==2){
	  final_spectra_ipmt->Print(outputpdf);
	} 
	if(ipmt ==3){
	  final_spectra_ipmt->Print(outputpdf + ')');
	} 

	calibration_mk1[ipmt] = xscale;
	calibration_mk1Err[ipmt] = xscaleErr;
	pmt_calib[ipmt] = abs(1.0 - Gauss4Poiss2->GetParameter(1));
	  
	//Initial calibration constant has been obtained. Now I multiply it by the slope of the spacing of the NPE (should be approx. 1) for a second estimate
	Double_t xscale_mk2 = xscale * p1;  
	Double_t xscale_mk2Err = Sqrt(Power(xscaleErr*p1, 2) +  Power(xscale*p1_err,2)); 
	
	calibration_mk2[ipmt] = xscale_mk2;
	calibration_mk2Err[ipmt] = xscale_mk2Err;
	pmt_calib_mk2[ipmt] = abs(1.0 - Gauss4Poiss2->GetParameter(1));

	// Take the error weighted average of Guess1 and Guess2 parameters

	calibration_weighted_ave[ipmt] = (calibration_mk1[ipmt]/pow(calibration_mk1Err[ipmt], 2) + calibration_mk2[ipmt]/pow(calibration_mk2Err[ipmt], 2))/(1/pow(calibration_mk1Err[ipmt], 2) + 1/pow(calibration_mk2Err[ipmt], 2));

	calibration_weighted_ave_err[ipmt] = 1/TMath:: Sqrt((1/pow(calibration_mk1Err[ipmt], 2) + 1/pow(calibration_mk2Err[ipmt], 2))); 

      
      } // This brace marks the end of the quadrant cut strategy
    } 


  cout << "Calibration constants are \nPMT#:   First Guess" << setw(25) << "Second Guess\n" << endl;
  for (Int_t i=0; i<4; i++){
    cout << Form("PMT%d:", i+1) << setw(8) << Form("%3.3f", calibration_mk1[i]) << " +/- " << Form("%3.3f", calibration_mk1Err[i]) << setw(13) << Form("%3.3f", calibration_mk2[i]) << " +/- " << Form("%3.3f", calibration_mk2Err[i]) <<  "\n";
  }
  printf("\n");

  //if (fCut) cout << (Pois_Chi[0] < Pois_Chi[1] ? "First Guess":"Second Guess") << " better characterizes the full Poisson character" << endl;

  //Start the process of writing the calibration information to file
  ofstream calibration_out;
  if(RunNumStart == RunNumEnd){
    calibration_out.open(Form("Calibration_plots/phgcer_calib_%i.param", RunNumStart), ios::out);
    calibration_out << Form("; Calibration parameters from calibrating run %i", RunNumStart) << endl; 
  }
  else
    {
      calibration_out.open(Form("Calibration_plots/phgcer_calib_%i-%i.param", RunNumStart, RunNumEnd), ios::out);
      calibration_out << Form("; Calibration parameters from calibrating runs %i-%i", RunNumStart, RunNumEnd) << endl;
    }
  if (!calibration_out.is_open()) cout << "Problem saving calibration constants, may have to update constants manually!" << endl;
  
  else
    {
      calibration_out << ("; Weighted average of first and second guesses") << endl;
      calibration_out << (" phgcer_adc_to_npe = ");
      for (Int_t ipmt = 0; ipmt < 4; ipmt++)
	{
	  if (ipmt != 3) calibration_out << Form("1.0/%3.3f, ",calibration_weighted_ave[ipmt]);
	  // For final PMT, do not print trailing comma and whitespace after value
	  else if (ipmt == 3) calibration_out << Form("1.0/%3.3f",calibration_weighted_ave[ipmt]);
	}
      calibration_out.close();
    }
  // Second time to save the parameters
  ofstream calibration_out2;
  if(RunNumStart == RunNumEnd)
    {
      calibration_out2.open(Form("Calibration_plots/phgcer_calib_err_%i.param", RunNumStart), ios::out);
    }
  else
    {
      calibration_out2.open(Form("Calibration_plots/phgcer_calib_err_%i-%i.param", RunNumStart, RunNumEnd), ios::out);
    }
  if (!calibration_out2.is_open()) cout << "Problem saving calibration constants, may have to update constants manually!" << endl;
  else
    {
      calibration_out2 <<("phgcer_adc_to_npe are: ")<<endl; 
      for (Int_t ipmt = 0; ipmt < 4; ipmt++){
	calibration_out2 << Form("%3i %3.3f  %3.3f",ipmt+1, calibration_mk1[ipmt],  calibration_mk1Err[ipmt] )<<endl;
      }
      calibration_out2 << ("From quality control")<<endl;
      for (Int_t ipmt = 0; ipmt < 4; ipmt++){
	calibration_out2 << Form("%3i %3.3f  %3.3f", ipmt+1, calibration_mk2[ipmt],  calibration_mk2Err[ipmt])<<endl;
      }
      calibration_out2 << ("Weighted Average")<<endl;
      for (Int_t ipmt = 0; ipmt < 4; ipmt++){
	calibration_out2 << Form("%3i %3.3f  %3.3f", ipmt+1, calibration_weighted_ave[ipmt], calibration_weighted_ave_err[ipmt])<<endl;
      }
      calibration_out2.close();
    }  
}
