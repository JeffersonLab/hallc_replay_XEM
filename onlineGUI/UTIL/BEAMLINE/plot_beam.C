void plot_beam(TString dir) {
  TH1F* h1d[4];
  if (dir=="Y") {
  h1d[0] = (TH1F*) gDirectory->Get("hBPMA_Y");
  h1d[1] = (TH1F*) gDirectory->Get("hBPMB_Y");
  h1d[2] = (TH1F*) gDirectory->Get("hBPMC_Y");
  h1d[3] = (TH1F*) gDirectory->Get("hBPMT_Y");
  } else {
  h1d[0] = (TH1F*) gDirectory->Get("hBPMA_X");
  h1d[1] = (TH1F*) gDirectory->Get("hBPMB_X");
  h1d[2] = (TH1F*) gDirectory->Get("hBPMC_X");
  h1d[3] = (TH1F*) gDirectory->Get("hBPMT_X");
  }
  Double_t mean_x[4];
     Double_t zbpm[4]={-320.82,-224.86,-129.44,0.};  
  for (Int_t nb=0;nb<4;nb++) {
    mean_x[nb]=h1d[nb]->GetMean();
  }
  TGraph *grbeamx = new TGraph(4,zbpm,mean_x);
  if (dir=="Y") grbeamx->SetTitle("; Z BPM (cm); Y BPM (cm) (+Y up) ");
  if (dir=="X") grbeamx->SetTitle("; Z BPM (cm); X BPM (cm) (+Y up) ");
    gPad->SetGridy();
  gPad->SetGridx();
  grbeamx->SetMarkerStyle(20);
  grbeamx->SetMarkerSize(1);
  grbeamx->GetXaxis()->SetLimits(-400,50);
  grbeamx->SetMinimum(-.5);
  grbeamx->SetMaximum(.5);
  grbeamx->Draw("AP");


  //
}
