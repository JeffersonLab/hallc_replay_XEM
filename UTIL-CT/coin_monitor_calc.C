void coin_monitor_calc(Int_t RunNumber = 0, Int_t TargetType = 0, Int_t KinematicsType = 0)
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
	//	cout << "Enter a kinematics index (see coin_monitor.README; -1 to exit): ";
	//	cin >> KinematicsType;
	//	if( KinematicsType<=0 ) return;
	//}


	// ========================================================================
	// A cheat to grab some efficiencies from the report output.
	// I'm doing this because I don't know how to access this from the root file
	// Note that this makes BIG assumptions about the structure of the template!
	//
	TString sysOutput;	// awk output
	TString field;          // description in report
	TObjArray* stringsPtr;  // used in tokenizing

	// report filename
	TString reportFile = Form("REPORT_OUTPUT/COIN/PRODUCTION/replay_coin_production_%d_-1.report", RunNumber);

	// awk template for getting value form report
	// Assumes value is separated from description by a colon.
	// You need to be careful if there are multiple matches in the file.
	TString awk_template = "awk -F':' '/%s/{print $2}' %s";
	
	// SHMS charge1 -------------------------------------------------------
	field = "SHMS BCM1  Beam Cut Charge";
	sysOutput = gSystem->GetFromPipe(Form(awk_template.Data(), field.Data(), reportFile.Data()));
	stringsPtr = sysOutput.Tokenize("\n");
	double pQ_BCM1 = ((TObjString *)(stringsPtr->At(0)))->String().Atof();

	// SHMS charge2 -------------------------------------------------------
	field = "SHMS BCM2  Beam Cut Charge";
	sysOutput = gSystem->GetFromPipe(Form(awk_template.Data(), field.Data(), reportFile.Data()));
	stringsPtr = sysOutput.Tokenize("\n");
	double pQ_BCM2 = ((TObjString *)(stringsPtr->At(0)))->String().Atof();

	// HMS charge1 --------------------------------------------------------
	field = "HMS BCM1  Beam Cut Charge";
	sysOutput = gSystem->GetFromPipe(Form(awk_template.Data(), field.Data(), reportFile.Data()));
	stringsPtr = sysOutput.Tokenize("\n");
	double hQ_BCM1 = ((TObjString *)(stringsPtr->At(0)))->String().Atof();

	// HMS charge2 --------------------------------------------------------
	field = "HMS BCM1  Beam Cut Charge";
	sysOutput = gSystem->GetFromPipe(Form(awk_template.Data(), field.Data(), reportFile.Data()));
	stringsPtr = sysOutput.Tokenize("\n");
	double hQ_BCM2 = ((TObjString *)(stringsPtr->At(0)))->String().Atof();

	// SHMS N TRIG --------------------------------------------------------
	field = "SHMS Accepted Physics Triggers";
	sysOutput = gSystem->GetFromPipe(Form(awk_template.Data(), field.Data(), reportFile.Data()));
	stringsPtr = sysOutput.Tokenize("\n");
	double pN_trig = ((TObjString *)(stringsPtr->At(0)))->String().Atof();

	// HMS N TRIG ---------------------------------------------------------
	field = "HMS Accepted Physics Triggers";
	sysOutput = gSystem->GetFromPipe(Form(awk_template.Data(), field.Data(), reportFile.Data()));
	stringsPtr = sysOutput.Tokenize("\n");
	double hN_trig = ((TObjString *)(stringsPtr->At(0)))->String().Atof();


	// Yield per mC
	double pYield = 1000 * pN_trig / ((pQ_BCM1 + pQ_BCM2)/2.0);
	double hYield = 1000 * hN_trig / ((hQ_BCM1 + hQ_BCM2)/2.0);

	// SHMS hadron track efficiency ---------------------------------------
	// Assume SHMS is the first match, HMS is the second
	field = "HADRON SING FID TRACK EFF";
	sysOutput = gSystem->GetFromPipe(Form(awk_template.Data(), field.Data(), reportFile.Data()));
	stringsPtr = sysOutput.Tokenize("\n");
	sysOutput = ((TObjString *)(stringsPtr->At(0)))->String(); // first match
	stringsPtr = sysOutput.Tokenize("+-"); // this line has erorr printed as well
	double pHadTrackEff    = ((TObjString *)(stringsPtr->At(0)))->String().Atof();
	double pHadTrackEffErr = ((TObjString *)(stringsPtr->At(1)))->String().Atof();

	// HMS electron track efficiency ---------------------------------------
	// Assume SHMS is the first match, HMS is the second
	field = "E SING FID TRACK EFFIC";
	sysOutput = gSystem->GetFromPipe(Form(awk_template.Data(), field.Data(), reportFile.Data()));
	stringsPtr = sysOutput.Tokenize("\n");
	sysOutput = ((TObjString *)(stringsPtr->At(1)))->String(); // second match
	stringsPtr = sysOutput.Tokenize("+-"); // this line has erorr printed as well
	double hElecTrackEff    = ((TObjString *)(stringsPtr->At(0)))->String().Atof();
	double hElecTrackEffErr = ((TObjString *)(stringsPtr->At(1)))->String().Atof();

	// SHMS and HMS 3/4 trigger efficiency ---------------------------------
	// Assume SHMS is the first match, HMS is the second
	field = "3_of_4 EFF";
	sysOutput = gSystem->GetFromPipe(Form(awk_template.Data(), field.Data(), reportFile.Data()));
	stringsPtr = sysOutput.Tokenize("\n");
	double p3of4Eff         = ((TObjString *)(stringsPtr->At(0)))->String().Atof();
	double h3of4Eff         = ((TObjString *)(stringsPtr->At(1)))->String().Atof();

	// Live Time ----------------------------------------------------------
	field = "ROC2 Pre-Scaled Ps6 ROC2 Computer Live Time";
	sysOutput = gSystem->GetFromPipe(Form(awk_template.Data(), field.Data(), reportFile.Data()));
	stringsPtr = sysOutput.Tokenize("\n");
	double roc2PS6liveTime  = ((TObjString *)(stringsPtr->At(0)))->String().Atof();

	// ========================================================================
	// Bookkeeping

	// Open old data, copy to new tree.
	// We skip if an old datum's run number matches the one the user specified.
	TFile* newYieldF = new TFile("ROOTfiles/coin_monitor_new.root", "RECREATE");
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

	TFile* yieldF = new TFile("ROOTfiles/coin_monitor.root","READ");
	TTree* TY_old = (TTree*)yieldF->Get("Y");

	int run_old, tgt_old, kin_old, pN_trig_old, hN_trig_old;
	double pYpc_old, hYpc_old, pQ_old, hQ_old;
	double pHadTrackEff_old, pHadTrackEffErr_old, hElecTrackEff_old, hElecTrackEffErr_old;
	double p3of4Eff_old, h3of4Eff_old, roc2PS6liveTime_old;
	TY_old->SetBranchAddress("run",&run_old);
	TY_old->SetBranchAddress("tgt",&tgt_old);
	TY_old->SetBranchAddress("kin",&kin_old);
	TY_old->SetBranchAddress("pYpc",&pYpc_old);
	TY_old->SetBranchAddress("hYpc",&hYpc_old);
	TY_old->SetBranchAddress("pN_trig",&pN_trig_old);
	TY_old->SetBranchAddress("hN_trig",&hN_trig_old);
	TY_old->SetBranchAddress("pQ",&pQ_old);
	TY_old->SetBranchAddress("hQ",&hQ_old);
	TY_old->SetBranchAddress("pHadTrackEff",&pHadTrackEff_old);
	TY_old->SetBranchAddress("pHadTrackEffErr",&pHadTrackEffErr_old);
	TY_old->SetBranchAddress("hElecTrackEff",&hElecTrackEff_old);
	TY_old->SetBranchAddress("hElecTrackEffErr",&hElecTrackEffErr_old);
	TY_old->SetBranchAddress("p3of4Eff",&p3of4Eff_old);
	TY_old->SetBranchAddress("h3of4Eff",&h3of4Eff_old);
	TY_old->SetBranchAddress("roc2PS6liveTime",&roc2PS6liveTime_old);

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
		pHadTrackEff_new     = pHadTrackEff_old;
		pHadTrackEffErr_new  = pHadTrackEffErr_old;
		hElecTrackEff_new    = hElecTrackEff_old;
		hElecTrackEffErr_new = hElecTrackEffErr_old;
		p3of4Eff_new         = p3of4Eff_old;
		h3of4Eff_new         = h3of4Eff_old;
		roc2PS6liveTime_new  = roc2PS6liveTime_old;

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
	pHadTrackEff_new     = pHadTrackEff;
	pHadTrackEffErr_new  = pHadTrackEffErr;
	hElecTrackEff_new    = hElecTrackEff;
	hElecTrackEffErr_new = hElecTrackEffErr;
	p3of4Eff_new         = p3of4Eff;
	h3of4Eff_new         = h3of4Eff;
	roc2PS6liveTime_new  = roc2PS6liveTime;
	TY_new->Fill();

	// Rename new root file to old one
	newYieldF->Write();
	newYieldF->Close();
	gSystem->Exec("mv ROOTfiles/coin_monitor_new.root ROOTfiles/coin_monitor.root");
}
