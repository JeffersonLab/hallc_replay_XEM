R__LOAD_LIBRARY(ScalerCalib_C)

void run(string fin="fin.root")
{

  //H: HMS, P: SHMS
  ScalerCalib scalib("H");
  scalib.SetInputFile(fin);
  scalib.SetPrintFlag(1); //0: bcm1 and bcm2 only, 1: all
  scalib.Run();

}



