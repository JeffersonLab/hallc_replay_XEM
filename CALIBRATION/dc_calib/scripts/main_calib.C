//Main Calibration Code
#include "DC_calib.h"
#include "DC_calib.C"
#include <iostream>
#include <ctime>
using namespace std;

int main_calib()
{

  //prevent root from displaying graphs while executing
  gROOT->SetBatch(1);


  //measure execution time
  clock_t cl;
  cl = clock();

  DC_calib obj("HMS", "../../../ROOTfiles/CALIB/hms_replay_dc_2088_-1.root", 2088, -1, "pid_elec", "card");                                 
  //DC_calib obj("SHMS", "../../../ROOTfiles/CALIB/shms_replay_dc_3219_-1.root", 3219, -1, "pid_elec", "card");                                 
  obj.setup_Directory();
  obj.SetPlaneNames();
  obj.GetDCLeafs();
  obj.AllocateDynamicArrays();
  obj.SetTdcOffset();
  obj.CreateHistoNames();
  obj.EventLoop("FillUncorrectedTimes");
  obj.Calculate_tZero();
  obj.EventLoop("ApplyT0Correction");
  obj.WriteTZeroParam();
  obj.WriteLookUpTable();
  obj.WriteToFile(1);  //set argument to (1) for debugging
 

  //stop clock
 cl = clock() - cl;
 cout << "execution time: " << cl/(double)CLOCKS_PER_SEC << " sec" << endl;

  return 0;
}
