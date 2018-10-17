// Macro to perform time-walk fits and extract the calibration parameters
// Author: Eric Pooser, pooser@jlab.org
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
#include <TPaveText.h>
#include <TProfile.h>
#include <TPolyLine.h>
#include <TObjArray.h>
#include <TMultiGraph.h>
#include <TF1.h>



// Declare ROOT files
TFile *histoFile, *outFile;

// Declare Output  Parameter File
ofstream outParam;

// Declare constants
static const UInt_t nPlanes    = 4;
static const UInt_t nSides     = 2;
static const UInt_t nBarsMax   = 16;
static const UInt_t nTwFitPars = 2;

static const Double_t tdcThresh      = 120.0;  // 30 mV in units of FADC channels
static const Double_t twFitRangeLow  = 20.0;
static const Double_t twFitRangeHigh = 600.0;
static const Double_t c0twParInit    = 1.0;
static const Double_t c1twParInit    = 1.0;

//Parameter values to be written to param file
Double_t c1[nPlanes][nSides][nBarsMax] = {0.};
Double_t c2[nPlanes][nSides][nBarsMax] = {0.};


static const Double_t fontSize       = 0.05;
static const Double_t yTitleOffset   = 0.75;
static const Double_t markerSize     = 2.0;
static const Double_t minScale       = 0.75;
static const Double_t maxScale       = 0.75;

static const UInt_t lineWidth = 4;
static const UInt_t lineStyle = 7;

static const UInt_t  nbars[nPlanes]      = {16, 10, 16, 10};
static const TString planeNames[nPlanes] = {"1x", "1y", "2x", "2y"};
static const TString sideNames[nSides]   = {"pos", "neg"};
static const TString twFitParNames[nTwFitPars]  = {"c_{1}", "c_{2}"};

// Declare directories
TDirectory *dataDir, *planeDir[nPlanes], *sideDir[nPlanes][nSides];
TDirectory *twDir[nPlanes][nSides];
// Declare fits
TF1 *twFit[nPlanes][nSides][nBarsMax], *avgParFit[nPlanes][nSides][nTwFitPars];
// Declare arrays
Double_t paddleIndex[nPlanes][nSides][nBarsMax];
Double_t twFitPar[nPlanes][nSides][nTwFitPars][nBarsMax], twFitParErr[nPlanes][nSides][nTwFitPars][nBarsMax];
Double_t avgPar[nPlanes][nSides][nTwFitPars];
Double_t minPar[nPlanes][nSides][nTwFitPars], maxPar[nPlanes][nSides][nTwFitPars];
Double_t chi2ndf[nPlanes][nSides][nBarsMax];
// Declare canvases
TCanvas *twFitCan[nPlanes][nSides], *twFitParCan[nTwFitPars];
// Declare histos
// 2D histos
TH2F *h2_adcTdcTimeDiffWalk[nPlanes][nSides][nBarsMax];
// Declare graphs
TGraph *twFitParGraph[nPlanes][nSides][nTwFitPars];
// Declare multi-graphs
TMultiGraph *twFitParMultiGraph[nPlanes][nTwFitPars];
// Declare legends
TLegend *twFitParLegend[nPlanes][nTwFitPars], *avgParLegend[nPlanes][nTwFitPars];
// Declare text objects
TPaveText *twFitParText[nPlanes][nSides][nBarsMax];

// Functions to extract calibration parameters
//=:=:=:=:=:=:
//=: Level 1
//=:=:=:=:=:=:

// Add color to fit lines
void addColorToFitLine(UInt_t style, UInt_t width, UInt_t color, TF1 *fit1D) {
  fit1D->SetLineStyle(lineStyle);
  fit1D->SetLineWidth(lineWidth);
  fit1D->SetLineColor(color);
  return;
} // addColorToFitLine()

// Add color to fit graphs
void addColorToGraph(UInt_t style, UInt_t size, UInt_t color, TGraph *graph) {
  graph->SetMarkerStyle(style);
  graph->SetMarkerSize(size);
  graph->SetMarkerColor(color);
  return;
} // addColorToFitGraph()

// Add centered titles to multigraphs
void modifyMultiGraph(TMultiGraph *mg, TString xtitle, TString ytitle) {
  mg->GetYaxis()->SetTitle(ytitle);
  mg->GetYaxis()->CenterTitle();
  mg->GetXaxis()->SetTitle(xtitle);
  mg->GetXaxis()->CenterTitle();
  return;
} // modifyMultiGraph()

