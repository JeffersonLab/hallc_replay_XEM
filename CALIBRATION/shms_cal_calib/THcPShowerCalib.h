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
#include <iomanip>

#include "TROOT.h"
#include "TFile.h"
#include "TTree.h"

#include <time.h>

#include <vector>

#define D_CALO_FP 275.    //distance from FP to the Preshower

//Whole calorimeter fid. limits
#define XMIN -60.
#define XMAX  60.
#define YMIN -58.
#define YMAX  58.

#define DELTA_MIN -10   //SHMS nominal acceptance
#define DELTA_MAX  22

#define PR_ADC_THR 0
#define SH_ADC_THR 0

#define BETA_MIN 0.5
#define BETA_MAX 1.5

#define HGCER_NPE_MIN 2
//#define NGCER_NPE_MIN 2
#define NGCER_NPE_MIN 0.   //

#define MIN_HIT_COUNT 5   //Minimum number of hits for a PMT to be calibrated.

using namespace std;

//
// SHMS Calorimeter calibration class.
//

class THcPShowerCalib {

 public:

  THcPShowerCalib(string, int, int);
  THcPShowerCalib();
  ~THcPShowerCalib();

  void Init();
  bool ReadShRawTrack(THcPShTrack &trk, UInt_t ientry);
  void CalcThresholds();
  void ComposeVMs();
  void SolveAlphas();
  void FillHEcal();
  void SaveAlphas();
  void SaveRawData();

  TH1F* hEunc;
  TH1F* hEuncSel;
  TH1F* hEcal;
  TH2F* hDPvsEcal;
  TH2F* hESHvsEPR;

 private:

  string fRunNumber;
  Double_t fLoThr;     // Low and high thresholds on the normalized uncalibrated
  Double_t fHiThr;     // energy deposition.
  UInt_t fNev;         // Number of processed events.

  TTree* fTree;
  UInt_t fNentries;
  UInt_t fNstart;
  UInt_t fNstop;

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

