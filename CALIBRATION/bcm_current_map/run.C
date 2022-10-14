R__LOAD_LIBRARY(./CALIBRATION/bcm_current_map/ScalerCalib.C)
//R__LOAD_LIBRARY(ScalerCalib.C)

void run(string fin="fin.root", string spec_name="H", int runNum=0)
{

  //H: HMS, P: SHMS
  ScalerCalib scalib(spec_name);
  scalib.SetInputFile(fin);
  scalib.SetRunNum(runNum);
  scalib.SetPrintFlag(1); //0: bcm1 and bcm2 only, 1: all
  scalib.Run();

}



