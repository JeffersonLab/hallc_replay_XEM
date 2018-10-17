// Macro to generate time-walk histograms for the hodoscopes
// Author: Eric Pooser, pooser@jlab.org

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
static const UInt_t nMaxBars    = 16;

static const TString spec  = "H";
static const TString detec = "hod";
static const TString trig  = "T";
static const TString hms   = "hms";

static const UInt_t  nbars[nPlanes]        = {16, 10, 16, 10};
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

static const Double_t hodoPulseAmpCutLow     = 25.0;   // Units of mV
static const Double_t hodoPulseAmpCutHigh    = 1000.0; // Units of mV
static const Double_t refAdcPulseAmpCutLow   = 50.0;   // Units of mV
static const Double_t refAdcPulseAmpCutHigh  = 60.0;   // Units of mV
static const Double_t refAdcPulseTimeCutLow  = 210.0;  // Units of ns
static const Double_t refAdcPulseTimeCutHigh = 225.0;  // Units of ns
static const Double_t adcTdcTimeDiffCutLow   = -100.0; // Units of ns
static const Double_t adcTdcTimeDiffCutHigh  = 100.0;  // Units of ns
static const Double_t calEtotCutVal          = 0.100;  // Units of GeV
static const Double_t cerNpeSumCutVal        = 1.5;    // Units of NPE
// static const Double_t adcTdcTimeDiffCutLow   = -6000.0;  // Units of ns
// static const Double_t adcTdcTimeDiffCutHigh  = 1000.0;  // Units of ns

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
Double_t refT1TdcTimeRaw, refT2TdcTimeRaw;
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
TH1F *h1_refT1TdcTimeRaw, *h1_refT2TdcTimeRaw;
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
Double_t calEtot, cerNpeSum;

Bool_t adcRefMultiplicityCut, adcRefPulseAmpCut, adcRefPulseTimeCut;
Bool_t edtmCut, adcErrorFlagCut, adcAndTdcHitCut;
Bool_t adcPulseAmpCut, adcTdcTimeDiffCut;
Bool_t calEtotCut, cerNpeSumCut;

void generatePlots(UInt_t iplane, UInt_t iside, UInt_t ipaddle) {

  // Create trigger aparatus directory
  trigRawDir = dynamic_cast <TDirectory*> (outFile->Get("trigAppRaw"));
  if (!trigRawDir) {trigRawDir = outFile->mkdir("trigAppRaw"); trigRawDir->cd();}
  else outFile->cd("trigAppRaw");
  // FADC reference
  if (!h1_refAdcPulseTimeRaw) h1_refAdcPulseTimeRaw = new TH1F("h1_refAdcPulseTimeRaw", "ROC1 Raw FADC Reference Pulse Time; Raw FADC Pulse Time (ns); Number of Entries / 100 ps", 4000, 0, 400);
  if (!h1_refAdcPulseAmp)     h1_refAdcPulseAmp     = new TH1F("h1_refAdcPulseAmp",     "ROC1 FADC Reference Pulse Amplitude; FADC Pulse Amplitude (mV); Number of Entries / 1 mV", 1000, 0, 1000);
  if (!h1_refAdcMultiplicity) h1_refAdcMultiplicity = new TH1F("h1_refAdcMultiplicity", "ROC1 FADC Reference Multiplicity; Raw FADC Multiplicity; Number of Entries", 5, -0.5, 4.5);
  // TDC reference
  if (!h1_refT1TdcTimeRaw) h1_refT1TdcTimeRaw = new TH1F("h1_refT1TdcTimeRaw", "ROC1 T1 Raw TDC Reference TDC Time (Slot 20, Channel 15); Raw TDC Time (ns); Number of Entries / 100 ps", 6000, 0, 600);
  if (!h1_refT2TdcTimeRaw) h1_refT2TdcTimeRaw = new TH1F("h1_refT2TdcTimeRaw", "ROC1 T2 Raw TDC Reference TDC Time (Slot 19, Channel 31); Raw TDC Time (ns); Number of Entries / 100 ps", 6000, 0, 600);
    
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
  if (!h2_adcTdcTimeDiffWalk[iplane][iside][ipaddle]) h2_adcTdcTimeDiffWalk[iplane][iside][ipaddle] = new TH2F(Form("h2_adcTdcTimeDiffWalk_paddle_%d", ipaddle+1), "TDC-ADC Time vs. Pulse Amp Plane "+planeNames[iplane]+" Side "+sideNames[iside]+Form(" Paddle %d", ipaddle+1)+"; Pulse Amplitude (mV) / 1 mV;  TDC-ADC Time (ns) / 100 ps", 1000, 0, 1000, 150, -20, 20);
  
} // generatePlots()

