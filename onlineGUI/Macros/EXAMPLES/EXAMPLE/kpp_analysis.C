
void UserScript() {

  // Declare constants
  static const Double_t clk2tdc     = 0.100;
  static const Double_t clk2adc     = 0.0625;
  static const UInt_t   ndcRefTimes = 10;

  static const UInt_t nbars_1x = 13;
  static const UInt_t nbars_1y = 13;
  static const UInt_t nbars_2x = 14;
  static const UInt_t nbars_2y = 21;

  static const UInt_t nshwr_blks      = 224;
  static const UInt_t npos_pshwr_blks = 14;
  static const UInt_t nneg_pshwr_blks = 14;

  static const UInt_t nhgc_pmts = 4;
  static const UInt_t nngc_pmts = 4;

  static const UInt_t npos_aero_pmts = 7;
  static const UInt_t nneg_aero_pmts = 7;

  static const UInt_t maxTdcHits = 128;
  static const UInt_t maxAdcHits = 4;

  // static const UInt_t adc_timeOffset = 175;

  // static const Double_t pshwr_adc2GeV = 0.001; 
  static const Double_t pshwr_neg_adc2GeV[nneg_pshwr_blks] = {1.0/466.2, 1.0/514.7, 1.0/325.5, 1.0/247.8, 
							      1.0/322.0, 1.0/176.2, 1.0/161.9, 1.0/197.7, 
							      1.0/269.9, 1.0/241.6, 1.0/193.7, 1.0/337.5, 
							      1.0/237.3, 1.0/82.0};
  
  static const Double_t pshwr_pos_adc2GeV[npos_pshwr_blks] = {1.0/411.2, 1.0/378.6, 1.0/357.9, 1.0/254.8, 
							      1.0/315.6, 1.0/331.0, 1.0/264.6, 1.0/305.4, 
							      1.0/149.8, 1.0/380.6, 1.0/183.1, 1.0/206.5, 
							      1.0/139.8, 1.0/163.1};

  static const Double_t shwr_adc2GeV  = 0.0005;

  // static const Double_t hgc_adc2npe = 1./205.;
  static const Double_t hgc_adc2npe[nhgc_pmts] = {1./218., 1./196., 1./182., 1./186.};
  

  // static const Double_t ngc_adc2npe = 1./150.;
  static const Double_t ngc_adc2npe[nngc_pmts] = {1.0/500., 1.0/500., 1.0/500., 1.0/500.};
  //static const Double_t ngc_adc2npe[nngc_pmts] = {1.0/5000., 1.0/5000., 1.0/5000., 1.0/5000.};

  static const Double_t aero_neg_adc2npe[nneg_aero_pmts] = {1./586.8, 1./536.5, 1./690.5, 1./563.1, 
							    1./574.4, 1./432.7, 1./601.5};
  
  // static const Double_t aero_adc2npe = 1./500.;
  static const Double_t aero_pos_adc2npe[npos_aero_pmts] = {1./496.7, 1./467.5, 1./554.3, 1./766.1, 
							    1./412.1, 1./517.9, 1./435.6};

  static const UInt_t maxNumTracks = 10;

  // =:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:

  // Declare variables
  Double_t p1X_tdcTime, p1Y_tdcTime, p2X_tdcTime, p2Y_tdcTime;
  Double_t p1T_tdcTime, p2T_tdcTime;
  Double_t pT1_tdcTime, pT2_tdcTime, pT3_tdcTime;
  Double_t pDCREF_tdcTime[ndcRefTimes];

  // Hodoscope hits
  Int_t    p1X_negAdcHits, p1Y_negAdcHits, p2X_negAdcHits, p2Y_negAdcHits;
  Int_t    p1X_negTdcHits, p1Y_negTdcHits, p2X_negTdcHits, p2Y_negTdcHits;
  Double_t p1X_negAdcPaddle[maxAdcHits*nbars_1x], p1Y_negAdcPaddle[maxAdcHits*nbars_1y];
  Double_t p2X_negAdcPaddle[maxAdcHits*nbars_2x], p2Y_negAdcPaddle[maxAdcHits*nbars_2y];
  Double_t p1X_negTdcPaddle[maxTdcHits*nbars_1x], p1Y_negTdcPaddle[maxTdcHits*nbars_1y];
  Double_t p2X_negTdcPaddle[maxTdcHits*nbars_2x], p2Y_negTdcPaddle[maxTdcHits*nbars_2y];
  Double_t p1X_negAdcPulseTime[maxAdcHits*nbars_1x], p1Y_negAdcPulseTime[maxAdcHits*nbars_1y];
  Double_t p2X_negAdcPulseTime[maxAdcHits*nbars_2x], p2Y_negAdcPulseTime[maxAdcHits*nbars_2y];
  Double_t p1X_negTdcTime[maxTdcHits*nbars_1x], p1Y_negTdcTime[maxTdcHits*nbars_1y];
  Double_t p2X_negTdcTime[maxTdcHits*nbars_2x], p2Y_negTdcTime[maxTdcHits*nbars_2y];
  Int_t    p1X_posAdcHits, p1Y_posAdcHits, p2X_posAdcHits, p2Y_posAdcHits;
  Int_t    p1X_posTdcHits, p1Y_posTdcHits, p2X_posTdcHits, p2Y_posTdcHits;
  Double_t p1X_posAdcPaddle[maxAdcHits*nbars_1x], p1Y_posAdcPaddle[maxAdcHits*nbars_1y];
  Double_t p2X_posAdcPaddle[maxAdcHits*nbars_2x], p2Y_posAdcPaddle[maxAdcHits*nbars_2y];
  Double_t p1X_posTdcPaddle[maxTdcHits*nbars_1x], p1Y_posTdcPaddle[maxTdcHits*nbars_1y];
  Double_t p2X_posTdcPaddle[maxTdcHits*nbars_2x], p2Y_posTdcPaddle[maxTdcHits*nbars_2y];
  Double_t p1X_posAdcPulseTime[maxAdcHits*nbars_1x], p1Y_posAdcPulseTime[maxAdcHits*nbars_1y];
  Double_t p2X_posAdcPulseTime[maxAdcHits*nbars_2x], p2Y_posAdcPulseTime[maxAdcHits*nbars_2y];
  Double_t p1X_posTdcTime[maxTdcHits*nbars_1x], p1Y_posTdcTime[maxTdcHits*nbars_1y];
  Double_t p2X_posTdcTime[maxTdcHits*nbars_2x], p2Y_posTdcTime[maxTdcHits*nbars_2y];

  // "Good" hodo hits
  Int_t    p1X_nGoodHodoHits, p1Y_nGoodHodoHits, p2X_nGoodHodoHits, p2Y_nGoodHodoHits;
  Double_t p1X_goodPaddle[maxAdcHits*nbars_1x], p1Y_goodPaddle[maxAdcHits*nbars_1y];
  Double_t p2X_goodPaddle[maxAdcHits*nbars_2x], p2Y_goodPaddle[maxAdcHits*nbars_2y];
  Double_t p1X_goodNegTdcTimeCorr[maxTdcHits*nbars_1x], p1Y_goodNegTdcTimeCorr[maxTdcHits*nbars_1y];
  Double_t p2X_goodNegTdcTimeCorr[maxTdcHits*nbars_2x], p2Y_goodNegTdcTimeCorr[maxTdcHits*nbars_2y];
  Double_t p1X_goodPosTdcTimeCorr[maxTdcHits*nbars_1x], p1Y_goodPosTdcTimeCorr[maxTdcHits*nbars_1y];
  Double_t p2X_goodPosTdcTimeCorr[maxTdcHits*nbars_2x], p2Y_goodPosTdcTimeCorr[maxTdcHits*nbars_2y];

  // Hodoscope focal plane times
  Double_t p1X_fpTime, p1Y_fpTime, p2X_fpTime, p2Y_fpTime;
  Double_t p1X_plTime, p1Y_plTime, p2X_plTime, p2Y_plTime;

  // Aerogel ADC
  Int_t paero_posHits, paero_negHits;
  Double_t paero_posPmt[maxAdcHits*npos_aero_pmts], paero_negPmt[maxAdcHits*nneg_aero_pmts];
  Double_t paero_posPulseTime[maxAdcHits*npos_aero_pmts], paero_negPulseTime[maxAdcHits*nneg_aero_pmts];
  Double_t paero_posPulseInt[maxAdcHits*npos_aero_pmts], paero_negPulseInt[maxAdcHits*nneg_aero_pmts];

  // Pre-Shower ADC
  Int_t ppshwr_posHits, ppshwr_negHits;
  Double_t ppshwr_posPmt[maxAdcHits*npos_pshwr_blks], ppshwr_negPmt[maxAdcHits*nneg_pshwr_blks];
  Double_t ppshwr_posPulseTime[maxAdcHits*npos_pshwr_blks], ppshwr_negPulseTime[maxAdcHits*nneg_pshwr_blks];
  Double_t ppshwr_posPulseInt[maxAdcHits*npos_pshwr_blks], ppshwr_negPulseInt[maxAdcHits*nneg_pshwr_blks];

  // Shower ADC
  Int_t pshwr_hits;
  Double_t pshwr_pmt[maxAdcHits*nshwr_blks], pshwr_pulseTime[maxAdcHits*nshwr_blks];
  Double_t pshwr_pulseInt[maxAdcHits*nshwr_blks];

  // Heavy gas Cherenkov ADC
  Int_t phgc_hits;
  Double_t phgc_pmt[maxAdcHits*nhgc_pmts], phgc_pulseTime[maxAdcHits*nhgc_pmts];
  Double_t phgc_pulseInt[maxAdcHits*nhgc_pmts];

  // Noble gas Cherenkov ADC
  Int_t pngc_hits;
  Double_t pngc_pmt[maxAdcHits*nngc_pmts], pngc_pulseTime[maxAdcHits*nngc_pmts];
  Double_t pngc_pulseInt[maxAdcHits*nngc_pmts];

  // Tracking information
  Int_t ntracks;
  Double_t trk_pmag[maxNumTracks];
  Double_t trk_beta[maxNumTracks];

  Long64_t nentries;

  // =:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:

  // Declare Histos
  TH1F *h_p1X_tdc, *h_p1Y_tdc, *h_p2X_tdc, *h_p2Y_tdc;
  TH1F *h_p1T_tdc, *h_p2T_tdc;
  TH1F *h_pT1_tdc, *h_pT2_tdc, *h_pT3_tdc;
  TH1F *h_p1XmpT2_tdc, *h_p1YmpT2_tdc, *h_p2XmpT2_tdc, *h_p2YmpT2_tdc;
  TH1F *h_p1XmpT3_tdc, *h_p1YmpT3_tdc, *h_p2XmpT3_tdc, *h_p2YmpT3_tdc;
  TH1F *h_p1TmpT2_tdc, *h_p2TmpT2_tdc, *h_p1TmpT3_tdc, *h_p2TmpT3_tdc;
  TH1F *h_pDCREF_tdc[ndcRefTimes];

  TH2F *h2_p1Xneg_pt_tt_diff, *h2_p1Yneg_pt_tt_diff, *h2_p2Xneg_pt_tt_diff, *h2_p2Yneg_pt_tt_diff;
  TH2F *h2_p1Xpos_pt_tt_diff, *h2_p1Ypos_pt_tt_diff, *h2_p2Xpos_pt_tt_diff, *h2_p2Ypos_pt_tt_diff;

  TH1F *h_p1X_fpTime, *h_p1Y_fpTime, *h_p2X_fpTime, *h_p2Y_fpTime;
  TH1F *h_p1X_plTime, *h_p1Y_plTime, *h_p2X_plTime, *h_p2Y_plTime;

  TH2F *h2_p1X_negTdcCorr, *h2_p1Y_negTdcCorr, *h2_p2X_negTdcCorr, *h2_p2Y_negTdcCorr;
  TH2F *h2_p1X_posTdcCorr, *h2_p1Y_posTdcCorr, *h2_p2X_posTdcCorr, *h2_p2Y_posTdcCorr;
  TH2F *h2_p1X_tdcCorrDiff, *h2_p1Y_tdcCorrDiff, *h2_p2X_tdcCorrDiff, *h2_p2Y_tdcCorrDiff;

  TH2F *h2_paero_posPulseTime_pT1_diff, *h2_paero_negPulseTime_pT1_diff;
  TH2F *h2_paero_posPulseTime_pT2_diff, *h2_paero_negPulseTime_pT2_diff;
  TH2F *h2_paero_posPulseTime_pT3_diff, *h2_paero_negPulseTime_pT3_diff;

  TH2F *h2_ppshwr_posPulseTime_pT1_diff, *h2_ppshwr_negPulseTime_pT1_diff;
  TH2F *h2_ppshwr_posPulseTime_pT2_diff, *h2_ppshwr_negPulseTime_pT2_diff;
  TH2F *h2_ppshwr_posPulseTime_pT3_diff, *h2_ppshwr_negPulseTime_pT3_diff;

  TH2F *h2_pshwr_pulseTime_pT1_diff;
  TH2F *h2_pshwr_pulseTime_pT2_diff;
  TH2F *h2_pshwr_pulseTime_pT3_diff;

  TH2F *h2_phgc_pulseTime_pT1_diff;
  TH2F *h2_phgc_pulseTime_pT2_diff;
  TH2F *h2_phgc_pulseTime_pT3_diff;

  TH2F *h2_pngc_pulseTime_pT1_diff;
  TH2F *h2_pngc_pulseTime_pT2_diff;
  TH2F *h2_pngc_pulseTime_pT3_diff;

  TH2F *h2_pshwr_vs_phgcer, *h2_pshwr_vs_pngcer, *h2_pshwr_vs_ppshwr;
  TH1F *h_paero_sum, *h_ppshwr_sum, *h_pshwr_sum, *h_ptotshwr_sum, *h_phgc_sum, *h_pngc_sum;

  TH2F *h2_EdivP_vs_P;

  // =:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:

  // Declare trees
  TTree *T = (TTree*) gDirectory->Get("T");

  // Acquire the number of entries
  nentries = T->GetEntries();

  // Acquire the branches
  // Trigger time information
  T->SetBranchAddress("T.shms.p1X_tdcTime", &p1X_tdcTime);
  T->SetBranchAddress("T.shms.p1Y_tdcTime", &p1Y_tdcTime);
  T->SetBranchAddress("T.shms.p2X_tdcTime", &p2X_tdcTime);
  T->SetBranchAddress("T.shms.p2Y_tdcTime", &p2Y_tdcTime);
  T->SetBranchAddress("T.shms.p1T_tdcTime", &p1T_tdcTime);
  T->SetBranchAddress("T.shms.p2T_tdcTime", &p2T_tdcTime);
  T->SetBranchAddress("T.shms.pT1_tdcTime", &pT1_tdcTime);
  T->SetBranchAddress("T.shms.pT2_tdcTime", &pT2_tdcTime);
  T->SetBranchAddress("T.shms.pT3_tdcTime", &pT3_tdcTime);

  // ==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//

  // DC reference times
  for (UInt_t iref = 0; iref < ndcRefTimes; iref++)
    T->SetBranchAddress(Form("T.shms.pDCREF%d_tdcTime", iref+1), &pDCREF_tdcTime[iref]);

  // ==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//

  // Hodoscope focal plane time calculation
  T->SetBranchAddress("P.hod.1x.fptime", &p1X_fpTime);
  T->SetBranchAddress("P.hod.1y.fptime", &p1Y_fpTime);
  T->SetBranchAddress("P.hod.2x.fptime", &p2X_fpTime);
  T->SetBranchAddress("P.hod.2y.fptime", &p2Y_fpTime);

  // ==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//

  // Hodoscope ADC information
  T->SetBranchAddress("Ndata.P.hod.1x.negAdcCounter", &p1X_negAdcHits);
  T->SetBranchAddress("Ndata.P.hod.1y.negAdcCounter", &p1Y_negAdcHits);
  T->SetBranchAddress("Ndata.P.hod.2x.negAdcCounter", &p2X_negAdcHits);
  T->SetBranchAddress("Ndata.P.hod.2y.negAdcCounter", &p2Y_negAdcHits);
  T->SetBranchAddress("P.hod.1x.negAdcCounter", p1X_negAdcPaddle);
  T->SetBranchAddress("P.hod.1y.negAdcCounter", p1Y_negAdcPaddle);
  T->SetBranchAddress("P.hod.2x.negAdcCounter", p2X_negAdcPaddle);
  T->SetBranchAddress("P.hod.2y.negAdcCounter", p2Y_negAdcPaddle);
  T->SetBranchAddress("P.hod.1x.negAdcPulseTimeRaw", p1X_negAdcPulseTime);
  T->SetBranchAddress("P.hod.1y.negAdcPulseTimeRaw", p1Y_negAdcPulseTime);
  T->SetBranchAddress("P.hod.2x.negAdcPulseTimeRaw", p2X_negAdcPulseTime);
  T->SetBranchAddress("P.hod.2y.negAdcPulseTimeRaw", p2Y_negAdcPulseTime);
  T->SetBranchAddress("Ndata.P.hod.1x.posAdcCounter", &p1X_posAdcHits);
  T->SetBranchAddress("Ndata.P.hod.1y.posAdcCounter", &p1Y_posAdcHits);
  T->SetBranchAddress("Ndata.P.hod.2x.posAdcCounter", &p2X_posAdcHits);
  T->SetBranchAddress("Ndata.P.hod.2y.posAdcCounter", &p2Y_posAdcHits);
  T->SetBranchAddress("P.hod.1x.posAdcCounter", p1X_posAdcPaddle);
  T->SetBranchAddress("P.hod.1y.posAdcCounter", p1Y_posAdcPaddle);
  T->SetBranchAddress("P.hod.2x.posAdcCounter", p2X_posAdcPaddle);
  T->SetBranchAddress("P.hod.2y.posAdcCounter", p2Y_posAdcPaddle);
  T->SetBranchAddress("P.hod.1x.posAdcPulseTimeRaw", p1X_posAdcPulseTime);
  T->SetBranchAddress("P.hod.1y.posAdcPulseTimeRaw", p1Y_posAdcPulseTime);
  T->SetBranchAddress("P.hod.2x.posAdcPulseTimeRaw", p2X_posAdcPulseTime);
  T->SetBranchAddress("P.hod.2y.posAdcPulseTimeRaw", p2Y_posAdcPulseTime);
  // Hodoscope TDC information
  T->SetBranchAddress("Ndata.P.hod.1x.negTdcCounter", &p1X_negTdcHits);
  T->SetBranchAddress("Ndata.P.hod.1y.negTdcCounter", &p1Y_negTdcHits);
  T->SetBranchAddress("Ndata.P.hod.2x.negTdcCounter", &p2X_negTdcHits);
  T->SetBranchAddress("Ndata.P.hod.2y.negTdcCounter", &p2Y_negTdcHits);
  T->SetBranchAddress("P.hod.1x.negTdcCounter", p1X_negTdcPaddle);
  T->SetBranchAddress("P.hod.1y.negTdcCounter", p1Y_negTdcPaddle);
  T->SetBranchAddress("P.hod.2x.negTdcCounter", p2X_negTdcPaddle);
  T->SetBranchAddress("P.hod.2y.negTdcCounter", p2Y_negTdcPaddle);
  T->SetBranchAddress("P.hod.1x.negTdcTime", p1X_negTdcTime);
  T->SetBranchAddress("P.hod.1y.negTdcTime", p1Y_negTdcTime);
  T->SetBranchAddress("P.hod.2x.negTdcTime", p2X_negTdcTime);
  T->SetBranchAddress("P.hod.2y.negTdcTime", p2Y_negTdcTime);
  T->SetBranchAddress("Ndata.P.hod.1x.posTdcCounter", &p1X_posTdcHits);
  T->SetBranchAddress("Ndata.P.hod.1y.posTdcCounter", &p1Y_posTdcHits);
  T->SetBranchAddress("Ndata.P.hod.2x.posTdcCounter", &p2X_posTdcHits);
  T->SetBranchAddress("Ndata.P.hod.2y.posTdcCounter", &p2Y_posTdcHits);
  T->SetBranchAddress("P.hod.1x.posTdcCounter", p1X_posTdcPaddle);
  T->SetBranchAddress("P.hod.1y.posTdcCounter", p1Y_posTdcPaddle);
  T->SetBranchAddress("P.hod.2x.posTdcCounter", p2X_posTdcPaddle);
  T->SetBranchAddress("P.hod.2y.posTdcCounter", p2Y_posTdcPaddle);
  T->SetBranchAddress("P.hod.1x.posTdcTime", p1X_posTdcTime);
  T->SetBranchAddress("P.hod.1y.posTdcTime", p1Y_posTdcTime);
  T->SetBranchAddress("P.hod.2x.posTdcTime", p2X_posTdcTime);
  T->SetBranchAddress("P.hod.2y.posTdcTime", p2Y_posTdcTime);

  // ==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//

  // "Good" hodoscope hits
  // Hodoscope corrected times (not corrected for TOF to focal plane)
  T->SetBranchAddress("Ndata.P.hod.1x.GoodPaddle", &p1X_nGoodHodoHits);
  T->SetBranchAddress("Ndata.P.hod.1y.GoodPaddle", &p1Y_nGoodHodoHits);
  T->SetBranchAddress("Ndata.P.hod.2x.GoodPaddle", &p2X_nGoodHodoHits);
  T->SetBranchAddress("Ndata.P.hod.2y.GoodPaddle", &p2Y_nGoodHodoHits);
  T->SetBranchAddress("P.hod.1x.GoodPaddle", p1X_goodPaddle);
  T->SetBranchAddress("P.hod.1y.GoodPaddle", p1Y_goodPaddle);
  T->SetBranchAddress("P.hod.2x.GoodPaddle", p2X_goodPaddle);
  T->SetBranchAddress("P.hod.2y.GoodPaddle", p2Y_goodPaddle);
  T->SetBranchAddress("P.hod.1x.GoodNegTdcTimeCorr", p1X_goodNegTdcTimeCorr);
  T->SetBranchAddress("P.hod.1y.GoodNegTdcTimeCorr", p1Y_goodNegTdcTimeCorr);
  T->SetBranchAddress("P.hod.2x.GoodNegTdcTimeCorr", p2X_goodNegTdcTimeCorr);
  T->SetBranchAddress("P.hod.2y.GoodNegTdcTimeCorr", p2Y_goodNegTdcTimeCorr);
  T->SetBranchAddress("P.hod.1x.GoodPosTdcTimeCorr", p1X_goodPosTdcTimeCorr);
  T->SetBranchAddress("P.hod.1y.GoodPosTdcTimeCorr", p1Y_goodPosTdcTimeCorr);
  T->SetBranchAddress("P.hod.2x.GoodPosTdcTimeCorr", p2X_goodPosTdcTimeCorr);
  T->SetBranchAddress("P.hod.2y.GoodPosTdcTimeCorr", p2Y_goodPosTdcTimeCorr);

  // ==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//

  T->SetBranchAddress("Ndata.P.aero.negAdcCounter", &paero_negHits);
  T->SetBranchAddress("Ndata.P.aero.posAdcCounter", &paero_posHits);
  T->SetBranchAddress("P.aero.negAdcCounter", paero_negPmt);
  T->SetBranchAddress("P.aero.posAdcCounter", paero_posPmt);
  T->SetBranchAddress("P.aero.negAdcPulseTimeRaw", paero_negPulseTime);
  T->SetBranchAddress("P.aero.posAdcPulseTimeRaw", paero_posPulseTime);
  T->SetBranchAddress("P.aero.negAdcPulseInt", paero_negPulseInt);
  T->SetBranchAddress("P.aero.posAdcPulseInt", paero_posPulseInt);

  // ==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//

  T->SetBranchAddress("Ndata.P.cal.pr.negAdcCounter", &ppshwr_negHits);
  T->SetBranchAddress("Ndata.P.cal.pr.posAdcCounter", &ppshwr_posHits);
  T->SetBranchAddress("P.cal.pr.negAdcCounter", ppshwr_negPmt);
  T->SetBranchAddress("P.cal.pr.posAdcCounter", ppshwr_posPmt);
  T->SetBranchAddress("P.cal.pr.negAdcPulseTimeRaw", ppshwr_negPulseTime);
  T->SetBranchAddress("P.cal.pr.posAdcPulseTimeRaw", ppshwr_posPulseTime);
  T->SetBranchAddress("P.cal.pr.negAdcPulseInt", ppshwr_negPulseInt);
  T->SetBranchAddress("P.cal.pr.posAdcPulseInt", ppshwr_posPulseInt);

  // ==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//

  T->SetBranchAddress("Ndata.P.cal.fly.adcCounter", &pshwr_hits);
  T->SetBranchAddress("P.cal.fly.adcCounter", pshwr_pmt);
  T->SetBranchAddress("P.cal.fly.adcPulseTimeRaw", pshwr_pulseTime);
  T->SetBranchAddress("P.cal.fly.adcPulseInt", pshwr_pulseInt);

  // ==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//

  T->SetBranchAddress("Ndata.P.hgcer.adcCounter", &phgc_hits);
  T->SetBranchAddress("P.hgcer.adcCounter", phgc_pmt);
  T->SetBranchAddress("P.hgcer.adcPulseTimeRaw", phgc_pulseTime);
  T->SetBranchAddress("P.hgcer.adcPulseInt", phgc_pulseInt);

  // ==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//

  T->SetBranchAddress("Ndata.P.ngcer.adcCounter", &pngc_hits);
  T->SetBranchAddress("P.ngcer.adcCounter", pngc_pmt);
  T->SetBranchAddress("P.ngcer.adcPulseTimeRaw", pngc_pulseTime);
  T->SetBranchAddress("P.ngcer.adcPulseInt", pngc_pulseInt);

  // ==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//

  T->SetBranchAddress("Ndata.P.tr.p", &ntracks);
  T->SetBranchAddress("P.tr.p", trk_pmag);
  T->SetBranchAddress("P.tr.beta", trk_beta);

  // =:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:

  // Create histos

  // ==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//

  h_p1X_tdc = new TH1F("h_p1X_tdc", "S1X Coincidence Time; TDC Time (ns); Counts / 1 ns", 300, 0, 300);
  h_p1Y_tdc = new TH1F("h_p1Y_tdc", "S1Y Coincidence Time; TDC Time (ns); Counts / 1 ns", 300, 0, 300);
  h_p2X_tdc = new TH1F("h_p2X_tdc", "S2X Coincidence Time; TDC Time (ns); Counts / 1 ns", 300, 0, 300);
  h_p2Y_tdc = new TH1F("h_p2Y_tdc", "S2Y Coincidence Time; TDC Time (ns); Counts / 1 ns", 300, 0, 300);
  h_p1T_tdc = new TH1F("h_p1T_tdc", "S1X/S1Y Coincidence Time; TDC Time (ns); Counts / 1 ns", 300, 0, 300);
  h_p2T_tdc = new TH1F("h_p2T_tdc", "S2X/S2Y Coincidence Time; TDC Time (ns); Counts / 1 ns", 300, 0, 300);
  h_pT1_tdc = new TH1F("h_pT1_tdc", "Hodoscope Trigger (Slot 20 Channel 15); TDC Time (ns); Counts / 1 ns", 300, 0, 300);
  h_pT2_tdc = new TH1F("h_pT2_tdc", "Hodoscope Trigger (Slot 19 Channel 31); TDC Time (ns); Counts / 1 ns", 300, 0, 300);
  h_pT3_tdc = new TH1F("h_pT3_tdc", "Hodoscope Trigger (Slot 19 Channel 38); TDC Time (ns); Counts / 1 ns", 300, 0, 300);
  for (UInt_t iref = 0; iref < ndcRefTimes; iref++)
    h_pDCREF_tdc[iref] = new TH1F(Form("h_pDCREF%d_tdc", iref+1), Form("DC Reference Time %d; TDC Time (ns); Counts / 1 ns", iref+1), 200, 400, 600);
  h_p1XmpT2_tdc = new TH1F("h_p1XmpT2_tdc", "Hodoscope Trigger (Slot 19 Channel 31) - S1X; TDC Time (ns); Counts / 1 ns", 150, 0, 150);
  h_p1YmpT2_tdc = new TH1F("h_p1YmpT2_tdc", "Hodoscope Trigger (Slot 19 Channel 31) - S1Y; TDC Time (ns); Counts / 1 ns", 150, 0, 150);
  h_p2XmpT2_tdc = new TH1F("h_p2XmpT2_tdc", "Hodoscope Trigger (Slot 19 Channel 31) - S2X; TDC Time (ns); Counts / 1 ns", 150, 0, 150);
  h_p2YmpT2_tdc = new TH1F("h_p2YmpT2_tdc", "Hodoscope Trigger (Slot 19 Channel 31) - S2Y; TDC Time (ns); Counts / 1 ns", 150, 0, 150);
  h_p1TmpT2_tdc = new TH1F("h_p1TmpT2_tdc", "Hodoscope Trigger (Slot 19 Channel 31) - S1; TDC Time (ns); Counts / 1 ns", 150, 0, 150);
  h_p2TmpT2_tdc = new TH1F("h_p2TmpT2_tdc", "Hodoscope Trigger (Slot 19 Channel 31) - S2; TDC Time (ns); Counts / 1 ns", 150, 0, 150);
  h_p1XmpT3_tdc = new TH1F("h_p1XmpT3_tdc", "Hodoscope Trigger (Slot 19 Channel 38) - S1X; TDC Time (ns); Counts / 1 ns", 150, 0, 150);
  h_p1YmpT3_tdc = new TH1F("h_p1YmpT3_tdc", "Hodoscope Trigger (Slot 19 Channel 38) - S1Y; TDC Time (ns); Counts / 1 ns", 150, 0, 150);
  h_p2XmpT3_tdc = new TH1F("h_p2XmpT3_tdc", "Hodoscope Trigger (Slot 19 Channel 38) - S2X; TDC Time (ns); Counts / 1 ns", 150, 0, 150);
  h_p2YmpT3_tdc = new TH1F("h_p2YmpT3_tdc", "Hodoscope Trigger (Slot 19 Channel 38) - S2Y; TDC Time (ns); Counts / 1 ns", 150, 0, 150);
  h_p1TmpT3_tdc = new TH1F("h_p1TmpT3_tdc", "Hodoscope Trigger (Slot 19 Channel 38) - S1; TDC Time (ns); Counts / 1 ns", 150, 0, 150);
  h_p2TmpT3_tdc = new TH1F("h_p2TmpT3_tdc", "Hodoscope Trigger (Slot 19 Channel 38) - S2; TDC Time (ns); Counts / 1 ns", 150, 0, 150);

  // ==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//

  h2_p1Xneg_pt_tt_diff = new TH2F("h2_p1Xneg_pt_tt_diff", "S1X- Pulse Time - TDC Time; Pulse Time - TDC Time (ns); Counts / 1 ns", nbars_1x, 0.5, nbars_1x + 0.5, 300, 0, 300);
  h2_p1Yneg_pt_tt_diff = new TH2F("h2_p1Yneg_pt_tt_diff", "S1Y- Pulse Time - TDC Time; Pulse Time - TDC Time (ns); Counts / 1 ns", nbars_1y, 0.5, nbars_1y + 0.5, 300, 0, 300);
  h2_p2Xneg_pt_tt_diff = new TH2F("h2_p2Xneg_pt_tt_diff", "S2X- Pulse Time - TDC Time; Pulse Time - TDC Time (ns); Counts / 1 ns", nbars_2x, 0.5, nbars_2x + 0.5, 300, 0, 300);
  h2_p2Yneg_pt_tt_diff = new TH2F("h2_p2Yneg_pt_tt_diff", "S2Y- Pulse Time - TDC Time; Pulse Time - TDC Time (ns); Counts / 1 ns", nbars_2y, 0.5, nbars_2y + 0.5, 300, 0, 300);
  h2_p1Xpos_pt_tt_diff = new TH2F("h2_p1Xpos_pt_tt_diff", "S1X- Pulse Time - TDC Time; Pulse Time - TDC Time (ns); Counts / 1 ns", nbars_1x, 0.5, nbars_1x + 0.5, 300, 0, 300);
  h2_p1Ypos_pt_tt_diff = new TH2F("h2_p1Ypos_pt_tt_diff", "S1Y- Pulse Time - TDC Time; Pulse Time - TDC Time (ns); Counts / 1 ns", nbars_1y, 0.5, nbars_1y + 0.5, 300, 0, 300);
  h2_p2Xpos_pt_tt_diff = new TH2F("h2_p2Xpos_pt_tt_diff", "S2X- Pulse Time - TDC Time; Pulse Time - TDC Time (ns); Counts / 1 ns", nbars_2x, 0.5, nbars_2x + 0.5, 300, 0, 300);
  h2_p2Ypos_pt_tt_diff = new TH2F("h2_p2Ypos_pt_tt_diff", "S2Y- Pulse Time - TDC Time; Pulse Time - TDC Time (ns); Counts / 1 ns", nbars_2y, 0.5, nbars_2y + 0.5, 300, 0, 300);

  // ==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//

  h_p1X_fpTime = new TH1F("h_p1X_fpTime", "S1X Focal Plane Time; TDC Time (ns); Counts / 1ns", 100, 0, 100);
  h_p1Y_fpTime = new TH1F("h_p1Y_fpTime", "S1Y Focal Plane Time; TDC Time (ns); Counts / 1ns", 100, 0, 100);
  h_p2X_fpTime = new TH1F("h_p2X_fpTime", "S2X Focal Plane Time; TDC Time (ns); Counts / 1ns", 100, 0, 100);
  h_p2Y_fpTime = new TH1F("h_p2Y_fpTime", "S2Y Focal Plane Time; TDC Time (ns); Counts / 1ns", 100, 0, 100);

  h_p1X_plTime = new TH1F("h_p1X_plTime", "S1X Plane Time; TDC Time (ns); Counts / 1ns", 120, -20, 100);
  h_p1Y_plTime = new TH1F("h_p1Y_plTime", "S1Y Plane Time; TDC Time (ns); Counts / 1ns", 120, -20, 100);
  h_p2X_plTime = new TH1F("h_p2X_plTime", "S2X Plane Time; TDC Time (ns); Counts / 1ns", 120, -20, 100);
  h_p2Y_plTime = new TH1F("h_p2Y_plTime", "S2Y Plane Time; TDC Time (ns); Counts / 1ns", 120, -20, 100);

  // ==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//

  h2_p1X_negTdcCorr = new TH2F("h2_p1X_negTdcCorr", "S1X- Corrected TDC Time vs. Paddle Number; Paddle Number; TDC Time (ns)", nbars_1x, 0.5, nbars_1x + 0.5, 200, -100, 100);
  h2_p1Y_negTdcCorr = new TH2F("h2_p1Y_negTdcCorr", "S1Y- Corrected TDC Time vs. Paddle Number; Paddle Number; TDC Time (ns)", nbars_1y, 0.5, nbars_1y + 0.5, 200, -100, 100);
  h2_p2X_negTdcCorr = new TH2F("h2_p2X_negTdcCorr", "S2X- Corrected TDC Time vs. Paddle Number; Paddle Number; TDC Time (ns)", nbars_2x, 0.5, nbars_2x + 0.5, 200, -100, 100);
  h2_p2Y_negTdcCorr = new TH2F("h2_p2Y_negTdcCorr", "S2Y- Corrected TDC Time vs. Paddle Number; Paddle Number; TDC Time (ns)", nbars_2y, 0.5, nbars_2y + 0.5, 200, -100, 100);
  h2_p1X_posTdcCorr = new TH2F("h2_p1X_posTdcCorr", "S1X+ Corrected TDC Time vs. Paddle Number; Paddle Number; TDC Time (ns)", nbars_1x, 0.5, nbars_1x + 0.5, 200, -100, 100);
  h2_p1Y_posTdcCorr = new TH2F("h2_p1Y_posTdcCorr", "S1Y+ Corrected TDC Time vs. Paddle Number; Paddle Number; TDC Time (ns)", nbars_1y, 0.5, nbars_1y + 0.5, 200, -100, 100);
  h2_p2X_posTdcCorr = new TH2F("h2_p2X_posTdcCorr", "S2X+ Corrected TDC Time vs. Paddle Number; Paddle Number; TDC Time (ns)", nbars_2x, 0.5, nbars_2x + 0.5, 200, -100, 100);
  h2_p2Y_posTdcCorr = new TH2F("h2_p2Y_posTdcCorr", "S2Y+ Corrected TDC Time vs. Paddle Number; Paddle Number; TDC Time (ns)", nbars_2y, 0.5, nbars_2y + 0.5, 200, -100, 100);
  h2_p1X_tdcCorrDiff = new TH2F("h2_p1X_tdcCorrDiff", "S1X-/S1X+ Corrected TDC Time Difference vs. Paddle Number; Paddle Number; TDC Time (ns)", nbars_1x, 0.5, nbars_1x + 0.5, 100, -50, 50);
  h2_p1Y_tdcCorrDiff = new TH2F("h2_p1Y_tdcCorrDiff", "S1Y-/S1Y+ Corrected TDC Time Difference vs. Paddle Number; Paddle Number; TDC Time (ns)", nbars_1y, 0.5, nbars_1y + 0.5, 100, -50, 50);
  h2_p2X_tdcCorrDiff = new TH2F("h2_p2X_tdcCorrDiff", "S2X-/S2X+ Corrected TDC Time Difference vs. Paddle Number; Paddle Number; TDC Time (ns)", nbars_2x, 0.5, nbars_2x + 0.5, 100, -50, 50);
  h2_p2Y_tdcCorrDiff = new TH2F("h2_p2Y_tdcCorrDiff", "S2Y-/S2Y+ Corrected TDC Time Difference vs. Paddle Number; Paddle Number; TDC Time (ns)", nbars_2y, 0.5, nbars_2y + 0.5, 100, -50, 50);

  // ==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//

  h2_paero_negPulseTime_pT1_diff = new TH2F("h2_paero_negPulseTime_pT1_diff", "SHMS Negative Aerogel ADC Pulse Time - T1; PMT Number; ADC Pulse Time - Trigger 1 Time (ns)", nneg_aero_pmts, 0.5, nneg_aero_pmts + 0.5, 300, -300, 0);
  h2_paero_negPulseTime_pT2_diff = new TH2F("h2_paero_negPulseTime_pT2_diff", "SHMS Negative Aerogel ADC Pulse Time - T2; PMT Number; ADC Pulse Time - Trigger 2 Time (ns)", nneg_aero_pmts, 0.5, nneg_aero_pmts + 0.5, 300, -300, 0);
  h2_paero_negPulseTime_pT3_diff = new TH2F("h2_paero_negPulseTime_pT3_diff", "SHMS Negative Aerogel ADC Pulse Time - T3; PMT Number; ADC Pulse Time - Trigger 3 Time (ns)", nneg_aero_pmts, 0.5, nneg_aero_pmts + 0.5, 300, -300, 0);
  h2_paero_posPulseTime_pT1_diff = new TH2F("h2_paero_posPulseTime_pT1_diff", "SHMS Positive Aerogel ADC Pulse Time - T1; PMT Number; ADC Pulse Time - Trigger 1 Time (ns)", npos_aero_pmts, 0.5, npos_aero_pmts + 0.5, 300, -300, 0);
  h2_paero_posPulseTime_pT2_diff = new TH2F("h2_paero_posPulseTime_pT2_diff", "SHMS Positive Aerogel ADC Pulse Time - T2; PMT Number; ADC Pulse Time - Trigger 2 Time (ns)", npos_aero_pmts, 0.5, npos_aero_pmts + 0.5, 300, -300, 0);
  h2_paero_posPulseTime_pT3_diff = new TH2F("h2_paero_posPulseTime_pT3_diff", "SHMS Positive Aerogel ADC Pulse Time - T3; PMT Number; ADC Pulse Time - Trigger 3 Time (ns)", npos_aero_pmts, 0.5, npos_aero_pmts + 0.5, 300, -300, 0);

  // ==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//

  h2_ppshwr_negPulseTime_pT1_diff = new TH2F("h2_ppshwr_negPulseTime_pT1_diff", "SHMS Negative Pre Shower ADC Pulse Time - T1; PMT Number; ADC Pulse Time - Trigger 1 Time (ns)", nneg_pshwr_blks, 0.5, nneg_pshwr_blks + 0.5, 300, -300, 0);
  h2_ppshwr_negPulseTime_pT2_diff = new TH2F("h2_ppshwr_negPulseTime_pT2_diff", "SHMS Negative Pre Shower ADC Pulse Time - T2; PMT Number; ADC Pulse Time - Trigger 2 Time (ns)", nneg_pshwr_blks, 0.5, nneg_pshwr_blks + 0.5, 300, -300, 0);
  h2_ppshwr_negPulseTime_pT3_diff = new TH2F("h2_ppshwr_negPulseTime_pT3_diff", "SHMS Negative Pre Shower ADC Pulse Time - T3; PMT Number; ADC Pulse Time - Trigger 3 Time (ns)", nneg_pshwr_blks, 0.5, nneg_pshwr_blks + 0.5, 300, -300, 0);
  h2_ppshwr_posPulseTime_pT1_diff = new TH2F("h2_ppshwr_posPulseTime_pT1_diff", "SHMS Positive Pre Shower ADC Pulse Time - T1; PMT Number; ADC Pulse Time - Trigger 1 Time (ns)", npos_pshwr_blks, 0.5, npos_pshwr_blks + 0.5, 300, -300, 0);
  h2_ppshwr_posPulseTime_pT2_diff = new TH2F("h2_ppshwr_posPulseTime_pT2_diff", "SHMS Positive Pre Shower ADC Pulse Time - T2; PMT Number; ADC Pulse Time - Trigger 2 Time (ns)", npos_pshwr_blks, 0.5, npos_pshwr_blks + 0.5, 300, -300, 0);
  h2_ppshwr_posPulseTime_pT3_diff = new TH2F("h2_ppshwr_posPulseTime_pT3_diff", "SHMS Positive Pre Shower ADC Pulse Time - T3; PMT Number; ADC Pulse Time - Trigger 3 Time (ns)", npos_pshwr_blks, 0.5, npos_pshwr_blks + 0.5, 300, -300, 0);

  // ==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//

  h2_pshwr_pulseTime_pT1_diff = new TH2F("h2_pshwr_pulseTime_pT1_diff", "SHMS Shower ADC Pulse Time - T1; PMT Number; ADC Pulse Time - Trigger 1 Time (ns)", nshwr_blks, 0.5, nshwr_blks + 0.5, 300, -300, 0);
  h2_pshwr_pulseTime_pT2_diff = new TH2F("h2_pshwr_pulseTime_pT2_diff", "SHMS Shower ADC Pulse Time - T2; PMT Number; ADC Pulse Time - Trigger 2 Time (ns)", nshwr_blks, 0.5, nshwr_blks + 0.5, 300, -300, 0);
  h2_pshwr_pulseTime_pT3_diff = new TH2F("h2_pshwr_pulseTime_pT3_diff", "SHMS Shower ADC Pulse Time - T3; PMT Number; ADC Pulse Time - Trigger 3 Time (ns)", nshwr_blks, 0.5, nshwr_blks + 0.5, 300, -300, 0);

  // ==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//

  h2_phgc_pulseTime_pT1_diff = new TH2F("h2_phgc_pulseTime_pT1_diff", "SHMS Heavy Gas Cherenkov ADC Pulse Time - T1; PMT Number; ADC Pulse Time - Trigger 1 Time (ns)", nhgc_pmts, 0.5, nhgc_pmts + 0.5, 300, -300, 0);
  h2_phgc_pulseTime_pT2_diff = new TH2F("h2_phgc_pulseTime_pT2_diff", "SHMS Heavy Gas Cherenkov ADC Pulse Time - T2; PMT Number; ADC Pulse Time - Trigger 2 Time (ns)", nhgc_pmts, 0.5, nhgc_pmts + 0.5, 300, -300, 0);
  h2_phgc_pulseTime_pT3_diff = new TH2F("h2_phgc_pulseTime_pT3_diff", "SHMS Heavy Gas Cherenkov ADC Pulse Time - T3; PMT Number; ADC Pulse Time - Trigger 3 Time (ns)", nhgc_pmts, 0.5, nhgc_pmts + 0.5, 300, -300, 0);

  // ==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//

  h2_pngc_pulseTime_pT1_diff = new TH2F("h2_pngc_pulseTime_pT1_diff", "SHMS Noble Gas Cherenkov ADC Pulse Time - T1; PMT Number; ADC Pulse Time - Trigger 1 Time (ns)", nngc_pmts, 0.5, nngc_pmts + 0.5, 300, -300, 0);
  h2_pngc_pulseTime_pT2_diff = new TH2F("h2_pngc_pulseTime_pT2_diff", "SHMS Noble Gas Cherenkov ADC Pulse Time - T2; PMT Number; ADC Pulse Time - Trigger 2 Time (ns)", nngc_pmts, 0.5, nngc_pmts + 0.5, 300, -300, 0);
  h2_pngc_pulseTime_pT3_diff = new TH2F("h2_pngc_pulseTime_pT3_diff", "SHMS Noble Gas Cherenkov ADC Pulse Time - T3; PMT Number; ADC Pulse Time - Trigger 3 Time (ns)", nngc_pmts, 0.5, nngc_pmts + 0.5, 300, -300, 0);

  // ==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//

  h2_pshwr_vs_phgcer = new TH2F ("h2_pshwr_vs_phgcer", "SHMS Shower vs. Heavy Gas Cherenkov; Number of HGC Photoelectrons / 1 Photoelectron; Shower Energy / 1 MeV", 500, 0, 500, 4000, 0, 4);
  h2_pshwr_vs_pngcer = new TH2F ("h2_pshwr_vs_pngcer", "SHMS Shower vs. Noble Gas Cherenkov; Number of NGC Photoelectrons / 1 Photoelectron; Shower Energy / 1 MeV", 500, 0, 500, 4000, 0, 4);
  h2_pshwr_vs_ppshwr = new TH2F ("h2_pshwr_vs_ppshwr", "SHMS Shower vs. Pre-Shower; Pre-Shower Energy / 1 MeV; Shower Energy / 1 MeV", 4000, 0, 4, 4000, 0, 4);
  h_paero_sum = new TH1F ("h_paero_sum", "SHMS Number of Aerogrel Photoelectrons; Number of Aerogel Photoelectrons; Counts / 1 Photoelectron", 500, 0, 500);
  h_ppshwr_sum = new TH1F ("h_ppshwr_sum", "SHMS Pre-Shower Energy; Pre-Shower Energy; Counts / 10 MeV", 4000, 0, 4);
  h_pshwr_sum = new TH1F ("h_pshwr_sum", "SHMS Shower Energy; Shower Energy; Counts / 10 MeV", 4000, 0, 4);
  h_ptotshwr_sum = new TH1F ("h_ptotshwr_sum", "SHMS Total Shower Energy; Total Shower Energy; Counts / 10 MeV", 1200, 0, 12);
  h_phgc_sum = new TH1F ("h_phgc_sum", "SHMS Number of HGC Photoelectrons; Number of HGC Photoelectrons; Counts / 1 Photoelectron", 500, 0, 500);
  h_pngc_sum = new TH1F ("h_pngc_sum", "SHMS Number of NGC Photoelectrons; Number of NGC Photoelectrons; Counts / 1 Photoelectron", 500, 0, 500);

  // ==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//

  h2_EdivP_vs_P = new TH2F ("h2_EdivP_vs_P", "SHMS E/p vs. p; p (geV); E/p", 600, 0, 6, 100, 0, 1);

  // =:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:

  // Loop of entries in tree
  for(UInt_t ievent = 0; ievent < nentries; ievent++) {

    T->GetEntry(ievent);

    if ((ievent)%1000 == 0)
      cout << "ievent = " << ievent << endl;

    if (ntracks != 1) continue;
    for (UInt_t itrack = 0; itrack < ntracks; itrack++) {
      
      if (TMath::Abs(trk_beta[itrack] - 1.) > 0.2) continue; 

      // cout << "========================" << endl;
      // cout << "Event Number = " << ievent << endl;
      // cout << "========================" << endl;

      // ==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==

      // Fill "good" hit histos
      for (UInt_t igoodhit = 0; igoodhit < p1X_nGoodHodoHits; igoodhit++) {
	h2_p1X_negTdcCorr->Fill(p1X_goodPaddle[igoodhit], p1X_goodNegTdcTimeCorr[igoodhit]);
	h2_p1X_posTdcCorr->Fill(p1X_goodPaddle[igoodhit], p1X_goodPosTdcTimeCorr[igoodhit]);
	h2_p1X_tdcCorrDiff->Fill(p1X_goodPaddle[igoodhit], p1X_goodPosTdcTimeCorr[igoodhit] - p1X_goodNegTdcTimeCorr[igoodhit]);
	if (TMath::Abs(p1X_goodPosTdcTimeCorr[igoodhit] - p1X_goodNegTdcTimeCorr[igoodhit]) > 0.1) continue;
	h_p1X_plTime->Fill(p1X_goodPosTdcTimeCorr[igoodhit]);
      }
      for (UInt_t igoodhit = 0; igoodhit < p1Y_nGoodHodoHits; igoodhit++) {
	h2_p1Y_negTdcCorr->Fill(p1Y_goodPaddle[igoodhit], p1Y_goodNegTdcTimeCorr[igoodhit]);
	h2_p1Y_posTdcCorr->Fill(p1Y_goodPaddle[igoodhit], p1Y_goodPosTdcTimeCorr[igoodhit]);
	h2_p1Y_tdcCorrDiff->Fill(p1Y_goodPaddle[igoodhit], p1Y_goodPosTdcTimeCorr[igoodhit] - p1Y_goodNegTdcTimeCorr[igoodhit]);
	if (TMath::Abs(p1Y_goodPosTdcTimeCorr[igoodhit] - p1Y_goodNegTdcTimeCorr[igoodhit]) > 0.1) continue;
	h_p1Y_plTime->Fill(p1Y_goodPosTdcTimeCorr[igoodhit]);
      }
      for (UInt_t igoodhit = 0; igoodhit < p2X_nGoodHodoHits; igoodhit++) {
	h2_p2X_negTdcCorr->Fill(p2X_goodPaddle[igoodhit], p2X_goodNegTdcTimeCorr[igoodhit]);
	h2_p2X_posTdcCorr->Fill(p2X_goodPaddle[igoodhit], p2X_goodPosTdcTimeCorr[igoodhit]);
	h2_p2X_tdcCorrDiff->Fill(p2X_goodPaddle[igoodhit], p2X_goodPosTdcTimeCorr[igoodhit] - p2X_goodNegTdcTimeCorr[igoodhit]);
	if (TMath::Abs(p2X_goodPosTdcTimeCorr[igoodhit] - p2X_goodNegTdcTimeCorr[igoodhit]) > 0.1) continue;
	h_p2X_plTime->Fill(p2X_goodPosTdcTimeCorr[igoodhit]);
      }
      for (UInt_t igoodhit = 0; igoodhit < p2Y_nGoodHodoHits; igoodhit++) {
	h2_p2Y_negTdcCorr->Fill(p2Y_goodPaddle[igoodhit], p2Y_goodNegTdcTimeCorr[igoodhit]);
	h2_p2Y_posTdcCorr->Fill(p2Y_goodPaddle[igoodhit], p2Y_goodPosTdcTimeCorr[igoodhit]);
	h2_p2Y_tdcCorrDiff->Fill(p2Y_goodPaddle[igoodhit], p2Y_goodPosTdcTimeCorr[igoodhit] - p2Y_goodNegTdcTimeCorr[igoodhit]);
	if (TMath::Abs(p2Y_goodPosTdcTimeCorr[igoodhit] - p2Y_goodNegTdcTimeCorr[igoodhit]) > 0.1) continue;
	h_p2Y_plTime->Fill(p2Y_goodPosTdcTimeCorr[igoodhit]);
      }
      // ==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==

      // Fill pulse/tdc time difference histos

      // cout << "Num of ADC Hits = " << p1X_negAdcHits << endl;
      // cout << "ADC Paddle Number = " << p1X_negAdcPaddle[iadchit] << endl;
      // cout << "Num of TDC Hits = " << p1X_negTdcHits << endl;
      // cout << "TDC Paddle Number = " << p1X_negTdcPaddle[itdchit] << endl;

      for (UInt_t iadchit = 0; iadchit < p1X_negAdcHits; iadchit++) {
	for (UInt_t itdchit = 0; itdchit < p1X_negTdcHits; itdchit++) {
	  if (p1X_negAdcPaddle[iadchit] != p1X_negTdcPaddle[itdchit]) continue;
	  h2_p1Xneg_pt_tt_diff->Fill(p1X_negAdcPaddle[iadchit], p1X_negAdcPulseTime[iadchit]*clk2adc - p1X_negTdcTime[itdchit]*clk2tdc);
	}
      }
      for (UInt_t iadchit = 0; iadchit < p1Y_negAdcHits; iadchit++) {
	for (UInt_t itdchit = 0; itdchit < p1Y_negTdcHits; itdchit++) {
	  if (p1Y_negAdcPaddle[iadchit] != p1Y_negTdcPaddle[itdchit]) continue;
	  h2_p1Yneg_pt_tt_diff->Fill(p1Y_negAdcPaddle[iadchit], p1Y_negAdcPulseTime[iadchit]*clk2adc - p1Y_negTdcTime[itdchit]*clk2tdc);
	}
      }
      for (UInt_t iadchit = 0; iadchit < p2X_negAdcHits; iadchit++) {
	for (UInt_t itdchit = 0; itdchit < p2X_negTdcHits; itdchit++) {
	  if (p2X_negAdcPaddle[iadchit] != p2X_negTdcPaddle[itdchit]) continue;
	  h2_p2Xneg_pt_tt_diff->Fill(p2X_negAdcPaddle[iadchit], p2X_negAdcPulseTime[iadchit]*clk2adc - p2X_negTdcTime[itdchit]*clk2tdc);
	}
      }
      for (UInt_t iadchit = 0; iadchit < p2Y_negAdcHits; iadchit++) {
	for (UInt_t itdchit = 0; itdchit < p2Y_negTdcHits; itdchit++) {
	  if (p2Y_negAdcPaddle[iadchit] != p2Y_negTdcPaddle[itdchit]) continue;
	  h2_p2Yneg_pt_tt_diff->Fill(p2Y_negAdcPaddle[iadchit], p2Y_negAdcPulseTime[iadchit]*clk2adc - p2Y_negTdcTime[itdchit]*clk2tdc);
	}
      }
      for (UInt_t iadchit = 0; iadchit < p1X_posAdcHits; iadchit++) {
	for (UInt_t itdchit = 0; itdchit < p1X_posTdcHits; itdchit++) {
	  if (p1X_posAdcPaddle[iadchit] != p1X_posTdcPaddle[itdchit]) continue;
	  h2_p1Xpos_pt_tt_diff->Fill(p1X_posAdcPaddle[iadchit], p1X_posAdcPulseTime[iadchit]*clk2adc - p1X_posTdcTime[itdchit]*clk2tdc);
	}
      }
      for (UInt_t iadchit = 0; iadchit < p1Y_posAdcHits; iadchit++) {
	for (UInt_t itdchit = 0; itdchit < p1Y_posTdcHits; itdchit++) {
	  if (p1Y_nGoodHodoHits < 1) continue;
	  if (p1Y_posAdcPaddle[iadchit] != p1Y_posTdcPaddle[itdchit]) continue;
	  h2_p1Ypos_pt_tt_diff->Fill(p1Y_posAdcPaddle[iadchit], p1Y_posAdcPulseTime[iadchit]*clk2adc - p1Y_posTdcTime[itdchit]*clk2tdc);
	}
      }
      for (UInt_t iadchit = 0; iadchit < p2X_posAdcHits; iadchit++) {
	for (UInt_t itdchit = 0; itdchit < p2X_posTdcHits; itdchit++) {
	  if (p2X_posAdcPaddle[iadchit] != p2X_posTdcPaddle[itdchit]) continue;
	  h2_p2Xpos_pt_tt_diff->Fill(p2X_posAdcPaddle[iadchit], p2X_posAdcPulseTime[iadchit]*clk2adc - p2X_posTdcTime[itdchit]*clk2tdc);
	}
      }
      for (UInt_t iadchit = 0; iadchit < p2Y_posAdcHits; iadchit++) {
	for (UInt_t itdchit = 0; itdchit < p2Y_posTdcHits; itdchit++) {
	  if (p2Y_posAdcPaddle[iadchit] != p2Y_posTdcPaddle[itdchit]) continue;
	  h2_p2Ypos_pt_tt_diff->Fill(p2Y_posAdcPaddle[iadchit], p2Y_posAdcPulseTime[iadchit]*clk2adc - p2Y_posTdcTime[itdchit]*clk2tdc);
	}
      }

      // ==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==

      // Fill trigger time histos
      if (p1X_tdcTime != 0.0) h_p1X_tdc->Fill(p1X_tdcTime*clk2tdc);
      if (p1Y_tdcTime != 0.0) h_p1Y_tdc->Fill(p1Y_tdcTime*clk2tdc);
      if (p2X_tdcTime != 0.0) h_p2X_tdc->Fill(p2X_tdcTime*clk2tdc);
      if (p2Y_tdcTime != 0.0) h_p2Y_tdc->Fill(p2Y_tdcTime*clk2tdc);
      if (p1T_tdcTime != 0.0) h_p1T_tdc->Fill(p1T_tdcTime*clk2tdc);
      if (p2T_tdcTime != 0.0) h_p2T_tdc->Fill(p2T_tdcTime*clk2tdc);

      h_pT1_tdc->Fill(pT1_tdcTime*clk2tdc);
      h_pT2_tdc->Fill(pT2_tdcTime*clk2tdc);
      h_pT3_tdc->Fill(pT3_tdcTime*clk2tdc);

      for (UInt_t iref = 0; iref < ndcRefTimes; iref++)
	h_pDCREF_tdc[iref]->Fill(pDCREF_tdcTime[iref]*clk2tdc);

      if (p1X_tdcTime != 0.0) h_p1XmpT2_tdc->Fill((pT2_tdcTime - p1X_tdcTime)*clk2tdc);
      if (p1Y_tdcTime != 0.0) h_p1YmpT2_tdc->Fill((pT2_tdcTime - p1Y_tdcTime)*clk2tdc);
      if (p2X_tdcTime != 0.0) h_p2XmpT2_tdc->Fill((pT2_tdcTime - p2X_tdcTime)*clk2tdc);
      if (p2Y_tdcTime != 0.0) h_p2YmpT2_tdc->Fill((pT2_tdcTime - p2Y_tdcTime)*clk2tdc);
      if (p1T_tdcTime != 0.0) h_p1TmpT2_tdc->Fill((pT2_tdcTime - p1T_tdcTime)*clk2tdc);
      if (p2T_tdcTime != 0.0) h_p2TmpT2_tdc->Fill((pT2_tdcTime - p2T_tdcTime)*clk2tdc);

      if (p1X_tdcTime != 0.0) h_p1XmpT3_tdc->Fill((pT3_tdcTime - p1X_tdcTime)*clk2tdc);
      if (p1Y_tdcTime != 0.0) h_p1YmpT3_tdc->Fill((pT3_tdcTime - p1Y_tdcTime)*clk2tdc);
      if (p2X_tdcTime != 0.0) h_p2XmpT3_tdc->Fill((pT3_tdcTime - p2X_tdcTime)*clk2tdc);
      if (p2Y_tdcTime != 0.0) h_p2YmpT3_tdc->Fill((pT3_tdcTime - p2Y_tdcTime)*clk2tdc);
      if (p1T_tdcTime != 0.0) h_p1TmpT3_tdc->Fill((pT3_tdcTime - p1T_tdcTime)*clk2tdc);
      if (p2T_tdcTime != 0.0) h_p2TmpT3_tdc->Fill((pT3_tdcTime - p2T_tdcTime)*clk2tdc);

      // ==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==

      h_p1X_fpTime->Fill(p1X_fpTime); h_p1Y_fpTime->Fill(p1Y_fpTime);
      h_p2X_fpTime->Fill(p2X_fpTime); h_p2Y_fpTime->Fill(p2Y_fpTime);

      // ==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==

      Double_t paero_sum = 0.0;
      for (UInt_t iaerohit = 0; iaerohit < paero_negHits; iaerohit++) {
	h2_paero_negPulseTime_pT1_diff->Fill(paero_negPmt[iaerohit], paero_negPulseTime[iaerohit]*clk2adc - pT1_tdcTime*clk2tdc);
	h2_paero_negPulseTime_pT2_diff->Fill(paero_negPmt[iaerohit], paero_negPulseTime[iaerohit]*clk2adc - pT2_tdcTime*clk2tdc);
	h2_paero_negPulseTime_pT3_diff->Fill(paero_negPmt[iaerohit], paero_negPulseTime[iaerohit]*clk2adc - pT3_tdcTime*clk2tdc);
	paero_sum += paero_negPulseInt[iaerohit]*aero_neg_adc2npe[iaerohit];
      }
      for (UInt_t iaerohit = 0; iaerohit < paero_posHits; iaerohit++) {
	h2_paero_posPulseTime_pT1_diff->Fill(paero_posPmt[iaerohit], paero_posPulseTime[iaerohit]*clk2adc - pT1_tdcTime*clk2tdc);
	h2_paero_posPulseTime_pT2_diff->Fill(paero_posPmt[iaerohit], paero_posPulseTime[iaerohit]*clk2adc - pT2_tdcTime*clk2tdc);
	h2_paero_posPulseTime_pT3_diff->Fill(paero_posPmt[iaerohit], paero_posPulseTime[iaerohit]*clk2adc - pT3_tdcTime*clk2tdc);
	paero_sum += paero_posPulseInt[iaerohit]*aero_pos_adc2npe[iaerohit];
      }
      if (paero_sum != 0.0) h_paero_sum->Fill(paero_sum);

      // ==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==

      Double_t ppshwr_sum = 0.0;
      for (UInt_t ipshwrhit = 0; ipshwrhit < ppshwr_negHits; ipshwrhit++) {
	h2_ppshwr_negPulseTime_pT1_diff->Fill(ppshwr_negPmt[ipshwrhit], ppshwr_negPulseTime[ipshwrhit]*clk2adc - pT1_tdcTime*clk2tdc);
	h2_ppshwr_negPulseTime_pT2_diff->Fill(ppshwr_negPmt[ipshwrhit], ppshwr_negPulseTime[ipshwrhit]*clk2adc - pT2_tdcTime*clk2tdc);
	h2_ppshwr_negPulseTime_pT3_diff->Fill(ppshwr_negPmt[ipshwrhit], ppshwr_negPulseTime[ipshwrhit]*clk2adc - pT3_tdcTime*clk2tdc);
	ppshwr_sum += ppshwr_negPulseInt[ipshwrhit]*pshwr_neg_adc2GeV[ipshwrhit];
      }
      for (UInt_t ipshwrhit = 0; ipshwrhit < ppshwr_posHits; ipshwrhit++) {
	h2_ppshwr_posPulseTime_pT1_diff->Fill(ppshwr_posPmt[ipshwrhit], ppshwr_posPulseTime[ipshwrhit]*clk2adc - pT1_tdcTime*clk2tdc);
	h2_ppshwr_posPulseTime_pT2_diff->Fill(ppshwr_posPmt[ipshwrhit], ppshwr_posPulseTime[ipshwrhit]*clk2adc - pT2_tdcTime*clk2tdc);
	h2_ppshwr_posPulseTime_pT3_diff->Fill(ppshwr_posPmt[ipshwrhit], ppshwr_posPulseTime[ipshwrhit]*clk2adc - pT3_tdcTime*clk2tdc);
	ppshwr_sum += ppshwr_posPulseInt[ipshwrhit]*pshwr_pos_adc2GeV[ipshwrhit];
      }
      if (ppshwr_sum != 0.0) h_ppshwr_sum->Fill(ppshwr_sum);


      // ==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==

      Double_t pshwr_sum = 0.0;
      for (UInt_t ishwrhit = 0; ishwrhit < pshwr_hits; ishwrhit++) {
	h2_pshwr_pulseTime_pT1_diff->Fill(pshwr_pmt[ishwrhit], pshwr_pulseTime[ishwrhit]*clk2adc - pT1_tdcTime*clk2tdc);
	h2_pshwr_pulseTime_pT2_diff->Fill(pshwr_pmt[ishwrhit], pshwr_pulseTime[ishwrhit]*clk2adc - pT2_tdcTime*clk2tdc);
	h2_pshwr_pulseTime_pT3_diff->Fill(pshwr_pmt[ishwrhit], pshwr_pulseTime[ishwrhit]*clk2adc - pT3_tdcTime*clk2tdc);
	pshwr_sum += pshwr_pulseInt[ishwrhit]*shwr_adc2GeV;
      }
      if (pshwr_sum != 0.0) h_pshwr_sum->Fill(pshwr_sum);
      if ((ppshwr_sum + pshwr_sum) != 0.0) h_ptotshwr_sum->Fill(ppshwr_sum + pshwr_sum);
      if (ppshwr_sum != 0.0 && pshwr_sum != 0.0) {
	h2_pshwr_vs_ppshwr->Fill(ppshwr_sum, pshwr_sum);   
      }
    
      for (UInt_t itrack = 0; itrack < ntracks; itrack++)
	if (trk_pmag[itrack] != 0 && ntracks == 1) h2_EdivP_vs_P->Fill(trk_pmag[0], (ppshwr_sum + pshwr_sum)/trk_pmag[0]);

      // ==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==

      Double_t phgc_sum = 0.0;
      for (UInt_t ihgchit = 0; ihgchit < phgc_hits; ihgchit++) {
	h2_phgc_pulseTime_pT1_diff->Fill(phgc_pmt[ihgchit], phgc_pulseTime[ihgchit]*clk2adc - pT1_tdcTime*clk2tdc);
	h2_phgc_pulseTime_pT2_diff->Fill(phgc_pmt[ihgchit], phgc_pulseTime[ihgchit]*clk2adc - pT2_tdcTime*clk2tdc);
	h2_phgc_pulseTime_pT3_diff->Fill(phgc_pmt[ihgchit], phgc_pulseTime[ihgchit]*clk2adc - pT3_tdcTime*clk2tdc);
	phgc_sum += phgc_pulseInt[ihgchit]*hgc_adc2npe[ihgchit];
      }
      if (phgc_sum != 0.0) h_phgc_sum->Fill(phgc_sum);
      if (phgc_sum != 0.0 && (ppshwr_sum + pshwr_sum) != 0.0) h2_pshwr_vs_phgcer->Fill(phgc_sum, ppshwr_sum + pshwr_sum);

      // ==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==

      Double_t pngc_sum = 0.0;
      for (UInt_t ingchit = 0; ingchit < pngc_hits; ingchit++) {
	h2_pngc_pulseTime_pT1_diff->Fill(pngc_pmt[ingchit], pngc_pulseTime[ingchit]*clk2adc - pT1_tdcTime*clk2tdc);
	h2_pngc_pulseTime_pT2_diff->Fill(pngc_pmt[ingchit], pngc_pulseTime[ingchit]*clk2adc - pT2_tdcTime*clk2tdc);
	h2_pngc_pulseTime_pT3_diff->Fill(pngc_pmt[ingchit], pngc_pulseTime[ingchit]*clk2adc - pT3_tdcTime*clk2tdc);
	pngc_sum += pngc_pulseInt[ingchit]*ngc_adc2npe[ingchit];
      }
      if (pngc_sum != 0.0) h_pngc_sum->Fill(pngc_sum);
      if (pngc_sum != 0.0 && (ppshwr_sum + pshwr_sum) != 0.0) h2_pshwr_vs_pngcer->Fill(pngc_sum, ppshwr_sum + pshwr_sum);

      // ==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==//==

    }
  }  // Entries loop
}  // UserScript function

void kpp_analysis(TString histname) {

  // Grab the histo
  TH1F *h1d;
  TH2F *h2d;

  h1d = dynamic_cast <TH1F*> (gDirectory->Get(histname));
  h2d = dynamic_cast <TH2F*> (gDirectory->Get(histname));

  // Grab histo directly if it does not already exist
  if(!h1d && !h2d) {
    UserScript();
    h1d = (TH1F*) (gDirectory->Get(histname));
    h2d = (TH2F*) (gDirectory->Get(histname));
    // Throw error
    if(!h1d || !h2d) {
      cout << "User histogram " << histname << " not found" << endl;
      exit(1);
    }
  }
  //else
  if (h2d) {
    h2d->SetStats(0);
    h2d->Draw("colz");
  }
  else {
    h1d->SetStats(0);
    h1d->Draw();
  }
}  // kpp_analysis function
