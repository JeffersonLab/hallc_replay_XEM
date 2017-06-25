//Note: to use this script the calibration constants need to be entered manually


void Calibration_Visualization(Int_t RunNumber=0, Int_t EventNumber=-1)
{
  if (RunNumber == 0)
    {
      cout << "Enter a Run Number (-1 to exit): ";
      cin >> RunNumber;
      if (RunNumber <= 0) return;
      cout << "Enter number of events: ";
      cin >> EventNumber;
    }
  TFile *F = new TFile(Form("../root_files/shms_calibration_%d_%d.root", RunNumber, EventNumber));
  //Enter Calibration constants here
  Double_t xscale[4] = {541.092, 432.955, 377.598, 488.450};

  TH1F *hgc_e[4][4]; //[quadrant][pmt]
  TH1F *hgc_e_npe[4][4];
  TH1F *hgc_pi[4][4];
  TH1F *hgc_pi_npe[4][4];

  for (Int_t i=0; i<4; i++)
    {
      for (Int_t j=0; j<4; j++)
	{
	  hgc_e[i][j] = (TH1F*)F->Get(Form("phgc_quad%d_pulseInt_e%d", i+1, j+1));
	  hgc_e[i][j]->Rebin(10);
	  hgc_e_npe[i][j] = new TH1F(Form("hgc_e_npe_quad%d_e%d", i+1, j+1), "Diagonal NPE with electron cut", hgc_e[i][j]->GetXaxis()->GetNbins(), hgc_e[i][j]->GetXaxis()->GetXmin()/xscale[i], hgc_e[i][j]->GetXaxis()->GetXmax()/xscale[i]);
	  hgc_pi[i][j] = (TH1F*)F->Get(Form("phgc_quad%d_pulseInt_pi%d", i+1, j+1));
	  hgc_pi[i][j]->Rebin(10);
	  hgc_pi_npe[i][j] = new TH1F(Form("hgc_pi_npe_quad%d_pi%d", i+1, j+1), "Diagonal NPE with pion cut", hgc_pi[i][j]->GetXaxis()->GetNbins(), hgc_pi[i][j]->GetXaxis()->GetXmin()/xscale[i], hgc_pi[i][j]->GetXaxis()->GetXmax()/xscale[i]);
	  for (Int_t l=0; l<hgc_e[i][j]->GetXaxis()->GetNbins(); l++)
	    {
	      Double_t y_e = hgc_e[i][j]->GetBinContent(l);
	      Double_t y_pi = hgc_pi[i][j]->GetBinContent(l);
	      hgc_e_npe[i][j]->SetBinContent(l,y_e);
	      hgc_pi_npe[i][j]->SetBinContent(l,y_pi);
	    }
	}
    }
  TList *od_e1 = new TList;
  od_e1->Add(hgc_e[1][0]), od_e1->Add(hgc_e[2][0]), od_e1->Add(hgc_e[3][0]);
  TList *od_e2 = new TList;
  od_e2->Add(hgc_e[0][1]), od_e2->Add(hgc_e[2][1]), od_e2->Add(hgc_e[3][1]);
  TList *od_e3 = new TList;
  od_e3->Add(hgc_e[0][2]), od_e3->Add(hgc_e[3][2]), od_e3->Add(hgc_e[1][2]);
  TList *od_e4 = new TList;
  od_e4->Add(hgc_e[1][3]), od_e4->Add(hgc_e[2][3]), od_e4->Add(hgc_e[0][3]);
  TList *od_pi1 = new TList;
  od_pi1->Add(hgc_pi[1][0]), od_pi1->Add(hgc_pi[2][0]), od_pi1->Add(hgc_pi[3][0]);
  TList *od_pi2 = new TList;
  od_pi2->Add(hgc_pi[0][1]), od_pi2->Add(hgc_pi[2][1]), od_pi2->Add(hgc_pi[3][1]);
  TList *od_pi3 = new TList;
  od_pi3->Add(hgc_pi[0][2]), od_pi3->Add(hgc_pi[3][2]), od_pi3->Add(hgc_pi[1][2]);
  TList *od_pi4 = new TList;
  od_pi4->Add(hgc_pi[1][3]), od_pi4->Add(hgc_pi[2][3]), od_pi4->Add(hgc_pi[0][3]);  
  TList *all_e1 = new TList;
  all_e1->Add(hgc_e[0][0]), all_e1->Add(hgc_e[1][0]), all_e1->Add(hgc_e[2][0]), all_e1->Add(hgc_e[3][0]);
  TList *all_e2 = new TList;
  all_e2->Add(hgc_e[0][1]), all_e2->Add(hgc_e[1][1]), all_e2->Add(hgc_e[2][1]), all_e2->Add(hgc_e[3][1]);
  TList *all_e3 = new TList;
  all_e3->Add(hgc_e[0][2]), all_e3->Add(hgc_e[1][2]), all_e3->Add(hgc_e[2][2]), all_e3->Add(hgc_e[3][2]);
  TList *all_e4 = new TList;
  all_e4->Add(hgc_e[0][3]), all_e4->Add(hgc_e[1][3]), all_e4->Add(hgc_e[2][3]), all_e4->Add(hgc_e[3][3]);
  TList *all_pi1 = new TList;
  all_pi1->Add(hgc_pi[0][0]), all_pi1->Add(hgc_pi[1][0]), all_pi1->Add(hgc_pi[2][0]), all_pi1->Add(hgc_pi[3][0]);
  TList *all_pi2 = new TList;
  all_pi2->Add(hgc_pi[0][1]), all_pi2->Add(hgc_pi[1][1]), all_pi2->Add(hgc_pi[2][1]), all_pi2->Add(hgc_pi[3][1]);
  TList *all_pi3 = new TList;
  all_pi3->Add(hgc_pi[0][2]), all_pi3->Add(hgc_pi[1][2]), all_pi3->Add(hgc_pi[2][2]), all_pi3->Add(hgc_pi[3][2]);
  TList *all_pi4 = new TList;
  all_pi4->Add(hgc_pi[0][3]), all_pi4->Add(hgc_pi[1][3]), all_pi4->Add(hgc_pi[2][3]), all_pi4->Add(hgc_pi[3][3]);

  TList *full = new TList;
  for (Int_t i=0; i<4; i++)
    {
      for (Int_t j=0; j<4; j++)
	{
	  full->Add(hgc_e[i][j]), full->Add(hgc_pi[i][j]);
	}
    }

  TH1F *offd_e1 = (TH1F*)hgc_e[0][0]->Clone("offd_e1");
  offd_e1->Reset();
  offd_e1->Merge(od_e1);

  TH1F *offd_e2 = (TH1F*)hgc_e[0][1]->Clone("offd_e2");
  offd_e2->Reset();
  offd_e2->Merge(od_e2);

  TH1F *offd_e3 = (TH1F*)hgc_e[0][2]->Clone("offd_e3");
  offd_e3->Reset();
  offd_e3->Merge(od_e3);

  TH1F *offd_e4 = (TH1F*)hgc_e[0][3]->Clone("offd_e4");
  offd_e4->Reset();
  offd_e4->Merge(od_e4);

  TH1F *alld_e1 = (TH1F*)hgc_e[0][0]->Clone("alld_e1");
  alld_e1->Reset();
  alld_e1->Merge(all_e1);

  TH1F *alld_e2 = (TH1F*)hgc_e[0][1]->Clone("alld_e2");
  alld_e2->Reset();
  alld_e2->Merge(all_e2);

  TH1F *alld_e3 = (TH1F*)hgc_e[0][2]->Clone("alld_e3");
  alld_e3->Reset();
  alld_e3->Merge(all_e3);

  TH1F *alld_e4 = (TH1F*)hgc_e[0][3]->Clone("alld_e4");
  alld_e4->Reset();
  alld_e4->Merge(all_e4);

  TH1F *offd_pi1 = (TH1F*)hgc_pi[0][0]->Clone("offd_pi1");
  offd_pi1->Reset();
  offd_pi1->Merge(od_pi1);

  TH1F *offd_pi2 = (TH1F*)hgc_pi[0][1]->Clone("offd_pi2");
  offd_pi2->Reset();
  offd_pi2->Merge(od_pi2);

  TH1F *offd_pi3 = (TH1F*)hgc_pi[0][2]->Clone("offd_pi3");
  offd_pi3->Reset();
  offd_pi3->Merge(od_pi3);

  TH1F *offd_pi4 = (TH1F*)hgc_pi[0][3]->Clone("offd_pi4");
  offd_pi4->Reset();
  offd_pi4->Merge(od_pi4);

  TH1F *alld_pi1 = (TH1F*)hgc_pi[0][0]->Clone("alld_pi1");
  alld_pi1->Reset();
  alld_pi1->Merge(all_pi1);

  TH1F *alld_pi2 = (TH1F*)hgc_pi[0][1]->Clone("alld_pi2");
  alld_pi2->Reset();
  alld_pi2->Merge(all_pi2);

  TH1F *alld_pi3 = (TH1F*)hgc_pi[0][2]->Clone("alld_pi3");
  alld_pi3->Reset();
  alld_pi3->Merge(all_pi3);

  TH1F *alld_pi4 = (TH1F*)hgc_pi[0][3]->Clone("alld_pi4");
  alld_pi4->Reset();
  alld_pi4->Merge(all_pi4);

  TH1F *alld_full = (TH1F*)hgc_e[0][0]->Clone("alld_full");
  alld_full->Reset();
  alld_full->Merge(full);

  TH1F *offd_e1_npe, *offd_pi1_npe;
  TH1F *offd_e2_npe, *offd_pi2_npe;
  TH1F *offd_e3_npe, *offd_pi3_npe;
  TH1F *offd_e4_npe, *offd_pi4_npe;

  offd_e1_npe = new TH1F("offd_e1_npe", "Sum of Off Diagonal electrons", offd_e1->GetXaxis()->GetNbins(), offd_e1->GetXaxis()->GetXmin()/xscale[0], offd_e1->GetXaxis()->GetXmax()/xscale[0]);

  offd_e2_npe = new TH1F("offd_e2_npe", "Sum of Off Diagonal PMT2", offd_e2->GetXaxis()->GetNbins(), offd_e2->GetXaxis()->GetXmin()/xscale[1], offd_e2->GetXaxis()->GetXmax()/xscale[1]);

  offd_e3_npe = new TH1F("offd_e3_npe", "Sum of Off Diagonal PMT3", offd_e3->GetXaxis()->GetNbins(), offd_e3->GetXaxis()->GetXmin()/xscale[2], offd_e3->GetXaxis()->GetXmax()/xscale[2]);

  offd_e4_npe = new TH1F("offd_e4_npe", "Sum of Off Diagonal PMT4", offd_e4->GetXaxis()->GetNbins(), offd_e4->GetXaxis()->GetXmin()/xscale[3], offd_e4->GetXaxis()->GetXmax()/xscale[3]);

  offd_pi1_npe = new TH1F("offd_pi1_npe", "Sum of Off Diagonal pions", offd_pi1->GetXaxis()->GetNbins(), offd_pi1->GetXaxis()->GetXmin()/xscale[0], offd_pi1->GetXaxis()->GetXmax()/xscale[0]);

  offd_pi2_npe = new TH1F("offd_pi2_npe", "Sum of Off Diagonal PMT2", offd_pi2->GetXaxis()->GetNbins(), offd_pi2->GetXaxis()->GetXmin()/xscale[1], offd_pi2->GetXaxis()->GetXmax()/xscale[1]);

  offd_pi3_npe = new TH1F("offd_pi3_npe", "Sum of Off Diagonal PMT3", offd_pi3->GetXaxis()->GetNbins(), offd_pi3->GetXaxis()->GetXmin()/xscale[2], offd_pi3->GetXaxis()->GetXmax()/xscale[2]);

  offd_pi4_npe = new TH1F("offd_pi4_npe", "Sum of Off Diagonal PMT4", offd_pi4->GetXaxis()->GetNbins(), offd_pi4->GetXaxis()->GetXmin()/xscale[3], offd_pi4->GetXaxis()->GetXmax()/xscale[3]);


  for (Int_t l=0; l < offd_e1->GetXaxis()->GetNbins(); l++)
    {
      Double_t y_e_1 = offd_e1->GetBinContent(l);
      Double_t y_e_2 = offd_e2->GetBinContent(l);
      Double_t y_e_3 = offd_e3->GetBinContent(l);
      Double_t y_e_4 = offd_e4->GetBinContent(l);
      Double_t y_pi_1 = offd_pi1->GetBinContent(l);
      Double_t y_pi_2 = offd_pi2->GetBinContent(l);
      Double_t y_pi_3 = offd_pi3->GetBinContent(l);
      Double_t y_pi_4 = offd_pi4->GetBinContent(l);
      offd_e1_npe->SetBinContent(l,y_e_1);
      offd_e2_npe->SetBinContent(l,y_e_2);
      offd_e3_npe->SetBinContent(l,y_e_3);
      offd_e4_npe->SetBinContent(l,y_e_4);
      offd_pi1_npe->SetBinContent(l,y_pi_1);
      offd_pi2_npe->SetBinContent(l,y_pi_2);
      offd_pi3_npe->SetBinContent(l,y_pi_3);
      offd_pi4_npe->SetBinContent(l,y_pi_4);
    }

  TH1F *alld_e1_npe, *alld_pi1_npe;
  TH1F *alld_e2_npe, *alld_pi2_npe;
  TH1F *alld_e3_npe, *alld_pi3_npe;
  TH1F *alld_e4_npe, *alld_pi4_npe;

  alld_e1_npe = new TH1F("alld_e1_npe", "Sum of All electrons", alld_e1->GetXaxis()->GetNbins(), alld_e1->GetXaxis()->GetXmin()/xscale[0], alld_e1->GetXaxis()->GetXmax()/xscale[0]);

  alld_e2_npe = new TH1F("alld_e2_npe", "Sum of All Diagonal PMT2", alld_e2->GetXaxis()->GetNbins(), alld_e2->GetXaxis()->GetXmin()/xscale[1], alld_e2->GetXaxis()->GetXmax()/xscale[1]);

  alld_e3_npe = new TH1F("alld_e3_npe", "Sum of All Diagonal PMT3", alld_e3->GetXaxis()->GetNbins(), alld_e3->GetXaxis()->GetXmin()/xscale[2], alld_e3->GetXaxis()->GetXmax()/xscale[2]);

  alld_e4_npe = new TH1F("alld_e4_npe", "Sum of All Diagonal PMT4", alld_e4->GetXaxis()->GetNbins(), alld_e4->GetXaxis()->GetXmin()/xscale[3], alld_e4->GetXaxis()->GetXmax()/xscale[3]);

  alld_pi1_npe = new TH1F("alld_pi1_npe", "Sum of All pions", alld_pi1->GetXaxis()->GetNbins(), alld_pi1->GetXaxis()->GetXmin()/xscale[0], alld_pi1->GetXaxis()->GetXmax()/xscale[0]);

  alld_pi2_npe = new TH1F("alld_pi2_npe", "Sum of All Diagonal PMT2", alld_pi2->GetXaxis()->GetNbins(), alld_pi2->GetXaxis()->GetXmin()/xscale[1], alld_pi2->GetXaxis()->GetXmax()/xscale[1]);

  alld_pi3_npe = new TH1F("alld_pi3_npe", "Sum of All Diagonal PMT3", alld_pi3->GetXaxis()->GetNbins(), alld_pi3->GetXaxis()->GetXmin()/xscale[2], alld_pi3->GetXaxis()->GetXmax()/xscale[2]);

  alld_pi4_npe = new TH1F("alld_pi4_npe", "Sum of All Diagonal PMT4", alld_pi4->GetXaxis()->GetNbins(), alld_pi4->GetXaxis()->GetXmin()/xscale[3], alld_pi4->GetXaxis()->GetXmax()/xscale[3]);


  for (Int_t l=0; l < alld_e1->GetXaxis()->GetNbins(); l++)
    {
      Double_t y_e_1 = alld_e1->GetBinContent(l);
      Double_t y_e_2 = alld_e2->GetBinContent(l);
      Double_t y_e_3 = alld_e3->GetBinContent(l);
      Double_t y_e_4 = alld_e4->GetBinContent(l);
      Double_t y_pi_1 = alld_pi1->GetBinContent(l);
      Double_t y_pi_2 = alld_pi2->GetBinContent(l);
      Double_t y_pi_3 = alld_pi3->GetBinContent(l);
      Double_t y_pi_4 = alld_pi4->GetBinContent(l);
      alld_e1_npe->SetBinContent(l,y_e_1);
      alld_e2_npe->SetBinContent(l,y_e_2);
      alld_e3_npe->SetBinContent(l,y_e_3);
      alld_e4_npe->SetBinContent(l,y_e_4);
      alld_pi1_npe->SetBinContent(l,y_pi_1);
      alld_pi2_npe->SetBinContent(l,y_pi_2);
      alld_pi3_npe->SetBinContent(l,y_pi_3);
      alld_pi4_npe->SetBinContent(l,y_pi_4);
    }

  TCanvas *one = new TCanvas("one", "NPE for electrons and pions in diagonal only");
  one->Divide(2,1);
  one->cd(1);
  hgc_e_npe[0][0]->SetLineColor(2);
  hgc_e_npe[1][1]->SetLineColor(1);
  hgc_e_npe[2][2]->SetLineColor(4);
  hgc_e_npe[3][3]->SetLineColor(8);

  hgc_e_npe[0][0]->GetYaxis()->SetRangeUser(0,2000);
  hgc_e_npe[0][0]->Draw();
  hgc_e_npe[1][1]->Draw("same");
  hgc_e_npe[2][2]->Draw("same");
  hgc_e_npe[3][3]->Draw("same");

  auto legend1 = new TLegend(0.5,0.7,0.7,0.9);
  legend1->AddEntry("hgc_e_npe_quad1_e1","PMT 1","ep");
  legend1->AddEntry("hgc_e_npe_quad2_e2","PMT 2","ep");
  legend1->AddEntry("hgc_e_npe_quad3_e3","PMT 3","ep");
  legend1->AddEntry("hgc_e_npe_quad4_e4","PMT 4","ep");
  legend1->Draw();
  gPad->Modified();

  one->cd(2);
  hgc_pi_npe[0][0]->SetLineColor(2);
  hgc_pi_npe[1][1]->SetLineColor(1);
  hgc_pi_npe[2][2]->SetLineColor(4);
  hgc_pi_npe[3][3]->SetLineColor(8);

  hgc_pi_npe[0][0]->GetYaxis()->SetRangeUser(0,2000);
  hgc_pi_npe[0][0]->Draw();
  hgc_pi_npe[1][1]->Draw("same");
  hgc_pi_npe[2][2]->Draw("same");
  hgc_pi_npe[3][3]->Draw("same");

  auto legend2 = new TLegend(0.5,0.7,0.7,0.9);
  legend2->AddEntry("hgc_pi_npe_quad1_pi1","PMT 1","ep");
  legend2->AddEntry("hgc_pi_npe_quad2_pi2","PMT 2","ep");
  legend2->AddEntry("hgc_pi_npe_quad3_pi3","PMT 3","ep");
  legend2->AddEntry("hgc_pi_npe_quad4_pi4","PMT 4","ep");
  legend2->Draw();
  gPad->Modified();

  TCanvas *diagonal = new TCanvas("diagonal","NPE for electrons and pions off diagonals");
  diagonal->Divide(2,1);
  diagonal->cd(1);
  offd_e1_npe->SetLineColor(2);
  offd_e2_npe->SetLineColor(1);
  offd_e3_npe->SetLineColor(4);
  offd_e4_npe->SetLineColor(8);

  offd_e1_npe->Draw();
  offd_e2_npe->Draw("same");
  offd_e3_npe->Draw("same");
  offd_e4_npe->Draw("same");

  auto legend3 = new TLegend(0.5,0.7,0.7,0.9);
  legend3->AddEntry("offd_e1_npe","PMT 1","ep");
  legend3->AddEntry("offd_e2_npe","PMT 2","ep");
  legend3->AddEntry("offd_e3_npe","PMT 3","ep");
  legend3->AddEntry("offd_e4_npe","PMT 4","ep");
  legend3->Draw();
  gPad->Modified();

  diagonal->cd(2);
  offd_pi1_npe->SetLineColor(2);
  offd_pi2_npe->SetLineColor(1);
  offd_pi3_npe->SetLineColor(4);
  offd_pi4_npe->SetLineColor(8);

  offd_pi1_npe->Draw();
  offd_pi2_npe->Draw("same");
  offd_pi3_npe->Draw("same");
  offd_pi4_npe->Draw("same");

  auto legend4 = new TLegend(0.5,0.7,0.7,0.9);
  legend4->AddEntry("offd_pi1_npe","PMT 1","ep");
  legend4->AddEntry("offd_pi2_npe","PMT 2","ep");
  legend4->AddEntry("offd_pi3_npe","PMT 3","ep");
  legend4->AddEntry("offd_pi4_npe","PMT 4","ep");
  legend4->Draw();
  gPad->Modified();

  TCanvas *all = new TCanvas("all","NPE for electrons and pions everything");
  all->Divide(2,1);
  all->cd(1);
  alld_e1_npe->SetLineColor(2);
  alld_e2_npe->SetLineColor(1);
  alld_e3_npe->SetLineColor(4);
  alld_e4_npe->SetLineColor(8);

  alld_e1_npe->Draw();
  alld_e2_npe->Draw("same");
  alld_e3_npe->Draw("same");
  alld_e4_npe->Draw("same");

  auto legend5 = new TLegend(0.5,0.7,0.7,0.9);
  legend5->AddEntry("alld_e1_npe","PMT 1","ep");
  legend5->AddEntry("alld_e2_npe","PMT 2","ep");
  legend5->AddEntry("alld_e3_npe","PMT 3","ep");
  legend5->AddEntry("alld_e4_npe","PMT 4","ep");
  legend5->Draw();
  gPad->Modified();

  all->cd(2);
  alld_pi1_npe->SetLineColor(2);
  alld_pi2_npe->SetLineColor(1);
  alld_pi3_npe->SetLineColor(4);
  alld_pi4_npe->SetLineColor(8);

  alld_pi1_npe->Draw();
  alld_pi2_npe->Draw("same");
  alld_pi3_npe->Draw("same");
  alld_pi4_npe->Draw("same");

  auto legend6 = new TLegend(0.5,0.7,0.7,0.9);
  legend6->AddEntry("alld_pi1_npe","PMT 1","ep");
  legend6->AddEntry("alld_pi2_npe","PMT 2","ep");
  legend6->AddEntry("alld_pi3_npe","PMT 3","ep");
  legend6->AddEntry("alld_pi4_npe","PMT 4","ep");
  legend6->Draw();
  gPad->Modified();

}
