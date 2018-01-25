{
  gStyle->SetOptStat(0);
  
  TCanvas *can1 = new TCanvas("can1", "can1", 1600, 800);
  can1->Divide(4, 2);
  
  can1->cd(1);
  //gPad->SetLogy();

  T->Draw("H.cer.npeSum>>h1(100,0,25)", "H.cer.npeSum>0.0");
  T->Draw("H.cer.npeSum>>h2(100,0,25)", "H.cer.npeSum>0.0&&T.hms.hCERSUM_tdcTimeRaw>0.0", "SAME");

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

  can1->cd(5);
  TH1F *h3 = (TH1F*)h1->Clone("h3");
  h3->Sumw2();
  h3->Divide(h2);
  h3->SetLineColor(9);
  h3->SetMarkerStyle(21);
  h3->SetMarkerSize(0.5);
  h3->Draw();

  TLegend *leg2 = new TLegend(0.7,0.7,0.9,0.9);
  leg2->AddEntry(h3, "CER", "lep");
  leg2->Draw();

  TLine *line1 = new TLine(0.0, 1.0, 5.0, 1.0);
  line1->SetLineWidth(3);
  line1->SetLineStyle(2);
  line1->SetLineColor(6);
  line1->Draw();

  //=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:

  can1->cd(2);
  //gPad->SetLogy();

  T->Draw("H.cal.etottracknorm>>h4(75,0,1.5)", "H.cal.etottracknorm>0.0");
  T->Draw("H.cal.etottracknorm>>h5(75,0,1.5)", "H.cal.etottracknorm>0.0&&T.hms.hPSHWRLO_tdcTimeRaw>0.0", "SAME");

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

  can1->cd(6);
  TH1F *h6 = (TH1F*)h4->Clone("h6");
  h6->Sumw2();
  h6->Divide(h5);
  h6->SetLineColor(9);
  h6->SetMarkerStyle(21);
  h6->SetMarkerSize(0.5);
  h6->Draw();

  TLegend *leg4 = new TLegend(0.7,0.7,0.9,0.9);
  leg4->AddEntry(h6, "PRSHWRLO", "lep");
  leg4->Draw();

  TLine *line2 = new TLine(0.0, 1.0, 1.0, 1.0);
  line2->SetLineWidth(3);
  line2->SetLineStyle(2);
  line2->SetLineColor(6);
  line2->Draw();

 //=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:

  can1->cd(3);
  //gPad->SetLogy();

  T->Draw("H.cal.etottracknorm>>h7(75,0,1.5)", "H.cal.etottracknorm>0.0");
  T->Draw("H.cal.etottracknorm>>h8(75,0,1.5)", "H.cal.etottracknorm>0.0&&T.hms.hPSHWRHI_tdcTimeRaw>0.0", "SAME");

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

  can1->cd(7);
  TH1F *h9 = (TH1F*)h7->Clone("h9");
  h9->Sumw2();
  h9->Divide(h8);
  h9->SetLineColor(9);
  h9->SetMarkerStyle(21);
  h9->SetMarkerSize(0.5);
  h9->Draw();

  TLegend *leg6 = new TLegend(0.7,0.7,0.9,0.9);
  leg6->AddEntry(h9, "PRSHWRHI", "lep");
  leg6->Draw();

  TLine *line3 = new TLine(0.0, 1.0, 1.0, 1.0);
  line3->SetLineWidth(3);
  line3->SetLineStyle(2);
  line3->SetLineColor(6);
  line3->Draw();

 //=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:

  can1->cd(4);
  //gPad->SetLogy();

  T->Draw("H.cal.etottracknorm>>h10(75,0,1.5)", "H.cal.etottracknorm>0.0");
  T->Draw("H.cal.etottracknorm>>h11(75,0,1.5)", "H.cal.etottracknorm>0.0&&T.hms.hSHWR_tdcTimeRaw>0.0", "SAME");

  h10->SetLineColor(2);
  h10->SetFillColor(2);
  h10->SetFillStyle(3004);
  
  h11->SetLineColor(4);
  h11->SetFillColor(4);
  h11->SetFillStyle(3005);
  
  TLegend *leg7 = new TLegend(0.7,0.7,0.9,0.9);
  leg7->AddEntry(h10, "ETOT NORM");
  leg7->AddEntry(h11, "SHWRLO");
  leg7->Draw();

  can1->cd(8);
  TH1F *h12 = (TH1F*)h10->Clone("h12");
  h12->Sumw2();
  h12->Divide(h11);
  h12->SetLineColor(9);
  h12->SetMarkerStyle(21);
  h12->SetMarkerSize(0.5);
  h12->Draw();

  TLegend *leg8 = new TLegend(0.7,0.7,0.9,0.9);
  leg8->AddEntry(h12, "SHWRLO", "lep");
  leg8->Draw();

  TLine *line4 = new TLine(0.0, 1.0, 0.5, 1.0);
  line4->SetLineWidth(3);
  line4->SetLineStyle(2);
  line4->SetLineColor(6);
  line4->Draw();

}
