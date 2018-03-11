//This scirpt will produce an updated version of hdc.param file, with
//the necessary t-zero corrections
#define time_shift 1280.0

void update_pdcparam()
{
	  
  //read run number from input file
  int run_NUM;
  TString f0 = "input_RUN.txt";
  ifstream infile(f0);
  infile >> run_NUM;   

  TString run = Form("run%d", run_NUM);

    int lin_NUM = 0;
	string t_zero[12];
	double tzero[12];
	string line;
   //open t_zero file
   ifstream ifs;
   ifs.open("../data_files/"+ run +"/tzero_weighted_avg_" + run + ".txt");
   

while (getline(ifs, line))
  {
    
    istringstream ss(line);
    char id;
    
    if ( ss >> t_zero)
      {
		
	if (id != '#') //skip comments   
	  {
	    //count lines  
	    lin_NUM = lin_NUM + 1;
	    cout << lin_NUM << endl;    
	       t_zero[lin_NUM-1] = line;
	       tzero[lin_NUM-1] = atof(t_zero[lin_NUM-1].c_str()); // convert string to double
	    cout << tzero[lin_NUM-1] << endl;
	  }       
	
      }     
    
  }
ifs.close();

//Update pdc.param parameter file 
TString pdc_param = "../../../PARAM/SHMS/DC/pdc_tracking_new.param";
ofstream ofs(pdc_param);

// ofs << ";---------------------------------------------------------------------" << endl;
// ofs <<"; SHMS_TRACKING"<< endl;
// ofs <<"; CTP parameter file containing all tracking parameters for the SHMS "<< endl;
// ofs <<";----------------------------------------------------------------------"<< endl;
// ofs <<"; sigma of wire chamber resolution for each plane "<< endl;
// ofs <<" pdc_sigma = 0.020 "<< endl;
// ofs <<"             0.020"<< endl;
// ofs <<"             0.020"<< endl;
// ofs <<"             0.020"<< endl;
// ofs <<"             0.020"<< endl;
// ofs <<"             0.020"<< endl;
// ofs <<"             0.020"<< endl;
// ofs <<"             0.020"<< endl;
// ofs <<"             0.020"<< endl;
// ofs <<"             0.020"<< endl;
// ofs <<"             0.020"<< endl;
// ofs <<"             0.020"<< endl;
// ofs <<"  pdc_tdc_min_win = -25000,-25000,-25000,-25000,-25000,-25000 "<< endl;
// ofs <<"  		    -25000,-25000,-25000,-25000,-25000,-25000 "<< endl;
// ofs <<"  pdc_tdc_max_win = 25000,25000,25000,25000,25000,25000 "<< endl;
// ofs <<"                    25000,25000,25000,25000,25000,25000 "<< endl;
// ofs <<"; hms drift chamber tdc's time per channel "<< endl;
// ofs <<"        pdc_tdc_time_per_channel = -0.10 "<< endl;
// ofs <<"; hms zero time for drift chambers	!DECREASING this number moves the hdtime plots to LOWER time. "<< endl;
// ofs <<"pdc_plane_time_zero =  ";


//*****************************************************************
//output all t_0 corrected values to pdc.param    
for (int i=0; i<12; i++) { 
{
if (i < = 5){
ofs  <<  time_shift - tzero[i]   << ","; 
}
if (i ==6) {ofs << "\n" << time_shift - tzero[6]  << ",";}
else if (i>6 && i <11) {
ofs << time_shift - tzero[i] << ","; 
}
if (i==11){ ofs << time_shift - tzero[i] << endl;}
}
}
//*****************************************************************
// ofs << "\n";
// ofs <<"; Dave Abbott's wire velocity correction "<< endl;
// ofs <<"pdc_wire_velocity = 12.0 "<< endl;
// ofs <<"pdc_central_time = 7,9,3,4,6,5 "<< endl;
// ofs << "                   7,5,3,4,6,6" << endl;
ofs.close();

//create a t_zero data file copy in another directory that will also use these values
TString tzero_dat = "../data_files/" + run + "/tzero.dat";
ofstream ofs(tzero_dat);

for (int i=0; i<12; i++) 
{
ofs  <<  tzero[i]  << endl; 
}
 
}
