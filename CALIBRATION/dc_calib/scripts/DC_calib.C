//SHMS DC Calibration: Implementation
#include<iostream>
#include "DC_calib.h"

using namespace std;

//_____________________________________________________________
DC_calib::DC_calib(string a, TString b, const int c, Long64_t d, TString e, string f)

  :spec(a),          //set spectrometer to 'HMS', or 'SHMS'  ex. DC_Calib(HMS, pdc_replay.C, 488, 50000)
   ifile_name(b),    //initialization list
   run_NUM(c),
   num_evts(d),
   pid(e),
   mode(f)
{
  //Initialize pointers
  dir_log    = NULL;
  tree       = NULL;
  in_file    = NULL;
  out_file   = NULL;
  main_dir   = NULL;
  plane_dt   = NULL;
  plane_dt_corr = NULL;
  cell_dt    = NULL;
  cell_dt_corr    = NULL;
  fitted_cell_dt = NULL;
  dt_vs_wire = NULL;
  dt_vs_wire_corr = NULL;
  offset        = NULL;

  entries                = NULL;
  t_zero                 = NULL;
  t_zero_err             = NULL;
  t_zero_final           = NULL;
  bin_max                = NULL;
  bin_maxContent         = NULL;
  time_max               = NULL;
  twenty_perc_maxContent = NULL;
  ref_time               = NULL;
  tZero_fit              = NULL;
  graph                  = NULL;
  gr1_canv               = NULL;

  if(mode=="card")
    {
  

  //--Card Variables
      entries_card           = NULL;
      t_zero_card            = NULL;
      t_zero_card_err        = NULL;
      card_hist              = NULL;
      corr_card_hist         = NULL;
      fitted_card_hist       = NULL;
      wire_min               = NULL;
      wire_max               = NULL;
      wireBinContentMax      = NULL;
      wireBinContentLow      = NULL;
      wireBinContentHigh     = NULL;
      wireBinHigh            = NULL;
      wireBinLow             = NULL;
      wireFitRangeLow        = NULL;
      wireFitRangeHigh       = NULL;

    }
  
}


//____________________________________________________________
DC_calib::~DC_calib()
{
  cout << "calling the destructor " << endl;  
  delete dir_log;  dir_log = NULL;
  delete in_file;  in_file  = NULL;
  delete out_file; out_file = NULL;             
  delete graph;    graph    = NULL;
  delete gr1_canv; gr1_canv = NULL;

  
  
  //Delete 1D Arrays pointers to free up 'heap' space
    
    delete [] dt_vs_wire; dt_vs_wire = NULL;
    delete [] dt_vs_wire_corr; dt_vs_wire_corr = NULL;
    delete [] plane_dt; plane_dt     = NULL;
    delete [] plane_dt_corr; plane_dt_corr = NULL;


    //free 2d dynamic array cell_dt[][]
    for(int ip = 0; ip<NPLANES; ip++) 
      { 
	delete [] entries[ip];
	delete [] t_zero[ip];
	delete [] t_zero_err[ip];
	delete [] t_zero_final[ip];
	delete [] cell_dt[ip]; 
	delete [] cell_dt_corr[ip]; 
	delete [] fitted_cell_dt[ip];
	delete [] bin_max[ip]; 
	delete [] bin_maxContent[ip]; 
	delete [] time_max[ip]; 
	delete [] twenty_perc_maxContent[ip]; 
	delete [] ref_time[ip];
	delete [] offset[ip];
     
	
	  
	if (mode=="card")
	  {
	    //Card Variables
	    delete [] entries_card[ip];        
	    delete [] t_zero_card[ip];
	    delete [] t_zero_card_err[ip];
	    delete [] card_hist[ip]; 
	    delete [] corr_card_hist[ip];
	    delete [] fitted_card_hist[ip];
	    delete [] wire_min[ip];
	    delete [] wire_max[ip];
	    delete [] wireBinContentMax[ip];
	    delete [] wireBinContentLow[ip];
	    delete [] wireBinContentHigh[ip];
	    delete [] wireBinHigh[ip];
	    delete [] wireBinLow[ip];
	    delete [] wireFitRangeHigh[ip];
	    delete [] wireFitRangeLow[ip];
	  }
	  
      }  
    
    delete [] entries;                     entries                = NULL;
    delete [] t_zero;                      t_zero                 = NULL;
    delete [] t_zero_err;                  t_zero_err             = NULL;
    delete [] t_zero_final;                t_zero_final           = NULL;
    delete [] cell_dt;                     cell_dt                = NULL;
    delete [] cell_dt_corr;                cell_dt_corr                = NULL;
    delete [] fitted_cell_dt;              fitted_cell_dt         = NULL;
    delete [] bin_max;                     bin_max                = NULL;
    delete [] bin_maxContent;              bin_maxContent         = NULL;
    delete [] time_max;                    time_max               = NULL;
    delete [] twenty_perc_maxContent;      twenty_perc_maxContent = NULL;
    delete [] ref_time;                    ref_time               = NULL;
    delete [] offset;                      offset                 = NULL;
    
   
    if (mode=="card")
      {
	//Card Variables 
	delete [] entries_card;                entries_card           = NULL;
	delete [] t_zero_card;                 t_zero_card            = NULL;
	delete [] t_zero_card_err;             t_zero_card_err        = NULL;
	delete [] card_hist;                   card_hist              = NULL;
	delete [] corr_card_hist;              corr_card_hist         = NULL;
	delete [] fitted_card_hist;            fitted_card_hist       = NULL;
	delete [] wire_min;                    wire_min               = NULL;
	delete [] wire_max;                    wire_max               = NULL;
	delete [] wireBinContentMax;           wireBinContentMax      = NULL;
	delete [] wireBinContentLow;           wireBinContentLow      = NULL;
	delete [] wireBinContentHigh;          wireBinContentHigh     = NULL;
	delete [] wireBinHigh;                 wireBinHigh            = NULL;
	delete [] wireBinLow;                  wireBinLow             = NULL;
	delete [] wireFitRangeLow;             wireFitRangeLow        = NULL;
	delete [] wireFitRangeHigh;            wireFitRangeHigh       = NULL;
      }
}


//____________________________________________________________
void DC_calib::setup_Directory()
{

  
  if (spec == "HMS")
    {
     
      dir_log = Form("mkdir -p ./%s_DC_%sLog_%d/", spec.c_str(), mode.c_str(), run_NUM);

      //Check if directory exists
      if (system(dir_log) != 0) 
	{
	  cout << "Creating Directory to store HMS Calibration Results . . ." << endl; 
	  system(dir_log);  //create directory to log calibration results
	}


    }

  else if (spec == "SHMS")
    {
      
      dir_log = Form("mkdir -p ./%s_DC_%sLog_%d/", spec.c_str(), mode.c_str(), run_NUM);

      //Check if directory exists
      if (system(dir_log) != 0) 
	{
	  cout << "Creating Directory to store SHMS Calibration Results . . ." << endl; 
	  system(dir_log);  //create directory to log calibration results
	}
      
      
    }

}




//___________________________________________________________
void DC_calib::SetPlaneNames()
{
  
  DETECTOR = "dc";
 
  //initialize DC plane names
  if(spec=="SHMS")
    {
      //--per wire ONLY
      percent = 0.40;
      t0_err_thrs = 15;  
      max_wire_entry = 1000;

      tdc_offset = 0.0;
      SPECTROMETER = "P";
      spectre = "p";
   
      planes[0] = plane_names[0]="1u1",  nwires[0] = 107; 
      planes[1] = plane_names[1]="1u2",  nwires[1] = 107; 
      planes[2] = plane_names[2]="1x1",  nwires[2] = 79;
      planes[3] = plane_names[3]="1x2",  nwires[3] = 79; 
      planes[4] = plane_names[4]="1v1",  nwires[4] = 107; 
      planes[5] = plane_names[5]="1v2",  nwires[5] = 107; 
      planes[6] = plane_names[6]="2v2",  nwires[6] = 107;
      planes[7] = plane_names[7]="2v1",  nwires[7] = 107;
      planes[8] = plane_names[8]="2x2",  nwires[8] = 79;
      planes[9] = plane_names[9]="2x1",  nwires[9] = 79;
      planes[10] = plane_names[10]="2u2", nwires[10] = 107;
      planes[11] = plane_names[11]="2u1", nwires[11] = 107;

      if (mode=="card")
	{
	  //cout << "Setting Plane Cards . . ." << endl;
      //---Per Card ONLY----
      plane_cards[0] = 7;
      plane_cards[1] = 7;
      plane_cards[2] = 5;
      plane_cards[3] = 5;
      plane_cards[4] = 7;
      plane_cards[5] = 7;
      plane_cards[6] = 7;
      plane_cards[7] = 7;
      plane_cards[8] = 5;
      plane_cards[9] = 5;
      plane_cards[10] = 7;
      plane_cards[11] = 7;
	}

    }
      
  
  else if(spec=="HMS")
    {
      
      //per wire ONLY
      percent = 0.20;  ///set 20% of max drit time content to fit around
      t0_err_thrs = 15;
      max_wire_entry = 1000;

      tdc_offset = 115.;  
      SPECTROMETER = "H";
      spectre="h";
      
      planes[0] = plane_names[0]="1u1",  nwires[0] = 96;  
      planes[1] = plane_names[1]="1u2",  nwires[1] = 96;
      planes[2] = plane_names[2]="1x1",  nwires[2] = 102;
      planes[3] = plane_names[3]="1x2",  nwires[3] = 102; 
      planes[4] = plane_names[4]="1v2",  nwires[4] = 96;
      planes[5] = plane_names[5]="1v1",  nwires[5] = 96;
      planes[6] = plane_names[6]="2v1",  nwires[6] = 96;
      planes[7] = plane_names[7]="2v2",  nwires[7] = 96;
      planes[8] = plane_names[8]="2x2",  nwires[8] = 102;
      planes[9] = plane_names[9]="2x1",  nwires[9] = 102;
      planes[10] = plane_names[10]="2u2", nwires[10] = 96;
      planes[11] = plane_names[11]="2u1", nwires[11] = 96;

      if (mode=="card")
	{
      //---Per-Card ONLY---
      plane_cards[0] = 6;
      plane_cards[1] = 6;
      plane_cards[2] = 7;
      plane_cards[3] = 7;
      plane_cards[4] = 6;
      plane_cards[5] = 6;
      plane_cards[6] = 6;
      plane_cards[7] = 6;
      plane_cards[8] = 7;
      plane_cards[9] = 7;
      plane_cards[10] = 6;
      plane_cards[11] = 6;
	}
    }
  
}

