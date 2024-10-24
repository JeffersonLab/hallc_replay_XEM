//Not intended to be standalone. Must be called in SCRIPT from top directory of hallc_replay_XEM
//Same functionality as any SCRIPT in hallc_replay_XEM
#include "SCRIPTS/HMS/hms_shared.h"
R__LOAD_LIBRARY(CALIBRATION/bcm_current_map/ScalerCalib_C.so)

void replay_hms_scalers(Int_t RunNumber=0, Int_t MaxEvent=0) {

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
  const char* ROOTFileNamePattern = "ROOTfiles/HMS/SCALERS/hms_replay_scalers_%d_%d.root";
  
  //Initialize gHcParms.
  //Shared HMS gHcParms setup located in ../hms_shared.h
  setupParms(RunNumber);
  
  // Add trigger apparatus
  THaApparatus* TRG = new THcTrigApp("T", "TRG");
  gHaApps->Add(TRG);
  // Add trigger detector to trigger apparatus
  THcTrigDet* hms = new THcTrigDet("hms", "HMS Trigger Information");
  TRG->AddDetector(hms);

  // Add event handler for EPICS events
  THaEpicsEvtHandler *hcepics = new THaEpicsEvtHandler("epics", "HC EPICS event type 181");
  gHaEvtHandlers->Add(hcepics);
  // Add event handler for scaler events
  THcScalerEvtHandler *hscaler = new THcScalerEvtHandler("H","Hall C scaler event type 129");
  hscaler->AddEvtType(2);
  hscaler->AddEvtType(129);
  hscaler->SetDelayedType(129);
  hscaler->SetUseFirstEvent(kTRUE);
  gHaEvtHandlers->Add(hscaler);
  
  /*
  // Add event handler for helicity scalers
  THcHelicityScaler *hhelscaler = new THcHelicityScaler("H", "Hall C helicity scaler");
  //hhelscaler->SetDebugFile("HHelScaler.txt");
  hhelscaler->SetROC(5);
  hhelscaler->SetUseFirstEvent(kTRUE);
  gHaEvtHandlers->Add(hhelscaler);
  */
  
  // Add event handler for DAQ configuration event
  THcConfigEvtHandler *hconfig = new THcConfigEvtHandler("hconfig", "Hall C configuration event handler");
  gHaEvtHandlers->Add(hconfig);

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
  // Set CODA version
  analyzer->SetCodaVersion(2);
  // Set EPICS event type
  analyzer->SetEpicsEvtType(181);
  // Define crate map
  analyzer->SetCrateMapFileName("MAPS/db_cratemap.dat");
  // Define output ROOT file
  analyzer->SetOutFile(ROOTFileName.Data());
  // Define DEF-file
  analyzer->SetOdefFile("DEF-files/HMS/EPICS/epics_short.def");
  // Define cuts file
  analyzer->SetCutFile("DEF-files/HMS/SCALERS/hscaler_cuts.def");  // optional
  // File to record accounting information for cuts
  analyzer->SetSummaryFile(Form("REPORT_OUTPUT/HMS/SCALERS/summary_scalers_%d_%d.report", RunNumber, MaxEvent));  // optional
  // Start the actual analysis.
  analyzer->Process(run);
  // Create report file from template
  analyzer->PrintReport("TEMPLATES/HMS/SCALERS/hscalers.template",
  			Form("REPORT_OUTPUT/HMS/SCALERS/replay_hms_scalers_%d_%d.report", RunNumber, MaxEvent));  // optional  

  //H: HMS, P: SHMS
  ScalerCalib scalib("H");
  scalib.SetInputFile(ROOTFileName.Data());
  scalib.SetRunNum(RunNumber);
  scalib.SetPrintFlag(1); //0: bcm1 and bcm2 only, 1: all
  scalib.Run();

}
