#ifndef ROOT_THcPShowerCalib
#define ROOT_THcPShowerCalib

#include "THcPShTrack.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TVectorD.h"
#include "TMatrixD.h"
#include "TDecompLU.h"
#include "TMath.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>

#include "TROOT.h"
#include "TFile.h"
#include "TTree.h"

#include "TF1.h"

#include <time.h>

#include <vector>

#include <string>

#define D_CALO_FP 292.64
#define D_EXIT_FP -307.

//Whole calorimeter fid. limits
#define XMIN -60.
#define XMAX  60.
#define YMIN -58.
#define YMAX  58.

#define PR_ADC_THR 0
#define SH_ADC_THR 0

using namespace std;

bool CollCut(double xptar , double ytar, double yptar, double delta);

//
// SHMS Calorimeter calibration class.
//

class THcPShowerCalib {

 public:

  THcPShowerCalib(string, int, int);
  THcPShowerCalib();
  ~THcPShowerCalib();

  void ReadThresholds();
  void Init();
  bool ReadShRawTrack(THcPShTrack &trk, UInt_t ientry);
  void CalcThresholds();
  void ComposeVMs();
  void SolveAlphas();
  void FillHEcal();
  void SaveAlphas();
  void SaveRawData();
  void fillHits();
  void howLong(Double_t goal);
  void fillCutBranch();
  Double_t GetDeltaMin(){return fDeltaMin;};  
  Double_t GetDeltaMax(){return fDeltaMax;};  
  Double_t GetBetaMin(){return fBetaMin;};  
  Double_t GetBetaMax(){return fBetaMax;};  
  Double_t GetCerMin(){return fNGCerMin;};  
  Double_t GetRatio(){return (Double_t)fNev/(fNstop-fNstart);};  

  TH1F* hEunc;
  TH1F* hEuncSel;
  TH1F* hEcal;
  TH2F* hDPvsEcal;
  TH2F* hESHvsEPR;
  TH2F* hEcalvxXbj;
  ////
  TH1F* hEPRunc;
  TH2F* hETOTvsEPR;
  TH2F* hETOTvsEPRunc;
  TH2F* hESHvsEPRunc;
  //wph more plots
  TH1F* hCer;
  TH1F* hP;
  TH1F* hDelta;
  TH1F* hBeta;
  TH2F* hClusTrk;
  TH2F* hCaloPos;
  TH2F* hCaloPos2;
  TH2F* hCaloPosWt;
  TH2F* hCaloPosNorm;
  TH2F* hCaloPosWtU;
  TH2F* hCaloPosNormU;
  TH2F* hCaloPosWtPint;
  TH2F* hCaloPosNormPint;
  TH2F*  hExitPos;
  //hits
  TH2F*  hpr;
  TH2F*  hsh;
  //gains
  TH2F*  hpra;
  TH2F*  hsha;
  TH2F*  pmtList;
  TH1F **hAdc;
  TH2F* yCalVsEp;
  TH2F* xCalVsEp;

 private:

  string fPrefix;
  Double_t fLoThr;     // Low and high thresholds on the normalized uncalibrated
  Double_t fHiThr;     // energy deposition.
  UInt_t fNev;         // Number of processed events.

  Double_t fDeltaMin, fDeltaMax;   // Delta range, %.
  Double_t fBetaMin, fBetaMax;     // Beta range
  Double_t fHGCerMin;              // Threshold heavy gas Cerenkov signal, p.e.
  Double_t fNGCerMin;              // Threshold noble gas Cerenkov signal, p.e.
  UInt_t fMinHitCount;             // Min. number of hits/chan. for calibration
  Double_t fEuncLoLo, fEuncHiHi;   // Range of uncalibrated Edep histogram
  UInt_t fEuncNBin;                // Binning of uncalibrated Edep histogram
  Double_t fEuncGFitLo,fEuncGFitHi;// Gaussian fit range of uncalib. Edep histo.
  Double_t sigmaRange;             // range for events to use for calib (e.g +/- 3*sigma)
  Double_t fxCaloMin;              // Threshold in xCalo. For when preshower blocks are turned OFF

  TTree* fTree;
  UInt_t fNentries;
  UInt_t fNstart;
  UInt_t fNstop;
  Int_t  fNstopRequested;

  // Declaration of leaves types

  // Preshower and Shower ADC signals.

  Double_t        P_pr_apos_p[THcPShTrack::fNrows_pr];
  Double_t        P_pr_aneg_p[THcPShTrack::fNrows_pr];
  Double_t        P_sh_a_p[THcPShTrack::fNcols_sh*THcPShTrack::fNrows_sh];

  // Track parameters.

  double          P_tr_n;
  Double_t        P_tr_p;
  Double_t        P_tr_x;   //X FP
  Double_t        P_tr_xp;
  Double_t        P_tr_y;   //Y FP
  Double_t        P_tr_yp;

  Double_t        P_tr_tg_dp;
  Double_t        P_tr_tg_ph;
  Double_t        P_tr_tg_th;
  Double_t        P_tr_tg_y;
  //  Double_t        P_dc_insideDipoleExit;
  Double_t        P_hgcer_npe[4];
  Double_t        P_ngcer_npe[4];
  Double_t        P_ngcer_npeSum;
  Double_t        P_hgcer_npeSum;
  Double_t        P_tr_beta;


  Double_t        P_cal_nclust;          //Preshower
  Double_t        P_cal_ntracks;         //Preshower
  Double_t        P_cal_fly_nclust;      //Shower
  Double_t        P_cal_fly_ntracks;     //Shower