//____________________________________________________________________________________
void DC_calib::SetTdcOffset()
{
  //cout << "Setting TDC offsets for the HMS " << endl;
  
  for (int ip = 0; ip < NPLANES; ip ++)
    {
      for (wire = 1; wire <= nwires[ip]; wire++)
	{
	  
	  if (ip== 0 && wire > 80)          {offset[ip][wire-1] = tdc_offset;}
	  else if (ip== 6 && wire > 80)     {offset[ip][wire-1] = tdc_offset;}
	  else if (ip == 0 && wire <= 80)   {offset[ip][wire-1] = 0.0;}
	  else if (ip == 6 && wire <= 80)   {offset[ip][wire-1] = 0.0;}
	  else if ((ip == 4 || ip == 10) && wire > 48 && wire <65) {offset[ip][wire-1] = tdc_offset;}
	  else if (ip == 4 && (wire <=48 || wire >= 65)) {offset[ip][wire-1] = 0.0;}
	  else if (ip == 10 && (wire <= 48 || wire >= 65)) {offset[ip][wire-1] = 0.0;}
	  else if (ip!=4 || ip!=0 || ip!=6 || ip!=10) {offset[ip][wire-1] = 0.0;}


	}
      
    }
  
  

}

//____________________________________________________________________________________

void DC_calib::GetDCLeafs()
{
  
 
  //open input root file
  in_file = new TFile(ifile_name, "READ" );
  
  //Get the tree
  tree = (TTree*)in_file->Get("T");
  
  Long64_t nentries = tree->GetEntries();

  if (num_evts > nentries)
    {
      cout << "Number of entries entered: " << num_evts << " exeeds MAX number of entries: " << nentries << endl;
      cout << "Setting the number of entries to:  " << nentries  <<  endl;
      
      num_evts = nentries;
    }
  else if (num_evts==-1)
    {
      cout << "Analyzing ALL entries: " << nentries << " entries " << endl;
      num_evts = nentries;
    }


  //Loop over each plane
  for (int ip = 0; ip < NPLANES; ip++)
    {
      
      base_name = SPECTROMETER+"."+DETECTOR+"."+plane_names[ip];
      ndatatime = "Ndata."+base_name+".time";
      drifttime = base_name+".time";
      wirenum = base_name+".wirenum";
      
      //Set Branch Address 
      tree->SetBranchAddress(wirenum, wire_num[ip]);   
      tree->SetBranchAddress(drifttime, drift_time[ip]);   
      tree->SetBranchAddress(ndatatime, &ndata_time[ip]);   
						     
    }
  
  
  if (spec=="SHMS")
    {
      cal_etot_leaf = "P.cal.etot";
      cer_npe_leaf = "P.ngcer.npeSum";  
    
      //Check Branch Status 
      status_cal = tree->GetBranchStatus(cal_etot_leaf);  //returns a boolean
      status_cer = tree->GetBranchStatus(cer_npe_leaf);  //return a boolean
      
      
      if ((!status_cal || !status_cer )&& (pid=="pid_elec"))
	{
	  cout << "*************ATTENTION!**************" << endl;
	  cout << "" << endl;
	  cout << " One or more of the following leafs " << endl;
	  cout << " is *NOT* present in current ROOTfile. " << endl;
	  cout << "1) " << cal_etot_leaf<< endl;
	  cout << "2) " << cer_npe_leaf << endl;
	  cout << "" << endl;
	  cout << "Please add them if you want to make " << endl;
	  cout << "any cuts during calibration." << endl;
	  cout << "" << endl;
	  cout << "OR, set the pid flag in main_calib.C " << endl;
	  cout << "to pid_kFALSE" << endl;
	  cout << "             Exiting NOW!          " << endl;
	  cout << "*************************************" << endl;

	  exit (EXIT_SUCCESS);
	}
    
      else
	{
	  tree->SetBranchAddress(cal_etot_leaf, &cal_etot);
	  tree->SetBranchAddress(cer_npe_leaf, &cer_npe);   
	}
	
    }

  else if (spec=="HMS")
    {
      cal_etot_leaf = "H.cal.etot";
      cer_npe_leaf = "H.cer.npeSum";  
     

      //Check Branch Status with Boolean
      status_cal = tree->GetBranchStatus(cal_etot_leaf);
      status_cer = tree->GetBranchStatus(cer_npe_leaf); 

      if ((!status_cal || !status_cer ) && (pid=="pid_elec"))
	{
	  cout << "*************ATTENTION!**************" << endl;
	  cout << "" << endl;
	  cout << " One or more of the following leafs " << endl;
	  cout << " is *NOT* present in current ROOTfile. " << endl;
	  cout << "1) " << cal_etot_leaf<< endl;
	  cout << "2) " << cer_npe_leaf << endl;
	  cout << "" << endl;
	  cout << "Please add them if you want to make " << endl;
	  cout << "any cuts during calibration." << endl;
	  cout << "" << endl;
	  cout << "OR, set the pid flag in main_calib.C "<< endl;
	  cout << "to: pid_kFALSE" << endl;
	  cout << "             Exiting NOW!          " << endl;
	  cout << "*************************************" << endl;
	  exit (EXIT_SUCCESS);
	}

      else
	{
	  tree->SetBranchAddress(cal_etot_leaf, &cal_etot);
	  tree->SetBranchAddress(cer_npe_leaf, &cer_npe);   
	}
 
      
    }


}


//_______________________________________________________________________
void DC_calib::AllocateDynamicArrays()
{
 

  dir_log = new char();
  
  //Allocate 1D dynamic arrays
  plane_dt      = new TH1F[NPLANES];       //create plane drift time histo 1Darray ( get_pdc_time_histo.C )
  plane_dt_corr = new TH1F[NPLANES];      //create plane drift times to store after applying tzero correction
  dt_vs_wire    = new TH2F[NPLANES];     //create wire drift time and 'drifttime vs wire' arrays
  dt_vs_wire_corr = new TH2F[NPLANES];

  //Allocate 2D dynamic arrays
  entries                 = new Int_t*[NPLANES];
  t_zero                  = new Double_t*[NPLANES];
  t_zero_err              = new Double_t*[NPLANES];
  t_zero_final            = new Double_t*[NPLANES];
  cell_dt                 = new TH1F*[NPLANES];     /*create array to store cell drift times*/    
  cell_dt_corr            = new TH1F*[NPLANES];
  fitted_cell_dt          = new TH1F*[NPLANES];     /*create array to store cell drift times*/    
  bin_max                 = new Int_t*[NPLANES];    /*Array to store the bin number corresponding to the drift time distribution peak*/
  bin_maxContent          = new Int_t*[NPLANES];    /*Array to store the content (# events) corresponding to the bin with maximum content*/
  time_max                = new Double_t*[NPLANES]; /*Array to store the x-axis(drift time (ns)) corresponding to bin_max*/
  twenty_perc_maxContent  = new Double_t*[NPLANES]; /*Array to store 20% of maximum bin content (peak)*/						     
  ref_time                = new Double_t*[NPLANES]; /*Array to store ref_time(time corresp. to 20% of peak) times for each sense wire*/             
  offset                  = new Double_t*[NPLANES];


  if (mode=="card")
    {
      //cout << "Allocating Card Variables . . ." << endl;
      //Card Variables
      entries_card            = new Int_t*[NPLANES];
      t_zero_card             = new Double_t*[NPLANES];
      t_zero_card_err         = new Double_t*[NPLANES];
      card_hist               = new TH1F*[NPLANES];  //Array to store uncorrected histogram per card
      fitted_card_hist        = new TH1F*[NPLANES];  //Array to store fitted card histogram
      corr_card_hist          = new TH1F*[NPLANES];  //Array to store corrected histogram per card
      wire_min                = new Int_t*[NPLANES];
      wire_max                = new Int_t*[NPLANES];
      wireBinContentMax       = new Double_t*[NPLANES];
      wireBinContentLow       = new Double_t*[NPLANES];
      wireBinContentHigh      = new Double_t*[NPLANES];
      wireBinHigh             = new Double_t*[NPLANES];
      wireBinLow              = new Double_t*[NPLANES];
      wireFitRangeLow         = new Double_t*[NPLANES];
      wireFitRangeHigh        = new Double_t*[NPLANES];
    }
  
  for(int ip=0; ip<NPLANES; ip++)
    {
      entries[ip]                 = new Int_t[nwires[ip]]; 
      t_zero[ip]                  = new Double_t[nwires[ip]];
      t_zero_err[ip]              = new Double_t[nwires[ip]];
      t_zero_final[ip]            = new Double_t[nwires[ip]];
      cell_dt[ip]                 = new TH1F[nwires[ip]];
      cell_dt_corr[ip]            = new TH1F[nwires[ip]];
      fitted_cell_dt[ip]          = new TH1F[nwires[ip]];
      bin_max[ip]                 = new Int_t[nwires[ip]];                   
      bin_maxContent[ip]          = new Int_t[nwires[ip]];              
      time_max[ip]                = new Double_t[nwires[ip]];                 
      twenty_perc_maxContent[ip]  = new Double_t[nwires[ip]];   						     
      ref_time[ip]                = new Double_t[nwires[ip]];                    
      offset[ip]                  = new Double_t[nwires[ip]];

    
      if (mode=="card")
	{
      
      //Card Variables
      entries_card[ip]            = new Int_t[plane_cards[ip]];
      t_zero_card[ip]             = new Double_t[plane_cards[ip]];
      t_zero_card_err[ip]         = new Double_t[plane_cards[ip]];
      card_hist[ip]               = new TH1F[plane_cards[ip]];
      fitted_card_hist[ip]        = new TH1F[plane_cards[ip]];
      corr_card_hist[ip]          = new TH1F[plane_cards[ip]];
      wire_min[ip]                = new Int_t[plane_cards[ip]];
      wire_max[ip]                = new Int_t[plane_cards[ip]];
      wireBinContentMax[ip]       = new Double_t [plane_cards[ip]];
      wireBinContentLow[ip]       = new Double_t [plane_cards[ip]];
      wireBinContentHigh[ip]      = new Double_t [plane_cards[ip]];
      wireBinLow[ip]              = new Double_t [plane_cards[ip]];
      wireBinHigh[ip]             = new Double_t [plane_cards[ip]];
      wireFitRangeLow[ip]         = new Double_t [plane_cards[ip]];
      wireFitRangeHigh[ip]        = new Double_t [plane_cards[ip]];

	}
    }
  
  
}