// Create canvases
TCanvas *makeCan(UInt_t numColumns, UInt_t numRows, UInt_t winWidth, UInt_t winHeight, TCanvas *can, TString name, TString title) {
  can = new TCanvas(name, title, winWidth, winHeight);  
  can->Divide(numColumns, numRows);
  return can;
} // makeCan()

// Define the time-walk fit function
Double_t twFitFunc(Double_t *a, Double_t *c) {
  Double_t twFitVal = c[0] + 1./(TMath::Power((a[0]/tdcThresh), c[1]));
  return twFitVal;
} // twFitFunc()

// Locate min or max value from input array
Double_t calcMinOrMax(Double_t *array, UInt_t iplane, TString minOrmax) {
  auto result = minmax_element(array, array+nbars[iplane]);
  if      (minOrmax == "min") return *result.first;
  else if (minOrmax == "max") return *result.second;
  else return 0.0;
} // calcMinOrMax()

//=:=:=:=:=:=:
//=: Level 2
//=:=:=:=:=:=:

// Perform the timw-walk fits
void doTwFits(UInt_t iplane, UInt_t iside, UInt_t ipaddle) {
  // Draw fits on canvas
  twFitCan[iplane][iside]->cd(ipaddle+1);
  gPad->SetLogz();
  h2_adcTdcTimeDiffWalk[iplane][iside][ipaddle]->Draw("COLZ");
  gPad->Modified(); gPad->Update();
  // Declare and initialize the fits
  twFit[iplane][iside][ipaddle] = new TF1("twFit", twFitFunc, twFitRangeLow, twFitRangeHigh, nTwFitPars);
  for (UInt_t ipar = 0; ipar < nTwFitPars; ipar++)
    twFit[iplane][iside][ipaddle]->SetParName(ipar, twFitParNames[ipar]);
  twFit[iplane][iside][ipaddle]->SetParameter(0, c0twParInit);
  twFit[iplane][iside][ipaddle]->SetParameter(1, c1twParInit);

  // Perform the fits and scream if it failed
  Int_t twFitStatus = h2_adcTdcTimeDiffWalk[iplane][iside][ipaddle]->Fit("twFit", "REQ");	
  if (twFitStatus != 0) 
    cout << "ERROR: Time Walk Fit Failed!!! " << "Status = " << twFitStatus << " For Plane: " <<  planeNames[iplane] << " Side: " << sideNames[iside] << " Paddle: " << ipaddle+1 << endl;		
  // Create text box to display fir parameters
  twFitParText[iplane][iside][ipaddle] = new TPaveText(0.4, 0.6, 0.895, 0.895, "NBNDC");
  twFitParText[iplane][iside][ipaddle]->AddText(Form("Entries = %.0f", h2_adcTdcTimeDiffWalk[iplane][iside][ipaddle]->GetEntries()));
  // Obtain the fit parameters and associated errors
  //  chi2[iplane][iside][ipaddle] = twFit[iplane][iside][ipaddle]->GetChisquare();
  for (UInt_t ipar = 0; ipar < nTwFitPars; ipar++) {
    twFitPar[iplane][iside][ipar][ipaddle]    = twFit[iplane][iside][ipaddle]->GetParameter(ipar);
    twFitParErr[iplane][iside][ipar][ipaddle] = twFit[iplane][iside][ipaddle]->GetParError(ipar);
    twFitParText[iplane][iside][ipaddle]->AddText(Form(twFitParNames[ipar]+" = %.2f #pm %.2f", twFitPar[iplane][iside][ipar][ipaddle], twFitParErr[iplane][iside][ipar][ipaddle]));
  } // Parameter loop
    twFitParText[iplane][iside][ipaddle]->AddText(Form("#chi^{2}/NDF = %.2f", twFit[iplane][iside][ipaddle]->GetChisquare()/twFit[iplane][iside][ipaddle]->GetNDF()));

 // Draw the fit parameter text
  twFitParText[iplane][iside][ipaddle]->SetFillColor(kWhite);
  twFitParText[iplane][iside][ipaddle]->SetTextAlign(12);
  twFitParText[iplane][iside][ipaddle]->SetFillStyle(3003);
  twFitParText[iplane][iside][ipaddle]->Draw();
  gPad->Modified(); gPad->Update();
  return;
} // doTwFits()

