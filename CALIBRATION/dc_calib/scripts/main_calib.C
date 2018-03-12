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
                                                                                                  
                                                                                                        //pid_elec, pid_hadron, dc_1hit,  pid_kFALSE (no PID cuts) 
                                                                                                        // |
                                                                                                        // v
//  DC_calib obj("HMS", "../../../ROOTfiles/hms_replay_production_all_1856_dcuncal.root", 1856,200, "pid_kFALSE");
  //DC_calib obj("SHMS", "../../../ROOTfiles/shms_replay_production_all_2071_-1_dcuncalib.root", 2071, 3000000, "pid_bkg"); 
  DC_calib obj("HMS", "../../../ROOTfiles/hms_coin_replay_production_1866_1000000.root", 1866, 1000, "pid_kFALSE");

  obj.setup_Directory();
  obj.printInitVar();
  obj.SetPlaneNames();
  obj.GetDCLeafs();
  obj.AllocateDynamicArrays();
  obj.CreateHistoNames();
  obj.EventLoop();
  obj.Calculate_tZero();
  obj.ApplyTZeroCorrection();
  obj.WriteTZeroParam();
  obj.WriteLookUpTable();
  obj.WriteToFile(1);  //set argument to (1) for debugging
   

  //stop clock
 cl = clock() - cl;
 cout << "execution time: " << cl/(double)CLOCKS_PER_SEC << " sec" << endl;

  return 0;
}