//_______________________________________________________________
void DC_calib::CreateHistoNames()
{
 
  for(int ip=0; ip<NPLANES; ip++)
    {
      
      //Set-Up plane drift time histo labels
      plane_dt_name  = plane_names[ip]+"_time"; 
      plane_dt_title = spec + " DC, Plane "+plane_names[ip]+" Drift Time";
      
      plane_dt[ip].SetName(plane_dt_name);
      plane_dt[ip].SetTitle(plane_dt_title);
      plane_dt[ip].SetBins(NBINS, MINBIN, MAXBIN);
      plane_dt[ip].SetXTitle("Drift Time (ns)");
      plane_dt[ip].SetYTitle("Number of Entries / 1 ns");

      
      plane_dt_name_corr  = plane_names[ip]+"corrected_time"; 
      plane_dt_title_corr = spec + " DC, Plane "+plane_names[ip]+" Corrected Drift Time";
      
      plane_dt_corr[ip].SetName(plane_dt_name_corr);
      plane_dt_corr[ip].SetTitle(plane_dt_title_corr);
      plane_dt_corr[ip].SetBins(NBINS, MINBIN, MAXBIN);
      plane_dt_corr[ip].SetXTitle("Drift Time (ns)");
      plane_dt_corr[ip].SetYTitle("Number of Entries / 1 ns");
      

      
      //Set-Up Drift Time vs. Wire Number Histos labels
      dt_vs_wire_name  = "dt_vs_wire_plane_"+plane_names[ip]; 
      dt_vs_wire_title = spec + " Drift Time vs. Wire: Plane "+plane_names[ip];
      
      dt_vs_wire[ip].SetName(dt_vs_wire_name);
      dt_vs_wire[ip].SetTitle(dt_vs_wire_title);
      dt_vs_wire[ip].SetBins(nwires[ip], 0., nwires[ip], NBINS, MINBIN, MAXBIN);
      dt_vs_wire[ip].SetXTitle("Wire Number");
      dt_vs_wire[ip].SetYTitle("Drift Time (ns) / 1 ns");  

      dt_vs_wire_corr[ip].SetName(dt_vs_wire_name);
      dt_vs_wire_corr[ip].SetTitle(dt_vs_wire_title);
      dt_vs_wire_corr[ip].SetBins(nwires[ip], 0., nwires[ip], NBINS, MINBIN, MAXBIN);
      dt_vs_wire_corr[ip].SetXTitle("Wire Number");
      dt_vs_wire_corr[ip].SetYTitle("Drift Time (ns) / 1 ns");  

      //Set-Up cell drift time histos names
      for (wire = 0; wire < nwires[ip]; wire++)
	{
	  cell_dt_name  = Form("Wire_%d", wire+1); 
	  cell_dt_title = spec + " DC Plane " +plane_names[ip] + Form(": Wire_%d", wire+1);
	  
	  cell_dt[ip][wire].SetName(cell_dt_name);
	  cell_dt[ip][wire].SetTitle(cell_dt_title);
	  cell_dt[ip][wire].SetBins(NBINS, MINBIN, MAXBIN);
	  cell_dt[ip][wire].SetXTitle("Drift Time (ns)");
	  cell_dt[ip][wire].SetYTitle("Number of Entries / 1 ns");	       

	  fitted_cell_dt_name  = Form("Wire_%d", wire+1); 
	  fitted_cell_dt_title = spec + " DC Plane " +plane_names[ip] + Form(": Wire_%d", wire+1);
	  
	  fitted_cell_dt[ip][wire].SetName(fitted_cell_dt_name);
	  fitted_cell_dt[ip][wire].SetTitle(fitted_cell_dt_title);
	  fitted_cell_dt[ip][wire].SetBins(200, MINBIN, MAXBIN);
	  fitted_cell_dt[ip][wire].SetXTitle("Drift Time (ns)");
	  fitted_cell_dt[ip][wire].SetYTitle("Number of Entries / 1 ns");

	  cell_dt_corr[ip][wire].SetName(cell_dt_name);
	  cell_dt_corr[ip][wire].SetTitle(cell_dt_title);
	  cell_dt_corr[ip][wire].SetBins(NBINS, MINBIN, MAXBIN);
	  cell_dt_corr[ip][wire].SetXTitle("Drift Time (ns)");
	  cell_dt_corr[ip][wire].SetYTitle("Number of Entries / 1 ns");

	} //End Loop over wires

      if (mode=="card")
	{
	  //cout << "Setting Card Histogram Names . . ." << endl;
	  //Loop over plane cards :: CARD_MODE
	  for (card = 0; card < plane_cards[ip]; card++ )
	    {
	      
	      card_hist_name = Form("UnCorr_Card_%d", card+1); 
	      card_hist_title = spec + " DC Plane " +plane_names[ip] + Form(": Uncorrected Card_%d", card+1);
	      
	      
	      card_hist[ip][card].SetName(card_hist_name);
	      card_hist[ip][card].SetTitle(card_hist_title);
	      card_hist[ip][card].SetBins(NBINS, MINBIN, MAXBIN);
	      card_hist[ip][card].SetXTitle("Drift Time (ns)");
	      card_hist[ip][card].SetYTitle("Number of Entries / 1 ns");
	      
	      fitted_card_hist_name = Form("Fitted_Card_%d", card+1); 
	      fitted_card_hist_title = spec + " DC Plane " +plane_names[ip] + Form(": Fitted Card_%d", card+1);
	      
	      fitted_card_hist[ip][card].SetName(fitted_card_hist_name);
	      fitted_card_hist[ip][card].SetTitle(fitted_card_hist_title);
	      fitted_card_hist[ip][card].SetBins(NBINS, MINBIN, MAXBIN);
	      fitted_card_hist[ip][card].SetXTitle("Drift Time (ns)");
	      fitted_card_hist[ip][card].SetYTitle("Number of Entries / 1 ns");
	      
	      
	      corr_card_hist_name = Form("Corr_Card_%d", card+1); 
	      corr_card_hist_title = spec + " DC Plane " +plane_names[ip] + Form(": Corrected Card_%d", card+1);
	      
	      
	      corr_card_hist[ip][card].SetName(corr_card_hist_name);
	      corr_card_hist[ip][card].SetTitle(corr_card_hist_title);
	      corr_card_hist[ip][card].SetBins(NBINS, MINBIN, MAXBIN);
	      corr_card_hist[ip][card].SetXTitle("Drift Time (ns)");
	      corr_card_hist[ip][card].SetYTitle("Number of Entries / 1 ns");
	      
	      
	    } //end loop over cards

	  
	} //end card mode

    } //End Loop over Planes
  
} //End CreateHistoNames() method

