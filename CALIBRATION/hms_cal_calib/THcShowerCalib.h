#ifndef ROOT_THcShowerCalib
#define ROOT_THcShowerCalib

#include "THcShTrack.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TVectorD.h"
#include "TMatrixD.h"
#include "TDecompLU.h"
#include "TDecompSVD.h"
#include "TMath.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>

#include "TROOT.h"
#include "TFile.h"
#include "TTree.h"

#define D_CALO_FP 338.69    //distance from FP to the calorimeter face
#define D_DPEXIT_FP -147.48    //distance from FP to the dipole exit

//Whole calorimeter
#define XMIN -65.4
#define XMAX  54.6
#define YMIN -30.
#define YMAX  30.

using namespace std;

//
// HMS Shower Counter calibration class.
//

class THcShowerCalib {

 public:
  THcShowerCalib(string, int, int);
  THcShowerCalib();
  ~THcShowerCalib();

  void ReadThresholds();
  void Init();
  bool ReadShRawTrack(THcShTrack &trk, UInt_t ientry);
  void CalcThresholds();
  void ComposeVMs();
  void SolveAlphas();
  void FillHEcal();
  void FillHEcalNoCor();
  void SaveAlphas();
  void SaveRawData();
  void FillCutBranch();
  void FillHitsGains();
  Double_t GetDeltaMin(){return fDeltaMin;};  
  Double_t GetDeltaMax(){return fDeltaMax;};  
  Double_t GetBetaMin(){return fBetaMin;};  
  Double_t GetBetaMax(){return fBetaMax;};  
  Double_t GetCerMin(){return fCerMin;};  
  Double_t GetRatio(){return (Double_t)fNev/(fNstop-fNstart);};  

  TH1F* hEunc;
  TH1F* hEuncSel;
  TH1F* hEcal;
  TH1F* hEcalNoCor;
  TH2F* hDPvsEcal;
  TH2F* hETAvsEPR;
  ////
  TH1F* hCer;
  TH1F* hP;
  TH1F* hDelta;
  TH1F* hBeta;
  TH2F* hClusTrk;
  TH1F* hNtrack;
  TH1F* hNclust;
  TH2F* hCaloPos;
  TH2F* hCaloPosWt;
  TH2F* hCaloPosNorm;
  TH2F* hCaloPosWtU;
  TH2F* hCaloPosNormU;
  TH2F* hExitPos;
  TH2F* pr1;
  TH2F* ta2;
  TH2F* ta3;
  TH2F* ta4;
  TH2F* pr1a;
  TH2F* ta2a;
  TH2F* ta3a;
  TH2F* ta4a;
  TH2F* pmtList;
  TH2F* cMat;
  TH2F* yCalVsEp;
  TH2F* xCalVsEp;
 
  TH1F **hAdc;

 private:

  string fPrefix;

  Double_t fDeltaMin, fDeltaMax;   // Delta range, %.
  Double_t fBetaMin, fBetaMax;     // Beta range
  Double_t fCerMin;                // Threshold Cerenkov signal, p.e.
  UInt_t fMinHitCount;             // Min. number of hits/chan. for calibration
  Double_t fEuncLoLo, fEuncHiHi;   // Range of uncalibrated Edep histogram
  UInt_t fEuncNBin;                // Binning of uncalibrated Edep histogram
  Double_t fEuncGFitLo,fEuncGFitHi;// Gaussian fit range of uncalib. Edep histo.
  Double_t sigmaRange;             // range for events to use for calib (e.g +/- 3*sigma)

  Double_t fLoThr;     // Low and high thresholds on the normalized uncalibrated
  Double_t fHiThr;     // energy deposition.
  UInt_t fNev;         // Number of processed events.

  TTree* fTree;
  UInt_t fNentries;
  UInt_t fNstart;
  UInt_t fNstop;
  Int_t  fNstopRequested;

  // Declaration of leaves types

  // Calorimeter ADC signals.

  Double_t        H_cal_1pr_aneg_p[THcShTrack::fNrows];
  Double_t        H_cal_1pr_apos_p[THcShTrack::fNrows];

  Double_t        H_cal_2ta_aneg_p[THcShTrack::fNrows];
  Double_t        H_cal_2ta_apos_p[THcShTrack::fNrows];

  Double_t        H_cal_3ta_aneg_p[THcShTrack::fNrows];
  Double_t        H_cal_3ta_apos_p[THcShTrack::fNrows];

  Double_t        H_cal_4ta_aneg_p[THcShTrack::fNrows];
  Double_t        H_cal_4ta_apos_p[THcShTrack::fNrows];

  // Track parameters.

  Double_t          H_tr_n;
  Double_t        H_tr_p;
  Double_t        H_tr_x;   //X FP
  Double_t        H_tr_xp;
  Double_t        H_tr_y;   //Y FP
  Double_t        H_tr_yp;
  Double_t        H_dc_insideDipoleExit;

  Double_t        H_tr_tg_dp;

  Double_t        H_cer_npeSum;
  Double_t        H_tr_beta;

  Double_t        H_cal_nclust;

  TBranch* b_H_cal_1pr_aneg_p;
  TBranch* b_H_cal_1pr_apos_p;

  TBranch* b_H_cal_2ta_aneg_p;
  TBranch* b_H_cal_2ta_apos_p;

  TBranch* b_H_cal_3ta_aneg_p;
  TBranch* b_H_cal_3ta_apos_p;

  TBranch* b_H_cal_4ta_aneg_p;
  TBranch* b_H_cal_4ta_apos_p;

