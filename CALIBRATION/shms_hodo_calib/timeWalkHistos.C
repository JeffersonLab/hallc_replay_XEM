// Macro to generate time-walk histograms for the hodoscopes
// Author: Eric Pooser, pooser@jlab.org
// Changes made by/Jacob Murphy (Ohio U) and Nathan Heinrich (UR) 2021/12/08, including new plotting bounds and fit style
#include <time.h>
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

// Declare replay data file and output file
TFile *replayFile, *outFile;

// Declare data tree
TTree *rawDataTree;

// Declare constants
static const UInt_t nPlanes     = 4;
static const UInt_t nSides      = 2;
static const UInt_t nAdcSignals = 3;
static const UInt_t nTdcSignals = 1;
static const UInt_t nSignals    = nAdcSignals + nTdcSignals;
static const UInt_t maxTdcHits  = 128;
static const UInt_t maxAdcHits  = 4;
static const UInt_t nMaxBars    = 21;

static const TString spec  = "P";
static const TString detec = "hod";
static const TString trig  = "T";
static const TString shms  = "shms";

static const UInt_t  nbars[nPlanes]        = {13, 13, 14, 18};
static const TString planeNames[nPlanes]   = {"1x", "1y", "2x", "2y"};
static const TString sideNames[nSides]     = {"pos", "neg"};
static const TString signalNames[nSignals] = {"Adc", "Tdc"};
static const TString adcData[nAdcSignals]  = {"ErrorFlag", "PulseTimeRaw", "PulseAmp"};
static const TString tdcData[nTdcSignals]  = {"TimeRaw"};

static const Double_t tdcChanToTime   = 0.09766;                    // Units of ns
static const Double_t adcChanToTime   = 0.0625;                   // Units of ns
static const Double_t adcDynamicRange = 1000.0;                   // Units of mV
static const Double_t nAdcChan        = 4096.0;                   // Units of ADC channels
static const Double_t adcChanTomV     = adcDynamicRange/nAdcChan; // Units of mV/ADC Chan

static const Double_t hodoPulseAmpCutLow     = 15.0;   // Units of mV
static const Double_t hodoPulseAmpCutHigh    = 1000.0; // Units of mV
static const Double_t refAdcPulseAmpCutLow   = 40.0;   // Units of mV
static const Double_t refAdcPulseAmpCutHigh  = 70.0;   // Units of mV
static const Double_t refAdcPulseTimeCutLow  = 300.0;  // Units of ns
static const Double_t refAdcPulseTimeCutHigh = 370.0;  // Units of ns
static const Double_t adcTdcTimeDiffCutLow   = 0.0; // Units of ns
static const Double_t adcTdcTimeDiffCutHigh  = 100.0;  // Units of ns
static const Double_t calEtotnormCutVal      = 0.7;    // Units of Normalized energy
static const Double_t cerNpeSumCutVal        = 0.5;    // Units of NPE in aerogel
// static const Double_t adcTdcTimeDiffCutLow   = -6000.0;  // Units of ns
// static const Double_t adcTdcTimeDiffCutHigh  = 1000.0;  // Units of ns

static const bool TimeWalkRangeSet = false; // this will determine if the TDC ADC Time Diff walk is done per plane or set with the adcTdcTimeDiffCutLow/High variables 

static const Double_t fontSize = 0.04;

// Declare variables to obtain from data tree
// Number of ADC & TDC hits
Int_t    adcHits[nPlanes][nSides][nSignals], tdcHits[nPlanes][nSides][nSignals];
// Paddle number which fired
Double_t adcPaddle[nPlanes][nSides][nSignals][nMaxBars*maxAdcHits];
Double_t tdcPaddle[nPlanes][nSides][nSignals][nMaxBars*maxTdcHits];
// FADC data
Double_t hodoAdcPulseTimeRaw[nPlanes][nSides][nSignals][nMaxBars*maxAdcHits];
Double_t hodoAdcPulseAmp[nPlanes][nSides][nSignals][nMaxBars*maxAdcHits];
Double_t hodoAdcErrorFlag[nPlanes][nSides][nSignals][nMaxBars*maxAdcHits];
// TDC Data
Double_t hodoTdcTimeRaw[nPlanes][nSides][nSignals][nMaxBars*maxTdcHits];
// Trigger apparatus data
Double_t refAdcPulseTimeRaw, refAdcPulseAmp, refAdcMultiplicity;
Double_t refT1TdcTimeRaw, refT2TdcTimeRaw, refT3TdcTimeRaw;
// Declare variables to steer data to arrays
TString *adcBaseName, *adcNdataName, *adcPaddleName;
TString *adcErrorFlagName, *adcPulseTimeRawName, *adcPulseAmpName;
TString *tdcBaseName, *tdcNdataName, *tdcPaddleName;
TString *tdcTimeRawName;
// Declare directories for output ROOT file
TDirectory *trigRawDir, *hodoRawDir, *hodoUncalibDir;
TDirectory *planeRawDir[nPlanes], *sideRawDir[nPlanes][nSides], *planeUncalibDir[nPlanes], *sideUncalibDir[nPlanes][nSides];
TDirectory *adcTimeWalkDir[nPlanes][nSides], *tdcTimeWalkDir[nPlanes][nSides], *adcTdcTimeDiffWalkDir[nPlanes][nSides];
// Declare histos
// 1D histos
TH1F *h1_refAdcPulseTimeRaw, *h1_refAdcPulseAmp, *h1_refAdcMultiplicity;
TH1F *h1_refT1TdcTimeRaw, *h1_refT2TdcTimeRaw, *h1_refT3TdcTimeRaw;
// 2D histos
TH2F *h2_adcErrorFlag[nPlanes][nSides];
TH2F *h2_adcPulseTimeRaw[nPlanes][nSides];
TH2F *h2_adcPulseAmp[nPlanes][nSides];
TH2F *h2_tdcTimeRaw[nPlanes][nSides];

