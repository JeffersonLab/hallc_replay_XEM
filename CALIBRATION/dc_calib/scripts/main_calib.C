//Main Calibration Code
#include "DC_calib.h"
#include "DC_calib.C"
#include <iostream>
#include <ctime>
using namespace std;

int main_calib(int runNumber, int nEvents=-1)
{

  //prevent root from displaying graphs while executing
  gROOT->SetBatch(1);


  //measure execution time
  clock_t cl;
  cl = clock();

  //DC_calib obj("HMS", Form("ROOTfiles/HMS/CALIBRATION/h_%d_%d.root",runNumber, nEvents), runNumber, -1, "pid_elec", "card");                       
  DC_calib obj("SHMS", Form("/lustre24/expphy/volatile/hallc/xem2/zoew/ROOTfiles/SHMS/CALIBRATION/shms_replay_dc_own2_%d_%d.root",runNumber, nEvents), runNumber, -1, "pid_elec", "wire");                                 
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
