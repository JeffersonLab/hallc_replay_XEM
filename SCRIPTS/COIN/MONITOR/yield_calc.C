void yield_calc(Int_t RunNumber = 0, Int_t TargetType = 0, Int_t KinematicsType = 0)
{

	// Get RunNumber if not provided.
	if(RunNumber == 0) {
		cout << "Enter a Run Number (-1 to exit): ";
		cin >> RunNumber;
		if( RunNumber<=0 ) return;
	}

	// Get if not provided.
	if(TargetType == 0) {
		cout << "Enter a target (1=LH2, 2=6% Carbon, 3=dummy, -1 to exit): ";
		cin >> TargetType;
		if( TargetType<=0 ) return;
	}

	// Get KinematicsType if not provided.
	//if(KinematicsType == 0) {
	//	cout << "Enter a kinematics index (see yield.README; -1 to exit): ";
	//	cin >> KinematicsType;
	//	if( KinematicsType<=0 ) return;
	//}

	// Open root file
	char* ROOTFileNamePattern = "../../../ROOTfiles/coin_replay_production_%d_-1.root";
	TString ROOTFileName = Form(ROOTFileNamePattern, RunNumber);
	TFile* hcanaF  = new TFile(ROOTFileName);
	TTree* tsp = (TTree*)hcanaF->Get("TSP");
	TTree* tsh = (TTree*)hcanaF->Get("TSH");

	// Set up tree variables
	double pQ_BCM1, pQ_BCM2, pN_trig, pQ_avg;
	tsp->SetBranchAddress("P.BCM1.scalerChargeCut",&pQ_BCM1);
	tsp->SetBranchAddress("P.BCM2.scalerChargeCut",&pQ_BCM2);
	tsp->SetBranchAddress("P.pTRIG6.scalerCut",&pN_trig);

	double hQ_BCM1, hQ_BCM2, hN_trig, hQ_avg;
	tsh->SetBranchAddress("H.BCM1.scalerChargeCut",&hQ_BCM1);
	tsh->SetBranchAddress("H.BCM2.scalerChargeCut",&hQ_BCM2);
	tsh->SetBranchAddress("H.hTRIG6.scalerCut",&hN_trig);

	// correct value is last in the scaler tree?
	int pN=tsp->GetEntries()-1; 
	int hN=tsp->GetEntries()-1; 
	tsp->GetEntry(pN);
	tsh->GetEntry(hN);

	// Yield per mC
	double pYield = 1000 * pN_trig / ((pQ_BCM1 + pQ_BCM2)/2.0);
	double hYield = 1000 * hN_trig / ((hQ_BCM1 + hQ_BCM2)/2.0);

	delete hcanaF;

	// Open old data, copy to new tree.
	// We skip if an old datum's run number matches the one the user specified.
	TFile* newYieldF = new TFile("coin_yield_new.root", "RECREATE");
	TTree* TY_new = new TTree("Y","coin_yield tree");
	int run_new, tgt_new, kin_new, pN_trig_new, hN_trig_new;
	double pYpc_new, hYpc_new, pQ_new, hQ_new;
	TY_new->Branch("run",&run_new);
	TY_new->Branch("tgt",&tgt_new);
	TY_new->Branch("kin",&kin_new);
	TY_new->Branch("pYpc",&pYpc_new);
	TY_new->Branch("hYpc",&hYpc_new);
	TY_new->Branch("pN_trig",&pN_trig_new);
	TY_new->Branch("hN_trig",&hN_trig_new);
	TY_new->Branch("pQ",&pQ_new);
	TY_new->Branch("hQ",&hQ_new);

	TFile* yieldF = new TFile("coin_yield.root","READ");
	TTree* TY_old = (TTree*)yieldF->Get("Y");

	int run_old, tgt_old, kin_old, pN_trig_old, hN_trig_old;
	double pYpc_old, hYpc_old, pQ_old, hQ_old;
	TY_old->SetBranchAddress("run",&run_old);
	TY_old->SetBranchAddress("tgt",&tgt_old);
	TY_old->SetBranchAddress("kin",&kin_old);
	TY_old->SetBranchAddress("pYpc",&pYpc_old);
	TY_old->SetBranchAddress("hYpc",&hYpc_old);
	TY_old->SetBranchAddress("pN_trig",&pN_trig_old);
	TY_old->SetBranchAddress("hN_trig",&hN_trig_old);
	TY_old->SetBranchAddress("pQ",&pQ_old);
	TY_old->SetBranchAddress("hQ",&hQ_old);

	newYieldF->cd();
	for (int i=0; i<TY_old->GetEntries(); i++) {
		TY_old->GetEntry(i);

		// Skip if matches user specified run
		if (run_old == RunNumber) {
			cout << "WARNING: Overwriting run " << RunNumber << endl;
			continue;
		}

		run_new     = run_old;
		tgt_new     = tgt_old;
		kin_new     = kin_old;
		pYpc_new    = pYpc_old;
		hYpc_new    = hYpc_old;
		pN_trig_new = pN_trig_old;
		hN_trig_new = pN_trig_old;
		pQ_new      = pQ_old;
		hQ_new      = hQ_old;

		TY_new->Fill();
	}

	// Add user specified run
	run_new     = RunNumber;
	tgt_new     = TargetType;
	kin_new     = KinematicsType;
	pYpc_new    = pYield;
	hYpc_new    = hYield;
	pN_trig_new = pN_trig;
	hN_trig_new = hN_trig;
	pQ_new      = ((pQ_BCM1 + pQ_BCM2)/2.0) / 1000.0;
	hQ_new      = ((hQ_BCM1 + hQ_BCM2)/2.0) / 1000.0;
	TY_new->Fill();

	// Rename new root file to old one
	newYieldF->Write();
	newYieldF->Close();
	gSystem->Exec("mv coin_yield_new.root coin_yield.root");
}
