

/*Script to extract reference time "t0"  for each sense wire in a given HMS Wire Chamber Plane with COSMIC RUNS.  
20% (MAX BIN CONTENT) is calculated per wire, and the corresponding bin is fitted linearly about +/- 
a certain number of bins and this fit is extrapolated to y=0(x-axis). The extrapolated value is take to be t0*/

#include <vector>
#include <TMath>

#define NPLANES 12

void get_wire_tzero()
{
  using namespace std;

  int run_NUM;
  TString f0 = "input_RUN.txt";
  ifstream infile(f0);
  infile >> run_NUM;   

  //check if tzero_weighted_avg text file exists (if it does, DELETE IT, otherwise new values will be appended to it, in addition to pre-existing tzero values)
  std::ifstream stream(Form("../data_files/run%d/tzero_weighted_avg_run%d.txt",run_NUM, run_NUM));
 if (stream.good())
  {
  gSystem->Exec(Form("rm ../data_files/run%d/tzero_weighted_avg_run%d.txt",run_NUM, run_NUM));
  }

 TString run = Form("run%d", run_NUM);

 //Declare plane names to loop over
 TString plane_names[NPLANES]={"1x1", "1y1", "1u1", "1v1", "1y2", "1x2", "2x1", "2y1", "2u1", "2v1", "2y2", "2x2"};

 //Declare a root file array to store individual DC cell drift times
 TString root_file;
 TFile *f[NPLANES];
   
 int total_wires;  //integer to store total sense wires for a plane chosen by the user
        
 //Loop over all planes
 for (int ip = 0; ip < NPLANES; ip++){

   //READ root file
   root_file = "../root_files/"+run+"/hms_DC_"+plane_names[ip]+Form("_%d.root",run_NUM);
   f[ip] = new TFile(root_file, "READ");

   //Create a file output file stream object to write t0 values to data file
   ofstream ofs;
   TString t_zero_file = "../data_files/" + run + "/hdc_"+plane_names[ip]+Form("tzero_run%d.dat", run_NUM);
   ofs.open (t_zero_file);

   //Set headers for subsequent columns of data
   ofs << "#WIRE " << "   "  << "t0" << "   " << "t0_err" << "   " << " entries " << endl;

   //Create root file to store fitted wire drift times histos and "t0 vs. wirenum"
   TString output_root_file = "../root_files/"+run+"/hmsDC_"+plane_names[ip]+Form("run%d_fitted_histos.root", run_NUM);
   TFile *g = new TFile(output_root_file,"RECREATE");

   f[ip]->cd();  //change to file containing the wire drift times histos
 
   int total_wires;  //integer to store total sense wires for a plane chosen by the user
   
   //Set variables depending on which plane is being studied
   if (ip == 0 || ip == 5 || ip == 6 || ip == 11) {  
     TH1F *cell_dt[113]; //declare array of histos to store drift times     
     total_wires=113; 

     //Declare bin properties for given sense wires in a plane

     int bin_max[113];                    /*Array to store the bin number corresponding to the drift time distribution peak*/
     int bin_maxContent[113];             /*Array to store the content (# events) corresponding to the bin with maximum content*/
     double time_max[113];                /*Array to store the x-axis(drift time (ns)) corresponding to bin_max*/
     double twenty_perc_maxContent[113];  /*Array to store 20% of maximum bin content (peak)*/						     
     double ref_time[113];               /*Array to store reference times for each sense wire*/

   }

   else if (ip == 2 || ip == 3 || ip == 8 || ip == 9) {
     TH1F *cell_dt[107];
     total_wires=107;      
   
     int bin_max[107];                                 
     int bin_maxContent[107];                           
     double time_max[107];                               
     double twenty_perc_maxContent[107];                
     double ref_time[107];          

   }	   
 
   else if (ip == 1 || ip == 4 || ip == 7 || ip == 10) {
     TH1F *cell_dt[52];
     total_wires=52;

     int bin_max[52];                                 
     int bin_maxContent[52];                           
     double time_max[52];                               
     double twenty_perc_maxContent[52];                
     double ref_time[52];          

   }	 
    
 	
   /*Get wire histos from root file and loop over each 
     sense wire of a plane in HMS Drift Chambers (DC1 or DC2)*/
 
   for (int sensewire=1; sensewire<=total_wires; sensewire++){

     //Get title of histos in root file
     TString drift_time_histo = Form("wire_%d", sensewire); 
 
     //Get drift time histograms from root file
     cell_dt[sensewire-1] = (TH1F*)f[ip]->Get(drift_time_histo);

     
     //Get bin with Maximum Content
     bin_max[sensewire-1] = cell_dt[sensewire-1]->GetMaximumBin();

     //Get content of bin_max
     bin_maxContent[sensewire-1] = cell_dt[sensewire-1]->GetBinContent(bin_max[sensewire-1]);
     
     //Get time (ns) [x-axis] corresponding to bin_max 
     time_max[sensewire-1] = cell_dt[sensewire-1]->GetXaxis()->GetBinCenter(bin_max[sensewire-1]);
     
     //Calculate 20% of max content
     twenty_perc_maxContent[sensewire-1] = bin_maxContent[sensewire-1] * 0.20;
     

   }
   
   
   
   //****************************************************//
   //Determine  which bin has around 20% max_BinContent *//
   //****************************************************//
   
   
   //Declarations
   int content_bin;      //stores content for each bin
   int counts;           //a counter used to count the number of bins that have >20% max bin content for a plane 
   int bin;              //store bin number
   int j;                //jth bin, used to loop over n bins 
   
   //Declare vector arrays 
   vector<int> content;               //stores bin content
   vector <int> bin_num;           //stores bin number
   
   
   //Loop over each wire 
   for(sensewire=1; sensewire<=total_wires; sensewire++) {
     
     //Loop over each bin for individual wire drift time histo
     for(bin=0; bin < bin_max[sensewire-1]; bin++) {
       
       content_bin = cell_dt[sensewire-1]->GetBinContent(bin);              //get bin content for all bins in a wire
       
       content.push_back(content_bin);                                      //add bin content to array
       bin_num.push_back(bin);                                              //add bin number to array
       
       
       // check if 2 bin contents have been stored and examine if these contents exceed or not 20% of peak
       if (content.size() == 2) {
	 
	 //initialize counter to count how many bin contents >= 20%
	 counts = 0;
	 
	 // Loop over 2 bin contents stored in array content
	 for (j=0; j<2; j++){
	   
	   if(content[j] > =  twenty_perc_maxContent[sensewire-1]){
	     counts = counts+1;
             
	     if(counts >= 2) { goto stop;}
	     
	     
	   }
	   
	   content.clear();
	   bin_num.clear();
	   
	 }
	 
       }
     }
     
     //Print the time(ns) and BIN NUM corresponding to 20% of MAX content 
     //if 2/2 elements exceeds 20% of Max content (for each plane)
     
   stop:
     ref_time[sensewire-1] = cell_dt[sensewire-1] ->GetXaxis() -> GetBinCenter(bin_num[0]); //Get time corresponding ~20% Max BIN CONTENT  
     
     //cout << " ******* " << "Wire " << sensewire << " ******* " << endl;
     //cout << "time (20% of Max BIN): " << ref_time[sensewire-1] << " ns" << endl;
     //cout << "BIN: " << bin_num[0] << endl;
     
     
     //*********************************************************//
     //*******Extract the "t0" Using a Fitting Procedure********//
     //*********************************************************//
     
     //Declarations
     int time_init;           //start fit value 
     int time_final;          //end fit value
     int t_zero;
     int entries;             //entries for each wire
     
     double m;                //slope
     double y_int;            //y-intercept
     double m_err;
     double y_int_err;
     double t_zero_err;
     
     //Get time corresponding to bin (fit range) 
     time_init = cell_dt[sensewire-1] -> GetXaxis() -> GetBinCenter(bin_num[0]-5); //choose bin range over which to fit
     time_final = cell_dt[sensewire-1] -> GetXaxis() -> GetBinCenter(bin_num[0]+5); 
     
     //Create Fit Function
     TF1* tZero_fit = new TF1("tZero_fit", "[0]*x + [1]", time_init, time_final);
     
     //Set Parameter Names and Values
     tZero_fit->SetParName(0, "slope");
     tZero_fit->SetParName(1, "y-int");
     tZero_fit->SetParameter(0, 1.0);
     tZero_fit->SetParameter(1, 1.0);
     
     //Fit Function in specified range
     cell_dt[sensewire-1]->Fit("tZero_fit", "QR");
     
     //Get Parameters and their errors
     m = tZero_fit->GetParameter(0);
     y_int = tZero_fit->GetParameter(1);
     m_err = tZero_fit->GetParError(0);
     y_int_err = tZero_fit->GetParError(1);
     
     //Calculate error on t0 using error propagation method of expanding partial derivatives
     t_zero = - y_int/m;
     t_zero_err = sqrt(y_int_err*y_int_err/(m*m) + y_int*y_int*m_err*m_err/(m*m*m*m) );
     entries = cell_dt[sensewire-1]->GetEntries();  //number of entries (triggers) per wire
     
     //Write "t0" values to file
     ofs << sensewire << "          " << t_zero << "          " << t_zero_err << "          " << entries << endl;
     
     //Change to output root file and write fitted histos to file
     g->cd();
     cell_dt[sensewire-1]->Write();
     
   }
   
   // Make Plot of t0 versus Wire Number 
   
   TCanvas *t = new TCanvas("t", "", 2000,500);
   t->SetGrid();

   
   TGraphErrors *graph = new TGraphErrors(t_zero_file, "%lg %lg %lg");
   graph->SetName("graph");
   TString title = "DC"+plane_names[ip]+": t0 versus sensewire";
   graph->SetTitle(title);
   graph->SetMarkerStyle(20);
   graph->SetMarkerColor(1);
   graph->GetXaxis()->SetLimits(0., total_wires);
   graph->GetXaxis()->SetTitle("Wire Number");
   graph->GetXaxis()->CenterTitle();
   graph->GetYaxis()->SetTitle("t-Zero (ns)");
   graph->GetYaxis()->CenterTitle();
   graph->GetYaxis()->SetRangeUser(-50.0, 50.0);
   graph->Draw("AP");
   t->Update();
   t->Write(title);   //write to a root file
   
   //close dat file
   ofs.close();
   //save plots
   //TString tzero_plots = "plots/"+run_NUM +"/hdc"+plane_names[ip]+Form("TESTING_tzero_v_wire_%d.eps", run);
   //t->SaveAs(tzero_plots);
   
   
   //*****************************************************************************************//
   //        CALCULATE THE "t0s" WEIGHTED AVERAGE FOR WIRE DRIFT TIMES WITH ENTRIES > = 300   //
   //*****************************************************************************************//
   
   
   //open t0 dat file
   ifstream ifs;
   ifs.open (t_zero_file);
   string line;
   
   //open new data file to write updated t0 values
   TString t_zero_file_corr = "../data_files/" + run + "/hdc_"+plane_names[ip]+Form("tzero_run%d_updated.txt", run_NUM);
   ofs.open(t_zero_file_corr);
   ofs << " #Wire " << "     " << " t_zero " << "     " << " t_zero_err " << "     " << " entries " << endl; 
   
   //Initialize variables related to weighted avg
   double sum_NUM;  //numerator of weighted avg
   double sum_DEN;   //denominator of weighted avg
   double weighted_AVG;
   double weighted_AVG_err; 
  
   //set them to zero to start sum inside while loop 
   sum_NUM = 0.0;
   sum_DEN = 0.0;
   
   weighted_AVG;
   weighted_AVG_err; 
   
   //read line bt line the t_zero_file
   while(getline(ifs, line)) {
     if(!line.length()|| line[0] == '#')
       continue;
     //	sensewire = 0, t_zero = 0.0, t_zero_err = 0.0, entries = 0 ; //set values to zero
     
     sscanf(line.c_str(), "%d %d %lf %d", &sensewire, &t_zero, &t_zero_err, &entries); //assign each of the variables above a data in the t_zero_file
     
     //Check if entries for each sensewire exceeds a certain number of events
     
     if (entries>300 && t_zero < 30) {
	
       //Calculate the weighted average of t0s
       sum_NUM = sum_NUM + t_zero/(t_zero_err*t_zero_err);
       sum_DEN = sum_DEN + 1.0/(t_zero_err*t_zero_err);      
       
       //cout << "sum_NUM : " << sum_NUM << endl;  
       //cout << "sum_DEN : " << sum_DEN << endl;  
       
    


       ofs << sensewire << "        " << t_zero << "        " << t_zero_err << "        " << entries << endl;

       
       
     }
     
   }
   
   
   
   weighted_AVG = sum_NUM / sum_DEN;
   weighted_AVG_err = sqrt( 1.0 / sum_DEN );
   
   
   
   //open new data file to write weighted average of updated t_zero values
   
   TString t_zero_AVG = Form("../data_files/run%d/tzero_weighted_avg_run%d.txt", run_NUM, run_NUM);
   
   ofstream ofile;
   ofile.open(t_zero_AVG, std::ofstream::out | std::ofstream::app); //open file in and output and append mode
   
   ofile << " #weighted_AVG " << "     " << " DC plane: " <<  plane_names[ip] << endl; 
   ofile << weighted_AVG << endl;
   
  
   
   
   
   ifs.close();

   // Make Plot of t0 versus Wire Number for entries > 300 events

   TCanvas *t1 = new TCanvas("t1", "", 2000,500);
   t1->SetGrid();

   //TString mygraph = "hdc"+plane_names[ip]+Form("_t_zero_run%d.txt", run);
   TGraphErrors *graph1 = new TGraphErrors(t_zero_file_corr, "%lg %lg %lg");
   graph1->SetName("graph1");
   TString title1 = "hdc"+plane_names[ip]+": t0 versus sensewire_corrected";
   graph1->SetTitle(title1);
   graph1->SetMarkerStyle(20);
   graph1->SetMarkerColor(1);
   //graph1->GetXaxis()->SetLimits(0., total_wires);
   graph1->GetXaxis()->SetTitle("Wire Number");
   graph1->GetXaxis()->CenterTitle();
   graph1->GetYaxis()->SetTitle("t-Zero (ns)");
   graph1->GetYaxis()->CenterTitle();
   graph1->GetYaxis()->SetRangeUser(-50.0, 50.0);
   graph1->Draw("AP");
   t1->Update();

   // Draw TLine
   TLine *wght_avg = new TLine(t1->GetUxmin(), weighted_AVG, t1->GetUxmax(), weighted_AVG);
   wght_avg->SetLineColor(kRed);
   wght_avg->SetLineWidth(2);
   wght_avg->SetLineStyle(2);
   wght_avg->Draw();
   
   //Add text to canvas
   TLatex* ltx1 =  new TLatex();
   ltx1->DrawLatex(t1->GetUxmax()*0.75,40, Form("Weighted Average = %lf #pm %lf ns", weighted_AVG, weighted_AVG_err) );
   
   t1->Write(title1);   //write canvas to a root file
   
   ofs.close();  //close data file

   








 }
 
 




}