  TBranch* b_P_tr_p;
  TBranch* b_P_pr_apos_p;
  TBranch* b_P_pr_aneg_p;
  TBranch* b_P_sh_a_p;
  TBranch* b_P_tr_n;
  //  TBranch* b_P_dc_insideDipoleExit;
  TBranch* b_P_tr_x;
  TBranch* b_P_tr_y;
  TBranch* b_P_tr_xp;
  TBranch* b_P_tr_yp;
  TBranch* b_P_tr_tg_dp;
  TBranch* b_P_tr_tg_ph;
  TBranch* b_P_tr_tg_th;
  TBranch* b_P_tr_tg_y;
  TBranch* b_P_hgcer_npe;
  TBranch* b_P_ngcer_npe;
  TBranch* b_P_hgcer_npeSum;
  TBranch* b_P_ngcer_npeSum;
  TBranch* b_P_tr_beta;

  TBranch* b_P_cal_nclust;
  TBranch* b_P_cal_ntracks;
  TBranch* b_P_cal_fly_nclust;
  TBranch* b_P_cal_fly_ntracks;

  // Quantities for calculations of the calibration constants.

  Double_t fe0;
  Double_t fqe[THcPShTrack::fNpmts];
  Double_t fq0[THcPShTrack::fNpmts];
  Double_t fQ[THcPShTrack::fNpmts][THcPShTrack::fNpmts];
  Double_t falphaU[THcPShTrack::fNpmts];   // 'unconstrained' calib. constants
  Double_t falphaC[THcPShTrack::fNpmts];   // the sought calibration constants
  Double_t falpha0[THcPShTrack::fNpmts];   // initial gains
  Double_t falpha1[THcPShTrack::fNpmts];   // unit gains

  UInt_t fHitCount[THcPShTrack::fNpmts];

};

//------------------------------------------------------------------------------

THcPShowerCalib::THcPShowerCalib() {};

//------------------------------------------------------------------------------

THcPShowerCalib::THcPShowerCalib(string Prefix, int nstart, int nstop) {
  fPrefix = Prefix;
  fNstart = nstart;
  //  fNstop = nstop;  defined in Init
  fNstopRequested = nstop;
};

//------------------------------------------------------------------------------

THcPShowerCalib::~THcPShowerCalib() {
};

//------------------------------------------------------------------------------

void THcPShowerCalib::SaveRawData() {

  // Output raw data into file for debug purposes. To be called after
  // calibration constants are determined.

  cout << "SaveRawData: Output raw data into Pcal_calib.raw_data." << endl;

  ofstream fout;
  fout.open("Pcal_calib.raw_data",ios::out);

  THcPShTrack trk;

  for (UInt_t ientry=fNstart; ientry<fNstop; ientry++) {

    if (ReadShRawTrack(trk, ientry)) {
      trk.SetEs(falphaC);
      trk.Print(fout);
    }

  }

  fout.close();

}

//------------------------------------------------------------------------------

void THcPShowerCalib::ReadThresholds() {

  //Read in threshold parameters and initial gains.

  fDeltaMin = 0.;
  fDeltaMax = 0.;
  fBetaMin = 0.;
  fBetaMax = 0.;
  fHGCerMin = 999.;
  fNGCerMin = 999.;
  fMinHitCount = 999999;
  fxCaloMin = -999.;

  for (UInt_t ipmt=0; ipmt<THcPShTrack::fNpmts; ipmt++) {
    falpha0[ipmt] = 30.0;
  };

  // ifstream fin( "CALIBRATION/shms_cal_calib/input_xCalCuts.dat" );
  ifstream fin( "CALIBRATION/shms_cal_calib/input.dat" );

  string line;
  istringstream iss;

  getline(fin, line);  iss.str(line);
  iss >> fDeltaMin >> fDeltaMax;
  getline(fin, line);  iss.str(line);
  iss >> fBetaMin >> fBetaMax;
  getline(fin, line);  iss.str(line);
  iss >> fHGCerMin;
  getline(fin, line);  iss.str(line);
  iss >> fNGCerMin;
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
  getline(fin, line);  iss.str(line);
  iss >> fxCaloMin;

  getline(fin, line);
  getline(fin, line);
  getline(fin, line);
 
  unsigned iblk = 0;

  //Preshower
  for (unsigned k=0; k<THcPShTrack::fNcols_pr; k++) {
      for (unsigned j=0; j<THcPShTrack::fNrows_pr; j++) {

	getline(fin, line, ',');
	//	cout << "line=" << line << endl;
	iss.str(line);
	if (j==0) {
	  string name;
	  iss >> name >> name >> falpha0[iblk];
	}
	else {
	  iss >> falpha0[iblk];
	}

	iss.clear();
	iblk++;
      }
  }

  //Shower
  for (unsigned k=0; k<THcPShTrack::fNcols_sh; k++) {
    for (unsigned j=0; j<THcPShTrack::fNrows_sh; j++) {

	getline(fin, line, ',');
	//	cout << "line=" << line << endl;
	iss.str(line);
	if (k==0 && j==0) {
	  string name;
	  iss >> name >> name >> falpha0[iblk];
	}
	else {
	  iss >> falpha0[iblk];
	}

	iss.clear();
	iblk++;
    }
  }

  fin.close();

  cout << "=================================================================\n";
  cout << "Thresholds:" << endl;
  cout << "  Delta min, max   = " << fDeltaMin << "  " << fDeltaMax << endl;
  cout << "  Beta min, max    = " << fBetaMin << "  " << fBetaMax << endl;
  cout << "  Heavy Gas Cerenkov min = " << fHGCerMin << endl;
  cout << "  Noble Gas Cerenkov min = " << fNGCerMin << endl;
  cout << "  Min. hit count   = " << fMinHitCount << endl;
  cout << "  Uncalibrated histo. range and binning: " << fEuncLoLo << "  "
       << fEuncHiHi << "  " << fEuncNBin << endl;
  cout << "  Uncalibrated histo. fit range (red line): " << fEuncGFitLo << "  "
       << fEuncGFitHi << endl;
  cout << "  Sigma range to select events (green fill): " << sigmaRange << endl;
  cout << "  Threshold in xCalo =  " << fxCaloMin << endl;

  cout << endl;

  cout << "Initial gain constants:\n";

  UInt_t j = 0;
  for (UInt_t k=0; k<THcPShTrack::fNcols_pr; k++) {
    k==0 ? cout << "pcal_neg_gain_cor =" : cout << "pcal_pos_gain_cor =";
    for (UInt_t i=0; i<THcPShTrack::fNrows_pr; i++)
      cout << fixed << setw(6) << setprecision(2) << falpha0[j++] << ",";
    cout << endl;
  }
  for (UInt_t k=0; k<THcPShTrack::fNcols_sh; k++) {
    k==0 ? cout << "pcal_arr_gain_cor =" : cout << "                   ";
    for (UInt_t i=0; i<THcPShTrack::fNrows_sh; i++)
      cout << fixed << setw(6) << setprecision(2) << falpha0[j++] << ",";
    cout << endl;
  }

  cout << "=================================================================\n";

  //  getchar();
}

