#include "TCanvas.h"
#include <TStyle.h>
#include <TH1.h>
#include <TF1.h>
#include "THcShowerCalib.h"

//
// A steering Root script for the HMS calorimeter calibration.
//

void hcal_calib(string RunNumber) {

  // Initialize the analysis clock
  clock_t t = clock();
  
 cout << "Calibrating run " << RunNumber << endl;

 THcShowerCalib theShowerCalib(RunNumber);

 theShowerCalib.Init();            // Initialize constants and variables
 theShowerCalib.CalcThresholds();  // Thresholds on the uncalibrated Edep/P
 theShowerCalib.ComposeVMs();      // Compute vectors amd matrices for calib.
 theShowerCalib.SolveAlphas();     // Solve for the calibration constants
 theShowerCalib.SaveAlphas();      // Save the constants
 // theShowerCalib.SaveRawData();  // Save raw data into file for debug purposes
 theShowerCalib.FillHEcal();       // Fill histograms

 // Plot histograms

 TCanvas* Canvas =
   new TCanvas("Canvas", "HMS Shower Counter calibration", 1000, 667);
 Canvas->Divide(2,2);

 Canvas->cd(1);

 // Normalized uncalibrated energy deposition.

 theShowerCalib.hEunc->DrawCopy();
  
 theShowerCalib.hEuncSel->SetFillColor(kGreen);
 theShowerCalib.hEuncSel->DrawCopy("same");

 // E(Tail) vs E(Preshower) plot.

 Canvas->cd(2);
 theShowerCalib.hETAvsEPR->Draw("colz");

 // Normalized energy deposition after calibration.

 Canvas->cd(3);
 gStyle->SetOptFit();

 theShowerCalib.hEcal->Fit("gaus","","",0.95,1.05);
 theShowerCalib.hEcal->GetFunction("gaus")->SetLineColor(2);
 theShowerCalib.hEcal->GetFunction("gaus")->SetLineWidth(1);
 theShowerCalib.hEcal->GetFunction("gaus")->SetLineStyle(1);

 // HMS delta(P) versus the calibrated energy deposition.

 Canvas->cd(4);
 theShowerCalib.hDPvsEcal->Draw("colz");

 // Calculate the analysis rate
 t = clock() - t;
 printf ("The analysis took %.1f seconds \n", ((float) t) / CLOCKS_PER_SEC);
}
