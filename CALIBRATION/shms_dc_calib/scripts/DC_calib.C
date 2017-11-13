//SHMS DC Calibration: Implementation
#include<iostream>
#include "DC_calib.h"

using namespace std;

//_____________________________________________________________
DC_calib::DC_calib(TString a, TString b, const int c, Long64_t d)

  :spec(a),          //set spectrometer to 'HMS', or 'SHMS'  ex. DC_Calib(HMS, pdc_replay.C, 488, 50000)
   ifile_name(b),    //initialization list
   run_NUM(c),
   num_evts(d)
   
{
  //Initialize pointers
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

  entries                = NULL;
  t_zero                 = NULL;
  t_zero_err             = NULL;
  bin_max                = NULL;
  bin_maxContent         = NULL;
  time_max               = NULL;
  twenty_perc_maxContent = NULL;
  ref_time               = NULL;
  tZero_fit              = NULL;

  graph                  = NULL;
  gr1_canv               = NULL;
  


  
}


//____________________________________________________________
DC_calib::~DC_calib()
{
  cout << "calling the destructor " << endl;  
  
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
	delete [] cell_dt[ip]; 
	delete [] cell_dt_corr[ip]; 
	delete [] fitted_cell_dt[ip];
	delete [] bin_max[ip]; 
	delete [] bin_maxContent[ip]; 
	delete [] time_max[ip]; 
	delete [] twenty_perc_maxContent[ip]; 
	delete [] ref_time[ip];  
      }  
    
    delete [] entries;                     entries                = NULL;
    delete [] t_zero;                      t_zero                 = NULL;
    delete [] t_zero_err;                  t_zero_err             = NULL;
    delete [] cell_dt;                     cell_dt                = NULL;
    delete [] cell_dt_corr;                cell_dt_corr                = NULL;
    delete [] fitted_cell_dt;              fitted_cell_dt         = NULL;
    delete [] bin_max;                     bin_max                = NULL;
    delete [] bin_maxContent;              bin_maxContent         = NULL;
    delete [] time_max;                    time_max               = NULL;
    delete [] twenty_perc_maxContent;      twenty_perc_maxContent = NULL;
    delete [] ref_time;                    ref_time               = NULL;
  
}

//____________________________________________________________
void DC_calib::printInitVar()
{
  cout << "Initialization variables: \n"
    "Input File: " << ifile_name << "\n"
    "Run #: " << run_NUM << "\n"
    "Events: " << num_evts << endl;
}



//___________________________________________________________
void DC_calib::SetPlaneNames()
{
  
  DETECTOR = "dc";
 
  //initialize DC plane names
  if(spec=="SHMS")
    {
      SPECTROMETER = "P";
      spectre = "p";
   
      plane_names[0]="1u1",  nwires[0] = 107;  
      plane_names[1]="1u2",  nwires[1] = 107;
      plane_names[2]="1x1",  nwires[2] = 79;
      plane_names[3]="1x2",  nwires[3] = 79; 
      plane_names[4]="1v1",  nwires[4] = 107;
      plane_names[5]="1v2",  nwires[5] = 107;
      plane_names[6]="2v2",  nwires[6] = 107;
      plane_names[7]="2v1",  nwires[7] = 107;
      plane_names[8]="2x2",  nwires[8] = 79;
      plane_names[9]="2x1",  nwires[9] = 79;
      plane_names[10]="2u2", nwires[10] = 107;
      plane_names[11]="2u1", nwires[11] = 107;
   
    }
      
  
  else if(spec=="HMS")
    {
        
      SPECTROMETER = "H";
      spectre="h";
      
      plane_names[0]="1u1",  nwires[0] = 96;  
      plane_names[1]="1u2",  nwires[1] = 96;
      plane_names[2]="1x1",  nwires[2] = 102;
      plane_names[3]="1x2",  nwires[3] = 102; 
      plane_names[4]="1v2",  nwires[4] = 96;
      plane_names[5]="1v1",  nwires[5] = 96;
      plane_names[6]="2v1",  nwires[6] = 96;
      plane_names[7]="2v2",  nwires[7] = 96;
      plane_names[8]="2x2",  nwires[8] = 102;
      plane_names[9]="2x1",  nwires[9] = 102;
      plane_names[10]="2u2", nwires[10] = 96;
      plane_names[11]="2u1", nwires[11] = 96;
    }
  
}


