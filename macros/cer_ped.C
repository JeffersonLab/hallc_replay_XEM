void cer_ped() {

  // Declare constants
  static const Double_t clk2tdc = 0.100;
  static const Double_t clk2adc = 0.0625;

  static const UInt_t nhgc_pmts = 4;
  static const UInt_t nngc_pmts = 4;

  static const UInt_t maxTdcHits = 128;
  static const UInt_t maxAdcHits = 4;

  // Heavy gas Cherenkov ADC variables
  Int_t phgc_hits;
  Double_t phgc_pmt[maxAdcHits*nhgc_pmts], phgc_pulsePedRaw[maxAdcHits*nhgc_pmts];
  Double_t phgc_pulseIntRaw[maxAdcHits*nhgc_pmts], phgc_pulseAmpRaw[maxAdcHits*nhgc_pmts];
  Double_t phgc_pulsePed[maxAdcHits*nhgc_pmts], phgc_pulseInt[maxAdcHits*nhgc_pmts];
  Double_t phgc_pulseAmp[maxAdcHits*nhgc_pmts], phgc_pulseTime[maxAdcHits*nhgc_pmts];

  // Noble gas Cherenkov ADC variables
  Int_t pngc_hits;
  Double_t pngc_pmt[maxAdcHits*nngc_pmts], pngc_pulsePedRaw[maxAdcHits*nngc_pmts];
  Double_t pngc_pulseIntRaw[maxAdcHits*nngc_pmts], pngc_pulseAmpRaw[maxAdcHits*nngc_pmts];
  Double_t pngc_pulsePed[maxAdcHits*nngc_pmts], pngc_pulseInt[maxAdcHits*nngc_pmts];
  Double_t pngc_pulseAmp[maxAdcHits*nngc_pmts], pngc_pulseTime[maxAdcHits*nngc_pmts];

  // Trigger time
  Double_t pT1_tdcTime;

  // Declare Histos
  TH2F *h2_phgc_pulsePedRaw_vs_pmt, *h2_phgc_pulseIntRaw_vs_pmt, *h2_phgc_pulseAmpRaw_vs_pmt;
  TH2F *h2_phgc_pulsePed_vs_pmt, *h2_phgc_pulseInt_vs_pmt, *h2_phgc_pulseAmp_vs_pmt, *h2_phgc_pulseTimeCorr_vs_pmt;

  TH2F *h2_pngc_pulsePedRaw_vs_pmt, *h2_pngc_pulseIntRaw_vs_pmt, *h2_pngc_pulseAmpRaw_vs_pmt;
  TH2F *h2_pngc_pulsePed_vs_pmt, *h2_pngc_pulseInt_vs_pmt, *h2_pngc_pulseAmp_vs_pmt, *h2_pngc_pulseTimeCorr_vs_pmt;

  // Declare ROOT files
  TFile *rif = new TFile("ROOTfiles/shms_replay_488_-1.root");
  TFile *rof = new TFile("cer_ped.root", "RECREATE");

  // Declare tree
  TTree *T = rif->Get("T");
  Long64_t nentries; 
    
  // Acquire the number of entries
  //nentries = T->GetEntries();
  nentries = 100000; 

  // Acquire the branches
  T->SetBranchAddress("Ndata.P.hgcer.adcCounter", &phgc_hits);
  T->SetBranchAddress("P.hgcer.adcCounter", phgc_pmt);
  T->SetBranchAddress("P.hgcer.adcPedRaw", phgc_pulsePedRaw);
  T->SetBranchAddress("P.hgcer.adcPed", phgc_pulsePed);
  T->SetBranchAddress("P.hgcer.adcPulseIntRaw", phgc_pulseIntRaw);
  T->SetBranchAddress("P.hgcer.adcPulseInt", phgc_pulseInt);
  T->SetBranchAddress("P.hgcer.adcPulseAmpRaw", phgc_pulseAmpRaw);
  T->SetBranchAddress("P.hgcer.adcPulseAmp", phgc_pulseAmp);
  T->SetBranchAddress("P.hgcer.adcPulseTimeRaw", phgc_pulseTime);

  T->SetBranchAddress("Ndata.P.ngcer.adcCounter", &pngc_hits);
  T->SetBranchAddress("P.ngcer.adcCounter", pngc_pmt);
  T->SetBranchAddress("P.ngcer.adcPedRaw", pngc_pulsePedRaw);
  T->SetBranchAddress("P.ngcer.adcPed", pngc_pulsePed);
  T->SetBranchAddress("P.ngcer.adcPulseIntRaw", pngc_pulseIntRaw);
  T->SetBranchAddress("P.ngcer.adcPulseInt", pngc_pulseInt);
  T->SetBranchAddress("P.ngcer.adcPulseAmpRaw", pngc_pulseAmpRaw);
  T->SetBranchAddress("P.ngcer.adcPulseAmp", pngc_pulseAmp);
  T->SetBranchAddress("P.ngcer.adcPulseTimeRaw", pngc_pulseTime);

  T->SetBranchAddress("T.shms.pT1_tdcTime", &pT1_tdcTime);

  // Declare histos
  h2_phgc_pulsePedRaw_vs_pmt = new TH2F("h2_phgc_pulsePedRaw_vs_pmt", "SHMS HGC Raw Pulse Pedestal; PMT Number; Raw Pulse Pedestal / 1 ADC Count", 4, 0.5, 4.5, 8000, -4000, 4000);
  h2_phgc_pulseIntRaw_vs_pmt = new TH2F("h2_phgc_pulseIntRaw_vs_pmt", "SHMS HGC Raw Pulse Integral; PMT Number; Raw Pulse Integral / 10 ADC Count", 4, 0.5, 4.5, 8000, -40000, 40000);
  h2_phgc_pulseAmpRaw_vs_pmt = new TH2F("h2_phgc_pulseAmpRaw_vs_pmt", "SHMS HGC Raw Pulse Amplitude; PMT Number; Raw Pulse Amplitude / 1 ADC Count", 4, 0.5, 4.5, 8100, -4100, 4100);

  h2_phgc_pulsePed_vs_pmt = new TH2F("h2_phgc_pulsePed_vs_pmt", "SHMS HGC Pulse Pedestal; PMT Number; Pulse Pedestal / 1 ADC Count", 4, 0.5, 4.5, 8000, -4000, 4000);
  h2_phgc_pulseInt_vs_pmt = new TH2F("h2_phgc_pulseInt_vs_pmt", "SHMS HGC Pulse Integral; PMT Number; Pulse Integral / 10 ADC Count", 4, 0.5, 4.5, 8000, -40000, 40000);
  h2_phgc_pulseAmp_vs_pmt = new TH2F("h2_phgc_pulseAmp_vs_pmt", "SHMS HGC Pulse Amplitude; PMT Number; Pulse Amplitude / 1 ADC Count", 4, 0.5, 4.5, 8100, -4100, 4100);
  h2_phgc_pulseTimeCorr_vs_pmt = new TH2F("h2_phgc_pulseTimeCorr_vs_pmt", "SHMS HGC Pulse Time Corrected; PMT Number; Pulse Time / 1 ns", 4, 0.5, 4.5, 8100, -4100, 4100);

  h2_pngc_pulsePedRaw_vs_pmt = new TH2F("h2_pngc_pulsePedRaw_vs_pmt", "SHMS NGC Raw Pulse Pedestal; PMT Number; Raw Pulse Pedestal / 1 ADC Count", 4, 0.5, 4.5, 8000, -4000, 4000);
  h2_pngc_pulseIntRaw_vs_pmt = new TH2F("h2_pngc_pulseIntRaw_vs_pmt", "SHMS NGC Raw Pulse Integral; PMT Number; Raw Pulse Integral / 10 ADC Count", 4, 0.5, 4.5, 8000, -40000, 40000);
  h2_pngc_pulseAmpRaw_vs_pmt = new TH2F("h2_pngc_pulseAmpRaw_vs_pmt", "SHMS NGC Raw Pulse Amplitude; PMT Number; Raw Pulse Amplitude / 1 ADC Count", 4, 0.5, 4.5, 8100, -4100, 4100);

  h2_pngc_pulsePed_vs_pmt = new TH2F("h2_pngc_pulsePed_vs_pmt", "SHMS NGC Pulse Pedestal; PMT Number; Pulse Pedestal / 1 ADC Count", 4, 0.5, 4.5, 8000, -4000, 4000);
  h2_pngc_pulseInt_vs_pmt = new TH2F("h2_pngc_pulseInt_vs_pmt", "SHMS NGC Pulse Integral; PMT Number; Pulse Integral / 10 ADC Count", 4, 0.5, 4.5, 8000, -40000, 40000);
  h2_pngc_pulseAmp_vs_pmt = new TH2F("h2_pngc_pulseAmp_vs_pmt", "SHMS NGC Pulse Amplitude; PMT Number; Pulse Amplitude / 1 ADC Count", 4, 0.5, 4.5, 8100, -4100, 4100);
  h2_pngc_pulseTimeCorr_vs_pmt = new TH2F("h2_pngc_pulseTimeCorr_vs_pmt", "SHMS NGC Pulse Time Corrected; PMT Number; Pulse Time / 1 ns", 4, 0.5, 4.5, 8100, -4100, 4100);

    // Loop of entries in tree
  for(UInt_t ievent = 0; ievent < nentries; ievent++) {

    T->GetEntry(ievent);
    if ((ievent)%1000 == 0) cout << "ievent = " << ievent << endl;

    for (UInt_t ihgchit = 0; ihgchit < phgc_hits; ihgchit++) {

      if (phgc_pulseAmpRaw[ihgchit] != 0.0) continue;
      
      h2_phgc_pulsePedRaw_vs_pmt->Fill(phgc_pmt[ihgchit], phgc_pulsePedRaw[ihgchit]);
      h2_phgc_pulseIntRaw_vs_pmt->Fill(phgc_pmt[ihgchit], phgc_pulseIntRaw[ihgchit]);      
      h2_phgc_pulseAmpRaw_vs_pmt->Fill(phgc_pmt[ihgchit], phgc_pulseAmpRaw[ihgchit]);

      h2_phgc_pulsePed_vs_pmt->Fill(phgc_pmt[ihgchit], phgc_pulsePed[ihgchit]);
      h2_phgc_pulseInt_vs_pmt->Fill(phgc_pmt[ihgchit], phgc_pulseInt[ihgchit]);       
      h2_phgc_pulseAmp_vs_pmt->Fill(phgc_pmt[ihgchit], phgc_pulseAmp[ihgchit]);

      h2_phgc_pulseTimeCorr_vs_pmt->Fill(phgc_pmt[ihgchit], phgc_pulseTime[ihgchit]*clk2adc - pT1_tdcTime*clk2tdc);

    } // HGC loop

    for (UInt_t ingchit = 0; ingchit < pngc_hits; ingchit++) {

      if (pngc_pulseAmpRaw[ingchit] != 0.0) continue;
      
      h2_pngc_pulsePedRaw_vs_pmt->Fill(pngc_pmt[ingchit], pngc_pulsePedRaw[ingchit]);
      h2_pngc_pulseIntRaw_vs_pmt->Fill(pngc_pmt[ingchit], pngc_pulseIntRaw[ingchit]);      
      h2_pngc_pulseAmpRaw_vs_pmt->Fill(pngc_pmt[ingchit], pngc_pulseAmpRaw[ingchit]);

      h2_pngc_pulsePed_vs_pmt->Fill(pngc_pmt[ingchit], pngc_pulsePed[ingchit]);
      h2_pngc_pulseInt_vs_pmt->Fill(pngc_pmt[ingchit], pngc_pulseInt[ingchit]);      
      h2_pngc_pulseAmp_vs_pmt->Fill(pngc_pmt[ingchit], pngc_pulseAmp[ingchit]);

      h2_pngc_pulseTimeCorr_vs_pmt->Fill(pngc_pmt[ingchit], pngc_pulseTime[ingchit]*clk2adc - pT1_tdcTime*clk2tdc);

    } // NGC loop

  }  // Entry loop

  rof->Write();

}  // End of cer_ped()