  TBranch* b_H_tr_n;
  TBranch* b_H_tr_x;
  TBranch* b_H_tr_y;
  TBranch* b_H_tr_xp;
  TBranch* b_H_tr_yp;
  TBranch* b_H_tr_p;
  TBranch* b_H_dc_insideDipoleExit;

  TBranch* b_H_tr_tg_dp;
 
  TBranch* b_H_cer_npeSum;
  TBranch* b_H_tr_beta;

  TBranch* b_H_cal_nclust;

  // Quantities for calculations of the calibration constants.

  Double_t fe0;
  Double_t fqe[THcShTrack::fNpmts];
  Double_t fq0[THcShTrack::fNpmts];
  Double_t fQ[THcShTrack::fNpmts][THcShTrack::fNpmts];
  Double_t falphaU[THcShTrack::fNpmts];   // 'unconstrained' calib. constants
  Double_t falphaC[THcShTrack::fNpmts];   // the sought calibration constants
  Double_t falpha0[THcShTrack::fNpmts];   // initial gains
  Double_t falpha1[THcShTrack::fNpmts];   // unit gains

  UInt_t fHitCount[THcShTrack::fNpmts];

};

//------------------------------------------------------------------------------

THcShowerCalib::THcShowerCalib() {};

//------------------------------------------------------------------------------

THcShowerCalib::THcShowerCalib(string Prefix, int nstart, int nstop) {
  fPrefix = Prefix;
  fNstart = nstart;
  //  fNstop = nstop;       //defined in Init
  fNstopRequested = nstop;
};

//------------------------------------------------------------------------------

THcShowerCalib::~THcShowerCalib() {
};

//------------------------------------------------------------------------------

void THcShowerCalib::SaveRawData() {

  // Output raw data into file for debug purposes. To be called after
  // calibration constants are determined.

  cout << "SaveRawData: Output raw data into hcal_calib.raw_data." << endl;

  ofstream fout;
  fout.open("hcal_calib.raw_data",ios::out);

  THcShTrack trk;

  for (UInt_t ientry=fNstart; ientry<fNstop; ientry++) {

    if (ReadShRawTrack(trk, ientry)) {
      trk.SetEs(falphaC);
      trk.Print(fout);
    }

  }

  fout.close();

}

//------------------------------------------------------------------------------

void THcShowerCalib::ReadThresholds() {

  //Read in threshold parameters and initial gains.

  fDeltaMin = 0.;
  fDeltaMax = 0.;
  fBetaMin = 0.;
  fBetaMax = 0.;
  fCerMin = 999.;
  fMinHitCount = 999999;

  for (UInt_t iblk=0; iblk<THcShTrack::fNblks; iblk++) {
    falpha0[iblk] = 0.;
  };

  ifstream fin( "input.dat" );

  string line;
  istringstream iss;

  getline(fin, line);  iss.str(line);
  iss >> fDeltaMin >> fDeltaMax;
  getline(fin, line);  iss.str(line);
  iss >> fBetaMin >> fBetaMax;
  getline(fin, line);  iss.str(line);
  iss >> fCerMin;
  getline(fin, line);  iss.str(line);
  iss >> fMinHitCount;
  getline(fin, line);  iss.str(line);
  iss >> fEuncLoLo >> fEuncHiHi;
  getline(fin, line);  iss.str(line);
  iss >> fEuncNBin;
  getline(fin, line);  iss.str(line);
  iss >> fEuncGFitLo >> fEuncGFitHi;
  getline(fin, line);  iss.str(line);
  iss >> sigmaRange;

  getline(fin, line);
  getline(fin, line);
  getline(fin, line);
 
  unsigned iblk = 0;

  for (int side=0; side<2; side++) {
    for (unsigned k=0; k<THcShTrack::fNcols; k++) {
      for (unsigned j=0; j<THcShTrack::fNrows; j++) {

	getline(fin, line, ',');
	//	cout << "line=" << line << endl;
	iss.str(line);
	if (k==0 && j==0) {
	  string name;
	  iss >> name >> falpha0[iblk];
	}
	else {
	  iss >> falpha0[iblk];
	}

	iss.clear();
	iblk++;
      }
    }
  }

  fin.close();

  cout << "=================================================================\n";
  cout << "Thresholds:" << endl;
  cout << "  Delta min, max   = " << fDeltaMin << "  " << fDeltaMax << endl;
  cout << "  Beta min, max    = " << fBetaMin << "  " << fBetaMax << endl;
  cout << "  Gas Cerenkov min = " << fCerMin << endl;
  cout << "  Min. hit count   = " << fMinHitCount << endl;
  cout << "  Uncalibrated histo. range and binning: " << fEuncLoLo << "  "
       << fEuncHiHi << "  " << fEuncNBin << endl;
  cout << "  Uncalibrated histo. fit range (red line): " << fEuncGFitLo << "  "
       << fEuncGFitHi << endl;
  cout << "  Sigma range to select events (green fill): " << sigmaRange << endl;
  cout << endl;

  cout << "Initial gain constants:\n";
  UInt_t j = 0;
  cout << "hcal_pos_gain_cor=";
  for (UInt_t i=0; i<THcShTrack::fNrows; i++)
    cout << fixed << setw(6) << setprecision(2) << falpha0[j++] << ",";
  cout << endl;
  for (Int_t k=0; k<3; k++) {
    cout << "                  ";
    for (UInt_t i=0; i<THcShTrack::fNrows; i++)
      cout << fixed << setw(6) << setprecision(2) << falpha0[j++] << ",";
    cout << endl;
  }
  cout << "hcal_neg_gain_cor=";
  for (UInt_t i=0; i<THcShTrack::fNrows; i++)
    cout << fixed << setw(6) << setprecision(2) << falpha0[j++] << ",";
  cout << endl;
  for (Int_t k=0; k<3; k++) {
    cout << "                  ";
    for (UInt_t i=0; i<THcShTrack::fNrows; i++)
      cout << fixed << setw(6) << setprecision(2) << falpha0[j++] << ",";
    cout << endl;
  }

  cout << "=================================================================\n";

  //  getchar();
}