//------------------------------------------------------------------------------

void THcPShowerCalib::Init() {

  //Reset ROOT and connect tree file.

  gROOT->Reset();

  char* fname = Form("ROOTfiles/%s.root",fPrefix.c_str());
  cout << "THcPShowerCalib::Init: Root file name = " << fname << endl;

  TFile *f = new TFile(fname);
  f->GetObject("T",fTree);

  fNentries = fTree->GetEntries();
  cout << "THcPShowerCalib::Init: fNentries= " << fNentries << endl;

  fNstopRequested<0 ? fNstop = fNentries :
                      fNstop = TMath::Min(unsigned(fNstopRequested), fNentries);
  cout << "                       fNstop   = " << fNstop << endl;

  // Set branch addresses.

  fTree->SetBranchAddress("P.cal.pr.goodPosAdcPulseInt", P_pr_apos_p,
			  &b_P_pr_apos_p);
  fTree->SetBranchAddress("P.cal.pr.goodNegAdcPulseInt", P_pr_aneg_p, 
			  &b_P_pr_aneg_p);
  fTree->SetBranchAddress("P.cal.fly.goodAdcPulseInt",  P_sh_a_p,
			  &b_P_sh_a_p);

  fTree->SetBranchAddress("P.dc.ntrack", &P_tr_n,&b_P_tr_n);
  //  fTree->SetBranchAddress("P.dc.InsideDipoleExit", &P_dc_insideDipoleExit,&b_P_dc_insideDipoleExit);
  fTree->SetBranchAddress("P.dc.x_fp", &P_tr_x,&b_P_tr_x);
  fTree->SetBranchAddress("P.dc.y_fp", &P_tr_y,&b_P_tr_y);
  fTree->SetBranchAddress("P.dc.xp_fp",&P_tr_xp,&b_P_tr_xp);
  fTree->SetBranchAddress("P.dc.yp_fp",&P_tr_yp,&b_P_tr_yp);
  fTree->SetBranchAddress("P.gtr.p", &P_tr_p,&b_P_tr_p);

  fTree->SetBranchAddress("P.gtr.dp", &P_tr_tg_dp,&b_P_tr_tg_dp);
  fTree->SetBranchAddress("P.gtr.ph", &P_tr_tg_ph,&b_P_tr_tg_ph);
  fTree->SetBranchAddress("P.gtr.th", &P_tr_tg_th,&b_P_tr_tg_th);
  fTree->SetBranchAddress("P.gtr.y",  &P_tr_tg_y, &b_P_tr_tg_y);
 
  fTree->SetBranchAddress("P.hgcer.npeSum", &P_hgcer_npeSum,&b_P_hgcer_npeSum);
  fTree->SetBranchAddress("P.ngcer.npeSum", &P_ngcer_npeSum,&b_P_ngcer_npeSum);

  fTree->SetBranchAddress("P.hod.beta", &P_tr_beta,&b_P_tr_beta);

  fTree->SetBranchAddress("P.cal.nclust", &P_cal_nclust,&b_P_cal_nclust);
  fTree->SetBranchAddress("P.cal.ntracks", &P_cal_ntracks,&b_P_cal_ntracks);
  fTree->SetBranchAddress("P.cal.fly.nclust", &P_cal_fly_nclust,
			  &b_P_cal_fly_nclust);
  fTree->SetBranchAddress("P.cal.fly.ntracks", &P_cal_fly_ntracks,
			  &b_P_cal_fly_ntracks);

  // Histogram declarations.

  hEunc = new TH1F("hEunc","Edep/P uncalibrated",fEuncNBin,fEuncLoLo,fEuncHiHi);
  hEcal = new TH1F("hEcal", "Edep/P calibrated", 200, 0., 2.);
  hDPvsEcal = new TH2F("hDPvsEcal", "#DeltaP versus Edep/P ",
		       400,0.,2., 440,fDeltaMin-1.,fDeltaMax+1.);
  hESHvsEPR = new TH2F("hESHvsEPR", "E_{SH} versus E_{PR}",
		       300,0.,1.5, 300,0.,1.5);
  hEcalvxXbj = new TH2F("hEcalvxXbj", "Calibrated EvsXbj", 100, 1.1, 3.0, 100, 0.5, 1.5);
  ////
  hEPRunc = new TH1F("hEPRunc", "EPR/P uncalibrated", 500, 0., 0.2);
  hETOTvsEPR = new TH2F("hETOTvsEPR", "E_{TOT} versus E_{PR}",
		       300,0.,1.5, 300,0.,1.5);
  hETOTvsEPRunc = new TH2F("hETOTvsEPRunc","E_{TOT} versus E_{PR} uncalibrated",
			   500,0.,0.2, 500,0.,0.2);
  hESHvsEPRunc = new TH2F("hESHvsEPRunc","E_{SH} versus E_{PR} uncalibrated",
			   500,0.,0.2, 500,0.,0.2);

  hClusTrk = new TH2F("hClusTrk","nTracks vs nClusters",9,-0.5,8.5, 41,-0.5,40.5);
  //wph add plot
  hCer = new TH1F("hCer","P.cer.npeSum",210, -0.5, 50.5);
  hP = new TH1F("hP","P.gtr.p",100, 0, 12);
  hDelta = new TH1F("hDelta","P.gtr.dp",100, -20, 30);
  hBeta = new TH1F("hBeta","P.hod.beta",100, 0, 2);
  hpr= new TH2F("hpr","Hits: Preshower(Layer1)",2, .5, 2.5, 14, 0.5, 14.5);
  hsh= new TH2F("hsh","Hits: Shower (Layer2)",14, 0.5, 14.5, 16, 0.5, 16.5);
  hpra= new TH2F("hpra","Calib. Gains: Preshower (Layer1)",2, .5, 2.5, 14, 0.5, 14.5);
  hsha= new TH2F("hsha","Calib. Gains:Shower (Layer2)",14, 0.5, 14.5, 16, 0.5, 16.5);
  hCaloPos = new TH2F("hCaloPos", "Tracks Projected to Calorimeter",100,YMIN-10, YMAX+10, 100, XMIN-10, XMAX+10);
  hCaloPos2 = new TH2F("hCaloPos2", "Tracks Projected to Calorimeter",100,YMIN-10, YMAX+10, 100, XMIN-10, XMAX+10);
  hCaloPosWt = new TH2F("hCaloPosWt", "Calibrated E/p at calorimeter",100,YMIN-10, YMAX+10,100, XMIN-10, XMAX+10);
  hCaloPosNorm = new TH2F("hCaloPosNorm", "Calibrated E/p at calorimeter",100,YMIN-10, YMAX+10,100, XMIN-10, XMAX+10);
  hCaloPosWtU = new TH2F("hCaloPosWtU", "Uncalibrated E/p at calorimeter",100,YMIN-10, YMAX+10,100, XMIN-10, XMAX+10);
  hCaloPosNormU = new TH2F("hCaloPosNormU", "Uncalibrated E/p at calorimeter",100,YMIN-10, YMAX+10,100, XMIN-10, XMAX+10);
  hCaloPosWtPint = new TH2F("hCaloPosWtPint", "Avg PulseInt per track at calorimeter",100,YMIN-10, YMAX+10,100, XMIN-10, XMAX+10);
  hCaloPosNormPint = new TH2F("hCaloPosNormPint", "Avg PulseInt at calorimeter",100,YMIN-10, YMAX+10,100, XMIN-10, XMAX+10);

  hExitPos = new TH2F("hExitPos", "Tracks Projected to Dipole Exit",100,-50, 50, 100, -50, 50);

  pmtList= new TH2F("pmtList","Signal vs Block# of pmt_hits_list",   253,-0.5,252.5, 100, 0, 250);
  yCalVsEp = new TH2F("yCalVsEp","Y_calo vs E/p", 150, 0, 1.5, 100,YMIN-10,YMAX+10);
  xCalVsEp = new TH2F("xCalVsEp","X_calo vs E/p", 150, 0, 1.5, 54, XMIN-10, XMAX+10);
  hAdc = new TH1F*[THcPShTrack::fNpmts];
  for(UInt_t i=0; i<THcPShTrack::fNpmts; i++){hAdc[i]=new TH1F(Form("hADC%d",i),"pulseInts",25,0.1,250);}


  // Initialize cumulative quantities.
  
  for (UInt_t i=0; i<THcPShTrack::fNpmts; i++) fHitCount[i] = 0;

  fe0 = 0.;

  for (UInt_t i=0; i<THcPShTrack::fNpmts; i++) {
    fqe[i] = 0.;
    fq0[i] = 0.;
    falphaU[i] = 0.;
    falphaC[i] = 0.;
    for (UInt_t j=0; j<THcPShTrack::fNpmts; j++) {
      fQ[i][j] = 0.;
    }
  }

  for (UInt_t ipmt=0; ipmt<THcPShTrack::fNpmts; ipmt++) {
    falpha1[ipmt] = 1.;
  }

};

