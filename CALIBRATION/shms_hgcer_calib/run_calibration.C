//Script to run over all SHMS HGCER calibration scripts ar once and update the necessary parameter files

void run_calibration()
{

  //User Input Run
  Int_t run_NUM;
  cout << "Enter run number: ";
  cin >> run_NUM;

  Int_t events_NUM;
  cout << "Enter number of events: ";
  cin >> events_NUM;

  //Create root and data file directories if they don't exist
  gROOT->ProcessLine(".! mkdir root_files");
  gROOT->ProcessLine(".! mkdir data_files");

  //Execute histogram creator & cutting script
  gROOT->ProcessLine(Form(".x scripts/analysis.C(%d, %d)", run_NUM, events_NUM));

  //cout << "I got here!" << endl;

  //Take those histograms and determine the calibration constants
  gROOT->ProcessLine(Form(".x scripts/calib_const.C(%d, %d)", run_NUM, events_NUM));

  //Take calibration constants and put into parameter file
  ofstream output;
  output.open(Form("data_files/phgcer.param.%d",run_NUM),ios::out);
  output << "; Calibration constants for run" << run_NUM << endl;
  output << endl;

  Double_t run_constants[4];
  cout << "To choose correct calibration constant, verify which plot is more linear." << endl;
  cout << "Gain constant for PMT1: ";
  cin >> run_constants[0];
  cout << endl << "Gain constant for PMT2: ";
  cin >> run_constants[1];
  cout << endl << "Gain constant for PMT3: ";
  cin >> run_constants[2];
  cout << endl << "Gain constant for PMT4: ";
  cin >> run_constants[3];

  output << "phgcer_gain_calib =" << setw(3) << setprecision(3) << run_constants[0] << "," << endl;
  for (Int_t i=1; i < 4; i++)
    {
      output << setw(22) << setprecision(3) << run_constants[i] << ",";
      output << endl;
    }

  output.close();
  cout << "Something went wrong? Consult the README!" << endl;
}