//------------------------------------------------------------------------------

void THcShowerCalib::Init() {

  //Reset ROOT and connect tree file.

  gROOT->Reset();

    char* fname = Form("ROOTfiles/%s.root",fPrefix.c_str());
  //   char* fname = Form("kaonRoot/%s.root",fPrefix.c_str());
 cout << "THcShowerCalib::Init: Root file name = " << fname << endl;

  TFile *f = new TFile(fname);
  f->GetObject("T",fTree);

  fNentries = fTree->GetEntries();
  cout << "THcShowerCalib::Init: fNentries= " << fNentries << endl;

  fNstopRequested<0 ? fNstop = fNentries :
                      fNstop = TMath::Min(unsigned(fNstopRequested), fNentries);
  cout << "                      fNstop   = " << fNstop << endl;
  //fNstop=50000;
  // Set branch addresses.

  fTree->SetBranchAddress("H.cal.1pr.goodNegAdcPulseInt", H_cal_1pr_aneg_p,
			  &b_H_cal_1pr_aneg_p);
  fTree->SetBranchAddress("H.cal.1pr.goodPosAdcPulseInt", H_cal_1pr_apos_p,
			  &b_H_cal_1pr_apos_p);

  fTree->SetBranchAddress("H.cal.2ta.goodNegAdcPulseInt", H_cal_2ta_aneg_p,
			  &b_H_cal_2ta_aneg_p);
  fTree->SetBranchAddress("H.cal.2ta.goodPosAdcPulseInt", H_cal_2ta_apos_p,
			  &b_H_cal_2ta_apos_p);

  fTree->SetBranchAddress("H.cal.3ta.goodNegAdcPulseInt", H_cal_3ta_aneg_p,
			  &b_H_cal_3ta_aneg_p);
  fTree->SetBranchAddress("H.cal.3ta.goodPosAdcPulseInt", H_cal_3ta_apos_p,
			  &b_H_cal_3ta_apos_p);

  fTree->SetBranchAddress("H.cal.4ta.goodNegAdcPulseInt", H_cal_4ta_aneg_p,
			  &b_H_cal_4ta_aneg_p);
  fTree->SetBranchAddress("H.cal.4ta.goodPosAdcPulseInt", H_cal_4ta_apos_p,
			  &b_H_cal_4ta_apos_p);

  fTree->SetBranchAddress("H.dc.ntrack", &H_tr_n,&b_H_tr_n);

  //wph
  fTree->SetBranchAddress("H.dc.InsideDipoleExit", &H_dc_insideDipoleExit,&b_H_dc_insideDipoleExit);

  fTree->SetBranchAddress("H.dc.x_fp",&H_tr_x,&b_H_tr_x);
  fTree->SetBranchAddress("H.dc.y_fp",&H_tr_y,&b_H_tr_y);
  fTree->SetBranchAddress("H.dc.xp_fp",&H_tr_xp,&b_H_tr_xp);
  fTree->SetBranchAddress("H.dc.yp_fp",&H_tr_yp,&b_H_tr_yp);

  //fTree->SetBranchAddress("H.gtr.x",&H_tr_x,&b_H_tr_x);
  //fTree->SetBranchAddress("H.gtr.y",&H_tr_y,&b_H_tr_y);
  //fTree->SetBranchAddress("H.gtr.th",&H_tr_xp,&b_H_tr_xp);
  //fTree->SetBranchAddress("H.gtr.ph",&H_tr_yp,&b_H_tr_yp);

  fTree->SetBranchAddress("H.gtr.p",&H_tr_p,&b_H_tr_p);

  fTree->SetBranchAddress("H.gtr.dp", &H_tr_tg_dp,&b_H_tr_tg_dp);
 
  fTree->SetBranchAddress("H.cer.npeSum", &H_cer_npeSum,&b_H_cer_npeSum);
  fTree->SetBranchAddress("H.hod.beta", &H_tr_beta,&b_H_tr_beta);

  fTree->SetBranchAddress("H.cal.nclust", &H_cal_nclust,&b_H_cal_nclust);

  // Histogram declarations.

  hEunc = new TH1F("hEunc","Edep/P uncalibrated",fEuncNBin,fEuncLoLo,fEuncHiHi);
  hEcal = new TH1F("hEcal", "Edep/P calibrated", 200, 0.05, 2.);
  hEcalNoCor = new TH1F("hEcalNoCor", "Edep/P calibrated without Y-corr.", 200, 0.05, 2.);
  hCer = new TH1F("hCer","H.cer.npeSum",210, -0.5, 20.5);
  hP = new TH1F("hP","H.gtr.p",100, 0, 12);
  hDelta = new TH1F("hDelta","H.gtr.dp",100, -12, 12);
  hBeta = new TH1F("hBeta","H.hod.beta",100, 0, 2);
  hNtrack = new TH1F("hNtrack","H.dc.ntrack",51, -0.5, 50.5);
  hNclust = new TH1F("hNclust","H.cal.nclust",21, -0.5, 20.5);

  hDPvsEcal = new TH2F("hDPvsEcal", "#DeltaP versus Edep/P ",
		       150,0.,1.5, 250,-12.5,12.5);
  hETAvsEPR = new TH2F("hETAvsEPR", "E_{TA} versus E_{PR}",
		       300,0.,1.5, 300,0.,1.5);
  Double_t padH=(XMAX - XMIN)/13; //Paddle Height

  //100,YMIN-10, YMAX+10, 100, XMIN-10, XMAX+10);
  hCaloPos = new TH2F("hCaloPos", "Tracks Projected to Calorimeter",100,YMIN-10, YMAX+10,54, XMIN-padH/4, XMAX+padH/4);
  hCaloPosWt = new TH2F("hCaloPosWt", "e/p weighted",100,YMIN-10, YMAX+10,54, XMIN-padH/4, XMAX+padH/4);
  hCaloPosNorm = new TH2F("hCaloPosNorm", "Calibrated E/p at calorimeter",100,YMIN-10, YMAX+10,54, XMIN-padH/4, XMAX+padH/4);
  hCaloPosWtU = new TH2F("hCaloPosWtU", "e/p weighted",100,YMIN-10, YMAX+10,54, XMIN-padH/4, XMAX+padH/4);
  hCaloPosNormU = new TH2F("hCaloPosNormU", "Uncalibrated E/p at calorimeter",100,YMIN-10, YMAX+10,54, XMIN-padH/4, XMAX+padH/4);
  hExitPos = new TH2F("hExitPos", "Tracks Projected to Dipole Exit",100,-50, 50, 100, -50, 50);
  hClusTrk = new TH2F("hClusTrk","nTracks vs nClusters",9,-0.5,8.5, 41,-0.5,40.5);

  pr1= new TH2F("pr1","Preshower (Layer1)",2, .5, 2.5, 13, 0.5, 13.5);
  ta2= new TH2F("ta2","Shower (Layer2)"   ,2, .5, 2.5, 13, 0.5, 13.5);
  ta3= new TH2F("ta3","Shower (Layer3)"   ,1, .5, 1.5, 13, 0.5, 13.5);
  ta4= new TH2F("ta4","Shower (Layer4)"   ,1, .5, 1.5, 13, 0.5, 13.5);

  pr1a= new TH2F("pr1a","Preshower (Layer1)",2, .8, 2.8, 13, 0.5, 13.5);
  ta2a= new TH2F("ta2a","Shower (Layer2)"   ,2, .8, 2.8, 13, 0.5, 13.5);
  ta3a= new TH2F("ta3a","Shower (Layer3)"   ,1, .7, 1.7, 13, 0.5, 13.5);
  ta4a= new TH2F("ta4a","Shower (Layer4)"   ,1, .7, 1.7, 13, 0.5, 13.5);

  cMat= new TH2F("cMat","Correlation Matrix",   78,-0.5,77.5,78,-0.5,77.5);

  pmtList= new TH2F("pmtList","Signal vs Block# of pmt_hits_list",   79,-0.5,78.5, 100, 0, 250);
  yCalVsEp = new TH2F("yCalVsEp","Y_calo vs E/p", 150, 0, 1.5, 100,YMIN-10,YMAX+10);
  xCalVsEp = new TH2F("xCalVsEp","X_calo vs E/p", 150, 0, 1.5, 54, XMIN-padH/4, XMAX+padH/4);
  hAdc = new TH1F*[THcShTrack::fNpmts];
  for(UInt_t i=0; i<THcShTrack::fNpmts; i++){hAdc[i]=new TH1F(Form("hADC%d",i),"pulseInts",25,0.1,250);}

  // Initialize qumulative quantities.
  
  for (UInt_t i=0; i<THcShTrack::fNpmts; i++) fHitCount[i] = 0;

  fe0 = 0.;

  for (UInt_t i=0; i<THcShTrack::fNpmts; i++) {
    fqe[i] = 0.;
    fq0[i] = 0.;
    falphaU[i] = 0.;
    falphaC[i] = 0.;
    for (UInt_t j=0; j<THcShTrack::fNpmts; j++) {
      fQ[i][j] = 0.;
    }
  }

  // Unit gains.

  for (UInt_t ipmt=0; ipmt<THcShTrack::fNpmts; ipmt++) {
    falpha1[ipmt] = 1.;
  }

};

