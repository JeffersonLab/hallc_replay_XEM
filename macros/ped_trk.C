

// static const Int_t numRuns    = 8;
static const Int_t numRuns    = 6;
static const Int_t numLayers  = 4;
// static const Int_t numPidLegs = 3;
static const Int_t numPidLegs = 2;

Int_t irun, ilayer, pid;
// Int_t runList[numRuns]   = {4681, 4709, 4745, 4786, 4817, 4828, 4876, 4900};
Int_t runList[numRuns]   = {4800, 4825, 4866, 4897, 4922, 4938};
// Int_t colorList[numRuns] = {1, 2, 4, 6, 7, 8, 9, 28}; 
Int_t colorList[numRuns] = {1, 2, 4, 6, 7, 8}; 
// Int_t fillList[numRuns]  = {3004, 3005, 3006, 3007, 3016, 3020, 3021, 3022};
// Int_t fillList[numRuns]  = {3003, 3003, 3003, 3003, 3003, 3003, 3003, 3003}; 
Int_t fillList[numRuns]  = {3003, 3003, 3003, 3003, 3003, 3003};
Double_t normFactor;

TH1F *layerHistos[numLayers];
TH1F *hASum[numRuns],    *hBSum[numRuns], *hCSum[numRuns], *hDSum[numRuns];
TH1F *hPreShwr[numRuns], *hShwr[numRuns], *hCerSum[numRuns], *pPreShwr[numRuns], *pHgcSum[numRuns];

TFile *rif[numRuns];

TCanvas *layerCan, *pidLegCan;

TLegend *layerLeg[numLayers], *pidLeg[numPidLegs];