//------------------------------------------------------------------------------

void THcPShowerCalib::CalcThresholds() {

  // Calculate +/-3 RMS thresholds on the uncalibrated total energy
  // depositions. These thresholds are used mainly to exclude potential
  // hadronic events due to the gas Cherenkov inefficiency.

  // Histogram uncalibrated energy depositions, get mean and RMS from the
  // histogram, establish +/-3 * RMS thresholds.

  //cout<< "THcPShowerCalib::CalcThresholds: FNentries = " << fNentries << endl;
  //  cout << "THcPShowerCalib::CalcThresholds: fNstart = " << fNstart << " "
  //       << "  fNstop = " << fNstop << endl;

  Int_t nev = 0;
  THcPShTrack trk;

  for (UInt_t ientry=fNstart; ientry<fNstop; ientry++) {

    if ( ReadShRawTrack(trk, ientry)) {

      //************wph*************
      Double_t  xCalo= P_tr_x + P_tr_xp*D_CALO_FP ;  //could have done trk.GetX()
      Double_t  yCalo= P_tr_y + P_tr_yp*D_CALO_FP ;
      Double_t  xExit= P_tr_x + P_tr_xp*D_EXIT_FP ; //but not here
      Double_t  yExit= P_tr_y + P_tr_yp*D_EXIT_FP ;

      hCaloPos->Fill(yCalo,xCalo);
      hExitPos->Fill(yExit,xExit);

      for(UInt_t i=0; i< THcPShTrack::fNrows_pr; i++)
	{
	  hAdc[i]->Fill(P_pr_apos_p[i]);
	  hAdc[i+THcPShTrack::fNrows_pr]->Fill(P_pr_aneg_p[i]);
	}

      for(UInt_t i= 0; i< THcPShTrack::fNpmts - 2*THcPShTrack::fNrows_pr; i++)
	{
	  hAdc[i]->Fill(P_sh_a_p[i]);
	}


	trk.SetEs(falpha0);             //Use initial gain constants here.
	Double_t Enorm = trk.Enorm();


	////
	if (Enorm > 0.) {
	  hEunc->Fill(Enorm);
	  hETOTvsEPRunc->Fill(trk.EPRnorm(),Enorm);          ////
	  hESHvsEPRunc->Fill(trk.EPRnorm(),trk.ESHnorm());   ////
	  hEPRunc->Fill(trk.EPRnorm());                      ////
	  nev++;
	}

    //    if (nev%100000 == 0)
    //      cout << "CalcThreshods: nev=" << nev << "  Enorm=" << Enorm << endl;
    }

    if (nev > 800000) break;
  };

  //Find max bin (peak) and fit in range
  //specified in input.dat
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
  fLoThr = gmean - gsigma*sigmaRange;
  fHiThr = gmean + gsigma*sigmaRange;
  cout << "CalcThreshods: fLoThr=" << fLoThr << "  fHiThr=" << fHiThr 
       << "  nev=" << nev << endl;

  Int_t nbins = hEunc->GetNbinsX();
  Int_t nlo = hEunc->FindBin(fLoThr);
  Int_t nhi = hEunc->FindBin(fHiThr);

  cout << "CalcThresholds: nlo=" << nlo << "  nhi=" << nhi 
       << "  nbins=" << nbins << endl;

  // Histogram of selected within the thresholds events.
  
  hEuncSel = (TH1F*)hEunc->Clone("hEuncSel");
  
  for (Int_t i=0; i<nlo; i++) hEuncSel->SetBinContent(i, 0.);
  for (Int_t i=nhi; i<nbins+1; i++) hEuncSel->SetBinContent(i, 0.);

};