//------------------------------------------------------------------------------

void THcShowerCalib::CalcThresholds() {

  // Calculate +/-3 RMS thresholds on the uncalibrated total energy
  // depositions. These thresholds are used mainly to exclude potential
  // hadronic events due to the gas Cherenkov inefficiency.

  // Histogram uncalibrated energy depositions, get mean and RMS from the
  // histogram, establish +/-3 * RMS thresholds.

  Int_t nev = 0;
  THcShTrack trk;

  for (UInt_t ientry=fNstart; ientry<fNstop; ientry++) {

    if (ReadShRawTrack(trk, ientry)) {

      //    trk.Print(cout);
      //    getchar();

      trk.SetEs(falpha0);             //Use initial gain constants here.
      Double_t Enorm = trk.Enorm();

      hEunc->Fill(Enorm);    

      nev++;
      //    cout << "CalcThreshods: nev=" << nev << "  Enorm=" << Enorm << endl;
    }

  };
  //wph modifing the code to fit +/- sigma from peak
  //Double_t max


  //  hEunc->Fit("gaus","0","",0.5, 1.5);
  Double_t maxBin= hEunc->GetMaximumBin();
  Double_t maxValue= hEunc->GetBinCenter(maxBin);
  fEuncGFitLo=fEuncGFitLo+maxValue;
  fEuncGFitHi=fEuncGFitHi+maxValue;
  hEunc->Fit("gaus","","",fEuncGFitLo, fEuncGFitHi);
  hEunc->GetFunction("gaus")->SetLineColor(2);
  hEunc->GetFunction("gaus")->SetLineWidth(1);
  hEunc->GetFunction("gaus")->SetLineStyle(1);
  TF1 *fit = hEunc->GetFunction("gaus");
  Double_t gmean  = fit->GetParameter(1);
  Double_t gsigma = fit->GetParameter(2);
  fLoThr = gmean - sigmaRange*gsigma;
  fHiThr = gmean + sigmaRange*gsigma;
  cout << "CalcThreshods: fLoThr=" << fLoThr << "  fHiThr=" << fHiThr 
       << "  nev=" << nev << endl;

  Int_t nbins = hEunc->GetNbinsX();
  Int_t nlo = hEunc->FindBin(fLoThr);
  Int_t nhi = hEunc->FindBin(fHiThr);

  cout << "CalcThresholds: nlo=" << nlo << "  nhi=" << nhi 
       << "  nbins=" << nbins << endl;

  // Histogram selected wthin the thresholds events.
  
  hEuncSel = (TH1F*)hEunc->Clone("hEuncSel");
  
  for (Int_t i=0; i<nlo; i++) hEuncSel->SetBinContent(i, 0.);
  for (Int_t i=nhi; i<nbins+1; i++) hEuncSel->SetBinContent(i, 0.);

};

