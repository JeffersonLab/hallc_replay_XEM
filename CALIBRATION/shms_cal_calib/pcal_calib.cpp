#include <TStyle.h>
#include <TCanvas.h>
#include <TH1.h>
#include <TF1.h>
#include "THcPShowerCalib.h"

//
// A steering Root script for the SHMS calorimeter calibration.
//

////void pcal_calib(Int_t RunNumber) {
void pcal_calib(string RunNumber) {
 
 cout << "Calibrating run " << RunNumber << endl;

 THcPShowerCalib theShowerCalib(RunNumber);

 theShowerCalib.Init();            // Initialize constants and variables
 theShowerCalib.CalcThresholds();  // Thresholds on the uncalibrated Edep/P
 theShowerCalib.ComposeVMs();      // Compute vectors amd matrices for calib.
 theShowerCalib.SolveAlphas();     // Solve for the calibration constants
 theShowerCalib.SaveAlphas();      // Save the constants
 //theShowerCalib.SaveRawData();   // Save raw data into file for debug purposes
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
 theShowerCalib.hESHvsEPR->Draw();

 // Normalized energy deposition after calibration.

 Canvas->cd(3);
 gStyle->SetOptFit();

 theShowerCalib.hEcal->Fit("gaus","","",0.9,1.1);
 theShowerCalib.hEcal->GetFunction("gaus")->SetLineColor(2);
 theShowerCalib.hEcal->GetFunction("gaus")->SetLineWidth(1);
 theShowerCalib.hEcal->GetFunction("gaus")->SetLineStyle(1);

 // SHMS delta(P) versus the calibrated energy deposition.

 Canvas->cd(4);
 theShowerCalib.hDPvsEcal->Draw();

}
