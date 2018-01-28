#ifndef __SCLAERCALIB_H__
#define __SCLAERCALIB_H__

#include <iostream>
#include <fstream>
#include <string>

#include <vector>

using namespace std;

enum PrintOption {DEFAULT, ALL};

typedef vector<double> ScalerContainer;
typedef ScalerContainer::iterator SCIterator;

class ScalerCalib {
  
 public:
  
  ScalerCalib(string name);
  virtual ~ScalerCalib();

  void SetInputFile(string fin){ filename = fin; }
  void SetPrintFlag(int p_opt){ fPrintFlag = p_opt; }
  int Run();

 private:

  string fName;
  int fPrintFlag;

  string runstr; // used to form a output file name
  string filename;
  string ofilename;

  void ClearContainers();
  int FillContainer();
  int PrintContainer(ScalerContainer sc);


  ScalerContainer evnum;
  ScalerContainer bcm1;
  ScalerContainer bcm2;
  ScalerContainer bcm4a;
  ScalerContainer bcm4b;
  ScalerContainer bcm17;

  ClassDef(ScalerCalib,0)

};

#endif /*__SCLAERCALIB_H__*/