//------------------------------------------------------------------------------

bool THcShowerCalib::ReadShRawTrack(THcShTrack &trk, UInt_t ientry) {

  //
  // Set a Shower track event from ntuple ientry.
  //

  fTree->GetEntry(ientry);

  if (ientry%100000 == 0) cout << "   ReadShRawTrack: " << ientry << endl;

  if (H_tr_n != 1) return 0;

  if (H_cal_nclust != 1) return 0;

  bool good_trk =   H_tr_tg_dp > fDeltaMin &&
		    H_tr_tg_dp < fDeltaMax &&
		    H_tr_x + H_tr_xp*D_CALO_FP > XMIN &&
		    H_tr_x + H_tr_xp*D_CALO_FP < XMAX &&
                    H_tr_y + H_tr_yp*D_CALO_FP > YMIN &&
                    H_tr_y + H_tr_yp*D_CALO_FP < YMAX &&
                    H_dc_insideDipoleExit == 1;

  if (!good_trk) return 0;

  bool good_cer = H_cer_npeSum > fCerMin ;
  if(!good_cer) return 0;

  bool good_beta = H_tr_beta > fBetaMin &&
                   H_tr_beta < fBetaMax ;
  if(!good_beta) return 0;

  trk.Reset(H_tr_p, H_tr_tg_dp, H_tr_x+D_CALO_FP*H_tr_xp, H_tr_xp,
	    H_tr_y+D_CALO_FP*H_tr_yp, H_tr_yp);

  for (UInt_t j=0; j<THcShTrack::fNrows; j++) {
    for (UInt_t k=0; k<THcShTrack::fNcols; k++) {

      Double_t adc_pos, adc_neg;

      switch (k) {
      case 0 : 
	adc_pos = H_cal_1pr_apos_p[j];
	adc_neg = H_cal_1pr_aneg_p[j];
	break;
      case 1 : 
	adc_pos = H_cal_2ta_apos_p[j];
	adc_neg = H_cal_2ta_aneg_p[j];
	break;
      case 2 : 
	adc_pos = H_cal_3ta_apos_p[j];
	adc_neg = H_cal_3ta_aneg_p[j];
	break;
      case 3 : 
	adc_pos = H_cal_4ta_apos_p[j];
	adc_neg = H_cal_4ta_aneg_p[j];
	break;
      default:
	cout << "*** ReadShRawTrack: column number k=" << k
	     << " out of range! ***" << endl;
      };

      UInt_t nb = j+1 + k*THcShTrack::fNrows;

      if (k==0 && adc_pos>0. && adc_neg>0.) {
	trk.AddHit(adc_pos, adc_neg, 0., 0., nb);
      } 
      if (k==1 && adc_pos>0. && adc_neg>0.) {
	trk.AddHit(adc_pos, adc_neg, 0., 0., nb);
      }
      if (k==2 && adc_pos>0. && adc_neg==0.) {
	trk.AddHit(adc_pos, adc_neg, 0., 0., nb);
      }
      if (k==3 && adc_pos>0. && adc_neg==0.) {
	trk.AddHit(adc_pos, adc_neg, 0., 0., nb);
      }
    }
  }

  return 1;
}

//------------------------------------------------------------------------------