//________________________________________________________________________
void DC_calib::GetCard()
{

  //ONLY required in :: CARD MODE
      
  if (spec=="HMS")
    {
      //---1U1 MIN----          ----1U1 MAX-----
      wire_min[0][0]=1;         wire_max[0][0]=16;
      wire_min[0][1]=17;        wire_max[0][1]=32;
      wire_min[0][2]=33;        wire_max[0][2]=48;
      wire_min[0][3]=49;        wire_max[0][3]=64;
      wire_min[0][4]=65;        wire_max[0][4]=80;
      wire_min[0][5]=81;        wire_max[0][5]=96;

      //--1U2 MIN----           ---1U2 MAX------
      wire_min[1][0]=1;         wire_max[1][0]=16;
      wire_min[1][1]=17;        wire_max[1][1]=32;
      wire_min[1][2]=33;        wire_max[1][2]=48;
      wire_min[1][3]=49;        wire_max[1][3]=64;
      wire_min[1][4]=65;        wire_max[1][4]=80;
      wire_min[1][5]=81;        wire_max[1][5]=96;

      //--1X1 MIN----           ----1X1 MAX-----
      wire_min[2][0]=1;         wire_max[2][0]=11;
      wire_min[2][1]=12;        wire_max[2][1]=27;
      wire_min[2][2]=28;        wire_max[2][2]=43;
      wire_min[2][3]=44;        wire_max[2][3]=59;
      wire_min[2][4]=60;        wire_max[2][4]=75;
      wire_min[2][5]=76;        wire_max[2][5]=91;
      wire_min[2][6]=92;        wire_max[2][6]=102;
      
      //--1X2 MIN-----         ----1X2 MAX------
      wire_min[3][0]=1;         wire_max[3][0]=11;
      wire_min[3][1]=12;        wire_max[3][1]=27;
      wire_min[3][2]=28;        wire_max[3][2]=43;
      wire_min[3][3]=44;        wire_max[3][3]=59;
      wire_min[3][4]=60;        wire_max[3][4]=75;
      wire_min[3][5]=76;        wire_max[3][5]=91;
      wire_min[3][6]=92;        wire_max[3][6]=102;

      //--1V2 MIN-----         ---1V2 MAX-------
      wire_min[4][0]=1;        wire_max[4][0]=16;
      wire_min[4][1]=17;       wire_max[4][1]=32;
      wire_min[4][2]=33;       wire_max[4][2]=48;
      wire_min[4][3]=49;       wire_max[4][3]=64;
      wire_min[4][4]=65;       wire_max[4][4]=80;
      wire_min[4][5]=81;       wire_max[4][5]=96;

      
      //--1V1 MIN-----         ---1V1 MAX-------
      wire_min[5][0]=1;        wire_max[5][0]=16;
      wire_min[5][1]=17;       wire_max[5][1]=32;
      wire_min[5][2]=33;       wire_max[5][2]=48;
      wire_min[5][3]=49;       wire_max[5][3]=64;
      wire_min[5][4]=65;       wire_max[5][4]=80;
      wire_min[5][5]=81;       wire_max[5][5]=96;

      //--2V1 MIN-----         ---2V1 MAX-------
      wire_min[6][0]=1;        wire_max[6][0]=16;
      wire_min[6][1]=17;       wire_max[6][1]=32;
      wire_min[6][2]=33;       wire_max[6][2]=48;
      wire_min[6][3]=49;       wire_max[6][3]=64;
      wire_min[6][4]=65;       wire_max[6][4]=80;
      wire_min[6][5]=81;       wire_max[6][5]=96;

      //--2V2 MIN-----         ---2V2 MAX-------
      wire_min[7][0]=1;        wire_max[7][0]=16;
      wire_min[7][1]=17;       wire_max[7][1]=32;
      wire_min[7][2]=33;       wire_max[7][2]=48;
      wire_min[7][3]=49;       wire_max[7][3]=64;
      wire_min[7][4]=65;       wire_max[7][4]=80;
      wire_min[7][5]=81;       wire_max[7][5]=96;

      //--2X2 MIN-----         ----2X2 MAX------
      wire_min[8][0]=1;         wire_max[8][0]=11;
      wire_min[8][1]=12;        wire_max[8][1]=27;
      wire_min[8][2]=28;        wire_max[8][2]=43;
      wire_min[8][3]=44;        wire_max[8][3]=59;
      wire_min[8][4]=60;        wire_max[8][4]=75;
      wire_min[8][5]=76;        wire_max[8][5]=91;
      wire_min[8][6]=92;        wire_max[8][6]=102;

      //--2X1 MIN-----         ----2X1 MAX------
      wire_min[9][0]=1;         wire_max[9][0]=11;
      wire_min[9][1]=12;        wire_max[9][1]=27;
      wire_min[9][2]=28;        wire_max[9][2]=43;
      wire_min[9][3]=44;        wire_max[9][3]=59;
      wire_min[9][4]=60;        wire_max[9][4]=75;
      wire_min[9][5]=76;        wire_max[9][5]=91;
      wire_min[9][6]=92;        wire_max[9][6]=102;
      

      //--2U2 MIN-----         ---2U2 MAX-------
      wire_min[10][0]=1;        wire_max[10][0]=16;
      wire_min[10][1]=17;       wire_max[10][1]=32;
      wire_min[10][2]=33;       wire_max[10][2]=48;
      wire_min[10][3]=49;       wire_max[10][3]=64;
      wire_min[10][4]=65;       wire_max[10][4]=80;
      wire_min[10][5]=81;       wire_max[10][5]=96;

       //--2U1 MIN-----         ---2U1 MAX-------
      wire_min[11][0]=1;        wire_max[11][0]=16;
      wire_min[11][1]=17;       wire_max[11][1]=32;
      wire_min[11][2]=33;       wire_max[11][2]=48;
      wire_min[11][3]=49;       wire_max[11][3]=64;
      wire_min[11][4]=65;       wire_max[11][4]=80;
      wire_min[11][5]=81;       wire_max[11][5]=96;


    } //end hms card defnitions

  else if (spec=="SHMS")

    {

      //---1U1 MIN---        ----1U1 MAX-------
      wire_min[0][0]=1,      wire_max[0][0]=15;
      wire_min[0][1]=16,     wire_max[0][1]=31;
      wire_min[0][2]=32,     wire_max[0][2]=47;
      wire_min[0][3]=48,     wire_max[0][3]=63;
      wire_min[0][4]=64,     wire_max[0][4]=79;
      wire_min[0][5]=80,     wire_max[0][5]=95;
      wire_min[0][6]=96,     wire_max[0][6]=107;

      //--1U2 MIN----        ---1U2 MAX--------
      wire_min[1][0]=1,      wire_max[1][0]=12;
      wire_min[1][1]=13,     wire_max[1][1]=28;
      wire_min[1][2]=29,     wire_max[1][2]=44;
      wire_min[1][3]=45,     wire_max[1][3]=60;
      wire_min[1][4]=61,     wire_max[1][4]=76;
      wire_min[1][5]=77,     wire_max[1][5]=92;
      wire_min[1][6]=93,     wire_max[1][6]=107;

      //--1X1 MIN----        ---1X1 MAX--------
      wire_min[2][0]=1,      wire_max[2][0]=16;
      wire_min[2][1]=17,     wire_max[2][1]=32;
      wire_min[2][2]=33,     wire_max[2][2]=48;
      wire_min[2][3]=49,     wire_max[2][3]=64;
      wire_min[2][4]=65,     wire_max[2][4]=79;

      //--1X2 MIN----        ---1X2 MAX--------
      wire_min[3][0]=1,      wire_max[3][0]=15;
      wire_min[3][1]=16,     wire_max[3][1]=31;
      wire_min[3][2]=32,     wire_max[3][2]=47;
      wire_min[3][3]=48,     wire_max[3][3]=63;
      wire_min[3][4]=64,     wire_max[3][4]=79;

      //--1V1 MIN----        --1V1 MAX---------
      wire_min[4][0]=1,      wire_max[4][0]=15;
      wire_min[4][1]=16,     wire_max[4][1]=31;
      wire_min[4][2]=32,     wire_max[4][2]=47;
      wire_min[4][3]=48,     wire_max[4][3]=63;
      wire_min[4][4]=64,     wire_max[4][4]=79;
      wire_min[4][5]=80,     wire_max[4][5]=95;
      wire_min[4][6]=96,     wire_max[4][6]=107;
      
      //--1V2 MIN----        --1V2 MAX---------
      wire_min[5][0]=1,      wire_max[5][0]=12;
      wire_min[5][1]=13,     wire_max[5][1]=28;
      wire_min[5][2]=29,     wire_max[5][2]=44;
      wire_min[5][3]=45,     wire_max[5][3]=60;
      wire_min[5][4]=61,     wire_max[5][4]=76;
      wire_min[5][5]=77,     wire_max[5][5]=92;
      wire_min[5][6]=93,     wire_max[5][6]=107;

      //--2V2 MIN----        --2V2 MAX---------
      wire_min[6][0]=1,      wire_max[6][0]=12;
      wire_min[6][1]=13,     wire_max[6][1]=28;
      wire_min[6][2]=29,     wire_max[6][2]=44;
      wire_min[6][3]=45,     wire_max[6][3]=60;
      wire_min[6][4]=61,     wire_max[6][4]=76;
      wire_min[6][5]=77,     wire_max[6][5]=92;
      wire_min[6][6]=93,     wire_max[6][6]=107;

      //--2V1 MIN----        --2V1 MAX---------
      wire_min[7][0]=1,      wire_max[7][0]=15;
      wire_min[7][1]=16,     wire_max[7][1]=31;
      wire_min[7][2]=32,     wire_max[7][2]=47;
      wire_min[7][3]=48,     wire_max[7][3]=63;
      wire_min[7][4]=64,     wire_max[7][4]=79;
      wire_min[7][5]=80,     wire_max[7][5]=95;
      wire_min[7][6]=96,     wire_max[7][6]=107;

      //--2X2 MIN----        --2X2 MAX---------
      wire_min[8][0]=1,      wire_max[8][0]=15;
      wire_min[8][1]=16,     wire_max[8][1]=31;
      wire_min[8][2]=32,     wire_max[8][2]=47;
      wire_min[8][3]=48,     wire_max[8][3]=63;
      wire_min[8][4]=64,     wire_max[8][4]=79;

      //--2X1 MIN----        --2X1 MAX---------
      wire_min[9][0]=1,      wire_max[9][0]=16;
      wire_min[9][1]=17,     wire_max[9][1]=32;
      wire_min[9][2]=33,     wire_max[9][2]=48;
      wire_min[9][3]=49,     wire_max[9][3]=64;
      wire_min[9][4]=65,     wire_max[9][4]=79;

      //--2U2 MIN----        --2U2 MAX---------
      wire_min[10][0]=1,      wire_max[10][0]=12;
      wire_min[10][1]=13,     wire_max[10][1]=28;
      wire_min[10][2]=29,     wire_max[10][2]=44;
      wire_min[10][3]=45,     wire_max[10][3]=60;
      wire_min[10][4]=61,     wire_max[10][4]=76;
      wire_min[10][5]=77,     wire_max[10][5]=92;
      wire_min[10][6]=93,     wire_max[10][6]=107;

      //--2U1 MIN----        --2U1 MAX---------
      wire_min[11][0]=1,      wire_max[11][0]=15;
      wire_min[11][1]=16,     wire_max[11][1]=31;
      wire_min[11][2]=32,     wire_max[11][2]=47;
      wire_min[11][3]=48,     wire_max[11][3]=63;
      wire_min[11][4]=64,     wire_max[11][4]=79;
      wire_min[11][5]=80,     wire_max[11][5]=95;
      wire_min[11][6]=96,     wire_max[11][6]=107;

      
    }  //end shms card definitions

  
} //End method getCard

