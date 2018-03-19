//SHMS DC Calibration Class
#ifndef DC_CALIB_H
#define DC_CALIB_H

#define NPLANES 12
#define NBINS 400
#define MINBIN -50.0
#define MAXBIN 350.0
#define TOTAL_BINS 189  
class DC_calib
{
 public:
  
  //consructor and destructor
  DC_calib(string a, TString b, const Int_t c, Long64_t d, TString e, string f);
  ~DC_calib();


  //Define Functions


  //----Per-Card Methods
  void GetCard();       
  void GetTwentyPercent_Card();  
  void FitCardDriftTime();
  void ApplyTZeroCorrectionPerCard(); 
  Double_t GetCardT0_alternative(Int_t ith_plane, Int_t ith_card);

  //---Per Global/Per Wire methods 
  void setup_Directory();
  void SetPlaneNames();
  void SetTdcOffset();
  void GetDCLeafs();
  void AllocateDynamicArrays();
  void CreateHistoNames();
  void EventLoop(string option);
  void WriteToFile(Int_t debug);
  // void CalcT0Historical();
  void Calculate_tZero();
  void GetTwentyPercent_Peak();
  void FitWireDriftTime();
  void WriteTZeroParam();
  void WriteLookUpTable();


 private:

  //Calibration mode
  string mode;
 
  Int_t run_NUM;
  Long64_t num_evts;
 
  TString pid;

  TTree *tree;
  Long64_t nentries;
  
  TString SPECTROMETER;
  TString spectre;
  string spec;
  TString DETECTOR;
  TString plane_names[NPLANES];
  string planes[NPLANES];
  
  TString base_name;
  TString ndatatime;
  
  TString drifttime;
  TString wirenum;

  TString cal_etot_leaf;   
  TString cer_npe_leaf;
 
  Double_t cal_etot;   //calorimeter normalized energy
  Double_t cer_npe;       //cerenkon photoelectron Sum

  //Boolean for checking if TBranch exists
  Bool_t status_cal;
  Bool_t status_cer;
  

  //Boolean for PID cuts
  Bool_t cal_elec;     //calorimeter normalized energy cut
  Bool_t cer_elec;     //cerenkov cut
  Bool_t good_event;    //single hit / event / plane o clean background
 
  Int_t cnts_ch1;
  Int_t cnts_ch2;
  Int_t ngood_evts;

  //Variables for setting up a run_directory to keep track of calibration files
  const char* dir_log;
  const char* dir_log_name;
  
  Int_t wire;
  
  Int_t ndata_time[NPLANES];
  Double_t drift_time[NPLANES][1000];

  Double_t wire_num[NPLANES][1000];

  Int_t nwires[NPLANES];


  //Declare variables to plot and save histo (dt = drift time)
  TString plane_dt_name;
  TString plane_dt_title;

  TString plane_dt_name_corr;
  TString plane_dt_title_corr;


  TString cell_dt_name;
  TString cell_dt_title;

  TString fitted_cell_dt_name;
  TString fitted_cell_dt_title;  
  
  TString dt_vs_wire_name;
  TString dt_vs_wire_title;

  TString ifile_name;
  TString ofile_name;
  
  TString itxtfile_name;
  TString otxtfile_name;  

  //Declare variables to store histograms
  TH1F *plane_dt;
  TH1F *plane_dt_corr;
  TH1F **cell_dt;
  TH1F **cell_dt_corr;
  TH1F **fitted_cell_dt;
  TH2F *dt_vs_wire;
  TH2F *dt_vs_wire_corr;

  //Read/Write ROOT files
  TFile *in_file;
  TFile *out_file;
  
  //Read/Write txt files
  ifstream in_txtFILE;
  ofstream out_txtFILE;

  TDirectory *main_dir;


  //Declare 2d dynamic arrays to get histogram bin properties
  
  Int_t **bin_max;
  Int_t **bin_maxContent;
  Double_t **time_max;
  Double_t **twenty_perc_maxContent;
  Double_t **ref_time;
 
  //variables to be used in loop over bins for wire drift time
  Int_t content_bin;      //stores content for each bin
  Int_t counts;           //a counter used to count the number of bins that have >20% max bin content for a plane 

  vector<Int_t>  content;               //stores bin content
  vector <Int_t> bin_num;               //stores bin number
   

  //Declare 'FIT' related variables
  Int_t **entries;               //wire drift time histo entries
  Int_t **entries_card;
  Int_t binx;
  Double_t time_init;           //start fit value 
  Double_t time_final;          //end fit value
  TF1 *tZero_fit;               //linear fit function

 
  Double_t m;                //slope
  Double_t y_int;            //y-intercept
  Double_t m_err;
  Double_t y_int_err;
  Double_t std_dev;
  Double_t **t_zero;         
  Double_t **t_zero_err;
  Double_t **t_zero_card;         
  Double_t **t_zero_card_err;
  
  //tzero with tdc offsets taken into account, 
  //to be written into tzero param file
  Double_t **t_zero_final; 

  //set limits on tzero fit procedure
  Double_t percent;        //set % of drift_time max bin content to do fit
  Double_t t0_err_thrs;    //set max error on t0 fit, as criteria for 'good' t0

  //declare variables to make plot of tzero v. wire number
  
  Double_t weighted_avg[NPLANES];
  TGraphErrors *graph;
  TString graph_title;
  TCanvas *gr1_canv;

  //Declare varibales to write LoookUp Table
  Int_t bin_count;
  Int_t bin_t0[NPLANES];
  Int_t bin_final[NPLANES];
  Int_t bin_Content[NPLANES];
  Double_t binContent_TOTAL[NPLANES];
  Double_t binSUM[NPLANES];
  Double_t lookup_value[NPLANES];
  TString lookup_table;
  TString headers;

  //Declare variables to apply constant offset in time
  //HMS
  Double_t **offset;
  Double_t tdc_offset;
  Double_t max_wire_entry;

  //---Per Card "SPECIFIC" Variables---

  Int_t plane_cards[NPLANES];    //number of disc. cards / plane
  Int_t card;

  //GetCardT0_alternative() method variables
  Double_t maxContent_frac;
  Double_t card_T0;

  //GetTwentyPercent_Card()/Fit Card methods variables
  Int_t binValLow; 
  Int_t binValHigh; 
  Int_t binSearchLow; 
  Int_t binSearchHigh;
  Double_t binDiffThreshHigh; 
  Double_t binDiffThreshLow;

  Double_t **wireBinContentMax;
  Double_t **wireBinContentLow;
  Double_t **wireBinContentHigh;
  Double_t **wireBinHigh;
  Double_t **wireBinLow;
  Double_t **wireFitRangeLow;
  Double_t **wireFitRangeHigh;

  //TStrings for Histo Names
  TString card_hist_name;
  TString card_hist_title;

  TString fitted_card_hist_name;
  TString fitted_card_hist_title;
  
  TString corr_card_hist_name;
  TString corr_card_hist_title;


  //Card Histograms
  TH1F **card_hist; 
  TH1F **corr_card_hist;
  TH1F **fitted_card_hist;

  Int_t **wire_min;
  Int_t **wire_max;

};










#endif  //DC_CALIBRATION_H
