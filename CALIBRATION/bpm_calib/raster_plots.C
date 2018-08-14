// this program is for bpm callibration in Hall C . 
// Time : Fall 2017 Run.

#include "TFile.h"
#include "TTree.h"
#include "TH1F.h"
#include "TF1.h"
#include "TGraph.h"
#include <fstream>
#include <iostream>
#include <TROOT.h>

void
raster_plots(Int_t run_NUM = 4251){
  
  
  gStyle->SetOptStat(0);
  gStyle->SetOptStat(0);
  gStyle->SetTitleFontSize(0.05);
  gStyle->SetNdivisions(505);
  gStyle->SetCanvasColor(10);
  gStyle->SetPadTopMargin(0.10);
  gStyle->SetPadLeftMargin(0.08);
  gStyle->SetPadRightMargin(0.08);
  gStyle->SetPadBottomMargin(.14);
  gStyle->SetTitleYOffset(1.09);
  gStyle->SetTitleXOffset(0.855);
  gStyle->SetTitleYSize(0.03);
  gStyle->SetTitleXSize(0.03);
  gStyle->SetLabelFont(62,"X");
  gStyle->SetLabelFont(62,"Y");
  gStyle->SetTitleFont(62,"X");
  gStyle->SetTitleFont(62,"Y");
  gStyle->SetLabelSize(0.025,"X");
  gStyle->SetLabelSize(0.025,"Y");
  gStyle->SetPaperSize(23,24);

  Double_t rasterAx;
  Double_t rasterAx_ADC;
  Double_t rasterAx_RawADC;
  Double_t rasterBx;
  Double_t rasterBx_ADC;
  Double_t rasterBx_RawADC;
  Double_t rasterAy;
  Double_t rasterAy_ADC;
  Double_t rasterAy_RawADC;
  Double_t rasterBy;
  Double_t rasterBy_ADC;
  Double_t rasterBy_RawADC;
  Double_t rasterA_radius;
  Double_t rasterB_radius;

  Double_t xoffA = 0.000;
  Double_t yoffA = 0.000;
  Double_t xoffB = 0.000;
  Double_t yoffB = 0.000;
  Double_t rAcut = 5.00;
  Double_t rBcut = 5.00;
  
  TFile *f = new TFile(Form("/Users/brash/Dropbox/Research/analysis/hallc_replay/ROOTfiles/coin_replay_production_%d_-1.root",run_NUM),"READ"); // %d : expects integer; %f expects float 
    
    
    TTree *T = (TTree*)f->Get("T");
    Int_t totev = T->GetEntries(); 
    //Read the branch for the BPM positions from the EPICS 
    T->SetBranchAddress("P.rb.raster.fr_xa",&rasterAx);
    T->SetBranchAddress("P.rb.raster.fr_xb",&rasterBx);
    T->SetBranchAddress("P.rb.raster.fr_ya",&rasterAy);
    T->SetBranchAddress("P.rb.raster.fr_yb",&rasterBy);
    T->SetBranchAddress("P.rb.raster.frxa_adc",&rasterAx_ADC);
    T->SetBranchAddress("P.rb.raster.frxb_adc",&rasterBx_ADC);
    T->SetBranchAddress("P.rb.raster.frya_adc",&rasterAy_ADC);
    T->SetBranchAddress("P.rb.raster.fryb_adc",&rasterBy_ADC);
    T->SetBranchAddress("P.rb.raster.frxaRawAdc",&rasterAx_RawADC);
    T->SetBranchAddress("P.rb.raster.frxbRawAdc",&rasterBx_RawADC);
    T->SetBranchAddress("P.rb.raster.fryaRawAdc",&rasterAy_RawADC);
    T->SetBranchAddress("P.rb.raster.frybRawAdc",&rasterBy_RawADC);
    //Creating the histogram of the BPM positions and 
    TH1F* hrasterAx =new TH1F("rasterAx","rasterAx",100,-3.00,3.00);
    TH1F* hrasterAy =new TH1F("rasterAy","rasterAy",100,-3.00,3.00);
    TH1F* hrasterBx =new TH1F("rasterBx","rasterBx",100,-3.00,3.00);
    TH1F* hrasterBy =new TH1F("rasterBy","rasterBy",100,-3.00,3.00);
    TH2F* hrasterAxAy =new TH2F("rasterAxAy","rasterAxAy",100,-3.00,3.00,100,-3.00,3.00);
    TH2F* hrasterBxBy =new TH2F("rasterBxBy","rasterBxBy",100,-3.00,3.00,100,-3.00,3.00);
    TH1F* hrasterA_radius =new TH1F("rasterA_radius","rasterA_radius",100,0.00,05.00);
    TH1F* hrasterB_radius =new TH1F("rasterB_radius","rasterB_radius",100,0.00,5.55);
    // Fill Histograms here   
    for (Int_t iev = 0 ; iev < totev ;iev ++){
      if (iev%1000 == 0) cout << "Event: " << iev << endl;
      T->GetEntry(iev);
      Double_t rA = sqrt((10.0*rasterAx-xoffA)*(10.0*rasterAx-xoffA)+(10.0*rasterAy-yoffA)*(10.0*rasterAy-yoffA));
      Double_t rB = sqrt((10.0*rasterBx-xoffB)*(10.0*rasterBx-xoffB)+(10.0*rasterBy-yoffB)*(10.0*rasterBy-yoffB));
      //if (ibcm1>1){
	hrasterAx ->Fill(10.0*rasterAx);
	hrasterBx ->Fill(10.0*rasterBx);
	hrasterAy ->Fill(10.0*rasterAy);
	hrasterBy ->Fill(10.0*rasterBy);
        hrasterAxAy->Fill(10.0*rasterAx,10.0*rasterAy);
        hrasterBxBy->Fill(10.0*rasterBx,10.0*rasterBy);
        if (rA<rAcut) hrasterA_radius->Fill(rA);
        if (rB<rBcut) hrasterB_radius->Fill(rB);
      //}
    }

    TCanvas *craster = new TCanvas("craster","Raster X, Y : Hall C", 800, 900);
    TCanvas *crasterA = new TCanvas("crasterA","RasterA X vs Y : Hall C", 800, 900);
    TCanvas *crasterB = new TCanvas("crasterB","RasterB X vs Y : Hall C", 800, 900);
    TCanvas *crasterRad = new TCanvas("crasterRad","Raster Radius : Hall C", 800, 900);
    //Draw the bpm_measured vs bpm_cal
    craster->Divide(2,2);   

    craster->cd(1);
    hrasterAx->Draw();
    craster->cd(2);
    hrasterAy->Draw();
    craster->cd(3);
    hrasterBx->Draw();
    craster->cd(4);
    hrasterBy->Draw();
    crasterA->cd();
    //crasterA->SetLogz();
    hrasterAxAy->Draw("COLZ");
    crasterB->cd();
    //crasterB->SetLogz();
    hrasterBxBy->Draw("COLZ");
    crasterRad->cd();
    crasterRad->Divide(1,2);
    crasterRad->cd(1);
    hrasterA_radius->Draw();
    crasterRad->cd(2);
    hrasterB_radius->Draw();

}

