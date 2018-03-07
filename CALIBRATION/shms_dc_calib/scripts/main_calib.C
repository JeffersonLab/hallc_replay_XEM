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
                                                                                                  
                                                                                                  //pid_elec, pid_kFALSE (no PID cuts) 
                                                                                                  // |
                                                                                                  // v
  DC_calib obj("HMS", "../../../ROOTfiles/hms_replay_production_all_1640_300000.root", 1640,290000, "pid_elec");
  
 
  obj.printInitVar();
  obj.SetPlaneNames();
  obj.GetDCLeafs();
  obj.AllocateDynamicArrays();
  obj.CreateHistoNames();
  obj.EventLoop();
  obj.Calculate_tZero();
  obj.WriteTZeroParam();
  obj.ApplyTZeroCorrection();
  obj.WriteLookUpTable();
  obj.WriteToFile(1);  //set argument to (1) for debugging
  
 

  //stop clock
 cl = clock() - cl;
 cout << "execution time: " << cl/(double)CLOCKS_PER_SEC << " sec" << endl;

  return 0;
}
