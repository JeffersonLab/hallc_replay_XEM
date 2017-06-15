//SCRIPT TO RUN OVER ALL HMS DC CALIBRATION SCRIPTS AT ONCE, AND UPDATE THE
//NECESSARY PARAMTER FILES hdriftmap.param and  hdc.param


  using namespace std;
 
  void run_Cal()
  {
  
    //User Input filename, Run and  number of events to analyze
    string file_name;
    cout << "Enter Root File Name (without '.root' extension): " << endl;
    cin >> file_name;
    
    
  
    int run_NUM;
    cout << "Enter Run Number: " << endl;
    cin >> run_NUM;
    
  
    int num_evts;
    cout << "Enter Number of Events to analyze: " << endl;
    cin >> num_evts;  
    
    string suffix = ".root";
    
    

    
    //Create input file with run number
    ofstream fout;
    fout.open("scripts/input_RUN.txt", std::ofstream::trunc);
    fout << file_name+suffix << "   ";
    fout << run_NUM << "   ";
    fout << num_evts << "   ";
    fout.close();
    
    
    
    
    //Create root and data files Directories if they dont exist
    const char *dir_root = "mkdir ./root_files/";
    const char *dir_data = "mkdir ./data_files/";
    
    if (system(dir_root) != 0 || system(dir_data) != 0) 
      {
	cout << " ... creating 'root_files' directory " << endl;
	cout << " ... creating 'data_files' directory " << endl;
		
	system(dir_root);
	system(dir_data);
      }
    
    if (std::system(dir_data) != 0) 
      {
	  
      }
    
    
    
    //Create run Directories if they dont exist
    const char *dir0 = Form("mkdir ./root_files/run%d", run_NUM);
    const char *dir1 = Form("mkdir ./data_files/run%d", run_NUM);
    
    if (system(dir0) != 0 || system(dir1) != 0) 
      {
	system(dir0);
	system(dir1);
     
      } 
      

    //Make a copy of the input root file to be calibrated;
    string original_filepath;
    string dc_uncal_filepath;
    
    original_filepath = "../../ROOTfiles/"+file_name+suffix;
    dc_uncal_filepath = "../../ROOTfiles/"+file_name+"_dc_uncal"+suffix; 
    gSystem->CopyFile(original_filepath.c_str(), dc_uncal_filepath.c_str()); 
    
	
    
    //change directories and execute scripts
    gSystem->cd("./scripts");
    gSystem->Exec("root -l -q get_pdc_time_histo.C");
	
    //Load and Loop over Make Class events to get individual drift times
    gROOT->LoadMacro("wire_drift_times.C");
    gROOT->ProcessLine("wire_drift_times t"); //process line allows one to execute interactive root commands from a script, such as this one
    gROOT->ProcessLine("t.Loop()");
    //gROOT->ProcessLine(".q");
    gROOT->Reset();
    
	
    
    gSystem->cd("./scripts");
    
	
    //execute code to get t0 from each wire in each plane
    gSystem->Exec("root -l -q -b get_wire_tzero.C");
    
    //execute code to update parameter file with new t0 values per wire
    gSystem->Exec("root -l -q get_tzero_per_wire_param.C"); 
    
    //execute code to get t0 corrected drift times
    gSystem->Exec("root -l -q get_pdc_time_histo_tzero_corrected.C");
    
    //execute code to update LookUp Table
    gSystem->Exec("root -l -q get_LookUp_Values.C");
    
    
  }
  

