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


  DC_calib obj("HMS", "../../../ROOTfiles/hms_replay_production_859_-1.root", 859,574333);
  
 
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