TH2F *h2_adcPulseTime[nPlanes][nSides];
TH2F *h2_tdcTime[nPlanes][nSides];
TH2F *h2_adcTdcTimeDiff[nPlanes][nSides];
TH2F *h2_adcPulseAmpCuts[nPlanes][nSides];
TH2F *h2_adcTimeWalk[nPlanes][nSides][nMaxBars];
TH2F *h2_tdcTimeWalk[nPlanes][nSides][nMaxBars];
TH2F *h2_adcTdcTimeDiffWalk[nPlanes][nSides][nMaxBars];

// Declare event data objects
UInt_t nentries, ievent, adcPaddleNum, tdcPaddleNum;

Int_t numAdcHits, numTdcHits;

Double_t adcErrorFlag, adcPulseTimeRaw, adcPulseTime, adcPulseAmp;
Double_t tdcTimeRaw, tdcTime, adcTdcTimeDiff;
Double_t calEtotnorm, cerNpeSum;

Bool_t adcRefMultiplicityCut, adcRefPulseAmpCut, adcRefPulseTimeCut;
Bool_t edtmCut, adcErrorFlagCut, adcAndTdcHitCut;
Bool_t adcPulseAmpCut, adcTdcTimeDiffCut;
Bool_t calEtotnormCut, cerNpeSumCut;

