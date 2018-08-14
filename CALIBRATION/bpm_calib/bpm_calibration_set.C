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
bpm_calibration_set(const char* finname  = "harp_info.txt"){
  
  
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
  
  
  Double_t hmin = -3.0;
  Double_t hmax =  3.0;
   
  Double_t bpmAz = 370.826; // unit cm , from survey 
  Double_t bpmBz = 224.968; // unit cm , from survey 
  Double_t bpmCz = 129.314; // unit cm , from survey 
  
  Double_t ibcm1;
  Double_t bpmAxpos,bpmAypos,bpmBxpos,bpmBypos,bpmCxpos,bpmCypos;
  Double_t ibcm1r;
  Double_t rasterAxpos,rasterAypos,rasterBxpos,rasterBypos;
  
  Double_t c1,c2,c3,c4,c5,c6,c7,c8,c9,c10,c11,c12,c13,c14; 
  std :: vector<Double_t>  hAx, hAy, hBx, hBy, hAz, hBz, herr ;
  std :: vector<Double_t>  bpmAx1,bpmAy1,bpmBx1, bpmBy1,bpmCx1, bpmCy1;
  //bpmAx1 : value of bpmAx from the Archive NOT EPICS 


  // read the data file harp_info.txt : 
  // Text file is filled up with the information from hc logentry : https://logbooks.jlab.org/files/2018/01/3517425/harp_scan.txt
  // data file structure :shms_run_num = shms run number, hAx = Harp A x pos, hAy = harp A y pos  , hBx = harp B x pos  , hBy = harp B y pos ,
  // hAz = harpA z pos (fixed comes from survey ), hBz = harpB z pos (fixed comes from survey) , 
  // bpmAx1 = bpmA x from archive, bpmAy1, bpmBx1, bpmCx1, bpmCy1, herr = error associated with harp(0.1 for all as suggested by Dave G. )  
  std :: vector<Int_t> shms_run_NUM;
  ifstream infile(finname);
  
  if(infile.fail()){
    cout << "Cannot open the file: " << endl;
    cout << "ok" << endl;   
    exit(1);
    
  }else{
    while(!infile.eof()){
      infile >> c1 >> c2 >> c3 >> c4 >> c5 >> c6 >> c7 >> c8 >> c9 >> c10 >> c11 >> c12 >> c13 >> c14 ;
      shms_run_NUM.push_back(c1);
      hAx.push_back(c2);
      hAy.push_back(c3);
      hBx.push_back(c4);
      hBy.push_back(c5);
      hAz.push_back(c6);
      hBz.push_back(c7);
      bpmAx1.push_back(c8);
      bpmAy1.push_back(c9);
      bpmBx1.push_back(c10);
      bpmBy1.push_back(c11);
      bpmCx1.push_back(c12);
      bpmCy1.push_back(c13);
      herr.push_back(c14);
    }
  }
  shms_run_NUM.pop_back();
  hAx.pop_back();
  hAy.pop_back();
  hBx.pop_back();
  hBy.pop_back();
  hAz.pop_back();
  hBz.pop_back();  
  bpmAx1.pop_back();
  bpmAy1.pop_back();
  bpmBx1.pop_back();
  bpmBy1.pop_back();
  bpmCx1.pop_back();
  bpmCy1.pop_back();
  herr.pop_back();

  cout << shms_run_NUM.size() << endl;   
  
  Int_t size = shms_run_NUM.size();
  cout << size <<" : size " <<  endl;
  
  
  std::vector<Double_t> bpmAxposmean(size), bpmAyposmean(size), bpmBxposmean(size), bpmByposmean(size);
  std::vector<Double_t> bpmCxposmean(size), bpmCyposmean(size);
  std::vector<Double_t> rasterAxposmean(size), rasterAyposmean(size), rasterBxposmean(size), rasterByposmean(size);
  
  std::vector<Double_t> bpmAxposmeanErr(size), bpmAyposmeanErr(size), bpmBxposmeanErr(size), bpmByposmeanErr(size);
  std::vector<Double_t> bpmCxposmeanErr(size), bpmCyposmeanErr(size);
  std::vector<Double_t> rasterAxposmeanErr(size), rasterAyposmeanErr(size), rasterBxposmeanErr(size), rasterByposmeanErr(size);
  
  std::vector<Double_t> bpmAxposmeanc(size), bpmAyposmeanc(size), bpmBxposmeanc(size), bpmByposmeanc(size);
  std::vector<Double_t> bpmCxposmeanc(size), bpmCyposmeanc(size);
  
  std::vector<Double_t> bpmAxposmeancErr(size), bpmAyposmeancErr(size), bpmBxposmeancErr(size), bpmByposmeancErr(size);
  std::vector<Double_t> bpmCxposmeancErr(size), bpmCyposmeancErr(size);
  
  std::vector<Double_t>err(size);
  std::vector<Double_t>errr(size);
  
  TCanvas *cmean = new TCanvas("cmean","Mean BPM Fits", 1800, 1200);
  cmean ->Divide(6,size);
  TCanvas *cmeanr = new TCanvas("cmeanr","Mean Raster Fits", 1800, 1200);
  cmeanr ->Divide(4,size);
  TCanvas *cmean2 = new TCanvas("cmean2","Mean BPM Fits", 1800, 1200);
  cmean2 ->Divide(6,size);

  for (UInt_t irr = 0; irr < hAx.size(); irr ++){  //Loop over raster runs starts here

    errr[irr] =0.0;
    
   
    //TFile *f = new TFile(Form("../hallc_replay/ROOTfiles/shms_replay_raster_simple_%d_-1.root",shms_run_NUM[ir]),"READ"); // %d : expects integer; %f expects float 
    
    TString file_format=gSystem->GetFromPipe("echo $hallc_replay_dir")+"/ROOTfiles/shms_replay_raster_simple_%d_-1.root";
    TFile *f = new TFile(Form(file_format,shms_run_NUM[irr]),"READ"); // %d : expects integer; %f expects float 
    TTree *T = (TTree*)f->Get("T");
    Int_t totev = T->GetEntries(); 
    //Read the branch for the raster positions from the event TREE 
    T->SetBranchAddress("ibcm1",&ibcm1r);
    T->SetBranchAddress("FRXApos",&rasterAxpos);
    T->SetBranchAddress("FRXBpos",&rasterBxpos);
    T->SetBranchAddress("FRYApos",&rasterAypos);
    T->SetBranchAddress("FRYBpos",&rasterBypos);
    //Creating the histogram of the raster positions and 
    TH1F* hrasterAxpos =new TH1F("rasterAxpos","rasterAxpos",100,-5,5);
    TH1F* hrasterAypos =new TH1F("rasterAypos","rasterAypos",100,-5,5);
    TH1F* hrasterBxpos =new TH1F("rasterBxpos","rasterBxpos",100,-5,5);
    TH1F* hrasterBypos =new TH1F("rasterBypos","rasterBypos",100,-5,5);
    // Fill Histograms here filling the Raster positions histograms   
    for (Int_t iev = 0 ; iev < totev ;iev ++){
      T->GetEntry(iev);
      if (ibcm1r>1){
	hrasterAxpos ->Fill(10.*rasterAxpos);
	hrasterAypos ->Fill(10.*rasterAypos);
	hrasterBxpos ->Fill(10.*rasterBxpos);
	hrasterBypos ->Fill(10.*rasterBypos);
      }
    }
    
    // TF1 *fit1 = (TF1 *)hbpmAxpos->GetFunction("gaus");
    // Double_t parameter0 = fit1->GetParameter(0);
    //cout << "parameter0 :"<<parameter0 << endl;
    
    cmeanr->cd(4*irr+1);
    hrasterAxpos->Draw();
    cmeanr->cd(4*irr+2);
    hrasterAypos->Draw();
    cmeanr->cd(4*irr+3);
    hrasterBxpos->Draw();
    cmeanr->cd(4*irr+4);
    hrasterBypos->Draw();
  
    cout <<" check raster" << endl; 


    rasterAxposmean[irr] = hrasterAxpos->GetMean();
    rasterAxposmeanErr[irr] = hrasterAxpos->GetRMS();
    cout <<"mean of x of raster A :" << rasterAxposmean[irr] << " +/- " << rasterAxposmeanErr[irr] << endl;
    rasterAyposmean[irr] = hrasterAypos->GetMean();
    rasterAyposmeanErr[irr] = hrasterAypos->GetRMS();
    cout <<"mean of y of raster A :" << rasterAyposmean[irr] << " +/- " << rasterAyposmeanErr[irr] << endl;
    rasterBxposmean[irr] = hrasterBxpos->GetMean();
    rasterBxposmeanErr[irr] = hrasterBxpos->GetRMS();
    cout <<"mean of x of raster B :" << rasterBxposmean[irr] << " +/- " << rasterBxposmeanErr[irr] << endl;
    rasterByposmean[irr] = hrasterBypos->GetMean();
    rasterByposmeanErr[irr] = hrasterBypos->GetRMS();
    cout <<"mean of y of raster B :" << rasterByposmean[irr] << " +/- " << rasterByposmeanErr[irr] << endl;
   
    cout << "******* ok raster*****" << endl;
    
  } // Loop over raster run ends here

  for (UInt_t ir = 0; ir < hAx.size(); ir ++){  //Loop over runs starts here

    err[ir] =0.0;
    
   
    //TFile *f = new TFile(Form("../hallc_replay/ROOTfiles/shms_replay_raster_simple_%d_-1.root",shms_run_NUM[ir]),"READ"); // %d : expects integer; %f expects float 
    
    TString file_format=gSystem->GetFromPipe("echo $hallc_replay_dir")+"/ROOTfiles/shms_replay_raster_simple_%d_-1.root";
    TFile *f = new TFile(Form(file_format,shms_run_NUM[ir]),"READ"); // %d : expects integer; %f expects float 
    TTree *T = (TTree*)f->Get("E");
    Int_t totev = T->GetEntries(); 
    //Read the branch for the BPM positions from the EPICS 
    T->SetBranchAddress("ibcm1",&ibcm1);
    T->SetBranchAddress("IPM3H07A.XRAW",&bpmAxpos);
    T->SetBranchAddress("IPM3H07A.YRAW",&bpmAypos);
    T->SetBranchAddress("IPM3H07B.XRAW",&bpmBxpos);
    T->SetBranchAddress("IPM3H07B.YRAW",&bpmBypos); 
    T->SetBranchAddress("IPM3H07C.XRAW",&bpmCxpos);
    T->SetBranchAddress("IPM3H07C.YRAW",&bpmCypos); 
    //Creating the histogram of the BPM positions and 
    TH1F* hbpmAxpos =new TH1F("bpmAxpos","bpmAxpos",100,hmin,hmax);
    TH1F* hbpmAypos =new TH1F("bpmAypos","bpmAypos",100,hmin,hmax);
    TH1F* hbpmBxpos =new TH1F("bpmBxpos","bpmBxpos",100,hmin,hmax);
    TH1F* hbpmBypos =new TH1F("bpmBypos","bpmBypos",100,hmin,hmax);
    TH1F* hbpmCxpos =new TH1F("bpmCxpos","bpmCxpos",100,hmin,hmax);
    TH1F* hbpmCypos =new TH1F("bpmCypos","bpmCypos",100,hmin,hmax);
    // Fill Histograms here filling the BPM positions histograms   
    for (Int_t iev = 0 ; iev < totev ;iev ++){
      T->GetEntry(iev);
      if (ibcm1>1){
	hbpmAxpos ->Fill(bpmAxpos);
	hbpmAypos ->Fill(bpmAypos);
	hbpmBxpos ->Fill(bpmBxpos);
	hbpmBypos ->Fill(bpmBypos);
	hbpmCxpos ->Fill(bpmCxpos);
	hbpmCypos ->Fill(bpmCypos);
      }
    }
    
    // TF1 *fit1 = (TF1 *)hbpmAxpos->GetFunction("gaus");
    // Double_t parameter0 = fit1->GetParameter(0);
    //cout << "parameter0 :"<<parameter0 << endl;
    
    cmean->cd(6*ir+1);
    //hbpmAxpos->Fit("gaus");
    hbpmAxpos->Draw();
    cmean->cd(6*ir+2);
    //hbpmAypos->Fit("gaus");
    hbpmAypos->Draw();
    cmean->cd(6*ir+3);
    //hbpmBxpos->Fit("gaus");
    hbpmBxpos->Draw();
    cmean->cd(6*ir+4);
    //hbpmBypos->Fit("gaus");
    hbpmBypos->Draw();
    cmean->cd(6*ir+5);
    //hbpmCxpos->Fit("gaus");
    hbpmCxpos->Draw();
    cmean->cd(6*ir+6);
    //hbpmCypos->Fit("gaus");
    hbpmCypos->Draw();
  
    cout <<" check " << endl; 


    //parameter(1) = mean of the gaussian functions   
    //bpmAxposmean[ir] = hbpmAxpos->GetFunction("gaus")->GetParameter(1);
    bpmAxposmean[ir] = hbpmAxpos->GetMean();
    bpmAxposmeanErr[ir] = hbpmAxpos->GetRMS();
    cout <<"mean of x of bpm A :" << bpmAxposmean[ir] << endl;
    //bpmAyposmean[ir] = hbpmAypos->GetFunction("gaus")->GetParameter(1);
    bpmAyposmean[ir] = hbpmAypos->GetMean();
    bpmAyposmeanErr[ir] = hbpmAypos->GetRMS();
    cout <<"mean of y of bpm A :" << bpmAyposmean[ir] << endl;
    //bpmBxposmean[ir]= hbpmBxpos->GetFunction("gaus")->GetParameter(1);
    bpmBxposmean[ir] = hbpmBxpos->GetMean();
    bpmBxposmeanErr[ir] = hbpmBxpos->GetRMS();
    cout <<"mean of x of bpm B :" << bpmBxposmean[ir] << endl;
    //bpmByposmean[ir] = hbpmBypos->GetFunction("gaus")->GetParameter(1);
    bpmByposmean[ir] = hbpmBypos->GetMean();
    bpmByposmeanErr[ir] = hbpmBypos->GetRMS();
    cout <<"mean of y of bpm B :" << bpmByposmean[ir] << endl;
    //bpmCxposmean[ir] = hbpmCxpos->GetFunction("gaus")->GetParameter(1);
    bpmCxposmean[ir] = hbpmCxpos->GetMean();
    bpmCxposmeanErr[ir] = hbpmCxpos->GetRMS();
    cout <<"mean of x of bpm C :" << bpmCxposmean[ir] << endl;
    //bpmCyposmean[ir] = hbpmCypos->GetFunction("gaus")->GetParameter(1);
    bpmCyposmean[ir] = hbpmCypos->GetMean();
    bpmCyposmeanErr[ir] = hbpmCypos->GetRMS();
    cout <<"mean of y of bpm C :" << bpmCyposmean[ir] << endl;
    
    cout << "******* ok *****" << endl;
    
  } // Loop over run ends here

  
  for (UInt_t ir = 0; ir < hAx.size(); ir ++){  //Second Loop over runs starts here

    err[ir] =0.0;
    
   
    //TFile *f = new TFile(Form("../hallc_replay/ROOTfiles/shms_replay_raster_simple_%d_-1.root",shms_run_NUM[ir]),"READ"); // %d : expects integer; %f expects float 
    
    TString file_format=gSystem->GetFromPipe("echo $hallc_replay_dir")+"/ROOTfiles/shms_replay_raster_simple_%d_-1.root";
    TFile *f = new TFile(Form(file_format,shms_run_NUM[ir]),"READ"); // %d : expects integer; %f expects float 
    TTree *T = (TTree*)f->Get("E");
    Int_t totev = T->GetEntries(); 
    //Read the branch for the BPM positions from the EPICS 
    T->SetBranchAddress("ibcm1",&ibcm1);
    T->SetBranchAddress("IPM3H07A.XRAW",&bpmAxpos);
    T->SetBranchAddress("IPM3H07A.YRAW",&bpmAypos);
    T->SetBranchAddress("IPM3H07B.XRAW",&bpmBxpos);
    T->SetBranchAddress("IPM3H07B.YRAW",&bpmBypos); 
    T->SetBranchAddress("IPM3H07C.XRAW",&bpmCxpos);
    T->SetBranchAddress("IPM3H07C.YRAW",&bpmCypos); 
    //Creating the histogram of the BPM positions and 
    TH1F* hbpmAxposc =new TH1F("bpmAxposc","bpmAxposc",100,hmin,hmax);
    TH1F* hbpmAyposc =new TH1F("bpmAyposc","bpmAyposc",100,hmin,hmax);
    TH1F* hbpmBxposc =new TH1F("bpmBxposc","bpmBxposc",100,hmin,hmax);
    TH1F* hbpmByposc =new TH1F("bpmByposc","bpmByposc",100,hmin,hmax);
    TH1F* hbpmCxposc =new TH1F("bpmCxposc","bpmCxposc",100,hmin,hmax);
    TH1F* hbpmCyposc =new TH1F("bpmCyposc","bpmCyposc",100,hmin,hmax);
    // Fill Histograms here filling the BPM positions histograms   
    for (Int_t iev = 0 ; iev < totev ;iev ++){
      T->GetEntry(iev);
      if (ibcm1>1){
	if (TMath::Abs(bpmAxposmean[ir]-bpmAxpos) < 1.5*bpmAxposmeanErr[ir] ) hbpmAxposc ->Fill(bpmAxpos);
	if (TMath::Abs(bpmAyposmean[ir]-bpmAypos) < 1.5*bpmAyposmeanErr[ir] ) hbpmAyposc ->Fill(bpmAypos);
	if (TMath::Abs(bpmBxposmean[ir]-bpmBxpos) < 1.5*bpmBxposmeanErr[ir] ) hbpmBxposc ->Fill(bpmBxpos);
	if (TMath::Abs(bpmByposmean[ir]-bpmBypos) < 1.5*bpmByposmeanErr[ir] ) hbpmByposc ->Fill(bpmBypos);
	if (TMath::Abs(bpmCxposmean[ir]-bpmCxpos) < 1.5*bpmCxposmeanErr[ir] ) hbpmCxposc ->Fill(bpmCxpos);
	if (TMath::Abs(bpmCyposmean[ir]-bpmCypos) < 1.5*bpmCyposmeanErr[ir] ) hbpmCyposc ->Fill(bpmCypos);
      }
    }
    
    // TF1 *fit1 = (TF1 *)hbpmAxpos->GetFunction("gaus");
    // Double_t parameter0 = fit1->GetParameter(0);
    //cout << "parameter0 :"<<parameter0 << endl;
    
    cmean2->cd(6*ir+1);
    //hbpmAxpos->Fit("gaus");
    hbpmAxposc->Draw();
    cmean2->cd(6*ir+2);
    //hbpmAypos->Fit("gaus");
    hbpmAyposc->Draw();
    cmean2->cd(6*ir+3);
    //hbpmBxpos->Fit("gaus");
    hbpmBxposc->Draw();
    cmean2->cd(6*ir+4);
    //hbpmBypos->Fit("gaus");
    hbpmByposc->Draw();
    cmean2->cd(6*ir+5);
    //hbpmCxpos->Fit("gaus");
    hbpmCxposc->Draw();
    cmean2->cd(6*ir+6);
    //hbpmCypos->Fit("gaus");
    hbpmCyposc->Draw();
  
    cout <<" check " << endl; 


    //parameter(1) = mean of the gaussian functions   
    //bpmAxposmean[ir] = hbpmAxpos->GetFunction("gaus")->GetParameter(1);
    bpmAxposmeanc[ir] = hbpmAxposc->GetMean();
    bpmAxposmeancErr[ir] = hbpmAxposc->GetRMS()/sqrt(hbpmAxposc->GetEntries());
    cout <<"mean of x of bpm A :" << bpmAxposmeanc[ir] << endl;
    //bpmAyposmean[ir] = hbpmAypos->GetFunction("gaus")->GetParameter(1);
    bpmAyposmeanc[ir] = hbpmAyposc->GetMean();
    bpmAyposmeancErr[ir] = hbpmAyposc->GetRMS()/sqrt(hbpmAyposc->GetEntries());
    cout <<"mean of y of bpm A :" << bpmAyposmeanc[ir] << endl;
    //bpmBxposmean[ir]= hbpmBxpos->GetFunction("gaus")->GetParameter(1);
    bpmBxposmeanc[ir] = hbpmBxposc->GetMean();
    bpmBxposmeancErr[ir] = hbpmBxposc->GetRMS()/sqrt(hbpmBxposc->GetEntries());
    cout <<"mean of x of bpm B :" << bpmBxposmeanc[ir] << endl;
    //bpmByposmean[ir] = hbpmBypos->GetFunction("gaus")->GetParameter(1);
    bpmByposmeanc[ir] = hbpmByposc->GetMean();
    bpmByposmeancErr[ir] = hbpmByposc->GetRMS()/sqrt(hbpmByposc->GetEntries());
    cout <<"mean of y of bpm B :" << bpmByposmeanc[ir] << endl;
    //bpmCxposmean[ir] = hbpmCxpos->GetFunction("gaus")->GetParameter(1);
    bpmCxposmeanc[ir] = hbpmCxposc->GetMean();
    bpmCxposmeancErr[ir] = hbpmCxposc->GetRMS()/sqrt(hbpmCxposc->GetEntries());
    cout <<"mean of x of bpm C :" << bpmCxposmeanc[ir] << endl;
    //bpmCyposmean[ir] = hbpmCypos->GetFunction("gaus")->GetParameter(1);
    bpmCyposmeanc[ir] = hbpmCyposc->GetMean();
    bpmCyposmeancErr[ir] = hbpmCyposc->GetRMS()/sqrt(hbpmCyposc->GetEntries());
    cout <<"mean of y of bpm C :" << bpmCyposmeanc[ir] << endl;
    
    cout << "******* ok *****" << endl;
  }  
  
  cout << "--------- ok -------" << endl;
  
  std:: vector<Double_t>  q0(size), q1(size);
  std:: vector<Double_t>  q0y(size), q1y(size);
  std:: vector<Double_t> bpmAx(size), bpmBx(size), bpmCx(size);
  std:: vector<Double_t> bpmAxErr(size), bpmBxErr(size), bpmCxErr(size);
  std:: vector<Double_t> bpmAy(size), bpmBy(size), bpmCy(size);
  std:: vector<Double_t> bpmAyErr(size), bpmByErr(size), bpmCyErr(size);
  std:: vector<Double_t> bpmAzz(size), bpmBzz(size), bpmCzz(size);
  std:: vector<Double_t> rasterAzz(size);
  std:: vector<Double_t> hxerr0(size), hxerr1(size);
  std:: vector<Double_t> hyerr0(size), hyerr1(size);

  Double_t ssize = sqrt(size);
  Int_t isize = ssize;
  Int_t jsize;
  if (size%isize == 0) {
          jsize = size/isize;
  }else{
          jsize = int(size/isize)+1;
  }

 TCanvas *ch = new TCanvas("ch","Hx vs Hz : Hall C", 800, 900);
  ch ->Divide(isize,jsize);
 TCanvas *chy = new TCanvas("chy","Hy vs Hz : Hall C", 800, 900);
  chy ->Divide(isize,jsize);
  
  for (Int_t i =0; i<hAx.size();i++){
    ch->cd(i+1);
    Double_t hx1[2] = {hAx[i],hBx[i]} ;
    // Double_t hBx1[1] = {hBx[i]} ;
    Double_t hz1[2] = {hAz[i],hBz[i]} ;
    // Double_t hBz1[1] = {hBz[i]} ;
    // cout << hBx[i] << endl;
    
    
    //  TCanvas *ch[i] = new TCanvas("ch","Hx vs Hz : Hall C", 800, 900);
    TGraphErrors *gr2 = new TGraphErrors(2,&hz1[0],&hx1[0],&err[0],&herr[0]);
    gr2->SetTitle("HAX vs HAZ ; HARP Z  ; HARP X");
    gr2->GetXaxis()->SetLimits(0.0,500.0);
    gr2->GetYaxis()->SetRangeUser(hmin,hmax);
    gr2->GetXaxis()->CenterTitle();
    gr2->GetYaxis()->CenterTitle();
    gr2->SetMarkerSize(0.95);
    gr2->SetMarkerStyle(20);
    gr2->SetMarkerColor(2);
    gr2->Draw("ape");
    gr2->Fit("pol1");
    q0[i] = gr2->GetFunction("pol1")->GetParameter(0);
    q1[i] = gr2->GetFunction("pol1")->GetParameter(1);
    hxerr0[i] = gr2->GetFunction("pol1")->GetParError(0);
    hxerr1[i] = gr2->GetFunction("pol1")->GetParError(1);
    // cout << q0[i] << endl;
    
   //  bpmAx[i] = (q1[i] * (bpmAz - hAz[i])) + hAx[i] ;
    //    bpmBx[i] = hBx[i] + (q1[i] * (bpmBz - hBz[i])) ;

    bpmBx[i] = (q1[i] * bpmBz ) + q0[i] ;
    bpmBxErr[i] = sqrt(hxerr1[i]*hxerr1[i]*bpmBz*bpmBz + hxerr0[i]*hxerr0[i]) ;
    bpmAx[i] = (q1[i] * bpmAz ) + q0[i] ;
    bpmAxErr[i] = sqrt(hxerr1[i]*hxerr1[i]*bpmAz*bpmAz + hxerr0[i]*hxerr0[i]) ;
    bpmCx[i] = (q1[i] * bpmCz ) + q0[i] ;
    bpmCxErr[i] = sqrt(hxerr1[i]*hxerr1[i]*bpmCz*bpmCz + hxerr0[i]*hxerr0[i]) ;
    cout << "check here for archive and epics variable comparison "<< endl;
    cout << bpmBx[i] << " :: " <<  bpmBx1[i] << endl;
    // cout << bpmBy[i] << " :: " <<  bpmBy1[i] << endl;
    // cout << bpmBy[i] << " :: " << bpmBy1[i] << endl;
    
  }
  
  for (Int_t i =0; i<hAy.size();i++){
    chy->cd(i+1);
    Double_t hy1[2] = {hAy[i],hBy[i]} ;
    // Double_t hBx1[1] = {hBx[i]} ;
    Double_t hz1[2] = {hAz[i],hBz[i]} ;
    // Double_t hBz1[1] = {hBz[i]} ;
    // cout << hBx[i] << endl;
    
    
    //  TCanvas *ch[i] = new TCanvas("ch","Hx vs Hz : Hall C", 800, 900);
    TGraphErrors *gr2y = new TGraphErrors(2,&hz1[0],&hy1[0],&err[0],&herr[0]);
    gr2y->SetTitle("HAY vs HAZ ; HARP Z  ; HARP Y");
    gr2y->GetXaxis()->SetLimits(0.0,500.0);
    gr2y->GetYaxis()->SetRangeUser(hmin,hmax);
    gr2y->GetXaxis()->CenterTitle();
    gr2y->GetYaxis()->CenterTitle();
    gr2y->SetMarkerSize(0.95);
    gr2y->SetMarkerStyle(20);
    gr2y->SetMarkerColor(2);
    gr2y->Draw("ape");
    gr2y->Fit("pol1");
    q0y[i] = gr2y->GetFunction("pol1")->GetParameter(0);
    q1y[i] = gr2y->GetFunction("pol1")->GetParameter(1);
    hyerr0[i] = gr2y->GetFunction("pol1")->GetParError(0);
    hyerr1[i] = gr2y->GetFunction("pol1")->GetParError(1);
    // cout << q0[i] << endl;
    
   //  bpmAx[i] = (q1[i] * (bpmAz - hAz[i])) + hAx[i] ;
    //    bpmBx[i] = hBx[i] + (q1[i] * (bpmBz - hBz[i])) ;

    bpmBy[i] = (q1y[i] * bpmBz ) + q0y[i] ;
    bpmByErr[i] = sqrt(hyerr1[i]*hyerr1[i]*bpmBz*bpmBz + hyerr0[i]*hyerr0[i]) ;
    bpmAy[i] = (q1y[i] * bpmAz ) + q0y[i] ;
    bpmAyErr[i] = sqrt(hyerr1[i]*hyerr1[i]*bpmAz*bpmAz + hyerr0[i]*hyerr0[i]) ;
    bpmCy[i] = (q1y[i] * bpmCz ) + q0y[i] ;
    bpmCyErr[i] = sqrt(hyerr1[i]*hyerr1[i]*bpmCz*bpmCz + hyerr0[i]*hyerr0[i]) ;
    cout << "check here for archive and epics variable comparison "<< endl;
    cout << bpmBy[i] << " :: " <<  bpmBy1[i] << endl;
    // cout << bpmBy[i] << " :: " <<  bpmBy1[i] << endl;
    // cout << bpmBy[i] << " :: " << bpmBy1[i] << endl;

  }
  
  for (Int_t i=0;i<size;i++){
    cout << "Checks on mean values from EPICS vs. archive values" << endl; 
    // cout <<  bpmAx[i] << "bpm Ax true value" << endl;
    cout <<  bpmAxposmeanc[i] << "  "<< bpmAx1[i] << endl;
    cout <<  bpmAyposmeanc[i] << "  "<< bpmAy1[i] << endl;
    cout <<  bpmBxposmeanc[i] << "  "<< bpmBx1[i] << endl;
    cout <<  bpmByposmeanc[i] << "  "<< bpmBy1[i] << endl;
    cout <<  bpmCxposmeanc[i] << "  "<< bpmCx1[i] << endl;
    cout <<  bpmCyposmeanc[i] << "  "<< bpmCy1[i] << endl;
    cout << endl;
  }


    TCanvas *cb = new TCanvas("cb","HARP vs BPM : Hall C", 800, 900);
    //Draw the bpm_measured vs bpm_cal
    cb->Divide(3,2);

    cb->cd(2);  
    //TGraph *gr3 = new TGraph(size,&bpmBx[0],&bpmBx1[0]);
    TGraphErrors *gr3 = new TGraphErrors(size,&bpmBxposmeanc[0],&bpmBx[0],&bpmBxposmeancErr[0],&bpmBxErr[0]);
    gr3->SetTitle("BPM B ; BPMBX  EPICS; BPMBX HARP");
    //gr1->GetXaxis()->SetLimits(-0.9,-0.1);
    // gr1->GetYaxis()->SetRangeUser(-0.83,-0.8);
    gr3->GetXaxis()->CenterTitle();
    gr3->GetYaxis()->CenterTitle();
    gr3->SetMarkerSize(0.85);
    gr3->SetMarkerStyle(20);
    gr3->SetMarkerColor(2);
    gr3->Draw("ape");
    gr3->Fit("pol1");
    Double_t bpmBx_p0=gr3->GetFunction("pol1")->GetParameter(0);
    Double_t bpmBx_p1=gr3->GetFunction("pol1")->GetParameter(1);
    Double_t bpmBx_e0=gr3->GetFunction("pol1")->GetParError(0);
    Double_t bpmBx_e1=gr3->GetFunction("pol1")->GetParError(1);

    
    //TCanvas *ca = new TCanvas("ca","HARP vs BPM : Hall C", 800, 900);
    //Draw the bpm_measured vs bpm_cal
    cb->cd(1);   
    TGraphErrors *gr4 = new TGraphErrors(size,&bpmAxposmeanc[0],&bpmAx[0],&bpmAxposmeancErr[0],&bpmAxErr[0]);
    gr4->SetTitle("BPM A ; BPMAX  EPICS; BPMAX HARP");
    //gr1->GetXaxis()->SetLimits(-0.9,-0.1);
    // gr1->GetYaxis()->SetRangeUser(-0.83,-0.8);
    gr4->GetXaxis()->CenterTitle();
    gr4->GetYaxis()->CenterTitle();
    gr4->SetMarkerSize(0.85);
    gr4->SetMarkerStyle(20);
    gr4->SetMarkerColor(2);
    gr4->Draw("ape");
    gr4->Fit("pol1");
    Double_t bpmAx_p0=gr4->GetFunction("pol1")->GetParameter(0);
    Double_t bpmAx_p1=gr4->GetFunction("pol1")->GetParameter(1);
    Double_t bpmAx_e0=gr4->GetFunction("pol1")->GetParError(0);
    Double_t bpmAx_e1=gr4->GetFunction("pol1")->GetParError(1);
    
    //TCanvas *cc = new TCanvas("cc","HARP vs BPM : Hall C", 800, 900);
    //Draw the bpm_measured vs bpm_cal
    cb->cd(3);   
    TGraphErrors *gr5 = new TGraphErrors(size,&bpmCxposmeanc[0],&bpmCx[0],&bpmCxposmeancErr[0],&bpmCxErr[0]);
    gr5->SetTitle("BPM C ; BPMCX  EPICS; BPMCX HARP");
    //gr1->GetXaxis()->SetLimits(-0.9,-0.1);
    // gr1->GetYaxis()->SetRangeUser(-0.83,-0.8);
    gr5->GetXaxis()->CenterTitle();
    gr5->GetYaxis()->CenterTitle();
    gr5->SetMarkerSize(0.85);
    gr5->SetMarkerStyle(20);
    gr5->SetMarkerColor(2);
    gr5->Draw("ape");
    gr5->Fit("pol1");
    Double_t bpmCx_p0=gr5->GetFunction("pol1")->GetParameter(0);
    Double_t bpmCx_p1=gr5->GetFunction("pol1")->GetParameter(1);
    Double_t bpmCx_e0=gr5->GetFunction("pol1")->GetParError(0);
    Double_t bpmCx_e1=gr5->GetFunction("pol1")->GetParError(1);
   
    //TCanvas *cby = new TCanvas("cby","HARP vs BPM : Hall C", 800, 900);
    //Draw the bpm_measured vs bpm_cal
    cb->cd(5);   
    //TGraph *gr3 = new TGraph(size,&bpmBx[0],&bpmBx1[0]);
    TGraphErrors *gr6 = new TGraphErrors(size,&bpmByposmeanc[0],&bpmBy[0],&bpmByposmeancErr[0],&bpmByErr[0]);
    gr6->SetTitle("BPM B ; BPMBY  EPICS; BPMBY HARP");
    //gr1->GetXaxis()->SetLimits(-0.9,-0.1);
    // gr1->GetYaxis()->SetRangeUser(-0.83,-0.8);
    gr6->GetXaxis()->CenterTitle();
    gr6->GetYaxis()->CenterTitle();
    gr6->SetMarkerSize(0.85);
    gr6->SetMarkerStyle(20);
    gr6->SetMarkerColor(2);
    gr6->Draw("ape");
    gr6->Fit("pol1");
    Double_t bpmBy_p0=gr6->GetFunction("pol1")->GetParameter(0);
    Double_t bpmBy_p1=gr6->GetFunction("pol1")->GetParameter(1);
    Double_t bpmBy_e0=gr3->GetFunction("pol1")->GetParError(0);
    Double_t bpmBy_e1=gr6->GetFunction("pol1")->GetParError(1);

    
    //TCanvas *cay = new TCanvas("cay","HARP vs BPM : Hall C", 800, 900);
    //Draw the bpm_measured vs bpm_cal
    cb->cd(4);  
    for (Int_t i =0; i<size; i++){
	   cout << bpmAy[i] << " +/- " << bpmAyErr[i] << " " << bpmAyposmeanc[i] << " +/- " << bpmAyposmeancErr[i] << endl;
    } 
    TGraphErrors *gr7 = new TGraphErrors(size,&bpmAyposmeanc[0],&bpmAy[0],&bpmAyposmeancErr[0],&bpmAyErr[0]);
    gr7->SetTitle("BPM A ; BPMAY  EPICS; BPMAY HARP");
    //gr1->GetXaxis()->SetLimits(-0.9,-0.1);
    // gr1->GetYaxis()->SetRangeUser(-0.83,-0.8);
    gr7->GetXaxis()->CenterTitle();
    gr7->GetYaxis()->CenterTitle();
    gr7->SetMarkerSize(0.85);
    gr7->SetMarkerStyle(20);
    gr7->SetMarkerColor(2);
    gr7->Draw("ape");
    gr7->Fit("pol1");
    Double_t bpmAy_p0=gr7->GetFunction("pol1")->GetParameter(0);
    Double_t bpmAy_p1=gr7->GetFunction("pol1")->GetParameter(1);
    Double_t bpmAy_e0=gr7->GetFunction("pol1")->GetParError(0);
    Double_t bpmAy_e1=gr7->GetFunction("pol1")->GetParError(1);
    
    //TCanvas *ccy = new TCanvas("ccy","HARP vs BPM : Hall C", 800, 900);
    //Draw the bpm_measured vs bpm_cal
    cb->cd(6);   
    TGraphErrors *gr8 = new TGraphErrors(size,&bpmCyposmeanc[0],&bpmCy[0],&bpmCyposmeancErr[0],&bpmCyErr[0]);
    gr8->SetTitle("BPM C ; BPMCY  EPICS; BPMCY HARP");
    //gr1->GetXaxis()->SetLimits(-0.9,-0.1);
    // gr1->GetYaxis()->SetRangeUser(-0.83,-0.8);
    gr8->GetXaxis()->CenterTitle();
    gr8->GetYaxis()->CenterTitle();
    gr8->SetMarkerSize(0.85);
    gr8->SetMarkerStyle(20);
    gr8->SetMarkerColor(2);
    gr8->Draw("ape");
    gr8->Fit("pol1");
    Double_t bpmCy_p0=gr8->GetFunction("pol1")->GetParameter(0);
    Double_t bpmCy_p1=gr8->GetFunction("pol1")->GetParameter(1);
    Double_t bpmCy_e0=gr8->GetFunction("pol1")->GetParError(0);
    Double_t bpmCy_e1=gr8->GetFunction("pol1")->GetParError(1);

// From BPM values with offsets
//
     bpmAx_p1 = -1.00111;
     bpmAx_p0 = -0.12309;
     bpmBx_p1 = -1.24023;
     bpmBx_p0 = -0.061674;
     bpmCx_p1 = -0.940987;
     bpmCx_p0 = -1.00727;
     bpmAy_p1 = 0.957734;
     bpmAy_p0 = -0.441772;
     bpmBy_p1 = 1.19394;
     bpmBy_p0 = 0.190904;
     bpmCy_p1 = 0.842774;
     bpmCy_p0 = 0.549787;
//
//
   
 TCanvas *chz = new TCanvas("chz","XPos vs zPos : Hall C", 800, 900);
  chz ->Divide(isize,jsize);
 TCanvas *chzz = new TCanvas("chzz","YPos vs zPos : Hall C", 800, 900);
  chzz ->Divide(isize,jsize);
  
  for (Int_t i =0; i<hAx.size();i++){
    bpmAzz[i] = bpmAz;
    bpmBzz[i] = bpmBz;
    bpmCzz[i] = bpmCz;
    rasterAzz[i] = 0.0;
    
    chz->cd(i+1);

    Double_t xx[5] = {hAx[i],hBx[i],bpmBx[i],bpmAx[i],bpmCx[i]};
    Double_t zz[5] = {hAz[i],hBz[i],bpmBzz[i],bpmAzz[i],bpmCzz[i]};
    Double_t xxx[3] = {bpmBx_p1*bpmBxposmeanc[i]+bpmBx_p0,bpmAx_p1*bpmAxposmeanc[i]+bpmAx_p0,bpmCx_p1*bpmCxposmeanc[i]+bpmCx_p0};
    Double_t zzz[3] = {bpmBzz[i],bpmAzz[i],bpmCzz[i]};
    Double_t xxxx[2] = {rasterAxposmean[i],rasterAyposmean[i]};
    Double_t exxxx[2] = {rasterAxposmeanErr[i],rasterAyposmeanErr[i]};
    Double_t zzzz[2] = {rasterAzz[i],rasterAzz[i]};
    Double_t ezzzz[2] = {0,0};

    TGraphErrors *gr21 = new TGraphErrors(5,&zz[0],&xx[0],&err[0],&herr[0]);
    gr21->SetTitle("HARP X vs Z ; Z  ; X");
    gr21->GetXaxis()->SetLimits(0.0,500.0);
    gr21->GetYaxis()->SetRangeUser(hmin,hmax);
    gr21->GetXaxis()->CenterTitle();
    gr21->GetYaxis()->CenterTitle();
    gr21->SetMarkerSize(0.95);
    gr21->SetMarkerStyle(20);
    gr21->SetMarkerColor(2);
    gr21->SetLineColor(2);
    gr21->Draw("ape");
    gr21->Fit("pol1");

    TGraph *gr211 = new TGraph(3,&zzz[0],&xxx[0]);
    gr211->GetXaxis()->SetLimits(0.0,500.0);
    gr211->GetYaxis()->SetRangeUser(hmin,hmax);
    gr211->GetXaxis()->CenterTitle();
    gr211->GetYaxis()->CenterTitle();
    gr211->SetMarkerSize(0.95);
    gr211->SetMarkerStyle(20);
    gr211->SetMarkerColor(3);
    gr211->SetLineColor(3);
    gr211->Draw("pS");
    gr211->Fit("pol1");
    
    TGraphErrors *gr2111 = new TGraphErrors(1,&zzzz[0],&xxxx[0],&ezzzz[0],&exxxx[0]);
    gr2111->GetXaxis()->SetLimits(0.0,500.0);
    gr2111->GetYaxis()->SetRangeUser(hmin,hmax);
    gr2111->GetXaxis()->CenterTitle();
    gr2111->GetYaxis()->CenterTitle();
    gr2111->SetMarkerSize(0.95);
    gr2111->SetMarkerStyle(20);
    gr2111->SetMarkerColor(1);
    gr2111->SetLineColor(1);
    gr2111->Draw("peS");
  }
  
  for (Int_t i =0; i<hAy.size();i++){
    
    chzz->cd(i+1);
    Double_t yy[5] = {hAy[i],hBy[i],bpmBy[i],bpmAy[i],bpmCy[i]};
    Double_t zz[5] = {hAz[i],hBz[i],bpmBzz[i],bpmAzz[i],bpmCzz[i]};
    Double_t yyy[3] = {bpmBy_p1*bpmByposmeanc[i]+bpmBy_p0,bpmAy_p1*bpmAyposmeanc[i]+bpmAy_p0,bpmCy_p1*bpmCyposmeanc[i]+bpmCy_p0};
    Double_t zzz[3] = {bpmBzz[i],bpmAzz[i],bpmCzz[i]};
    Double_t yyyy[2] = {rasterAxposmean[i],rasterAyposmean[i]};
    Double_t eyyyy[2] = {rasterAxposmeanErr[i],rasterAyposmeanErr[i]};
    Double_t zzzz[2] = {rasterAzz[i],rasterAzz[i]};
    Double_t ezzzz[2] = {0,0};

    TGraphErrors *gr24 = new TGraphErrors(5,&zz[0],&yy[0],&err[0],&herr[0]);
    gr24->SetTitle("HARP Y vs Z ; Z  ; X");
    gr24->GetXaxis()->SetLimits(0.0,500.0);
    gr24->GetYaxis()->SetRangeUser(hmin,hmax);
    gr24->GetXaxis()->CenterTitle();
    gr24->GetYaxis()->CenterTitle();
    gr24->SetMarkerSize(0.95);
    gr24->SetMarkerStyle(20);
    gr24->SetMarkerColor(2);
    gr24->SetLineColor(2);
    gr24->Draw("ape");
    gr24->Fit("pol1");

    TGraph *gr25 = new TGraph(3,&zzz[0],&yyy[0]);
    gr25->SetTitle("HARP Y vs Z ; Z  ; X");
    gr25->GetXaxis()->SetLimits(0.0,500.0);
    gr25->GetYaxis()->SetRangeUser(hmin,hmax);
    gr25->GetXaxis()->CenterTitle();
    gr25->GetYaxis()->CenterTitle();
    gr25->SetMarkerSize(0.95);
    gr25->SetMarkerStyle(20);
    gr25->SetMarkerColor(3);
    gr25->SetLineColor(3);
    gr25->Draw("pS");
    gr25->Fit("pol1");
    
    TGraphErrors *gr255 = new TGraphErrors(1,&zzzz[1],&yyyy[1],&ezzzz[1],&eyyyy[1]);
    gr255->SetTitle("HARP Y vs Z ; Z  ; X");
    gr255->GetXaxis()->SetLimits(0.0,500.0);
    gr255->GetYaxis()->SetRangeUser(hmin,hmax);
    gr255->GetXaxis()->CenterTitle();
    gr255->GetYaxis()->CenterTitle();
    gr255->SetMarkerSize(0.95);
    gr255->SetMarkerStyle(20);
    gr255->SetMarkerColor(1);
    gr255->SetLineColor(1);
    gr255->Draw("peS");

  }

    cout << "BPM Ax:  Slope = " << bpmAx_p1 << " +/- " << bpmAx_e1 << endl;
    cout << "BPM Ax:  Constant = " << bpmAx_p0 << " +/- " << bpmAx_e0 << endl;
    cout << endl;
    cout << "BPM Bx:  Slope = " << bpmBx_p1 << " +/- " << bpmBx_e1 << endl;
    cout << "BPM Bx:  Constant = " << bpmBx_p0 << " +/- " << bpmBx_e0 << endl;
    cout << endl;
    cout << "BPM Cx:  Slope = " << bpmCx_p1 << " +/- " << bpmCx_e1 << endl;
    cout << "BPM Cx:  Constant = " << bpmCx_p0 << " +/- " << bpmCx_e0 << endl;
    cout << endl;
  
    cout << "BPM Ay:  Slope = " << bpmAy_p1 << " +/- " << bpmAy_e1 << endl;
    cout << "BPM Ay:  Constant = " << bpmAy_p0 << " +/- " << bpmAy_e0 << endl;
    cout << endl;
    cout << "BPM By:  Slope = " << bpmBy_p1 << " +/- " << bpmBy_e1 << endl;
    cout << "BPM By:  Constant = " << bpmBy_p0 << " +/- " << bpmBy_e0 << endl;
    cout << endl;
    cout << "BPM Cy:  Slope = " << bpmCy_p1 << " +/- " << bpmCy_e1 << endl;
    cout << "BPM Cy:  Constant = " << bpmCy_p0 << " +/- " << bpmCy_e0 << endl;
    cout << endl;
  

}

