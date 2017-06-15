#define NPLANES 12

using namespace std;
void get_pdc_time_histo_tzero_corrected()
{
  
  
  int run_NUM;
  Long64_t num_evts;        //added
  string input_file;   //added

  TString f0 = "input_RUN.txt";
  ifstream infile(f0);
  infile >> input_file >> run_NUM >> num_evts;   

  TString run = Form("run%d", run_NUM);

  //this script reads all tzero values inside tzero_group.dat and assigns them to their corresponding planes. 
  //these values will then be used to shift the drift time histos, wire by wire (t0-correction wire-by-wire)

  int ip;  //to loop over planes
  int sw;  //to loop over sensewires


  int tot_wires[NPLANES] = {107, 107, 79, 79, 107, 107, 107, 107, 79, 79, 107, 107};

  //sum over all wires in both DC
  static int wire_sum = 0;
  for (ip=0; ip<NPLANES; ip++) {
    wire_sum = wire_sum + tot_wires[ip];
  }
  

  //open and read tzero data file 
  ifstream file;
  file.open("../data_files/"+run+"/tzero_values_per_wire.dat");
  
  string line;
  int counter;
  Double_t value;
  Double_t tzero_offset[wire_sum];
   

  counter = 0;
    
  while (getline(file, line)) {
    
    if (line[0]!='#' ) 
      {
	sscanf(line.c_str(), "%lf", &value);  //pass data in file to variable 'value'
	tzero_offset[counter] = value;  //write all tzero values for all wires in both DC to an array
	//cout << tzero_offset[counter] << endl;
	counter++;
      }
  }
  

  //***************************************************************
  
  //pass all tzero values into their corresponding planes
  
  //declare a 2d dynamic array to store tzero values to be used for t0-correction
  Double_t **tzero = new Double_t*[NPLANES];
    
  for (ip = 0; ip < NPLANES; ip++) 
    {
      tzero[ip] = new Double_t[tot_wires[ip]];
    }
  
  //initialize 2d dynamic array to 0.0
  for (ip = 0; ip<NPLANES; ip++) {
    for (sw = 0; sw<tot_wires[ip]; sw++) {
      tzero[ip][sw] = 0.0;
 
    }
  }
    
  counter = 0;
  for (ip = 0; ip<NPLANES; ip++) {
   
    for (sw = 0; sw < tot_wires[ip]; sw++) {
      
      tzero[ip][sw] = tzero_offset[counter];   //tzero corrections that must be added wire by wire
      //cout << tzero[ip][sw] << endl;
      //cout <<  tzero[ip][sw] << " :: "<<tzero_offset[counter] << endl;
	   counter++;
    }
  }





  //*****************************************************************************************

  //THIS SCRIPT WILL READ the drift times array tzero[ip][sw] on a wire basis and apply the tzero correction.



  int i;
 
  int nbins = 200; // set number of bins in histos
  int bin_width = 2; 
  int bin_Content;
  Double_t shift;  //will be the t0 offset

  //Declare plane names to loop over
  
  TString plane_names[NPLANES]={"1u1", "1u2", "1x1", "1x2", "1v1", "1v2", "2v2", "2v1", "2x2", "2x1", "2u2", "2u1"};
  
  //Declare a root file to read individual DC cell drift times
  TString root_file;
  TFile *f[NPLANES];

  //Declare root file were NEW tzero corrected histograms will be added
  TString file_name =  Form("../root_files/run%d/shms_tzero_corr_histos.root", run_NUM);
  TFile *g = new TFile(file_name, "RECREATE");
    

  TH1F *h_add[NPLANES]; //t0-corrected plane drift times 

 
 

 //Loop over all planes
 for (ip = 0; ip < NPLANES; ip++){

   //READ wire drift time root file per plane
   root_file = "../root_files/"+run+"/shms_DC_"+plane_names[ip]+Form("_%d_wire_histos.root",run_NUM);
   f[ip] = new TFile(root_file, "READ");
   

   h_add[ip] =new TH1F("plane_"+plane_names[ip]+"drifttime", "", nbins, -50, 350);
   
   h_add[ip]->GetXaxis()->SetTitle("Drift Time (ns)");
   h_add[ip]->GetYaxis()->SetTitle("Number of Entries / 2 ns");

   TH1F *cell_dt[tot_wires[ip]];
   TH1F *cell_dt_corr[tot_wires[ip]];
 
 	
   //Get wire histos from root file and loop over each 
   //  sense wire of a plane in shms Drift Chambers (DC1 or DC2)
   
   f[ip]->cd(); //change to file containing uncorrected wire drift times
   
   for (sw=1; sw<=tot_wires[ip]; sw++){

    

     //set title of histos in root file
     TString drift_time_histo = Form("wire_%d", sw); 
 
     //Get drift time histograms from root file
     cell_dt[sw-1] = (TH1F*)f[ip]->Get(drift_time_histo);  
     

     //Create corrected wire histos
     cell_dt_corr[sw-1] = new TH1F(plane_names[ip]+Form("_wire_%d_corr", sw), "", nbins, -50, 350);
     
     cell_dt_corr[sw-1]->GetXaxis()->SetTitle("Drift Time (ns)");
     cell_dt_corr[sw-1]->GetYaxis()->SetTitle("Number of Entries / 2 ns");


     shift = tzero[ip][sw-1];  //the shift represents how much the drift time histo needs to be offset

	//cout << "sw: " << sw << " :: " << "offset: " << shift << endl;
    
     //************APPLY TZERO OFFSET ON A WIRE-BY-WIRE BASIS TO ALL WIRES IN ALL PLANES***************


     //INCLUDE the code 'shift.C ', which shifts a histogram 

     for (i=1; i<=nbins; i++) {
       
       bin_Content = cell_dt[sw-1]->GetBinContent(i);
       
       
       cell_dt_corr[sw-1]->SetBinContent(i-shift/bin_width, bin_Content);  //apply the t0 correction
       
     }




     //*************************************************************************************************
     


     //write wire drift times (after tzero corrections) to file
     g->cd();
     cell_dt_corr[sw-1]->Write(plane_names[ip]+Form("_wire_%d", sw), TObject::kWriteDelete); 
     
     
     //add all cell drift times into a single plane
     h_add[ip]->Add(cell_dt_corr[sw-1]);
     
     
     
     
   } // end loop over sense wires
   
   
   //Wire combined wire drift times (t0 -corrected) for each plane to file, 
   g->cd();
   h_add[ip]->Write();
   

 } // end loop over planes
 
 
 
  

}