void timeWalkHistos(TString inputname,Int_t runNum, string SPEC_flg) {    //SPEC_flg ---> "hms"  or "coin"

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
  // replayFile = new TFile("ROOTfiles/hms_replay_production_all_1267_-1.root", "READ");
   replayFile = new TFile(inputname, "READ");
  //replayFile = new TFile("/lustre/expphy/volatile/hallc/comm2017/pooser/tw-data/hms_replay_production_all_1267_-1.root", "READ");

  // replayFile = new TFile(Form("ROOTfiles/hms_replay_production_all_%d_-1.root", runNum), "READ");
  // replayFile = new TFile(Form("ROOTfiles/hms_coin_replay_production_%d_-1.root", runNum), "READ");

  outFile    = new TFile("timeWalkHistos.root", "RECREATE");
  // Obtain the tree
  rawDataTree = dynamic_cast <TTree*> (replayFile->Get("T"));
  // Acquire the trigger apparatus data
  rawDataTree->SetBranchAddress(Form("T.%s.hFADC_TREF_ROC1_adcPulseTimeRaw", SPEC_flg.c_str()), &refAdcPulseTimeRaw);
  rawDataTree->SetBranchAddress(Form("T.%s.hFADC_TREF_ROC1_adcPulseAmp", SPEC_flg.c_str()),     &refAdcPulseAmp);
  rawDataTree->SetBranchAddress(Form("T.%s.hFADC_TREF_ROC1_adcMultiplicity", SPEC_flg.c_str()), &refAdcMultiplicity);
  rawDataTree->SetBranchAddress(Form("T.%s.hT1_tdcTimeRaw", SPEC_flg.c_str()), &refT1TdcTimeRaw);
  rawDataTree->SetBranchAddress(Form("T.%s.hT2_tdcTimeRaw", SPEC_flg.c_str()), &refT2TdcTimeRaw);
  rawDataTree->SetBranchAddress("H.cal.etot", &calEtot);
  rawDataTree->SetBranchAddress("H.cer.npeSum", &cerNpeSum);
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

  // Loop over the events and fill histograms
  nentries = rawDataTree->GetEntries();
  //nentries = 1200000;

  // nentries = 100000;
  cout << "\n******************************************"    << endl;
  cout << nentries << " Events Will Be Processed"           << endl;
  cout << "******************************************\n"    << endl;
  for (ievent = 0; ievent < nentries; ievent++) {
    // Acquire the event from the data tree
    rawDataTree->GetEntry(ievent);
    // Fiducial PID cuts
    calEtotCut   = (calEtot   < calEtotCutVal);
    cerNpeSumCut = (cerNpeSum < cerNpeSumCutVal);
    //  calEtotCut =1;
    //cerNpeSumCut =1;
    //if (calEtotCut || cerNpeSumCut) continue;
    // Fill trigger apparatus histos
    h1_refAdcPulseTimeRaw->Fill(refAdcPulseTimeRaw*adcChanToTime);
    h1_refAdcPulseAmp->Fill(refAdcPulseAmp);
    h1_refAdcMultiplicity->Fill(refAdcMultiplicity);
    h1_refT1TdcTimeRaw->Fill(refT1TdcTimeRaw*tdcChanToTime);
    h1_refT2TdcTimeRaw->Fill(refT2TdcTimeRaw*tdcChanToTime);
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
	      // edtmCut = (numTdcHits == nbars[0] || numTdcHits == nbars[2] || numTdcHits == nbars[3]);
	      // Implement cuts
	      // if (edtmCut) continue;
	      for (Int_t itdchit = 0; itdchit < numTdcHits; itdchit++) {
		// Obtain variables
		tdcPaddleNum = tdcPaddle[iplane][iside][isignal][itdchit];
		tdcTimeRaw   = hodoTdcTimeRaw[iplane][iside][isignal][itdchit]*tdcChanToTime;
		tdcTime      = tdcTimeRaw - refT2TdcTimeRaw*tdcChanToTime;
		if (tdcData[itdcdata] == "TimeRaw") {
		  h2_tdcTimeRaw[iplane][iside]->Fill(tdcPaddleNum, tdcTimeRaw);
		  h2_tdcTime[iplane][iside]->Fill(tdcPaddleNum, tdcTime);
		} // RDC raw time signal
	      } // TDC hit loop
	    } // TDC signal data loop
	  } // TDC signal

	  // Define cuts
	  adcRefMultiplicityCut = (refAdcMultiplicity == 1.0);
	  adcRefPulseAmpCut     = (refAdcPulseAmp < refAdcPulseAmpCutLow || refAdcPulseAmp > refAdcPulseAmpCutHigh);
	  adcRefPulseTimeCut    = (refAdcPulseTimeRaw*adcChanToTime < refAdcPulseTimeCutLow || refAdcPulseTimeRaw*adcChanToTime > refAdcPulseTimeCutHigh);
	  // Implement cuts
	  // if (adcRefMultiplicityCut || adcRefPulseAmpCut || adcRefPulseTimeCut) continue;	  
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
			tdcTime        = tdcTimeRaw - refT2TdcTimeRaw*tdcChanToTime;
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

    if (ievent % 100000 == 0 && ievent != 0)
      cout << ievent << " Events Have Been Processed..." << endl;

  } // rawDataTree event loop

  cout << "\n***************************************" << endl;
  cout << ievent << " Events Were Processed"          << endl;
  cout << "***************************************\n" << endl;

  // Calculate the analysis rate
  t = clock() - t;
  printf ("The Analysis Took %.1f seconds \n", ((float) t) / CLOCKS_PER_SEC);
  printf ("The Analysis Event Rate Was %.3f kHz \n", (ievent + 1) / (((float) t) / CLOCKS_PER_SEC*1000.));

  outFile->Write();
  //outFile->Close();

  //return 0;

} // time_walk_calib()
