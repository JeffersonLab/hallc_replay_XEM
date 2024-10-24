#include <TStyle.h>
#include <TLine.h>
#include <TArc.h>
#include <TCanvas.h>
#include <TPaveText.h>
#include <TH1.h>
#include <TF1.h>
#include "THcPShowerCalib.h"

//
// A steering Root script for the SHMS calorimeter calibration.
//

void pcal_calib(string Prefix, int nstop=-1, int nstart=0) {

  bool DRAW = 0;  //flag to draw extra plots   /////////////////DEFAULT IS 1, make sure to change back!!!!!!!!////////////////////////
  bool SAVE = 1;  //flag to save plots in root file 

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
  theShowerCalib.fillHits();       // Fill hits
  theShowerCalib.fillCutBranch();       // Fill cut histos.
  //  theShowerCalib.howLong(100);       // How many triggers to reach stat. goal

  // Plot histograms
 Int_t cwid=900;
 Int_t cht=450;
 Int_t cxt=50;
  //___________Canvas 1_______________
  TCanvas* Canvas =
   new TCanvas("Canvas", "SHMS Shower Counter calibration",0,0,cwid,cht);
  Canvas->Divide(4,2);

  Canvas->cd(1);

  // Normalized uncalibrated energy deposition.
  theShowerCalib.hEunc->DrawCopy();
  theShowerCalib.hEuncSel->SetFillColor(kGreen);
  theShowerCalib.hEuncSel->DrawCopy("same");

 // E(Tail) vs E(Preshower) plot.
  Canvas->cd(2);
  theShowerCalib.hESHvsEPR->Draw("colz");

  // Normalized energy deposition after calibration.
  Canvas->cd(5);
  gStyle->SetOptFit();

 //wph______ Fit over range centered by max bin_________
 //**commented out next line to improve location of fit mean**
 // theShowerCalib.hEcal->Fit("gaus","O","",0.5,1.5); 
 Double_t maxBin= theShowerCalib.hEcal->GetMaximumBin();
 Double_t maxValue= theShowerCalib.hEcal->GetBinCenter(maxBin);
 theShowerCalib.hEcal->Fit("gaus","O","",maxValue-.1, maxValue+.2);
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
  Canvas->cd(6);
  theShowerCalib.hDPvsEcal->Draw("colz");

  // Uncalibrated e/p at calorimeter
  Canvas->cd(3);
  //  theShowerCalib.hCaloPosNormU->GetZaxis()->SetRangeUser(0.5,1.5);
  theShowerCalib.hCaloPosNormU->Draw("COLZ");
  theShowerCalib.hCaloPosNormU->SetStats(0);
  theShowerCalib.hCaloPosNormU->GetZaxis()->SetRangeUser(0.5,1.5);

  // *************Draw shower blocks **************
  Int_t nRows=16;
  Int_t nCols=14;
  Double_t thkRow=9;
  Double_t startRow=-thkRow*nRows/2.;
  Double_t startCol=-thkRow*nCols/2;
  TLine *tr[nRows];
  TLine *tc[nCols];
  for (Int_t i=0;i<=nRows;i++)
    {
      Double_t y= startRow+i*thkRow;
      //   tr[i]=new TLine(-63,y,63,y);
      tr[i]=new TLine(startCol,y,startCol+thkRow*nCols,y);
      tr[i]->SetLineColor(kRed-3);
      tr[i]->Draw("same");
    } 
  for (Int_t i=0;i<=nCols;i++)
    {
      Double_t x= startCol+i*thkRow;
      tc[i]=new TLine(x,startRow,x,startRow+thkRow*nRows);
      tc[i]->SetLineColor(kRed-3);
      tc[i]->Draw("same");
    } 
  // *********Done drawing shower blocks **************

  // Calibrated e/p at calorimeter
  Canvas->cd(7);
  theShowerCalib.hCaloPosNorm->GetZaxis()->SetRangeUser(0.5,1.5);
  theShowerCalib.hCaloPosNorm->Draw("COLZ");
  theShowerCalib.hCaloPosNorm->SetStats(0);
  for (Int_t i=0;i<=nRows;i++){tr[i]->Draw("same");} 
  for (Int_t i=0;i<=nCols;i++){tc[i]->Draw("same");} 

  Canvas->cd(8);
  theShowerCalib.hEcalvxXbj->Draw("colz");

  // Save histograms in another file for further use
//  fname = Form("caloHistos%d.root" , run);
  TFile f("histos.root","new");
  theShowerCalib.hEunc->Write();
  theShowerCalib.hEcal->Write();
  theShowerCalib.hESHvsEPR->Write();
  theShowerCalib.hDPvsEcal->Write();
  theShowerCalib.hCaloPosNormU->Write();
  theShowerCalib.hCaloPosNorm->Write();
  theShowerCalib.hEcalvxXbj->Write();

  // Save canvas in a pdf format.
  Canvas->Print(Form("PDFs/%s_%d_%d.pdf",Prefix.c_str(),nstart,nstop));

// A bunch of diagnostic plots added Oct. 2019
// SET DRAW==0 at beginning of code to avoid drawing the extras 
//wph

 TCanvas *Canvas2, *Canvas3, *Canvas4;
 if(DRAW==1){ 
 Canvas2 =
   new TCanvas("Canvas2", "Hits, gains, and Projections",cwid+cxt,0,cwid,cht);
 Canvas3 =
   new TCanvas("Canvas3", "Branches for cuts",0,cht+cxt,cwid,cht);
 Canvas4 =
   new TCanvas("Canvas4", "Pulse Integrals, x/y_calo vs E/p",cwid+cxt,cht+cxt,cwid,cht);
 //___________Canvas 2_______________
  Canvas2->Divide(3,2);

  //Hits on preshower
  Canvas2->cd(1);
  theShowerCalib.hpr->Draw("colz TEXT");
  //Gains on shower
  Canvas2->cd(2);
  theShowerCalib.hsh->Draw("colz TEXT");
  //Projection at calorimeter
  Canvas2->cd(3);
  theShowerCalib.hCaloPos->Draw("colz");
  //Hits on preshower
  Canvas2->cd(4);
  theShowerCalib.hpra->Draw("colz TEXT");
  //Gains on preshower
  Canvas2->cd(5);
  theShowerCalib.hsha->Draw("colz TEXT");
  //Projection at dipole exit
  Canvas2->cd(6);
  theShowerCalib.hExitPos->Draw("colz");
  TArc *aR=new TArc(11.549/2,0,23.81,-90,90);
  aR->Draw("same only");
  aR->SetLineColor(kRed);
  TArc *aL=new TArc(-11.549/2,0,23.81,90,270);
  aL->Draw("same only");
  aL->SetLineColor(kRed);
  TLine *lt=new TLine(-11.549/2,23.81,11.549/2,23.81);
  lt->Draw("same");
  lt->SetLineColor(kRed);
  TLine *lb=new TLine(-11.549/2,-23.81,11.549/2,-23.81);
  lb->Draw("same");
  lb->SetLineColor(kRed);
  theShowerCalib.hExitPos->Draw("same colz");

  Canvas2->Print(Form("PDFs/hits_%s_%d_%d.pdf",Prefix.c_str(),nstart,nstop));
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

  //Stat box to give % events loss on cuts
  //NOT FINISHED :(
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

  Canvas3->Print(Form("PDFs/cuts_%s_%d_%d.pdf",Prefix.c_str(),nstart,nstop));

 //___________Canvas 4_______________
 Canvas4->Divide(3,2);

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
 Canvas4->cd(3);
  theShowerCalib.hCaloPosNormPint->Draw("COLZ");
  for (Int_t i=0;i<=nRows;i++){tr[i]->Draw("same");} 
  for (Int_t i=0;i<=nCols;i++){tc[i]->Draw("same");} 

 // Y_calo vs E/p
 Canvas4->cd(4);
 theShowerCalib.yCalVsEp->Draw("COLZ");

 Int_t nCol=14;
 Double_t thkCol=9;
 Double_t startCol=-thkCol*nCol/2;
 TLine *tcol[nCol+1];
 for (Int_t i=0;i<=nCol;i++)
 {
   Double_t y= startCol+i*thkCol;
   tcol[i]=new TLine(0.1,y,1.4,y);
   tcol[i]->SetLineColor(kRed-3);
   tcol[i]->Draw("same");
 }

 // X_calo vs E/p
 Canvas4->cd(5);
 theShowerCalib.xCalVsEp->Draw("COLZ");
 //  Lines representing block edges
 Int_t nRows=16;
 Double_t thkRow=9;
 Double_t startRow=-thkRow*nRows/2;
 TLine *t[nRows+1];
 for (Int_t i=0;i<=nRows;i++)
 {
   Double_t y= startRow+i*thkRow;
   t[i]=new TLine(0.1,y,1.4,y);
   t[i]->SetLineColor(kRed-3);
   t[i]->Draw("same");
 }

  Canvas4->Print(Form("PDFs/pInt_%s_%d_%d.pdf",Prefix.c_str(),nstart,nstop));

 }

 if (SAVE==1){
  TFile* froot=new TFile(Form("ROOTfiles/SHMS/CALIBRATION/%s_%d_%d.root",Prefix.c_str(),nstart,nstop),
			 "RECREATE");
  cout << "Savings histograms...." << endl;
  theShowerCalib.hEunc->Write();
  theShowerCalib.hEuncSel->Write();
  theShowerCalib.hESHvsEPR->Write();
  theShowerCalib.hEcal->Write();
  theShowerCalib.hDPvsEcal->Write();
  theShowerCalib.hETOTvsEPR->Write();        ////
  theShowerCalib.hEPRunc->Write();           ////
  theShowerCalib.hETOTvsEPRunc->Write();     ////
  theShowerCalib.hESHvsEPRunc->Write();      ////
  Canvas->Write();
  Canvas2->Write();
  Canvas3->Write();
  Canvas4->Write();
  froot->Close();
 }

  // Calculate the analysis rate
  t = clock() - t;
  printf ("The analysis took %.1f seconds \n", ((float) t) / CLOCKS_PER_SEC);
}
