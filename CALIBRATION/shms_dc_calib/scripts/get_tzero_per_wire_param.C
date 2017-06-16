#include <iostream>
#include <fstream>
#include <math.h>
#include <string>

#define NPLANES 12

using namespace std;
void get_tzero_per_wire_param()
{

  
 //read run number from input file
  int run_NUM;
  Long64_t num_evts;        //added
  string input_file;   //added
  TString f0 = "input_RUN.txt";
  ifstream infile(f0);
  infile >> input_file >> run_NUM >> num_evts;   

  TString run = Form("run%d", run_NUM);

  TString planes[NPLANES] =   {"1u1","1u2","1x1","1x2","1v1","1v2","2v2","2v1","2x2","2x1","2u2","2u1"};   //planes 0, 1, 2, 3, 4, 5 OK :: 


  int fNWires[NPLANES] = {107, 107, 79, 79, 107, 107, 107, 107, 79, 79, 107, 107};

 int sw;
 int ip;
 
Int_t **wire = new Int_t*[NPLANES];
Double_t **t0 = new Double_t*[NPLANES];
//Double_t **t0_err = new Double_t*[NPLANES];
//Int_t **entries = new Int_t*[NPLANES];

 for (ip = 0; ip < NPLANES; ip++) {
   t0[ip] = new Double_t[fNWires[ip]];
   for (sw=0; sw<fNWires[ip]; sw++) {
     t0[ip][sw] = 0.0;
   }
 }

 
  
//Loop over each plane
 for(ip=0; ip<NPLANES; ip++) {
   
         //write plane headers

      int nwire;
      double nt0;
      double nt0_err;
      int nentries;


    //open and read each wire tzero file
      TString file_name = Form("../data_files/run%d/shms_dc_", run_NUM) + planes[ip] + Form("tzero_run%d_updated.dat", run_NUM);
      
      //  cout << file_name << endl;

      string line;
      ifstream input;
      input.open(file_name);
      
    
       sw = 0;
      while (getline(input, line))
	{
	  // cout << line << endl;
	  if (line[0]!='#')
	    {	  
	      if(!input.good()) {break;}
	      sscanf(line.c_str(), "%d %lf", &nwire, &nt0);
	     
	      if(sw>0) {
	      
	       t0[ip][nwire-1] = nt0;
	    
	      }
	      sw++;
	    
	    }
	  
	} 
   
      input.close();
    
    


     
 } //end loop over planes
 
 
//create output file stream to write tzero values to a data file
 ofstream out;
 TString tzero_file = "../data_files/"+run+"/tzero_values_per_wire.dat";
 out.open(tzero_file);
 
 //Create an output parameter file to store tzero values 
 ofstream ofs;
 TString wire_tzero = Form("../../../PARAM/SHMS/DC/pdc_tzero_per_wire_run%d_NEW.param", run_NUM);
 ofs.open (wire_tzero);
 

 for (ip=0; ip<NPLANES; ip++) { 

   //write plane headers
   ofs << "ptzero"+planes[ip] << "=" << endl;
   out << "#PLANE: " << planes[ip] << endl;

   for (sw=0; sw<fNWires[ip]; sw++) {

     out << setprecision(3) << t0[ip][sw] << endl;
     
     if (sw <= 10) 
       { 
	 ofs << setprecision(3) << t0[ip][sw] << fixed << ",";
       }
     else if (sw>10 && sw <(fNWires[ip]-1))
       {
	 ofs << setprecision(3) << t0[ip][sw] << ((sw+1) % 16 ? ", " : "\n") << fixed;
       }
     else if (sw==78 || sw == 106) {
       ofs << setprecision(3) << t0[ip][sw] << fixed << endl;
      }

    

   }
 }

 //close files
 ofs.close();
 out.close();

}
 
 



 