//________________________________________________________________
void DC_calib::EventLoop(string option="")
{

  if (mode=="card")
    {
      //cout << "Executing GetCard() Method . . ." << endl;
      GetCard();
    }
  //cout << "successful GetCards() " << endl;
  //Initialize counter to count how many good events (5/6 hits / chamber)
  ngood_evts = 0;

  //Loop over all entries
  for(Long64_t i=0; i<num_evts; i++)
    {

      tree->GetEntry(i);  
      
      //------READ USER 'pid' input to determine particle type to calibrate----------
      
      //NO PID Cut,
      if(pid=="pid_kFALSE")
	{
	  cal_elec = 1;
	  cer_elec = 1;         
	}
      
      //PID Cut, Set Bool_t to actual leaf value, and see if it passes cut
      else if (pid=="pid_elec")
	{
	  cal_elec = cal_etot>0.1;  //normalize energy > 0.1 (bkg cleanup)
	  cer_elec = cer_npe>1.0;     //number of photoelec. > 1 (electrons)
	  
	}
      
      else 
	{
	  cout << "Enter which particle to calibrate in main_calib.C: " << endl;
	  cout << "For electrons: 'pid_elec' " << endl;
	  cout << "NO PID Cuts: 'pid_KFALSE' " << endl;
	  cout << "Exiting NOW!" << endl;
	  exit (EXIT_SUCCESS);
	}
      
      //----------------------------------------------------------------------------
      //cout << "Set PID Flags " << endl;
     //Initialize chamber hit counter
      cnts_ch1=0;
      cnts_ch2=0;
       

      for(int ip=0; ip<NPLANES; ip++)
	{
	  
	  //Count how many planes were hit by the event
	  if(ip<=5 && ndata_time[ip]==1) 
	    { 
	      cnts_ch1++; 
	    }

	  if(ip>5 && ndata_time[ip]==1) 
	    {
	      cnts_ch2++; 
	    }
	  
	}

      //Count the number of events that had at least 5/6 hits / chamber
      if (cnts_ch1>4 && cnts_ch2>4)
	{
	  ngood_evts++;
	}
      
      good_event=kFALSE;
      

      //***good event definition***: cal_energy > 100 MeV, cer_npeSum > 1.0
      good_event = cal_elec && cer_elec; //&& cnts_ch1>4 && cnts_ch2>4;
      
   
	  // cout << "passed cut: " << i << endl;
	  for(Int_t ip=0; ip<NPLANES; ip++)
	    {
	      // cout << "PLANE: " << ip << endl;

   	  if (good_event&&ndata_time[ip]==1)
        	{
		
	      
	      //-----------------------------------------------------------------------------------------	  
	      
		  
		  //Loop over number of hits for each trigger in each DC plane 
		  for(Int_t j = 0; j < ndata_time[ip]; j++)    
		    {
		      
		      //get wire hit for ith event in 'ip' plane
		      wire = int(wire_num[ip][j]);

		      //cout << "About to enter Wire Mode in EventLoop. . ." << endl;
		       
		       

		      if (mode=="wire")
			{

		      //-----------WIRE MODE ONLY----------------------------
		      
		      if (option=="FillUncorrectedTimes")
			{
			  //Fill uncorrected plane drift times  (from: get_pdc_time_histo.C )
			  plane_dt[ip].Fill(drift_time[ip][j] - offset[ip][wire-1]); 
			  dt_vs_wire[ip].Fill(wire_num[ip][j], drift_time[ip][j] - offset[ip][wire-1]);
			  cell_dt[ip][wire-1].Fill(drift_time[ip][j] - offset[ip][wire-1]);
			  fitted_cell_dt[ip][wire-1].Fill(drift_time[ip][j] - offset[ip][wire-1]);
			}

		      
		      else if (option=="ApplyT0Correction")
			{
			  //Fill corrected plane drift times 
			  plane_dt_corr[ip].Fill(drift_time[ip][j] - offset[ip][wire-1] - t_zero[ip][wire-1]); 
			  cell_dt_corr[ip][wire-1].Fill(drift_time[ip][j] - offset[ip][wire-1] - t_zero[ip][wire-1]);
			  dt_vs_wire_corr[ip].Fill(wire_num[ip][j], drift_time[ip][j] - offset[ip][wire-1] - t_zero[ip][wire-1]);
			  t_zero_final[ip][wire-1] = offset[ip][wire-1] + t_zero[ip][wire-1];
			}

		      //-------------END WIRE MODE ONLY------------------------

			}
		       
		      //cout << "About To Enter Card Mode in EventLoop() " << endl;

		      if (mode=="card")
		      	{

			  
		      //cout << "Entered in Card Mode " << endl;
			      //------------CARD MODE ONLY-----------------------------
			      
			  if (option=="FillUncorrectedTimes")
			    {
			     
			      
			      //Fill uncorrected plane drift times 
			      plane_dt[ip].Fill(drift_time[ip][j] - offset[ip][wire-1]); 
			      dt_vs_wire[ip].Fill(wire_num[ip][j], drift_time[ip][j] - offset[ip][wire-1]);
			      
			      //Loop over plane cards
			      for (card = 0; card < plane_cards[ip]; card++ )
				{
			    
				  //Conditions
				  if (wire >= wire_min[ip][card] && wire <= wire_max[ip][card])
				    {
				      //Fill Uncorrected Cards dRIFT tIME
				      card_hist[ip][card].Fill(drift_time[ip][j]);
				      fitted_card_hist[ip][card].Fill(drift_time[ip][j]);
				  
				    } //End loop over wires
				  
				} //End loop over cards
			      
			    } //end option argument
		    

			  else if (option=="ApplyT0Correction")
			    {
			 
			      for (card = 0; card < plane_cards[ip]; card++ )
				{
				  
				  //Fill Corrected plane drift times (using the CARD method)
				  
				  if (wire >= wire_min[ip][card] && wire <= wire_max[ip][card])
				    {
			  
				      //Fill Corrected Plane Drift Times
				      plane_dt_corr[ip].Fill(drift_time[ip][j] - t_zero_card[ip][card]);
				      
				      
				      //Fill Corrected Card Drift Times
				      dt_vs_wire_corr[ip].Fill(wire_num[ip][j], drift_time[ip][j] - t_zero_card[ip][card]);  
				      corr_card_hist[ip][card].Fill(drift_time[ip][j]-t_zero_card[ip][card]);
   				      //t_zero_final[ip][wire-1] = t_zero_card[ip][card];
				  
				      
				    } 
				  
				} //loop over cards
			      
			    } //end option argument
			  
			  //------------END CARD MODE ONLY------------------------
			      
			  
			  } //End CARD MODE
	       
		    } //end loop over hits

	     	 
//-----------------------------------------------------------------------------------------
	      
		} //good event cut   
       
	    } //end plane loop
    
    } //end loop over events

} // end event loop method



//_________________________________________________________________________
/*
void DC_calib::CalcT0Historical()
{
  //Loop over DC PLANES
  for (int ip = 0; ip<NPLANES; ip++)
    {
      //Loop over DC wires
      for (wire = 0; wire < nwires[ip]; wire++)
	{
	  double tempt0 = 0;

	  if (cell_dt[ip][wire].GetEntries()>500){
	    bin_max[ip][wire] = cell_dt[ip][wire].GetMaximumBin()+10;
	    double histMax = 0;
	    for (Int_t bin = 1; bin <= bin_max[ip][wire]+10; bin++){
	      double avgMax = 0;
	      double sumMax = 0;
	      int nseq = 0;
	      int nmin = bin+8;
	      
	      for (int ii = bin; ii<=nmin; ii++){
		double cc = cell_dt[ip][wire].GetBinContent(ii);
		if (cc>10){
		  sumMax += cc;
		  nseq++;
		}
	      }
	      
	      if (nseq>6){
		avgMax = sumMax/(double)nseq;
	      }
	      if (avgMax>histMax){
		histMax = avgMax;
	      }	   
	    }//end loop bins
	    twenty_perc_maxContent[ip][wire] = 0.2*histMax;
	    
	    //Use the 20% to find the t0 tdc_offset
	    for (Int_t bin = 10; bin <= MAXBIN-8; bin++){
	      double binMax = 0;
	      int nseq = 0;
	      int nmin = bin+8;
	      
	      for (int ii = bin; ii<=nmin; ii++){
		double cc = cell_dt[ip][wire].GetBinContent(ii);
		if (cc>twenty_perc_maxContent[ip][wire]){
		  nseq++;
		}
	      }
	      
	      if (nseq>7){
		int binRef = 0;
		cell_dt[ip][wire].GetBinWithContent(twenty_perc_maxContent[ip][wire], binRef, 1,bin_max[ip][wire],10.0);
		tempt0 = cell_dt[ip][wire].GetXaxis()->GetBinCenter(binRef);
		break;
	      }
	    }//end loop bins
	  }//end entries>500

	  t_zero[ip][wire] = tempt0;
	 
	}//end loop wires
    }//end loop planes
}
*/

