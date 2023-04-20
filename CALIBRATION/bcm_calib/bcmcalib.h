



#ifndef BCMCALIB_H
#define BCMCALIB_H

#include <string>
#include <stdio.h> 
#include <iomanip>
#include <locale>
#include <algorithm>
#include <array>
#include <vector>
#include "TSQLServer.h"
#include "TSQLResult.h"
#include "TSQLRow.h"
#include "TTree.h"
#include "TROOT.h"
#include "TStyle.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <iomanip>
#include "TH1.h"
#include "TChain.h"
#include "TCanvas.h"
#include "TMath.h"
#include "TLatex.h"
#include "TLeaf.h"
#include "TGraphErrors.h"
#include "TSystem.h"
#include "TMultiGraph.h"
#include "TLegend.h"
#include "TH2.h"
#include "TLine.h"
#include "TEllipse.h"
#include "TCut.h"
#include "TProfile.h"
#include "TFile.h"
#include "TEventList.h"
#include "TSpectrum.h"
#include <TString.h>

const char* PATHS[] = {
  "./",
  "/volatile/hallc/xem2/nathaly/bcm_root",
  "/cache/hallc/xem2/analysis/ONLINE/REPLAYS/SHMS/PRODUCTION/",
  0
};


TChain* LoadRun(Int_t run, const char* path, const char* tree, Int_t debug)
{
    TChain* tt = new TChain(tree);

    TString basename = Form("shms_replay_production_%d",run);
    TString rootfile = basename + "_-1.root";
    
    TString dir = path;
    if (!dir.EndsWith("/")) dir.Append("/");

    rootfile.Prepend(dir.Data());

    Long_t split = 1;
    while ( !gSystem->AccessPathName(rootfile.Data()) ) {
	cout << rootfile << endl;
        tt->Add(rootfile.Data());
	split++;
	rootfile = basename + "_-" + split + ".root";
	rootfile.Prepend(dir.Data());
    }
 
    if (split<=1) {
	if (debug>0) {
      	   cerr << "Can not find ROOT file for run " << run << endl;
    	}
	delete tt;
        tt = 0;
    }

    return tt;
}
  
TChain* LoadRun(Int_t run, const char* tree = "T")
{
    Int_t i=0;

    TChain* tt = 0;
    while (PATHS[i]) {
	     tt = LoadRun(run,PATHS[i++],tree,0);
	     if (tt) break;
    }
   
    if (!tt){
        cerr << "Can not find ROOT file for run " << run << endl;
        return 0;
    }

    return tt;
}

Double_t computeMean(std::vector<Double_t> array) {
    Int_t size = array.size();
    double sum{};
    for (int i = 0; i < size; i++)  {
        sum += array[i];
    }
    return sum / size;
}


Double_t calculateSD(std::vector<Double_t> array) {

   Int_t size = array.size();
   Double_t sum{}, SD{};

    for (int i = 0; i < size; i++)  {
        sum += array[i];
    }
    Double_t mean = sum / size;

    for(int i = 0; i < size; i++) {
    	SD += pow(array[i] - mean, 2);
    }
    return sqrt(SD/(size-1));

}
#endif