void generatePlots(UInt_t iplane, UInt_t iside, UInt_t ipaddle) {
  // Create trigger aparatus directory
  trigRawDir = dynamic_cast <TDirectory*> (outFile->Get("trigAppRaw"));
  if (!trigRawDir) {trigRawDir = outFile->mkdir("trigAppRaw"); trigRawDir->cd();}
  else outFile->cd("trigAppRaw");
  // FADC reference
  if (!h1_refAdcPulseTimeRaw) h1_refAdcPulseTimeRaw = new TH1F("h1_refAdcPulseTimeRaw", "ROC2 Raw FADC Reference Pulse Time; Raw FADC Pulse Time (ns); Number of Entries / 100 ps", 4000, 0, 400);
  if (!h1_refAdcPulseAmp)     h1_refAdcPulseAmp     = new TH1F("h1_refAdcPulseAmp",     "ROC2 FADC Reference Pulse Amplitude; FADC Pulse Amplitude (mV); Number of Entries / 1 mV", 1000, 0, 1000);
  if (!h1_refAdcMultiplicity) h1_refAdcMultiplicity = new TH1F("h1_refAdcMultiplicity", "ROC2 FADC Reference Multiplicity; Raw FADC Multiplicity; Number of Entries", 5, -0.5, 4.5);
  // TDC reference
  if (!h1_refT1TdcTimeRaw) h1_refT1TdcTimeRaw = new TH1F("h1_refT1TdcTimeRaw", "ROC2 T1 Raw TDC Reference TDC Time (Slot 20, Channel 15); Raw TDC Time (ns); Number of Entries / 100 ps", 6000, 0, 600);
  if (!h1_refT2TdcTimeRaw) h1_refT2TdcTimeRaw = new TH1F("h1_refT2TdcTimeRaw", "ROC2 T2 Raw TDC Reference TDC Time (Slot 19, Channel 31); Raw TDC Time (ns); Number of Entries / 100 ps", 6000, 0, 600);
  if (!h1_refT3TdcTimeRaw) h1_refT3TdcTimeRaw = new TH1F("h1_refT3TdcTimeRaw", "ROC2 T3 Raw TDC Reference TDC Time (Slot 19, Channel 38); Raw TDC Time (ns); Number of Entries / 100 ps", 6000, 0, 600);
  
  // Create plane directory for raw hodoscope quantities
  hodoRawDir = dynamic_cast <TDirectory*> (outFile->Get("hodoRaw"));
  if (!hodoRawDir) {hodoRawDir = outFile->mkdir("hodoRaw"); hodoRawDir->cd();}
  else outFile->cd("hodoRaw");
  planeRawDir[iplane] = dynamic_cast <TDirectory*> (hodoRawDir->Get(planeNames[iplane]));
  if (!planeRawDir[iplane]) {planeRawDir[iplane] = hodoRawDir->mkdir(planeNames[iplane]); planeRawDir[iplane]->cd();}
  else hodoRawDir->cd(planeNames[iplane]);
  // Create side directory
  sideRawDir[iplane][iside] = dynamic_cast <TDirectory*> (planeRawDir[iplane]->Get(sideNames[iside]));
  if (!sideRawDir[iplane][iside]) {sideRawDir[iplane][iside] = planeRawDir[iplane]->mkdir(sideNames[iside]); sideRawDir[iplane][iside]->cd();}
  else outFile->cd("hodoRaw/"+planeNames[iplane]+"/"+sideNames[iside]);
  // Book histos
  if (!h2_adcErrorFlag[iplane][iside])    h2_adcErrorFlag[iplane][iside]    = new TH2F("h2_adcErrorFlag", "FADC Error Flag Plane "+planeNames[iplane]+" Side "+sideNames[iside]+"; PMT Number; FADC Error Flag", nbars[iplane], 0.5, nbars[iplane]+0.5, 2, -0.5, 1.5);
  if (!h2_adcPulseTimeRaw[iplane][iside]) h2_adcPulseTimeRaw[iplane][iside] = new TH2F("h2_adcPulseTimeRaw", "Raw FADC Pulse Time Plane "+planeNames[iplane]+" Side "+sideNames[iside]+"; PMT Number; Raw FADC Pulse Time (ns) / 100 ps", nbars[iplane], 0.5, nbars[iplane]+0.5, 4000, 0, 400);
  if (!h2_adcPulseAmp[iplane][iside])     h2_adcPulseAmp[iplane][iside]     = new TH2F("h2_adcPulseAmp", "FADC Pulse Amplitude Plane "+planeNames[iplane]+" Side "+sideNames[iside]+"; PMT Number; FADC Pulse Amplitude (mV) / 1 mV", nbars[iplane], 0.5, nbars[iplane]+0.5, 1000, 0, 1000);
  if (!h2_tdcTimeRaw[iplane][iside])      h2_tdcTimeRaw[iplane][iside]      = new TH2F("h2_tdcTimeRaw", "Raw TDC Time Plane "+planeNames[iplane]+" Side "+sideNames[iside]+"; PMT Number; Raw TDC Time (ns) / 100 ps", nbars[iplane], 0.5, nbars[iplane]+0.5, 4000, 0, 400);
  
  // Create plane directory for uncalibrated hodoscope quantities
  hodoUncalibDir = dynamic_cast <TDirectory*> (outFile->Get("hodoUncalib"));
  if (!hodoUncalibDir) {hodoUncalibDir = outFile->mkdir("hodoUncalib"); hodoUncalibDir->cd();}
  else outFile->cd("hodoUncalib");
  planeUncalibDir[iplane] = dynamic_cast <TDirectory*> (hodoUncalibDir->Get(planeNames[iplane]));
  if (!planeUncalibDir[iplane]) {planeUncalibDir[iplane] = hodoUncalibDir->mkdir(planeNames[iplane]); planeUncalibDir[iplane]->cd();}
  else hodoUncalibDir->cd(planeNames[iplane]);
  // Create side directory
  sideUncalibDir[iplane][iside] = dynamic_cast <TDirectory*> (planeUncalibDir[iplane]->Get(sideNames[iside]));
  if (!sideUncalibDir[iplane][iside]) {sideUncalibDir[iplane][iside] = planeUncalibDir[iplane]->mkdir(sideNames[iside]); sideUncalibDir[iplane][iside]->cd();}
  else outFile->cd("hodoUncalib/"+planeNames[iplane]+"/"+sideNames[iside]);
  // Book histos
  if (!h2_adcPulseTime[iplane][iside])    h2_adcPulseTime[iplane][iside]    = new TH2F("h2_adcPulseTime", "FADC Pulse Time Plane "+planeNames[iplane]+" Side "+sideNames[iside]+"; PMT Number;  FADC Pulse Time (ns) / 100 ps", nbars[iplane], 0.5, nbars[iplane]+0.5, 8000, -400, 400);
  if (!h2_tdcTime[iplane][iside])         h2_tdcTime[iplane][iside]         = new TH2F("h2_tdcTime", "TDC Time Plane "+planeNames[iplane]+" Side "+sideNames[iside]+"; PMT Number;  TDC Time (ns) / 100 ps", nbars[iplane], 0.5, nbars[iplane]+0.5, 8000, -400, 400);
  if (!h2_adcTdcTimeDiff[iplane][iside])  h2_adcTdcTimeDiff[iplane][iside]  = new TH2F("h2_adcTdcTimeDiff", "TDC-ADC Time Plane "+planeNames[iplane]+" Side "+sideNames[iside]+"; PMT Number;  TDC - ADC Time (ns) / 100 ps", nbars[iplane], 0.5, nbars[iplane]+0.5, 16000, -800, 800);
  if (!h2_adcPulseAmpCuts[iplane][iside]) h2_adcPulseAmpCuts[iplane][iside] = new TH2F("h2_adcPulseAmpCuts", "FADC Pulse Amplitude Cuts Plane "+planeNames[iplane]+" Side "+sideNames[iside]+"; PMT Number; FADC Pulse Amplitude (mV) / 1 mV", nbars[iplane], 0.5, nbars[iplane]+0.5, 1000, 0, 1000);
  // Create ADC time-walk directory
  adcTimeWalkDir[iplane][iside] = dynamic_cast <TDirectory*> (sideUncalibDir[iplane][iside]->Get("adcTimeWalk"));
  if (!adcTimeWalkDir[iplane][iside]) {adcTimeWalkDir[iplane][iside] = sideUncalibDir[iplane][iside]->mkdir("adcTimeWalk"); adcTimeWalkDir[iplane][iside]->cd();}
  else (outFile->cd("hodoUncalib/"+planeNames[iplane]+"/"+sideNames[iside]+"/adcTimeWalk"));
  // Book histos
  if (!h2_adcTimeWalk[iplane][iside][ipaddle]) h2_adcTimeWalk[iplane][iside][ipaddle] = new TH2F(Form("h2_adcTimeWalk_paddle_%d", ipaddle+1), "ADC Time vs. Pulse Amp Plane "+planeNames[iplane]+" Side "+sideNames[iside]+Form(" Paddle %d", ipaddle+1)+"; Pulse Amplitude (mV) / 1 mV;  ADC Time (ns) / 100 ps", 1000, 0, 1000, 4000, -200, 200);
  // Create TDC time-walk directory
  tdcTimeWalkDir[iplane][iside] = dynamic_cast <TDirectory*> (sideUncalibDir[iplane][iside]->Get("tdcTimeWalk"));
  if (!tdcTimeWalkDir[iplane][iside]) {tdcTimeWalkDir[iplane][iside] = sideUncalibDir[iplane][iside]->mkdir("tdcTimeWalk"); tdcTimeWalkDir[iplane][iside]->cd();}
  else (outFile->cd("hodoUncalib/"+planeNames[iplane]+"/"+sideNames[iside]+"/tdcTimeWalk"));
  // Book histos
  if (!h2_tdcTimeWalk[iplane][iside][ipaddle]) h2_tdcTimeWalk[iplane][iside][ipaddle] = new TH2F(Form("h2_tdcTimeWalk_paddle_%d", ipaddle+1), "TDC Time vs. Pulse Amp Plane "+planeNames[iplane]+" Side "+sideNames[iside]+Form(" Paddle %d", ipaddle+1)+"; Pulse Amplitude (mV) / 1 mV;  TDC Time (ns) / 100 ps", 1000, 0, 1000, 4000, -300, 100);
  // Create TDC-ADC time-walk directory
  adcTdcTimeDiffWalkDir[iplane][iside] = dynamic_cast <TDirectory*> (sideUncalibDir[iplane][iside]->Get("adcTdcTimeDiffWalk"));
  if (!adcTdcTimeDiffWalkDir[iplane][iside]) {adcTdcTimeDiffWalkDir[iplane][iside] = sideUncalibDir[iplane][iside]->mkdir("adcTdcTimeDiffWalk"); adcTdcTimeDiffWalkDir[iplane][iside]->cd();}
  else (outFile->cd("hodoUncalib/"+planeNames[iplane]+"/"+sideNames[iside]+"/adcTdcTimeDiffWalk"));
  // Book histos
  if (!h2_adcTdcTimeDiffWalk[iplane][iside][ipaddle] && !TimeWalkRangeSet){ // NH 2021 11 26 changed this so that each plane could have the y-range set seperately
    if (iplane == 0)
      h2_adcTdcTimeDiffWalk[iplane][iside][ipaddle] = new TH2F(Form("h2_adcTdcTimeDiffWalk_paddle_%d", ipaddle+1), "TDC-ADC Time vs. Pulse Amp Plane "+planeNames[iplane]+" Side "+sideNames[iside]+Form(" Paddle %d", ipaddle+1)+"; Pulse Amplitude (mV) / 1 mV;  TDC-ADC Time (ns) / 100 ps", 500, 0, 500, 1500, 15, 35);
    if(iplane == 1)
      h2_adcTdcTimeDiffWalk[iplane][iside][ipaddle] = new TH2F(Form("h2_adcTdcTimeDiffWalk_paddle_%d", ipaddle+1), "TDC-ADC Time vs. Pulse Amp Plane "+planeNames[iplane]+" Side "+sideNames[iside]+Form(" Paddle %d", ipaddle+1)+"; Pulse Amplitude (mV) / 1 mV;  TDC-ADC Time (ns) / 100 ps", 500, 0, 500, 1500, 20, 40);
    if(iplane == 2)
      h2_adcTdcTimeDiffWalk[iplane][iside][ipaddle] = new TH2F(Form("h2_adcTdcTimeDiffWalk_paddle_%d", ipaddle+1), "TDC-ADC Time vs. Pulse Amp Plane "+planeNames[iplane]+" Side "+sideNames[iside]+Form(" Paddle %d", ipaddle+1)+"; Pulse Amplitude (mV) / 1 mV;  TDC-ADC Time (ns) / 100 ps", 500, 0, 500, 1500, 15, 35);
    if(iplane == 3)
      h2_adcTdcTimeDiffWalk[iplane][iside][ipaddle] = new TH2F(Form("h2_adcTdcTimeDiffWalk_paddle_%d", ipaddle+1), "TDC-ADC Time vs. Pulse Amp Plane "+planeNames[iplane]+" Side "+sideNames[iside]+Form(" Paddle %d", ipaddle+1)+"; Pulse Amplitude (mV) / 1 mV;  TDC-ADC Time (ns) / 100 ps", 500, 0, 500, 1500, 33, 53);
  }else{
    if(!h2_adcTdcTimeDiffWalk[iplane][iside][ipaddle] && TimeWalkRangeSet)
      h2_adcTdcTimeDiffWalk[iplane][iside][ipaddle] = new TH2F(Form("h2_adcTdcTimeDiffWalk_paddle_%d", ipaddle+1), "TDC-ADC Time vs. Pulse Amp Plane "+planeNames[iplane]+" Side "+sideNames[iside]+Form(" Paddle %d", ipaddle+1)+"; Pulse Amplitude (mV) / 1 mV;  TDC-ADC Time (ns) / 100 ps", 500, 0, 500, 1500, adcTdcTimeDiffCutLow, adcTdcTimeDiffCutHigh);
  }
} // generatePlots()

