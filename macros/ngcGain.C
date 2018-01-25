{

  TCanvas *can = new TCanvas("can", "", 1800, 800);
  can->Divide(2, 2);
 
 TCanvas *can2 = new TCanvas("can2", "", 1800, 800);
  can2->Divide(2, 2);

 TCanvas *can3 = new TCanvas("can3", "", 1800, 800);
  can3->Divide(2, 2);

 TCanvas *can4 = new TCanvas("can4", "", 1800, 800);
  can4->Divide(2, 2);
 TCanvas *can5 = new TCanvas("can5", "", 1800, 800);
  can5->Divide(2, 2);
  
  can->cd(1);
  T->Draw("P.ngcer.goodAdcPulseInt[0]>>pmt1(100, 0, 200)", "P.ngcer.goodAdcPulseInt[0]>0.0&&P.ngcer.numTracksMatched[0]>0.0&&P.cal.etracknorm>0.7&&P.ngcer.goodAdcPulseAmp[1]<10.&&P.ngcer.goodAdcPulseAmp[2]<10.&&P.ngcer.goodAdcPulseAmp[3]<10.&&P.ngcer.goodAdcTdcDiffTime[0]>-40.&&P.ngcer.goodAdcTdcDiffTime[0]<-10.");

  can->cd(2);
  T->Draw("P.ngcer.goodAdcPulseInt[1]>>pmt2(100, 0, 200)", "P.ngcer.goodAdcPulseInt[1]>0.0&&P.ngcer.numTracksMatched[1]>0.0&&P.cal.etracknorm>0.7&&P.ngcer.goodAdcPulseAmp[0]<10.&&P.ngcer.goodAdcPulseAmp[2]<10.&&P.ngcer.goodAdcPulseAmp[3]<10.&&P.ngcer.goodAdcTdcDiffTime[1]>-40.&&P.ngcer.goodAdcTdcDiffTime[1]<-10.");

  can->cd(3);
  T->Draw("P.ngcer.goodAdcPulseInt[2]>>pmt3(100, 0, 200)", "P.ngcer.goodAdcPulseInt[2]>0.0&&P.ngcer.numTracksMatched[2]>0.0&&P.cal.etracknorm>0.7&&P.ngcer.goodAdcPulseAmp[0]<10.&&P.ngcer.goodAdcPulseAmp[1]<10.&&P.ngcer.goodAdcPulseAmp[3]<10.&&P.ngcer.goodAdcTdcDiffTime[2]>-40.&&P.ngcer.goodAdcTdcDiffTime[2]<-10.");

  can->cd(4);
  T->Draw("P.ngcer.goodAdcPulseInt[3]>>pmt4(100, 0, 200)", "P.ngcer.goodAdcPulseInt[3]>0.0&&P.ngcer.numTracksMatched[3]>0.0&&P.cal.etracknorm>0.7&&P.ngcer.goodAdcPulseAmp[0]<10.&&P.ngcer.goodAdcPulseAmp[1]<10.&&P.ngcer.goodAdcPulseAmp[2]<10.&&P.ngcer.goodAdcTdcDiffTime[3]>-40.&&P.ngcer.goodAdcTdcDiffTime[3]<0.");

  can5->cd(1);
  T->Draw("P.ngcer.npe[0]>>npe1(100, 0, 100)", "P.ngcer.goodAdcPulseInt[0]>0.0&&P.ngcer.numTracksMatched[0]>0.0&&P.cal.etracknorm>0.7&&P.ngcer.goodAdcPulseAmp[1]<10.&&P.ngcer.goodAdcPulseAmp[2]<10.&&P.ngcer.goodAdcPulseAmp[3]<10.&&P.ngcer.goodAdcTdcDiffTime[0]>-40.&&P.ngcer.goodAdcTdcDiffTime[0]<-10.");

  can5->cd(2);
  T->Draw("P.ngcer.npe[1]>>npe2(100, 0, 100)", "P.ngcer.goodAdcPulseInt[1]>0.0&&P.ngcer.numTracksMatched[1]>0.0&&P.cal.etracknorm>0.7&&P.ngcer.goodAdcPulseAmp[0]<10.&&P.ngcer.goodAdcPulseAmp[2]<10.&&P.ngcer.goodAdcPulseAmp[3]<10.&&P.ngcer.goodAdcTdcDiffTime[1]>-40.&&P.ngcer.goodAdcTdcDiffTime[1]<-10.");

  can5->cd(3);
  T->Draw("P.ngcer.npe[2]>>npe3(100, 0, 100)", "P.ngcer.goodAdcPulseInt[2]>0.0&&P.ngcer.numTracksMatched[2]>0.0&&P.cal.etracknorm>0.7&&P.ngcer.goodAdcPulseAmp[0]<10.&&P.ngcer.goodAdcPulseAmp[1]<10.&&P.ngcer.goodAdcPulseAmp[3]<10.&&P.ngcer.goodAdcTdcDiffTime[2]>-40.&&P.ngcer.goodAdcTdcDiffTime[2]<-10.");

  can5->cd(4);
  T->Draw("P.ngcer.npe[3]>>npe4(100, 0, 100)", "P.ngcer.goodAdcPulseInt[3]>0.0&&P.ngcer.numTracksMatched[3]>0.0&&P.cal.etracknorm>0.7&&P.ngcer.goodAdcPulseAmp[0]<10.&&P.ngcer.goodAdcPulseAmp[1]<10.&&P.ngcer.goodAdcPulseAmp[2]<10.&&P.ngcer.goodAdcTdcDiffTime[3]>-40.&&P.ngcer.goodAdcTdcDiffTime[3]<0.");
  

  can2->cd(1);
  T->Draw("P.ngcer.goodAdcPulseAmp[0]>>pmt11(500, 0, 100)", "P.ngcer.goodAdcPulseInt[0]>0.0&&P.cal.etracknorm<0.4");

  can2->cd(2);
  T->Draw("P.ngcer.goodAdcPulseAmp[1]>>pmt21(500, 0, 100)", "P.ngcer.goodAdcPulseInt[1]>0.0&&P.cal.etracknorm<0.4");

  can2->cd(3);
  T->Draw("P.ngcer.goodAdcPulseAmp[2]>>pmt31(500, 0, 100)", "P.ngcer.goodAdcPulseInt[2]>0.0&&P.cal.etracknorm<0.4");

  can2->cd(4);
  T->Draw("P.ngcer.goodAdcPulseAmp[3]>>pmt41(500, 0, 100)", "P.ngcer.goodAdcPulseInt[3]>0.0&&P.cal.etracknorm<0.4");


  //Raw TDC Times
  can3->cd(1);
  T->Draw("P.ngcer.goodAdcTdcDiffTime[0]>>pmt111(200, -100, 100)", "P.ngcer.goodAdcPulseInt[0]>0.0&&P.ngcer.numTracksMatched[1]>0.0&&P.cal.etracknorm>0.7");

  can3->cd(2);
  T->Draw("P.ngcer.goodAdcTdcDiffTime[1]>>pmt211(200, -100, 100)", "P.ngcer.goodAdcPulseInt[1]>0.0&&P.ngcer.numTracksMatched[0]>0.0&&P.cal.etracknorm>0.7");

  can3->cd(3);
  T->Draw("P.ngcer.goodAdcTdcDiffTime[2]>>pmt311(200, -100, 100)", "P.ngcer.goodAdcPulseInt[2]>0.0&&P.ngcer.numTracksMatched[3]>0.0&&P.cal.etracknorm>0.7");

  can3->cd(4);
  T->Draw("P.ngcer.goodAdcTdcDiffTime[3]>>pmt411(200, -100, 100)", "P.ngcer.goodAdcPulseInt[3]>0.0&&P.ngcer.numTracksMatched[2]>0.0&&P.cal.etracknorm>0.7");

  can4->cd(1);
  T->Draw("P.ngcer.goodAdcPulseAmp[0]:10*T.shms.pNGCER_adcPulseAmp","T.shms.pNGCER_adcPulseAmp>0.&&P.ngcer.goodAdcPulseAmp[0]>0.0&&P.ngcer.numTracksMatched[0]>0.0&&P.cal.etracknorm>0.7","colz");
  can4->cd(2);
  T->Draw("P.ngcer.goodAdcPulseAmp[1]:10*T.shms.pNGCER_adcPulseAmp","T.shms.pNGCER_adcPulseAmp>0.&&P.ngcer.goodAdcPulseAmp[1]>0.0&&P.ngcer.numTracksMatched[1]>0.0&&P.cal.etracknorm>0.7","colz");
  can4->cd(3);
  T->Draw("P.ngcer.goodAdcPulseAmp[2]:10*T.shms.pNGCER_adcPulseAmp","T.shms.pNGCER_adcPulseAmp>0.&&P.ngcer.goodAdcPulseAmp[2]>0.0&&P.ngcer.numTracksMatched[2]>0.0&&P.cal.etracknorm>0.7","colz");
  can4->cd(4);
  T->Draw("P.ngcer.goodAdcPulseAmp[3]:10*T.shms.pNGCER_adcPulseAmp","T.shms.pNGCER_adcPulseAmp>0.&&P.ngcer.goodAdcPulseAmp[3]>0.0&&P.ngcer.numTracksMatched[3]>0.0&&P.cal.etracknorm>0.7","colz");

  // T.ngcer_adcPulseTime

}