// Calculate the averege of the time-walk fit parameters
void calcParAvg(UInt_t iplane, UInt_t iside) {
  for (UInt_t ipar = 0; ipar < nTwFitPars; ipar++) {
    // Calculate the weighted average while ignoring fit errors provided by Minuit
    avgParFit[iplane][iside][ipar] = new TF1("avgParFit", "pol0", 1, nbars[iplane]);
    avgParFit[iplane][iside][ipar]->SetParName(0, "#bar{"+twFitParNames[ipar]+"}");
    // Add color to fit lines
    if (iside == 0) addColorToFitLine(lineStyle, lineWidth, kRed,  avgParFit[iplane][iside][ipar]);
    if (iside == 1) addColorToFitLine(lineStyle, lineWidth, kBlue, avgParFit[iplane][iside][ipar]);
    // Initialize the parameters
    if (ipar == 0) avgParFit[iplane][iside][ipar]->SetParameter(0, c0twParInit);
    if (ipar == 1) avgParFit[iplane][iside][ipar]->SetParameter(1, c1twParInit);
    // Perform the least squares fit 
    Int_t avgParFitStatus = twFitParGraph[iplane][iside][ipar]->Fit("avgParFit", "REQ");
    if (avgParFitStatus != 0) 
      cout << "ERROR: Parameter Fit Failed!!! " << "Status = " << avgParFitStatus << " For Plane: " <<  planeNames[iplane] << " Side: " << sideNames[iside] << endl;
    // Store the fit parameters
    avgPar[iplane][iside][ipar] = avgParFit[iplane][iside][ipar]->GetParameter(0);
    // Add graphs to multi graph
    twFitParMultiGraph[iplane][ipar]->Add(twFitParGraph[iplane][iside][ipar]);
    // Calculate the min and max value of each parameter
    minPar[iplane][iside][ipar] = calcMinOrMax(twFitPar[iplane][iside][ipar], iplane, "min");
    maxPar[iplane][iside][ipar] = calcMinOrMax(twFitPar[iplane][iside][ipar], iplane, "max");
  } // Parameter loop
  return;
} // calcParAvg()

