void replay_shms (Int_t RunNumber = 0, Int_t MaxEvent = 0) {

  // Get RunNumber and MaxEvent if not provided.
  if(RunNumber == 0) {
    cout << "Enter a Run Number (-1 to exit): ";
    cin >> RunNumber;
    if( RunNumber<=0 ) return;
  }
  if(MaxEvent == 0) {
    cout << "\nNumber of Events to analyze: ";
    cin >> MaxEvent;
    if(MaxEvent == 0) {
      cerr << "...Invalid entry\n";
      exit;
    }
  }

  // Create file name patterns.
  const char* RunFileNamePattern = "shms_all_%05d.dat";
  vector<TString> pathList;
    pathList.push_back(".");
    pathList.push_back("./raw");
    pathList.push_back("./raw/../raw.copiedtotape");
    pathList.push_back("./cache");

  const char* ROOTFileNamePattern = "ROOTfiles/shms_replay_%d_%d.root";
  
  // Load global parameters
  // Add variables to global list.
  gHcParms->Define("gen_run_number", "Run Number", RunNumber);
  gHcParms->AddString("g_ctp_database_filename", "DBASE/SHMS/STD/standard.database");
  // Load varibles from files to global list.
  gHcParms->Load(gHcParms->GetString("g_ctp_database_filename"), RunNumber);
  // g_ctp_parm_filename and g_decode_map_filename should now be defined.
  gHcParms->Load(gHcParms->GetString("g_ctp_kinematics_filename"), RunNumber);
  gHcParms->Load(gHcParms->GetString("g_ctp_parm_filename"));
  gHcParms->Load(gHcParms->GetString("g_ctp_calib_filename"));
  // Load params for SHMS trigger configuration
  gHcParms->Load("PARAM/TRIG/tshms.param");

  // Load the Hall C detector map
  gHcDetectorMap = new THcDetectorMap();
  gHcDetectorMap->Load("MAPS/SHMS/DETEC/STACK/shms_stack.map");

  // Add trigger apparatus
  THaApparatus* TRG = new THcTrigApp("T", "TRG");
  gHaApps->Add(TRG);
  // Add trigger detector to trigger apparatus
  THcTrigDet* shms = new THcTrigDet("shms", "SHMS Trigger Information");
  TRG->AddDetector(shms);

  // Set up the equipment to be analyzed.
  THaApparatus* SHMS = new THcHallCSpectrometer("P", "SHMS");
  gHaApps->Add(SHMS);
  // Add Noble Gas Cherenkov to SHMS apparatus
  THcCherenkov* ngcer = new THcCherenkov("ngcer", "Noble Gas Cherenkov");
  SHMS->AddDetector(ngcer);
  // Add drift chambers to SHMS apparatus
  THcDC* dc = new THcDC("dc", "Drift Chambers");
  SHMS->AddDetector(dc);
  // Add hodoscope to SHMS apparatus
  THcHodoscope* hod = new THcHodoscope("hod", "Hodoscope");
  SHMS->AddDetector(hod);
  // Add Heavy Gas Cherenkov to SHMS apparatus
  THcCherenkov* hgcer = new THcCherenkov("hgcer", "Heavy Gas Cherenkov");
  SHMS->AddDetector(hgcer);
  // Add Aerogel Cherenkov to SHMS apparatus
  THcAerogel* aero = new THcAerogel("aero", "Aerogel");
  SHMS->AddDetector(aero);
  // Add calorimeter to SHMS apparatus
  THcShower* cal = new THcShower("cal", "Calorimeter");
  SHMS->AddDetector(cal);

  // Include golden track information
  THaGoldenTrack* gtr = new THaGoldenTrack("P.gtr", "SHMS Golden Track", "P");
  gHaPhysics->Add(gtr);

  // Add handler for prestart event 125.
  THcConfigEvtHandler* ev125 = new THcConfigEvtHandler("HC", "Config Event type 125");
  gHaEvtHandlers->Add(ev125);

  // Set up the analyzer - we use the standard one,
  // but this could be an experiment-specific one as well.
  // The Analyzer controls the reading of the data, executes
  // tests/cuts, loops over Acpparatus's and PhysicsModules,
  // and executes the output routines.
  THcAnalyzer* analyzer = new THcAnalyzer;

  // A simple event class to be output to the resulting tree.
  // Creating your own descendant of THaEvent is one way of
  // defining and controlling the output.
  THaEvent* event = new THaEvent;

  // Define the run(s) that we want to analyze.
  // We just set up one, but this could be many.
  THaRun* run = new THaRun( pathList, Form(RunFileNamePattern, RunNumber) );

  // Eventually need to learn to skip over, or properly analyze
  // the pedestal events
  run->SetEventRange(1, MaxEvent);    // Physics Event number, does not
                                      // include scaler or control events.
  run->SetNscan(1);
  run->SetDataRequired(0x7);
  run->Print();

  // Define the analysis parameters
  TString ROOTFileName = Form(ROOTFileNamePattern, RunNumber, MaxEvent);
  analyzer->SetCountMode(2);    // 0 = counter is # of physics triggers
                                // 1 = counter is # of all decode reads
                                // 2 = counter is event number
  analyzer->SetEvent(event);
  // Define crate map
  analyzer->SetCrateMapFileName("MAPS/db_cratemap.dat");
  // Define output ROOT file
  analyzer->SetOutFile(ROOTFileName.Data());
  // Define DEF-file
  analyzer->SetOdefFile("DEF-files/SHMS/STACK/pstackana.def");
  // Define cuts file
  analyzer->SetCutFile("DEF-files/SHMS/STACK/pstackana_cuts.def");  // optional
  // File to record accounting information for cuts
  analyzer->SetSummaryFile(Form("REPORT_OUTPUT/SHMS/STACK/summary_stack_%d_%d.report", RunNumber, MaxEvent));  // optional
  // Start the actual analysis.
  analyzer->Process(run);
  // Create report file from template
  analyzer->PrintReport("TEMPLATES/SHMS/STACK/pstackana.template",
  			Form("REPORT_OUTPUT/SHMS/STACK/replay_shms_stack_%d_%d.report", RunNumber, MaxEvent));  // optional

}