//_________________________________________________________________________
void DC_calib::GetTwentyPercent_Peak()
{
  
  //Loop over DC PLANES
  for (int ip = 0; ip<NPLANES; ip++)
    {

      
     
      //Loop over DC wires
      for (wire = 0; wire < nwires[ip]; wire++)
	{
	 
	  bin_max[ip][wire]                = cell_dt[ip][wire].GetMaximumBin();                      //Get bin with Maximum Content
	  bin_maxContent[ip][wire]         = cell_dt[ip][wire].GetBinContent(bin_max[ip][wire]);	      //Get content of bin_max
	  time_max[ip][wire]               = cell_dt[ip][wire].GetXaxis()->GetBinCenter(bin_max[ip][wire]);  //Get time (ns) [x-axis] corresponding to bin_max 
	  twenty_perc_maxContent[ip][wire] = bin_maxContent[ip][wire] * percent;	                      
	  //Calculate 20% of max bin content
	  //ref_time[ip][wire] = cell_dt[ip][wire].GetBinCenter(cell_dt[ip][wire].FindBin(twenty_perc_maxContent[ip][wire]));
	  //Loop over DC drift time bins
	  for (Int_t bin = 1; bin <= bin_max[ip][wire]; bin++)
	    {
	      content_bin = cell_dt[ip][wire].GetBinContent(bin);              //get bin content for all bins in a wire
	      
	      content.push_back(content_bin);                                      //add bin content to array
	      bin_num.push_back(bin);                                              //add bin number to array
	      
	      // check if 2 bin contents have been stored and examine if these contents exceed or not 20% of peak
	      if (content.size() == 5) {
		//initialize counter to count how many bin contents >= 20%
		counts = 0;
		
		// Loop over 2 bin contents stored in array content
		for (Int_t j=0; j<5; j++)
		  {
		    if(content[j] >=  twenty_perc_maxContent[ip][wire])
		      {
			counts = counts+1;
			if(counts >= 4) { goto stop;}
		      }
		    
		    content.clear();
		    bin_num.clear();
		    
		  }
		
	      }
	      
	    }
	stop:
	  ref_time[ip][wire] = cell_dt[ip][wire].GetXaxis()->GetBinCenter(bin_num[0]); //Get time corresponding ~20% Max BIN CONTENT  
	  
	  
	} // end wire loop
      
    } //end planes loop
 
} //End GetTwentyPercent_Peak() method


//____________________________________________________________________________________
void DC_calib::FitWireDriftTime()
{
  
  Double_t sum_NUM= 0.0;
  Double_t sum_DEN=0.0;




  //Loop over planes
  for (Int_t ip = 0; ip < NPLANES; ip++)
    {
    
      //Loop over DC sense wires
      for (wire = 0; wire < nwires[ip]; wire++)
	{
	  //Get Bin no. associated with the reference time
	  binx = cell_dt[ip][wire].GetXaxis()->FindBin(ref_time[ip][wire]);
	  
	  //Get time corresponding to bin (fit range) 
	  time_init = cell_dt[ip][wire].GetXaxis()->GetBinCenter(binx - 10); //choose bin range over which to fit
	  time_final = cell_dt[ip][wire].GetXaxis()->GetBinCenter(binx + 10); 
	  
	  //Create Fit Function
	  tZero_fit = new TF1("tZero_fit", "[0]*x + [1]", time_init, time_final);
	  
	  //Set Parameter Names and Values
	  tZero_fit->SetParName(0, "slope");
	  tZero_fit->SetParName(1, "y-int");
	  tZero_fit->SetParameter(0, 1.0);
	  tZero_fit->SetParameter(1, 1.0);

	  entries[ip][wire] = fitted_cell_dt[ip][wire].GetEntries();

	  //Avoid mssg due to zero entries: Warning in <Fit>: Fit data is empty 
	  //	  if (entries[ip][wire] !=0)
	  //{
	      //Fit Function in specified range
	      fitted_cell_dt[ip][wire].Fit("tZero_fit", "QR");
	      //}
	      gStyle->SetOptFit(1);
	  //Get Parameters and their errors
	  m = tZero_fit->GetParameter(0);
	  y_int = tZero_fit->GetParameter(1);
	  m_err = tZero_fit->GetParError(0);
	  y_int_err = tZero_fit->GetParError(1);
	  std_dev = fitted_cell_dt[ip][wire].GetStdDev();

	  //Require sufficient events and NOT CRAZY! tzero values, otherwis, set t0 to ZERO
	  if ( abs(-y_int/m) < std_dev*5.0 && m > 0.2 && entries[ip][wire]>max_wire_entry  )
	    {
	      t_zero[ip][wire] = - y_int/m ;
	      t_zero_err[ip][wire] = sqrt(y_int_err*y_int_err/(m*m) + y_int*y_int*m_err*m_err/(m*m*m*m) );
	      
	      if (t_zero_err[ip][wire] < t0_err_thrs)
		{
		  //calculate the weighted average     
		  sum_NUM = sum_NUM + t_zero[ip][wire]/pow(t_zero_err[ip][wire],2);
		  sum_DEN = sum_DEN + 1.0/ (pow(t_zero_err[ip][wire],2));
		  //cout << "wire: " << wire + 1 << " :: " << "tzero: " << t_zero[ip][wire] << endl;
		}
	      else if (t_zero_err[ip][wire] >= t0_err_thrs) { 

		t_zero[ip][wire] = 0.0;

	      }
	      
	    }

	 
	 
	  else if (abs(-y_int/m)>=5.0*std_dev ||  m <= 0.2  || entries[ip][wire] <= max_wire_entry || t_zero_err[ip][wire] >= t0_err_thrs )
		{
		      t_zero[ip][wire] = 0.0;
		}	  

	} //END LOOP OVER WIRES

      weighted_avg[ip] = sum_NUM /sum_DEN; 
      
      //Set weighted averages to tzero[ip][wire]
      //Loop over wires to re-assign tzero per wires
      for (wire = 0; wire < nwires[ip]; wire++)
	{
	  if (t_zero[ip][wire] == 0.0)
	    {
	      t_zero[ip][wire] = weighted_avg[ip];
	      t_zero_final[ip][wire] = offset[ip][wire] + weighted_avg[ip];
	   }
	      
	}
      
      
    }//END LOOP OVER PLANES  

  
} //End FitWireDriftTime() method

//_______________________________________________________________________
void DC_calib::GetTwentyPercent_Card()
{

  binValLow = binValHigh = binSearchLow = 0, binSearchHigh = 0;
  binDiffThreshHigh = 100; binDiffThreshLow = 100;

  for (int ip = 0; ip<NPLANES; ip++)
    {
	
      for (card = 0; card < plane_cards[ip]; card++)
	{
	
	   binSearchHigh =  fitted_card_hist[ip][card].GetMaximumBin();           //returns bin value of maximum content
	   wireBinContentMax[ip][card] = fitted_card_hist[ip][card].GetMaximum(); // return maximum histo bin content
	   entries_card[ip][card] = fitted_card_hist[ip][card].GetEntries();      // return entries of card histograms

	   //if (entries_card[ip][card] < 2000)
	   //{
	   //  wireBinContentLow[ip][card]  = wireBinContentMax[ip][card]*0.20;        // Get content with 10% of max bin content                                                          
	   //  wireBinContentHigh[ip][card] = wireBinContentMax[ip][card]*0.40;       // Get content with 30% of max bin content              
	   //   }

	   //else if (entries_card[ip][card] >= 2000)
	   //{
	       wireBinContentLow[ip][card]  = wireBinContentMax[ip][card]*0.20;        // Get content with 20% of max bin content
	       wireBinContentHigh[ip][card] = wireBinContentMax[ip][card]*0.60;       // Get content with 60% of max bin content
	       //}
	   
	   // | content_of_desired_bin - (binSearch_HighContent-binSearchLowContent) | <= binDiffTherhold 
	   fitted_card_hist[ip][card].GetBinWithContent(wireBinContentLow[ip][card],  binValLow,  binSearchLow, binSearchHigh, binDiffThreshLow); 
	   fitted_card_hist[ip][card].GetBinWithContent(wireBinContentHigh[ip][card], binValHigh, binSearchLow, binSearchHigh, binDiffThreshHigh); 
	  	   
	   while(binValLow == 0)
	     {

	       binDiffThreshLow = binDiffThreshLow  + 100;
	       	       
	       fitted_card_hist[ip][card].GetBinWithContent(wireBinContentLow[ip][card],  binValLow,  binSearchLow, binSearchHigh, binDiffThreshLow); 	  	     
		     
	     }
	    	 
	   while(binValHigh == 0)
	     {

	       binDiffThreshHigh = binDiffThreshHigh + 100;

	       fitted_card_hist[ip][card].GetBinWithContent(wireBinContentHigh[ip][card],  binValHigh,  binSearchLow, binSearchHigh, binDiffThreshHigh);
	     }	     
	  
	  wireBinLow[ip][card]  = binValLow;
	  wireBinHigh[ip][card] = binValHigh;
	  wireFitRangeLow[ip][card]  = fitted_card_hist[ip][card].GetBinCenter(wireBinLow[ip][card]);
	  wireFitRangeHigh[ip][card] = fitted_card_hist[ip][card].GetBinCenter(wireBinHigh[ip][card]);

	} //end plane cards loop

    } //end planes loop
  
} //end  GetTwentyPercent_Card() method