//____________________________________________________________________________________

void DC_calib::GetDCLeafs()
{
  cout << "DC_calib::GetDCLeafs " << endl;

  //open input root file
  in_file = new TFile(ifile_name, "READ" );
  
  //Get the tree
  tree = (TTree*)in_file->Get("T");


  //Loop over each plane
  for (int ip = 0; ip < NPLANES; ip++)
    {
      
      base_name = SPECTROMETER+"."+DETECTOR+"."+plane_names[ip];
      ndatatime = "Ndata."+base_name+".time";
      ndatawirenum = "Ndata."+base_name+".wirenum";
      drifttime = base_name+".time";
      wirenum = base_name+".wirenum";
      
      //Set Branch Address 
      tree->SetBranchAddress(wirenum, wire_num[ip]);   
      tree->SetBranchAddress(drifttime, drift_time[ip]);   
      tree->SetBranchAddress(ndatawirenum, &ndata_wirenum[ip]);
      tree->SetBranchAddress(ndatatime, &ndata_time[ip]);   
						     
    }

  ntrack = SPECTROMETER + "." + DETECTOR + ".ntrack";
  //  etracknorm = SPECTROMETER + ".cal.etracknorm"; 
 
  tree->SetBranchAddress(ntrack, &dc_ntrack);
  //tree->SetBranchAddress(etracknorm, &psh_etracknorm);   

 
}

void DC_calib::AllocateDynamicArrays()
{
 

  //Allocate 1D dynamic arrays
  plane_dt      = new TH1F[NPLANES];       //create plane drift time histo 1Darray ( get_pdc_time_histo.C )
  plane_dt_corr = new TH1F[NPLANES];      //create plane drift times to store after applying tzero correction
  dt_vs_wire    = new TH2F[NPLANES];     //create wire drift time and 'drifttime vs wire' arrays
  dt_vs_wire_corr = new TH2F[NPLANES];

  //Allocate 2D dynamic arrays
  entries                 = new Int_t*[NPLANES];
  t_zero                  = new Double_t*[NPLANES];
  t_zero_err              = new Double_t*[NPLANES];
  cell_dt                 = new TH1F*[NPLANES];     /*create array to store cell drift times*/    
  cell_dt_corr            = new TH1F*[NPLANES];
  fitted_cell_dt          = new TH1F*[NPLANES];     /*create array to store cell drift times*/    
  bin_max                 = new Int_t*[NPLANES];    /*Array to store the bin number corresponding to the drift time distribution peak*/
  bin_maxContent          = new Int_t*[NPLANES];    /*Array to store the content (# events) corresponding to the bin with maximum content*/
  time_max                = new Double_t*[NPLANES]; /*Array to store the x-axis(drift time (ns)) corresponding to bin_max*/
  twenty_perc_maxContent  = new Double_t*[NPLANES]; /*Array to store 20% of maximum bin content (peak)*/						     
  ref_time                = new Double_t*[NPLANES]; /*Array to store ref_time(time corresp. to 20% of peak) times for each sense wire*/                  
  
 
  for(int ip=0; ip<NPLANES; ip++)
    {
      entries[ip]                 = new Int_t[nwires[ip]]; 
      t_zero[ip]                  = new Double_t[nwires[ip]];
      t_zero_err[ip]              = new Double_t[nwires[ip]];
      cell_dt[ip]                 = new TH1F[nwires[ip]];
      cell_dt_corr[ip]            = new TH1F[nwires[ip]];
      fitted_cell_dt[ip]          = new TH1F[nwires[ip]];
      bin_max[ip]                 = new Int_t[nwires[ip]];                   
      bin_maxContent[ip]          = new Int_t[nwires[ip]];              
      time_max[ip]                = new Double_t[nwires[ip]];                 
      twenty_perc_maxContent[ip]  = new Double_t[nwires[ip]];   						     
      ref_time[ip]                = new Double_t[nwires[ip]];                    
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
	  fitted_cell_dt[ip][wire].SetBins(NBINS, MINBIN, MAXBIN);
	  fitted_cell_dt[ip][wire].SetXTitle("Drift Time (ns)");
	  fitted_cell_dt[ip][wire].SetYTitle("Number of Entries / 1 ns");

	  cell_dt_corr[ip][wire].SetName(cell_dt_name);
	  cell_dt_corr[ip][wire].SetTitle(cell_dt_title);
	  cell_dt_corr[ip][wire].SetBins(NBINS, MINBIN, MAXBIN);
	  cell_dt_corr[ip][wire].SetXTitle("Drift Time (ns)");
	  cell_dt_corr[ip][wire].SetYTitle("Number of Entries / 1 ns");

	}
      
    }

}
  

