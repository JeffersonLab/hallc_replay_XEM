
void UserScript() {

  // Declare constants
  static const Double_t clk2tdc     = 0.100;
  static const Double_t clk2adc     = 0.0625;
  static const UInt_t   ndcRefTimes = 10;

  static const UInt_t nbars_1x = 13;
  static const UInt_t nbars_1y = 13;
  static const UInt_t nbars_2x = 14;
  static const UInt_t nbars_2y = 21;

  static const UInt_t maxTdcHits = 128;
  static const UInt_t maxAdcHits = 4;

  // =:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:

  // Declare variables
  Double_t p1X_tdcTime, p1Y_tdcTime, p2X_tdcTime, p2Y_tdcTime;
  Double_t p1T_tdcTime, p2T_tdcTime;
  Double_t pT1_tdcTime, pT2_tdcTime, pT3_tdcTime;
  Double_t pDCREF_tdcTime[ndcRefTimes];

  // Negative hodoscope hits
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

  TH1F *h_p1X_fpTime, *h_p1Y_fpTime, *h_p2X_fpTime, *h_p2Y_fpTime;

  TH2F *h2_p1X_negTdcCorr, *h2_p1Y_negTdcCorr, *h2_p2X_negTdcCorr, *h2_p2Y_negTdcCorr;
  TH2F *h2_p1X_posTdcCorr, *h2_p1Y_posTdcCorr, *h2_p2X_posTdcCorr, *h2_p2Y_posTdcCorr;

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
  // DC reference times
  for (UInt_t iref = 0; iref < ndcRefTimes; iref++)
    T->SetBranchAddress(Form("T.shms.pDCREF%d_tdcTime", iref+1), &pDCREF_tdcTime[iref]);
  // Hodoscope focal plane time calculation
  T->SetBranchAddress("P.hod.1x.fptime", &p1X_fpTime);
  T->SetBranchAddress("P.hod.1y.fptime", &p1Y_fpTime);
  T->SetBranchAddress("P.hod.2x.fptime", &p2X_fpTime);
  T->SetBranchAddress("P.hod.2y.fptime", &p2Y_fpTime);
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

  // =:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:
          
  // Create histos
  h_p1X_tdc = new TH1F("h_p1X_tdc", "S1X Coincidence Time; TDC Time (ns); Counts / 1 ns", 200, 0, 200);
  h_p1Y_tdc = new TH1F("h_p1Y_tdc", "S1Y Coincidence Time; TDC Time (ns); Counts / 1 ns", 200, 0, 200);
  h_p2X_tdc = new TH1F("h_p2X_tdc", "S2X Coincidence Time; TDC Time (ns); Counts / 1 ns", 200, 0, 200);
  h_p2Y_tdc = new TH1F("h_p2Y_tdc", "S2Y Coincidence Time; TDC Time (ns); Counts / 1 ns", 200, 0, 200);
  h_p1T_tdc = new TH1F("h_p1T_tdc", "S1X/S1Y Coincidence Time; TDC Time (ns); Counts / 1 ns", 200, 0, 200);
  h_p2T_tdc = new TH1F("h_p2T_tdc", "S2X/S2Y Coincidence Time; TDC Time (ns); Counts / 1 ns", 200, 0, 200);
  h_pT1_tdc = new TH1F("h_pT1_tdc", "Hodoscope Trigger (Slot 20 Channel 15); TDC Time (ns); Counts / 1 ns", 200, 0, 200);
  h_pT2_tdc = new TH1F("h_pT2_tdc", "Hodoscope Trigger (Slot 19 Channel 31); TDC Time (ns); Counts / 1 ns", 200, 0, 200);
  h_pT3_tdc = new TH1F("h_pT3_tdc", "Hodoscope Trigger (Slot 19 Channel 38); TDC Time (ns); Counts / 1 ns", 200, 0, 200);
  for (UInt_t iref = 0; iref < ndcRefTimes; iref++)
    h_pDCREF_tdc[iref] = new TH1F(Form("h_pDCREF%d_tdc", iref+1), Form("DC Reference Time %d; TDC Time (ns); Counts / 1 ns", iref+1), 200, 1200, 1400);
  h_p1XmpT2_tdc = new TH1F("h_p1XmpT2_tdc", "Hodoscope Trigger (Slot 19 Channel 31) - S1X; TDC Time (ns); Counts / 1 ns", 200, 0, 200);
  h_p1YmpT2_tdc = new TH1F("h_p1YmpT2_tdc", "Hodoscope Trigger (Slot 19 Channel 31) - S1Y; TDC Time (ns); Counts / 1 ns", 200, 0, 200);
  h_p2XmpT2_tdc = new TH1F("h_p2XmpT2_tdc", "Hodoscope Trigger (Slot 19 Channel 31) - S2X; TDC Time (ns); Counts / 1 ns", 200, 0, 200);
  h_p2YmpT2_tdc = new TH1F("h_p2YmpT2_tdc", "Hodoscope Trigger (Slot 19 Channel 31) - S2Y; TDC Time (ns); Counts / 1 ns", 200, 0, 200);
  h_p1TmpT2_tdc = new TH1F("h_p1TmpT2_tdc", "Hodoscope Trigger (Slot 19 Channel 31) - S1; TDC Time (ns); Counts / 1 ns", 200, 0, 200);
  h_p2TmpT2_tdc = new TH1F("h_p2TmpT2_tdc", "Hodoscope Trigger (Slot 19 Channel 31) - S2; TDC Time (ns); Counts / 1 ns", 200, 0, 200);
  h_p1XmpT3_tdc = new TH1F("h_p1XmpT3_tdc", "Hodoscope Trigger (Slot 19 Channel 38) - S1X; TDC Time (ns); Counts / 1 ns", 200, 0, 200);
  h_p1YmpT3_tdc = new TH1F("h_p1YmpT3_tdc", "Hodoscope Trigger (Slot 19 Channel 38) - S1Y; TDC Time (ns); Counts / 1 ns", 200, 0, 200);
  h_p2XmpT3_tdc = new TH1F("h_p2XmpT3_tdc", "Hodoscope Trigger (Slot 19 Channel 38) - S2X; TDC Time (ns); Counts / 1 ns", 200, 0, 200);
  h_p2YmpT3_tdc = new TH1F("h_p2YmpT3_tdc", "Hodoscope Trigger (Slot 19 Channel 38) - S2Y; TDC Time (ns); Counts / 1 ns", 200, 0, 200);
  h_p1TmpT3_tdc = new TH1F("h_p1TmpT3_tdc", "Hodoscope Trigger (Slot 19 Channel 38) - S1; TDC Time (ns); Counts / 1 ns", 200, 0, 200);
  h_p2TmpT3_tdc = new TH1F("h_p2TmpT3_tdc", "Hodoscope Trigger (Slot 19 Channel 38) - S2; TDC Time (ns); Counts / 1 ns", 200, 0, 200);
  h2_p1Xneg_pt_tt_diff = new TH2F("h2_p1Xneg_pt_tt_diff", "S1X- Pulse Time - TDC Time; Pulse Time - TDC Time (ns); Counts / 1 ns", nbars_1x, 0.5, nbars_1x + 0.5, 500, -250, 250);
  h2_p1Yneg_pt_tt_diff = new TH2F("h2_p1Yneg_pt_tt_diff", "S1Y- Pulse Time - TDC Time; Pulse Time - TDC Time (ns); Counts / 1 ns", nbars_1y, 0.5, nbars_1y + 0.5, 500, -250, 250);
  h2_p2Xneg_pt_tt_diff = new TH2F("h2_p2Xneg_pt_tt_diff", "S2X- Pulse Time - TDC Time; Pulse Time - TDC Time (ns); Counts / 1 ns", nbars_2x, 0.5, nbars_2x + 0.5, 500, -250, 250);
  h2_p2Yneg_pt_tt_diff = new TH2F("h2_p2Yneg_pt_tt_diff", "S2Y- Pulse Time - TDC Time; Pulse Time - TDC Time (ns); Counts / 1 ns", nbars_2y, 0.5, nbars_2y + 0.5, 500, -250, 250);
  h_p1X_fpTime = new TH1F("h_p1X_fpTime", "S1X Focal Plane Time; TDC Time (ns); Counts / 1ns", 100, 0, 100);
  h_p1Y_fpTime = new TH1F("h_p1Y_fpTime", "S1Y Focal Plane Time; TDC Time (ns); Counts / 1ns", 100, 0, 100);
  h_p2X_fpTime = new TH1F("h_p2X_fpTime", "S2X Focal Plane Time; TDC Time (ns); Counts / 1ns", 100, 0, 100);
  h_p2Y_fpTime = new TH1F("h_p2Y_fpTime", "S2Y Focal Plane Time; TDC Time (ns); Counts / 1ns", 100, 0, 100);
  h2_p1X_negTdcCorr = new TH2F("h2_p1X_negTdcCorr", "S1X- Corrected TDC Time vs. Paddle Number; Paddle Number; TDC Time (ns)", nbars_1x, 0.5, nbars_1x + 0.5, 100, 0, 100);
  h2_p1Y_negTdcCorr = new TH2F("h2_p1Y_negTdcCorr", "S1Y- Corrected TDC Time vs. Paddle Number; Paddle Number; TDC Time (ns)", nbars_1y, 0.5, nbars_1y + 0.5, 100, 0, 100);
  h2_p2X_negTdcCorr = new TH2F("h2_p2X_negTdcCorr", "S2X- Corrected TDC Time vs. Paddle Number; Paddle Number; TDC Time (ns)", nbars_2x, 0.5, nbars_2x + 0.5, 100, 0, 100);
  h2_p2Y_negTdcCorr = new TH2F("h2_p2Y_negTdcCorr", "S2Y- Corrected TDC Time vs. Paddle Number; Paddle Number; TDC Time (ns)", nbars_2y, 0.5, nbars_2y + 0.5, 100, 0, 100);
  h2_p1X_posTdcCorr = new TH2F("h2_p1X_posTdcCorr", "S1X+ Corrected TDC Time vs. Paddle Number; Paddle Number; TDC Time (ns)", nbars_1x, 0.5, nbars_1x + 0.5, 100, 0, 100);
  h2_p1Y_posTdcCorr = new TH2F("h2_p1Y_posTdcCorr", "S1Y+ Corrected TDC Time vs. Paddle Number; Paddle Number; TDC Time (ns)", nbars_1y, 0.5, nbars_1y + 0.5, 100, 0, 100);
  h2_p2X_posTdcCorr = new TH2F("h2_p2X_posTdcCorr", "S2X+ Corrected TDC Time vs. Paddle Number; Paddle Number; TDC Time (ns)", nbars_2x, 0.5, nbars_2x + 0.5, 100, 0, 100);
  h2_p2Y_posTdcCorr = new TH2F("h2_p2Y_posTdcCorr", "S2Y+ Corrected TDC Time vs. Paddle Number; Paddle Number; TDC Time (ns)", nbars_2y, 0.5, nbars_2y + 0.5, 100, 0, 100);

  // =:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:

  // Loop of entries in tree
  for(UInt_t ievent = 0; ievent < nentries; ievent++) {
    
    T->GetEntry(ievent);

    if (p1X_nGoodHodoHits != 1 || p1Y_nGoodHodoHits != 1 || p2X_nGoodHodoHits != 1  || p2Y_nGoodHodoHits != 1) continue;

    // cout << "Event number = " << ievent << endl;
    for (UInt_t igoodhit = 0; igoodhit < p1X_nGoodHodoHits; igoodhit++) {
    //   // cout << "Number of good hits in 1x = " << p1X_nGoodHodoHits << endl;
    //   // cout << "The paddle which was hit = " << p1X_goodPaddle[igoodhit] << endl;
    //   // cout << "Negative TDC corrected time = " << p1X_goodNegTdcTimeCorr[igoodhit] << endl;
    //   // cout << "Positive TDC corrected time = " << p1X_goodPosTdcTimeCorr[igoodhit] << endl;
      h2_p1X_negTdcCorr->Fill(p1X_goodPaddle[igoodhit], p1X_goodNegTdcTimeCorr[igoodhit]);
      h2_p1X_posTdcCorr->Fill(p1X_goodPaddle[igoodhit], p1X_goodPosTdcTimeCorr[igoodhit]);
    }
    for (UInt_t igoodhit = 0; igoodhit < p1Y_nGoodHodoHits; igoodhit++) {
      h2_p1Y_negTdcCorr->Fill(p1Y_goodPaddle[igoodhit], p1Y_goodNegTdcTimeCorr[igoodhit]);
      h2_p1Y_posTdcCorr->Fill(p1Y_goodPaddle[igoodhit], p1Y_goodPosTdcTimeCorr[igoodhit]);
    }
    for (UInt_t igoodhit = 0; igoodhit < p2X_nGoodHodoHits; igoodhit++) {
      h2_p2X_negTdcCorr->Fill(p2X_goodPaddle[igoodhit], p2X_goodNegTdcTimeCorr[igoodhit]);
      h2_p2X_posTdcCorr->Fill(p2X_goodPaddle[igoodhit], p2X_goodPosTdcTimeCorr[igoodhit]);
    }
    for (UInt_t igoodhit = 0; igoodhit < p2Y_nGoodHodoHits; igoodhit++) {
      h2_p2Y_negTdcCorr->Fill(p2Y_goodPaddle[igoodhit], p2Y_goodNegTdcTimeCorr[igoodhit]);
      h2_p2Y_posTdcCorr->Fill(p2Y_goodPaddle[igoodhit], p2Y_goodPosTdcTimeCorr[igoodhit]);
    }

    if (p1X_negAdcHits > 0 && p1X_negTdcHits > 0) {
      for (UInt_t iadchit = 0; iadchit < p1X_negAdcHits; iadchit++) {
    	for (UInt_t itdchit = 0; itdchit < p1X_negTdcHits; itdchit++) {
    	  if (p1X_negAdcPaddle[iadchit] != p1X_negTdcPaddle[itdchit]) continue;
    	  h2_p1Xneg_pt_tt_diff->Fill(p1X_negAdcPaddle[iadchit], (p1X_negAdcPulseTime[iadchit]*clk2adc - p1X_negTdcTime[itdchit]*clk2adc));
    	}
      }
    }
           
    // Fill histos
    h_p1X_tdc->Fill(p1X_tdcTime*clk2tdc); h_p1Y_tdc->Fill(p1Y_tdcTime*clk2tdc);
    h_p2X_tdc->Fill(p2X_tdcTime*clk2tdc); h_p2Y_tdc->Fill(p2Y_tdcTime*clk2tdc);
    h_p1T_tdc->Fill(p1T_tdcTime*clk2tdc); h_p2T_tdc->Fill(p2T_tdcTime*clk2tdc);
    h_pT1_tdc->Fill(pT1_tdcTime*clk2tdc); h_pT2_tdc->Fill(pT2_tdcTime*clk2tdc); h_pT3_tdc->Fill(pT3_tdcTime*clk2tdc);
    for (UInt_t iref = 0; iref < ndcRefTimes; iref++)
      h_pDCREF_tdc[iref]->Fill(pDCREF_tdcTime[iref]*clk2tdc);
    h_p1XmpT2_tdc->Fill((pT2_tdcTime - p1X_tdcTime)*clk2tdc); h_p1YmpT2_tdc->Fill((pT2_tdcTime - p1Y_tdcTime)*clk2tdc);
    h_p2XmpT2_tdc->Fill((pT2_tdcTime - p2X_tdcTime)*clk2tdc); h_p2YmpT2_tdc->Fill((pT2_tdcTime - p2Y_tdcTime)*clk2tdc);
    h_p1TmpT2_tdc->Fill((pT2_tdcTime - p1T_tdcTime)*clk2tdc); h_p2TmpT2_tdc->Fill((pT2_tdcTime - p2T_tdcTime)*clk2tdc);
    h_p1XmpT3_tdc->Fill((pT3_tdcTime - p1X_tdcTime)*clk2tdc); h_p1YmpT3_tdc->Fill((pT3_tdcTime - p1Y_tdcTime)*clk2tdc);
    h_p2XmpT3_tdc->Fill((pT3_tdcTime - p2X_tdcTime)*clk2tdc); h_p2YmpT3_tdc->Fill((pT3_tdcTime - p2Y_tdcTime)*clk2tdc);
    h_p1TmpT3_tdc->Fill((pT3_tdcTime - p1T_tdcTime)*clk2tdc); h_p2TmpT3_tdc->Fill((pT3_tdcTime - p2T_tdcTime)*clk2tdc);
    //h_p1Xneg_pt_tt_diff->Fill(p1X_negAdcPulseTime*clk2adc - p1X_negTdcTime); h_p1Yneg_pt_tt_diff->Fill(p1Y_negAdcPulseTime*clk2adc - p1Y_negTdcTime);
    //h_p2Xneg_pt_tt_diff->Fill(p2X_negAdcPulseTime*clk2adc - p2X_negTdcTime); h_p2Yneg_pt_tt_diff->Fill(p2Y_negAdcPulseTime*clk2adc - p2Y_negTdcTime);
    h_p1X_fpTime->Fill(p1X_fpTime); h_p1Y_fpTime->Fill(p1Y_fpTime);
    h_p2X_fpTime->Fill(p2X_fpTime); h_p2Y_fpTime->Fill(p2Y_fpTime);

    // for (UInt_t ihit = 0; ihit < p1X_negAdcHits; ihit++){
    //   //T->SetBranchAddress("P.hod.1x.negAdcCounter", &p1X_negAdcPaddle[ihit]);
    // }

  }  // Entries loop
}  // UserScript function

void kpp_analysis(TString histname) {

  // Grab the histo
  TH1F *h;  h  = dynamic_cast <TH1F*> gDirectory->Get(histname);
  
  // Grab histo directly if it does not already exist
  if(!h) {
    
    UserScript();
    h = (TH1F*) gDirectory->Get(histname);

    // Throw error
    if(!h) { cout << "User histogram " << histname << " not found" << endl; exit(1);}
  }  // Histo existing condition
  //else 
  h->Draw("colz");

}  // kpp_analysis function
