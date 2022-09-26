//Not intended to be standalone. Must be called in SCRIPT from top directory of hallc_replay_XEM
//Same functionality as any SCRIPT in hallc_replay_XEM
#include "SCRIPTS/HMS/hms_shared.h"

void replay_hms_raster_simple(Int_t RunNumber=0, Int_t MaxEvent=0) {

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
  const char* ROOTFileNamePattern = "ROOTfiles/hms_replay_scalers_%d_%d.root";
  
  //Initialize gHcParms.
  //Shared HMS gHcParms setup located in ../hms_shared.h
  setupParms(RunNumber);

  // Add handler for EPICS events
  THaEpicsEvtHandler *hcepics = new THaEpicsEvtHandler("epics", "HC EPICS event type 180");
  gHaEvtHandlers->Add(hcepics);
  // Add Rastered Beam Apparatus
  THaApparatus* beam = new THcRasteredBeam("H.rb", "Rastered Beamline");
  gHaApps->Add(beam);

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
  THcRun* run = new THcRun( pathList, Form(RunFileNamePattern, RunNumber) );
  // Set to read in Hall C run database parameters
  run->SetRunParamClass("THcRunParameters");

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
  // Set EPICS event type
  analyzer->SetEpicsEvtType(180);
  // Define crate map
  analyzer->SetCrateMapFileName("MAPS/db_cratemap.dat");
  // Define output ROOT file
  analyzer->SetOutFile(ROOTFileName.Data());
  // Define DEF-file
  analyzer->SetOdefFile("DEF-files/HMS/RASTER/hms_raster_simple.def");
  // Define cuts file
  analyzer->SetCutFile("DEF-files/HMS/RASTER/hms_raster_simple_cuts.def");    // optional
  // Start the actual analysis.
  analyzer->Process(run);

}
