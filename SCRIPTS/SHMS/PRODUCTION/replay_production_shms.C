//Not intended to be standalone. Must be called in SCRIPT from top directory of hallc_replay_XEM
//Same functionality as any SCRIPT in hallc_replay_XEM
#include "SCRIPTS/SHMS/shms_shared.h"

void replay_production_shms (Int_t RunNumber = 0, Int_t MaxEvent = 0) {

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
  // Create file name patterns.
  const char* RunFileNamePattern = "shms_all_%05d.dat";  //Raw data file name pattern
//THESE ARE THE ORIGINALS  
  // const char* ROOTFileNamePattern = "ROOTfiles/SHMS/PRODUCTION/shms_replay_production_%d_%d.root";
//THESE ARE THE EXTRA ONES FOR OFFSET STUDY
  const char* ROOTFileNamePattern = "tempOUTPUT/shms_replay_production_%d_%d.root";
  TString ROOTFileName = Form(ROOTFileNamePattern, RunNumber, MaxEvent);
  //Specifics for the replay
  TString odef_file = "DEF-files/SHMS/PRODUCTION/pstackana_production.def";
// IF the replay is for runs with preshower blocks turned ON (like most of the production data), use this one:
  TString cdef_file = "DEF-files/SHMS/PRODUCTION/CUTS/pstackana_production_cuts.def";
// IF the replay is for runs with preshower blocks turned OFF, use this one:
//  TString cdef_file = "DEF-files/SHMS/PRODUCTION/CUTS/pstackana_production_cuts_xCalCuts.def";
//THESE ARE THE ORIGINALS  
  // TString summary_file = Form("REPORT_OUTPUT/SHMS/PRODUCTION/summary_production_%d_%d.report",
	// 		      RunNumber, MaxEvent);
  // TString report_file  = Form("REPORT_OUTPUT/SHMS/PRODUCTION/replay_shms_production_%d_%d.report",
	// 		      RunNumber, MaxEvent);
//THESE ARE THE EXTRA ONES FOR OFFSET STUDY
  TString summary_file = Form("tempOUTPUT/summary_production_%d_%d.report",
			      RunNumber, MaxEvent);
  TString report_file  = Form("tempOUTPUT/replay_shms_production_%d_%d.report",
			      RunNumber, MaxEvent);

  //Initialize gHcParms.
  //Shared SHMS gHcParms setup located in ../shms_shared.h
  //setupParms(RunNumber);
    // Load global parameters
  gHcParms->Define("gen_run_number", "Run Number", RunNumber);
  gHcParms->AddString("g_ctp_database_filename", "DBASE/SHMS/standard.database");
  gHcParms->Load(gHcParms->GetString("g_ctp_database_filename"), RunNumber);
  gHcParms->Load(gHcParms->GetString("g_ctp_parm_filename"));
  gHcParms->Load(gHcParms->GetString("g_ctp_kinematics_filename"), RunNumber);
  gHcParms->Load(gHcParms->GetString("g_ctp_det_calib_filename"));
  gHcParms->Load(gHcParms->GetString("g_ctp_bcm_calib_filename"));
  gHcParms->Load(gHcParms->GetString("g_ctp_optics_filename"));
  // Load parameters for SHMS trigger configuration
  gHcParms->Load(gHcParms->GetString("g_ctp_trig_config_filename"));
  // Load fadc debug parameters
  gHcParms->Load("PARAM/SHMS/GEN/p_fadc_debug.param");
  // Load BCM values
  ifstream bcmFile;
  TString bcmParamFile = Form("PARAM/SHMS/BCM/bcmcurrent_%d.param", RunNumber);
  bcmFile.open(bcmParamFile);
  if (bcmFile.is_open()) gHcParms->Load(bcmParamFile);
  
  // Load the Hall C detector map
  gHcDetectorMap = new THcDetectorMap();
  gHcDetectorMap->Load(gHcParms->GetString("g_ctp_map_filename"));
  //Initialize SHMS single-arm DAQ with detectors
  //Shared SHMS apparatus setup located in ../shms_shared.h
  //setupApparatus();
    // Add trigger apparatus
  THaApparatus* TRG = new THcTrigApp("T", "TRG");
  gHaApps->Add(TRG);
  // Add trigger detector to trigger apparatus
  THcTrigDet* shms = new THcTrigDet("shms", "SHMS Trigger Information");
  TRG->AddDetector(shms);

  // Set up the equipment to be analyzed.
  THcHallCSpectrometer* SHMS = new THcHallCSpectrometer("P", "SHMS");
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

  // Add rastered beam apparatus
  THaApparatus* beam = new THcRasteredBeam("P.rb", "Rastered Beamline");
  gHaApps->Add(beam);
  // Add physics modules
  // Add beam current monitor module
  if (bcmFile.is_open()) {
    THcBCMCurrent* bcm = new THcBCMCurrent("P.bcm", "BCM Module");
    gHaPhysics->Add(bcm);
  }
  // Calculate reaction point
  THcReactionPoint* prp = new THcReactionPoint("P.react", "SHMS reaction point", "P", "P.rb");
  gHaPhysics->Add(prp);
  // Calculate extended target corrections
  THcExtTarCor* pext = new THcExtTarCor("P.extcor", "SHMS extended target corrections", "P", "P.react");
  gHaPhysics->Add(pext);
  // Calculate golden track quantites
  THaGoldenTrack* gtr = new THaGoldenTrack("P.gtr", "SHMS Golden Track", "P");
  gHaPhysics->Add(gtr);
  // Calculate primary (scattered beam - usually electrons) kinematics
  THcPrimaryKine* kin = new THcPrimaryKine("P.kin", "SHMS Single Arm Kinematics", "P", "P.rb");
  gHaPhysics->Add(kin);
  // Calculate the hodoscope efficiencies
  THcHodoEff* peff = new THcHodoEff("phodeff", "SHMS hodo efficiency", "P.hod");
  gHaPhysics->Add(peff);   

  // Add event handler for prestart event 125.
  THcConfigEvtHandler* ev125 = new THcConfigEvtHandler("HC", "Config Event type 125");
  gHaEvtHandlers->Add(ev125);
  // Add event handler for EPICS events
  THaEpicsEvtHandler* hcepics = new THaEpicsEvtHandler("epics", "HC EPICS event type 180");
  gHaEvtHandlers->Add(hcepics);
  // Add event handler for scaler events
  THcScalerEvtHandler* pscaler = new THcScalerEvtHandler("P", "Hall C scaler event type 1");
  pscaler->AddEvtType(1);
  pscaler->AddEvtType(129);
  pscaler->SetDelayedType(129);
  pscaler->SetUseFirstEvent(kTRUE);
  gHaEvtHandlers->Add(pscaler);

  /*
  //Add event handler for helicity scalers
  THcHelicityScaler *phelscaler = new THcHelicityScaler("P", "Hall C helicity scaler");
  //phelscaler->SetDebugFile("PHelScaler.txt");
  phelscaler->SetROC(8);   
  phelscaler->SetUseFirstEvent(kTRUE); 
  gHaEvtHandlers->Add(phelscaler); 
  */
  
  // Add event handler for DAQ configuration event
  THcConfigEvtHandler *pconfig = new THcConfigEvtHandler("pconfig", "Hall C configuration event handler");
  gHaEvtHandlers->Add(pconfig);

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
