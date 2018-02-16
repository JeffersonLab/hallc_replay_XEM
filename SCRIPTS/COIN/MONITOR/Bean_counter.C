// Filename: Bean_counter.C
// Description: delta, xptar,yptar, ytar distributions for SHMS and HMS and W, Em, Pm and Cointime distributions 
// are plotted and the number of good counts are printed out to add to the count of good events.
// Needs runnumber, and if target is "h" or "c"

void Bean_counter(Int_t runNumber, Int_t targ){

 //read the input file from data
  TString fileNameD = "/net/cdaq/cdaql3data/cdaq/hallc-online/ROOTfiles/" ;
  fileNameD += "coin_replay_production_"; //read the root file from data
  fileNameD += runNumber; //read the root file from data
  fileNameD += "_-1.root"; //read the root file from data
  TFile *f1 = new TFile(fileNameD);
  TTree *tt = (TTree*)f1->Get("T");
  //get the relevant branch
  int nentriesD = tt->GetEntries();
  cout<<"Entries:\t"<<nentriesD<<endl;
  TString fileO;
   fileO += "HISTOGRAMS/COIN/ROOT/run_"; //read the root file from data
   fileO += runNumber; //read the root file from data
   fileO += "_hists_coin.root"; //read the root file from data

   //TFile *fout = new TFile(fileO,"RECREATE");

  gROOT->SetBatch(kTRUE);

  
  //make histograms:
  TH1D *h_hdelta = new TH1D("h_hdelta","HMS DELTA (%)",100,-12,12);
  TH1D *h_hxptar = new TH1D("h_hxptar","HMS XPTAR (rad)",100,-0.1,0.1);
  TH1D *h_hyptar = new TH1D("h_hyptar","HMS YPTAR (rad)",100,-0.08,0.08);
  TH1D *h_hytar = new TH1D("h_hytar","HMS YTAR (cm)",100,-12.0,12.0);

  TH1D *h_pdelta = new TH1D("h_pdelta","SHMS DELTA (%)",100,-15,15);
  TH1D *h_pxptar = new TH1D("h_pxptar","SHMS XPTAR (rad)",100,-0.06,0.06);
  TH1D *h_pyptar = new TH1D("h_pyptar","SHMS YPTAR (rad)",100,-0.06,0.06);
  TH1D *h_pytar = new TH1D("h_pytar","SHMS YTAR (cm)",100,-12.0,12.0);

  TH1D *h_Emd = new TH1D("h_Emd",Form("Missing Energy (GeV) Run:%d",runNumber),200,-0.15,0.25);
  TH1D *h_Wd = new TH1D("h_Wd",Form("W (GeV) Run:%d",runNumber), 150, 0., 2.);
  TH1D *h_pmd = new TH1D("h_pmd","Pm (GeV/c)", 100, -0.02, 0.35);
  TH1D *h1PcointimeROC1 = new TH1D("SHMS ROC1 Corrected Coin Time","SHMS ROC1 Corrected Coin Time; cointime [ns]", 200, -10, 10);    
  TH1D *h1PcointimeROC2 = new TH1D("SHMS ROC2 Corrected Coin Time","SHMS ROC2 Corrected Coin Time; cointime [ns]", 200, -10, 10); 


  Double_t HgtrX, HgtrTh, HgtrY, HgtrPh, hdelta, PgtrX, PgtrTh, PgtrY, PgtrPh, pdelta;
  Double_t HgtrBetaCalc, PgtrBetaCalc, evtType, PgtrP, HgtrP, PhodStatus, PhodStartTime, PhodfpHitsTime;
  Double_t cointime, HhodStatus, HhodStartTime, HhodfpHitsTime, SHMSpartMass, HMSpartMass;
  Double_t pkinW, pEm, pPm, modPm, pbeta, hbeta, hcalepr, hcaletot, hcernpe, pcaletot, pcalepr, pcernpe;
  Double_t TcoinpTRIG1_ROC1_tdcTimeRaw, TcoinpTRIG4_ROC1_tdcTimeRaw, TcoinpTRIG1_ROC2_tdcTimeRaw;
  Double_t TcoinhTRIG1_ROC1_tdcTimeRaw, TcoinhTRIG1_ROC2_tdcTimeRaw, TcoinhTRIG4_ROC1_tdcTimeRaw;
  Double_t TcoinhTRIG4_ROC2_tdcTimeRaw, TcoinpTRIG4_ROC2_tdcTimeRaw;

  int cnts=0;
  TCut hpdelta;
  TCut evtCut;                                                                   
  TCut eBetaCut;                                                                 
  TCut pBetaCut;                                                                 
  TCut cerCut;                                                                   
  TCut calCut;                                                                   
  TCut hodoTimeCut;
  TCut Wcut;
  TCut Emcut;
  TCut Pmcut;                                                              

  tt->SetBranchAddress("P.gtr.p", &PgtrP); 
  tt->SetBranchAddress("H.gtr.p", &HgtrP); 
  tt->SetBranchAddress("P.gtr.beta", &pbeta);                                                                                 
  tt->SetBranchAddress("H.gtr.beta", &hbeta); 
  tt->SetBranchAddress("H.gtr.dp", &hdelta);                                                                                
  tt->SetBranchAddress("P.gtr.dp", &pdelta);                                                                                
                                                                                                                            
  tt->SetBranchAddress("P.cal.eprtracknorm", &pcalepr);                                                                    
  tt->SetBranchAddress("P.cal.etottracknorm", &pcaletot);                                                                  
  tt->SetBranchAddress("P.hgcer.npeSum", &pcernpe);                                                 
                                                                                                             
  tt->SetBranchAddress("H.cal.eprtracknorm", &hcalepr);                                            
  tt->SetBranchAddress("H.cal.etottracknorm", &hcaletot);                                          
  tt->SetBranchAddress("H.cer.npeSum", &hcernpe); 
  tt->SetBranchAddress("H.kin.primary.W", &pkinW);                                                    
  if (targ == 1) {                
    tt->SetBranchAddress("P.kin.secondary.emiss", &pEm);                           
  }
  else if (targ == 2){
    tt->SetBranchAddress("P.kin.secondary.emiss_nuc", &pEm);                          }                     

  tt->SetBranchAddress("P.kin.secondary.pmiss", &pPm);                                                    
                                                                                                              
  tt->SetBranchAddress("P.hod.goodstarttime", &PhodStatus);                                              
  tt->SetBranchAddress("P.hod.starttime", &PhodStartTime);                                               
  tt->SetBranchAddress("P.hod.fpHitsTime", &PhodfpHitsTime);                                             
                                                                                                             
  tt->SetBranchAddress("H.hod.goodstarttime", &HhodStatus);                                              
  tt->SetBranchAddress("H.hod.starttime", &HhodStartTime);                                               
  tt->SetBranchAddress("H.hod.fpHitsTime", &HhodfpHitsTime);                                             
                                                                                                             
  tt->SetBranchAddress("H.gtr.x", &HgtrX);                                                               
  tt->SetBranchAddress("H.gtr.th", &HgtrTh);                                                             
  tt->SetBranchAddress("H.gtr.y", &HgtrY);                                                               
  tt->SetBranchAddress("H.gtr.ph", &HgtrPh);                                                             

  tt->SetBranchAddress("P.gtr.x", &PgtrX);                                                               
  tt->SetBranchAddress("P.gtr.th", &PgtrTh);                                                             
  tt->SetBranchAddress("P.gtr.y", &PgtrY);                                                               
  tt->SetBranchAddress("P.gtr.ph", &PgtrPh);                                                             
                                                                                                             
  tt->SetBranchAddress("T.coin.pTRIG1_ROC1_tdcTimeRaw", &TcoinpTRIG1_ROC1_tdcTimeRaw);                   
  tt->SetBranchAddress("T.coin.pTRIG4_ROC1_tdcTimeRaw", &TcoinpTRIG4_ROC1_tdcTimeRaw);
  tt->SetBranchAddress("T.coin.pTRIG1_ROC2_tdcTimeRaw", &TcoinpTRIG1_ROC2_tdcTimeRaw);                   
  tt->SetBranchAddress("T.coin.pTRIG4_ROC2_tdcTimeRaw", &TcoinpTRIG4_ROC2_tdcTimeRaw);                   
                                                                                                             
  tt->SetBranchAddress("T.coin.hTRIG1_ROC1_tdcTimeRaw", &TcoinhTRIG1_ROC1_tdcTimeRaw);                   
  tt->SetBranchAddress("T.coin.hTRIG4_ROC1_tdcTimeRaw", &TcoinhTRIG4_ROC1_tdcTimeRaw);                   
  tt->SetBranchAddress("T.coin.hTRIG1_ROC2_tdcTimeRaw", &TcoinhTRIG1_ROC2_tdcTimeRaw);                   
  tt->SetBranchAddress("T.coin.hTRIG4_ROC2_tdcTimeRaw", &TcoinhTRIG4_ROC2_tdcTimeRaw);                   

   SHMSpartMass = 0.93827208; // proton mass in GeV/c^2                 
   HMSpartMass = 0.000510998; // electron mass in GeV/c^2                 
   hpdelta = "P.gtr.dp > -10 && P.gtr.dp < 10 && H.gtr.dp > -8 && H.gtr.dp < 8";
   eBetaCut = "P.gtr.beta > 0.8 && P.gtr.beta < 1.3";                        
   pBetaCut = "H.gtr.beta > 0.8 &&  H.gtr.beta < 1.3";                       
   cerCut = "P.hgcer.npeSum < 0.1 && H.cer.npeSum > 0.5";                    
   calCut = "H.cal.etottracknorm > 0.8 && H.cal.etottracknorm < 1.5 && H.cal.eprtracknorm  > 0.2";         
   hodoTimeCut ="P.hod.goodstarttime == 1 && H.hod.goodstarttime == 1";      
   Wcut = "H.kin.primary.W >0.75 && H.kin.primary.W < 1.1";

  

  TCanvas *canvas1 = new TCanvas("canvas1","canvas1");                           
  tt->Draw("P.hod.starttime >> SHMShodoStartTime", eBetaCut && pBetaCut && cerCut && calCut && hodoTimeCut);  
  TH1D *h1PhodoStartTime = (TH1D*)gDirectory->Get("SHMShodoStartTime");
  h1PhodoStartTime->GetXaxis()->SetTitle("SHMS hodo start time [ns]");           
  Double_t PhodoStartTimeMean = h1PhodoStartTime->GetMean();                     
  TCanvas *canvas2 = new TCanvas("canvas2","canvas2");                           
  tt->Draw("H.hod.starttime >> HMShodoStartTime", eBetaCut && pBetaCut && cerCut && calCut && hodoTimeCut);  
  TH1D *h1HhodoStartTime = (TH1D*)gDirectory->Get("HMShodoStartTime");           
  h1HhodoStartTime->GetXaxis()->SetTitle("HMS hodo start time [ns]");            
  Double_t HhodoStartTimeMean = h1HhodoStartTime->GetMean();                     

 
  Double_t pOffset = 1.5; //9.5 + 10;  // in ns                                  
  Double_t hOffset = 335;                                                        
  Double_t speedOfLight = 29.9792; // in cm/ns                                   
  Double_t SHMScentralPathLen = 18.1*100;  // SHMS Target to focal plane path length converted to cm  
  Double_t SHMSpathLength = 18.1*100;      // For now assume that it's same as SHMScentralPathLen  
  Double_t HMScentralPathLen = 22*100;     // HMS Target to focal plane path length converted to cm
  Double_t DeltaHMSpathLength;             // For now assume that it's same as HMScentralPathLen 
  Double_t SHMScoinCorr = 0.0;                                                   
  Double_t HMScoinCorr = 0.0;                                                    
  Double_t SHMSrawCoinTimeROC1;                                                  
  Double_t SHMSrawCoinTimeROC2;                                                  
  Double_t HMSrawCoinTimeROC1;                                                   
  Double_t HMSrawCoinTimeROC2;                                                   
                                                                                   
  Double_t SHMScorrCoinTimeROC1;                                                 
  Double_t SHMScorrCoinTimeROC2;                                                 
  Double_t HMScorrCoinTimeROC1;                                                  
  Double_t HMScorrCoinTimeROC2;                                                  
 
  for (int kk=0; kk<nentriesD; kk++){
    tt->GetEntry(kk);
    if (kk % 50000 == 0) cout << kk*100/nentriesD << "   % of data done" << endl;
      evtType = tt->GetLeaf("fEvtHdr.fEvtType")->GetValue(); 
    if (pbeta>0.6 && pbeta<1.4 && hbeta>0.8 && hbeta<1.2 && hcernpe>0. && hcaletot >0.6 && hcaletot<2.0 && PhodStatus == 1 && HhodStatus ==1 && hdelta > -10 && hdelta < 10 && pdelta > -15 && pdelta < 15 && pcernpe < 0.1) 
    { 
	DeltaHMSpathLength = 12.462*HgtrTh + 0.1138*HgtrTh*HgtrX - 0.0154*HgtrX - 72.292*HgtrTh*HgtrTh - 0.0000544*HgtrX*HgtrX - 116.52*HgtrPh*HgtrPh;               
      PgtrBetaCalc = PgtrP/sqrt(PgtrP*PgtrP + SHMSpartMass*SHMSpartMass);        
      HgtrBetaCalc = HgtrP/sqrt(HgtrP*HgtrP + HMSpartMass*HMSpartMass);          
      SHMScoinCorr = SHMScentralPathLen / (speedOfLight*PgtrBetaCalc) + (SHMSpathLength - SHMScentralPathLen) / speedOfLight*PgtrBetaCalc + (PhodoStartTimeMean - PhodfpHitsTime);                                                                   
      HMScoinCorr = HMScentralPathLen / (speedOfLight*HgtrBetaCalc) + DeltaHMSpathLength / speedOfLight*HgtrBetaCalc + (HhodoStartTimeMean - HhodfpHitsTime);      
      SHMScorrCoinTimeROC1 = (TcoinpTRIG1_ROC1_tdcTimeRaw*0.1 - SHMScoinCorr) - (TcoinpTRIG4_ROC1_tdcTimeRaw*0.1 - HMScoinCorr) - pOffset; // 0.1 to convert to ns 
      SHMScorrCoinTimeROC2 = (TcoinpTRIG1_ROC2_tdcTimeRaw*0.1 - SHMScoinCorr) - (TcoinpTRIG4_ROC2_tdcTimeRaw*0.1 - HMScoinCorr) - pOffset;                         
      h1PcointimeROC1->Fill(SHMScorrCoinTimeROC1);                               
      h1PcointimeROC2->Fill(SHMScorrCoinTimeROC2);
      if (targ == 1)
       {
       if (pkinW > 0.75 && pkinW < 1.15)
        {
        h_hdelta->Fill(hdelta);
        h_hxptar->Fill(HgtrPh);
        h_hyptar->Fill(HgtrTh);
        h_hytar->Fill(HgtrY);
        h_pdelta->Fill(pdelta);
        h_pxptar->Fill(PgtrPh);
        h_pyptar->Fill(PgtrTh);
        h_pytar->Fill(PgtrY);
        h_Emd->Fill(pEm+0.025);
        h_Wd->Fill(pkinW);
        h_pmd->Fill(pPm-0.04);
        cnts++;
        }
      }
      else if (targ == 2)
      {
	if (sqrt(pPm*pPm) < 0.6)
        {
        h_hdelta->Fill(hdelta);
        h_hxptar->Fill(HgtrPh);
        h_hyptar->Fill(HgtrTh);
        h_hytar->Fill(HgtrY);
        h_pdelta->Fill(pdelta);
        h_pxptar->Fill(PgtrPh);
        h_pyptar->Fill(PgtrTh);
        h_pytar->Fill(PgtrY);
        h_Emd->Fill(pEm);
        h_Wd->Fill(pkinW);
        modPm = sqrt(pPm*pPm);
        h_pmd->Fill(modPm);
        cnts++;
        }
      }
    }
      
  }



  gROOT->SetBatch(kFALSE);

  TCanvas *can1 = new TCanvas ("can1","can1");
  can1->Divide(4,2);

  can1->cd(1);
  h_hdelta->Draw();  
  can1->cd(2);
  h_hxptar->Draw();  
  can1->cd(3);
  h_hyptar->Draw();  
  can1->cd(4);
  h_hytar->Draw();  
  can1->cd(5);
  h_pdelta->Draw();  
  can1->cd(6);
  h_pxptar->Draw();  
  can1->cd(7);
  h_pyptar->Draw();  
  can1->cd(8);
  h_pytar->Draw();  
  //  can1->SaveAs("tmp1.pdf");

  TCanvas *can2 = new TCanvas ("can2",Form("can2 Run: %d",runNumber));
  can2->Divide(2,2);
  can2->cd(1);
  h_Wd->SetFillColor(kBlue);
  h_Wd->Draw();
  can2->cd(2); 
  h_Emd->SetFillColor(kBlue);
  h_Emd->Draw(); 
  can2->cd(3); 
  h_pmd->SetFillColor(kBlue);
  h_pmd->Draw(); 
  can2->cd(4); 
  h1PcointimeROC1->SetLineColor(kBlue);
  h1PcointimeROC1->SetLineWidth(3);
  h1PcointimeROC1->Draw();

  // can2->SaveAs("tmp.pdf");

  //fout->Write();
  //  fout->Close();
  cout << "-------------------------------------------------------------------------------------------------------" << endl;
  cout << "NOTE:>>>>  " << cnts <<"  <<<<<  GOOD EVENTS have passed all cuts (update the white board and excel sheet)" << endl;
  cout << "-------------------------------------------------------------------------------------------------------" << endl;

}
