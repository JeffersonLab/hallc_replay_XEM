// Small code to check the histograms for the coincidence runs 

#include "TFile.h"
#include "TTree.h"
#include "TH1F.h"
#include "TF1.h"
#include "TGraph.h"
#include <fstream>
#include <iostream>
#include <TROOT.h>


void 

coin_histo(){

  Int_t run_NUM ;
  cout << "hms_run_NUM "<< endl;
  cin >> run_NUM ; 
  
  
  TFile *f = new TFile(Form("/net/cdaq/cdaql3data/cdaq/hallc-online/ROOTfiles/coin_replay_production_%d_-1.root",run_NUM),"READ"); // %d : expects integer; %f expects float
  
  
  TTree *T = (TTree*)f->Get("T");
  
  
  Int_t totev = T->GetEntries();
  cout << totev << endl;
  

  // Read the branch 
  
  Double_t HkinQ2;
  Double_t HkinW;
  Double_t pkinpmiss;
  Double_t pkinemiss; 
  Double_t Hgtrdp;
  Double_t Hgtrbeta;
  Double_t Pgtrdp;
  Double_t Pgtrbeta;
  Double_t Hgtrph;
  Double_t Hgtrth;
  Double_t Hgtry;
  Double_t Pgtrph;
  Double_t Pgtrth;
  Double_t Pgtry;
  Double_t hcernpe; 
  Double_t pcernpe;
  Double_t hcalepr;
  Double_t hcaletot;
  



  T->SetBranchAddress("H.kin.primary.Q2", &HkinQ2);  
  T->SetBranchAddress("H.kin.primary.W", &HkinW); 
  T->SetBranchAddress("P.kin.secondary.pmiss",&pkinpmiss);
  T->SetBranchAddress("P.kin.secondary.emiss",&pkinemiss);
  T->SetBranchAddress("H.gtr.dp",&Hgtrdp);
  T->SetBranchAddress("H.gtr.beta",&Hgtrbeta);
  T->SetBranchAddress("P.gtr.dp",&Pgtrdp);
  T->SetBranchAddress("P.gtr.beta",&Pgtrbeta);
  T->SetBranchAddress("H.gtr.ph",&Hgtrph);
  T->SetBranchAddress("H.gtr.th",&Hgtrth);
  T->SetBranchAddress("H.gtr.y",&Hgtry);
  T->SetBranchAddress("P.gtr.ph",&Pgtrph);
  T->SetBranchAddress("P.gtr.th",&Pgtrth);
  T->SetBranchAddress("P.gtr.y",&Pgtry);
  T->SetBranchAddress("H.cal.eprtracknorm", &hcalepr);
  T->SetBranchAddress("P.hgcer.npeSum", &pcernpe);
  T->SetBranchAddress("H.cer.npeSum", &hcernpe);
  T->SetBranchAddress("H.cal.etottracknorm", &hcaletot);

  //Creating the histogram 
  TH1F* hHkinQ2 =new TH1F("HkinQ2","HkinQ2",50,5,15);
  TH1F* hHkinW = new TH1F("HkinW","HkinW",50,0,2);
  TH1F* hpkinpmiss = new TH1F("pkinpmiss","pkinpmiss",50,-0.2,0.2);
  TH1F* hpkinemiss = new TH1F("pkinemiss","pkinemiss",50,-100,100);
  TH1F* hHgtrdp = new TH1F("Hgtrdp","Hgtrdp",50,-15,15);
  TH1F* hHgtrbeta = new TH1F("Hgtrbeta","hgtrbeta",50,0.5,1.5);
  TH1F* hPgtrdp = new TH1F("Pgtrdp","Pgtrdp",100,-15,15);
  TH1F* hPgtrbeta= new TH1F("Pgtrbeta","Pgtrbeta",50,0.5,1.5);
  TH1F* hHgtrph = new TH1F("Hgtrph","Hgtrph",50,-0.1,0.1);
  TH1F* hHgtrth= new TH1F("Hgtrth","Hgtrth",50,-0.1,0.1);
  TH1F* hHgtry=new TH1F("Hgtry","Hgtry",50,-10,10);
  TH1F* hPgtrph = new TH1F("Pgtrph","Pgtrph",50, -0.1, 0.1);
  TH1F* hPgtrth= new TH1F("Pgtrth","Pgtrth",50,-0.1,0.1);
  TH1F* hPgtry=new TH1F("Pgtry","Pgtry",50,-10,10);
  TH1F* hHcernpe=new TH1F("hcernpe","hcernpe",50,0,20);
  TH1F* hPcernpe=new TH1F("pcernpe","pcernpe",50,0,20);
  TH1F* hHcaletot=new TH1F("hcaletot","hcaletot",50,0,2.0);
  TH1F* hHcalepr=new TH1F("hcalepr","hcalepr",50,0,2.0);
  
  //Fill the Histogram 
  for (Int_t iev = 0 ; iev < totev ;iev ++){
    T->GetEntry(iev);
    hHcalepr->Fill(hcalepr);
    hHcaletot->Fill(hcaletot);
    hHcernpe->Fill(hcernpe);
    hPcernpe->Fill(pcernpe);
    hHkinQ2 ->Fill(HkinQ2);
    hHkinW ->Fill(HkinW);
    hpkinpmiss ->Fill(pkinpmiss);
    hpkinemiss ->Fill(pkinemiss);
    hHgtrdp ->Fill(Hgtrdp);
    hHgtrbeta -> Fill(Hgtrbeta);
    hPgtrdp -> Fill(Pgtrdp);
    hPgtrbeta-> Fill(Pgtrbeta);
    hHgtrph -> Fill(Hgtrph);
    hHgtrth-> Fill(Hgtrth);
    hHgtry->Fill(Hgtry);
    hPgtrph->Fill(Pgtrph);
    hPgtrth->Fill(Pgtrth);
    hPgtry->Fill(Pgtry);
    
    
  }
}
