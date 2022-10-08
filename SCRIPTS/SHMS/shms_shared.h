//This isn't really a header of sorts.
//It is simply a collection of functions that should always
//remain the same between analyses of the SHMS.  

//This helps ensure all the scripts created and used will have common gHcParms and Detector aparatuses.

//Make 'global' because I am lazy (and setupApparatus requires it).
ifstream bcmFile;

vector<TString> paths_to_data() {
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
}

// Define the analysis parameters
void setupParms(Int_t RunNumber) {
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
  TString bcmParamFile = Form("PARAM/SHMS/BCM/bcmcurrent_%d.param", RunNumber);
  bcmFile.open(bcmParamFile);
  if (bcmFile.is_open()) gHcParms->Load(bcmParamFile);
  
  // Load the Hall C detector map
  gHcDetectorMap = new THcDetectorMap();
  gHcDetectorMap->Load(gHcParms->GetString("g_ctp_map_filename"));
  return;
}

void setupApparatus() {
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
  return;
}
