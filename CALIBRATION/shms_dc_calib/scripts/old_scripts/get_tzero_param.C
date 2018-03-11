#include <iostream>
#include <fstream>
#include <math.h>
#include <string>

#define NPLANES 12


void get_tzero_param()
{

 //read run number from input file
  int run_NUM;
  TString f0 = "input_RUN.txt";
  ifstream infile(f0);
  infile >> run_NUM;   

  TString run = Form("run%d", run_NUM);

  //                             0     1     2     3     4     5     6     7     8     9    10     11
  //TString planes[NPLANES] = {"1u1","1u2","1x1","1x2","1v1","1v2","2u1","2u2","2x1","2x2","2v1","2v2"};

  TString planes[NPLANES] =   {"1u1","1u2","1x1","1x2","1v1","1v2","2v2","2v1","2x2","2x1","2u2","2u1"};   //planes 0, 1, 2, 3, 4, 5 OK :: 

  //what needs to be changed

  //2u1 (6) --> 2v2 (11)||    2u2 (7) --> 2v1 (10)||,  2x1 (8) --> 2x2(9)||,   


int fNWires[NPLANES] = {107, 107, 79, 79, 107, 107, 107, 107, 79, 79, 107, 107};
int group_size[NPLANES] = {7, 7, 5, 5, 7, 7, 7, 7, 5, 5, 7, 7};

int group;
int sw;
int ip;


  
//allocate space for calc. of  weighted avg of cable group
Double_t *sum_NUM; 
Double_t *sum_DEN; 
Double_t *weighted_AVG;
Double_t *weighted_AVG_err;

Int_t *wire;
Double_t *t0;
Double_t *t0_err;
Int_t *entries;

Int_t **dc_group_min;
Int_t **dc_group_max;

//dynamic allocation of a 2d array in dim. of plane and groups, (depending on which plane, group size is 5 or 7)
dc_group_min = new Int_t*[NPLANES];
for (ip=0; ip<NPLANES; ip++){
  dc_group_min[ip] = new Int_t[group_size[ip]];
}


//Fill the 2d array with 1st sensewire corresponding to each group in a given plane
 for (ip=0; ip<NPLANES; ip++) {

   dc_group_min[ip][0]=1;  //set initial array value to 1st sensewire in the group
   for (group=1; group < group_size[ip]; group++){
 
     if (ip==2 || ip==9){
      //dc_group_min[ip][5] = {1, 17, 33, 49, 65};   //plane 2(1x1) or plane9(2x1)
       dc_group_min[ip][group] = dc_group_min[ip][group-1] + 16;
     }     
       
     else if (ip==3 || ip==8){
       //dc_group_min[ip][5] = {{1, 16, 32, 48, 64}};  //plane 3(1x2) or plane 8(2x2)
       dc_group_min[ip][1]=16;
       dc_group_min[ip][group+1] = dc_group_min[ip][group] + 16;
     }
          
     else if(ip==0 || ip==4 || ip==11 || ip ==7){
       // dc_group_min[ip][7] = {{1, 16, 32, 48, 64, 80, 96}}; //planes: 1u1, 1v1, 2u1, 2v1  
       dc_group_min[ip][1]=16;
       dc_group_min[ip][group+1] = dc_group_min[ip][group] + 16;
     }
     
     else if(ip==1 || ip==5 || ip==10 || ip==6){
       // dc_group_min[ip][7] = {{1, 13, 29, 45, 61, 77, 93}}; //planes 1u2, 1v2, 2u2, 2v2
       dc_group_min[ip][1]=13;
       dc_group_min[ip][group+1] = dc_group_min[ip][group] + 16;
     }
     
   }//end loop over groups
   
   
 }//end loop over planes

 //****************************************

 //dynamic allocation of a 2d array in dim. of plane and groups, (depending on which plane, group size is 5 or 7)
dc_group_max = new Int_t*[NPLANES];
for (ip=0; ip<NPLANES; ip++){
  dc_group_max[ip] = new Int_t[group_size[ip]];
}

//Fill the 2d array with last sensewire corresponding to each group in a given plane
 for (ip=0; ip<NPLANES; ip++) {

   
   for (group=1; group < group_size[ip]; group++){
 
     if (ip==2 || ip==9){
      //dc_group_max[ip][5] = {16, 32, 48, 64, 79};   //plane 2(1x1) or plane 9(2x1)
       dc_group_max[ip][0]=16;
       dc_group_max[ip][group] = dc_group_max[ip][group-1] + 16;
     }     
       
     else if (ip==3 || ip==8){
       //dc_group_max[ip][5] = {15, 31, 47, 63, 79}; //plane 3(1x2) or plane 8(2x2)
       dc_group_max[ip][0]=15;
       dc_group_max[ip][group] = dc_group_max[ip][group-1] + 16;
     }
          
     else if(ip==0 || ip==4 || ip==11 || ip ==7){
       // dc_group_max[ip][7] = {15, 31, 47, 63, 79, 95, 107}; //planes: 1u1, 1v1, 2u1, 2v1 
       dc_group_max[ip][0]=15;
       dc_group_max[ip][group] = dc_group_max[ip][group-1] + 16;
     }
     
     else if(ip==1 || ip==5 || ip==10 || ip==6){
       // dc_group_max[ip][7] = {12, 28, 44, 60, 76, 92, 107}; //planes 1u2, 1v2, 2u2, 2v2
       dc_group_max[ip][0]=12;
       dc_group_max[ip][group] = dc_group_max[ip][group-1] + 16;
     }
     
   }//end loop over groups
   
   
 }//end loop over planes





 //*********************************
 //sanity check to see if arrays were filled properly!
 // for (int i=0; i<7; i++) {
 // cout << dc_group_max[1][i] << endl;
 // }
 //note: some planes max values exceeded th ephysical max wire, 
 //but does not pose any problems when setting the limits
 //*********************************
 
 
 

  //declare a 2d dynamic arrays to store weighted average per group
  Double_t** weighted_avg = new Double_t*[NPLANES];
  Double_t** weighted_avg_err = new Double_t*[NPLANES];
 
 for (ip = 0; ip < NPLANES; ip++) {
    weighted_avg[ip] = new Double_t[group_size[ip]];
    weighted_avg_err[ip] = new Double_t[group_size[ip]];
 }
  
  //initialize a 2d dynamic array
 for (ip=0; ip<NPLANES; ip++) {
   for (group=0; group<group_size[ip]; group++) {
     weighted_avg[ip][group] = 0.0;
     weighted_avg_err[ip][group] = 0.0;
    }
  }
  

//Loop over each plane
 for(ip=0; ip<NPLANES; ip++) {


      
    wire = new Int_t[fNWires[ip]];
    t0 = new Double_t[fNWires[ip]];
    t0_err = new Double_t[fNWires[ip]];
    entries = new Int_t[fNWires[ip]];

    
    //open and read each wire tzero file
    ifstream ifs;
    ifs.open(Form("../data_files/run%d/hdc_"+planes[ip]+"tzero_run%d_updated.txt", run_NUM, run_NUM ) );
    


    if (ifs.is_open()) {
      //    cout << "File opened!" << endl;
       ifs.ignore(50000, '\n');    //ignore the first line 
      
      for (sw=0; sw<fNWires[ip]; ++sw) {
	ifs >> wire[sw] >> t0[sw] >> t0_err[sw] >> entries[sw];
	//cout
	//	  << "wire: " << wire[sw] << "  " << endl;
	//  << "t0: " << t0[sw] << "  "
	//  << "t0_err: " << t0_err[sw] << "  "
	//  << "entries: " << entries[sw] << endl;
      } 
      ifs.close();
    }  // ifstream condition
    else {
      // cout << "File not opened!" << endl;
    }
    
    //Loop over each ribbon cable group of wires
    for (group =0; group<group_size[ip]; group++) {
      
	//initialize variables
	double sum_num = 0.0;
	double sum_den = 0.0;
	double w_avg = 0.0;
	double w_avg_err = 0.0;

      for (sw = 1; sw<=fNWires[ip]; sw++) {
	
	// compute the sums if the sense wires lie in a given group
	if (sw >= dc_group_min[ip][group] && sw <= dc_group_max[ip][group] && t0[sw-1]!=0) {
	  
	  //calculate numerator and denominator of weighted average for each group
	 
	  sum_num = sum_num + t0[sw-1]/(t0_err[sw-1]*t0_err[sw-1]);
	  sum_den = sum_den + 1.0/(t0_err[sw-1]*t0_err[sw-1]);  
	 
	  
	}
       
      } // end loop over sensewires
    
      //compute weighted average per ribbon cable group
      if (sum_den!=0.0) { 
	w_avg = sum_num / sum_den;
	w_avg_err = sqrt( 1.0 / sum_den ); 
	
	weighted_avg[ip][group] = w_avg; //write weighted average to an array
	weighted_avg_err[ip][group] = w_avg_err;
      }
      
    } //end loop over groups
    
 } //end loop over planes


  //create output file stream to write tzero values to a data file
  ofstream out;
  TString tzero_file = "../data_files/"+run+"/tzero_group.dat";
  out.open(tzero_file);
    
  //Create an output parameter file to store tzero values 
  ofstream ofs;
  TString wire_tzero = Form("./pdc_tzero_per_wire_run%d.param", run_NUM);
  ofs.open (wire_tzero);

  //loop over planes
  for (ip = 0; ip < NPLANES; ip++) 
    {
      //write plane headers
      ofs << "ptzero"+planes[ip] << "=" << endl;
      out << "#PLANE: " << planes[ip] << endl;
      //loop over group
      for (group = 0; group < group_size[ip]; group++)
	{
	  out << "#GROUP: " << group+1 << endl;

	  //loop over sensewires
	  for (sw = 1; sw<=fNWires[ip]; sw++) {
	    
	    if (sw >= dc_group_min[ip][group] && sw <= dc_group_max[ip][group]) 
	      {
	       out << setprecision(7) << weighted_avg[ip][group] << endl;
		if (sw<fNWires[ip])
		  {
		    ofs << setprecision(7) << weighted_avg[ip][group] << ((sw+1) % 16 ? ", " : "\n") << fixed;
		  }
		else {
		  ofs << setprecision(7) << weighted_avg[ip][group] << fixed << endl;
		  ofs << endl;
		}
	      }
	    
	  } // end loop over sensewires
	  
	}// end loop over groups 
    
    }//end loop over planes

  //close files
  ofs.close();
  out.close();

}


      
      
 

    
    
   

  