void THcShowerCalib::ComposeVMs() {

  //
  // Fill in vectors and matrixes for the gain constant calculations.
  //

  fNev = 0;
  THcShTrack trk;

  // Loop over the shower track events in the ntuples.

  for (UInt_t ientry=fNstart; ientry<fNstop; ientry++) {

    if (ReadShRawTrack(trk, ientry)) {

      // Set energy depositions with default gains.
      // Calculate normalized to the track momentum total energy deposition,
      // check it against the thresholds.

      trk.SetEs(falpha0);
      Double_t Enorm = trk.Enorm();
      if (Enorm>fLoThr && Enorm<fHiThr) {

	trk.SetEs(falpha1);   // Set energies with unit gains for now.
	// trk.Print(cout);

	fe0 += trk.GetP();    // Accumulate track momenta.

	vector<pmt_hit> pmt_hit_list;     // Container to save PMT hits

	// Loop over hits.

	for (UInt_t i=0; i<trk.GetNhits(); i++) {

	  THcShHit* hit = trk.GetHit(i);
	  //hit->Print(cout);
	  
	  UInt_t nb = hit->GetBlkNumber();

	  // Fill the qe and q0 vectors (for positive side PMT).

	  fqe[nb-1] += hit->GetEpos() * trk.GetP();
	  fq0[nb-1] += hit->GetEpos();

	  // Save the PMT hit.

	  pmt_hit_list.push_back( pmt_hit{hit->GetEpos(), nb} );

	  fHitCount[nb-1]++;   //Accrue the hit counter.

	  // Do same for the negative side PMTs.

	  if (nb <= THcShTrack::fNnegs) {
	    fqe[THcShTrack::fNblks+nb-1] += hit->GetEneg() * trk.GetP();
	    fq0[THcShTrack::fNblks+nb-1] += hit->GetEneg();

	    pmt_hit_list.push_back(pmt_hit{hit->GetEneg(),
		  THcShTrack::fNblks+nb} );

	    fHitCount[THcShTrack::fNblks+nb-1]++;
	  };

	}      //over hits

	// Fill in the correlation matrix Q by retrieving the PMT hits.
	
	for (vector<pmt_hit>::iterator i=pmt_hit_list.begin();
	     i < pmt_hit_list.end(); i++) {

	  UInt_t ic = (*i).channel;
	  Double_t is = (*i).signal;
	  pmtList->Fill(ic,is);

	  for (vector<pmt_hit>::iterator j=i;
	       j < pmt_hit_list.end(); j++) {

	    UInt_t jc = (*j).channel;
	    Double_t js = (*j).signal;


	    fQ[ic-1][jc-1] += is*js;
	    if (jc != ic) fQ[jc-1][ic-1] += is*js;
	  }
	}

	fNev++;

      };   // if within the thresholds

    };   // success in reading

  };     // over entries

  // Take averages.

  fe0 /= fNev;
  for (UInt_t i=0; i<THcShTrack::fNpmts; i++) {
    fqe[i] /= fNev;
    fq0[i] /= fNev;
  }

  for (UInt_t i=0; i<THcShTrack::fNpmts; i++)
    for (UInt_t j=0; j<THcShTrack::fNpmts; j++)
      fQ[i][j] /= fNev;


  //Histogram correlation matrix
  //  for (UInt_t i=0; i<THcShTrack::fNpmts; i++)
  //    for (UInt_t j=0; j<THcShTrack::fNpmts; j++)
  //      cMat->Fill(j,i,fQ[i][j]);

  // Output vectors and matrixes, for debug purposes.
  /*  
  ofstream q0out;
  q0out.open("q0.deb",ios::out);
  for (UInt_t i=0; i<THcShTrack::fNpmts; i++)
    q0out << fq0[i] << " " << i << endl;
  q0out.close();

  ofstream qeout;
  qeout.open("qe.deb",ios::out);
  for (UInt_t i=0; i<THcShTrack::fNpmts; i++)
    qeout << fqe[i] << " " << i << endl;
  qeout.close();

  ofstream Qout;
  Qout.open("Q.deb",ios::out);
  for (UInt_t i=0; i<THcShTrack::fNpmts; i++)
    for (UInt_t j=0; j<THcShTrack::fNpmts; j++)
      Qout << fQ[i][j] << " " << i << " " << j << endl;
  Qout.close();
  */
};

//------------------------------------------------------------------------------

void THcShowerCalib::SolveAlphas() {

  //
  // Solve for the sought calibration constants, by use of the Root
  // matrix algebra package.
  //

  TMatrixD Q(THcShTrack::fNpmts,THcShTrack::fNpmts);
  TVectorD q0(THcShTrack::fNpmts);
  TVectorD qe(THcShTrack::fNpmts);
  TVectorD au(THcShTrack::fNpmts);
  TVectorD ac(THcShTrack::fNpmts);
  Bool_t ok;

  cout << "Solving Alphas..." << endl;
  cout << endl;

  // Print out hit numbers.

  cout << "Hit counts:" << endl;
  UInt_t j = 0;
  cout << "Positives:";
  for (UInt_t i=0; i<THcShTrack::fNrows; i++)
    cout << setw(6) << fHitCount[j++] << ",";
  cout << endl;
  for (Int_t k=0; k<3; k++) {
    cout << "          ";
    for (UInt_t i=0; i<THcShTrack::fNrows; i++)
      cout << setw(6) << fHitCount[j++] << ",";
    cout << endl;
  }
  cout << "Negatives:";
  for (UInt_t i=0; i<THcShTrack::fNrows; i++)
    cout << setw(6) << fHitCount[j++] << ",";
  cout << endl;
  cout << "          ";
  for (UInt_t i=0; i<THcShTrack::fNrows; i++)
    cout << setw(6) << fHitCount[j++] << ",";
  cout << endl;

  // Initialize the vectors and the matrix of the Root algebra package.

  for (UInt_t i=0; i<THcShTrack::fNpmts; i++) {
    q0[i] = fq0[i];
    qe[i] = fqe[i];
    for (UInt_t k=0; k<THcShTrack::fNpmts; k++) {
      Q[i][k] = fQ[i][k];
    }
  }
  //  Q.Print();//wph
  // Sanity check.

  for (UInt_t i=0; i<THcShTrack::fNpmts; i++) {

    // Check zero hit channels: the vector and matrix elements should be 0.

    if (fHitCount[i] == 0) {

      if (q0[i] != 0. || qe[i] != 0.) {

	cout << "*** Inconsistency in chanel " << i << ": # of hits  "
	     << fHitCount[i] << ", q0=" << q0[i] << ", qe=" << qe[i];

	for (UInt_t k=0; k<THcShTrack::fNpmts; k++) {
	  if (Q[i][k] !=0. || Q[k][i] !=0.)
	    cout << ", Q[" << i << "," << k << "]=" << Q[i][k]
		 << ", Q[" << k << "," << i << "]=" << Q[k][i];
	}

	cout << " ***" << endl;
      }
    }

    // The hit channels: the vector elements should be non zero.

    if ( (fHitCount[i] != 0) && (q0[i] == 0. || qe[i] == 0.) ) {
      cout << "*** Inconsistency in chanel " << i << ": # of hits  "
	   << fHitCount[i] << ", q0=" << q0[i] << ", qe=" << qe[i]
	   << " ***" << endl;
    }

  } //sanity check

  // Low hit number channels: exclude from calculation. Assign all the
  // correspondent elements 0, except self-correlation Q(i,i)=1.

  cout << endl;
  cout << "Channels with hit number less than " << fMinHitCount
       << " will not be calibrated." << endl;
  cout << endl;

  for (UInt_t i=0; i<THcShTrack::fNpmts; i++) {

    if (fHitCount[i] < fMinHitCount) {
      cout << "Channel " << i << ", " << fHitCount[i]
	   << " hits, will not be calibrated." << endl;
      q0[i] = 0.;
      qe[i] = 0.;
      for (UInt_t k=0; k<THcShTrack::fNpmts; k++) {
	Q[i][k] = 0.;
	Q[k][i] = 0.;
      }
      Q[i][i] = 1.;
    }

  }
  //  Q.Print();//wph
  // Declare LU decomposition method for the correlation matrix Q.

    TDecompLU lu(Q);
  //  TDecompSVD lu(Q);
  Double_t d1,d2;
  lu.Det(d1,d2);
  cout << "cond:" << lu.Condition() << endl;
  cout << "det :" << d1*TMath::Power(2.,d2) << endl;
  cout << "tol :" << lu.GetTol() << endl;

  // Solve equation Q x au = qe for the 'unconstrained' calibration (gain)
  // constants au.

  au = lu.Solve(qe,ok);
  cout << "au: ok=" << ok << endl;
  //  au.Print();

  // Find the sought 'constrained' calibration constants next.

  Double_t t1 = fe0 - au * q0;         // temporary variable.
  //  cout << "t1 =" << t1 << endl;

  TVectorD Qiq0(THcShTrack::fNpmts);   // an intermittent result
  Qiq0 = lu.Solve(q0,ok);
  cout << "Qiq0: ok=" << ok << endl;
  //  Qiq0.Print();

  Double_t t2 = q0 * Qiq0;             // another temporary variable
  //  cout << "t2 =" << t2 << endl;

  ac = (t1/t2) *Qiq0 + au;             // the sought gain constants
  //  cout << "ac:" << endl;
  //  ac.Print();

  // Assign the gain arrays.

  for (UInt_t i=0; i<THcShTrack::fNpmts; i++) {
    falphaU[i] = au[i];
    falphaC[i] = ac[i];
  }

}

