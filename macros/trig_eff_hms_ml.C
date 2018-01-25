{
  gStyle->SetOptStat(0);

  TCanvas *can1 = new TCanvas("can1", "can1", 1600, 800);
  
  can1->Divide(3, 2);
  can1->cd(1);
  gPad->SetLogy();

  T->Draw("H.cer.npeSum>>h1(100,0,50)", "H.cer.npeSum>0.0&&T.hms.hEL_LO_tdcTimeRaw>0.0");
  T->Draw("H.cer.npeSum>>h2(100,0,50)", "H.cer.npeSum>0.0&&T.hms.hEL_LO_CER_tdcTimeRaw>0.0", "SAME");
  T->Draw("H.cer.npeSum>>h3(100,0,50)", "H.cer.npeSum>0.0&&T.hms.hEL_HI_tdcTimeRaw>0.0", "SAME");

  h1->SetLineColor(2);
  h1->SetFillColor(2);
  h1->SetFillStyle(3004);
  
  h2->SetLineColor(4);
  h2->SetFillColor(4);
  h2->SetFillStyle(3005);
  
  h3->SetLineColor(8);
  h3->SetFillColor(8);
  h3->SetFillStyle(3006);
  
  TLegend *leg1 = new TLegend(0.7,0.7,0.9,0.9);
  leg1->AddEntry(h1, "NPE Sum EL-LO");
  leg1->AddEntry(h2, "NPE Sum EL-LO-CER");
  leg1->AddEntry(h3, "NPE Sum EL-HI");
  leg1->Draw();

  can1->cd(4);
  TH1F *h22 = (TH1F*)h1->Clone("h22");
  h22->Sumw2();
  h22->Divide(h2);
  h22->SetLineColor(2);
  h22->Draw();

  TH1F *h23 = (TH1F*)h1->Clone("h23");
  h23->Sumw2();
  h23->Divide(h3);
  h23->SetLineColor(4);
  h23->Draw("SAME");

  TH1F *h24 = (TH1F*)h2->Clone("h24");
  h24->Sumw2();
  h24->Divide(h3);
  h24->SetLineColor(8);
  h24->Draw("SAME");

  TLegend *leg8 = new TLegend(0.7,0.7,0.9,0.9);
  leg8->AddEntry(h22, "EL-LO / EL-LO-CER", "lep");
  leg8->AddEntry(h23, "EL-LO / EL-HI", "lep");
  leg8->AddEntry(h24, "EL-LO-CER / EL-HI", "lep");
  leg8->Draw();

  // =:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:

  can1->cd(2);
  gPad->SetLogy();

  T->Draw("H.cer.npe[0]>>h4(100,0,50)", "H.cer.npe[0]>0.0&&T.hms.hEL_LO_tdcTimeRaw>0.0");
  T->Draw("H.cer.npe[0]>>h5(100,0,50)", "H.cer.npe[0]>0.0&&T.hms.hEL_LO_CER_tdcTimeRaw>0.0", "SAME");
  T->Draw("H.cer.npe[0]>>h6(100,0,50)", "H.cer.npe[0]>0.0&&T.hms.hEL_HI_tdcTimeRaw>0.0", "SAME");

  h4->SetLineColor(2);
  h4->SetFillColor(2);
  h4->SetFillStyle(3004);

  h5->SetLineColor(4);
  h5->SetFillColor(4);
  h5->SetFillStyle(3005);

  h6->SetLineColor(8);
  h6->SetFillColor(8);
  h6->SetFillStyle(3006);

  TLegend *leg2 = new TLegend(0.7,0.7,0.9,0.9);
  leg2->AddEntry(h4, "PMT 1 EL-LO");
  leg2->AddEntry(h5, "PMT 1 EL-LO-CER");
  leg2->AddEntry(h6, "PMT 1 EL-HI");
  leg2->Draw();

  can1->cd(5);
  TH1F *h25 = (TH1F*)h4->Clone("h25");
  h25->Sumw2();
  h25->Divide(h5);
  h25->SetLineColor(2);
  h25->Draw();

  TH1F *h26 = (TH1F*)h4->Clone("h26");
  h26->Sumw2();
  h26->Divide(h6);
  h26->SetLineColor(4);
  h26->Draw("SAME");

  TH1F *h27 = (TH1F*)h5->Clone("h27");
  h27->Sumw2();
  h27->Divide(h6);
  h27->SetLineColor(8);
  h27->Draw("SAME");

  TLegend *leg9 = new TLegend(0.7,0.7,0.9,0.9);
  leg9->AddEntry(h25, "EL-LO / EL-LO-CER", "lep");
  leg9->AddEntry(h26, "EL-LO / EL-HI", "lep");
  leg9->AddEntry(h27, "EL-LO-CER / EL-HI", "lep");
  leg9->Draw();

  // =:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:

  can1->cd(3);
  gPad->SetLogy();

  T->Draw("H.cer.npe[1]>>h7(100,0,50)", "H.cer.npe[1]>0.0&&T.hms.hEL_LO_tdcTimeRaw>0.0");
  T->Draw("H.cer.npe[1]>>h8(100,0,50)", "H.cer.npe[1]>0.0&&T.hms.hEL_LO_CER_tdcTimeRaw>0.0", "SAME");
  T->Draw("H.cer.npe[1]>>h9(100,0,50)", "H.cer.npe[1]>0.0&&T.hms.hEL_HI_tdcTimeRaw>0.0", "SAME");

  h7->SetLineColor(2);
  h7->SetFillColor(2);
  h7->SetFillStyle(3004);

  h8->SetLineColor(4);
  h8->SetFillColor(4);
  h8->SetFillStyle(3005);

  h9->SetLineColor(8);
  h9->SetFillColor(8);
  h9->SetFillStyle(3006);

  TLegend *leg3 = new TLegend(0.7,0.7,0.9,0.9);
  leg3->AddEntry(h7, "PMT 2 EL-LO");
  leg3->AddEntry(h8, "PMT 2 EL-LO-CER");
  leg3->AddEntry(h9, "PMT 2 EL-HI");
  leg3->Draw();

  can1->cd(6);
  TH1F *h28 = (TH1F*)h7->Clone("h28");
  h28->Sumw2();
  h28->Divide(h8);
  h28->SetLineColor(2);
  h28->Draw();

  TH1F *h29 = (TH1F*)h7->Clone("h29");
  h29->Sumw2();
  h29->Divide(h9);
  h29->SetLineColor(4);
  h29->Draw("SAME");

  TH1F *h30 = (TH1F*)h8->Clone("h30");
  h30->Sumw2();
  h30->Divide(h9);
  h30->SetLineColor(8);
  h30->Draw("SAME");

  TLegend *leg11 = new TLegend(0.7,0.7,0.9,0.9);
  leg11->AddEntry(h28, "EL-LO / EL-LO-CER", "lep");
  leg11->AddEntry(h29, "EL-LO / EL-HI", "lep");
  leg11->AddEntry(h30, "EL-LO-CER / EL-HI", "lep");
  leg11->Draw();

  // =:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:
  // /-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-
  // =:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:


  TCanvas *can2 = new TCanvas("can2", "can2", 1600, 800);
  
  can2->Divide(3, 2);
  can2->cd(1);
  gPad->SetLogy();

  T->Draw("H.cal.eprtracknorm>>h10(75, 0, 1.5)", "H.cal.eprtracknorm>0.0&&T.hms.hEL_LO_tdcTimeRaw>0.0");
  T->Draw("H.cal.eprtracknorm>>h11(75, 0, 1.5)", "H.cal.eprtracknorm>0.0&&T.hms.hEL_LO_CER_tdcTimeRaw>0.0", "SAME");
  T->Draw("H.cal.eprtracknorm>>h12(75, 0, 1.5)", "H.cal.eprtracknorm>0.0&&T.hms.hEL_HI_tdcTimeRaw>0.0", "SAME");

  h10->SetLineColor(2);
  h10->SetFillColor(2);
  h10->SetFillStyle(3004);

  h11->SetLineColor(4);
  h11->SetFillColor(4);
  h11->SetFillStyle(3005);

  h12->SetLineColor(8);
  h12->SetFillColor(8);
  h12->SetFillStyle(3006);

  TLegend *leg4 = new TLegend(0.7,0.7,0.9,0.9);
  leg4->AddEntry(h10, "PrSh E-Norm EL-LO");
  leg4->AddEntry(h11, "PrSh E-Norm EL-LO-CER");
  leg4->AddEntry(h12, "PrSh E-Norm EL-HI");
  leg4->Draw();

  // =:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:

  can2->cd(2);
  gPad->SetLogy();

  T->Draw("H.cal.etracknorm>>h13(75, 0, 1.5)", "H.cal.etracknorm>0.0&&T.hms.hEL_LO_tdcTimeRaw>0.0");
  T->Draw("H.cal.etracknorm>>h14(75, 0, 1.5)", "H.cal.etracknorm>0.0&&T.hms.hEL_LO_CER_tdcTimeRaw>0.0", "SAME");
  T->Draw("H.cal.etracknorm>>h15(75, 0, 1.5)", "H.cal.etracknorm>0.0&&T.hms.hEL_HI_tdcTimeRaw>0.0", "SAME");

  h13->SetLineColor(2);
  h13->SetFillColor(2);
  h13->SetFillStyle(3004);

  h14->SetLineColor(4);
  h14->SetFillColor(4);
  h14->SetFillStyle(3005);

  h15->SetLineColor(8);
  h15->SetFillColor(8);
  h15->SetFillStyle(3006);

  TLegend *leg5 = new TLegend(0.7,0.7,0.9,0.9);
  leg5->AddEntry(h13, "Sh E-Norm EL-LO");
  leg5->AddEntry(h14, "Sh E-Norm EL-LO-CER");
  leg5->AddEntry(h15, "Sh E-Norm EL-HI");
  leg5->Draw();

  // =:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:

  can2->cd(3);
  gPad->SetLogy();

  T->Draw("H.cal.etottracknorm>>h31(75, 0, 1.5)", "H.cal.etottracknorm>0.0&&T.hms.hEL_LO_tdcTimeRaw>0.0");
  T->Draw("H.cal.etottracknorm>>h32(75, 0, 1.5)", "H.cal.etottracknorm>0.0&&T.hms.hEL_LO_CER_tdcTimeRaw>0.0", "SAME");
  T->Draw("H.cal.etottracknorm>>h33(75, 0, 1.5)", "H.cal.etottracknorm>0.0&&T.hms.hEL_HI_tdcTimeRaw>0.0", "SAME");

  h31->SetLineColor(2);
  h31->SetFillColor(2);
  h31->SetFillStyle(3004);

  h32->SetLineColor(4);
  h32->SetFillColor(4);
  h32->SetFillStyle(3005);

  h33->SetLineColor(8);
  h33->SetFillColor(8);
  h33->SetFillStyle(3006);

  TLegend *leg12 = new TLegend(0.7,0.7,0.9,0.9);
  leg12->AddEntry(h31, "Total E-Norm EL-LO");
  leg12->AddEntry(h32, "Total E-Norm EL-LO-CER");
  leg12->AddEntry(h33, "Total E-Norm EL-HI");
  leg12->Draw();

  // =:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:

  can2->cd(4);
  TH1F *h16 = (TH1F*)h10->Clone("h16");
  h16->Sumw2();
  h16->Divide(h11);
  h16->SetLineColor(2);
  h16->Draw();

  TH1F *h17 = (TH1F*)h10->Clone("h17");
  h17->Sumw2();
  h17->Divide(h12);
  h17->SetLineColor(4);
  h17->Draw("SAME");

  TH1F *h18 = (TH1F*)h11->Clone("h18");
  h18->Sumw2();
  h18->Divide(h12);
  h18->SetLineColor(8);
  h18->Draw("SAME");

  TLegend *leg6 = new TLegend(0.7,0.7,0.9,0.9);
  leg6->AddEntry(h16, "EL-LO / EL-LO-CER", "lep");
  leg6->AddEntry(h17, "EL-LO / EL-HI", "lep");
  leg6->AddEntry(h18, "EL-LO-CER / EL-HI", "lep");
  leg6->Draw();

  can2->cd(5);
  TH1F *h19 = (TH1F*)h13->Clone("h19");
  h19->Sumw2();
  h19->Divide(h14);
  h19->SetLineColor(2);
  h19->Draw();

  TH1F *h20 = (TH1F*)h13->Clone("h20");
  h20->Sumw2();
  h20->Divide(h15);
  h20->SetLineColor(4);
  h20->Draw("SAME");

  TH1F *h21 = (TH1F*)h14->Clone("h21");
  h21->Sumw2();
  h21->Divide(h15);
  h21->SetLineColor(8);
  h21->Draw("SAME");

  TLegend *leg7 = new TLegend(0.7,0.7,0.9,0.9);
  leg7->AddEntry(h19, "EL-LO / EL-LO-CER", "lep");
  leg7->AddEntry(h20, "EL-LO / EL-HI", "lep");
  leg7->AddEntry(h21, "EL-LO-CER / EL-HI", "lep");
  leg7->Draw();

  can2->cd(6);
  TH1F *h34 = (TH1F*)h31->Clone("h34");
  h34->Sumw2();
  h34->Divide(h32);
  h34->SetLineColor(2);
  h34->Draw();

  TH1F *h35 = (TH1F*)h31->Clone("h35");
  h35->Sumw2();
  h35->Divide(h33);
  h35->SetLineColor(4);
  h35->Draw("SAME");

  TH1F *h36 = (TH1F*)h32->Clone("h36");
  h36->Sumw2();
  h36->Divide(h34);
  h36->SetLineColor(8);
  h36->Draw("SAME");

  TLegend *leg13 = new TLegend(0.7,0.7,0.9,0.9);
  leg13->AddEntry(h34, "EL-LO / EL-LO-CER", "lep");
  leg13->AddEntry(h35, "EL-LO / EL-HI", "lep");
  leg13->AddEntry(h36, "EL-LO-CER / EL-HI", "lep");
  leg13->Draw();

}
