#include <TSystem.h>
#include <TString.h>
#include "TFile.h"
#include "TTree.h"
#include <TNtuple.h>
#include "TCanvas.h"
#include <iostream>
#include <fstream>
#include "TMath.h"
#include "TH1F.h"
#include <TH2.h>
#include <TStyle.h>
#include <TGraph.h>
#include <TROOT.h>
#include <TMath.h>
#include <TLegend.h>
#include <TPaveLabel.h>
#include <TProfile.h>
#include <TPolyLine.h>
#include <TObjArray.h>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include<math.h>
using namespace std;

void make_hist_shms_dc(TString pathBasename="",Int_t nrun=2043){
   if (pathBasename=="") {
     cout << " Input the pathBasename of the root file (assumed to be in worksim)" << endl;
     cin >> pathBasename;
   }
gStyle->SetPalette(1,0);
 gStyle->SetOptStat(1000011);
 gStyle->SetOptFit(11);
 gStyle->SetTitleOffset(1.,"Y");
 gStyle->SetTitleOffset(.7,"X");
 gStyle->SetLabelSize(0.04,"XY");
 gStyle->SetTitleSize(0.06,"XY");
 gStyle->SetPadLeftMargin(0.12);
   TString inputroot;
   inputroot=pathBasename+".root";
   TString outputhist;
   outputhist= pathBasename+"_shms_dc_hist.root";
 TObjArray HList(0);
     TString outputpdf;
    outputpdf=pathBasename+".pdf";
 TString htitle=pathBasename;
 TPaveLabel *title = new TPaveLabel(.15,.90,0.95,.99,htitle,"ndc");
 //
TFile *fsimc = new TFile(inputroot); 
TTree *tsimc = (TTree*) fsimc->Get("T");
// Define branches
 static const Int_t plnum=12;
 const char* plname[plnum]={"u1","u2","x1","x2","v1","v2","v2","v1","x2","x1","u2","u1"};
 const char* chname[plnum]={"1","1","1","1","1","1","2","2","2","2","2","2"};
 static const Int_t nwires[plnum]={107, 107, 79, 79, 107, 107,107, 107, 79, 79, 107, 107};
 Int_t nhits[plnum];
 Double_t resid[plnum];
 Double_t exclresid[plnum];
 Double_t dist[plnum][107];
 Double_t wire[plnum][107];
 Double_t time[plnum][107];
 tsimc->SetBranchAddress("P.dc.residual",resid) ;
 tsimc->SetBranchAddress("P.dc.residualExclPlane",exclresid) ;
 for (Int_t ip=0;ip<plnum;ip++) {
   tsimc->SetBranchAddress(Form("Ndata.P.dc.%s%s.dist",chname[ip],plname[ip]),&nhits[ip]) ;
   tsimc->SetBranchAddress(Form("P.dc.%s%s.dist",chname[ip],plname[ip]),&dist[ip]) ;
   tsimc->SetBranchAddress(Form("P.dc.%s%s.time",chname[ip],plname[ip]),&time[ip]) ;
   tsimc->SetBranchAddress(Form("P.dc.%s%s.wirenum",chname[ip],plname[ip]),&wire[ip]) ;
 }
 Double_t dist_best[plnum];
 tsimc->SetBranchAddress("P.dc.dist_best",dist_best) ;
 Double_t etotnorm;
   tsimc->SetBranchAddress("P.cal.etotnorm",&etotnorm) ;
 Double_t W;
   tsimc->SetBranchAddress("P.kin.W",&W) ;
 Double_t starttime;
   tsimc->SetBranchAddress("P.hod.starttime",&starttime) ;
 Double_t goodstarttime;
   tsimc->SetBranchAddress("P.hod.goodstarttime",&goodstarttime) ;
 Double_t npesum;
 if (nrun>=1721) {
   cout << " 3pass " << endl;
    tsimc->SetBranchAddress("P.ngcer.npeSum",&npesum);
 }
 if (nrun<1721) {
   cout << " Onepass  nrun= " << nrun << endl;
    tsimc->SetBranchAddress("P.hgcer.npeSum",&npesum);
 }
 Double_t sp1_id;
   tsimc->SetBranchAddress("P.dc.sp1_id",&sp1_id) ;
 Double_t ntrack;
   tsimc->SetBranchAddress("P.dc.ntrack",&ntrack) ;
 Double_t dcnsp;
   tsimc->SetBranchAddress("P.dc.nsp",&dcnsp) ;
 Double_t ndchit;
   tsimc->SetBranchAddress("P.dc.tnhit",&ndchit) ;
 Double_t sp2_id;
   tsimc->SetBranchAddress("P.dc.sp2_id",&sp2_id) ;
 Double_t xfp;
   tsimc->SetBranchAddress("P.dc.x_fp",&xfp) ;
 Double_t xpfp;
   tsimc->SetBranchAddress("P.dc.xp_fp",&xpfp) ;
 Double_t yfp;
   tsimc->SetBranchAddress("P.dc.y_fp",&yfp) ;
 Double_t ypfp;
   tsimc->SetBranchAddress("P.dc.yp_fp",&ypfp) ;
 Double_t nsp[2];
 Double_t nhits_ch[2];
  Double_t nhits_sp[2][200];
Double_t stub_x[2][200];
 Double_t stub_xp[2][200];
 Double_t stub_y[2][200];
 Double_t stub_yp[2][200];
 Double_t chi2_best;
 tsimc->SetBranchAddress("P.dc.chisq",&chi2_best) ;
   Int_t Unhits_ch[2];
   Double_t Upos_ch[2][100];
   Int_t Xnhits_ch[2];
   Double_t Xpos_ch[2][100];
   Int_t Vnhits_ch[2];
   Double_t Vpos_ch[2][100];
   Int_t UXnhits_ch[2];
   Double_t UXxpos_ch[2][100];
   Double_t UXypos_ch[2][100];
   Int_t VXnhits_ch[2];
   Double_t VXxpos_ch[2][100];
   Double_t VXypos_ch[2][100];
 const char* chname2[2]={"1","2"};
 for (Int_t ich=0;ich<2;ich++) {
   tsimc->SetBranchAddress(Form("Ndata.P.dc.Ch%s.U_pos",chname2[ich]),&Unhits_ch[ich]) ;
   tsimc->SetBranchAddress(Form("P.dc.Ch%s.U_pos",chname2[ich]),&Upos_ch[ich]) ;
   tsimc->SetBranchAddress(Form("Ndata.P.dc.Ch%s.X_pos",chname2[ich]),&Xnhits_ch[ich]) ;
   tsimc->SetBranchAddress(Form("P.dc.Ch%s.X_pos",chname2[ich]),&Xpos_ch[ich]) ;
   tsimc->SetBranchAddress(Form("Ndata.P.dc.Ch%s.V_pos",chname2[ich]),&Vnhits_ch[ich]) ;
   tsimc->SetBranchAddress(Form("P.dc.Ch%s.V_pos",chname2[ich]),&Vpos_ch[ich]) ;
   tsimc->SetBranchAddress(Form("Ndata.P.dc.Ch%s.UX_posx",chname2[ich]),&UXnhits_ch[ich]) ;
   tsimc->SetBranchAddress(Form("P.dc.Ch%s.UX_posx",chname2[ich]),&UXxpos_ch[ich]) ;
   tsimc->SetBranchAddress(Form("P.dc.Ch%s.UX_posy",chname2[ich]),&UXypos_ch[ich]) ;
   tsimc->SetBranchAddress(Form("Ndata.P.dc.Ch%s.VX_posx",chname2[ich]),&VXnhits_ch[ich]) ;
   tsimc->SetBranchAddress(Form("P.dc.Ch%s.VX_posx",chname2[ich]),&VXxpos_ch[ich]) ;
   tsimc->SetBranchAddress(Form("P.dc.Ch%s.VX_posy",chname2[ich]),&VXypos_ch[ich]) ;
   tsimc->SetBranchAddress(Form("P.dc.Ch%s.nhit",chname2[ich]),&nhits_ch[ich]) ;
   tsimc->SetBranchAddress(Form("P.dc.Ch%s.spacepoints",chname2[ich]),&nsp[ich]);
   tsimc->SetBranchAddress(Form("P.dc.Ch%s.stub_x",chname2[ich]),&stub_x[ich]) ;
   tsimc->SetBranchAddress(Form("P.dc.Ch%s.stub_xp",chname2[ich]),&stub_xp[ich]) ;
   tsimc->SetBranchAddress(Form("P.dc.Ch%s.stub_y",chname2[ich]),&stub_y[ich]) ;
   tsimc->SetBranchAddress(Form("P.dc.Ch%s.stub_yp",chname2[ich]),&stub_yp[ich]) ;
 }
 TString temp;
   // ntrack==0 histo
 TH1F *hstarttime_ntr0;
   temp=Form("Ntrck==0 Run %d ; Hod Starttime ; Counts",nrun);
   hstarttime_ntr0 = new TH1F("hstarttime_ntr0",temp,280,0,140);
   HList.Add(hstarttime_ntr0);
 TH1F *hndchit_ntr0;
   temp=Form("Ntrck==0 Run %d ; Number DC hits  ; Counts",nrun);
   hndchit_ntr0= new TH1F("hndchit_ntr0",temp,20,0,20.);
   HList.Add(hndchit_ntr0);
 TH1F *hndchit_ntr0_ecalcut;
   temp=Form("Ntrck==0 Run %d ; ecal>0.5 Number DC hits  ; Counts",nrun);
   hndchit_ntr0_ecalcut= new TH1F("hndchit_ntr0_ecalcut",temp,20,0,20.);
   HList.Add(hndchit_ntr0_ecalcut);
  TH1F *h_nsp_ntr0_ecalcut;
   temp=Form("Ntrck==0 Run %d ; ecal>0.5 Number DC sp  ; Counts",nrun);
   h_nsp_ntr0_ecalcut= new TH1F("h_nsp_ntr0_ecalcut",temp,20,0,20.);
   HList.Add(h_nsp_ntr0_ecalcut);
   TH1F *h_chisq = new TH1F("h_chisq",Form(" Run %d ; Chisq  ; Counts",nrun), 400, 0,100);
   HList.Add(h_chisq);
   TH1F *h_chisq_wide = new TH1F("h_chisq_wide",Form(" Run %d ; Chisq  ; Counts",nrun), 500, 0,1000);
   HList.Add(h_chisq_wide);
   // Define histograms
 TH1F* hetotnorm;
 TH2F* hetotnorm_npesum;
 TH1F* hxfp_resid_cut;
 TH1F* hyfp_resid_cut;
 TH1F* hndchit;
 TH2F* hstub1x_xp_diff;
 TH1F* hstub1_xp_diff;
 TH1F* hstub2_xp_diff;
 TH1F* hstubx_diff;
 TH1F* hstubx_diff_n12;
 TH1F* hstubx_diff_ngt12;
 TH1F* hstubxp_diff;
 TH1F* hstubxp_diff_n12;
 TH1F* hstubxp_diff_n11;
 TH1F* hstubxp_diff_ngt12;
 TH1F* hstuby_diff;
 TH1F* hstubyp_diff;
 TH1F* hwire[plnum];
 TH1F* hwire_residcut[plnum];
 TH1F* hdist[plnum];
 TH1F* htime[plnum];
 TH1F* hresid[plnum];
 TH1F* hexclresid[plnum];
 TH2F* hexclresid_chisq_cut_pl_0_1;
 TH2F* hexclresid_chisq_cut_pl_2_3;
 TH2F* hexclresid_chisq_cut_pl_4_5;
 TH2F* hexclresid_chisq_cut_pl_6_7;
 TH2F* hexclresid_chisq_cut_pl_8_9;
 TH2F* hexclresid_chisq_cut_pl_10_11;
 TH1F* hresid_cut[plnum];
 TH1F* hexclresid_cut[plnum];
 TH2F* hwire_time[plnum];
 TH2F* hwire_dist[plnum];
 TH2F* hwire_resid[plnum];
 TH2F* hdist_resid[plnum];
 TH2F* hstarttime_resid[plnum];
 TH2F* hstubxdiff_resid[plnum];
 TH2F* hstubydiff_resid[plnum];
 TH2F* hstubxpdiff_resid[plnum];
 TH2F* hstub1_xp_x;
 TH2F* hstub2_xp_x;
 TH2F* hstubxpdiff_xfp;
 TH2F* hstubxdiff_xfp;
 TH2F* hstubydiff_yfp;
 TH2F* hstubydiff_ypfp;
 TH2F* hstubxpdiff_xfp_cut;
 TH2F* hstubxdiff_xfp_cut;
 TH2F* htime_dist[plnum];
 TH1F* hch1_nsp;
 TH1F* hch2_nsp;
 TH2F* hnsp_dc_stub;
 TH1F* hstarttime;
 TH1F* hntrack;
 TH1F* hW;
 TH2F* hW_chisq;
 TH2F* hNhits_chisq;
 TH1F* hW_cut;
 TH1F* hW_cut1;
 TH1F* hW_cutlo;
 TH1F* h_dist2[2];
   temp=Form("Run %d ; Ch1 UX-VX cluster dist ;  ",nrun);
   h_dist2[0] = new TH1F("h_ch1_dist2",temp,2000,0,20);
   HList.Add(h_dist2[0]);
   temp=Form("Run %d ; Ch2  UX-VX cluster dist ;  ",nrun);
   h_dist2[1] = new TH1F("h_ch2_dist2",temp,2000,0,20);
   HList.Add(h_dist2[1]);
   temp=Form("Run %d ; Nhits ; chisq ",nrun);
   hNhits_chisq = new TH2F("hNhits_chisq",temp,100,0,100,500,0,1000);
   HList.Add(hNhits_chisq);
   temp=Form("Run %d ; W ; chisq ",nrun);
   hW_chisq = new TH2F("hW_chisq",temp,200,.85,1.1,500,0,1000);
   HList.Add(hW_chisq);
   temp=Form("Run %d ; W ; Counts",nrun);
   hW = new TH1F("hW",temp,200,.85,1.1);
   HList.Add(hW);
   temp=Form("Run %d ; W (chi2 >20); Counts",nrun);
   hW_cut = new TH1F("hW_cut",temp,200,.85,1.1);
   HList.Add(hW_cut);
   temp=Form("Run %d ; W (chi2 >100); Counts",nrun);
   hW_cut1 = new TH1F("hW_cut1",temp,200,.85,1.1);
   HList.Add(hW_cut1);
   temp=Form("Run %d ; W (chi2 <100); Counts",nrun);
   hW_cutlo = new TH1F("hW_cutlo",temp,200,.85,1.1);
   HList.Add(hW_cutlo);
   temp=Form("Run %d ; SHMS Ntracks ; Counts",nrun);
   hntrack = new TH1F("hntrack",temp,100,0,100);
   HList.Add(hntrack);
   temp=Form("Run %d ; Hod Starttime ; Counts",nrun);
   hstarttime = new TH1F("hstarttime",temp,280,0,140);
   HList.Add(hstarttime);
   temp=Form("Run %d ; X_fp Resid cut ; Counts",nrun);
   hxfp_resid_cut = new TH1F("hxfp_resid_cut",temp,100,-50,50);
   HList.Add(hxfp_resid_cut);
   temp=Form("Run %d ; Y_fp Resid cut ; Counts",nrun);
   hyfp_resid_cut = new TH1F("hyfp_resid_cut",temp,100,-50,50);
   HList.Add(hyfp_resid_cut);
   temp=Form("Run %d ; Etotnorm  ; Counts",nrun);
   hetotnorm = new TH1F("hetotnorm",temp,100,0,2.0);
   HList.Add(hetotnorm);
   if (nrun > 1721) {
   temp=Form("Run %d ; Etotnorm  ; NG npeSum",nrun);
   hetotnorm_npesum = new TH2F("hetotnorm_npesum",temp,150,0,1.5,250,0,50);
   } else {
   temp=Form("Run %d ; Etotnorm  ; HG npeSum",nrun);
   hetotnorm_npesum = new TH2F("hetotnorm_npesum",temp,150,0,1.5,60,0,15);
   }
   HList.Add(hetotnorm_npesum);
   temp=Form("Run %d ; Number DC hits  ; Counts",nrun);
   hndchit = new TH1F("hndchit",temp,20,0,20.);
   HList.Add(hndchit);
   temp=Form("Run %d ; DC Num spacepoints  ; Ch1 + Ch2 spacepoints",nrun);
   hnsp_dc_stub = new TH2F("hnsp_dc_stub",temp,20,0,20.,20,0,20.);
   HList.Add(hch1_nsp);
   temp=Form("Run %d ; Chamber 1 Num spacepoints  ; Counts",nrun);
   hch1_nsp = new TH1F("hch1_nsp",temp,20,0,20.);
   HList.Add(hch1_nsp);
   temp=Form("Run %d ; Chamber 2 Num spacepoints  ; Counts",nrun);
   hch2_nsp = new TH1F("hch2_nsp",temp,20,0,20.);
   HList.Add(hch2_nsp);
   temp=Form("Run %d ; Track Stub1 Xp -K*X  ; Track Stub1 X",nrun);
   hstub1x_xp_diff = new TH2F("hstub1x_xp_diff",temp,100,-1.,1.,100,-50.,50.);
   HList.Add(hstub1x_xp_diff);
   temp=Form("Run %d ; Track Stub1 Xp -K*X  ; Counts",nrun);
   hstub1_xp_diff = new TH1F("hstub1_xp_diff",temp,200,-1.,1.);
   HList.Add(hstub1_xp_diff);
   temp=Form("Run %d ; Track Stub2 Xp -K*X  ; Counts",nrun);
   hstub2_xp_diff = new TH1F("hstub2_xp_diff",temp,200,-1.,1.);
   HList.Add(hstub1_xp_diff);
   temp=Form("Run %d ; Track Stub1 Xp  ; Track Stub1 X",nrun);
   hstub1_xp_x = new TH2F("hstub1_xp_x",temp,100,-2.,2.,100,-50.,50.);
   HList.Add(hstub1_xp_x);
   temp=Form("Run %d ; Track Stub2 Xp  ; Track Stub2 X",nrun);
   hstub2_xp_x = new TH2F("hstub2_xp_x",temp,100,-2.,2.,100,-50.,50.);
   HList.Add(hstub2_xp_x);
   temp=Form("Run %d ; Stub Xp diff ; Xfp",nrun);
   hstubxpdiff_xfp = new TH2F("hstubxpdiff_Xfp",temp,100,-1.2,1.2,100,-50.,50.);
   HList.Add(hstubxpdiff_xfp);
   temp=Form("Run %d ; Stub X diff ; Xfp",nrun);
   hstubxdiff_xfp = new TH2F("hstubxdiff_Xfp",temp,100,-15.,15.,100,-50.,50.);
   HList.Add(hstubxdiff_xfp);
   temp=Form("Run %d ; Stub Xp diff ; Xfp",nrun);
   hstubxpdiff_xfp_cut = new TH2F("hstubxpdiff_cut_Xfp",temp,100,-1.2,1.2,100,-50.,50.);
   HList.Add(hstubxpdiff_xfp_cut);
   temp=Form("Run %d ; Stub X diff ; Xfp",nrun);
   hstubxdiff_xfp_cut = new TH2F("hstubxdiff_cut_Xfp",temp,100,-15.,15.,100,-50.,50.);
   HList.Add(hstubxdiff_xfp_cut);
   temp=Form("Run %d ; Stub Y diff ; Yfp",nrun);
   hstubydiff_yfp = new TH2F("hstubydiff_yfp",temp,100,-5.,5.,100,-40.,40.);
   HList.Add(hstubydiff_yfp);
   temp=Form("Run %d ; Stub Y diff ; Ypfp",nrun);
   hstubydiff_ypfp = new TH2F("hstubydiff_ypfp",temp,100,-5.,5.,100,-.05,.05);
   HList.Add(hstubydiff_ypfp);
 temp=Form("Run %d ; Stub X diff ; Counts",nrun);
   hstubx_diff = new TH1F("hstubx_diff",temp,200,-15.,15.);
   HList.Add(hstubx_diff);
   temp=Form("Run %d ; Stub Y diff ; Counts",nrun);
   hstuby_diff = new TH1F("hstuby_diff",temp,200,-10.,10.);
   HList.Add(hstuby_diff);
   temp=Form("Run %d ; Stub Xp diff ; Counts",nrun);
   hstubxp_diff = new TH1F("hstubxp_diff",temp,200,-1.,1.);
   HList.Add(hstubxp_diff);
   temp=Form("Run %d ; Stub Xp diff Nhits==12 ; Counts",nrun);
   hstubxp_diff_n12 = new TH1F("hstubxp_diff_n12",temp,200,-1.,1.);
   HList.Add(hstubxp_diff_n12);
   temp=Form("Run %d ; Stub Xp diff Nhits==11 ; Counts",nrun);
   hstubxp_diff_n11 = new TH1F("hstubxp_diff_n11",temp,200,-1.,1.);
   HList.Add(hstubxp_diff_n11);
   temp=Form("Run %d ; Stub Xp diff Nhits>12 ; Counts",nrun);
   hstubxp_diff_ngt12 = new TH1F("hstubxp_diff_ngt12",temp,200,-1.,1.);
   HList.Add(hstubxp_diff_ngt12);
   temp=Form("Run %d ; Stub Yp diff ; Counts",nrun);
   hstubyp_diff = new TH1F("hstubyp_diff",temp,200,-1.,1.);
   HList.Add(hstubyp_diff);
   temp=Form("Run %d Chisq cut ; Plane  %s%s Excl Resid  ; Plane  %s%s Excl Resid ",nrun,chname[0],plname[0],chname[1],plname[1]);
   hexclresid_chisq_cut_pl_0_1 = new TH2F(Form("exclresid_chisq_cut_%s%s_%s%s",chname[0],plname[0],chname[1],plname[1]),temp,200,-.5,.5,200,-.5,.5);
   HList.Add(hexclresid_chisq_cut_pl_0_1);
   temp=Form("Run %d Chisq cut ; Plane  %s%s Excl Resid  ; Plane  %s%s Excl Resid ",nrun,chname[2],plname[2],chname[3],plname[3]);
   hexclresid_chisq_cut_pl_2_3 = new TH2F(Form("exclresid_chisq_cut_%s%s_%s%s",chname[2],plname[2],chname[3],plname[3]),temp,200,-.5,.5,200,-.5,.5);
   HList.Add(hexclresid_chisq_cut_pl_2_3);
   temp=Form("Run %d Chisq cut ; Plane  %s%s Excl Resid  ; Plane  %s%s Excl Resid ",nrun,chname[4],plname[4],chname[5],plname[5]);
   hexclresid_chisq_cut_pl_4_5 = new TH2F(Form("exclresid_chisq_cut_%s%s_%s%s",chname[4],plname[4],chname[5],plname[5]),temp,200,-.5,.5,200,-.5,.5);
   HList.Add(hexclresid_chisq_cut_pl_4_5);
   temp=Form("Run %d Chisq cut ; Plane  %s%s Excl Resid  ; Plane  %s%s Excl Resid ",nrun,chname[6],plname[6],chname[7],plname[7]);
   hexclresid_chisq_cut_pl_6_7 = new TH2F(Form("exclresid_chisq_cut_%s%s_%s%s",chname[6],plname[6],chname[7],plname[7]),temp,200,-.5,.5,200,-.5,.5);
   HList.Add(hexclresid_chisq_cut_pl_6_7);
   temp=Form("Run %d Chisq cut ; Plane  %s%s Excl Resid  ; Plane  %s%s Excl Resid ",nrun,chname[8],plname[8],chname[9],plname[9]);
   hexclresid_chisq_cut_pl_8_9 = new TH2F(Form("exclresid_chisq_cut_%s%s_%s%s",chname[8],plname[8],chname[9],plname[9]),temp,200,-.5,.5,200,-.5,.5);
   HList.Add(hexclresid_chisq_cut_pl_8_9);
   temp=Form("Run %d Chisq cut ; Plane  %s%s Excl Resid  ; Plane  %s%s Excl Resid ",nrun,chname[10],plname[10],chname[11],plname[11]);
   hexclresid_chisq_cut_pl_10_11 = new TH2F(Form("exclresid_chisq_cut_%s%s_%s%s",chname[10],plname[10],chname[11],plname[11]),temp,200,-.5,.5,200,-.5,.5);
   HList.Add(hexclresid_chisq_cut_pl_10_11);
   for (Int_t ip=0;ip<plnum;ip++) {
   temp=Form("Run %d ; Plane  %s%s Resid ; Counts",nrun,chname[ip],plname[ip]);
   hresid[ip] = new TH1F(Form("resid_%s%s",chname[ip],plname[ip]),temp,200,-.5,.5);
   HList.Add(hresid[ip]);
   temp=Form("Run %d ; Plane  %s%s Excl Resid ; Counts",nrun,chname[ip],plname[ip]);
   hexclresid[ip] = new TH1F(Form("exclresid_%s%s",chname[ip],plname[ip]),temp,200,-.5,.5);
   HList.Add(hexclresid[ip]);
   temp=Form("Run %d ; Plane  %s%s Resid Cut ; Counts",nrun,chname[ip],plname[ip]);
   hresid_cut[ip] = new TH1F(Form("resid_cut_%s%s",chname[ip],plname[ip]),temp,200,-.5,.5);
   HList.Add(hresid[ip]);
   temp=Form("Run %d ; Plane  %s%s Excl Resid ; Counts",nrun,chname[ip],plname[ip]);
   hexclresid_cut[ip] = new TH1F(Form("exclresid_cut_%s%s",chname[ip],plname[ip]),temp,200,-.5,.5);
   HList.Add(hexclresid_cut[ip]);
   temp=Form("Run %d ; Plane  %s%s Wire  ; Counts",nrun,chname[ip],plname[ip]);
   hwire[ip] = new TH1F(Form("wire_%s%s",chname[ip],plname[ip]),temp,nwires[ip],0,nwires[ip]);
   HList.Add(hwire[ip]);
   temp=Form("Run %d Resid Cut ; Plane  %s%s Wire  ; Counts",nrun,chname[ip],plname[ip]);
   hwire_residcut[ip] = new TH1F(Form("wire_residcut_%s%s",chname[ip],plname[ip]),temp,nwires[ip],0,nwires[ip]);
   HList.Add(hwire_residcut[ip]);
   temp=Form("Run %d ; Plane  %s%s Dist ; Counts",nrun,chname[ip],plname[ip]);
   hdist[ip] = new TH1F(Form("dist_%s%s",chname[ip],plname[ip]),temp,120,0.,.6);
   HList.Add(hdist[ip]);
   temp=Form("Run %d ; Plane  %s%s Dist ; Counts",nrun,chname[ip],plname[ip]);
   htime[ip] = new TH1F(Form("time_%s%s",chname[ip],plname[ip]),temp,250,-50,250);
   HList.Add(htime[ip]);
  temp=Form("Run %d ; Plane  %s%s Wire ; Resid",nrun,chname[ip],plname[ip]);
   hwire_resid[ip] = new TH2F(Form("wire_resid_%s%s",chname[ip],plname[ip]),temp,nwires[ip],0,nwires[ip],200,-.2,.2);
   HList.Add(hwire_resid[ip]);
  temp=Form("Run %d ; Plane  %s%s Wire ; Resid",nrun,chname[ip],plname[ip]);
  hdist_resid[ip] = new TH2F(Form("dist_resid_%s%s",chname[ip],plname[ip]),temp,50,0,.5,50,-.5,.5);
   HList.Add(hwire_resid[ip]);
  temp=Form("Run %d ; Plane  %s%s Start time ; Resid",nrun,chname[ip],plname[ip]);
  hstarttime_resid[ip] = new TH2F(Form("starttime_resid_%s%s",chname[ip],plname[ip]),temp,280,0,140,200,-.2,.2);
   HList.Add(hstarttime_resid[ip]);
  temp=Form("Run %d ; Stub X diff ;Plane  %s%s  Resid",nrun,chname[ip],plname[ip]);
  hstubxdiff_resid[ip] = new TH2F(Form("stubxdiff_resid_%s%s",chname[ip],plname[ip]),temp,100,-20,20,200,-.2,.2);
   HList.Add(hstubxdiff_resid[ip]);
  temp=Form("Run %d ; Stub Xp diff ;  Plane  %s%s Resid",nrun,chname[ip],plname[ip]);
  hstubxpdiff_resid[ip] = new TH2F(Form("stubxpdiff_resid_%s%s",chname[ip],plname[ip]),temp,100,-1.,1.,200,-.2,.2);
   HList.Add(hstubxpdiff_resid[ip]);
  temp=Form("Run %d ; Stub Y diff ; Plane  %s%s Resid",nrun,chname[ip],plname[ip]);
  hstubydiff_resid[ip] = new TH2F(Form("stubydiff_resid_%s%s",chname[ip],plname[ip]),temp,100,-6.,6.,200,-.2,.2);
   HList.Add(hstubxpdiff_resid[ip]);
   temp=Form("Run %d ; Plane  %s%s Wire ; Time",nrun,chname[ip],plname[ip]);
   hwire_time[ip] = new TH2F(Form("wire_time_%s%s",chname[ip],plname[ip]),temp,nwires[ip],0,nwires[ip],125,-50,200);
   HList.Add(hwire_time[ip]);
   temp=Form("Run %d ; Plane  %s%s Wire ; Dist",nrun,chname[ip],plname[ip]);
   hwire_dist[ip] = new TH2F(Form("wire_dist_%s%s",chname[ip],plname[ip]),temp,nwires[ip],0,nwires[ip],120,0,.6);
   temp=Form("Run %d ; Plane  %s%s Time ; Dist",nrun,chname[ip],plname[ip]);
   htime_dist[ip] = new TH2F(Form("time_dist_%s%s",chname[ip],plname[ip]),temp,200,-50,250,120,0,.6);
   HList.Add(htime_dist[ip]);
 }

// loop over entries
 Int_t nch1;
 Int_t nch2;
 Double_t stub_xpdiff;
 Double_t stub_xdiff;
 Double_t stub_ypdiff;
 Double_t stub_ydiff;
Long64_t nentries = tsimc->GetEntries();
	for (int i = 0; i < nentries; i++) {
      		tsimc->GetEntry(i);
                if (i%10000==0) cout << " Entry = " << i << endl;
                nch1=0;
                nch2=0;
                stub_ydiff=-9999;
                stub_ypdiff=-9999;
                stub_xdiff=-9999;
                stub_xpdiff=-9999;
                for (Int_t ip=0;ip<plnum;ip++) {
		  if (ip<6&&nhits[ip]==1) nch1++;
		  if (ip>5&&nhits[ip]==1) nch2++;
		}
		hetotnorm->Fill(etotnorm);
		hetotnorm_npesum->Fill(etotnorm,npesum);
		if (ntrack==0) {
                  hstarttime_ntr0->Fill(starttime);
		  hndchit_ntr0->Fill(ndchit);
		  if (etotnorm>.6) {
                       hndchit_ntr0_ecalcut->Fill(ndchit);
		       if (ndchit>=10) h_nsp_ntr0_ecalcut->Fill(dcnsp);
                  }
		}
		if (etotnorm>.6 && goodstarttime)  hntrack->Fill(ntrack);
		if (etotnorm>.6 && goodstarttime && ntrack>=1 && nsp[0]>=1 && nsp[1]>=1) {
		  //
		for (Int_t nch=0;nch<2;nch++) {
		  Double_t dist2=0;
		for (Int_t it=0;it<UXnhits_ch[nch];it++) {
		  Double_t ux_posx=UXxpos_ch[nch][it];
		  Double_t ux_posy=UXypos_ch[nch][it];
		for (Int_t it2=0;it2<VXnhits_ch[nch];it2++) {
		  Double_t vx_posx=VXxpos_ch[nch][it2];
		  Double_t vx_posy=VXypos_ch[nch][it2];
                  dist2= pow(ux_posx-vx_posx,2) + pow(ux_posy-vx_posy,2) ;
		  h_dist2[nch]->Fill(sqrt(dist2));
		}}
		}
		  //
		  hW->Fill(W);
		  hW_chisq->Fill(W,chi2_best);
		  hNhits_chisq->Fill(float(nhits_ch[0]+nhits_ch[1]),chi2_best);
		  if (chi2_best > 20) hW_cut->Fill(W);
		  if (chi2_best > 100) hW_cut1->Fill(W);
		  if (chi2_best < 100) hW_cutlo->Fill(W);
                  hstarttime->Fill(starttime);
                  hnsp_dc_stub->Fill(dcnsp,nsp[0]+nsp[1]);
		  hch1_nsp->Fill(nsp[0]);
		  hch2_nsp->Fill(nsp[1]);
		  if (sp1_id <= nsp[0] && sp2_id <= nsp[1]) {
                     stub_ydiff=stub_y[0][int(sp1_id)]-stub_y[1][int(sp2_id)];
                     stub_ypdiff=stub_yp[0][int(sp1_id)]-stub_yp[1][int(sp2_id)];
                     stub_xdiff=stub_x[0][int(sp1_id)]-stub_x[1][int(sp2_id)];
                      stub_xpdiff=stub_xp[0][int(sp1_id)]-stub_xp[1][int(sp2_id)];
		    hstubx_diff->Fill(stub_xdiff);
		    hstuby_diff->Fill(stub_ydiff);
		    hstubxp_diff->Fill(stub_xpdiff);
		    if (nsp[0]==1 && nsp[1]==1) hstubxp_diff_n12->Fill(stub_xpdiff);
		    if (nsp[0]>1 && nsp[1]==1) hstubxp_diff_ngt12->Fill(stub_xpdiff);
		    if (nsp[0]==1 && nsp[1]>1) hstubxp_diff_n11->Fill(stub_xpdiff);
		    hstubyp_diff->Fill(stub_ypdiff);
		  }
                  hndchit->Fill(ndchit);
                  hstub1x_xp_diff->Fill(stub_xp[0][int(sp1_id)]-.0018*stub_x[0][int(sp1_id)],stub_x[0][int(sp1_id)]);
                  hstub1_xp_diff->Fill(stub_xp[0][int(sp1_id)]-.0018*stub_x[0][int(sp1_id)]);
                  hstub2_xp_diff->Fill(stub_xp[1][int(sp2_id)]-.0018*stub_x[1][int(sp2_id)]);
                 hstub1_xp_x->Fill(stub_xp[0][int(sp1_id)],stub_x[0][int(sp1_id)]);
                  hstub2_xp_x->Fill(stub_xp[1][int(sp2_id)],stub_x[1][int(sp2_id)]);
                  hstubxpdiff_xfp->Fill(stub_xpdiff,xfp);
		  hstubxdiff_xfp->Fill(stub_xdiff,xfp);
		  hstubydiff_yfp->Fill(stub_ydiff,yfp);
		  hstubydiff_ypfp->Fill(stub_ydiff,ypfp);
		  if (TMath::Abs(stub_xp[0][int(sp1_id)]-.0018*stub_x[0][int(sp1_id)]) > 0.2 || TMath::Abs(stub_xp[1][int(sp2_id)]-.0018*stub_x[1][int(sp2_id)]) > 0.2) {
                  hstubxpdiff_xfp_cut->Fill(stub_xpdiff,xfp);
		  hstubxdiff_xfp_cut->Fill(stub_xdiff,xfp);
		  }
 		  if (TMath::Abs(resid[2])>.1) {
		    hyfp_resid_cut->Fill(yfp);
		    hxfp_resid_cut->Fill(xfp);
		  }
                  if (1==1) {
		    hexclresid_chisq_cut_pl_0_1->Fill(exclresid[0],exclresid[1]);
		    hexclresid_chisq_cut_pl_2_3->Fill(exclresid[2],exclresid[3]);
		    hexclresid_chisq_cut_pl_4_5->Fill(exclresid[4],exclresid[5]);
		    hexclresid_chisq_cut_pl_6_7->Fill(exclresid[6],exclresid[7]);
		    hexclresid_chisq_cut_pl_8_9->Fill(exclresid[8],exclresid[9]);
		    hexclresid_chisq_cut_pl_10_11->Fill(exclresid[10],exclresid[11]);
		  }
                  h_chisq->Fill(chi2_best);
                  h_chisq_wide->Fill(chi2_best);
                  for (Int_t ip=0;ip<plnum;ip++) {
                  hresid[ip]->Fill(resid[ip]);
                  hexclresid[ip]->Fill(exclresid[ip]);
                  if (chi2_best>10) hresid_cut[ip]->Fill(resid[ip]);
                  if (chi2_best>10) hexclresid_cut[ip]->Fill(exclresid[ip]);
		  hstarttime_resid[ip]->Fill(starttime,resid[ip]);
                  hstubxdiff_resid[ip]->Fill(stub_xdiff,resid[ip]);
                  hstubxpdiff_resid[ip]->Fill(stub_xpdiff,resid[ip]);
                  hstubydiff_resid[ip]->Fill(stub_ydiff,resid[ip]);
                  hdist_resid[ip]->Fill(dist_best[ip],resid[ip]);
 		   for (Int_t id=0;id<nhits[ip];id++) {
		   if (id==0) hwire_resid[ip]->Fill(wire[ip][id],resid[ip]);
                   hwire[ip]->Fill(wire[ip][id]);
                   if (resid[ip] > 0.02) hwire_residcut[ip]->Fill(wire[ip][id]);
                   hdist[ip]->Fill(dist[ip][id]);
                   htime[ip]->Fill(time[ip][id]);
                   hwire_time[ip]->Fill(wire[ip][id],time[ip][id]);
                   hwire_dist[ip]->Fill(wire[ip][id],dist[ip][id]);
                   htime_dist[ip]->Fill(time[ip][id],dist[ip][id]);
		   }
	          }
 		} // if pid /event selection
	}
	//
 TFile hsimc(outputhist,"recreate");
	HList.Write();
//
}