//------------------------------------------------------------------------------

void THcShowerCalib::FillHEcal() {

  //
  // Fill histogram of the normalized energy deposition, 2-d histogram
  // of momentum deviation versus normalized energy deposition.
  //

  //  ofstream output;
  //  output.open("calibrated.deb",ios::out);

  //  ofstream evFile;
  //  evFile.open("eventNumberCalib.txt",ios::out);

  Int_t nev = 0;

  THcShTrack trk;

  for (UInt_t ientry=fNstart; ientry<fNstop; ientry++) {

    if (ReadShRawTrack(trk, ientry)) {

      //    trk.Print(cout);
      //************wph*************
      Double_t  xCalo= H_tr_x + H_tr_xp*D_CALO_FP ;  //could have done trk.GetX()
      Double_t  yCalo= H_tr_y + H_tr_yp*D_CALO_FP ;
      Double_t  xExit= H_tr_x + H_tr_xp*D_DPEXIT_FP ; //but not here
      Double_t  yExit= H_tr_y + H_tr_yp*D_DPEXIT_FP ;

      hCaloPos->Fill(yCalo,xCalo);
      hExitPos->Fill(yExit,xExit);
      
      
      //       Causes Seg Fault
      //       for(UInt_t i=0; i<THcShTrack::fNpmts;i++)
      //	 {
      
      for(UInt_t i=0; i< THcShTrack::fNrows; i++)
	{
  	 hAdc[i]->Fill(H_cal_1pr_apos_p[i]);
	 hAdc[i+13]->Fill(H_cal_1pr_aneg_p[i]);
	 hAdc[i+26]->Fill(H_cal_2ta_apos_p[i]);
	 hAdc[i+39]->Fill(H_cal_2ta_aneg_p[i]);
	 hAdc[i+52]->Fill(H_cal_3ta_apos_p[i]);
	 hAdc[i+65]->Fill(H_cal_4ta_apos_p[i]);

	}
      
	 //	 }


      //******************************



      trk.SetEs(falphaC);        // use the 'constrained' calibration constants
      Double_t P = trk.GetP();
      Double_t delta = trk.GetDp();
      Double_t Enorm = trk.Enorm();

      //    cout << "CalcThreshods: nev=" << nev << "  Enorm=" << Enorm << endl;
      hEcal->Fill(Enorm);

      hDPvsEcal->Fill(Enorm,delta,1.);
      hCaloPosWt->Fill(yCalo,xCalo,Enorm);
      hCaloPosNorm->Divide(hCaloPosWt,hCaloPos);
      hETAvsEPR->Fill(trk.EPRnorm(), trk.ETAnorm());
      yCalVsEp->Fill(Enorm, trk.GetY());
      xCalVsEp->Fill(Enorm, trk.GetX());

      //Plots with uncalibrated E
      trk.SetEs(falphaU); 
      hCaloPosWtU->Fill(yCalo,xCalo,trk.Enorm());
      hCaloPosNormU->Divide(hCaloPosWtU,hCaloPos);	
      nev++;

      //      output << Enorm*P/1000. << " " << P/1000. << " " << delta << " "
      //     << trk.GetX() << " " << trk.GetY() << endl;

      //      evFile << Enorm << "\t" << nev <<endl;

    }
 
  };

  //  output.close();
  //  evFile.close();

  cout << "FillHEcal: " << nev << " events filled" << endl;
};