void ped_trk() {

  layerCan = new TCanvas("layerCan", "layerCan", 1600, 1600);
  layerCan->Divide(2, 2);

  pidLegCan = new TCanvas("pidLegCan", "pidLegCan", 1600, 1600);
  pidLegCan->Divide(2, 1);
  
  for (ilayer = 0; ilayer < numLayers; ilayer++)
    layerLeg[ilayer] = new TLegend(0.1, 0.6, 0.2, 0.9);

  for (ipid = 0; ipid < numPidLegs; ipid++)
    pidLeg[ipid] = new TLegend(0.1, 0.6, 0.2, 0.9);


  for (irun = 0; irun < numRuns; irun++) {
    
    // rif[irun] = new TFile(Form("ROOTfiles/hms_coin_replay_production_%d_50000.root", runList[irun]), "READ");
    rif[irun] = new TFile(Form("ROOTfiles/shms_coin_replay_production_%d_50000.root", runList[irun]), "READ");
    
    // layerCan->cd(1);
    // hASum[irun] = dynamic_cast <TH1F*> (rif[irun]->FindObjectAny("htrig_hASum_adc_pped"));
    // hASum[irun]->SetLineColor(colorList[irun]);
    // hASum[irun]->SetFillColor(colorList[irun]);
    // hASum[irun]->SetFillStyle(fillList[irun]);
    // normFactor = hASum[irun]->GetEntries();
    // hASum[irun]->Scale(1./normFactor);
    // hASum[irun]->Draw("hist same");
    // hASum[irun]->GetYaxis()->SetRangeUser(0, 0.4);
    // layerLeg[0]->AddEntry(hASum[irun], Form("Run %d", runList[irun]));
    // layerLeg[0]->Draw();
    
    // layerCan->cd(2);
    // hBSum[irun] = dynamic_cast <TH1F*> (rif[irun]->FindObjectAny("htrig_hBSum_adc_pped"));
    // hBSum[irun]->SetLineColor(colorList[irun]);
    // hBSum[irun]->SetFillColor(colorList[irun]);
    // hBSum[irun]->SetFillStyle(fillList[irun]);
    // normFactor = hBSum[irun]->GetEntries();
    // hBSum[irun]->Scale(1./normFactor);
    // hBSum[irun]->Draw("hist same"); 
    // hBSum[irun]->GetYaxis()->SetRangeUser(0, 0.4);
    // layerLeg[1]->AddEntry(hBSum[irun], Form("Run %d", runList[irun]));
    // layerLeg[1]->Draw(); 

    // layerCan->cd(3);
    // hCSum[irun] = dynamic_cast <TH1F*> (rif[irun]->FindObjectAny("htrig_hCSum_adc_pped"));
    // hCSum[irun]->SetLineColor(colorList[irun]);
    // hCSum[irun]->SetFillColor(colorList[irun]);
    // hCSum[irun]->SetFillStyle(fillList[irun]);
    // normFactor = hCSum[irun]->GetEntries();
    // hCSum[irun]->Scale(1./normFactor);
    // hCSum[irun]->Draw("hist same");
    // hCSum[irun]->GetYaxis()->SetRangeUser(0, 0.4);
    // layerLeg[2]->AddEntry(hCSum[irun], Form("Run %d", runList[irun]));
    // layerLeg[2]->Draw();

    // layerCan->cd(4);
    // hDSum[irun] = dynamic_cast <TH1F*> (rif[irun]->FindObjectAny("htrig_hDSum_adc_pped"));
    // hDSum[irun]->SetLineColor(colorList[irun]);
    // hDSum[irun]->SetFillColor(colorList[irun]);
    // hDSum[irun]->SetFillStyle(fillList[irun]);
    // normFactor = hDSum[irun]->GetEntries();
    // hDSum[irun]->Scale(1./normFactor);
    // hDSum[irun]->Draw("hist same");  
    // hDSum[irun]->GetYaxis()->SetRangeUser(0, 0.4);
    // layerLeg[3]->AddEntry(hDSum[irun], Form("Run %d", runList[irun]));
    // layerLeg[3]->Draw();

    // pidLegCan->cd(1);
    // hPreShwr[irun] = dynamic_cast <TH1F*> (rif[irun]->FindObjectAny("htrig_hpreShwr_adc_pped"));
    // hPreShwr[irun]->SetLineColor(colorList[irun]);
    // hPreShwr[irun]->SetFillColor(colorList[irun]);
    // hPreShwr[irun]->SetFillStyle(fillList[irun]);
    // normFactor = hPreShwr[irun]->GetEntries();
    // hPreShwr[irun]->Scale(1./normFactor);
    // hPreShwr[irun]->Draw("hist same"); 
    // hPreShwr[irun]->GetYaxis()->SetRangeUser(0, 0.4);
    // pidLeg[0]->AddEntry(hPreShwr[irun], Form("Run %d", runList[irun]));
    // pidLeg[0]->Draw();    

    // pidLegCan->cd(2);
    // hShwr[irun] = dynamic_cast <TH1F*> (rif[irun]->FindObjectAny("htrig_hshwr_adc_pped"));
    // hShwr[irun]->SetLineColor(colorList[irun]);
    // hShwr[irun]->SetFillColor(colorList[irun]);
    // hShwr[irun]->SetFillStyle(fillList[irun]);
    // normFactor = hShwr[irun]->GetEntries();
    // hShwr[irun]->Scale(1./normFactor);
    // hShwr[irun]->Draw("hist same"); 
    // hShwr[irun]->GetYaxis()->SetRangeUser(0, 0.4);
    // pidLeg[1]->AddEntry(hShwr[irun], Form("Run %d", runList[irun]));
    // pidLeg[1]->Draw();

    // pidLegCan->cd(3);
    // hCerSum[irun] = dynamic_cast <TH1F*> (rif[irun]->FindObjectAny("htrig_hcersum_adc_pped"));
    // hCerSum[irun]->SetLineColor(colorList[irun]);
    // hCerSum[irun]->SetFillColor(colorList[irun]);
    // hCerSum[irun]->SetFillStyle(fillList[irun]);
    // normFactor = hCerSum[irun]->GetEntries();
    // hCerSum[irun]->Scale(1./normFactor);
    // hCerSum[irun]->Draw("hist same"); 
    // hCerSum[irun]->GetYaxis()->SetRangeUser(0, 0.4);
    // pidLeg[2]->AddEntry(hCerSum[irun], Form("Run %d", runList[irun]));
    // pidLeg[2]->Draw();

    pidLegCan->cd(1);
    pPreShwr[irun] = dynamic_cast <TH1F*> (rif[irun]->FindObjectAny("ptrig_prshwr_sum_pped"));
    pPreShwr[irun]->SetLineColor(colorList[irun]);
    pPreShwr[irun]->SetFillColor(colorList[irun]);
    pPreShwr[irun]->SetFillStyle(fillList[irun]);
    normFactor = pPreShwr[irun]->GetEntries();
    pPreShwr[irun]->Scale(1./normFactor);
    pPreShwr[irun]->Draw("hist same"); 
    pPreShwr[irun]->GetYaxis()->SetRangeUser(0, 0.5);
    pidLeg[0]->AddEntry(pPreShwr[irun], Form("Run %d", runList[irun]));
    pidLeg[0]->Draw();    

    pidLegCan->cd(2);
    pHgcSum[irun] = dynamic_cast <TH1F*> (rif[irun]->FindObjectAny("ptrig_phgc_sum_pped"));
    pHgcSum[irun]->SetLineColor(colorList[irun]);
    pHgcSum[irun]->SetFillColor(colorList[irun]);
    pHgcSum[irun]->SetFillStyle(fillList[irun]);
    normFactor = pHgcSum[irun]->GetEntries();
    pHgcSum[irun]->Scale(1./normFactor);
    pHgcSum[irun]->Draw("hist same"); 
    pHgcSum[irun]->GetYaxis()->SetRangeUser(0, 0.5);
    pidLeg[1]->AddEntry(pHgcSum[irun], Form("Run %d", runList[irun]));
    pidLeg[1]->Draw();

  }
}
