#include <iostream>
#include "TFile.h"
#include "TH1D.h"
#include "TKey.h"

using namespace std;

void comp_histos(TString old_file="",TString new_file="") {
  if (old_file=="") {
    cout << "Enter original version root file name: " << endl;
    cin >> old_file;
  }
  if (new_file=="") {
    cout << "Enter new version root file name: " << endl;
    cin >> new_file;
  }
  TFile* f1= new TFile(old_file,"READ");
  if (f1->IsZombie()) {
    cout << "Cannot find : " << old_file << endl;
    return;
  } 
  TFile* f2= new TFile(new_file,"READ");
  if (f2->IsZombie()) {
    cout << "Cannot find : " << new_file << endl;
    return;
  } 
  TIter nextkey1(f1->GetListOfKeys());
  TIter nextkey2(f2->GetListOfKeys());
  TKey *key1;
  TKey *key2;
  ofstream Matchoutfile("Output/hist_diff_Match_output.txt");
  ofstream NoMatchoutfile("Output/hist_diff_NoMatch_output.txt");
  Int_t cnt=0;
  Int_t igood;
  Double_t test_chi;
  Int_t test_ndf;
  TCanvas *can = new TCanvas("can"," ",800,800);
  while ( (key1= (TKey*)nextkey1()) && (key2= (TKey*)nextkey2())) {
    TClass *cl1= gROOT->GetClass(key1->GetClassName());
    TClass *cl2= gROOT->GetClass(key2->GetClassName());
    if ( cl1->InheritsFrom ("TH1") && cl2->InheritsFrom ("TH1") && !(cl1->InheritsFrom ("TH2") && cl2->InheritsFrom ("TH2"))) {
      TH1 *h1 = (TH1*)key1->ReadObj();
      TH1 *h2 = (TH1*)key2->ReadObj();
      if (h1->Integral()>0 && h2->Integral()>0) {
	h1->Chi2TestX(h2,test_chi,test_ndf,igood,"UU");
      if (test_chi >0) {
	NoMatchoutfile << " histogram " << key1->GetTitle() << " does not match chi2 = "  << test_chi<< " " << endl;
      can->Clear();
      can->Divide(1,1);
	can->cd(1);
       h1->Draw();
      h2->Draw("same");
      h2->SetLineColor(2);
      cnt++;
      if (cnt == 1) can->Print("Output/hist_diff_NoMatchoutput.pdf(");
      if (cnt >1) can->Print("Output/hist_diff_NoMatchoutput.pdf");
       can->Update();
       can->WaitPrimitive();
       //can->Clear();
      } else {
 	Matchoutfile << " histogram " << key1->GetTitle() << " matches"  << endl;
      }
      }
    }
    //
    if ( cl1->InheritsFrom ("TH2") && cl2->InheritsFrom ("TH2")) {
      TH2 *hh1 = (TH2*)key1->ReadObj();
      TH2 *hh2 = (TH2*)key2->ReadObj();
      if ( (hh1->Integral()>0 && hh2->Integral()>0)) {
	hh1->Chi2TestX(hh2,test_chi,test_ndf,igood,"UU");
	if (test_chi >1) {
	NoMatchoutfile << "2d  histogram " << key1->GetTitle() << " does not match chi2 = "  << test_chi<< " " << endl;
      can->Clear();
      can->Divide(1,2);
      can->cd(1);
       hh1->Draw("colz");
      can->cd(2);
       hh2->Draw("colz");
       can->Update();
       can->WaitPrimitive();
      cnt++;
      if (cnt == 1) can->Print("Output/hist_diff_NoMatchoutput.pdf(");
      if (cnt >1) can->Print("Output/hist_diff_NoMatchoutput.pdf");
       //can->Clear();
      } else {
 	Matchoutfile << " histogram " << key1->GetTitle() << " matches" << endl;
      }
      }
    }
    //
    //
  }
  //
  if (cnt >0) can->Print("Output/hist_diff_NoMatchoutput.pdf)");
  can->Close();
  if (cnt == 0) NoMatchoutfile << " All histograms matched. Congratulations !" <<  endl;
  if (cnt == 0) cout << " All histograms matched. Congratulations !" <<  endl;
  if (cnt > 0) NoMatchoutfile << " Some histograms did not matched. " <<  endl;
  if (cnt > 0) cout << " Some histograms did not matched. " <<  endl;
  cout << " Histograms with matched output are listed in Output/hist_diff_Match_output.txt" << endl;
  cout << " Histograms with unmatched output are listed in Output/hist_diff_NoMatch_output.txt"<< endl;
  //
}