//------------------------------------------------------------------------------

void THcShowerCalib::FillHEcalNoCor() {

  // FILL WITHOUT USING Y CORRECTION
  // Fill histogram of the normalized energy deposition
  // of momentum deviation versus normalized energy deposition. 
  //

  Int_t nev = 0;

  THcShTrack trk;

  for (UInt_t ientry=fNstart; ientry<fNstop; ientry++) {

    if (ReadShRawTrack(trk, ientry)) {

      trk.SetEsNoCor(falphaC);        // use the 'constrained' calibration constants
      Double_t P = trk.GetP();
      Double_t delta = trk.GetDp();
      Double_t Enorm = trk.Enorm();

      hEcalNoCor->Fill(Enorm);

      nev++;
    }

  };

  cout << "FillHEcalNoCor: " << nev << " events filled" << endl;
};

//------------------------------------------------------------------------------

void THcShowerCalib::SaveAlphas() {

  //
  // Output the gain constants in a format suitable for inclusion in the
  // hcal.param file to be used in the analysis.
  //

  ofstream output;
  char* fname = Form("pcal.param.%s_%d_%d", fPrefix.c_str(),
		     fNstart, fNstopRequested);
  cout << "SaveAlphas: fname=" << fname << endl;

  output.open(fname,ios::out);

  output << "; Calibration constants for file " << fPrefix << ".root"
	 << ", " << fNev << " events processed" << endl;
  output << endl;
  output <<";"<< "fDeltaMin  fDeltaMax" <<"\t"<< "fBetaMin fBetaMax" <<"\t"<< "fLoThr" <<"\t"<< "fHiThr"; 
  output << endl;
  output <<";"<< fDeltaMin <<"\t"<< fDeltaMax <<"\t"<< fBetaMin <<"\t"<< fBetaMax <<"\t"<< fLoThr <<"\t"<< fHiThr; 
  output << endl;




  UInt_t j = 0;
  output << "hcal_pos_gain_cor=";
  for (UInt_t i=0; i<THcShTrack::fNrows; i++)
    output << fixed << setw(6) << setprecision(2) << falphaC[j++] << ",";
  output << endl;
  for (Int_t k=0; k<3; k++) {
    output << "                  ";
    for (UInt_t i=0; i<THcShTrack::fNrows; i++)
      output << fixed << setw(6) << setprecision(2) << falphaC[j++] << ",";
    output << endl;
  }
  output << "hcal_neg_gain_cor=";
  for (UInt_t i=0; i<THcShTrack::fNrows; i++)
    output << fixed << setw(6) << setprecision(2) << falphaC[j++] << ",";
  output << endl;
  output << "                  ";
  for (UInt_t i=0; i<THcShTrack::fNrows; i++)
    output << fixed << setw(6) << setprecision(2) << falphaC[j++] << ",";
  output << endl;
  for (Int_t k=0; k<2; k++) {
    output << "                  ";
    for (UInt_t i=0; i<THcShTrack::fNrows; i++)
      output << fixed << setw(6) << setprecision(2) << 0. << ",";
    output << endl;
  }

  output.close();
}

//------------------------------------------------------------------------------

void THcShowerCalib::FillCutBranch() {
  cout <<"Filling cut branches..."<<endl;
  Int_t nev=0;
  for (UInt_t ientry=fNstart; ientry<fNstop; ientry++) 
    {
      fTree->GetEntry(ientry);
      hCer->Fill(H_cer_npeSum);
      hP->Fill(H_tr_p);
      hDelta->Fill(H_tr_tg_dp);
      hBeta->Fill(H_tr_beta);
      hNclust->Fill(H_cal_nclust);
      hNtrack->Fill(H_tr_n);
            hClusTrk->Fill(H_cal_nclust,H_tr_n);  
      nev++;
    }
}

void THcShowerCalib::FillHitsGains(){
  cout <<"Filling hitd and gains ..."<<endl;
  for (UInt_t i=0; i<THcShTrack::fNpmts; i++) 
    {
      //      cout << i << "\t" << fHitCount[i] << "\t" << i % 13 <<  endl;
    }

  for (UInt_t i=0; i<THcShTrack::fNpmts; i++) 
    {
      if (i<13         ) pr1->Fill(1,i % 13 + 1, fHitCount[i]);
      if (i>=13 && i<26) ta2->Fill(1,i % 13 + 1, fHitCount[i]);
      if (i>=26 && i<39) ta3->Fill(1,i % 13 + 1, fHitCount[i]);
      if (i>=39 && i<52) ta4->Fill(1,i % 13 + 1, fHitCount[i]);
      if (i>=52 && i<65) pr1->Fill(2,i % 13 + 1, fHitCount[i]);
      if (i>=65        ) ta2->Fill(2,i % 13 + 1, fHitCount[i]);

      if (i<13         ) pr1a->Fill(1,i % 13 + 1, falphaC[i]);
      if (i>=13 && i<26) ta2a->Fill(1,i % 13 + 1, falphaC[i]);
      if (i>=26 && i<39) ta3a->Fill(1,i % 13 + 1, falphaC[i]);
      if (i>=39 && i<52) ta4a->Fill(1,i % 13 + 1, falphaC[i]);
      if (i>=52 && i<65) pr1a->Fill(2,i % 13 + 1, falphaC[i]);
      if (i>=65        ) ta2a->Fill(2,i % 13 + 1, falphaC[i]);
}
}


#endif
