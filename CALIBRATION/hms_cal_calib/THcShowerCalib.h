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
  void SaveAlphas();
  void SaveRawData();

  TH1F* hEunc;
  TH1F* hEuncSel;
  TH1F* hEcal;
  TH2F* hDPvsEcal;
  TH2F* hETAvsEPR;

 private:

  string fPrefix;

  Double_t fDeltaMin, fDeltaMax;   // Delta range, %.
  Double_t fBetaMin, fBetaMax;     // Beta range
  Double_t fCerMin;                // Threshold Cerenkov signal, p.e.
  UInt_t fMinHitCount;             // Min. number of hits/chan. for calibration
  Double_t fEuncLoLo, fEuncHiHi;   // Range of uncalibrated Edep histogram
  UInt_t fEuncNBin;                // Binning of uncalibrated Edep histogram
  Double_t fEuncGFitLo,fEuncGFitHi;// Gaussian fit range of uncalib. Edep histo.

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

  double          H_tr_n;
  Double_t        H_tr_p;
  Double_t        H_tr_x;   //X FP
  Double_t        H_tr_xp;
  Double_t        H_tr_y;   //Y FP
  Double_t        H_tr_yp;

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
  cout << "  Uncalibrated histo. fit range: " << fEuncGFitLo << "  "
       << fEuncGFitHi << endl;
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
  fTree->SetBranchAddress("H.dc.x_fp",&H_tr_x,&b_H_tr_x);
  fTree->SetBranchAddress("H.dc.y_fp",&H_tr_y,&b_H_tr_y);
  fTree->SetBranchAddress("H.dc.xp_fp",&H_tr_xp,&b_H_tr_xp);
  fTree->SetBranchAddress("H.dc.yp_fp",&H_tr_yp,&b_H_tr_yp);
  fTree->SetBranchAddress("H.gtr.p",&H_tr_p,&b_H_tr_p);

  fTree->SetBranchAddress("H.gtr.dp", &H_tr_tg_dp,&b_H_tr_tg_dp);
 
  fTree->SetBranchAddress("H.cer.npeSum", &H_cer_npeSum,&b_H_cer_npeSum);
  fTree->SetBranchAddress("H.hod.beta", &H_tr_beta,&b_H_tr_beta);

  fTree->SetBranchAddress("H.cal.nclust", &H_cal_nclust,&b_H_cal_nclust);

  // Histogram declarations.

  hEunc = new TH1F("hEunc","Edep/P uncalibrated",fEuncNBin,fEuncLoLo,fEuncHiHi);
  hEcal = new TH1F("hEcal", "Edep/P calibrated", 200, 0., 2.);
  hDPvsEcal = new TH2F("hDPvsEcal", "#DeltaP versus Edep/P ",
		       150,0.,1.5, 250,-12.5,12.5);
  hETAvsEPR = new TH2F("hETAvsEPR", "E_{TA} versus E_{PR}",
		       300,0.,1.5, 300,0.,1.5);

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

    if (nev > 200000) break;
  };

  //  hEunc->Fit("gaus","0","",0.5, 1.5);
  hEunc->Fit("gaus","","",fEuncGFitLo, fEuncGFitHi);
  hEunc->GetFunction("gaus")->SetLineColor(2);
  hEunc->GetFunction("gaus")->SetLineWidth(1);
  hEunc->GetFunction("gaus")->SetLineStyle(1);
  TF1 *fit = hEunc->GetFunction("gaus");
  Double_t gmean  = fit->GetParameter(1);
  Double_t gsigma = fit->GetParameter(2);
  fLoThr = gmean - 3.*gsigma;
  fHiThr = gmean + 3.*gsigma;
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
		    H_tr_y + H_tr_yp*D_CALO_FP < YMAX ;
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

  ofstream output;
  output.open("calibrated.deb",ios::out);

  Int_t nev = 0;

  THcShTrack trk;

  for (UInt_t ientry=fNstart; ientry<fNstop; ientry++) {

    if (ReadShRawTrack(trk, ientry)) {
      //    trk.Print(cout);

      trk.SetEs(falphaC);        // use the 'constrained' calibration constants
      Double_t P = trk.GetP();
      Double_t delta = trk.GetDp();
      Double_t Enorm = trk.Enorm();

      hEcal->Fill(Enorm);

      hDPvsEcal->Fill(Enorm,delta,1.);

      hETAvsEPR->Fill(trk.EPRnorm(), trk.ETAnorm());

      output << Enorm*P/1000. << " " << P/1000. << " " << delta << " "
      	     << trk.GetX() << " " << trk.GetY() << endl;

      nev++;
    }

    if (nev > 200000) break;
  };

  output.close();

  cout << "FillHEcal: " << nev << " events filled" << endl;
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

#endif