//________________________________________________________________
void DC_calib::EventLoop()
{


  //Loop over all entries
  for(Long64_t i=0; i<num_evts; i++)
    {
      //cout << "entry: " << i << endl;
      tree->GetEntry(i);  

      for(Int_t ip=0; ip<NPLANES; ip++)
	{
	  // cout << "PLANE: " << ip << endl;
      
//-----------------------------------------------------------------------------------------	  
	 
	  //Loop over number of hits for each trigger in each DC plane 
	  for(Int_t j = 0, k = 0; j < ndata_time[ip], k < ndata_wirenum[ip]; j++, k++)    
	    {
	

	      //get wire hit for ith event in 'ip' plane
		wire = int(wire_num[ip][k]);
	      
		//	if (dc_ntrack == 1 && psh_etracknorm > 0.6) {
		//Fill uncorrected plane drift times  (from: get_pdc_time_histo.C )
		plane_dt[ip].Fill(drift_time[ip][j]); 
		
		
	
		//cout << "wire hit: " << wire << endl;
		//Fill uncorrected wire (cell) drift times and dt-vs-wire Histograms
		dt_vs_wire[ip].Fill(wire_num[ip][k], drift_time[ip][j]);
		cell_dt[ip][wire-1].Fill(drift_time[ip][j]);
		fitted_cell_dt[ip][wire-1].Fill(drift_time[ip][j]);
		//  cout << "drift time: " << drift_time[ip][j] << endl;
		
		//} // end cut
	
	   
//-----------------------------------------------------------------------------------------
	      
	    } 
      
	
      
	
	} //end loop over planes
    
    } //end loop over events

}


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
	    
	    //Use the 20% to find the t0 offset
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
	  twenty_perc_maxContent[ip][wire] = bin_maxContent[ip][wire] * 0.20;	                      
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
	  
	  
	}
      
    }
 
}


//____________________________________________________________________________________
void DC_calib::FitWireDriftTime()
{

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
	  if (abs(-y_int/m) < std_dev*5.0 && m > 0.0 && entries[ip][wire]>100)
	    {
	      t_zero[ip][wire] = - y_int/m ;
	      t_zero_err[ip][wire] = sqrt(y_int_err*y_int_err/(m*m) + y_int*y_int*m_err*m_err/(m*m*m*m) );
	      
	    }
	  else // (entries[ip][wire] < 1000 || m < 0.0)
	    {
	      
	      t_zero[ip][wire] = 0.0;   
	      t_zero_err[ip][wire] = 0.0;
	      
	    }


	} //END LOOP OVER WIRES
      
      
    }//END LOOP OVER PLANES  

  
}


//________________________________________________________________
void DC_calib::Calculate_tZero()
{
  
  //CalcT0Historical();

  GetTwentyPercent_Peak();
  FitWireDriftTime();
}


