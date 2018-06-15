R__LOAD_LIBRARY(ScalerCalib_C)

void run(string fin="fin.root", string spec_name="H")
{

  //H: HMS, P: SHMS
  ScalerCalib scalib(spec_name);
  scalib.SetInputFile(fin);
  scalib.SetPrintFlag(1); //0: bcm1 and bcm2 only, 1: all
  scalib.Run();

}



