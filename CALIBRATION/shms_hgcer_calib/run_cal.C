#include <TProof.h>
#include <iostream>
#include <fstream>
#include <string>
#include <stdio.h>

void run_cal(Int_t RunNumber = 0, Int_t NumEvents = 0)
{
  if (RunNumber == 0)
    {
      cout << "Enter a Run Number (-1 to exit): ";
      cin >> RunNumber;
      if (RunNumber <= 0) return;
    }
  if (NumEvents == 0)
    {
      cout << "\nNumber of Events to analyze: ";
      cin >> NumEvents;
      }

  cin.ignore(numeric_limits<streamsize>::max(), '\n');

  string calib_raw;
  cout << "\nEnter options for calibration  (enter NA to skip): ";
  getline(std::cin, calib_raw);
  TString calib_option = calib_raw;
  
  string eff_raw;
  cout << "\nEnter options for efficiency (enter NA to skip): ";
  getline(std::cin, eff_raw);
  TString eff_option = eff_raw;

  TChain ch("T");
  ch.Add(Form("../../ROOTfiles/shms_replay_production_%d_%d.root", RunNumber, NumEvents));
  TProof *proof = TProof::Open("");
  proof->SetProgressDialog(0);  
  ch.SetProof();

  if (calib_option != "NA")
    {
      ch.Process("calibration.C+",calib_option);

      cout << "\n\nUpdate calibration constants with the better estimate (y/n)? ";
      
      TString user_input;
      cin >> user_input;
      if (user_input == "y")
	{
	  ifstream temp;
	  temp.open("calibration_temp.txt", ios::in);
	  if (temp.is_open())
	    {
	      if (calib_option.Contains("NGC")) rename("calibration_temp.txt", Form("../../PARAM/SHMS/NGCER/CALIB/pngcer_calib_%d.param", RunNumber));

	      else rename("calibration_temp.txt", Form("../../PARAM/SHMS/HGCER/CALIB/phgcer_calib_%d.param", RunNumber));
	    }

	  else cout << "Error opening calibration constants, may have to update constants manually!" << endl;
	   
	}

      else
	{
	  remove("calibration_temp.txt"); 
	}
    }

  if (eff_option != "NA") ch.Process("efficiencies.C+",eff_option);
}
