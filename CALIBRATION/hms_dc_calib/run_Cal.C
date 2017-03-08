//SCRIPT TO RUN OVER ALL HMS DC CALIBRATION SCRIPTS AT ONCE, AND UPDATE THE
//NECESSARY PARAMTER FILES hdriftmap.param and  hdc.param
void run_Cal()
{
      
  //User Input Run
  int run_NUM;
  cout << "Enter Run Number: " << endl;
  cin >> run_NUM;

  //Create input file with run number
  ofstream fout;
  fout.open("scripts/input_RUN.txt");
  fout << run_NUM << endl;
  fout.close();


  //Create root and data files Directories if they dont exist
  char *dir_root = "mkdir ./root_files/";
  char *dir_data = "mkdir ./data_files/";

  if (system(dir_root || dir_data) != 0) {
    system(dir_root);
    system(dir_data);
  }

  //Create run Directories if they dont exist
  char *dir0 = Form("mkdir ./root_files/run%d", run_NUM);
  char *dir1 = Form("mkdir ./data_files/run%d", run_NUM);

  if (system(dir0 || dir1) != 0) {
    system(dir0);
    system(dir1);
  }

  

  //change directories and execute scripts
  gSystem->cd("./scripts");
  gSystem->Exec("root -l -q get_hdc_time_histo.C");
  
  //Load and Loop over Make Class events to get individual drift times
  gROOT->LoadMacro("wire_drift_times.C");
  gROOT->ProcessLine("wire_drift_times t"); //process line allows one to execute interactive root commands from a script, such as this one
  gROOT->ProcessLine("t.Loop()");
  //gROOT->ProcessLine(".q");
  gROOT->Reset();
  
  

gSystem->cd("./scripts");

  
  //execute code to get t0 from each wire in each plane
  gSystem->Exec("root -l -q -b get_wire_tzero.C");
 
  //execute code to update hdc parameter file
  gSystem->Exec("root -l -q update_hdcparam.C");

  //execute code to get t0 corrected drift times
  gSystem->Exec("root -l -q get_hdc_time_histo_tzero_corrected.C");
  
 //execute code to update LookUp Table
  gSystem->Exec("root -l -q get_LookUp_Values.C");

  
  
}
