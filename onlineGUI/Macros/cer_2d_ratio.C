// UserScript.C
//
// Helper macro to build additional histograms
#include <TH2.h>
void UserScript()
{
  TTree *T=(TTree*)gDirectory->Get("T");
          Double_t ng_npesum;
	  T->SetBranchAddress("P.ngcer.npeSum", &ng_npesum);
          Double_t hg_npesum;
	  T->SetBranchAddress("P.hgcer.npeSum", &hg_npesum);
          Double_t etrack_norm;
	  T->SetBranchAddress("P.cal.etracknorm", &etrack_norm);
          Double_t x_fp;
	  T->SetBranchAddress("P.dc.x_fp", &x_fp);
          Double_t delta;
	  T->SetBranchAddress("P.gtr.dp", &delta);
          Double_t y_fp;
	  T->SetBranchAddress("P.dc.y_fp", &y_fp);
          Double_t xp_fp;
	  T->SetBranchAddress("P.dc.xp_fp", &xp_fp);
          Double_t yp_fp;
	  T->SetBranchAddress("P.dc.yp_fp", &yp_fp);
          TH2F* hng_xy_w_sumphe;
          TH2F* hng_xy_sumphe;
          TH2F* hng_xy;
          hng_xy_w_sumphe = new TH2F("hng_xy_w_sumphe"," NG Cer; Y at NG (cm); X at NG (cm)",40,-40,40,40,-40,40);
          hng_xy_sumphe = new TH2F("hng_xy_sumphe"," NG Cer (Z scale Total Npe); Y at NG (cm); X at NG (cm)",40,-40,40,40,-40,40);
          hng_xy = new TH2F("hng_xy"," NG Cer; Y at NG (cm); X at NG (cm)",40,-40,40,40,-40,40);
          hng_xy_elec = new TH2F("hng_xy_elec"," NG Cer; Y at NG (cm); X at NG (cm)",40,-40,40,40,-40,40);
          hng_xy_elec_eff = new TH2F("hng_xy_elec_eff"," NG Cer Eff; Y at NG (cm); X at NG (cm)",40,-40,40,40,-40,40);
          hhg_xy_w_sumphe = new TH2F("hhg_xy_w_sumphe"," HG Cer; Y at HG (cm); X at HG (cm)",60,-60,60,60,-60,60);
          hhg_xy_sumphe = new TH2F("hhg_xy_sumphe"," HG Cer (Z scale Total Npe); Y at HG (cm); X at HG (cm)",60,-60,60,60,-60,60);
          hhg_xy = new TH2F("hhg_xy"," HG Cer; Y at HG (cm); X at HG (cm)",60,-60,60,60,-60,60);
          hhg_xy_elec = new TH2F("hhg_xy_elec"," HG Cer; Y at HG (cm); X at HG (cm)",60,-60,60,60,-60,60);
          hhg_xy_elec_eff = new TH2F("hhg_xy_elec_eff"," HG Cer Eff; Y at HG (cm); X at HG (cm)",60,-60,60,60,-60,60);
  for(UInt_t iev = 0, N = T->GetEntries(); iev < N; iev++) {
    T->GetEntry(iev);
    if (iev%10000==0) cout << " iev = " << iev << endl;
    if ( etrack_norm > 0.8 && delta > -10 && delta < 22. ) {
    hng_xy_w_sumphe->Fill(y_fp-90*yp_fp,x_fp-90*xp_fp,ng_npesum);    
    hng_xy->Fill(y_fp-90*yp_fp,x_fp-90*xp_fp,1);    
    if (ng_npesum > .5) hng_xy_elec->Fill(y_fp-90*yp_fp,x_fp-90*xp_fp,1);    
    hhg_xy_w_sumphe->Fill(y_fp+156*yp_fp,x_fp+156*xp_fp,hg_npesum);    
    hhg_xy->Fill(y_fp+156*yp_fp,x_fp+156*xp_fp,1);    
    if (hg_npesum > .5) hhg_xy_elec->Fill(y_fp+156*yp_fp,x_fp+156*xp_fp,1);    
    }
  }
  hng_xy_sumphe->Divide(hng_xy_w_sumphe,hng_xy,1,1,"B");
  hhg_xy_sumphe->Divide(hhg_xy_w_sumphe,hhg_xy,1,1,"B");
  hng_xy_elec_eff->Divide(hng_xy_elec,hng_xy,1,1,"B");
  hhg_xy_elec_eff->Divide(hhg_xy_elec,hhg_xy,1,1,"B");
}
//
void cer_2d_ratio(TString histname){
  h2d = (TH2F*) gDirectory->Get(histname);
  if(!h2d) {
    UserScript();
    h2d = (TH2F*) gDirectory->Get(histname);
  }
  if (h2d) {
  h2d->SetStats(0);
  h2d->Draw("colz");
  //  h2d->SetMinimum(0);
  //h2d->SetMaximum(100);
    h2d->GetXaxis()->SetTitleOffset(.6);
    h2d->GetXaxis()->SetTitleSize(0.08);
    h2d->GetYaxis()->SetTitleOffset(.6);
    h2d->GetYaxis()->SetTitleSize(0.08);
  }
  //
}
