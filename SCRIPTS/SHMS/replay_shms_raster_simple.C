void replay_shms_raster_simple(Int_t RunNumber=0, Int_t MaxEvent=0) {

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
  const char* RunFileNamePattern = "raw/shms_all_%05d.dat";
  const char* ROOTFileNamePattern = "ROOTfiles/shms_raster_simple_%d.root";
  // Add variables to global list.
  gHcParms->Define("gen_run_number", "Run Number", RunNumber);
  gHcParms->AddString("g_ctp_database_filename", "DBASE/standard.database");

  // Load varibles from files to global list.
  gHcParms->Load(gHcParms->GetString("g_ctp_database_filename"), RunNumber);

  // g_ctp_parm_filename and g_decode_map_filename should now be defined.
  gHcParms->Load(gHcParms->GetString("g_ctp_kinematics_filename"), RunNumber);
  gHcParms->Load(gHcParms->GetString("g_ctp_parm_filename"));

  // Load params for HMS trigger configuration
  gHcParms->Load("PARAM/TRIG/tshms_raster.param");

  // Load the Hall C style detector map
  gHcDetectorMap = new THcDetectorMap();
  gHcDetectorMap->Load("MAPS/SHMS/DETEC/praster_simple.map");
  
  // Set up the equipment to be analyzed.
  //THaApparatus* HMS = new THcHallCSpectrometer("H", "HMS");
  //gHaApps->Add(HMS);
  // Add hodoscope to HMS apparatus
  //THcHodoscope* hod = new THcHodoscope("hod", "Hodoscope");
  //HMS->AddDetector(hod);

  // Add trigger apparatus
  THaApparatus* TRG = new THcTrigApp("T", "TRG");
  gHaApps->Add(TRG);
  // Add trigger detector to trigger apparatus
  THcTrigDet* shms = new THcTrigDet("shms", "SHMS Trigger Information");
  TRG->AddDetector(shms);

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
  char RunFileName[100];
  sprintf(RunFileName, RunFileNamePattern, RunNumber);
  THaRun* run = new THaRun(RunFileName);

  // Eventually need to learn to skip over, or properly analyze
  // the pedestal events
  run->SetEventRange(1, MaxEvent);    // Physics Event number, does not
                                      // include scaler or control events.
  run->SetNscan(1);
  run->SetDataRequired(0x7);
  run->Print();

  // Define the analysis parameters
  TString ROOTFileName = Form(ROOTFileNamePattern, RunNumber);
  analyzer->SetCountMode(2);    // 0 = counter is # of physics triggers
                                // 1 = counter is # of all decode reads
                                // 2 = counter is event number
 analyzer->SetEvent(event);
 analyzer->SetCrateMapFileName("MAPS/db_cratemap.dat");
 analyzer->SetOutFile(ROOTFileName.Data());
 analyzer->SetOdefFile("DEF-files/SHMS/RASTER/shms_raster_simple.def");
 analyzer->SetCutFile("DEF-files/SHMS/RASTER/shms_raster_simple_cuts.def");    // optional

 // File to record cuts accounting information
 //analyzer->SetSummaryFile("summary_example.log");    // optional

  // Start the actual analysis.
  analyzer->Process(run);
  // Create report file from template.
  //analyzer->PrintReport(    // optional
  //  "TEMPLATES/dcana.template",
  //  Form("REPORT_OUTPUT/replay_hms_%05d.report", RunNumber)
  //);
}
