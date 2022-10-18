//Not intended to be standalone. Must be called in SCRIPT from top directory of hallc_replay_XEM
//Same functionality as any SCRIPT in hallc_replay_XEM
#include "SCRIPTS/HMS/hms_shared.h"

void replay_production_hms(Int_t RunNumber=0, Int_t MaxEvent=0) {

  // Get RunNumber and MaxEvent if not provided.
  if(RunNumber == 0) { cout << "Enter a Run Number (-1 to exit): ";
    cin >> RunNumber; if( RunNumber<=0 ) {
      cerr << "...Invalid RunNumber entry\n";exit;}}
  if(MaxEvent == 0) {cout << "\nNumber of Events to analyze: ";
    cin >> MaxEvent; if(MaxEvent == 0) {
      cerr << "...Invalid MaxEvent entry\n";exit;}}
  vector<TString> pathList =paths_to_data();

  // Create file name patterns.
  const char* RunFileNamePattern = "hms_all_%05d.dat";  //Raw data file name pattern
  const char* ROOTFileNamePattern = "ROOTfiles/HMS/PRODUCTION/hms_replay_production_%d_%d.root";
  TString ROOTFileName = Form(ROOTFileNamePattern, RunNumber, MaxEvent);
  //Specifics for the replay
  TString odef_file = "DEF-files/HMS/PRODUCTION/hstackana_production.def";
  TString cdef_file = "DEF-files/HMS/PRODUCTION/CUTS/hstackana_production_cuts.def";
  TString summary_file = Form("REPORT_OUTPUT/HMS/PRODUCTION/summary_production_%d_%d.report",
			      RunNumber, MaxEvent);
  TString report_file  = Form("REPORT_OUTPUT/HMS/PRODUCTION/replay_hms_production_%d_%d.report",
			      RunNumber, MaxEvent);

  //Initialize gHcParms.
  //Shared HMS gHcParms setup located in ../hms_shared.h
  setupParms(RunNumber);
  //Initialize HMS single-arm DAQ with detectors
  //Shared HMS apparatus setup located in ../hms_shared.h
  setupApparatus();

  // Set up the analyzer - we use the standard one,
  THcAnalyzer* analyzer = new THcAnalyzer;

  // A simple event class to be output to the resulting tree.
  THaEvent* event = new THaEvent;

  // Define the run that we want to analyze.
  THcRun* run = new THcRun( pathList, Form(RunFileNamePattern, RunNumber) );

  // Set to read in Hall C run database parameters
  run->SetRunParamClass("THcRunParameters");
  
  // Eventually need to learn to skip over, or properly analyze the pedestal events
  run->SetEventRange(1, MaxEvent); // Physics Event number, does not include scaler or control events.
  run->SetNscan(1);
  run->SetDataRequired(0x7);
  run->Print();

  analyzer->SetCountMode(2);  // 0 # of physics triggers,1 # of all decode reads,2 event #
  analyzer->SetEvent(event);
  // Set EPICS event type
  analyzer->SetEpicsEvtType(180);
  analyzer->AddEpicsEvtType(181);
  // Define crate map
  analyzer->SetCrateMapFileName("MAPS/db_cratemap.dat");

  // Define output ROOT file
  analyzer->SetOutFile(ROOTFileName.Data());
  // Define DEF-file
  analyzer->SetOdefFile(odef_file);
  // Define cuts file
  analyzer->SetCutFile(cdef_file);  // optional
  // File to record accounting information for cuts
  analyzer->SetSummaryFile(summary_file);  // optional

  // Start the actual analysis.
  analyzer->Process(run);

  // Create report file from template
  analyzer->PrintReport(gHcParms->GetString("g_ctp_template_filename"),
  			report_file);  // optional

}
