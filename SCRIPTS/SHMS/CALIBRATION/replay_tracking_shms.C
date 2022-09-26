//Not intended to be standalone. Must be called in SCRIPT from top directory of hallc_replay_XEM
//Same functionality as any SCRIPT in hallc_replay_XEM
#include "SCRIPTS/SHMS/shms_shared.h"

void replay_tracking_shms (Int_t RunNumber = 0, Int_t MaxEvent = 0, 
			   double x_stub_c=25., double y_stub_c=7., double xp_stub_c=0.7,
			   double yp_stub_c=0.2, double sp_c=1.2, int max_pr_hits=25,
			   int p_min_combos=4, int p_min_hit=5) {

  // Get RunNumber and MaxEvent if not provided.
  if(RunNumber == 0) { cout << "Enter a Run Number (-1 to exit): ";
    cin >> RunNumber; if( RunNumber<=0 ) {
      cerr << "...Invalid RunNumber entry\n";exit;}}
  if(MaxEvent == 0) {cout << "\nNumber of Events to analyze: ";
    cin >> MaxEvent; if(MaxEvent == 0) {
      cerr << "...Invalid MaxEvent entry\n";exit;}}
  vector<TString> pathList =paths_to_data();

  std::ofstream tracking_param;
  tracking_param.open(Form("./tracking_param_tmp_%3.1f_%3.1f_%3.2f_%3.2f_%3.2f_%d_%d_%d.param",x_stub_c, y_stub_c, xp_stub_c, yp_stub_c, sp_c, max_pr_hits, p_min_combos, p_min_hit));
  tracking_param << Form("pxt_track_criterion = %f\n",x_stub_c);  
  tracking_param << Form("pyt_track_criterion = %f\n",y_stub_c);  
  tracking_param << Form("pxpt_track_criterion = %f\n",xp_stub_c);  
  tracking_param << Form("pypt_track_criterion = %f\n",yp_stub_c);  
  tracking_param << Form("pspace_point_criterion = %f, %f\n", sp_c, sp_c);
  tracking_param << Form("pmax_pr_hits = %d, %d\n", max_pr_hits, max_pr_hits);  
  tracking_param << Form("pmin_combos = %d, %d\n",p_min_combos, p_min_combos);  
  tracking_param << Form("pmin_hit = %d, %d\n",p_min_hit, p_min_hit);  
  tracking_param.close();

  // Create file name patterns.
  const char* RunFileNamePattern = "shms_all_%05d.dat";  //Raw data file name pattern
  const char* ROOTFileNamePattern = "ROOTfiles/SHMS/CALIBRATION/shms_replay_tracking_%d_%d_%3.1f_%3.1f_%3.2f_%3.2f_%3.2f_%d_%d_%d.root";
  TString ROOTFileName = Form(ROOTFileNamePattern, RunNumber, MaxEvent, x_stub_c, y_stub_c, xp_stub_c, yp_stub_c, sp_c, max_pr_hits, p_min_combos, p_min_hit);
  //Specifics for the replay
  TString odef_file = "DEF-files/SHMS/PRODUCTION/pstackana_production.def";
  TString cdef_file = "DEF-files/SHMS/PRODUCTION/CUTS/pstackana_production_cuts.def";
  TString summary_file = Form("REPORT_OUTPUT/SHMS/",
			      RunNumber, MaxEvent);
  TString report_file  = Form("REPORT_OUTPUT/SHMS/CALIBRATION/replay_shms_production_%d_%d_%3.1f_%3.1f_%3.2f_%3.2f_%3.2f_%d_%d_%d.report", RunNumber, MaxEvent, x_stub_c, y_stub_c, xp_stub_c, yp_stub_c, sp_c, max_pr_hits, p_min_combos, p_min_hit);

  //Initialize gHcParms.
  //Shared SHMS gHcParms setup located in ../shms_shared.h
  setupParms(RunNumber);
  //==============================================================
  //Load tracking parameters from file for this run
  cout << "Setting tracking parameters from input.\n";
  gHcParms->Load(Form("./tracking_param_tmp_%3.1f_%3.1f_%3.2f_%3.2f_%3.2f_%d_%d_%d.param",x_stub_c, y_stub_c, xp_stub_c, yp_stub_c, sp_c, max_pr_hits, p_min_combos, p_min_hit));
  //Show that the parameters were actually loaded properly.
  gHcParms->PrintFull("*criterion");
  gHcParms->PrintFull("pmax_pr_hits");
  gHcParms->PrintFull("pmin_combos");
  gHcParms->PrintFull("pmin_hit");
  //==============================================================

  //Initialize SHMS single-arm DAQ with detectors
  //Shared SHMS apparatus setup located in ../shms_shared.h
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
  //analyzer->SetSummaryFile(summary_file);  // optional

  // Start the actual analysis.
  analyzer->Process(run);

  // Create report file from template
  //analyzer->PrintReport(gHcParms->GetString("g_ctp_template_filename"),
  //			report_file);  // optional

}
