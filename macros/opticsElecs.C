{

  auto can = new TCanvas("can", "can", 800, 1600);
  can->Divide(1, 2);
  can->cd(1);
  T->Draw("P.gtr.y>>h1(100, -5., 5.)", "P.ngcer.npeSum > 2.0 && P.gtr.dp > -10.0 && P.gtr.dp < 22.0");
  can->cd(2);
  // T->Draw("P.cal.etottracknorm>>h2(150, 0.01, 1.5)", "abs(P.gtr.y) < 5. && P.gtr.dp > -10. && P.gtr.dp < 22. && P.ngcer.npeSum > 2.","");
  T->Draw("P.cal.etottracknorm>>h2(150, 0.01, 1.5)", "P.gtr.dp > -10. && P.gtr.dp < 22. && P.ngcer.npeSum > 2.","");
  
  Double_t numElecs, numElecsPerTrigger, numTrigsNeeded;
  Double_t numReplayedTrigs = 50000.;
  Double_t numWantedElecs   = 15000.;
  numElecs = h2->Integral(80, 120);
  cout << "Number of e-s from all optics foils =  " << numElecs << endl;
  numElecsPerTrigger = numElecs/numReplayedTrigs;
  numTrigsNeeded     = numWantedElecs/numElecsPerTrigger;
  cout << Form("Number of triggers required for %.0f e-s = ", numWantedElecs) << numTrigsNeeded << endl;

}
