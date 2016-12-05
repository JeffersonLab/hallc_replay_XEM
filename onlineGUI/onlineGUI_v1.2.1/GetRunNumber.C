// GetRunNumber.C
//
// Helper macro for OnlineGUI to determine runnumber
//
#ifdef USEPODD
#include "src/THaRun.h"
#endif

UInt_t GetRunNumber()
{

#ifdef USEPODD
  // This is specific to the Hall-A C++/ROOT Analyzer (PODD)
  //  Must use "analyzer" to obtain this info.

  THaRun* runinfo = (THaRun*)gROOT->FindObject("Run_Data");
  if(runinfo==NULL) return 0;
  return runinfo->GetNumber();
#else
  return 0;
#endif


}