//------------------------------------------------------------------------------

bool THcPShowerCalib::ReadShRawTrack(THcPShTrack &trk, UInt_t ientry) {

  //
  // Set a Shower track event from ntuple ientry.
  //

  fTree->GetEntry(ientry);

  if (ientry%100000 == 0) cout << "   ReadShRawTrack: " << ientry << endl;

  // Request single electron track in calorimeter's fid. volume.
  //

  if (P_tr_n != 1) return 0;

  bool good_trk =   P_tr_tg_dp > fDeltaMin &&
                    P_tr_tg_dp < fDeltaMax &&
                    (P_tr_x + P_tr_xp*D_CALO_FP) > fxCaloMin;

  if (!good_trk) return 0;

  good_trk = CollCut(P_tr_tg_th, P_tr_tg_y, P_tr_tg_ph, P_tr_tg_dp);

  if (!good_trk) return 0;

  good_trk =   good_trk && 
		    P_tr_x + P_tr_xp*D_CALO_FP > XMIN &&
		    P_tr_x + P_tr_xp*D_CALO_FP < XMAX &&
                    P_tr_y + P_tr_yp*D_CALO_FP > YMIN &&
                    P_tr_y + P_tr_yp*D_CALO_FP < YMAX ;
    //                    P_dc_insideDipoleExit == 1;

  if (!good_trk) return 0;

  ////
  if (P_cal_nclust !=1) return 0;
  if (P_cal_fly_nclust !=1) return 0;

  ////
  good_trk = P_tr_xp > -0.045+0.0025*P_tr_x;
  if (!good_trk) return 0;

    bool good_ngcer = P_ngcer_npeSum >= fNGCerMin ;
    if(!good_ngcer) return 0;

  bool good_hgcer = P_hgcer_npeSum >= fHGCerMin  ;
  if(!good_hgcer) return 0;

  bool good_beta = P_tr_beta > fBetaMin &&
                   P_tr_beta < fBetaMax ;
  if(!good_beta) return 0;

  // Set track coordinates and slopes at the face of Preshower.

  trk.Reset(P_tr_p, P_tr_tg_dp, P_tr_x+D_CALO_FP*P_tr_xp, P_tr_xp,
  	    P_tr_y+D_CALO_FP*P_tr_yp, P_tr_yp);

  // Set Preshower hits.

  UInt_t nb = 0;

  for (UInt_t k=0; k<THcPShTrack::fNcols_pr; k++) {
    for (UInt_t j=0; j<THcPShTrack::fNrows_pr; j++) {
      nb++;
      Double_t adc;
      switch (k) {
      case 0: adc = P_pr_aneg_p[j]; break;
      case 1: adc = P_pr_apos_p[j]; break;
      default : cout << "*** Wrong PreShower column! ***" << endl;
      }
      if (adc > PR_ADC_THR) trk.AddHit(adc, 0., nb);
    }
  }

  // Set Shower hits.

  for (UInt_t k=0; k<THcPShTrack::fNcols_sh; k++) {
    for (UInt_t j=0; j<THcPShTrack::fNrows_sh; j++) {
      nb++;
      Double_t adc = P_sh_a_p[k*THcPShTrack::fNrows_sh + j];
      if (adc > SH_ADC_THR) {
	trk.AddHit(adc, 0., nb);
      }
    }
  }

  return 1;
}