//___________________________________________________________________
Double_t DC_calib::GetCardT0_alternative(Int_t ip, Int_t card)
{

  //NOTE**: This method must be called within FitCardDriftTime() method
  // if the fit t0 error/card entries exceed a threshold
  
  //  cout << "Inside GetCardT0 Method . . ." << endl;
  
    cout << "Calling GetCardT0_alternative ( " << ip << " , " << card << " ) " << endl;
 
  
  binSearchHigh = fitted_card_hist[ip][card].GetMaximumBin();
  maxContent_frac = fitted_card_hist[ip][card].GetBinContent(binSearchHigh) * 0.20;

  //cout << "Maximum Bin Number: " << binSearchHigh << endl;
  //cout << "Maximum Bin Content: " << fitted_card_hist[ip][card].GetBinContent(binSearchHigh) << endl;
  //cout << "20% of Max Content: " << maxContent_frac << endl;

  //cout << "****About to loop over bins**** ... " << endl;
  //Loop over card drift time bins
  for (Int_t bin=1; bin <= binSearchHigh; bin++)
    {
      content_bin = fitted_card_hist[ip][card].GetBinContent(bin);
      
      content.push_back(content_bin);
      bin_num.push_back(bin);

      //      cout << "bin: " << bin << endl;
      // cout << "content size: " << content.size() << endl;
      //  cout << "About to check content size " << endl;
      if (content.size() == 10)
	{
	  counts = 0;

	  //  cout << "About to loop over 5 bins " << endl;
	  for (Int_t j=0; j<10; j++)
	    {
	      //cout << "content["<<j<<"]: " << content[j] << endl; 
	      //cout << "max_Content frac: " << maxContent_frac << endl;
	      //cout << "counts: " << counts << endl;
	      if(content[j] > 0)
		{
		  counts = counts + 1;
		  if(counts >=8) {goto stop;}
		} //end 'if' statement
	      
	      content.clear();
	      bin_num.clear();

	    } //end loop over 5 bin sample

	} //end 'if' stmnt checking content size

    } //end loop over bins

 stop:
  // cout << "bin_num[0]:  " << bin_num[0] << endl;
  card_T0 = fitted_card_hist[ip][card].GetXaxis()->GetBinCenter(bin_num[0]);
  //cout << "card_T0 = " << card_T0 << endl;
  return card_T0;


}

//___________________________________________________________________
void DC_calib::FitCardDriftTime()
{
  cout << "Entering FitCardDriftTime Method . . ." << endl;

  for (Int_t ip = 0; ip < NPLANES; ip++)
    {

      cout << "Plane : " << ip << endl;
    
     //Loop over DC cards
      for (card = 0; card < plane_cards[ip]; card++)
	{
     
	  cout << "card: " << card << endl;
	  tZero_fit = new TF1("tZero_fit", "[0]*x + [1]", wireFitRangeLow[ip][card], wireFitRangeHigh[ip][card]);
       
	  //Set Parameter Names and Values
	  tZero_fit->SetParName(0, "slope");
	  tZero_fit->SetParName(1, "y-int");
	  tZero_fit->SetParameter(0, 1.0);
	  tZero_fit->SetParameter(1, 1.0);
      
	  entries_card[ip][card] = fitted_card_hist[ip][card].GetEntries();

	  cout << "entries: " << entries_card[ip][card] << endl;

	  //Fit Function in specified range
	  fitted_card_hist[ip][card].Fit("tZero_fit", "QR");
       
	  gStyle->SetOptFit(1);
	  
	  //Get Parameters and their errors
	  m = tZero_fit->GetParameter(0);
	  y_int = tZero_fit->GetParameter(1);
	  m_err = tZero_fit->GetParError(0);
	  y_int_err = tZero_fit->GetParError(1);
	  std_dev = fitted_card_hist[ip][card].GetStdDev();
      
	  //Require sufficient events and NOT CRAZY! tzero values, otherwis, set t0 to ZERO
	  if ((abs(-y_int/m) < std_dev*5.0 && m > 0.0 )  || entries_card[ip][card]>2000)
	    {
	 
	      t_zero_card[ip][card] = - y_int/m ;
	      t_zero_card_err[ip][card] = sqrt(y_int_err*y_int_err/(m*m) + y_int*y_int*m_err*m_err/(m*m*m*m) );
    
	      //if error of t0 is bad  (15), then set t0 = 0, since it is liekly that #evts is very low and insignificant to be corrected
	      if(t_zero_card_err[ip][card] > t0_err_thrs || entries_card[ip][card] <= 2000 || m < 0.0)
		{
		  t_zero_card[ip][card] = GetCardT0_alternative(ip, card);
		  t_zero_card_err[ip][card] = 0.0;
		  //cout << "About to execute GetCardT0_alternative() " << endl;
		  cout << "plane: " << ip << " :: card: " << card << "t_zero_card: " << GetCardT0_alternative(ip, card) << endl;
		  
		}

	    }

	  //ensure to assign card tzero values to zero, if card entries are not sufficient                                                                                                                            
	  else if (abs(-y_int/m)>=5.0*std_dev ||  m <= 0.0  || entries_card[ip][card] <= 2000)                                                                                                              
	    {                                                                                                                                                                                                         	     	
	      t_zero_card[ip][card] = GetCardT0_alternative(ip, card);  
	      t_zero_card_err[ip][card] = 0.0;                                                                                                                           
	      //cout << "About to execute GetCardT0_alternative() " << endl;                                                                                             
	      cout << "plane: " << ip << " :: card: " << card << "t_zero_card: " << GetCardT0_alternative(ip, card) << endl;                       
	    }                                                                                                                                                                                                         
	  

	      for(wire=1; wire<=nwires[ip]; wire++)
		{
   
		  if (wire >= wire_min[ip][card] && wire <=wire_max[ip][card]) //entries_card[ip][card]>max_wire_entry)
		    {
		   
		      t_zero[ip][wire-1] = t_zero_card[ip][card];
		      t_zero_err[ip][wire-1] = t_zero_card_err[ip][card];
		      t_zero_final[ip][wire-1] = t_zero_card[ip][card];                                                                                                                                                                      
                                                                                                                                                                                                        
		    } //end 'if' statement for wire group selection
		
		
		  //ensure to assign wire tzero values to zero, if card entries are not sufficient
		  //else if (wire >= wire_min[ip][card] && wire <=wire_max[ip][card] && t_zero_card_err[ip][card] > t0_err_thrs)//entries_card[ip][card] <= max_wire_entry)
		  //{
		  //  t_zero[ip][wire-1] = 0;
		  //  t_zero_final[ip][wire-1] = 0;                                                                                   
														                                                                                      
		  //}
		    
		} //end wire loop

	}  //end loop over cards
    
    } // end loop over planes

} //end FitCardDriftTime() method

//________________________________________________________________
void DC_calib::Calculate_tZero()
{
  
  //CalcT0Historical();

  if (mode=="wire")
    {
      GetTwentyPercent_Peak();
      FitWireDriftTime();
    }

  else if (mode=="card")
    {
   
      GetTwentyPercent_Card();
      FitCardDriftTime();
     
    }

}


//__________________________________________________________________________
void DC_calib::WriteTZeroParam()
{
  otxtfile_name =  "./"+spec+"_DC_"+mode.c_str()+"Log_"+ std::to_string(run_NUM) +"/"+spectre+"dc_tzero_per_wire_"+std::to_string(run_NUM)+".param";
  out_txtFILE.open(otxtfile_name);
  
  for (int ip=0; ip<NPLANES; ip++) { 
	  
    //write plane headers
    out_txtFILE << spectre+"tzero"+plane_names[ip] << "=" << endl;

    //cout << "Plane: "  << ip << endl;
   for (wire=0; wire<nwires[ip]; wire++) 
     {
       //cout << "Plane: " << ip << " ::  wire: " << wire+1 << " :: " << "tzero_final: "<< t_zero_final[ip][wire] << endl;
       if (wire <= 10) 
	 { 
	   out_txtFILE << setprecision(6) << t_zero_final[ip][wire] << fixed << ",";
	 }
       else if (wire>10 && wire <(nwires[ip]-1))
	 {
	   out_txtFILE << setprecision(6) << t_zero_final[ip][wire] << ((wire+1) % 16 ? ", " : "\n") << fixed;
	 }
       else if (wire==nwires[ip]-1) 
	 {
	   out_txtFILE << setprecision(6) << t_zero_final[ip][wire] << fixed << endl;
     
	   }
       
     } //END LOOP OVER WIRES

  } //END LOOP OVER PLANES
  
  out_txtFILE.close();
  
} //end WriteTZeroParam() Method

//_________________________________________________________________________________
void DC_calib::WriteLookUpTable()
{
  otxtfile_name = "./"+spec+"_DC_"+mode.c_str()+"Log_"+std::to_string(run_NUM)+"/"+spectre+"dc_calib_"+std::to_string(run_NUM)+".param";
  out_txtFILE.open(otxtfile_name);
  Double_t t_offset_firstbin = 0.0;
  //Set headers for subsequent columns of data
  out_txtFILE << Form("; Lookup Table: RUN %d", run_NUM) << "\n";
  out_txtFILE << "; number of bins in time to distance lookup table" << "\n";
  out_txtFILE << Form(spectre+"driftbins = %d", TOTAL_BINS+1) << "\n";
  out_txtFILE << "; number of 1st bin in table in ns" << "\n";
  out_txtFILE << spectre+Form("drift1stbin=%f", t_offset_firstbin + 1.) << "\n";
  out_txtFILE << "; bin size in ns" << "\n";
  out_txtFILE << spectre+"driftbinsz=1" << "\n";
  
//Loop over each plane of hms/shms Drift Chambers (DC1 & DC2)

  for (int ip=0; ip<NPLANES; ip++){
   
    
    //Get bin corresponding to t0 = 0 ns
    bin_t0[ip] = plane_dt_corr[ip].GetXaxis()->FindBin(t_offset_firstbin);
   
    //Get final bin 
    bin_final[ip] = bin_t0[ip] + TOTAL_BINS;
   
    //Find total BIN Content over entire integration range
    binContent_TOTAL[ip] = 0.; //set counter to zero


    for (int bin = bin_t0[ip]; bin <= bin_final[ip]; bin ++ ) {
     
      bin_Content[ip] = plane_dt_corr[ip].GetBinContent(bin);
     
      binContent_TOTAL[ip] = bin_Content[ip] + binContent_TOTAL[ip];
     
      //   cout << "Bin: " << bin << endl;
      //   cout << "Content " << bin_Content[ip] << endl;
      //   cout << "Content SUM : " << binContent_TOTAL[ip] << endl;
    }
   
    TString headers = spectre+"wc" + plane_names[ip] + "fract=";      
    out_txtFILE << headers;	  
   
    //Calculate LookUp Value
   
    binSUM[ip] = 0.0;
    bin_count = 0;
   
    for (int bin = bin_t0[ip]; bin <= bin_final[ip]; bin++) {
     
      bin_Content[ip] = plane_dt_corr[ip].GetBinContent(bin);
      binSUM[ip] = binSUM[ip] + bin_Content[ip];
     
     
      lookup_value[ip] = binSUM[ip] / binContent_TOTAL[ip];
      bin_count = bin_count + 1;
     
      if (bin_count <= 16 ) {
	out_txtFILE << setprecision(5) << lookup_value[ip] << fixed << ",";
      }
     
      else if (bin_count >16 && bin_count <= TOTAL_BINS) {
	out_txtFILE << setprecision(5) << lookup_value[ip] << ((bin_count+1) % 20 ? "," : "\n") << fixed; 
      }
      else {
	out_txtFILE  << setprecision(5) << lookup_value[ip] <<  fixed << endl;	  
      }
     
    } //END LOOP OVER plane drift time BINS

  } //END LOOP OVER PLANES                     

  out_txtFILE.close();

} //end WriteLookUpTable() method



