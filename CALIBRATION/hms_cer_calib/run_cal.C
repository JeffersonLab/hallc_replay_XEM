#include <TProof.h>
#include <iostream>
#include <fstream>
#include <string>
#include <stdio.h>

void run_cal(Int_t RunNumber = 0, Int_t NumEvents = 0, Int_t coin = 0)
{
  if (RunNumber == 0) {
    cout << "Enter a Run Number (-1 to exit): ";
    cin >> RunNumber;
    if (RunNumber <= 0) return;
  }
  if (NumEvents == 0) {
    cout << "\nNumber of Events to analyze: ";
    cin >> NumEvents;
  }
  if (coin == 0) {
    cout << "\nIf this is a coincident run enter 1: ";
    cin >> coin;
  }

  cin.ignore(numeric_limits<streamsize>::max(), '\n');

  string calib_raw;
  cout << "\nEnter options for calibration  (enter NA to skip): ";
  getline(std::cin, calib_raw);
  TString calib_option = calib_raw;

  cout << "\n\n";

  TChain ch("T");
  if (coin == 1) ch.Add(Form("../../ROOTfiles/coin_replay_production_%d_%d.root", RunNumber, NumEvents));
  else ch.Add(Form("../../ROOTfiles/hms_replay_production_all_%d_%d.root", RunNumber, NumEvents));
  TProof *proof = TProof::Open("");
  //proof->SetProgressDialog(0);  
  ch.SetProof();

  if (calib_option != "NA") {
    //Start calibration process
    ch.Process("calibration.C+",calib_option);

    cout << "\n\nUpdate calibration constants with the better estimate (y/n)? ";
      
    TString user_input;
    cin >> user_input;
    if (user_input == "y") {
      ifstream temp;
      temp.open("calibration_temp.txt", ios::in);
      if (temp.is_open()) {
	rename("calibration_temp.txt", Form("../../PARAM/HMS/CER/hcer_calib_%d.param", RunNumber));
      }

      else cout << "Error opening calibration constants, may have to update constants manually!" << endl;
	   
    }

    else {
      remove("calibration_temp.txt"); 
    }
  }
}
