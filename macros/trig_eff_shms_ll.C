{
  gStyle->SetOptStat(0);

  TCanvas *can1 = new TCanvas("can1", "can1", 1600, 800);
  can1->Divide(3, 2);
  
  can1->cd(1);
  //gPad->SetLogy();

  T->Draw("P.hgcer.npeSum>>h1(100,0,50)", "P.hgcer.npeSum>0.0");
  T->Draw("P.hgcer.npeSum>>h2(100,0,50)", "P.hgcer.npeSum>0.0&&T.shms.pHGCSUM_tdcTimeRaw>0.0", "SAME");

  h1->SetLineColor(2);
  h1->SetFillColor(2);
  h1->SetFillStyle(3004);
  
  h2->SetLineColor(4);
  h2->SetFillColor(4);
  h2->SetFillStyle(3005);
  
  TLegend *leg1 = new TLegend(0.7,0.7,0.9,0.9);
  leg1->AddEntry(h1, "NPE Sum");
  leg1->AddEntry(h2, "CER");
  leg1->Draw();

  can1->cd(4);
  TH1F *h3 = (TH1F*)h1->Clone("h3");
  h3->Sumw2();
  h3->Divide(h2);
  h3->SetLineColor(2);
  h3->Draw();

  TLegend *leg2 = new TLegend(0.7,0.7,0.9,0.9);
  leg2->AddEntry(h3, "CER", "lep");
  leg2->Draw();

  //=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:

  can1->cd(2);
  //gPad->SetLogy();

  T->Draw("P.cal.etottracknorm>>h4(150,0,1.5)", "P.cal.etottracknorm>0.0");
  T->Draw("P.cal.etottracknorm>>h5(150,0,1.5)", "P.cal.etottracknorm>0.0&&T.shms.pPSHWRLO_tdcTimeRaw>0.0", "SAME");

  h4->SetLineColor(2);
  h4->SetFillColor(2);
  h4->SetFillStyle(3004);
  
  h5->SetLineColor(4);
  h5->SetFillColor(4);
  h5->SetFillStyle(3005);
  
  TLegend *leg3 = new TLegend(0.7,0.7,0.9,0.9);
  leg3->AddEntry(h4, "ETOT NORM");
  leg3->AddEntry(h5, "PRSHWRLO");
  leg3->Draw();

  can1->cd(5);
  TH1F *h6 = (TH1F*)h4->Clone("h6");
  h6->Sumw2();
  h6->Divide(h5);
  h6->SetLineColor(2);
  h6->Draw();

  TLegend *leg4 = new TLegend(0.7,0.7,0.9,0.9);
  leg4->AddEntry(h6, "PRSHWRLO", "lep");
  leg4->Draw();

 //=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:

  can1->cd(3);
  //gPad->SetLogy();

  T->Draw("P.cal.etottracknorm>>h7(150,0,1.5)", "P.cal.etottracknorm>0.0");
  T->Draw("P.cal.etottracknorm>>h8(150,0,1.5)", "P.cal.etottracknorm>0.0&&T.shms.pPSHWRHI_tdcTimeRaw>0.0", "SAME");

  h7->SetLineColor(2);
  h7->SetFillColor(2);
  h7->SetFillStyle(3004);
  
  h8->SetLineColor(4);
  h8->SetFillColor(4);
  h8->SetFillStyle(3005);
  
  TLegend *leg5 = new TLegend(0.7,0.7,0.9,0.9);
  leg5->AddEntry(h7, "ETOT NORM");
  leg5->AddEntry(h8, "PRSHWRHI");
  leg5->Draw();

  can1->cd(6);
  TH1F *h9 = (TH1F*)h7->Clone("h9");
  h9->Sumw2();
  h9->Divide(h8);
  h9->SetLineColor(2);
  h9->Draw();

  TLegend *leg6 = new TLegend(0.7,0.7,0.9,0.9);
  leg6->AddEntry(h9, "PRSHWRHI", "lep");
  leg6->Draw();

}