//------------------------------------------------------------------------------

void THcPShowerCalib::ComposeVMs() {

  //
  // Fill in vectors and matrixes for the gain constant calculations.
  //

  fNev = 0;
  THcPShTrack trk;

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

	  THcPShHit* hit = trk.GetHit(i);
	  // hit->Print(cout);

	  UInt_t nb = hit->GetBlkNumber();

	  // Fill the qe and q0 vectors.

	  fqe[nb-1] += hit->GetEdep() * trk.GetP();
	  fq0[nb-1] += hit->GetEdep();

	  // Save the PMT hit.

	  pmt_hit_list.push_back( pmt_hit{hit->GetEdep(), nb} );

	  fHitCount[nb-1]++;   //Accrue the hit counter.

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

      };   // if within enorm thresholds

    };   // success in reading

  };     // over entries

  // Take averages.

  fe0 /= fNev;
  for (UInt_t i=0; i<THcPShTrack::fNpmts; i++) {
    fqe[i] /= fNev;
    fq0[i] /= fNev;
  }

  for (UInt_t i=0; i<THcPShTrack::fNpmts; i++)
    for (UInt_t j=0; j<THcPShTrack::fNpmts; j++)
      fQ[i][j] /= fNev;

  // Output vectors and matrixes, for debug purposes.
  /*
  ofstream q0out;
  q0out.open("q0.deb",ios::out);
  for (UInt_t i=0; i<THcPShTrack::fNpmts; i++)
    q0out << setprecision(20) << fq0[i] << " " << i << endl;
  q0out.close();

  ofstream qeout;
  qeout.open("qe.deb",ios::out);
  for (UInt_t i=0; i<THcPShTrack::fNpmts; i++)
    qeout << setprecision(20) << fqe[i] << " " << i << endl;
  qeout.close();

  ofstream Qout;
  Qout.open("Q.deb",ios::out);
  for (UInt_t i=0; i<THcPShTrack::fNpmts; i++)
    for (UInt_t j=0; j<THcPShTrack::fNpmts; j++)
      Qout << setprecision(20) << fQ[i][j] << " " << i << " " << j << endl;
  Qout.close();

  ofstream sout;
  sout.open("signal.deb",ios::out);
  for (UInt_t i=0; i<THcPShTrack::fNpmts; i++) {
    double sig_sum = fq0[i] * fNev;
    double sig2_sum = fQ[i][i] * fNev;
    int nhit = fHitCount[i];
    double sig = 0.;
    double err = 0.;
    if (nhit != 0) {
      sig = sig_sum/nhit;
      double rms2 = sig2_sum/nhit - (sig_sum/nhit)*(sig_sum/nhit);
      if (rms2 > 0.) {
	double rms = TMath::Sqrt(rms2);
	err = rms/TMath::Sqrt(double(nhit));
      }
    }

    sout << sig << " " << err << " " << nhit << " " << i << endl;
  }
  sout.close();
  */
};

//------------------------------------------------------------------------------

