//SHMS DC Calibration Class
#ifndef DC_CALIB_H
#define DC_CALIB_H

#define NPLANES 12
#define NBINS 400
#define MINBIN -50.5
#define MAXBIN 349.5
#define TOTAL_BINS 274  
class DC_calib
{
 public:
  
  //consructor and destructor
  DC_calib(TString a, TString b, const Int_t c, Long64_t d);
  ~DC_calib();


  //Define Functions
  void printInitVar();
  void SetPlaneNames();
  void GetDCLeafs();
  void AllocateDynamicArrays();
  void CreateHistoNames();
  void EventLoop();
  void WriteToFile(Int_t debug);
  void CalcT0Historical();
  void Calculate_tZero();
  void GetTwentyPercent_Peak();
  void FitWireDriftTime();
  void WriteTZeroParam();
  void ApplyTZeroCorrection();
  void WriteLookUpTable();


 private:
 
  Int_t run_NUM;
  Long64_t num_evts;
 

  TTree *tree;
  Long64_t nentries;
  
  TString SPECTROMETER;
  TString spectre;
  TString spec;
  TString DETECTOR;
  TString plane_names[NPLANES];

  TString base_name;
  TString ndatatime;
  TString ndatawirenum;
  
  TString drifttime;
  TString wirenum;

  TString ntrack;
  TString etracknorm; 

  Double_t dc_ntrack;
  Double_t psh_etracknorm;

  Int_t wire;
  
  Int_t ndata_time[NPLANES];
  Double_t drift_time[NPLANES][1000];

  Int_t ndata_wirenum[NPLANES];
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
  
  //declare variables to make plot of tzero v. wire number
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



};










#endif  //DC_CALIBRATION_H
