//Script to add t0 correction to SHMS DC drift times

#define NPLANES 12

void get_pdc_time_histo_tzero_corrected()
{

	  
  //read run number from input file
  int run_NUM;
  TString f0 = "input_RUN.txt";
  ifstream infile(f0);
  infile >> run_NUM;   

  TString run = Form("run%d", run_NUM);


	  //open file
	  TFile *f = new TFile(Form("../../../ROOTfiles/shms_replay_%d.root", run_NUM), "READ");

	  //updates file
	  TFile *g = new TFile(Form("../root_files/run%d/shms_dc_t0_corrected_%d.root", run_NUM, run_NUM), "UPDATE"); // create new file to store histo

      f->cd();

     //Get the tree
     TTree *tree = (TTree*)f->Get("T");

	TString SPECTROMETER="P";
	TString DETECTOR="dc";
	TString plane_names[NPLANES]={"1u1", "1u2", "1x1", "1x2", "1v1", "1v2", "2v2", "2v1", "2x2", "2x1", "2u2", "2u1"};

    //Declare Variables to Loop Over
    Int_t Ndata[NPLANES];
    Double_t pdc_time[NPLANES][1000];
    
    //Declare Histogram array to store AVG drift times per plane
    TH1F* h[NPLANES];
	
	g->cd();
	
	//Loop over each plane
	for(Int_t ip=0; ip<NPLANES; ip++){
		TString base_name = SPECTROMETER+"."+DETECTOR+"."+plane_names[ip];
		TString ndata_name = "Ndata."+base_name+".time";
		TString drift_time = base_name+".time";

		TString drift_time_histo = "pdc"+plane_names[ip]+"_time: t0_corr"; 
        TString title = "pdc"+plane_names[ip]+"_drifttime: t0-corrected";
     
     //Set Branch Address
     tree->SetBranchAddress(drift_time, pdc_time[ip]);   
     tree->SetBranchAddress(ndata_name, &Ndata[ip]);  /* Ndata represents number of triggers vs number of hits that each trigger produced.
                                                      A hit is refer to as when a trigger(traversing particle), ionizes the WC gas and ionized
                                                      electrons reach the rearest sense wire, producing a detectable signal in the O'scope */
	
     //Create Histograms
     h[ip] = new TH1F(drift_time_histo, title, 200, -50, 350);  //set time to 400 ns/200 bins = 2ns/bin
}
	
	
	//open and read tzero data file
    ifstream ifs;
    ifs.open("../data_files/" + run + "/tzero.dat");
    
	double t_zero_offsets[NPLANES];

     for (ip=0; ip < 12; ip++) {	 
	 ifs >> t_zero_offsets[ip];  //add tzero offsets to array
   }
   
    //Declare number of entries in the tree
    Long64_t nentries = tree->GetEntries(); //number of triggers (particles that passed through all 4 hodo planes)

    //Loop over all entries
    for(Long64_t i=0; i<nentries; i++)
    {
	tree->GetEntry(i);
    
    
    //Loop over number of hits for each trigger in each DC plane 
    for(ip=0; ip<NPLANES; ip++){
   
 
    
    for(Int_t j=0; j<Ndata[ip]; j++){
	
	h[ip]->Fill(pdc_time[ip][j] - t_zero_offsets[ip]); //add t0 offset correction 
       }
      
      

			
	
	}

}



		
//Write histograms to file
g->Write();



}