void THcPShowerCalib::SolveAlphas() {

  //
  // Solve for the sought calibration constants, by use of the Root
  // matrix algebra package.
  //

  TMatrixD Q(THcPShTrack::fNpmts,THcPShTrack::fNpmts);
  TVectorD q0(THcPShTrack::fNpmts);
  TVectorD qe(THcPShTrack::fNpmts);
  TVectorD au(THcPShTrack::fNpmts);
  TVectorD ac(THcPShTrack::fNpmts);
  Bool_t ok;

  cout << "Solving Alphas..." << endl;
  cout << endl;

  // Print out hit numbers.

  cout << "Hit counts:" << endl;
  UInt_t j = 0;
  
  for (UInt_t k=0; k<THcPShTrack::fNcols_pr; k++) {
    k==0 ? cout << "Preshower:" : cout << "        :";
    for (UInt_t i=0; i<THcPShTrack::fNrows_pr; i++)
      cout << setw(6) << fHitCount[j++] << ",";
    cout << endl;
  }

  for (UInt_t k=0; k<THcPShTrack::fNcols_sh; k++) {
    k==0 ? cout << "Shower   :" : cout << "        :";
    for (UInt_t i=0; i<THcPShTrack::fNrows_sh; i++)
      cout << setw(6) << fHitCount[j++] << ",";
    cout << endl;
  }

  // Initialize the vectors and the matrix of the Root algebra package.

  for (UInt_t i=0; i<THcPShTrack::fNpmts; i++) {
    q0[i] = fq0[i];
    qe[i] = fqe[i];
    for (UInt_t k=0; k<THcPShTrack::fNpmts; k++) {
      Q[i][k] = fQ[i][k];
    }
  }

  // Sanity check.

  for (UInt_t i=0; i<THcPShTrack::fNpmts; i++) {

    // Check zero hit channels: the vector and matrix elements should be 0.

    if (fHitCount[i] == 0) {

      if (q0[i] != 0. || qe[i] != 0.) {

	cout << "*** Inconsistency in chanel " << i << ": # of hits  "
	     << fHitCount[i] << ", q0=" << q0[i] << ", qe=" << qe[i];

	for (UInt_t k=0; k<THcPShTrack::fNpmts; k++) {
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

  for (UInt_t i=0; i<THcPShTrack::fNpmts; i++) {

    if (fHitCount[i] < fMinHitCount) {
      cout << "Channel " << i << ", " << fHitCount[i]
	   << " hits, will not be calibrated." << endl;
      q0[i] = 0.;
      qe[i] = 0.;
      for (UInt_t k=0; k<THcPShTrack::fNpmts; k++) {
	Q[i][k] = 0.;
	Q[k][i] = 0.;
      }
      Q[i][i] = 1.;
    }

  }

  // Declare LU decomposition method for the correlation matrix Q.

  TDecompLU lu(Q);
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

  TVectorD Qiq0(THcPShTrack::fNpmts);   // an intermittent result
  Qiq0 = lu.Solve(q0,ok);
  cout << "Qiq0: ok=" << ok << endl;
  //  Qiq0.Print();

  Double_t t2 = q0 * Qiq0;             // another temporary variable
  //  cout << "t2 =" << t2 << endl;

  ac = (t1/t2) *Qiq0 + au;             // the sought gain constants
  // cout << "ac:" << endl;
  //  ac.Print();

  // Assign the gain arrays.

  for (UInt_t i=0; i<THcPShTrack::fNpmts; i++) {
    falphaU[i] = au[i];
    falphaC[i] = ac[i];
  }

}

//------------------------------------------------------------------------------

void THcPShowerCalib::FillHEcal() {

  //
  // Fill histogram of the normalized energy deposition, and 2-d histogram
  // of momentum deviation versus normalized energy deposition.
  // Output event by event energy depositions and momenta for debug purposes.
  //

  //  ofstream output;
  //  output.open("calibrated.deb",ios::out);

  Int_t nev = 0;

  THcPShTrack trk;

  for (UInt_t ientry=fNstart; ientry<fNstop; ientry++) {

    if (ReadShRawTrack(trk, ientry)) {
      //    trk.Print(cout);

      //    trk.Print(cout);
      //************wph*************
      Double_t  xCalo= P_tr_x + P_tr_xp*D_CALO_FP ;  //could have done trk.GetX()
      Double_t  yCalo= P_tr_y + P_tr_yp*D_CALO_FP ; 
      Double_t  xExit= P_tr_x + P_tr_xp*D_EXIT_FP ; //but not here
      Double_t  yExit= P_tr_y + P_tr_yp*D_EXIT_FP ;


      trk.SetEs(falphaC);       // use the 'constrained' calibration constants
      Double_t P = trk.GetP();
      Double_t delta = trk.GetDp();
      Double_t Enorm = trk.Enorm();
      Double_t th = trk.GetXp();
      Double_t ph = trk.GetYp();
      Double_t p_spec = 9.2;
      Double_t th_central = 8.5;
      Double_t Xbj;

      Double_t E = 10.542134056112856; //Beam energy
      Double_t Mp = 0.938;              //Mass of proton
      Double_t nu, Q2, W, Ep, thetaRad;      //energy transfer; momentum transfer; invariant mass; energy of scattered e; radians
      thetaRad = TMath::ACos((cos(th_central*3.1415/180.0) - ph * sin(th_central*3.1415/180.0)) / TMath::Sqrt(1.0 + th * th + ph * ph));
      Ep = p_spec * (1.0 + 0.01*delta); //delta
      Q2 = (TMath::Sin(thetaRad/2.0))*(TMath::Sin(thetaRad/2.0))*4.0*E*Ep;
      nu = E - Ep; //nu
      W = TMath::Sqrt(Mp*Mp + 2.0*Mp*nu - Q2);
      Xbj = Q2/(2.0*Mp*nu);
      //cout << "Xbj: " << Xbj << endl;
      // cout << "delta: " << delta << endl;
      // cout << "th: " << th << endl;
      // cout << "ph: " << ph << endl;

      ////
      if (Enorm>0) {
	hEcal->Fill(Enorm);
  hEcalvxXbj->Fill(Xbj,Enorm);
	hDPvsEcal->Fill(Enorm,delta,1.);
	hCaloPos2->Fill(yCalo,xCalo);
	hCaloPosWt->Fill(yCalo,xCalo,Enorm);
	hCaloPosNorm->Divide(hCaloPosWt,hCaloPos2);	
	hESHvsEPR->Fill(trk.EPRnorm(), trk.ESHnorm());
	hETOTvsEPR->Fill(trk.EPRnorm(), trk.Enorm());      ////
	yCalVsEp->Fill(Enorm, trk.GetY());
	xCalVsEp->Fill(Enorm, trk.GetX());

	//Plots with uncalibrated E
	trk.SetEs(falpha0); 
	hCaloPosWtU->Fill(yCalo,xCalo,trk.Enorm());
	hCaloPosNormU->Divide(hCaloPosWtU,hCaloPos2);	

	//Plots with PulseInt per track
	//Set gain=1 then pulseInt=Enorm*P
	trk.SetEs(falpha1);
	hCaloPosWtPint->Fill(yCalo,xCalo,trk.Enorm()*trk.GetP());
	hCaloPosNormPint->Divide(hCaloPosWtPint,hCaloPos);	
	nev++;
      }


    }

    if (nev > 2000000) break;
  };

  //  output.close();

  cout << "FillHEcal: " << nev << " events filled" << endl;
};

//------------------------------------------------------------------------------

void THcPShowerCalib::SaveAlphas() {

  //
  // Output the gain constants in a format suitable for inclusion in the
  // pcal.param file to be used in the analysis.
  //

  ofstream output;
  cout << "P_tr_p  " << P_tr_p << endl;
  char* fname = Form("goodParam/pcal_8p5Deg9p20GeV.param");
  cout << "SaveAlphas: fname=" << fname << endl;

  output.open(fname,ios::out);

  output << "; Calibration constants for file " << fPrefix 
	 << ", " << fNev << " events processed" << endl;
  output << endl;

  UInt_t j = 0;

  for (UInt_t k=0; k<THcPShTrack::fNcols_pr; k++) {
    k==0 ? output << "pcal_neg_gain_cor =" : output << "pcal_pos_gain_cor =";
    for (UInt_t i=0; i<THcPShTrack::fNrows_pr; i++){
      if(falphaC[j] > -0.01 && falphaC[j] < 0.01){
         output << fixed << setw(6) << setprecision(2) << falpha0[j++] << ",";
      }
      else{
      output << fixed << setw(6) << setprecision(2) << falphaC[j++] << ",";
      }
    }
    output << endl;
  }

  for (UInt_t k=0; k<THcPShTrack::fNcols_sh; k++) {
    k==0 ? output << "pcal_arr_gain_cor =" : output << "                   ";
    for (UInt_t i=0; i<THcPShTrack::fNrows_sh; i++){
      if(falphaC[j] > -0.01 && falphaC[j] < 0.01){
         output << fixed << setw(6) << setprecision(2) << falpha0[j++] << ",";
      }
      else{
      output << fixed << setw(6) << setprecision(2) << falphaC[j++] << ",";
      }
    }
    output << endl;
  }
  
  output.close();
}

//-----------------------------------------------------------------------------

bool CollCut(double xptar, double ytar, double yptar, double delta) {

// SHMS octagonal collimator cut.

  const double FullHeight = 25.;   //cm
  const double FullWidth  = 17.;
  const double MidHeight  = 12.5;
  const double MidWidth   =  8.5;

  //Mark (04/16/18).
  double yc = -0.019*(delta)+yptar*253.-40.*0.00052*(delta)+ytar;

  double xc = xptar*253.;

  if (xc > FullHeight/2.) return false;
  if (xc < -FullHeight/2.) return false;
  if (yc > FullWidth/2.) return false;
  if (yc < -FullWidth/2.) return false;

  double slope = (FullHeight-MidHeight)/(FullWidth-MidWidth);
  if (xc > MidHeight/2. + slope*(yc+FullWidth/2.)) return false;
  if (xc > FullHeight/2. - slope*(yc-MidWidth/2.)) return false;
  if (xc < -FullHeight/2. + slope*(yc-MidWidth/2.)) return false;
  if (xc < -MidHeight/2. - slope*(yc+FullWidth/2.)) return false;

  return true;
}

void THcPShowerCalib::howLong(Double_t goal=20.){
      Double_t max=0;

  for (UInt_t i=0; i<THcPShTrack::fNpmts; i++) 
    {
      Int_t row=0;
      Int_t col=0;
      Double_t ratio=(Double_t)fHitCount[i]/(Double_t)fNentries;
      Double_t val=goal/ratio;
      Double_t maxCol=0;
      Double_t maxRow=0;
      if (i<28) 
	{
	  cout << "Preshower Block #" << i%14 +1 << setw(14) << "You will need " << val <<" triggers" << endl;
	  if(val>max && val < 1E38)max=val;
	}
      if (i>=28 )        
	{
	  row=(i-28)%16+1;
	  col=((i-28)-(i-28)%16)/16+1;
	  cout << "Shower Column#"<<col<<" Row #" << row << setw(14)<<" You will need " << val <<" triggers" << endl;
	  if(val>max && val < 1E38)max=val;
	}
    }
  cout << endl << endl;
 cout << "You need " << max/1000000. << "M triggers to get " << goal << " events per block"<<endl;
  cout << endl << endl;
}


void THcPShowerCalib::fillHits(){

  for (UInt_t i=0; i<THcPShTrack::fNpmts; i++) 
    {
      //      cout << i << "\t" << fHitCount[i] << "\t" << i % 13 <<  endl;
    }

  for (UInt_t i=0; i<THcPShTrack::fNpmts; i++) 
    {
      Int_t row=0;
      Int_t col=0;

      if (i<14         ) 
	{
	  hpr->Fill(1,i % 14 + 1, fHitCount[i]);
	  hpra->Fill(1,i % 14 + 1, falphaC[i]);
	}
      if (i>=14 && i<28) 
	{
	  hpr->Fill(2,i % 14 + 1, fHitCount[i]);
	  hpra->Fill(2,i % 14 + 1, falphaC[i]);
	}
      if (i>=28 )        
	{
	  row=(i-28)%16+1;
	  col=((i-28)-(i-28)%16)/16+1;
	  hsh->Fill(15-col ,row , fHitCount[i]);
	  hsha->Fill(15-col ,row , falphaC[i]);
	  //	  cout <<"i:"<<i<<"\t col:"<<col  << "\t row:"<< row <<endl;
	}
    }
}


void THcPShowerCalib::fillCutBranch() {
  Int_t nev=0;
  for (UInt_t ientry=fNstart; ientry<fNstop; ientry++) 
    {
    fTree->GetEntry(ientry);
    hCer->Fill(P_ngcer_npeSum);
    hP->Fill(P_tr_p);
    hDelta->Fill(P_tr_tg_dp);
    hBeta->Fill(P_tr_beta);
    hClusTrk->Fill(P_cal_nclust,P_tr_n);  
    nev++;
    if (nev > 200000) break; //going to use this number to 
                             //calculate percentages
    }
}
#endif
