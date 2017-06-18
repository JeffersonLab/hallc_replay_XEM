//Script to add necessary drift time histograms/plane from original root file to new root file


#define NPLANES 12
#define NBINS 400
#define bin_min -50.5
#define bin_max 349.5

void get_pdc_time_histo_tzero_corrected_v2()
{

  //Read Run Number from txt file
  int run_NUM;
  Long64_t num_evts;        //added
  string input_file;   //added


  TString f0 = "input_RUN.txt";
  ifstream infile(f0);
  infile >> input_file >> run_NUM >> num_evts;

  TString run = Form("run%d", run_NUM);

  //*****************READ WIRE TZERO VALUES*********************************
  
  
//this script reads all tzero values inside tzero_group.dat and assigns them to their corresponding planes. 
  //these values will then be used to shift the drift time histos, wire by wire (t0-correction wire-by-wire)

  int ip;  //to loop over planes
  int sw;  //to loop over sensewires


  int tot_wires[NPLANES] = {107, 107, 79, 79, 107, 107, 107, 107, 79, 79, 107, 107};

  //sum over all wires in both DC
  int wire_sum = 0;
  for (ip=0; ip<NPLANES; ip++) {
    wire_sum = wire_sum + tot_wires[ip];
  }
  

  //open and read tzero data file 
  ifstream file;
  file.open("../data_files/"+run+"/tzero_values_per_wire.dat");
  
  string line;
  int counter;
  Double_t value;
  Double_t *tzero_offset;
  tzero_offset = new Double_t[wire_sum];
  

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
      // cout << tzero[ip][sw] << endl;
      //cout <<  tzero[ip][sw] << " :: "<<tzero_offset[counter] << endl;
	   counter++;
    }
  }

  



  //*************************************************************************

  TString ext (".root"); //define a string to find in a file
  
  std::size_t found = input_file.find(ext); //find the pos of the string 
  if (found!=std::string::npos)
    //std::cout << " '.root' found at: " << found << '\n'; //found is the character position where ".root" is found
  
    input_file.erase (found,5);  //erase .root                      
    // std::cout << input_file << '\n';

  TString file_name = "../../../ROOTfiles/"+input_file+"_dc_uncal.root";
  
  //open file
  TFile *f = new TFile(file_name, "READ");

  //create new file
  TFile *g = new TFile(Form("../root_files/"+run+"/shms_tzero_corr_histo.root", run_NUM), "RECREATE"); // create new file to store histo

  f->cd();

  //Get the tree
  TTree *tree = (TTree*)f->Get("T");
  TString SPECTROMETER="P";
  TString DETECTOR="dc";
  TString plane_names[NPLANES]={"1u1", "1u2", "1x1", "1x2", "1v1", "1v2", "2v2", "2v1", "2x2", "2x1", "2u2", "2u1"};
  
  //Declare Variables to Loop Over
  Int_t Ndata_time[NPLANES];
  Int_t Ndata_wirenum[NPLANES];
  Double_t pdc_wirenum[NPLANES][107];
  Double_t pdc_time[NPLANES][1000];
  

  //Declare Histogram array to store AVG drift times per plane
  TH1F* h[NPLANES];
	
  g->cd();
	
  //Loop over each plane
  for(Int_t ip=0; ip<NPLANES; ip++){
    TString base_name = SPECTROMETER+"."+DETECTOR+"."+plane_names[ip];
    TString ndata_time = "Ndata."+base_name+".time";
    TString ndata_wirenum = "Ndata."+base_name+".wirenum";
    TString drift_time = base_name+".time";
    TString wirenum_hit = base_name+".wirenum";

    TString drift_time_histo = "pdc"+plane_names[ip]+"_time"; 
    TString title = "pdc"+plane_names[ip]+"_drifttime";
     
    //Set Branch Address
    tree->SetBranchAddress(drift_time, pdc_time[ip]);   
    tree->SetBranchAddress(ndata_time, &Ndata_time[ip]);                // Ndata represents number of triggers vs number of hits that each trigger produced.
    tree->SetBranchAddress(ndata_wirenum, &Ndata_wirenum[ip]);      // Ndata represents number of triggers vs number of hits that each trigger produced.
    tree->SetBranchAddress(wirenum_hit, pdc_wirenum[ip]);	    // A hit is refer to as when a trigger(traversing particle), ionizes the WC gas and ionized
						                    //electrons reach the rearest sense wire, producing a detectable signal in the O'scope 
  
  
    //Create Histograms
    h[ip] = new TH1F(drift_time_histo, title, NBINS, bin_min, bin_max);  //set time to 400 ns/200 bins = 2ns/bin
    h[ip]->GetXaxis()->SetTitle("Drift Time (ns)");
    h[ip]->GetYaxis()->SetTitle("Number of Entries / 1 ns");
  }
 

  //declare some variables
  int wirenum;
  double shift;

  //Declare number of entries in the tree
  Long64_t nentries = tree->GetEntries(); //number of triggers (particles that passed through all 4 hodo planes)
   

  //Loop over all entries
  for(Long64_t i=0; i<num_evts; i++)
    {
      tree->GetEntry(i);
      //cout << "****event:**** " << i << endl;
          //Loop over number of hits for each trigger in each DC plane 
      for(Int_t ip=0; ip<NPLANES; ip++){       
	//cout << "Plane: " << plane_names[ip] << endl;
	for(Int_t j=0, k=0; j<Ndata_time[ip] && k<Ndata_wirenum[ip]; j++, k++){
	  
	  wirenum = int(pdc_wirenum[ip][k]); //get the wirenumber hit
	  
	  shift = tzero[ip][wirenum-1];  //get the tzero corresponding to wire hit
	  
	  // cout << plane_names[ip] << " :: " << wirenum << " :: " << shift << endl;

	  h[ip]->Fill(pdc_time[ip][j] - shift);  //shidt the plane drift time event-by-event
	
   
	  // cout << "time: " << pdc_time[ip][k] << endl;
	 
	}
       
      }

    }
 
  //Write histograms to file
  
  g->Write();
  
}