//________________________________________________________________
void DC_calib::WriteToFile(Int_t debug = 0)
{

  
  //create output ROOT file to write UnCALIB./CALIB. histos
  ofile_name = spec+"_DC_driftimes.root";
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
	otxtfile_name = "t_zero_values_"+plane_names[ip]+".dat";
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

	gr1_canv = new TCanvas("gr1", "", 2000, 500);
	gr1_canv->SetGrid();
	//write TGraph: tzero v. wire number to root file
	itxtfile_name = "t_zero_values_"+plane_names[ip]+".dat";
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
 
    } //END DEBUG   


}
  

//__________________________________________________________________________
void DC_calib::WriteTZeroParam()
{
  otxtfile_name =  "./"+spectre+"dc_tzero_per_wire_"+std::to_string(run_NUM)+".param";
  out_txtFILE.open(otxtfile_name);
  
  for (int ip=0; ip<NPLANES; ip++) { 
	  
    //write plane headers
    out_txtFILE << spectre+"tzero"+plane_names[ip] << "=" << endl;

   for (wire=0; wire<nwires[ip]; wire++) 
     {
       if (wire <= 10) 
	 { 
	   out_txtFILE << setprecision(6) << t_zero[ip][wire] << fixed << ",";
	 }
       else if (wire>10 && wire <(nwires[ip]-1))
	 {
	   out_txtFILE << setprecision(6) << t_zero[ip][wire] << ((wire+1) % 16 ? ", " : "\n") << fixed;
	 }
       else if (wire==nwires[ip]-1) 
	 {
	   out_txtFILE << setprecision(6) << t_zero[ip][wire] << fixed << endl;
	 }
       
     } //END LOOP OVER WIRES

  } //END LOOP OVER PLANES
  
  out_txtFILE.close();
  
}



//_______________________________________________________________________
void DC_calib::ApplyTZeroCorrection()
{
  
 
  cout << "ApplyT0Corr  "<< endl;

  //Loop over all entries
  for(Long64_t i=0; i<num_evts; i++)
    {
      tree->GetEntry(i);      
      for(Int_t ip=0; ip<NPLANES; ip++)
	{
	   
	  //Loop over number of hits for each trigger in each DC plane 
	  
	  for(Int_t j = 0, k = 0; j < ndata_time[ip], k < ndata_wirenum[ip]; j++, k++)    
	    {
	      //get wire hit for ith event in 'ip' plane
	      wire = int(wire_num[ip][k]);

	      //if (dc_ntrack == 1 && psh_etracknorm > 0.6) {
		
	      //Fill corrected plane drift times 
		plane_dt_corr[ip].Fill(drift_time[ip][j] - t_zero[ip][wire-1]); 
		cell_dt_corr[ip][wire-1].Fill(drift_time[ip][j] - t_zero[ip][wire-1]);
		dt_vs_wire_corr[ip].Fill(wire_num[ip][k], drift_time[ip][j] - t_zero[ip][wire-1]);
		//} //end cut
		
	    } //end loop over hits
	  
	    
	} //end loop over planes
      
    } //end loop over events
}

//_________________________________________________________________________________
void DC_calib::WriteLookUpTable()
{
  otxtfile_name = "./"+spectre+"dc_calib_"+std::to_string(run_NUM)+".param";
  out_txtFILE.open(otxtfile_name);
  
  //Set headers for subsequent columns of data
  out_txtFILE << Form("; Lookup Table: RUN %d", run_NUM) << "\n";
  out_txtFILE << "; number of bins in time to distance lookup table" << "\n";
  out_txtFILE << Form(spectre+"driftbins = %d", TOTAL_BINS+1) << "\n";
  out_txtFILE << "; number of 1st bin in table in ns" << "\n";
  out_txtFILE << spectre+"drift1stbin=0" << "\n";
  out_txtFILE << "; bin size in ns" << "\n";
  out_txtFILE << spectre+"driftbinsz=1" << "\n";
  
//Loop over each plane of hms/shms Drift Chambers (DC1 & DC2)

  for (int ip=0; ip<NPLANES; ip++){
   

    //Get bin corresponding to t0 = 0 ns
    bin_t0[ip] = plane_dt_corr[ip].GetXaxis()->FindBin(0.0);
   
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

}