// Draw the time-walk fit parameter multi-graph
void drawParams(UInt_t iplane) {
  // Populate the parameter canvases
  for (UInt_t ipar = 0; ipar < nTwFitPars; ipar++) {
    twFitParCan[ipar]->cd(iplane+1);
    twFitParMultiGraph[iplane][ipar]->Draw("AP");
    gPad->Modified(); gPad->Update();
    // Add aesthetics
    modifyMultiGraph(twFitParMultiGraph[iplane][ipar], "Paddle Number", "Fit Parameter "+twFitParNames[ipar]);
    twFitParMultiGraph[iplane][ipar]->GetXaxis()->SetNdivisions(21);
    twFitParMultiGraph[iplane][ipar]->Draw("AP");
    // Draw legends
    twFitParLegend[iplane][ipar] = new TLegend(0.1, 0.7, 0.3, 0.9);
    avgParLegend[iplane][ipar]   = new TLegend(0.6, 0.7, 0.9, 0.9);
    for(UInt_t iside = 0; iside < nSides; iside++) {
      if (iside == 0) twFitParLegend[iplane][ipar]->AddEntry(twFitParGraph[iplane][iside][ipar], "Positive Side", "p");
      if (iside == 1) twFitParLegend[iplane][ipar]->AddEntry(twFitParGraph[iplane][iside][ipar], "Negative Side", "p");
      avgParLegend[iplane][ipar]->AddEntry(avgParFit[iplane][iside][ipar], "#bar{"+twFitParNames[ipar]+"}"+Form(" = %.2f", avgPar[iplane][iside][ipar]),"l");
      // Define the time-walk fit parameter multigraph y-axis range
      if (iside == 0) {
	// Set y-axis range min
	if (minPar[iplane][iside][ipar] < 0.0 || minPar[iplane][iside+1][ipar] < 0.0) {
	  if (minPar[iplane][iside][ipar] < minPar[iplane][iside+1][ipar]) 
	    twFitParMultiGraph[iplane][ipar]->SetMinimum(minPar[iplane][iside][ipar] + minScale*minPar[iplane][iside][ipar]);
	  else twFitParMultiGraph[iplane][ipar]->SetMinimum(minPar[iplane][iside+1][ipar] + minScale*minPar[iplane][iside+1][ipar]);
	}
	if (minPar[iplane][iside][ipar] > 0.0 || minPar[iplane][iside+1][ipar] > 0.0) {
	  if (minPar[iplane][iside][ipar] < minPar[iplane][iside+1][ipar]) 
	    twFitParMultiGraph[iplane][ipar]->SetMinimum(minPar[iplane][iside][ipar] - minScale*minPar[iplane][iside][ipar]);
	  else twFitParMultiGraph[iplane][ipar]->SetMinimum(minPar[iplane][iside+1][ipar] - minScale*minPar[iplane][iside+1][ipar]);
	}
	// Set y-axis range max
	if (maxPar[iplane][iside][ipar] < 0.0 || maxPar[iplane][iside+1][ipar] < 0.0) {
	  if (maxPar[iplane][iside][ipar] < maxPar[iplane][iside+1][ipar])
	    twFitParMultiGraph[iplane][ipar]->SetMaximum(maxPar[iplane][iside+1][ipar] - maxScale*maxPar[iplane][iside+1][ipar]);
	  else twFitParMultiGraph[iplane][ipar]->SetMaximum(maxPar[iplane][iside][ipar] - maxScale*maxPar[iplane][iside][ipar]);
	}
	if (maxPar[iplane][iside][ipar] > 0.0 || maxPar[iplane][iside+1][ipar] > 0.0) {
	  if (maxPar[iplane][iside][ipar] < maxPar[iplane][iside+1][ipar])
	    twFitParMultiGraph[iplane][ipar]->SetMaximum(maxPar[iplane][iside+1][ipar] + maxScale*maxPar[iplane][iside+1][ipar]);
	  else twFitParMultiGraph[iplane][ipar]->SetMaximum(maxPar[iplane][iside][ipar] + maxScale*maxPar[iplane][iside][ipar]);
	}
      } // Side index = 0 condition
    } // Side loop
    twFitParLegend[iplane][ipar]->Draw();
    avgParLegend[iplane][ipar]->Draw();
  } // Parameter loop
  return;
} // drawParams


