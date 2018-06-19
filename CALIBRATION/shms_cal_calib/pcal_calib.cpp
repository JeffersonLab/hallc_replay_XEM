#include <TStyle.h>
#include <TCanvas.h>
#include <TH1.h>
#include <TF1.h>
#include "THcPShowerCalib.h"

//
// A steering Root script for the SHMS calorimeter calibration.
//

void pcal_calib(string Prefix, int nstop=-1, int nstart=0) {

  // Initialize the analysis clock
  clock_t t = clock();
 
  cout << "Calibrating file " << Prefix << ".root, events "
       << nstart << " -- " << nstop << endl;

  THcPShowerCalib theShowerCalib(Prefix, nstart, nstop);

  theShowerCalib.ReadThresholds();  // Read in threshold param-s and intial gains
  theShowerCalib.Init();            // Initialize constants and variables
  theShowerCalib.CalcThresholds();  // Thresholds on the uncalibrated Edep/P
  theShowerCalib.ComposeVMs();      // Compute vectors amd matrices for calib.
  theShowerCalib.SolveAlphas();     // Solve for the calibration constants
  theShowerCalib.SaveAlphas();      // Save the constants
  //theShowerCalib.SaveRawData();   // Save raw data into file for debuging
  theShowerCalib.FillHEcal();       // Fill histograms

  // Plot histograms

  TCanvas* Canvas =
    new TCanvas("Canvas", "PHMS Shower Counter calibration", 1000, 667);
  Canvas->Divide(2,2);

  Canvas->cd(1);

  // Normalized uncalibrated energy deposition.

  theShowerCalib.hEunc->DrawCopy();
  
  theShowerCalib.hEuncSel->SetFillColor(kGreen);
  theShowerCalib.hEuncSel->DrawCopy("same");

  Canvas->cd(2);
  theShowerCalib.hESHvsEPR->Draw("colz");

  // Normalized energy deposition after calibration.

  Canvas->cd(3);
  gStyle->SetOptFit();

  theShowerCalib.hEcal->Fit("gaus","O","",0.5,1.5);
  TF1 *fit = theShowerCalib.hEcal->GetFunction("gaus");
  Double_t gmean  = fit->GetParameter(1);
  Double_t gsigma = fit->GetParameter(2);
  double gLoThr = gmean - 2.*gsigma;
  double gHiThr = gmean + 2.*gsigma;
  cout << "gLoThr=" << gLoThr << "  gHiThr=" << gHiThr << endl;
  theShowerCalib.hEcal->Fit("gaus","","",gLoThr,gHiThr);

  theShowerCalib.hEcal->GetFunction("gaus")->SetLineColor(2);
  theShowerCalib.hEcal->GetFunction("gaus")->SetLineWidth(1);
  theShowerCalib.hEcal->GetFunction("gaus")->SetLineStyle(1);

  // SHMS delta(P) versus the calibrated energy deposition.

  Canvas->cd(4);
  theShowerCalib.hDPvsEcal->Draw("colz");

  // Save canvas in a pdf format.
  Canvas->Print(Form("%s_%d_%d.pdf",Prefix.c_str(),nstart,nstop));

  // Save histograms in root file.

  TFile* froot=new TFile(Form("%s_%d_%d.root",Prefix.c_str(),nstart,nstop),
			 "RECREATE");
  theShowerCalib.hEunc->Write();
  theShowerCalib.hEuncSel->Write();
  theShowerCalib.hESHvsEPR->Write();
  theShowerCalib.hEcal->Write();
  theShowerCalib.hDPvsEcal->Write();
  theShowerCalib.hETOTvsEPR->Write();        ////
  theShowerCalib.hEPRunc->Write();           ////
  theShowerCalib.hETOTvsEPRunc->Write();     ////
  theShowerCalib.hESHvsEPRunc->Write();      ////
  froot->Close();

  // Calculate the analysis rate
  t = clock() - t;
  printf ("The analysis took %.1f seconds \n", ((float) t) / CLOCKS_PER_SEC);
}
