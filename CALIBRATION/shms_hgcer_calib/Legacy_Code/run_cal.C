#include <TProof.h>
#include <iostream>
#include <fstream>
#include <string>
#include <stdio.h>

void run_cal(Int_t RunNumber1 = 0, Int_t NumEvents = 0, Int_t coin = 0)
{
  if (RunNumber1 == 0)
    {
      cout << "Enter a Run Number (-1 to exit): ";
      cin >> RunNumber1;
      if (RunNumber1 <= 0) return;
    } 
  /* if (RunNumber2 == 0)
    {
      cout << "Enter a Run Number (-1 to exit): ";
      cin >> RunNumber2;
      if (RunNumber2 <= 0) return;
    }
  if (RunNumber3 == 0)
    {
      cout << "Enter a Run Number (-1 to exit): ";
      cin >> RunNumber3;
      if (RunNumber3 <= 0) return;
    }
    if (RunNumber4 == 0)
    {
      cout << "Enter a Run Number (-1 to exit): ";
      cin >> RunNumber4;
      if (RunNumber4 <= 0) return;
      }*/
  if (NumEvents == 0)
    {
      cout << "\nNumber of Events to analyze for all runs: ";
      cin >> NumEvents;
    }
  if (coin == 0)
    {
      cout << "\nIf this is a coincident run enter 1: ";
      cin >> coin;
    }

  cin.ignore(numeric_limits<streamsize>::max(), '\n');

  string calib_raw;
  cout << "\nEnter options for calibration  (enter NA to skip): ";
  getline(std::cin, calib_raw);
  TString calib_option = calib_raw;
  
  /*  string eff_raw;
      cout << "\nEnter options for efficiency (enter NA to skip): ";
      getline(std::cin, eff_raw);
      TString eff_option = eff_raw;

      cout << "\n\n"; */
  
  TChain ch("T");
  if (coin == 1){ch.Add(Form("../../ROOTfiles/shms_coin_replay_production_all_%d_%d.root",RunNumber1, NumEvents));}
    //ch.Add(Form("../../ROOTfiles/shms_coin_replay_production_all_%d_%d.root", RunNumber2, NumEvents)); 
    // ch.Add(Form("../../ROOTfiles/shms_coin_replay_production_all_%d_%d.root",RunNumber3, NumEvents));}  
  //ch.Add(Form("../../ROOTfiles/shms_coin_replay_production_all_%d_%d.root",RunNumber4, NumEvents));}  

  else ch.Add(Form("ROOTfiles/shms_replay_production_all_%d_%d.root", RunNumber1, NumEvents));
  TProof *proof = TProof::Open("workers=4");
  proof->SetProgressDialog(0);  
  ch.SetProof();

  if (calib_option != "NA")
    {
      //Start calibration process
      ch.Process("calibration.C",calib_option);
    
      cout << "\n\nUpdate calibration constants with the better estimate (y/n)? ";
      
      TString user_input;
      cin >> user_input;
      if (user_input == "y")
      	{
	  ifstream temp;
	  temp.open("calibration_temp.txt", ios::in);
	  if (temp.is_open())
	    {  
	      //if (calib_option.Contains("NGC")) rename("calibration_temp.txt", Form("../../PARAM/SHMS/NGCER/CALIB/pngcer_calib_%d.param", RunNumber1));
	      rename("calibration_temp.txt", Form("Calibration_plots/phgcer_calib%d.param", RunNumber1));
	      rename("calibration_temp1.txt", Form("Calibration_plots/phgcer_calib_err%d.param", RunNumber1));

	      // else rename("calibration_temp.txt", Form("../../PARAM/SHMS/HGCER/CALIB/phgcer_calib_%d.param", RunNumber));
	    
	      rename("Calibration_plots/Calibration_plots.pdf", Form("Calibration_plots/Calibration_plots%d.pdf", RunNumber1));
	    }
	  else cout << "Error opening calibration constants, may have to update constants manually!" << endl;
	    
	   
	}
    }

}
