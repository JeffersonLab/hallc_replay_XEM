{
    TFile* newYieldF = new TFile("ROOTfiles/coin_monitor.root", "RECREATE");
    TTree* TY_new = new TTree("Y","coin_monitor tree");
    int run_new, tgt_new, kin_new, pN_trig_new, hN_trig_new;
    double pYpc_new, hYpc_new, pQ_new, hQ_new;
    double pHadTrackEff_new, pHadTrackEffErr_new, hElecTrackEff_new, hElecTrackEffErr_new;
    double p3of4Eff_new, h3of4Eff_new, roc2PS6liveTime_new;
    TY_new->Branch("run",&run_new);
    TY_new->Branch("tgt",&tgt_new);
    TY_new->Branch("kin",&kin_new);
    TY_new->Branch("pYpc",&pYpc_new);
    TY_new->Branch("hYpc",&hYpc_new);
    TY_new->Branch("pN_trig",&pN_trig_new);
    TY_new->Branch("hN_trig",&hN_trig_new);
    TY_new->Branch("pQ",&pQ_new);
    TY_new->Branch("hQ",&hQ_new);
    TY_new->Branch("pHadTrackEff",&pHadTrackEff_new);
    TY_new->Branch("pHadTrackEffErr",&pHadTrackEffErr_new);
    TY_new->Branch("hElecTrackEff",&hElecTrackEff_new);
    TY_new->Branch("hElecTrackEffErr",&hElecTrackEffErr_new);
    TY_new->Branch("p3of4Eff",&p3of4Eff_new);
    TY_new->Branch("h3of4Eff",&h3of4Eff_new);
    TY_new->Branch("roc2PS6liveTime",&roc2PS6liveTime_new);
    newYieldF->Write();
    newYieldF->Close();
}
