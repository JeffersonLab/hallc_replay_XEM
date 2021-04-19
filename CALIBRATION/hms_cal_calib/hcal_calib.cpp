#include "TCanvas.h"
#include "TLine.h"
#include <TStyle.h>
#include <TH1.h>
#include <TEllipse.h>
#include <TF1.h>
#include <TPaveText.h>
#include "THcShowerCalib.h"

//
// A steering Root script for the HMS calorimeter calibration.
//

void hcal_calib(string Prefix, int nstop=-1, int nstart=0) {

  bool DRAW = 1;  //flag to draw extra plots

  // Initialize the analysis clock
  clock_t t = clock();
  
  cout << "Calibrating file " << Prefix << ".root, events "
       << nstart << " -- " << nstop << endl;

  THcShowerCalib theShowerCalib(Prefix, nstart, nstop);

 theShowerCalib.ReadThresholds();  // Read in threshold param-s and intial gains
 theShowerCalib.Init();            // Initialize constants and variables
 theShowerCalib.CalcThresholds();  // Thresholds on the uncalibrated Edep/P
 theShowerCalib.ComposeVMs();      // Compute vectors amd matrices for calib.
 theShowerCalib.SolveAlphas();     // Solve for the calibration constants
 theShowerCalib.SaveAlphas();      // Save the constants
 // theShowerCalib.SaveRawData();  // Save raw data into file for debug purposes
 theShowerCalib.FillHEcal();       // Fill histograms
 // theShowerCalib.FillHEcalNoCor();  // Removes Y correction (pulseInt * gain * yCor) = E
 theShowerCalib.FillCutBranch();       // Fill diagnostic histos
 theShowerCalib.FillHitsGains();       // Plot hits and gains

 // Plot histograms
 //___________Canvas 1_______________
 TCanvas* Canvas =
 new TCanvas("Canvas", "HMS Shower Counter calibration", 1000, 667);
 Canvas->Divide(3,2);

 // Normalized uncalibrated energy deposition.
 Canvas->cd(1);
 theShowerCalib.hEunc->DrawCopy();
 theShowerCalib.hEuncSel->SetFillColor(kGreen);
 theShowerCalib.hEuncSel->DrawCopy("same");

 // E(Tail) vs E(Preshower) plot.
 Canvas->cd(2);
 theShowerCalib.hETAvsEPR->Draw("colz");

 // Uncalibrated e/p at calorimeter
 Canvas->cd(3);
 //  theShowerCalib.hCaloPosNormU->GetZaxis()->SetRangeUser(0.5,1.5);
 theShowerCalib.hCaloPosNormU->Draw("COLZ");
 theShowerCalib.hCaloPosNormU->SetStats(0);
 theShowerCalib.hCaloPosNormU->GetZaxis()->SetRangeUser(0.5,1.5);

 // Normalized energy deposition after calibration.
 Canvas->cd(4);
 gStyle->SetOptFit();
 //______wph Fit over range centered by max bin_________
 //**commented out next line to improve location of fit mean**
 // theShowerCalib.hEcal->Fit("gaus","O","",0.5,1.5); 
 Double_t maxBin= theShowerCalib.hEcal->GetMaximumBin();
 Double_t maxValue= theShowerCalib.hEcal->GetBinCenter(maxBin);
 theShowerCalib.hEcal->Fit("gaus","O","",maxValue-.1, maxValue+.2);
 TF1 *fit = theShowerCalib.hEcal->GetFunction("gaus");
 Double_t gmean  = fit->GetParameter(1);
 Double_t gsigma = fit->GetParameter(2);
 double gLoThr = gmean - 1.*gsigma;
 double gHiThr = gmean + 2.*gsigma;
 cout << "gLoThr=" << gLoThr << "  gHiThr=" << gHiThr << endl;
 theShowerCalib.hEcal->Fit("gaus","","",gLoThr,gHiThr);
 theShowerCalib.hEcal->GetFunction("gaus")->SetLineColor(2);
 theShowerCalib.hEcal->GetFunction("gaus")->SetLineWidth(1);
 theShowerCalib.hEcal->GetFunction("gaus")->SetLineStyle(1);
 // theShowerCalib.hEcalNoCor->SetLineColor(kGreen);
 // theShowerCalib.hEcalNoCor->Draw("same");

 // HMS delta(P) versus the calibrated energy deposition.
 Canvas->cd(5);
 theShowerCalib.hDPvsEcal->Draw("colz");

 
 // Canvas->cd(5);
 // theShowerCalib.hCaloPosWt->Draw("COLZ");
 Canvas->cd(6);
 theShowerCalib.hCaloPosNorm->GetZaxis()->SetRangeUser(0.05,1.2);
 theShowerCalib.hCaloPosNorm->SetTitle("Normalized E/p at Calorimeter");
  theShowerCalib.hCaloPosNorm->Draw("COLZ");
 
 // Save canvas in a pdf format.
 Canvas->Print(Form("%s_%d_%d.pdf",Prefix.c_str(),nstart,nstop));

// A bunch of diagnostic plots added Oct. 2019
//wph
 if(DRAW==1){ 
 TCanvas* Canvas2 =
   new TCanvas("Canvas2", "Hits, gains, and Projections", 1000, 667);

 TCanvas* Canvas3 =
   new TCanvas("Canvas3", "Branches for cuts", 1000, 667);

 TCanvas* Canvas4 =
   new TCanvas("Canvas4", "Pulse Integrals, x/y_calo vs E/p", 1000, 667);

 //___________Canvas 2_______________
 Canvas2->Divide(3,2);
 // Projection at calorimeter
 Canvas2->cd(1);
 theShowerCalib.hCaloPos->Draw("COLZ");

 // Projection at dipole
 Canvas2->cd(2);
 TEllipse *el1=new TEllipse( 0, 2.8, 46.507, 46.507);
 el1->SetLineColor(kRed);
 theShowerCalib.hExitPos->Draw("COLZ");
 el1->Draw("same");
 theShowerCalib.hExitPos->Draw("COLZ SAME");

 // Hits and gains for each block by layer
 Canvas2->cd(3);
 theShowerCalib.pr1->SetStats(0);
 theShowerCalib.pr1->Draw("TEXT COLZ");
 theShowerCalib.pr1a->Draw("TEXT SAME");
 Canvas2->cd(4);
 theShowerCalib.ta2->Draw("TEXT COLZ");
 theShowerCalib.ta2->SetStats(0);
 theShowerCalib.ta2a->Draw("TEXT SAME");
 Canvas2->cd(5);
 theShowerCalib.ta3->Draw("TEXT COLZ");
 theShowerCalib.ta3->SetStats(0);
 theShowerCalib.ta3a->Draw("TEXT SAME");
 Canvas2->cd(6);
 theShowerCalib.ta4->SetStats(0);
 theShowerCalib.ta4->Draw("TEXT COLZ");
 theShowerCalib.ta4a->Draw("TEXT SAME");
 // Set range for hits
 Double_t maxHit=1.3*theShowerCalib.pr1->GetMaximum();
 theShowerCalib.pr1->GetZaxis()->SetRangeUser(0,maxHit);
 theShowerCalib.ta2->GetZaxis()->SetRangeUser(0,maxHit);
 theShowerCalib.ta3->GetZaxis()->SetRangeUser(0,maxHit);
 theShowerCalib.ta4->GetZaxis()->SetRangeUser(0,maxHit);
 //gains font color
 theShowerCalib.pr1a->SetMarkerColor(kRed);
 theShowerCalib.ta2a->SetMarkerColor(kRed);
 theShowerCalib.ta3a->SetMarkerColor(kRed);
 theShowerCalib.ta4a->SetMarkerColor(kRed);

 //___________Canvas 3_______________
 Canvas3->Divide(3,2);
 //Cherenkov Cut
 Canvas3->cd(1);
 theShowerCalib.hCer->Draw();
 gPad->SetLogy();
 Double_t top=theShowerCalib.hCer->GetMaximum();
 Double_t locx=theShowerCalib.GetCerMin();
 TLine *cerl=new TLine(locx,0,locx,top);
 cerl->SetLineColor(kMagenta);
 cerl->Draw("same");

 // H.gtr.p
 Canvas3->cd(2);
 theShowerCalib.hP->Draw();

 //Delta Cut 
 Canvas3->cd(3);
 theShowerCalib.hDelta->Draw();
 top=theShowerCalib.hDelta->GetMaximum();
 locx=theShowerCalib.GetDeltaMin();
 TLine *dplmin=new TLine(locx,0,locx,top);
 dplmin->SetLineColor(kMagenta);
 dplmin->Draw("same");
 locx=theShowerCalib.GetDeltaMax();
 TLine *dplmax=new TLine(locx,0,locx,top);
 dplmax->SetLineColor(kMagenta);
 dplmax->Draw("same");

 //Beta Cut 
 Canvas3->cd(4);
 theShowerCalib.hBeta->Draw();
 gPad->SetLogy();
 top=theShowerCalib.hBeta->GetMaximum();
 locx=theShowerCalib.GetBetaMin();
 TLine *betalmin=new TLine(locx,0,locx,top);
 betalmin->SetLineColor(kMagenta);
 betalmin->Draw("same");
 locx=theShowerCalib.GetBetaMax();
 TLine *betalmax=new TLine(locx,0,locx,top);
 betalmax->SetLineColor(kMagenta);
 betalmax->Draw("same");
 // ncluster and ntrack
 Canvas3->cd(5);
 theShowerCalib.hClusTrk->Draw("BOX");
 //gPad->SetLogy();
 // theShowerCalib.hNclust->Draw();
 // Canvas3->cd(6);
 // gPad->SetLogy();
 // theShowerCalib.hNtrack->Draw();

 Canvas3->cd(6);
  TPaveText *pt=new TPaveText(0.1,0.1,.9,.9);
  pt->AddText("Percentage of Events passing cuts");
  Double_t val= theShowerCalib.GetRatio()*100;
  pt->AddText(Form("All Cuts:                    %.2f%%",val));
  //__________________________________________________________
  Double_t fb=theShowerCalib.GetCerMin();
  Double_t bmin=theShowerCalib.hCer->GetXaxis()->FindBin(fb);
  fb=theShowerCalib.hCer->GetXaxis()->GetXmax();
  Double_t bmax=theShowerCalib.hCer->GetXaxis()->FindBin(fb);
  Double_t intg=theShowerCalib.hCer->Integral(bmin,bmax);
  Double_t nentries=theShowerCalib.hCer->GetEntries();
  val=intg/nentries*100;
  pt->AddText(Form("Cherenkov:                   %.2f%%",val));
  //__________________________________________________________
  fb=theShowerCalib.GetBetaMin();
  bmin=theShowerCalib.hBeta->GetXaxis()->FindBin(fb);
  fb=theShowerCalib.GetBetaMax();
  bmax=theShowerCalib.hBeta->GetXaxis()->FindBin(fb);
  intg=theShowerCalib.hBeta->Integral(bmin,bmax);
  nentries=theShowerCalib.hBeta->GetEntries();
  val=intg/nentries*100;
  pt->AddText(Form("Beta:                        %.2f%%",val));
  //__________________________________________________________
  fb=theShowerCalib.GetDeltaMin();
  bmin=theShowerCalib.hDelta->GetXaxis()->FindBin(fb);
  fb=theShowerCalib.GetDeltaMax();
  bmax=theShowerCalib.hDelta->GetXaxis()->FindBin(fb);
  intg=theShowerCalib.hDelta->Integral(bmin,bmax);
  nentries=theShowerCalib.hDelta->GetEntries();
  val=intg/nentries*100;
  pt->AddText(Form("Delta:                       %.2f%%",val));
  //__________________________________________________________
  Double_t bx=theShowerCalib.hClusTrk->GetXaxis()->FindBin(1);
  Double_t by=theShowerCalib.hClusTrk->GetYaxis()->FindBin(1);
  Double_t tot=theShowerCalib.hClusTrk->GetBinContent(bx,by);
  nentries=theShowerCalib.hClusTrk->GetEntries();
  val=tot/nentries*100;
  pt->AddText(Form("nClusters==1 && nTracks==1:  %.2f%%",val));
  //__________________________________________________________
  pt->Draw();

 //___________Canvas 4_______________
 Canvas4->Divide(2,2);

 //Pulse Intregral vs Block Number
 Canvas4->cd(1);
 theShowerCalib.pmtList->Draw("COLZ");

 // All 78 pulse integrals
 Canvas4->cd(2);
 Float_t adcmax=0;
 Float_t temp=0;
 for(UInt_t i=1; i<78; i++){
   temp=theShowerCalib.hAdc[i]->GetMaximum();
   if (temp>adcmax)adcmax=temp;}
 theShowerCalib.hAdc[0]->Draw();
 theShowerCalib.hAdc[0]->GetYaxis()->SetRangeUser(0,adcmax);
 for(UInt_t i=1; i<78; i++){theShowerCalib.hAdc[i]->Draw("same");}

 // Y_calo vs E/p
 Canvas4->cd(3);
 theShowerCalib.yCalVsEp->Draw("COLZ");

 // X_calo vs E/p
 Canvas4->cd(4);
 theShowerCalib.xCalVsEp->Draw("COLZ");
 //  Lines representing block edges
 Int_t nRows=13;
 Double_t thkRow=10;
 Double_t startRow=-70.4;
 TLine *t[nRows+1];
 for (Int_t i=0;i<=nRows;i++)
 {
   Double_t y= startRow+i*thkRow;
   t[i]=new TLine(0.1,y,1.4,y);
   t[i]->SetLineColor(kRed);
   t[i]->Draw("same");
 }
 
 // Save canvases
 Canvas2->Print(Form("PDFs/hits_%s_%d_%d.pdf",Prefix.c_str(),nstart,nstop));
 Canvas3->Print(Form("PDFs/cuts_%s_%d_%d.pdf",Prefix.c_str(),nstart,nstop));
 Canvas3->Print(Form("PDFs/pInt_%s_%d_%d.pdf",Prefix.c_str(),nstart,nstop));
 }

 // Calculate the analysis rate
 //   TFile *oFile=new TFile("hcal_out.root","RECREATE");
 //  theShowerCalib.hEcal->Write();
 //  oFile->Close();
 t = clock() - t;
 printf ("The analysis took %.1f seconds \n", ((float) t) / CLOCKS_PER_SEC);

}
