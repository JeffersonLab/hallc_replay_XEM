// Vijay Kumar, Univerity of Regina - 24/07/20
// vijay36361@gmail.com

#include <TProof.h>
#include <iostream>
#include <fstream>
#include <string>
#include <stdio.h>

// Expected input is, rootfile prefix, number of events per run, number of runs to chain (1 to 3) and run number(s) 
void run_cal(string RunPrefix = "", Int_t NumEvents = 0, Int_t nRuns = 0, Int_t RunNumber1 = 0, Int_t RunNumber2 = 0, Int_t RunNumber3 = 0, Int_t RunNumber4 = 0, Int_t RunNumber5 = 0, Int_t RunNumber6 = 0, Int_t RunNumber7 = 0, Int_t RunNumber8 = 0, Int_t RunNumber9 = 0, Int_t RunNumber10 = 0)
{
  TString Hostname = gSystem->HostName();
  TString User = (gSystem->GetUserInfo())->fUser;
  TString Rootpath;
  TString rootFileNameString1;
  TString rootFileNameString2;
  TString rootFileNameString3;
  TString rootFileNameString4;
  TString rootFileNameString5;
  TString rootFileNameString6;
  TString rootFileNameString7;
  TString rootFileNameString8;
  TString rootFileNameString9;
  TString rootFileNameString10;
  TString RunPref;
  
  cout << "Processing HGC calibration, expected input is, rootfile prefix, number of runs to chain (1 to 10), run number(s) and number of events per run" << endl;

  RunPref = RunPrefix;
  if(RunPref == "") 
    {
      cout << "Enter a Rootfile name prefix (Assumed format is PREFIX_RUN#_#EVENTS.root): ";
      cin >> RunPrefix;
      RunPref = RunPrefix;
    }
  if (NumEvents == 0)
    {
      cout << "\nNumber of Events to analyze for all runs: ";
      cin >> NumEvents;
    }
  if(nRuns == 0)
    {
      cout << "Enter the number of runs to chain (1 to 10): ";
      cin >> nRuns;
      if (nRuns != 1 && nRuns !=2 && nRuns != 3 && nRuns != 4 &&  nRuns != 5 && nRuns != 6 && nRuns != 7 && nRuns != 8 && nRuns != 9 && nRuns != 10){
      cerr << "...Invalid entry\n";
      cerr << "Enter integer between 1 and 10 \n";
      return;
      }   
    }
  if (RunNumber1 == 0)
    {
      cout << "Enter 1st run number : ";
      cin >> RunNumber1;
      if (RunNumber1 <= 0) return;
    }
  if(nRuns > 1)
    {
      if (RunNumber2 == 0)
	{
	  cout << "Enter 2nd run number : ";
	  cin >> RunNumber2;
	  if (RunNumber2 <= 0) return;
	}
    }
  if(nRuns > 2)
    {
      if (RunNumber3 == 0)
	{
	  cout << "Enter 3rd run number : ";
	  cin >> RunNumber3;
	  if (RunNumber3 <= 0) return;
	}
    }
  if(nRuns > 3)
    {
    if (RunNumber4 == 0)
      {
	cout << "Enter 4th run number : ";
	cin >> RunNumber4;
	if (RunNumber4 <= 0) return;
      }
    }
  if(nRuns > 4)
    {
      if (RunNumber5 == 0)
	{
	  cout << "Enter 5th run number : ";
	  cin >> RunNumber5;
	  if (RunNumber5 <= 0) return;
	}
    }

 if(nRuns > 5)
   {
     if (RunNumber6 == 0)
       {
	 cout << "Enter 6th run number : ";
	 cin >> RunNumber6;
	 if (RunNumber6 <= 0) return;
       }
   }
 if(nRuns > 6)
   {
     if (RunNumber7 == 0)
       {
	 cout << "Enter 7th run number : ";
	 cin >> RunNumber7;
	 if (RunNumber7 <= 0) return;
       }
   }
 
 if(nRuns > 7)
   {
     if (RunNumber8 == 0)
       {
	 cout << "Enter 8th run number : ";
	 cin >> RunNumber8;
	 if (RunNumber8 <= 0) return;
       }
   }
 
 if(nRuns > 8)
   {
    if (RunNumber9 == 0)
      {
	cout << "Enter 9th run number : ";
	cin >> RunNumber9;
	if (RunNumber9 <= 0) return;
      }
   }

 if(nRuns > 9)
   {
     if (RunNumber10 == 0)
       {
	 cout << "Enter 10th run number : ";
	 cin >> RunNumber10;
	 if (RunNumber10 <= 0) return;
       }
   }
 
  cin.ignore(numeric_limits<streamsize>::max(), '\n');
  
  // Change or add your own paths as needed!
  // This is where the script will look for the rootfiles to analyse
  if(Hostname.Contains("farm"))
    { 
      Rootpath = "/group/c-kaonlt/USERS/"+User+"/hallc_replay_lt/ROOTfiles/";
    }
  else if(Hostname.Contains("qcd"))
    {
    Rootpath = "/group/c-kaonlt/USERS/"+User+"/hallc_replay_lt/ROOTfiles/";
    }
  
  rootFileNameString1 = Rootpath + Form("%s_%i_%i.root", RunPrefix.c_str(), RunNumber1, NumEvents);
  // Check files exist
  if (gSystem->AccessPathName(rootFileNameString1) == kTRUE)
    {
      cerr << "!!!!! ERROR !!!!! " << endl << rootFileNameString1 <<  " not found" << endl <<  "!!!!! ERRROR !!!!!" << endl;
      return; 
    }
  if(nRuns > 1)
    {
      rootFileNameString2 = Rootpath + Form("%s_%i_%i.root", RunPrefix.c_str(), RunNumber2, NumEvents);
      if (gSystem->AccessPathName(rootFileNameString2) == kTRUE)
	{
	  cerr << "!!!!! ERROR !!!!! " << endl << rootFileNameString2 <<  " not found" << endl <<  "!!!!! ERRROR !!!!!" << endl;
	  return; 
	}
    }
  if(nRuns > 2)
    {
      rootFileNameString3 = Rootpath + Form("%s_%i_%i.root", RunPrefix.c_str(), RunNumber3, NumEvents);
      if (gSystem->AccessPathName(rootFileNameString3) == kTRUE)
	{
	  cerr << "!!!!! ERROR !!!!! " << endl << rootFileNameString3 <<  " not found" << endl <<  "!!!!! ERRROR !!!!!" << endl;
	  return; 
	}
    }
  if(nRuns > 3)
    {
      rootFileNameString4 = Rootpath + Form("%s_%i_%i.root", RunPrefix.c_str(), RunNumber4, NumEvents);
      if (gSystem->AccessPathName(rootFileNameString4) == kTRUE)
	{
	  cerr << "!!!!! ERROR !!!!! " << endl << rootFileNameString4 <<  " not found" << endl <<  "!!!!! ERRROR !!!!!" << endl;
	  return; 
	}
    }
  if(nRuns > 4)
    {
      rootFileNameString5 = Rootpath + Form("%s_%i_%i.root", RunPrefix.c_str(), RunNumber5, NumEvents);
      if (gSystem->AccessPathName(rootFileNameString5) == kTRUE)
	{
	  cerr << "!!!!! ERROR !!!!! " << endl << rootFileNameString5 <<  " not found" << endl <<  "!!!!! ERRROR !!!!!" << endl;
	  return; 
	}
    }
  if(nRuns > 5)
    {
      rootFileNameString6 = Rootpath + Form("%s_%i_%i.root", RunPrefix.c_str(), RunNumber6, NumEvents);
      if (gSystem->AccessPathName(rootFileNameString6) == kTRUE)
	{
	  cerr << "!!!!! ERROR !!!!! " << endl << rootFileNameString6 <<  " not found" << endl <<  "!!!!! ERRROR !!!!!" << endl;
	  return; 
	}
    } 
  if(nRuns > 6)
    {
      rootFileNameString7 = Rootpath + Form("%s_%i_%i.root", RunPrefix.c_str(), RunNumber7, NumEvents);
      if (gSystem->AccessPathName(rootFileNameString7) == kTRUE)
	{
	  cerr << "!!!!! ERROR !!!!! " << endl << rootFileNameString7 <<  " not found" << endl <<  "!!!!! ERRROR !!!!!" << endl;
	  return; 
	}
    } 
  if(nRuns > 7)
    {
      rootFileNameString8 = Rootpath + Form("%s_%i_%i.root", RunPrefix.c_str(), RunNumber8, NumEvents);
      if (gSystem->AccessPathName(rootFileNameString8) == kTRUE)
	{
	  cerr << "!!!!! ERROR !!!!! " << endl << rootFileNameString8 <<  " not found" << endl <<  "!!!!! ERRROR !!!!!" << endl;
	  return; 
	}
    } 
  if(nRuns > 8)
    {
      rootFileNameString9 = Rootpath + Form("%s_%i_%i.root", RunPrefix.c_str(), RunNumber9, NumEvents);
      if (gSystem->AccessPathName(rootFileNameString9) == kTRUE)
	{
	  cerr << "!!!!! ERROR !!!!! " << endl << rootFileNameString9 <<  " not found" << endl <<  "!!!!! ERRROR !!!!!" << endl;
	  return; 
	}
    } 
  if(nRuns > 9)
    {
      rootFileNameString10 = Rootpath + Form("%s_%i_%i.root", RunPrefix.c_str(), RunNumber10, NumEvents);
      if (gSystem->AccessPathName(rootFileNameString10) == kTRUE)
	{
	  cerr << "!!!!! ERROR !!!!! " << endl << rootFileNameString10 <<  " not found" << endl <<  "!!!!! ERRROR !!!!!" << endl;
	  return; 
	}
    }
    
  TChain ch("T");
  ch.Add(rootFileNameString1);
  if(nRuns >1)
    {
      ch.Add(rootFileNameString2);
    }
  if (nRuns > 2)
    {
      ch.Add(rootFileNameString3);
    }
  if(nRuns >3)
    {
      ch.Add(rootFileNameString4);
    }
  if (nRuns > 4)
    {
      ch.Add(rootFileNameString5);
    }

  if (nRuns > 5)
    {
      ch.Add(rootFileNameString6);
    } 

  if (nRuns > 6)
    {
      ch.Add(rootFileNameString7);
    } 
  
  if (nRuns > 7)
    {
      ch.Add(rootFileNameString8);
    } 
  
  if (nRuns > 8)
    {
      ch.Add(rootFileNameString9);
    } 
  
  if (nRuns > 9)
    {
      ch.Add(rootFileNameString10);
    }  
 
  TProof *proof = TProof::Open("workers=4");
  TString option;
  if (nRuns==1)
    {
      option = Form("%i,%i", RunNumber1, RunNumber1);
    }
  else if (nRuns == 2)
    {
      option = Form("%i,%i", RunNumber1, RunNumber2);
    }
  else if (nRuns == 3)
    {
      option = Form("%i,%i", RunNumber1, RunNumber3);
    }
  else if (nRuns == 4)
    {
      option = Form("%i,%i", RunNumber1, RunNumber4);
    }
  else if (nRuns == 5)
    {
      option = Form("%i,%i", RunNumber1, RunNumber5);
    }
  else if (nRuns == 6)
    {
      option = Form("%i,%i", RunNumber1, RunNumber6);
    }
  else if (nRuns == 7)
    {
      option = Form("%i,%i", RunNumber1, RunNumber7);
    }
  else if (nRuns == 8)
    {
      option = Form("%i,%i", RunNumber1, RunNumber8);
    }
  else if (nRuns == 9)
    {
      option = Form("%i,%i", RunNumber1, RunNumber9);
    }
  else if (nRuns == 10)
    {
    option = Form("%i,%i", RunNumber1, RunNumber10);
    }

  proof->SetProgressDialog(0);  
  ch.SetProof();
  //Start calibration process
  ch.Process("calibration.C",option);

}