void timeWalkHistos(TString inputname, Int_t runNum, string SPEC_flg) {  //SPEC_flg--> "shms" or "coin"

  // Global ROOT settings
  gStyle->SetOptFit();
  gStyle->SetStatFormat(".2f");
  gStyle->SetTitleFontSize(fontSize);
  gStyle->SetLabelSize(fontSize, "XY");
  gStyle->SetTitleSize(fontSize, "XY");
  
  // Initialize the analysis clock
  clock_t t;
  t = clock();

  // Obtain the replay data file and create new output ROOT file
    
  replayFile = new TFile(inputname, "READ");
  // replayFile = new TFile(Form("ROOTfiles/shms_coin_replay_production_%d_-1.root", runNum), "READ");
  TString outFileName = Form("timeWalkHistos_%d.root", runNum ); // SK 13/5/19 - new .root output for each run tested                                                                                             
  outFile    = new TFile(outFileName, "RECREATE");
  // Obtain the tree
  rawDataTree = dynamic_cast <TTree*> (replayFile->Get("T"));


  Double_t phod_1xnhits;
  Double_t phod_1ynhits;
  Double_t phod_2xnhits;
  Double_t phod_2ynhits;

  //C.Y. Dec 21, 2021 | Variables for Good ADC,TDC Hits per plane side (this will be used to require 2-ended PMT hits)
  Double_t phod_1x_posAdcHits;
  Double_t phod_1x_negAdcHits;
  Double_t phod_1x_posTdcHits;
  Double_t phod_1x_negTdcHits;

  Double_t phod_1y_posAdcHits;
  Double_t phod_1y_negAdcHits;
  Double_t phod_1y_posTdcHits;
  Double_t phod_1y_negTdcHits;

  Double_t phod_2x_posAdcHits;
  Double_t phod_2x_negAdcHits;
  Double_t phod_2x_posTdcHits;
  Double_t phod_2x_negTdcHits;

  Double_t phod_2y_posAdcHits;
  Double_t phod_2y_negAdcHits;
  Double_t phod_2y_posTdcHits;
  Double_t phod_2y_negTdcHits;
  
  // Acquire the trigger apparatus data
  rawDataTree->SetBranchAddress(Form("T.%s.pFADC_TREF_ROC2_adcPulseTimeRaw", SPEC_flg.c_str()), &refAdcPulseTimeRaw);
  rawDataTree->SetBranchAddress(Form("T.%s.pFADC_TREF_ROC2_adcPulseAmp", SPEC_flg.c_str()),     &refAdcPulseAmp);
  rawDataTree->SetBranchAddress(Form("T.%s.pFADC_TREF_ROC2_adcMultiplicity", SPEC_flg.c_str()), &refAdcMultiplicity);
  rawDataTree->SetBranchAddress(Form("T.%s.pT1_tdcTimeRaw", SPEC_flg.c_str()), &refT1TdcTimeRaw);
  rawDataTree->SetBranchAddress(Form("T.%s.pT2_tdcTimeRaw", SPEC_flg.c_str()), &refT2TdcTimeRaw);
  rawDataTree->SetBranchAddress(Form("T.%s.pT3_tdcTimeRaw", SPEC_flg.c_str()), &refT3TdcTimeRaw);

  //C.Y. Dec 21. 2021 | Suggestion: These types of cuts should be placed externaly to let the uset
  // select the appropiate cut based on their experiment
  rawDataTree->SetBranchAddress("P.cal.etracknorm", &calEtotnorm);
  rawDataTree->SetBranchAddress("P.hgcer.npeSum", &cerNpeSum);
  //rawDataTree->SetBranchAddress("P.ngcer.npeSum", &cerNpeSum);

  rawDataTree->SetBranchAddress("P.hod.1x.nhits", &phod_1xnhits);
  rawDataTree->SetBranchAddress("P.hod.1y.nhits", &phod_1ynhits);
  rawDataTree->SetBranchAddress("P.hod.2x.nhits", &phod_2xnhits);
  rawDataTree->SetBranchAddress("P.hod.2y.nhits", &phod_2ynhits);

  //C.Y. Dec 21, 2021 | Added variable for total number of good ADC, TDC hits per plane side 
  rawDataTree->SetBranchAddress("P.hod.1x.totNumGoodPosAdcHits", &phod_1x_posAdcHits);
  rawDataTree->SetBranchAddress("P.hod.1x.totNumGoodNegAdcHits", &phod_1x_negAdcHits);
  rawDataTree->SetBranchAddress("P.hod.1x.totNumGoodPosTdcHits", &phod_1x_posTdcHits);
  rawDataTree->SetBranchAddress("P.hod.1x.totNumGoodNegTdcHits", &phod_1x_negTdcHits);

  rawDataTree->SetBranchAddress("P.hod.1y.totNumGoodPosAdcHits", &phod_1y_posAdcHits);
  rawDataTree->SetBranchAddress("P.hod.1y.totNumGoodNegAdcHits", &phod_1y_negAdcHits);
  rawDataTree->SetBranchAddress("P.hod.1y.totNumGoodPosTdcHits", &phod_1y_posTdcHits);
  rawDataTree->SetBranchAddress("P.hod.1y.totNumGoodNegTdcHits", &phod_1y_negTdcHits);

  rawDataTree->SetBranchAddress("P.hod.2x.totNumGoodPosAdcHits", &phod_2x_posAdcHits);
  rawDataTree->SetBranchAddress("P.hod.2x.totNumGoodNegAdcHits", &phod_2x_negAdcHits);
  rawDataTree->SetBranchAddress("P.hod.2x.totNumGoodPosTdcHits", &phod_2x_posTdcHits);
  rawDataTree->SetBranchAddress("P.hod.2x.totNumGoodNegTdcHits", &phod_2x_negTdcHits);

  rawDataTree->SetBranchAddress("P.hod.2y.totNumGoodPosAdcHits", &phod_2y_posAdcHits);
  rawDataTree->SetBranchAddress("P.hod.2y.totNumGoodNegAdcHits", &phod_2y_negAdcHits);
  rawDataTree->SetBranchAddress("P.hod.2y.totNumGoodPosTdcHits", &phod_2y_posTdcHits);
  rawDataTree->SetBranchAddress("P.hod.2y.totNumGoodNegTdcHits", &phod_2y_negTdcHits);

				
// Loop over the planes, sides, signals, leafs, and fill data arrays
  for(UInt_t iplane = 0; iplane < nPlanes; iplane++) {
    

    for(UInt_t iside = 0; iside < nSides; iside++) {
      // Generate directory structure and histograms
      for(UInt_t ipaddle = 0; ipaddle < nbars[iplane]; ipaddle++)
	generatePlots(iplane, iside, ipaddle);	
      for(UInt_t isignal = 0; isignal < nSignals; isignal++) {

	// Acquire the hodoscope ADC data objects
	if(signalNames[isignal] == "Adc") {
	  // Define the leaf strings of interest
	  adcBaseName   = new TString(spec+"."+detec+"."+planeNames[iplane]+"."+
				      sideNames[iside]+signalNames[isignal]);
	  adcNdataName  = new TString("Ndata."+*adcBaseName+"Counter");
	  adcPaddleName = new TString(*adcBaseName+"Counter");
	  // Fill the leafs
	  rawDataTree->SetBranchAddress(*adcNdataName, &adcHits[iplane][iside][isignal]);
	  rawDataTree->SetBranchAddress(*adcPaddleName, &adcPaddle[iplane][iside][isignal]);
	  // Loop over the adc data objects
	  for(UInt_t iadcdata = 0; iadcdata < nAdcSignals; iadcdata++) {
	    if (adcData[iadcdata] == "ErrorFlag") {
	      adcErrorFlagName = new TString(*adcBaseName+"ErrorFlag");
	      rawDataTree->SetBranchAddress(*adcErrorFlagName, &hodoAdcErrorFlag[iplane][iside][isignal]);
	    } // Error flag leaf
	    if (adcData[iadcdata] == "PulseTimeRaw") {
	      adcPulseTimeRawName = new TString(*adcBaseName+"PulseTimeRaw");
	      rawDataTree->SetBranchAddress(*adcPulseTimeRawName, &hodoAdcPulseTimeRaw[iplane][iside][isignal]);
	    } // Raw pulse time leaf
	    if (adcData[iadcdata] == "PulseAmp") {
	      adcPulseAmpName = new TString(*adcBaseName+"PulseAmp");
	      rawDataTree->SetBranchAddress(*adcPulseAmpName, &hodoAdcPulseAmp[iplane][iside][isignal]);
	    } // Pedestal subtracted pulse amplitude leaf
	  } // ADC signal data loop
	} // ADC signal

	  // Acquire the hodoscope TDC data objects
	if(signalNames[isignal] == "Tdc") {
	  // Define the leaf strings of interest
	  tdcBaseName   = new TString(spec+"."+detec+"."+planeNames[iplane]+"."+
				      sideNames[iside]+signalNames[isignal]);
	  tdcNdataName  = new TString("Ndata."+*tdcBaseName+"Counter");
	  tdcPaddleName = new TString(*tdcBaseName+"Counter");
	  // Fill the leafs
	  rawDataTree->SetBranchAddress(*tdcNdataName,  &tdcHits[iplane][iside][isignal]);
	  rawDataTree->SetBranchAddress(*tdcPaddleName, &tdcPaddle[iplane][iside][isignal]);
	  // Loop over the TDC data objects
	  for(UInt_t itdcdata = 0; itdcdata < nTdcSignals; itdcdata++) {
	    if (tdcData[itdcdata] == "TimeRaw") {
	      tdcTimeRawName = new TString(*tdcBaseName+"TimeRaw");
	      rawDataTree->SetBranchAddress(*tdcTimeRawName, &hodoTdcTimeRaw[iplane][iside][isignal]);
	    } // Raw TDC time leaf
	  } // TDC signal data loop
	} // TDC signal	
	
      } // Signal loop
    } // Side loop
  } // Plane loop

  Bool_t good_hits;
  Bool_t good_two_ended_hits_1x;
  Bool_t good_two_ended_hits_1y;
  Bool_t good_two_ended_hits_2x;
  Bool_t good_two_ended_hits_2y;
  
    // Loop over the events and fill histograms
  nentries = rawDataTree->GetEntries();
  
  cout << "\n******************************************"    << endl;
  cout << nentries << " Events Will Be Processed"           << endl;
  cout << "******************************************\n"    << endl;
  for (ievent = 0; ievent < nentries; ievent++) {
    // Acquire the event from the data tree
    rawDataTree->GetEntry(ievent);
  
    good_hits = phod_1xnhits==1&&phod_1ynhits==1&&phod_2xnhits==1&&phod_2ynhits==1;
    
    //C.Y. Dec 21, 2021 | Add 2-ended PMT hit requirement for both ADCs and TDCs
    good_two_ended_hits_1x = phod_1x_posAdcHits==1 && phod_1x_negAdcHits==1 && phod_1x_posTdcHits==1 && phod_1x_negTdcHits==1;
    good_two_ended_hits_1y = phod_1y_posAdcHits==1 && phod_1y_negAdcHits==1 && phod_1y_posTdcHits==1 && phod_1y_negTdcHits==1;
    good_two_ended_hits_2x = phod_2x_posAdcHits==1 && phod_2x_negAdcHits==1 && phod_2x_posTdcHits==1 && phod_2x_negTdcHits==1;
    good_two_ended_hits_2y = phod_2y_posAdcHits==1 && phod_2y_negAdcHits==1 && phod_2y_posTdcHits==1 && phod_2y_negTdcHits==1;
    

    // Fiducial PID cuts
    calEtotnormCut   = (calEtotnorm < calEtotnormCutVal);
    cerNpeSumCut = (cerNpeSum < cerNpeSumCutVal); // JM 31-10-21: Editing cer cuts for good event selection. Added non-zero requirement
    if (calEtotnormCut || cerNpeSumCut) continue;
    
    //if (!good_hits) continue; 
    //if (!(good_two_ended_hits_1x && good_two_ended_hits_1y && good_two_ended_hits_2x && good_two_ended_hits_2y)) continue; //C.Y. Added 2-ended hit requirement
    
    if (!(good_hits && good_two_ended_hits_1x && good_two_ended_hits_1y && good_two_ended_hits_2x && good_two_ended_hits_2y)) continue; //C.Y. Added 2-ended hit requirement         

    //cout << "evt = " << ievent << endl;
    //cout << "phod_1xnhits = " << phod_1xnhits << endl;
    //if(good_two_ended_hits_1x){
    /*
    cout << "evt = " << ievent << endl;                                                                                                      
    cout << "phod_1xnhits = " << phod_1xnhits << endl;  
    cout << "phod_1x_posAdcHits = " << phod_1x_posAdcHits << endl;                     
    cout << "phod_1x_negAdcHits = " << phod_1x_negAdcHits << endl;                                                                       
    cout << "phod_1x_posTdcHits = " << phod_1x_posTdcHits << endl;                                                            
    cout << "phod_1x_negTdcHits = " << phod_1x_negTdcHits << endl;  
    cout << "-------" << endl;
    cout << "" << endl;
    */
    //}
    
    // Fill trigger apparatus histos
    h1_refAdcPulseTimeRaw->Fill(refAdcPulseTimeRaw*adcChanToTime);
    h1_refAdcPulseAmp->Fill(refAdcPulseAmp);
    h1_refAdcMultiplicity->Fill(refAdcMultiplicity);
    h1_refT1TdcTimeRaw->Fill(refT1TdcTimeRaw*tdcChanToTime);
    h1_refT2TdcTimeRaw->Fill(refT2TdcTimeRaw*tdcChanToTime);
    h1_refT3TdcTimeRaw->Fill(refT3TdcTimeRaw*tdcChanToTime);
    // Loop over the planes, sides, signals, data arrays, and fill hodoscope histograms
    for(UInt_t iplane = 0; iplane < nPlanes; iplane++) {
      for(UInt_t iside = 0; iside < nSides; iside++) {
	for(UInt_t isignal = 0; isignal < nSignals; isignal++) {

	  // Acquire the hodoscope ADC data objects
	  if(signalNames[isignal] == "Adc") { 
	    // Loop over the ADC data objects
	    for(UInt_t iadcdata = 0; iadcdata < nAdcSignals; iadcdata++) {
	      numAdcHits = adcHits[iplane][iside][isignal];
	      for (Int_t iadchit = 0; iadchit < numAdcHits; iadchit++) {
		// Obtain variables
		adcPaddleNum    = adcPaddle[iplane][iside][isignal][iadchit];
		adcErrorFlag    = hodoAdcErrorFlag[iplane][iside][isignal][iadchit];
		adcPulseTimeRaw = hodoAdcPulseTimeRaw[iplane][iside][isignal][iadchit]*adcChanToTime;
		adcPulseTime    = adcPulseTimeRaw - refAdcPulseTimeRaw*adcChanToTime;
		adcPulseAmp     = hodoAdcPulseAmp[iplane][iside][isignal][iadchit];
		// Fill histos
		if (adcData[iadcdata] == "ErrorFlag") 
		  h2_adcErrorFlag[iplane][iside]->Fill(adcPaddleNum, adcErrorFlag);
		if (adcData[iadcdata] == "PulseTimeRaw") {
		  h2_adcPulseTimeRaw[iplane][iside]->Fill(adcPaddleNum, adcPulseTimeRaw);
		  h2_adcPulseTime[iplane][iside]->Fill(adcPaddleNum, adcPulseTime);	  
		}
		if (adcData[iadcdata] == "PulseAmp") 
		  h2_adcPulseAmp[iplane][iside]->Fill(adcPaddleNum, adcPulseAmp);
	      } // ADC hit loop
	    } // ADC signal data loop
	  } // ADC signal

	    // Acquire the hodoscope TDC data objects
	  if(signalNames[isignal] == "Tdc") { 
	    // Loop over the TDC data objects
	    for(UInt_t itdcdata = 0; itdcdata < nTdcSignals; itdcdata++) {
	      numTdcHits = tdcHits[iplane][iside][isignal];
	      // Define cuts
	      //edtmCut = (numTdcHits == nbars[0] || numTdcHits == nbars[2] || numTdcHits == nbars[3]);
	      // Implement cuts
	      //if (edtmCut) continue;
	      for (Int_t itdchit = 0; itdchit < numTdcHits; itdchit++) {
		// Obtain variables
		tdcPaddleNum = tdcPaddle[iplane][iside][isignal][itdchit];
		tdcTimeRaw   = hodoTdcTimeRaw[iplane][iside][isignal][itdchit]*tdcChanToTime;
		if (iplane == 3 && iside == 0) tdcTime = tdcTimeRaw - refT2TdcTimeRaw*tdcChanToTime;
		else tdcTime = tdcTimeRaw - refT1TdcTimeRaw*tdcChanToTime;
		if (tdcData[itdcdata] == "TimeRaw") {
		  h2_tdcTimeRaw[iplane][iside]->Fill(tdcPaddleNum, tdcTimeRaw);
		  h2_tdcTime[iplane][iside]->Fill(tdcPaddleNum, tdcTime);
		} // RDC raw time signal
	      } // TDC hit loop
	    } // TDC signal data loop
	  } // TDC signal

	    // Define cuts
	  adcRefMultiplicityCut = (refAdcMultiplicity < 1.0); //cut only zeros
	  adcRefPulseAmpCut     = (refAdcPulseAmp < refAdcPulseAmpCutLow || refAdcPulseAmp > refAdcPulseAmpCutHigh);
	  adcRefPulseTimeCut    = (refAdcPulseTimeRaw*adcChanToTime < refAdcPulseTimeCutLow || refAdcPulseTimeRaw*adcChanToTime > refAdcPulseTimeCutHigh);
	  // Implement cuts
	  if (adcRefMultiplicityCut || adcRefPulseAmpCut || adcRefPulseTimeCut) continue;	  
	  // Acquire the hodoscope ADC data objects
	  if(signalNames[isignal] == "Adc") { 
	    // Loop over the signals again
	    for(UInt_t jsignal = 0; jsignal < nSignals; jsignal++) {
	      // Acquire othe hodoscope TDC data objects
	      if(signalNames[jsignal] == "Tdc") { 
		// Loop over the ADC data objects
		for(UInt_t iadcdata = 0; iadcdata < nAdcSignals; iadcdata++) {
		  if (adcData[iadcdata] != "PulseTimeRaw") continue;
		  // Acquire the number of ADC hits
		  numAdcHits = adcHits[iplane][iside][isignal];
		  for (Int_t iadchit = 0; iadchit < numAdcHits; iadchit++) {
		    // Obtain variables
		    adcPaddleNum    = UInt_t (adcPaddle[iplane][iside][isignal][iadchit]);
		    adcErrorFlag    = hodoAdcErrorFlag[iplane][iside][isignal][iadchit];
		    adcPulseTimeRaw = hodoAdcPulseTimeRaw[iplane][iside][isignal][iadchit]*adcChanToTime;
		    adcPulseTime    = adcPulseTimeRaw - refAdcPulseTimeRaw*adcChanToTime;
		    adcPulseAmp     = hodoAdcPulseAmp[iplane][iside][isignal][iadchit];
		    // Define cuts
		    adcErrorFlagCut = (adcErrorFlag != 0);
		    adcPulseAmpCut  = (adcPulseAmp < hodoPulseAmpCutLow || adcPulseAmp > hodoPulseAmpCutHigh);
		    // Implement cuts
		    if (adcErrorFlagCut || adcPulseAmpCut) continue; 
		    // Loop over the TDC data objects
		    for(UInt_t itdcdata = 0; itdcdata < nTdcSignals; itdcdata++) {
		      if (tdcData[itdcdata] != "TimeRaw") continue;
		      // Acquire the number of TDC hits
		      numTdcHits = tdcHits[iplane][iside][jsignal];
		      // Define cuts
		      //edtmCut = (numTdcHits == nbars[0] || numTdcHits == nbars[2] || numTdcHits == nbars[3]);
		      // Implement cuts
		      //if (edtmCut) continue;
		      for (Int_t itdchit = 0; itdchit < numTdcHits; itdchit++) {
			// Obtain variables
			tdcPaddleNum   = UInt_t (tdcPaddle[iplane][iside][jsignal][itdchit]);
			tdcTimeRaw     = hodoTdcTimeRaw[iplane][iside][jsignal][itdchit]*tdcChanToTime;
			if (iplane == 3 && iside == 0) tdcTime = tdcTimeRaw - refT2TdcTimeRaw*tdcChanToTime;
			else tdcTime = tdcTimeRaw - refT1TdcTimeRaw*tdcChanToTime;
			adcTdcTimeDiff = tdcTime - adcPulseTime;
			// Define cuts
			adcAndTdcHitCut   = (adcPaddleNum != tdcPaddleNum);
			adcTdcTimeDiffCut = (adcTdcTimeDiff < adcTdcTimeDiffCutLow || adcTdcTimeDiff > adcTdcTimeDiffCutHigh);
			// Implement cuts
			if (adcAndTdcHitCut || adcTdcTimeDiffCut) continue;
			h2_adcPulseAmpCuts[iplane][iside]->Fill(tdcPaddleNum, adcPulseAmp);
			h2_adcTdcTimeDiff[iplane][iside]->Fill(tdcPaddleNum, adcTdcTimeDiff);
			h2_adcTimeWalk[iplane][iside][tdcPaddleNum-1]->Fill(adcPulseAmp, adcPulseTime);
			h2_tdcTimeWalk[iplane][iside][tdcPaddleNum-1]->Fill(adcPulseAmp, tdcTime);
			h2_adcTdcTimeDiffWalk[iplane][iside][tdcPaddleNum-1]->Fill(adcPulseAmp, adcTdcTimeDiff);
			h2_adcTdcTimeDiffWalk[iplane][iside][tdcPaddleNum-1]->GetXaxis()->CenterTitle();
			h2_adcTdcTimeDiffWalk[iplane][iside][tdcPaddleNum-1]->GetYaxis()->CenterTitle();
		      } // TDC hit loop
		    } // TDC signal loop
		  } // ADC hit loop
		} // ADC signal loop
	      } // TDC signal
	    } // Nested signal loop
	  } // ADC signal

	} // Signal loop
      } // Side loop
    } // Plane loop
    
    //C.Y. Feb 25, 2022 | print the percentage of events analyzed 
    cout << std::setprecision(2) << double(ievent) / nentries * 100. << "  % " << std::flush << "\r"; 

    if (ievent % 100000 == 0 && ievent != 0)
      cout << ievent << " Events Have Been Processed..." << endl;

  } // rawDataTree event loop

  cout << "\n***************************************" << endl;
  cout << ievent << " Events Were Processed"          << endl;
  cout << "***************************************\n" << endl;

  // Calculate the analysis rate
  t = clock() - t;
  printf ("The Analysis Took %.1f seconds (%.1f min.) \n", ((float) t) / CLOCKS_PER_SEC,  ((float) t) / CLOCKS_PER_SEC/60.) ;
  printf ("The Analysis Event Rate Was %.3f kHz \n", (ievent + 1) / (((float) t) / CLOCKS_PER_SEC*1000.));

  outFile->Write();
  outFile->Close();

  //return 0;

} 
