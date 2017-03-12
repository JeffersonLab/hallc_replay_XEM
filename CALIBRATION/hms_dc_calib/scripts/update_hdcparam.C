//This scirpt will produce an updated version of hdc.param file, with
//the necessary t-zero corrections
#define time_shift 1300.0

void update_hdcparam()
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

//Update hdc.param parameter file
TString new_tzero = "../../../PARAM/HMS/DC/new_tzero.param";
ofstream ofs(new_tzero);

//output all t_0 corrected values to hdc.param
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

ofs.close();

//create a t_zero data file copy in another directory that will also use these values
TString tzero_dat = "../data_files/" + run + "/tzero.dat";
ofstream ofs(tzero_dat);

for (int i=0; i<12; i++)
{
ofs  <<  tzero[i]  << endl;
}

}