//________________________________________________________________
void DC_calib::WriteToFile(Int_t debug = 0)
{

  
  //create output ROOT file to write UnCALIB./CALIB. histos
  ofile_name = "./"+spec+"_DC_"+mode.c_str()+"Log_"+std::to_string(run_NUM) +"/"+spec+"_DC_driftimes.root";
  out_file   = new TFile(ofile_name, "RECREATE"); 

  
  //***NOTE***  debug = 0 (OFF), 1 (ON)
  //-------------------------------------------------------------------
  if (debug == 1) 
    {
      //------write uncorrected plane drift time histos to a directory on FILE--------
      main_dir = out_file->mkdir("uncorr_plane_times");   
      main_dir->cd();
      
      for(Int_t ip=0; ip<NPLANES; ip++)
	{
	  //write histos to root file
	  plane_dt[ip].Write();
	}

     

   //------write corrected plane drift time histos to a directory on FILE--------
      main_dir = out_file->mkdir("corr_plane_times");   
      main_dir->cd();
      
      for(Int_t ip=0; ip<NPLANES; ip++)
	{
	  //write histos to root file
	  plane_dt_corr[ip].Write();
	}      
      
      
  //----------------------------------------------------------------------

      if (mode=="wire")
	{
  //--------write uncorrected cell drift times histos to FILE-------- 
   
  main_dir = out_file->mkdir("uncorr_wire_times");
  
    for (int ip=0; ip<NPLANES; ip++)
      {
	
	// create planes sub-directories to store wire drift times
	(main_dir->mkdir("plane "+plane_names[ip], ""))->cd();
	
	dt_vs_wire[ip].Write(); //write 2d drifttimet_vs_wire to FILE
	
	for (wire = 0; wire < nwires[ip]; wire++)
	  {
	    
	    cell_dt[ip][wire].Write();   //Write wire driftimes to FILE
	    
	  }
      
     } //END LOOP OVER PLANES

  //--------write corrected cell drift times histos to FILE-------- 
    
  main_dir = out_file->mkdir("corr_wire_times");
  
    for (int ip=0; ip<NPLANES; ip++)
      {
	
	// create planes sub-directories to store wire drift times
	(main_dir->mkdir("plane "+plane_names[ip], ""))->cd();
	
	dt_vs_wire_corr[ip].Write(); //write 2d drifttimet_vs_wire to FILE
	
	for (wire = 0; wire < nwires[ip]; wire++)
	  {
	    
	    cell_dt_corr[ip][wire].Write();   //Write wire driftimes to FILE
	    
	  }
      
      } //END LOOP OVER PLANES


 //------------------------------------------------------------------------

   //-------Write Fitted Wire Drift Time histos to FILE----------------------------
  

  main_dir = out_file->mkdir("fitted_wire_drift_times");
  
    for (int ip=0; ip<NPLANES; ip++)
      {
	
	// create planes sub-directories to store fitted wire drift times
	(main_dir->mkdir("plane "+plane_names[ip], ""))->cd();
		
	for (wire = 0; wire < nwires[ip]; wire++)
	  {
	    fitted_cell_dt[ip][wire].Write();   //Write wire driftimes to FILE
	    
	  }
      
      } //END LOOP OVER PLANES
  
    //-----Write 'tzero' values to a TEXT FILE--------------------
    
    //open a text FILE to write
    for (int ip = 0; ip < NPLANES; ip++) 
      {

	otxtfile_name = Form("./%s_DC_%sLog_%d/t_zero_values_%s.dat", spec.c_str(), mode.c_str(), run_NUM, planes[ip].c_str());
	out_txtFILE.open(otxtfile_name);
	out_txtFILE << "#Plane_" + plane_names[ip] << endl;
	out_txtFILE << "#Wire " << setw(12) << "tzero " << setw(12) << "t_zero_err " << setw(12) << "entries" << endl;
	
	for (wire = 0; wire < nwires[ip]; wire++) 
	  {
	    out_txtFILE << wire+1 << "    " << t_zero[ip][wire] << "     " << t_zero_err[ip][wire] << "             " << entries[ip][wire] << endl;
	  }
	
	out_txtFILE.close();
      
      }
     
//------------------------------------------------------------------------
    
    main_dir = out_file->mkdir("t0_vs_wire");
    main_dir->cd();
    
    for (int ip=0; ip<NPLANES; ip++)
      {
	
	//fit_tzero[ip] = new TF1(Form("fit%d", ip), "pol0", 1, nwires[ip]);
	gr1_canv = new TCanvas("gr1", "", 2000, 500);
	gr1_canv->SetGrid();
	//write TGraph: tzero v. wire number to root file
	itxtfile_name =  "./"+spec+"_DC_"+mode.c_str()+"Log_"+ std::to_string(run_NUM) +"/"+"t_zero_values_"+plane_names[ip]+".dat";
	graph = new TGraphErrors(itxtfile_name, "%lg %lg %lg");
	graph->SetName("graph");
	
	graph_title = "DC "+plane_names[ip]+": t0 vs. Wire";
	graph->SetTitle(graph_title);
	
	graph->SetMarkerStyle(20);
	graph->SetMarkerColor(1);
	
	graph->GetXaxis()->SetLimits(0., nwires[ip]);
	graph->GetXaxis()->SetTitle("Wire Number");
	graph->GetXaxis()->CenterTitle();
	graph->GetYaxis()->SetTitle("t-Zero (ns)");
	graph->GetYaxis()->CenterTitle();
	graph->GetYaxis()->SetRangeUser(-50.0, 50.0);
	
	graph->Draw("AP");
	gr1_canv->Update();
	gr1_canv->Write(graph_title);   //write to a root file
  
	  }

 
	} //end "wire mode" 

      if (mode=="card")
	{
	  //--------write uncorrected CARD drift times histos to FILE--------
	  
	  main_dir = out_file->mkdir("uncorr_card_times");
	  
	  for (int ip=0; ip<NPLANES; ip++)
	    {
	      
	// create planes sub-directories to store card drift times
	(main_dir->mkdir("plane "+plane_names[ip], ""))->cd();
	
	dt_vs_wire[ip].Write(); //write 2d drifttimet_vs_wire to FILE
	
	for (card = 0; card < plane_cards[ip]; card++)
	  {
	    
	    card_hist[ip][card].Write();   //Write wire driftimes to FILE
	    
	  } //end card loop

	    }//end plane loop

	  

    //--------write corrected CARD drift times histos to FILE--------
   
	  main_dir = out_file->mkdir("corr_card_times");
	  
	  for (int ip=0; ip<NPLANES; ip++)
	    {
	      
	      // create planes sub-directories to store card drift times
	      (main_dir->mkdir("plane "+plane_names[ip], ""))->cd();
	      
	      dt_vs_wire_corr[ip].Write(); //write 2d drifttimet_vs_wire to FILE
	      
	      for (card = 0; card < plane_cards[ip]; card++)
		{
		  
		  corr_card_hist[ip][card].Write();   //Write wire driftimes to FILE
		  
		} //end card loop
	      
	    } //end plane loop
	  
	  
	  //-------Write Fitted Card Drift Time histos to FILE----------------------------
  

	  main_dir = out_file->mkdir("fitted_card_drift_times");
	  
	  for (int ip=0; ip<NPLANES; ip++)
	    {
	      
	      // create planes sub-directories to store fitted wire drift times
	      (main_dir->mkdir("plane "+plane_names[ip], ""))->cd();
	      
	      for (card = 0; card < plane_cards[ip]; card++)
		{
		  fitted_card_hist[ip][card].Write();   //Write card driftimes to FILE
		  
		}
	      
	    } //end plane loop
	  
	  //-----Write 'tzero-per-card' values to a TEXT FILE--------------------
	  
	  //open a text FILE to write
	  
	  for (int ip = 0; ip < NPLANES; ip++) 
	    {
	      otxtfile_name = "./"+spec+"_DC_"+mode.c_str()+"Log_"+ std::to_string(run_NUM) +"/"+"t_zeroCARD_values_"+plane_names[ip]+".dat";
	      out_txtFILE.open(otxtfile_name);
	      out_txtFILE << "#Plane_" + plane_names[ip] << endl;
	      out_txtFILE << "#Card " << setw(12) << "tzero " << setw(12) << "t_zero_err " << setw(12) << "entries" << endl;
	      
	      for (card = 0; card < plane_cards[ip]; card++) 
		{
		  out_txtFILE << card << "    " << t_zero_card[ip][card] << "     " << t_zero_card_err[ip][card] << "             " << entries_card[ip][card] << endl;
		} //end card loop
	      
	      out_txtFILE.close();
	      
	    } //end plane loop
	  

	} //end "card mode"
      
    } //END DEBUG   
  

} //end WriteToFile() method
  
