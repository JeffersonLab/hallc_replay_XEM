/*This code produces a lookup table necessary to convert drift times to
drift distances in the HMS drift chambers
*/

#define NPLANES 12
#define TOTAL_BINS 137

void get_LookUp_Values() {

   
  //Read Run Number from txt file
  int run_NUM;
 TString f0 = "input_RUN.txt";
 ifstream infile(f0);
 infile >> run_NUM;
 
//Open root file containing drift time histos
 TFile *f = new TFile(Form("../root_files/run%d/hms_dc_t0_corrected_%d.root", run_NUM, run_NUM),"READ");
 
 //Define histogram array
 TH1F *h[NPLANES];
 
 //Define the number Drift Chamber planes
 TString plane_names[NPLANES]={"1x1", "1y1", "1u1", "1v1", "1y2", "1x2", "2x1", "2y1", "2u1", "2v1", "2y2", "2x2"};
 
 //Declare bin properties
	int bin_t0[NPLANES];
  int bin_final[NPLANES];					/*Array to store the bin number corresponding to last bin*/
  int bin_Content[NPLANES];          /*Array to store the content (# events) corresponding to the bin with maximum content*/
  double binContent_TOTAL[NPLANES];     /*Array to store sum of all bin contents for each plane*/
  double binSUM[NPLANES];
  int bin;
  int binx;
  double lookup_value[NPLANES];           /*Array to store lookup values for each plane*/ 
  
  //Create an output file to store lookup values  
  ofstream ofs;
 TString lookup_table = "../../../PARAM/HMS/DC/hdriftmap_new.param";
 ofs.open (lookup_table);
 
 
//Set headers for subsequent columns of data
 ofs << Form("; Lookup Table: RUN %d", run_NUM) << "\n";
 ofs << "; number of bins in Carlos's time to distance lookup table" << "\n";
 ofs << Form("hdriftbins = %d", TOTAL_BINS+1) << "\n";
 ofs << "; number of 1st bin in Carlos's table in ns" << "\n";
 ofs << "hdrift1stbin=0" << "\n";
 ofs << "; bin size in ns" << "\n";
 ofs << "hdriftbinsz=2" << "\n";
 
 
 
 //Loop over each plane of HMS Drift Chambers (DC1 & DC2)

 for (int ip=0; ip<NPLANES; ip++){
   
   TString drift_time_histo = "hdc"+plane_names[ip]+"_time: t0_corr"; 

   //Get drift time histograms from root file
   h[ip] = (TH1F*)f->Get(drift_time_histo);

   //Get bin corresponding to t0 = 0 ns
  bin_t0[ip] = h[ip]->GetXaxis()->FindBin(0.0);
   
   //Get final bin 
   bin_final[ip] = bin_t0[ip] + TOTAL_BINS;
   
   
   
   //Find total BIN Content over entire integration range
   binContent_TOTAL[ip] = 0; //set counter to zero

   for (bin = bin_t0[ip]; bin <= bin_final[ip]; bin ++ ) {
     
     bin_Content[ip] = h[ip] -> GetBinContent(bin);
     
     binContent_TOTAL[ip] = bin_Content[ip] + binContent_TOTAL[ip];
     
     //   cout << "Bin: " << bin << endl;
     //   cout << "Content " << bin_Content[ip] << endl;
     //   cout << "Content SUM : " << binContent_TOTAL[ip] << endl;
   }
   
   TString headers = "hwc" + plane_names[ip] + "fract=";      
   ofs << headers;	  
   
   //Calculate LookUp Value
   
   binSUM[ip] = 0.0;
   int bin_count = 0;
   
   for (bin = bin_t0[ip]; bin <= bin_final[ip]; bin++) {
     
     bin_Content[ip] = h[ip] -> GetBinContent(bin);
     binSUM[ip] = binSUM[ip] + bin_Content[ip];
     
     
     lookup_value[ip] = binSUM[ip] / binContent_TOTAL[ip];
     bin_count = bin_count + 1;
     
     if (bin_count < = 8 ) {
       ofs << setprecision(5) << lookup_value[ip] << fixed << ",";
     }
     
     else if (bin_count >8 && bin_count < 138) {
       ofs << setprecision(5) << lookup_value[ip] << ((bin_count+1) % 10 ? "," : "\n") << fixed; 
     }
     else {
       ofs << setprecision(5) << lookup_value[ip] << fixed << endl;	  
     }
     
   }

 }                                            
 
}


	 
  






