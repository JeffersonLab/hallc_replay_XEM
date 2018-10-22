#include <iostream>
#include "TFile.h"
#include "TH1D.h"


void flyeye_occupancy(TString hist1name) {
  TH1F* H1;
  TH2F* cal2d;
  Int_t cal_nrows=16.;
  Int_t cal_ncols=14.;
  Int_t cal_dist_diff=9.;
  
  
  TString cal2dname=hist1name+"_2d";
  H1 = (TH1F*) gDirectory->Get(hist1name);
  cal2d = (TH2F*) gDirectory->Get(cal2dname);

  if (!cal2d) {
    //    cal2d = new TH2F("cal2d","SHMS Calormeter occupancy ; Y (cm) ;X (cm)",14,-63,63,16,-72,72);
        cal2d = new TH2F("cal2d","SHMS Shower  Cccupancy ; Row Number ;  Col Number",14,0,14,16,0,16);
     for (Int_t iblk = 0; iblk < (H1->GetSize()-2); iblk++) {
      Int_t col = iblk/cal_nrows+1;
      Int_t row = iblk%cal_nrows+1;
      Double_t Ypos_cal = (cal_ncols/2.-col)*cal_dist_diff;// horizontal position +Y left
      Double_t Xpos_cal = (cal_nrows/2.-row)*cal_dist_diff; //vertical position +X down
      //     cout << iblk << " " << col << " " << row<< " " << Ypos_cal<< " " << Xpos_cal << endl;
      cal2d->SetBinContent(col,row,H1->GetBinContent(iblk+1));
     }
 
  }
  cal2d->SetStats(0);
  cal2d->Draw("colz");
}
