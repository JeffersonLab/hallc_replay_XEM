///////////////////////////////////////////////////////////////////
//  Macro to help with online analysis
//    B. Moffit (moffit@jlab.org)
//    - Originally written in Oct. 2003
//  Documentation and updates available at:
//    http://www.jlab.org/~moffit/onlineGUI/
//

#include <TTree.h>
#include <TFile.h>
#include <TGButton.h>
#include <TGFrame.h>
#include <TRootEmbeddedCanvas.h>
#include "TGLabel.h"
#include "TGString.h"
#include <RQ_OBJECT.h>
#include <TQObject.h>
#include <vector>
#include <TString.h>
#include <TCut.h>
#include <TTimer.h>
#include "TH1.h"
#include "TH2.h"
#include "TH3.h"

#define UPDATETIME 2000

using namespace std;

/* TTree *raw; */
/* TFile *file; */
struct fileObject {
  TString name;
  TString title;
  TString type;
  TString directory;
};

struct RootFileObject {
  TFile*                            RootFile;
  vector <TTree*>                   RootTree;
  vector <Int_t>                    TreeEntries;
  vector < vector <TString> >       TreeVars;
  Bool_t                            fUpdate;
  TH1D                             *mytemp1d;
  TH2D                             *mytemp2d;
  TH3D                             *mytemp3d;
  vector <fileObject>               vfileObjects;
};

struct drawcommand {
  TString variable;
  TString cut;
  TString type;
  TString title;
  TString treename;
  TString nostat;
  TString noshowgolden;
  TString macro;
  TString directory;
  TString objname;
  TString objtype;
  TString objtitle;
};

class OnlineConfig {
  RQ_OBJECT("OnlineConfig");
  // Class that takes care of the config file
private:
  TString confFileName;                   // config filename
  ifstream *fConfFile;                    // original config file
  void ParseFile();
  vector < vector <TString> > sConfFile;  // the config file, in memory
  // pageInfo is the vector of the pages containing the sConfFile index
  //   and how many commands issued within that page (title, 1d, etc.)
  TString rootfilename;  //  Just the name
  TString goldenrootfilename; // Golden rootfile for comparisons
  TString protorootfile; // Prototype for getting the rootfilename
  TString configfilestart; // 
  TString guicolor; // User's choice of background color
  TString plotsdir; // Where to store sample plots.. automatically stored as .jpg's).
  UInt_t canvaswidth; // Width of histogram canvas
  UInt_t canvasheight; // Width of histogram canvas
  vector < pair <UInt_t,UInt_t> > pageInfo; 
  vector <TCut> cutList; 
  vector <UInt_t> GetDrawIndex(UInt_t);
  Bool_t fFoundCfg;
  Bool_t fMonitor;
  Bool_t ParseForMultiPlots();

public:
  OnlineConfig();
  OnlineConfig(TString);
  Bool_t ParseConfig();
  TString GetRootFile() { return rootfilename; };
  TString GetGoldenFile() { return goldenrootfilename; };
  TString GetGuiColor() { return guicolor; };
  TString GetPlotsDir() { return plotsdir; };
  TCut GetDefinedCut(TString ident);
  vector <TString> GetCutIdent();
  // Page utilites
  UInt_t  GetPageCount() { return pageInfo.size(); };
  pair <UInt_t,UInt_t> GetPageDim(UInt_t);
  Bool_t IsLogxy(UInt_t page);
  Bool_t IsLogx(UInt_t page);
  Bool_t IsLogy(UInt_t page);
  Bool_t IsLogz(UInt_t page);
  TString GetPageTitle(UInt_t);
  UInt_t GetDrawCount(UInt_t);           // Number of histograms in a page
  drawcommand GetDrawCommand(UInt_t,UInt_t);
  vector <TString> SplitString(TString,TString);
  void OverrideRootFile(UInt_t);
  Bool_t IsMonitor() { return fMonitor; };
  UInt_t GetCanvasWidth() { return canvaswidth; };
  UInt_t GetCanvasHeight() { return canvasheight; };
  TString GetConfigStart() {return configfilestart;}; // 
};

class OnlineGUI {
  // Class that takes care of the GUI
  RQ_OBJECT("OnlineGUI")
private:
	//UInt_t fRunNum;
  TGMainFrame                      *fMain;
  TGHorizontalFrame                *fTopframe;
  TGVerticalFrame                  *vframe;
  TGRadioButton                    *fRadioPage[50];
  TGPictureButton                  *wile;
  TRootEmbeddedCanvas              *fEcanvas;
  TGHorizontalFrame                *fBottomFrame;
  TGHorizontalFrame                *hframe;
  TGTextButton                     *fNext;
  TGTextButton                     *fPrev;
  TGTextButton                     *fExit;
  TGLabel                          *fRunNumber;
  TGTextButton                     *fPrint;
  TCanvas                          *fCanvas; // Present Embedded canvas
  OnlineConfig                     *fConfig;
  UInt_t                            current_page;
  RootFileObject                    fRootFile;
  RootFileObject                    fGoldenFile;
  Bool_t                            doGolden;
  UInt_t                            runNumber;
  TTimer                           *timer;
  Bool_t                            fPrintOnly;
  Bool_t                            fRootOnly;
  Bool_t                            fFileAlive;

public:
  OnlineGUI(OnlineConfig&,Bool_t,Bool_t,UInt_t RunNum=0);
  void CreateGUI(const TGWindow *p, UInt_t w, UInt_t h);
  virtual ~OnlineGUI();
  void DoDraw();
  void DrawPrev();
  void DrawNext();
  void DoRadio();
  void CheckPageButtons();
  // Specific Draw Methods
  void GetFileObjects(RootFileObject *r);
  void GetTreeVars(RootFileObject *r);
  void GetRootTree(RootFileObject *r);
  UInt_t GetTreeIndex(TString,RootFileObject *r);
  UInt_t GetTreeIndexFromName(TString, RootFileObject *r);
  drawcommand fileObject2command(drawcommand,RootFileObject *r);
  void TreeDraw(drawcommand); 
  void HistDraw(drawcommand);
  void MacroDraw(drawcommand);
  void CanvasDraw(drawcommand,UInt_t);
  void GraphDraw(drawcommand);
  void DoDrawClear(RootFileObject *r);
  void TimerUpdate();
  void BadDraw(TString);
  void CheckRootFile();
  Int_t OpenRootFile();
  void ObtainRunNumber();
  void PrintToFile();
  void PrintPages();
  void MyCloseWindow();
  void CloseGUI();
};
