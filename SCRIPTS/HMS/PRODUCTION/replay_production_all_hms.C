//Not intended to be standalone. Must be called in SCRIPT from top directory of hallc_replay_XEM
//Same functionality as any SCRIPT in hallc_replay_XEM
#include "SCRIPTS/HMS/hms_shared.h"

void replay_production_all_hms (Int_t RunNumber=0, Int_t MaxEvent=0) {

  // Get RunNumber and MaxEvent if not provided.
  if(RunNumber == 0) { cout << "Enter a Run Number (-1 to exit): ";
    cin >> RunNumber; if( RunNumber<=0 ) {
      cerr << "...Invalid RunNumber entry\n";exit;}}
  if(MaxEvent == 0) {cout << "\nNumber of Events to analyze: ";
    cin >> MaxEvent; if(MaxEvent == 0) {
      cerr << "...Invalid MaxEvent entry\n";exit;}}
  //vector<TString> pathList =paths_to_data();
  vector<TString> pathList;
  pathList.push_back(".");
  pathList.push_back("./raw");
  pathList.push_back("./raw-sp18");
  pathList.push_back("./raw-sp19");
  pathList.push_back("./raw/../raw.copiedtotape");
  pathList.push_back("./CACHE_LINKS/cache_pionlt");
  pathList.push_back("./CACHE_LINKS/cache_cafe"); 
  pathList.push_back("./CACHE_LINKS/cache_sp18");
  pathList.push_back("./CACHE_LINKS/cache_sp19"); 
  pathList.push_back("./CACHE_LINKS/cache_xem2"); 
  return pathList;

  // Create file name patterns.
  const char* RunFileNamePattern = "hms_all_%05d.dat";  //Raw data file name pattern
  const char* ROOTFileNamePattern = "ROOTfiles/hms_replay_production_all_%d_%d.root";
  TString ROOTFileName = Form(ROOTFileNamePattern, RunNumber, MaxEvent);
  //Specifics for the replay
  TString odef_file = "DEF-files/HMS/PRODUCTION/hstackana_production_all.def";
  TString cdef_file = "DEF-files/HMS/PRODUCTION/CUTS/hstackana_production_cuts.def";
  TString summary_file = Form("REPORT_OUTPUT/HMS/PRODUCTION/summary_all_production_%d_%d.report", RunNumber, MaxEvent);
  TString report_file  = Form("REPORT_OUTPUT/HMS/PRODUCTION/replay_hms_all_production_%d_%d.report", RunNumber, MaxEvent);

  //Initialize gHcParms.
  //Shared HMS gHcParms setup located in ../hms_shared.h
  //setupParms(RunNumber);
    // Load Global parameters
  // Add variables to global list.
  gHcParms->Define("gen_run_number", "Run Number", RunNumber);
  gHcParms->AddString("g_ctp_database_filename", "DBASE/HMS/standard.database");
  gHcParms->Load(gHcParms->GetString("g_ctp_database_filename"), RunNumber);
  gHcParms->Load(gHcParms->GetString("g_ctp_kinematics_filename"), RunNumber);
  gHcParms->Load(gHcParms->GetString("g_ctp_parm_filename"));
  gHcParms->Load(gHcParms->GetString("g_ctp_det_calib_filename"));
  gHcParms->Load(gHcParms->GetString("g_ctp_bcm_calib_filename"));
  gHcParms->Load(gHcParms->GetString("g_ctp_optics_filename"));
  // Load parameters for SHMS trigger configuration
  gHcParms->Load(gHcParms->GetString("g_ctp_trig_config_filename"));
  // Load hpcentral momentum offset 
  // Load fadc debug parameters
  gHcParms->Load("PARAM/HMS/GEN/h_fadc_debug_fa22.param");
  
  // Load BCM values
  TString bcmParamFile = Form("PARAM/HMS/BCM/bcmcurrent_%d.param", RunNumber);
  bcmFile.open(bcmParamFile);
  if (bcmFile.is_open()) gHcParms->Load(bcmParamFile);

  // Load the Hall C detector map
  gHcDetectorMap = new THcDetectorMap();
  gHcDetectorMap->Load(gHcParms->GetString("g_ctp_map_filename"));
  //Initialize HMS single-arm DAQ with detectors
  //Shared HMS apparatus setup located in ../hms_shared.h
  //setupApparatus();
  // Add trigger apparatus
  THaApparatus* TRG = new THcTrigApp("T", "TRG");
  gHaApps->Add(TRG);
  // Add trigger detector to trigger apparatus
  THcTrigDet* hms = new THcTrigDet("hms", "HMS Trigger Information");
  TRG->AddDetector(hms);

  // Set up the equipment to be analyzed.
  THcHallCSpectrometer* HMS = new THcHallCSpectrometer("H", "HMS");
  gHaApps->Add(HMS);
  // Add drift chambers to HMS apparatus
  THcDC* dc = new THcDC("dc", "Drift Chambers");
  HMS->AddDetector(dc);
  // Add hodoscope to HMS apparatus
  THcHodoscope* hod = new THcHodoscope("hod", "Hodoscope");
  HMS->AddDetector(hod);
  // Add Cherenkov to HMS apparatus
  THcCherenkov* cer = new THcCherenkov("cer", "Heavy Gas Cherenkov");
  HMS->AddDetector(cer);
  // Add Aerogel Cherenkov to HMS apparatus
  // THcAerogel* aero = new THcAerogel("aero", "Aerogel");
  // HMS->AddDetector(aero);
  // Add calorimeter to HMS apparatus
  THcShower* cal = new THcShower("cal", "Calorimeter");
  HMS->AddDetector(cal);

  // Add rastered beam apparatus
  THaApparatus* beam = new THcRasteredBeam("H.rb", "Rastered Beamline");
  gHaApps->Add(beam);  
  // Add physics modules
  // Add beam current monitor module
  if (bcmFile.is_open()) {
    THcBCMCurrent* bcm = new THcBCMCurrent("H.bcm", "BCM Module");
    gHaPhysics->Add(bcm);
  }
  // Calculate reaction point
  THcReactionPoint* hrp = new THcReactionPoint("H.react", "HMS reaction point", "H", "H.rb");
  gHaPhysics->Add(hrp);
  // Calculate extended target corrections
  THcExtTarCor* hext = new THcExtTarCor("H.extcor", "HMS extended target corrections", "H", "H.react");
  gHaPhysics->Add(hext);
  // Calculate golden track quantities
  THaGoldenTrack* gtr = new THaGoldenTrack("H.gtr", "HMS Golden Track", "H");
  gHaPhysics->Add(gtr);
  // Calculate primary (scattered beam - usually electrons) kinematics
  THcPrimaryKine* hkin = new THcPrimaryKine("H.kin", "HMS Single Arm Kinematics", "H", "H.rb");
  gHaPhysics->Add(hkin);
  // Calculate the hodoscope efficiencies
  THcHodoEff* heff = new THcHodoEff("hhodeff", "HMS hodo efficiency", "H.hod");
  gHaPhysics->Add(heff);

  // Add handler for prestart event 125.
  THcConfigEvtHandler* ev125 = new THcConfigEvtHandler("HC", "Config Event type 125");
  gHaEvtHandlers->Add(ev125);
  // Add handler for EPICS events
  THaEpicsEvtHandler *hcepics = new THaEpicsEvtHandler("epics", "HC EPICS event type 180");
  gHaEvtHandlers->Add(hcepics);
  // Add handler for scaler events
  THcScalerEvtHandler *hscaler = new THcScalerEvtHandler("H", "Hall C scaler event type 2");  
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