  Double_t        P_hgcer_npe[4];
  Double_t        P_ngcer_npe[4];
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
  TBranch* b_P_tr_x;
  TBranch* b_P_tr_y;
  TBranch* b_P_tr_xp;
  TBranch* b_P_tr_yp;
  TBranch* b_P_tr_tg_dp;
  TBranch* b_P_hgcer_npe;
  TBranch* b_P_ngcer_npe;
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

THcPShowerCalib::THcPShowerCalib(string RunNumber, int nstart, int nstop) {
  fRunNumber = RunNumber;
  fNstart = nstart;
  fNstop = nstop;
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

  //  for (UInt_t ientry=0; ientry<fNentries; ientry++) {
  for (UInt_t ientry=TMath::Max(UInt_t(0),fNstart);
       ientry<TMath::Min(fNstop,fNentries); ientry++) {

    if (ReadShRawTrack(trk, ientry)) {
      trk.SetEs(falphaC);
      trk.Print(fout);
    }

  }

  fout.close();

}

//------------------------------------------------------------------------------

void THcPShowerCalib::Init() {

  //Reset ROOT and connect tree file.

  gROOT->Reset();

  char* fname = Form("ROOTfiles/shms_replay_%s.root",fRunNumber.c_str());
  cout << "THcPShowerCalib::Init: Root file name = " << fname << endl;

  TFile *f = new TFile(fname);
  f->GetObject("T",fTree);

  fNentries = fTree->GetEntries();
  cout << "THcPShowerCalib::Init: fNentries= " << fNentries << endl;

  fTree->SetBranchAddress("P.cal.pr.goodPosAdcPulseInt", P_pr_apos_p,
			  &b_P_pr_apos_p);
  fTree->SetBranchAddress("P.cal.pr.goodNegAdcPulseInt", P_pr_aneg_p, 
			  &b_P_pr_aneg_p);
  fTree->SetBranchAddress("P.cal.fly.goodAdcPulseInt",  P_sh_a_p,
			  &b_P_sh_a_p);

  fTree->SetBranchAddress("P.tr.n", &P_tr_n,&b_P_tr_n);
  fTree->SetBranchAddress("P.tr.x", &P_tr_x,&b_P_tr_x);
  fTree->SetBranchAddress("P.tr.y", &P_tr_y,&b_P_tr_y);
  fTree->SetBranchAddress("P.tr.th",&P_tr_xp,&b_P_tr_xp);
  fTree->SetBranchAddress("P.tr.ph",&P_tr_yp,&b_P_tr_yp);
  fTree->SetBranchAddress("P.tr.p", &P_tr_p,&b_P_tr_p);

  fTree->SetBranchAddress("P.tr.tg_dp", &P_tr_tg_dp,&b_P_tr_tg_dp);
 
  fTree->SetBranchAddress("P.hgcer.npe", P_hgcer_npe,&b_P_hgcer_npe);
  fTree->SetBranchAddress("P.ngcer.npe", P_ngcer_npe,&b_P_ngcer_npe);

  fTree->SetBranchAddress("P.tr.beta", &P_tr_beta,&b_P_tr_beta);

  fTree->SetBranchAddress("P.cal.nclust", &P_cal_nclust,&b_P_cal_nclust);
  fTree->SetBranchAddress("P.cal.ntracks", &P_cal_ntracks,&b_P_cal_ntracks);
  fTree->SetBranchAddress("P.cal.fly.nclust", &P_cal_fly_nclust,
			  &b_P_cal_fly_nclust);
  fTree->SetBranchAddress("P.cal.fly.ntracks", &P_cal_fly_ntracks,
			  &b_P_cal_fly_ntracks);

  // Histogram declarations.

  hEunc = new TH1F("hEunc", "Edep/P uncalibrated", 500, 0., 0.2);
  hEcal = new TH1F("hEcal", "Edep/P calibrated", 200, 0., 2.);
  hDPvsEcal = new TH2F("hDPvsEcal", "#DeltaP versus Edep/P ",
		       400,0.,2., 440,DELTA_MIN-1.,DELTA_MAX+1.);
  hESHvsEPR = new TH2F("hESHvsEPR", "E_{SH} versus E_{PR}",
		       300,0.,1.5, 300,0.,1.5);

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

  // Initial gains, 1 for all.

  for (UInt_t ipmt=0; ipmt<THcPShTrack::fNpmts; ipmt++) {
    falpha0[ipmt] = 1.;
  };

  // Unit gains.

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
  cout << "THcPShowerCalib::CalcThresholds: fNstart = " << fNstart << " "
       << "  fNstop = " << fNstop << endl;

  Int_t nev = 0;
  THcPShTrack trk;

  //  for (UInt_t ientry=0; ientry<fNentries; ientry++) {
  for (UInt_t ientry=TMath::Max(UInt_t(0),fNstart);
       ientry<TMath::Min(fNstop,fNentries); ientry++) {

    if ( ReadShRawTrack(trk, ientry)) {

	//    trk.Print(cout);
	//    getchar();

	trk.SetEs(falpha0);             //Use initial gain constants here.
	Double_t Enorm = trk.Enorm();

	hEunc->Fill(Enorm);

	nev++;
    //    if (nev%100000 == 0)
    //      cout << "CalcThreshods: nev=" << nev << "  Enorm=" << Enorm << endl;
    }

  };

  Double_t mean = hEunc->GetMean();
  Double_t rms = hEunc->GetRMS();
  cout << "CalcThreshods: mean=" << mean << "  rms=" << rms << endl;

  fLoThr = mean - 3.*rms;
  fHiThr = mean + 3.*rms;

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

  //  if (P_cal_nclust != 1) return 0;
  //  if (P_cal_fly_nclust != 1) return 0;

  bool good_trk =   P_tr_tg_dp > DELTA_MIN &&
		    P_tr_tg_dp < DELTA_MAX &&
		    P_tr_x + P_tr_xp*D_CALO_FP > XMIN &&
		    P_tr_x + P_tr_xp*D_CALO_FP < XMAX &&
                    P_tr_y + P_tr_yp*D_CALO_FP > YMIN &&
		    P_tr_y + P_tr_yp*D_CALO_FP < YMAX ;
  if (!good_trk) return 0;

  bool good_ngcer = P_ngcer_npe[0] > NGCER_NPE_MIN ||
		    P_ngcer_npe[1] > NGCER_NPE_MIN ||
		    P_ngcer_npe[2] > NGCER_NPE_MIN ||
		    P_ngcer_npe[3] > NGCER_NPE_MIN  ;
  if(!good_ngcer) return 0;

  bool good_hgcer = P_hgcer_npe[0] > HGCER_NPE_MIN ||
		    P_hgcer_npe[1] > HGCER_NPE_MIN ||
		    P_hgcer_npe[2] > HGCER_NPE_MIN ||
		    P_hgcer_npe[3] > HGCER_NPE_MIN  ;
  if(!good_hgcer) return 0;

  bool good_beta = P_tr_beta > BETA_MIN &&
                   P_tr_beta < BETA_MAX ;
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

  //  for (UInt_t ientry=0; ientry<fNentries; ientry++) {
  for (UInt_t ientry=TMath::Max(UInt_t(0),fNstart);
       ientry<TMath::Min(fNstop,fNentries); ientry++) {

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
  cout << "Channels with hit number less than " << MIN_HIT_COUNT 
       << " will not be calibrated." << endl;
  cout << endl;

  for (UInt_t i=0; i<THcPShTrack::fNpmts; i++) {

    if (fHitCount[i] < MIN_HIT_COUNT) {
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

  //  for (UInt_t ientry=0; ientry<fNentries; ientry++) {
  for (UInt_t ientry=TMath::Max(UInt_t(0),fNstart);
       ientry<TMath::Min(fNstop,fNentries); ientry++) {

    if (ReadShRawTrack(trk, ientry)) {
      //    trk.Print(cout);

      trk.SetEs(falphaC);       // use the 'constrained' calibration constants
      Double_t P = trk.GetP();
      Double_t delta = trk.GetDp();
      Double_t Enorm = trk.Enorm();

      hEcal->Fill(Enorm);

      hDPvsEcal->Fill(Enorm,delta,1.);

      hESHvsEPR->Fill(trk.EPRnorm(), trk.ESHnorm());

      //      output << Enorm*P/1000. << " " << P/1000. << " " << delta << " "
      //	     << trk.GetX() << " " << trk.GetY() << endl;

      nev++;
    }

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
  char* fname = Form("pcal.param.%s_%d-%d",fRunNumber.c_str(),fNstart,fNstop);
  cout << "SaveAlphas: fname=" << fname << endl;

  output.open(fname,ios::out);

  output << "; Calibration constants for run " << fRunNumber 
	 << ", " << fNev << " events processed" << endl;
  output << endl;

  UInt_t j = 0;

  for (UInt_t k=0; k<THcPShTrack::fNcols_pr; k++) {
    k==0 ? output << "pcal_neg_gain_cor =" : output << "pcal_pos_gain_cor =";
    for (UInt_t i=0; i<THcPShTrack::fNrows_pr; i++)
      output << fixed << setw(6) << setprecision(2) << falphaC[j++] << ",";
    output << endl;
  }

  for (UInt_t k=0; k<THcPShTrack::fNcols_sh; k++) {
    k==0 ? output << "pcal_arr_gain_cor =" : output << "                  ";
    for (UInt_t i=0; i<THcPShTrack::fNrows_sh; i++)
      output << fixed << setw(6) << setprecision(2) << falphaC[j++] << ",";
    output << endl;
  }

  output.close();
}

#endif