//Add a method to Get Fit Parameters
void WriteFitParam(int runNUM)
{

  TString outPar_Name = Form("../../PARAM/HMS/HODO/hhodo_TWcalib_%d.param", runNUM);
  outParam.open(outPar_Name);
  outParam << Form(";HMS Hodoscopes Output Parameter File: Run %d", runNUM) << endl;
  outParam << " " << endl;
  //  outParam << "htofusinginvadc=0 " << " ;set to zero to NOT read old style hodo calib parameters" << endl;
  outParam << "hTDC_threshold=" << tdcThresh << ". ;units of mV " << endl;
  outParam << " " << endl;
  
  //Fill 3D Par array
  for (UInt_t iplane=0; iplane < nPlanes; iplane++)
    {
      
      for (UInt_t iside=0; iside < nSides; iside++) {
	      

	for(UInt_t ipaddle = 0; ipaddle < nbars[iplane]; ipaddle++) {
	 
	  c1[iplane][iside][ipaddle] = twFit[iplane][iside][ipaddle]->GetParameter("c_{1}");
	  c2[iplane][iside][ipaddle] = twFit[iplane][iside][ipaddle]->GetParameter("c_{2}");
	  chi2ndf[iplane][iside][ipaddle] =  twFit[iplane][iside][ipaddle]->GetChisquare()/twFit[iplane][iside][ipaddle]->GetNDF();

	} //end paddle loop

      } //end side loop
    
    } //end plane loop

  //Wrtie to Param FIle
   
  outParam << ";Param c1-Pos" << endl;
  outParam << "; " << setw(12) << "1x " << setw(15) << "1y " << setw(15) << "2x " << setw(15) << "2y " << endl;
  outParam << "hc1_Pos = ";
  //Loop over all paddles
  for(UInt_t ipaddle = 0; ipaddle < nBarsMax; ipaddle++) {
    //Write c1-Pos values
    if(ipaddle==0){
    outParam << c1[0][0][ipaddle]  << "," << setw(15) << c1[1][0][ipaddle] << ","  << setw(15) << c1[2][0][ipaddle] << ","  << setw(15) << c1[3][0][ipaddle] << fixed << endl; 
    }
    else {
      outParam << setw(17) << c1[0][0][ipaddle] << "," << setw(15) << c1[1][0][ipaddle] << ","  << setw(15) << c1[2][0][ipaddle] << "," << setw(15) << c1[3][0][ipaddle] << fixed << endl;    
    }
  } //end loop over paddles
  
  outParam << " " << endl;
  outParam << ";Param c1-Neg" << endl;
  outParam << "; " << setw(12) << "1x " << setw(15) << "1y " << setw(15) << "2x " << setw(15) << "2y " << endl;
  outParam << "hc1_Neg = ";                                                                                                                                                                            
  //Loop over all paddles
  for(UInt_t ipaddle = 0; ipaddle < nBarsMax; ipaddle++) { 
    //Write c1-Neg values
    if(ipaddle==0){
    outParam << c1[0][1][ipaddle] << "," << setw(15) << c1[1][1][ipaddle] << ","  << setw(15) << c1[2][1][ipaddle] << "," << setw(15) << c1[3][1][ipaddle] << fixed << endl; 
    }
    else {
      outParam << setw(17) << c1[0][1][ipaddle] << "," << setw(15) << c1[1][1][ipaddle] << ","  << setw(15) << c1[2][1][ipaddle] << "," << setw(15) << c1[3][1][ipaddle] << fixed << endl;
    }
} //end loop over paddles
  
  outParam << " " << endl;
  outParam << ";Param c2-Pos" << endl;
  outParam << "; " << setw(12) << "1x " << setw(15) << "1y " << setw(15) << "2x " << setw(15) << "2y " << endl;
  outParam << "hc2_Pos = ";                                                                                                                                                                            
  //Loop over all paddles
  for(UInt_t ipaddle = 0; ipaddle < nBarsMax; ipaddle++) { 
    //Write c2-Pos values
    if(ipaddle==0)
      {
	outParam << c2[0][0][ipaddle] << "," << setw(15) << c2[1][0][ipaddle] << ","  << setw(15) << c2[2][0][ipaddle] << "," << setw(15) << c2[3][0][ipaddle] << fixed << endl; 
      }
    else {
      outParam << setw(17) << c2[0][0][ipaddle] << "," << setw(15) << c2[1][0][ipaddle] << ","  << setw(15) << c2[2][0][ipaddle] << "," << setw(15) << c2[3][0][ipaddle] << fixed << endl;                                            
    }
  } //end loop over paddles
  
  outParam << " " << endl;
  outParam << ";Param c2-Neg" << endl;
  outParam << "; " << setw(12) << "1x " << setw(15) << "1y " << setw(15) << "2x " << setw(15) << "2y " << endl;
  outParam << "hc2_Neg = ";                                                                                                                                                                            
  //Loop over all paddles
  for(UInt_t ipaddle = 0; ipaddle < nBarsMax; ipaddle++) { 
    //Write c2-Neg values
    if (ipaddle==0){
    outParam << c2[0][1][ipaddle] << "," << setw(15) << c2[1][1][ipaddle] << ","  << setw(15) << c2[2][1][ipaddle]<< ","  << setw(15) << c2[3][1][ipaddle] << fixed << endl; 
    }
    else{
      outParam << setw(17) << c2[0][1][ipaddle]<< ","  << setw(15) << c2[1][1][ipaddle]<< ","   << setw(15) << c2[2][1][ipaddle]<< ","  << setw(15) << c2[3][1][ipaddle] << fixed << endl;
    }
  } //end loop over paddles
  
  outParam.close();
} //end method


//=:=:=:=:=
//=: Main
//=:=:=:=:=

