void replay_hms(Int_t RunNumber=0, Int_t MaxEvent=0) {

  //
  //
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

    const char* RunFileNamePattern="raw/test_%d.log.0";
    const char* ROOTFileNamePattern = "ROOTfiles/hms1190_%d.root";
    //
  gHcParms->Define("gen_run_number", "Run Number", RunNumber);
  gHcParms->AddString("g_ctp_database_filename", "DBASE/standard.database");

  gHcParms->Load(gHcParms->GetString("g_ctp_database_filename"), RunNumber);

  // g_ctp_parm_filename and g_decode_map_filename should now be defined

  gHcParms->Load(gHcParms->GetString("g_ctp_kinematics_filename"), RunNumber);
  gHcParms->Load(gHcParms->GetString("g_ctp_parm_filename"));



  // Generate db_cratemap to correspond to map file contents
  char command[100];
  sprintf(command,"MAPS/make_cratemap.pl < %s > MAPS/db_cratemap.dat",gHcParms->GetString("g_decode_map_filename"));
  system(command);

  // Load the Hall C style detector map
  gHcDetectorMap=new THcDetectorMap();
  gHcDetectorMap->Load(gHcParms->GetString("g_decode_map_filename"));

  // Set up the equipment to be analyzed.

  THaApparatus* HMS = new THcHallCSpectrometer("H","HMS");
  gHaApps->Add( HMS );

  //  HMS->AddDetector( new THcHodoscope("hod", "Hodoscope" ));
  //HMS->AddDetector( new THcShower("cal", "Shower" ));
  //THcCherenkov* cherenkov = new THcCherenkov("cher", "Gas Cerenkov" );
  // HMS->AddDetector( cherenkov );
  HMS->AddDetector( new THcDC("dc", "Drift Chambers" ));
  //THcAerogel* aerogel = new THcAerogel("aero", "Aerogel Cerenkov" );
  //HMS->AddDetector( aerogel );
  //
  //THcScalerEvtHandler *hscaler = new THcScalerEvtHandler("HS","HC scaler event type 0");
  //  hscaler->SetDebugFile("HScaler.txt");
  //gHaEvtHandlers->Add (hscaler);
  //
   //


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
  sprintf(RunFileName,RunFileNamePattern,RunNumber);
  THaRun* run = new THaRun(RunFileName);

  // Eventually need to learn to skip over, or properly analyze
  // the pedestal events
  run->SetEventRange(1,MaxEvent);//  Physics Event number, does not
                                // include scaler or control events
  run->SetNscan(1);
  run->SetDataRequired(0x7);
  run->Print();

  // Define the analysis parameters
  analyzer->SetCountMode( 2 ); // 0 = counter is # of physics triggers
			       //1 = counter is # of all decode reads
                               //2= counter is event number
  analyzer->SetEvent( event );
  analyzer->SetOutFile( ROOTFileName.Data() );
  analyzer->SetOdefFile("Def-files/hdana.def");
  analyzer->SetCutFile("Def-files/hdana_cuts.def");        // optional

  // File to record cuts accounting information
  //  analyzer->SetSummaryFile("summary_example.log"); // optional

  analyzer->Process(run);     // start the actual analysis
  analyzer->PrintReport("TEMPLATES/report.template",Form("REPORT_OUTPUT/replay_both_%05d.report",RunNumber));
}