void timeWalkCalib(int run) {

using namespace std;

//prevent root from displaying graphs while executing
//gROOT->SetBatch(1);

  // ROOT settings
  gStyle->SetTitleFontSize(fontSize);
  gStyle->SetLabelSize(fontSize, "XY");
  gStyle->SetTitleSize(fontSize, "XY");
  gStyle->SetTitleOffset(yTitleOffset, "Y");
  gStyle->SetStatFormat(".2f");
  gStyle->SetOptFit(0);
  gStyle->SetOptStat(0);

  // Read the ROOT file containing the time-walk histos
  histoFile = new TFile("timeWalkHistos.root", "READ");
  // Obtain the top level directory
  dataDir = dynamic_cast <TDirectory*> (histoFile->FindObjectAny("hodoUncalib"));
  // Create the parameter canvases
  for (UInt_t ipar = 0; ipar < nTwFitPars; ipar++)
    twFitParCan[ipar] = makeCan(2, 2, 1600, 800, twFitParCan[ipar], twFitParNames[ipar]+"FitParCan", "Parameter "+twFitParNames[ipar]+" Canvas");
  // Loop over the planes
  for(UInt_t iplane = 0; iplane < nPlanes; iplane++) {
    //for(UInt_t iplane = 0; iplane < 1; iplane++) {
    // Obtain the plane directory
    planeDir[iplane] = dynamic_cast <TDirectory*> (dataDir->FindObjectAny(planeNames[iplane]));
    // Create multigraphs
    for (UInt_t ipar = 0; ipar < nTwFitPars; ipar++) 
      twFitParMultiGraph[iplane][ipar] = new TMultiGraph(planeNames[iplane]+"_"+twFitParNames[ipar]+"_Multigraph", "Plane "+planeNames[iplane]+" Parameter "+twFitParNames[ipar]);
    // Loop over the sides
    for(UInt_t iside = 0; iside < nSides; iside++) {
      // Obtain the side and time walk directories
      sideDir[iplane][iside] = dynamic_cast <TDirectory*> (planeDir[iplane]->FindObjectAny(sideNames[iside]));
      twDir[iplane][iside]   = dynamic_cast <TDirectory*> (sideDir[iplane][iside]->FindObjectAny("adcTdcTimeDiffWalk"));
      // Create the time-walk histo and fit canvases
      if (planeNames[iplane] != "1y" || planeNames[iplane] != "2y") twFitCan[iplane][iside] = makeCan(4, 4, 1600, 800, twFitCan[iplane][iside], planeNames[iplane]+"_"+sideNames[iside]+"_twFitCan", planeNames[iplane]+"_"+sideNames[iside]+"_twFitCan");
      if (planeNames[iplane] == "1y" || planeNames[iplane] == "2y") twFitCan[iplane][iside] = makeCan(4, 3, 1600, 800, twFitCan[iplane][iside], planeNames[iplane]+"_"+sideNames[iside]+"_twFitCan", planeNames[iplane]+"_"+sideNames[iside]+"_twFitCan");
      // Loop over the paddles
      for(UInt_t ipaddle = 0; ipaddle < nbars[iplane]; ipaddle++) {
	// Populate the paddle index arrays
	paddleIndex[iplane][iside][ipaddle] = Double_t (ipaddle + 1);
	// Obtain the time-walk histos
	h2_adcTdcTimeDiffWalk[iplane][iside][ipaddle] = dynamic_cast <TH2F*> (twDir[iplane][iside]->FindObjectAny(Form("h2_adcTdcTimeDiffWalk_paddle_%d", ipaddle+1)));
	// Perform the time-walk fits while ignoring the S2Y plane (for now)
	// if (planeNames[iplane] == "2y") continue;
	doTwFits(iplane, iside, ipaddle);
      } // Paddle loop
      // Produce the time-walk fit parameter graphs
      for (UInt_t ipar = 0; ipar < nTwFitPars; ipar++) {
      	// Populate graphs and multi-graphs
      	twFitParGraph[iplane][iside][ipar] = new TGraph(nbars[iplane], paddleIndex[iplane][iside], twFitPar[iplane][iside][ipar]);
      	if (iside == 0) addColorToGraph(22, markerSize, kRed,  twFitParGraph[iplane][iside][ipar]);
      	if (iside == 1) addColorToGraph(23, markerSize, kBlue, twFitParGraph[iplane][iside][ipar]);
      	// // Add graphs to multi graph
      	// twFitParMultiGraph[iplane][ipar]->Add(twFitParGraph[iplane][iside][ipar]);
      } // Parameter loop
      // Calculate the average of the time-walk fit parameters
      calcParAvg(iplane, iside);
    } // Side loop
    // Draw the time-walk parameter graphs
    drawParams(iplane);
  } // Plane loop 
  //Write to a param file
  WriteFitParam(run);

}






