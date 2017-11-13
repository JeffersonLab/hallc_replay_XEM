///////////////////////////////////////////////////////////////////
//  Macro to help with online analysis
//    B. Moffit (moffit@jlab.org)
//    - Originally written in Oct. 2003
//  Documentation and updates available at:
//    http://www.jlab.org/~moffit/onlineGUI/
//

#include "online.h"
#include <string>
#include <fstream>
#include <iostream>
#include <list>
#include <TMath.h>
#include <TBranch.h>
#include <TGClient.h>
#include <TCanvas.h>
#include <TStyle.h>
#include <TROOT.h>
#include <TGImageMap.h>
#include <TGFileDialog.h>
#include <TKey.h>
#include <TObjString.h>
#include <TSystem.h>
#include <TLatex.h>
#include <TText.h>
#include <TGraph.h>
#include "GetRootFileName.C"
#include "GetRunNumber.C"
#include "TPaveText.h"
#ifdef STANDALONE
#include <TApplication.h>
#endif
// #define DEBUGGETFILEOBJECTS
// #define DEBUGPARSE
// #define DEBUG
//#define DEBUG2
//#define NOISY
//#define INTERNALSTYLE

//TString guiDirectory = "onlineGUI_v1.2.1/";
TString guiDirectory = "./";

///////////////////////////////////////////////////////////////////
//  Class: OnlineConfig
//
//     Utility class that reads in a text file (.cfg) and
//     stores it's contents.
//
//

OnlineConfig::OnlineConfig()
{
    // Constructor.  Without an argument, will use default "standard" config
    fMonitor = kFALSE;
    OnlineConfig("standard");
}

OnlineConfig::OnlineConfig(TString anatype)
{
    // Constructor.  Takes the config anatype as the only argument.
    //  Loads up the configuration file, and stores it's contents for access.

    confFileName = anatype;
    configfilestart=anatype;
    if(!confFileName.Contains(".cfg"))
        confFileName += ".cfg";
    fMonitor = kFALSE;
    fFoundCfg = kFALSE;

    // Attempt to find the config file in specified (first argument,
    // relative or absolute) location, then in guiDirectory
    fConfFile = new ifstream(confFileName.Data());
    if ( ! (*fConfFile) ) {
        cerr << "OnlineConfig() WARNING: config file " << confFileName.Data()
            << " does not exist" << endl;
        cerr << " Checking the " << guiDirectory << " directory" << endl;
        confFileName.Prepend(guiDirectory+"/");
        fConfFile = new ifstream(confFileName.Data());
        if ( ! (*fConfFile) ) {
            // Didn't find it... load up the default config
            confFileName = guiDirectory+"/default.cfg";
            cout << "OnlineConfig()  Trying " << confFileName.Data()
                << " as default configuration." << endl
                << " (May be ok.)" << endl;
            fConfFile = new ifstream(confFileName.Data());
            if ( ! (*fConfFile) ) {
                cerr << "OnlineConfig() WARNING: no file "
                    << confFileName.Data() <<endl;
                cerr << "You need a configuration to run.  Ask an expert."<<endl;
                fFoundCfg = kFALSE;
                //      return;
            } else {
                fFoundCfg = kTRUE;
            }
        } else {
            fFoundCfg = kTRUE;
        }
    } else {
        fFoundCfg = kTRUE;
    }

    if(fFoundCfg) {
        clog << "GUI Configuration loading from "
            << confFileName.Data() << endl;
    }

    ParseFile();

    // Load up a user configurable style (if it exists)
    TString styleFile = guiDirectory+"onlineGUI_Style.C";
    if(!gSystem->AccessPathName(styleFile))
        gROOT->Macro(styleFile);

    fConfFile->close();
    delete fConfFile;

}

void OnlineConfig::ParseFile()
{
    // Reads in the Config File, and makes the proper calls to put
    //  the information contained into memory.

    if(!fFoundCfg) {
        return;
    }

    TString comment = "#";
    vector<TString> strvect;
    TString sinput, sline;
    while (sline.ReadLine(*fConfFile)) {
        strvect.clear();
        // Strip away leading and trailing spaces.
        sline = sline.Strip(TString::kBoth,' ');
        // If the line starts with a comment, skip it.
        if(sline.BeginsWith(comment)) continue;
        // If the line contains a comment, split it up into two and keep
        // the uncommented (first) part
        if(sline.Contains(comment)) {
            TObjArray *tmpoarr = sline.Tokenize(comment);
            TObjString *tmpostr = (TObjString*)tmpoarr->First();
            sline = tmpostr->GetString().Strip(TString::kTrailing,' ');
            delete tmpoarr;
        }
        // Now split up the line according to 's and spaces.  This is for
        // looking for the histogram using it's title (which may contain
        // spaces) instead of its name.
        if(sline.Contains("'")) {
            TObjArray *tmpoarr = sline.Tokenize("'");
            Bool_t quote_on_odd = kTRUE;
            if(!(sline.BeginsWith("'"))) quote_on_odd=kFALSE;
            TIterator *next = tmpoarr->MakeIterator();
            TObjString *tmpostr; Bool_t odd=kTRUE;
            while((tmpostr=(TObjString*)next->Next())!=NULL) {
                if((quote_on_odd&&odd) || (!quote_on_odd&&!odd)) {
                    strvect.push_back(tmpostr->GetString());
                    odd = !odd;
                } else {
                    TObjArray *tmpoarr2 = tmpostr->GetString().Tokenize(" ");
                    TIterator *next2 = tmpoarr2->MakeIterator();
                    TObjString *tmpostr2;
                    while((tmpostr2=(TObjString*)next2->Next())!=NULL) {
                        strvect.push_back(tmpostr2->GetString());
                    }
                    delete tmpoarr2;
                    odd = !odd;
                }
            }
            delete tmpoarr;
        } else { // No 's.. just split according to spaces
            TObjArray *tmpoarr2 = sline.Tokenize(" ");
            TIterator *next2 = tmpoarr2->MakeIterator();
            TObjString *tmpostr2;
            while((tmpostr2=(TObjString*)next2->Next())!=NULL) {
                strvect.push_back(tmpostr2->GetString());
            }
            delete tmpoarr2;
        }
        sConfFile.push_back(strvect);

    }

#ifdef DEBUGPARSE
    for(UInt_t ii=0; ii<sConfFile.size(); ii++) {
        cout << "Line " << ii << endl << "  ";
        for(UInt_t jj=0; jj<sConfFile[ii].size(); jj++)
            cout << sConfFile[ii][jj] << " ";
        cout << endl;
    }
#endif

    cout << "     " << sConfFile.size() << " lines read from "
        << confFileName << endl;

}

Bool_t OnlineConfig::ParseConfig()
{
    //  Goes through each line of the config [must have been ParseFile()'d]
    //   and interprets.

    if(!fFoundCfg) {
        return kFALSE;
    }

    UInt_t command_cnt=0;
    UInt_t j=0;
    Bool_t hasMultiPlots=kFALSE;
    canvaswidth = 800;		// Default canvas size
    canvasheight = 600;
    // If statement for each high level command (cut, newpage, etc)
    for(UInt_t i=0;i<sConfFile.size();i++) {
        // "newpage" command
        if(sConfFile[i][0] == "newpage") {
            // sConfFile[i] is first of pair
            for(j=i+1;j<sConfFile.size();j++) {
                if(sConfFile[j][0] != "newpage") {
                    // Count how many commands within the page
                    command_cnt++;
                } else break;
            }
            pageInfo.push_back(make_pair(i,command_cnt));
            i += command_cnt;
            command_cnt=0;
        }
        if(sConfFile[i][0] == "watchfile") {
            fMonitor = kTRUE;
        }
        if(sConfFile[i][0] == "definecut") {
            if(sConfFile[i].size()>3) {
                cerr << "cut command has too many arguments" << endl;
                continue;
            }
            TCut tempCut(sConfFile[i][1],sConfFile[i][2]);
            cutList.push_back(tempCut);
        }
        if(sConfFile[i][0] == "rootfile") {
            if(sConfFile[i].size() != 2) {
                cerr << "WARNING: rootfile command does not have the "
                    << "correct number of arguments"
                    << endl;
                continue;
            }
            if(!rootfilename.IsNull()) {
                cerr << "WARNING: too many rootfile's defined. "
                    << " Will only use the first one."
                    << endl;
                continue;
            }
            rootfilename = sConfFile[i][1];
        }
        if(sConfFile[i][0] == "goldenrootfile") {
            if(sConfFile[i].size() != 2) {
                cerr << "WARNING: goldenfile command does not have the "
                    << "correct number of arguments"
                    << endl;
                continue;
            }
            if(!goldenrootfilename.IsNull()) {
                cerr << "WARNING: too many goldenrootfile's defined. "
                    << " Will only use the first one."
                    << endl;
                continue;
            }
            goldenrootfilename = sConfFile[i][1];
        }
        if(sConfFile[i][0] == "protorootfile") {
            if(sConfFile[i].size() != 2) {
                cerr << "WARNING: protorootfile command does not have the "
                    << "correct number of arguments"
                    << endl;
                continue;
            }
            if(!protorootfile.IsNull()) {
                cerr << "WARNING: too many protorootfile's defined. "
                    << " Will only use the first one."
                    << endl;
                continue;
            }
            protorootfile = sConfFile[i][1];
        }
        if(sConfFile[i][0] == "guicolor") {
            if(sConfFile[i].size() != 2) {
                cerr << "WARNING: guicolor command does not have the "
                    << "correct number of arguments (needs 1)"
                    << endl;
                continue;
            }
            if(!guicolor.IsNull()) {
                cerr << "WARNING: too many guicolor's defined. "
                    << " Will only use the first one."
                    << endl;
                continue;
            }
            guicolor = sConfFile[i][1];
        }
        if(sConfFile[i][0] == "canvassize") {
            if(sConfFile[i].size() != 3) {
                cerr << "WARNING: canvassize command does not have the "
                    << "correct number of arguments (needs 2)"
                    << endl;
                continue;
            }
            canvaswidth = UInt_t(atoi(sConfFile[i][1]));
            canvasheight = UInt_t(atoi(sConfFile[i][2]));
        }
        if(sConfFile[i][0] == "plotsdir") {
            if(sConfFile[i].size() != 2) {
                cerr << "WARNING: plotsdir command does not have the "
                    << "correct number of arguments (needs 1)"
                    << endl;
                continue;
            }
            if(!plotsdir.IsNull()) {
                cerr << "WARNING: too many plotdir's defined. "
                    << " Will only use the first one."
                    << endl;
                continue;
            }
            plotsdir = sConfFile[i][1];
        }
        if(sConfFile[i][0] == "multiplot") {
            hasMultiPlots = kTRUE;
        }

    }

    if(hasMultiPlots) {
        if(!ParseForMultiPlots()) {
            cerr << "ParseForMultiPlots() Failed" << endl;
        }
    }

#ifdef NOISY
    for(UInt_t i=0; i<GetPageCount(); i++) {
        cout << "Page " << i << " (" << GetPageTitle(i) << ")"
            << " will draw " << GetDrawCount(i)
            << " histograms." << endl;
    }
#endif

    cout << "Number of pages defined = " << GetPageCount() << endl;
    cout << "Number of cuts defined = " << cutList.size() << endl;

    if (fMonitor)
        cout << "Will periodically update plots" << endl;
    if(!goldenrootfilename.IsNull()) {
        cout << "Will compare chosen histrograms with the golden rootfile: "
            << endl
            << goldenrootfilename << endl;
    }

    return kTRUE;

}

Bool_t OnlineConfig::ParseForMultiPlots() {
    // Parse through each line of sConfFile,
    // and replace each "multiplot" command with a real draw entry

    vector < vector <TString> > newConfFile;
    for(UInt_t iter=0;iter<sConfFile.size();iter++) {
        if(sConfFile[iter][0]=="multiplot") {
            // First argument is the low limit,
            // second argument is the high limit
            UInt_t lolimit = (UInt_t)sConfFile[iter][1].Atoi();
            UInt_t hilimit = (UInt_t)sConfFile[iter][2].Atoi();

            // For the rest of the line, replace all of the XXXXX's
            // with an iteration from lolimit to hilimit.
            for(UInt_t imult=lolimit;imult<=hilimit;imult++) {
                vector <TString> newline;
                for(UInt_t ielem=3; ielem<sConfFile[iter].size();ielem++) {
                    TString origString = sConfFile[iter][ielem];
                    TString newString = origString;
                    newString.ReplaceAll("XXXXX",Form("%d",imult));
                    newline.push_back(newString);
                }
                newConfFile.push_back(newline);
            }
        } else {
            newConfFile.push_back(sConfFile[iter]);
        }
    }

#ifdef DEBUG
    for(UInt_t iter=0;iter<newConfFile.size();iter++) {
        for(UInt_t iline=0;iline<newConfFile[iter].size();iline++) {
            cout << newConfFile[iter][iline] << " ";
        }
        cout << endl;
    }
#endif

    // Out with the old, in with the new.
    sConfFile.clear();
    sConfFile = newConfFile;

    // Now need to recalculate pageInfo.
    UInt_t command_cnt=0;
    UInt_t j=0;

    pageInfo.clear();
    // If statement for each high level command (cut, newpage, etc)
    for(UInt_t i=0;i<sConfFile.size();i++) {
        // "newpage" command
        if(sConfFile[i][0] == "newpage") {
            // sConfFile[i] is first of pair
            for(j=i+1;j<sConfFile.size();j++) {
                if(sConfFile[j][0] != "newpage") {
                    // Count how many commands within the page
                    command_cnt++;
                } else break;
            }
            pageInfo.push_back(make_pair(i,command_cnt));
            i += command_cnt;
            command_cnt=0;
        }
    }

    return kTRUE;

}

TCut OnlineConfig::GetDefinedCut(TString ident) {
    // Returns the defined cut, according to the identifier

    for(UInt_t i=0; i<cutList.size(); i++) {
        if((TString)cutList[i].GetName() == ident.Data()) {
            TCut tempCut = cutList[i].GetTitle();
            return tempCut;
        }
    }
    return "";
}

vector <TString> OnlineConfig::GetCutIdent() {
    // Returns a vector of the cut identifiers, specified in config
    vector <TString> out;

    for(UInt_t i=0; i<cutList.size(); i++) {
        out.push_back(cutList[i].GetName());
    }

    return out;
}

Bool_t OnlineConfig::IsLogx(UInt_t page) {
    // Check if last word on line is "logx"

    UInt_t page_index = pageInfo[page].first;
    Int_t word_index = sConfFile[page_index].size()-1;
    if (word_index <= 0) return kFALSE;
    TString option = sConfFile[page_index][word_index];
    if(option == "logx") return kTRUE;
    return kFALSE;

}

Bool_t OnlineConfig::IsLogy(UInt_t page) {
    // Check if last word on line is "logy"

    UInt_t page_index = pageInfo[page].first;
    Int_t word_index = sConfFile[page_index].size()-1;
    if (word_index <= 0) return kFALSE;
    TString option = sConfFile[page_index][word_index];
    if(option == "logy") return kTRUE;
    return kFALSE;

}

Bool_t OnlineConfig::IsLogz(UInt_t page) {
    // Check if last word on line is "logz"

    UInt_t page_index = pageInfo[page].first;
    Int_t word_index = sConfFile[page_index].size()-1;
    if (word_index <= 0) return kFALSE;
    TString option = sConfFile[page_index][word_index];
    if(option == "logz") return kTRUE;
    return kFALSE;

}


pair <UInt_t, UInt_t> OnlineConfig::GetPageDim(UInt_t page)
{
    // If defined in the config, will return those dimensions
    //  for the indicated page.  Otherwise, will return the
    //  calculated dimensions required to fit all histograms.

    pair <UInt_t, UInt_t> outDim;

    // This is the page index in sConfFile.
    UInt_t page_index = pageInfo[page].first;

    UInt_t size1 = 2;

    if (IsLogx(page)) size1 = 3;  // last word is "logy"
    if (IsLogy(page)) size1 = 3;  // last word is "logy"
    if (IsLogz(page)) size1 = 3;  // last word is "logy"

    // If the dimensions are defined, return them.
    if(sConfFile[page_index].size()>size1-1) {
        if(sConfFile[page_index].size() == size1) {
            outDim = make_pair(UInt_t(atoi(sConfFile[page_index][1])),
                    UInt_t(atoi(sConfFile[page_index][1])));
            return outDim;
        } else if (sConfFile[page_index].size() == size1+1) {
            outDim = make_pair(UInt_t(atoi(sConfFile[page_index][1])),
                    UInt_t(atoi(sConfFile[page_index][2])));
            return outDim;
        } else {
            cout << "Warning: newpage command has too many arguments. "
                << "Will automatically determine dimensions of page."
                << endl;
        }
    }

    // If not defined, return the "default."
    UInt_t draw_count = GetDrawCount(page);
    UInt_t dim = UInt_t(TMath::Nint(sqrt(double(draw_count+1))));
    outDim = make_pair(dim,dim);

    return outDim;
}

TString OnlineConfig::GetPageTitle(UInt_t page)
{
    // Returns the title of the page.
    //  if it is not defined in the config, then return "Page #"

    TString title;

    UInt_t iter_command = pageInfo[page].first+1;

    for(UInt_t i=0; i<pageInfo[page].second; i++) { // go through each command
        if(sConfFile[iter_command+i][0] == "title") {
            // Combine the strings, and return it
            for (UInt_t j=1; j<sConfFile[iter_command+i].size(); j++) {
                title += sConfFile[iter_command+i][j];
                title += " ";
            }
            title.Chop();
            return title;
        }
    }
    title = "Page "; title += page;
    return title;
}

vector <UInt_t> OnlineConfig::GetDrawIndex(UInt_t page)
{
    // Returns an index of where to find the draw commands within a page
    //  within the sConfFile vector

    vector <UInt_t> index;
    UInt_t iter_command = pageInfo[page].first+1;

    for(UInt_t i=0; i<pageInfo[page].second; i++) {
        if(sConfFile[iter_command+i][0] != "title") {
            index.push_back(iter_command+i);
        }
    }

    return index;
}

UInt_t OnlineConfig::GetDrawCount(UInt_t page)
{
    // Returns the number of histograms that have been request for this page
    UInt_t draw_count=0;

    for(UInt_t i=0; i<pageInfo[page].second; i++) {
        if(sConfFile[pageInfo[page].first+i+1][0] != "title") draw_count++;
    }

    return draw_count;

}

drawcommand OnlineConfig::GetDrawCommand(UInt_t page, UInt_t nCommand)
{
    // Returns the vector of strings pertaining to a specific page, and
    //   draw command from the config.
    // Return vector of TStrings:
    //  0: variable
    //  1: cut - only for Tree variables
    //  2: type
    //  3: title
    //  4: treename - only for Tree variables
    //  5: nostat
    //  6: noshowgolden  - only for Histograms
    //  This should probably be replaced with an actual structure

    drawcommand out_command;
    vector <UInt_t> command_vector = GetDrawIndex(page);
    UInt_t index = command_vector[nCommand];

#ifdef DEBUG
    cout << "OnlineConfig::GetDrawCommand(" << page << ","
        << nCommand << ")" << endl;
#endif

    // First line is the variable
    if(sConfFile[index].size()>=1) {
        out_command.variable = sConfFile[index][0];
    }
    if(sConfFile[index].size()>=2) {
        if((sConfFile[index][1] != "-type") &&
                (sConfFile[index][1] != "-title") &&
                (sConfFile[index][1] != "-tree") &&
                (sConfFile[index][1] != "-noshowgolden") &&
                (sConfFile[index][1] != "-nostat")) {
            if(out_command.variable=="macro") {
                out_command.macro = sConfFile[index][1];
            } else {
                out_command.cut = sConfFile[index][1];
            }
        }
    }

    // Now go through the rest of that line..
    for (UInt_t i=1; i<sConfFile[index].size(); i++) {
        if(sConfFile[index][i]=="-type") {
            out_command.type = sConfFile[index][i+1];
            i = i+1;
        } else if(sConfFile[index][i]=="-title") {
            // Put the entire title, surrounded by quotes, as one TString
            TString title;
            UInt_t j=0;
            for(j=i+1; j<sConfFile[index].size(); j++) {
                TString word = sConfFile[index][j];
                if( (word.BeginsWith("\"")) && (word.EndsWith("\"")) ) {
                    title = word.ReplaceAll("\"","");
                    out_command.title = title;
                    i = j;
                    break;
                } else if(word.BeginsWith("\"")) {
                    title = word.ReplaceAll("\"","");
                } else if(word.EndsWith("\"")) {
                    title += " " + word.ReplaceAll("\"","");
                    out_command.title = title;
                    i = j;
                    break;
                } else {
                    title += " " + word;
                }
            }
            // If we got here, and out_command.title is null... then just copy the contents
            // of "title" to out_command.title (taking out any "'s along the way)
            if(out_command.title.IsNull())
                out_command.title = title.ReplaceAll("\"","");
        } else if(sConfFile[index][i]=="-tree") {
            out_command.treename = sConfFile[index][i+1];
            i = i+1;
        } else if(sConfFile[index][i]=="-nostat") {
            out_command.nostat = "nostat";
        } else if(sConfFile[index][i]=="-noshowgolden") {
            out_command.noshowgolden = "noshowgolden";
        }


#ifdef DEBUG
        cout << endl;
#endif
    }
#ifdef DEBUG
    cout << sConfFile[index].size() << ": ";
    for(UInt_t i=0; i<sConfFile[index].size(); i++) {
        cout << sConfFile[index][i] << " ";
    }
    cout << endl;
#endif
    return out_command;
}

vector <TString> OnlineConfig::SplitString(TString instring,TString delim)
{
    // Utility to split up a string on the deliminator.
    //  returns a vector of strings.

    vector <TString> v;

    TString remainingString = instring;
    TString tempstring = instring;
    int i;

    while (remainingString.Index(delim) != -1) {
        i = remainingString.Index(delim);
        tempstring.Remove(i);
        v.push_back(tempstring);
        remainingString.Remove(0,i+1);
        while(remainingString.Index(delim) == 0) {
            remainingString.Remove(0,1);
        }
        tempstring = remainingString;
        // Cut out the rest, if it's a comment (starts with #)
        if(TString(remainingString.Strip(TString::kLeading,' ')).BeginsWith("#")) {
            tempstring = "";
            break;
        }
    }

    while(tempstring.EndsWith(delim)) {
        tempstring.Chop();
    }

    if(!tempstring.IsNull()) v.push_back(tempstring);

    return v;
}

void OnlineConfig::OverrideRootFile(UInt_t runnumber)
{
    // Override the ROOT file defined in the cfg file If
    // protorootfile is used, construct filename using it, otherwise
    // uses a helper macro "GetRootFileName.C(UInt_t runnumber)

    if(!protorootfile.IsNull()) {
        TString rn = "";
        rn += runnumber;
        protorootfile.ReplaceAll("XXXXX",rn.Data());
        rootfilename = protorootfile;
    } else {
        rootfilename = GetRootFileName(runnumber);
    }

    cout << "Overridden File name: " << rootfilename << endl;
}

///////////////////////////////////////////////////////////////////
//  Class: OnlineGUI
//
//    Creates a GUI to display the commands used in OnlineConfig
//
//

OnlineGUI::OnlineGUI(OnlineConfig& config, Bool_t printonly, Bool_t rootonly,UInt_t RunNum):
    runNumber(RunNum),
    timer(0),
    fFileAlive(kFALSE)
{
    // Constructor.  Get the config pointer, and make the GUI.

    fConfig = &config;

    if(printonly || rootonly) {
        fPrintOnly=kTRUE;
        PrintPages();
    } else {
        fPrintOnly=kFALSE;
        CreateGUI(gClient->GetRoot(),200,200);
    }
}

void OnlineGUI::CreateGUI(const TGWindow *p, UInt_t w, UInt_t h)
{

    // Open the RootFile.  Die if it doesn't exist.
    //  unless we're watching a file.
    fRootFile.RootFile = new TFile(fConfig->GetRootFile(),"READ");
    if(!fRootFile.RootFile->IsOpen()) {
        cout << "ERROR:  rootfile: " << fConfig->GetRootFile()
            << " does not exist"
            << endl;
        if(fConfig->IsMonitor()) {
            cout << "Will wait... hopefully.." << endl;
        } else {
#ifdef STANDALONE
            gApplication->Terminate();
#else
            return;
#endif
        }
    } else {
        fFileAlive = kTRUE;
        ObtainRunNumber();
        // Open the Root Trees.  Give a warning if it's not there..
        GetFileObjects(&fRootFile);
        GetRootTree(&fRootFile);
        GetTreeVars(&fRootFile);
        for(UInt_t i=0; i<fRootFile.RootTree.size(); i++) {
            if(fRootFile.RootTree[i]==0) {
                fRootFile.RootTree.erase(fRootFile.RootTree.begin() + i);
            }
        }

    }
    TString goldenfilename=fConfig->GetGoldenFile();
    if(!goldenfilename.IsNull()) {
        fGoldenFile.RootFile = new TFile(goldenfilename,"READ");
        if(!fGoldenFile.RootFile->IsOpen()) {
            cout << "ERROR: goldenrootfile: " << goldenfilename
                << " does not exist.  Oh well, no comparison plots."
                << endl;
            doGolden = kFALSE;
            fGoldenFile.RootFile=NULL;
        } else {
            // Open the Root Trees.  Give a warning if it's not there..
            GetFileObjects(&fGoldenFile);
            GetRootTree(&fGoldenFile);
            GetTreeVars(&fGoldenFile);
            for(UInt_t i=0; i<fGoldenFile.RootTree.size(); i++) {
                if(fGoldenFile.RootTree[i]==0) {
                    fGoldenFile.RootTree.erase(fGoldenFile.RootTree.begin() + i);
                }
            }
            doGolden = kTRUE;
        }
    } else {
        doGolden=kFALSE;
        fGoldenFile.RootFile=NULL;
    }


    // Create the main frame
    fMain = new TGMainFrame(p,w,h);
    fMain->Connect("CloseWindow()", "OnlineGUI", this, "MyCloseWindow()");
    ULong_t lightgreen, lightblue, red, mainguicolor;
    gClient->GetColorByName("lightgreen",lightgreen);
    gClient->GetColorByName("lightblue",lightblue);
    gClient->GetColorByName("red",red);

    UInt_t canvas_width = fConfig->GetCanvasWidth();
    UInt_t canvas_height = fConfig->GetCanvasHeight();

    Bool_t good_color=kFALSE;
    TString usercolor = fConfig->GetGuiColor();
    if(!usercolor.IsNull()) {
        good_color = gClient->GetColorByName(usercolor,mainguicolor);
    }

    if(!good_color) {
        if(!usercolor.IsNull()) {
            cout << "Bad guicolor (" << usercolor << ").. using default." << endl;
        }
        if(fConfig->IsMonitor()) {
            // Default background color for Online Monitor
            mainguicolor = lightgreen;
        } else {
            // Default background color for Normal Online Display
            mainguicolor = lightblue;
        }
    }

    fMain->SetBackgroundColor(mainguicolor);

    // Top frame, to hold page buttons and canvas
    fTopframe = new TGHorizontalFrame(fMain,200,200);
    fTopframe->SetBackgroundColor(mainguicolor);
    fMain->AddFrame(fTopframe, new TGLayoutHints(kLHintsExpandX
                | kLHintsExpandY,10,10,10,1));

    // Create a verticle frame widget with radio buttons
    //  This will hold the page buttons
    vframe = new TGVerticalFrame(fTopframe,40,200);
    vframe->SetBackgroundColor(mainguicolor);
    TString buff;
    for(UInt_t i=0; i<fConfig->GetPageCount(); i++) {
        buff = fConfig->GetPageTitle(i);
        fRadioPage[i] = new TGRadioButton(vframe,buff,i);
        fRadioPage[i]->SetBackgroundColor(mainguicolor);
    }

    fRadioPage[0]->SetState(kButtonDown);
    current_page = 0;

    for (UInt_t i=0; i<fConfig->GetPageCount(); i++) {
        vframe->AddFrame(fRadioPage[i],
                new TGLayoutHints(kLHintsLeft |
                    kLHintsCenterY,5,5,3,4));
        fRadioPage[i]->Connect("Pressed()", "OnlineGUI", this, "DoRadio()");
    }
    if(!fConfig->IsMonitor()) {
        wile =
            new TGPictureButton(vframe,
                    gClient->GetPicture(guiDirectory+"/hcsmall.png"));
        wile->Connect("Pressed()","OnlineGUI", this,"DoDraw()");
    } else {
        wile =
            new TGPictureButton(vframe,
                    gClient->GetPicture(guiDirectory+"/panguin.xpm"));
        wile->Connect("Pressed()","OnlineGUI", this,"DoDrawClear(&fRootFile)");
    }
    wile->SetBackgroundColor(mainguicolor);

    vframe->AddFrame(wile,
            new TGLayoutHints(kLHintsBottom|kLHintsLeft,5,10,4,2));


    fTopframe->AddFrame(vframe,
            new TGLayoutHints(kLHintsCenterX|
                kLHintsCenterY,2,2,2,2));

    // Create canvas widget
    fEcanvas = new TRootEmbeddedCanvas("Ecanvas", fTopframe, canvas_width, canvas_height);
    fEcanvas->SetBackgroundColor(mainguicolor);
    fTopframe->AddFrame(fEcanvas, new TGLayoutHints(kLHintsExpandY,10,10,10,1));
    fCanvas = fEcanvas->GetCanvas();

    // Create the bottom frame.  Contains control buttons
    fBottomFrame = new TGHorizontalFrame(fMain,canvas_width+400,200);
    fBottomFrame->SetBackgroundColor(mainguicolor);
    fMain->AddFrame(fBottomFrame, new TGLayoutHints(kLHintsExpandX,10,10,10,10));

    // Create a horizontal frame widget with buttons
    hframe = new TGHorizontalFrame(fBottomFrame,canvas_width+400,40);
    hframe->SetBackgroundColor(mainguicolor);
    fBottomFrame->AddFrame(hframe,new TGLayoutHints(kLHintsExpandX,200,20,2,2));

    fPrev = new TGTextButton(hframe,"Prev");
    fPrev->SetBackgroundColor(mainguicolor);
    fPrev->Connect("Clicked()","OnlineGUI",this,"DrawPrev()");
    hframe->AddFrame(fPrev, new TGLayoutHints(kLHintsCenterX,5,5,1,1));

    fNext = new TGTextButton(hframe,"Next");
    fNext->SetBackgroundColor(mainguicolor);
    fNext->Connect("Clicked()","OnlineGUI",this,"DrawNext()");
    hframe->AddFrame(fNext, new TGLayoutHints(kLHintsCenterX,5,5,1,1));

    fExit = new TGTextButton(hframe,"Exit GUI");
    fExit->SetBackgroundColor(red);
    fExit->Connect("Clicked()","OnlineGUI",this,"CloseGUI()");

    hframe->AddFrame(fExit, new TGLayoutHints(kLHintsCenterX,5,5,1,1));

    TString Buff;
    if(runNumber==0) {
        Buff = "";
    } else {
        Buff = "Run #";
        Buff += runNumber;
    }
    TGString labelBuff(Buff);

    fRunNumber = new TGLabel(hframe,Buff);
    fRunNumber->SetBackgroundColor(mainguicolor);
    hframe->AddFrame(fRunNumber,new TGLayoutHints(kLHintsCenterX,5,5,1,1));

    fPrint = new TGTextButton(hframe,"Print To &File");
    fPrint->SetBackgroundColor(mainguicolor);
    fPrint->Connect("Clicked()","OnlineGUI",this,"PrintToFile()");
    hframe->AddFrame(fPrint, new TGLayoutHints(kLHintsCenterX,5,5,1,1));


    // Set a name to the main frame
    if(fConfig->IsMonitor()) {
        fMain->SetWindowName("Parity ANalysis GUI moNitor");
    } else {
        fMain->SetWindowName("Online Analysis GUI");
    }

    // Map all sub windows to main frame
    fMain->MapSubwindows();

    // Initialize the layout algorithm
    fMain->Resize(fMain->GetDefaultSize());

    // Map main frame
    fMain->MapWindow();

#ifdef DEBUG
    fMain->Print();
#endif

    if(fFileAlive) DoDraw();

    if(fConfig->IsMonitor()) {
        timer = new TTimer();
        if(fFileAlive) {
            timer->Connect(timer,"Timeout()","OnlineGUI",this,"TimerUpdate()");
        } else {
            timer->Connect(timer,"Timeout()","OnlineGUI",this,"CheckRootFile()");
        }
        timer->Start(UPDATETIME);
    }

}

void OnlineGUI::DoDraw()
{
 
  // The main Drawing Routine.
 
#ifdef INTERNALSTYLE
    gStyle->SetOptStat(1110);
    gStyle->SetStatFontSize(0.1);
#endif
    if (fConfig->IsLogx(current_page)) {
        gStyle->SetOptLogx(1);
    } else {
        gStyle->SetOptLogx(0);
    }
    if (fConfig->IsLogy(current_page)) {
        gStyle->SetOptLogy(1);
    } else {
        gStyle->SetOptLogy(0);
    }
    if (fConfig->IsLogz(current_page)) {
        gStyle->SetOptLogz(1);
    } else {
        gStyle->SetOptLogz(0);
    }
#ifdef INTERNALSTYLE
  gStyle->SetTitleH(0.10);
  gStyle->SetTitleW(0.40);
  //gStyle->SetLabelSize(0.10,"X");
  //gStyle->SetLabelSize(0.10,"Y");
  gStyle->SetLabelSize(0.05,"X");
  gStyle->SetLabelSize(0.05,"Y");
  gStyle->SetTitleSize(0.045,"X");
  gStyle->SetTitleSize(0.045,"Y");
  gStyle->SetPadLeftMargin(0.14);
  gStyle->SetNdivisions(505,"X");
  gStyle->SetNdivisions(404,"Y");
  gStyle->SetPalette(1);
  gROOT->ForceStyle();
#endif

    // Determine the dimensions of the canvas..
    UInt_t draw_count = fConfig->GetDrawCount(current_page);
#ifdef INTERNALSTYLE
    if(draw_count>=8) {
        gStyle->SetLabelSize(0.08,"X");
        gStyle->SetLabelSize(0.08,"Y");
    }
#endif
    //   Int_t dim = Int_t(round(sqrt(double(draw_count))));
    pair <UInt_t,UInt_t> dim = fConfig->GetPageDim(current_page);

#ifdef DEBUG
    cout << "Dimensions: " << dim.first << "X"
        << dim.second << endl;
#endif

    // Create a nice clean canvas.
    fCanvas->Clear();
    fCanvas->Divide(dim.first,dim.second);

    drawcommand thiscommand;
    // Draw the histograms.
    for(UInt_t i=0; i<draw_count; i++) {
        thiscommand = fConfig->GetDrawCommand(current_page,i);
        thiscommand = fileObject2command(thiscommand,&fRootFile);
        fCanvas->cd(i+1);
        if (thiscommand.variable == "macro") {
            MacroDraw(thiscommand);
        } else if (thiscommand.objtype.Contains("TH")) {
            HistDraw(thiscommand);
        } else if (thiscommand.objtype.Contains("TCanvas")) {
            CanvasDraw(thiscommand,i+1);
        } else if (thiscommand.objtype.Contains("TGraph")) {
            GraphDraw(thiscommand);
        } else { // otherwise... assume that the objtype is a branch from a tree
            TreeDraw(thiscommand);
        }
    }

    fCanvas->cd();
    fCanvas->Update();

    if(!fPrintOnly) {
        CheckPageButtons();
    }

}

void OnlineGUI::DrawNext()
{
    // Handler for the "Next" button.
    fRadioPage[current_page]->SetState(kButtonUp);
    // The following line triggers DoRadio()
    fRadioPage[current_page+1]->SetState(kButtonDown,true);

}

void OnlineGUI::DrawPrev()
{
    // Handler for the "Prev" button.
    fRadioPage[current_page]->SetState(kButtonUp);
    // The following line triggers DoRadio()
    fRadioPage[current_page-1]->SetState(kButtonDown,true);

}

void OnlineGUI::DoRadio()
{
    // Handle the radio buttons
    //  Find out which button has been pressed..
    //   turn off the previous button...
    //   then draw the appropriate page.
    // This routine also handles the Draw from the Prev/Next buttons
    //   - makes a call to DoDraw()

    UInt_t pagecount = fConfig->GetPageCount();
    TGButton *btn = (TGButton *) gTQSender;
    UInt_t id = btn->WidgetId();

    if (id <= pagecount) {
        fRadioPage[current_page]->SetState(kButtonUp);
    }

    current_page = id;
    if(!fConfig->IsMonitor()) DoDraw();

}

void OnlineGUI::CheckPageButtons()
{
    // Checks the current page to see if it's the first or last page.
    //  If so... turn off the appropriate button.
    //  If not.. turn on both buttons.

    if(current_page==0) {
        fPrev->SetState(kButtonDisabled);
        if(fConfig->GetPageCount()!=1)
            fNext->SetState(kButtonUp);
    } else if(current_page==fConfig->GetPageCount()-1) {
        fNext->SetState(kButtonDisabled);
        if(fConfig->GetPageCount()!=1)
            fPrev->SetState(kButtonUp);
    } else {
        fPrev->SetState(kButtonUp);
        fNext->SetState(kButtonUp);
    }
}

void OnlineGUI::GetFileObjects(RootFileObject* fLocalRootFileObj)
{
    // Utility to find all of the objects within a File (TTree, TH1F, etc).
    //  The pair stored in the vector is <ObjName, ObjType>
    //  For histograms, the title is also stored
    //    (in case the the name is not very descriptive... like when
    //    using h2root)
    //  If there's no good keys.. do nothing.
#ifdef DEBUGGETFILEOBJECTS
    cout << "Keys = " << fLocalRootFileObj->RootFile->ReadKeys() << endl;
#endif
    if(fLocalRootFileObj->RootFile->ReadKeys()==0) {
        fLocalRootFileObj->fUpdate = kFALSE;
        //     delete fLocalRootFileObj->RootFile;
        //     fLocalRootFileObj->RootFile = 0;
        //     CheckRootFile();
        return;
    }
    fLocalRootFileObj->vfileObjects.clear();

    TIter next( (fLocalRootFileObj->RootFile)->GetListOfKeys() );
    TKey *key = new TKey();

    // Do the search
    while((key=(TKey*)next())!=0) {
#ifdef DEBUGGETFILEOBJECTS
        cout << "Key = " << key << endl;
#endif
        TString objname = key->GetName();
        TString objtype = key->GetClassName();
        TString objtitle = key->GetTitle();
#ifdef DEBUGGETFILEOBJECTS
        cout << objname << " " << objtype << endl;
#endif

        if(objtype.Contains("TDirectory")) {
            TDirectory *thisdir = (TDirectory*)fLocalRootFileObj->RootFile->Get(objname);
            TIter nextInDir(thisdir->GetListOfKeys());
            TKey *innerkey = new TKey();
            while((innerkey=(TKey*)nextInDir())!=0) {
                TString iobjname = innerkey->GetName();
                TString iobjtype = innerkey->GetClassName();
                TString iobjtitle = innerkey->GetTitle();
                TString iobjdir = objname;
#ifdef DEBUGGETFILEOBJECTS
                cout << iobjname << " " << iobjtype << endl;
#endif
                fileObject thisObject;
                thisObject.name = iobjname;
                thisObject.type = iobjtype;
                thisObject.title = iobjtitle;
                thisObject.directory = objname;
                fLocalRootFileObj->vfileObjects.push_back(thisObject);
            }
        } else {
            fileObject thisObject;
            thisObject.name = objname;
            thisObject.type = objtype;
            thisObject.title = objtitle;
            thisObject.directory = "";
            fLocalRootFileObj->vfileObjects.push_back(thisObject);
        }

    }
    fLocalRootFileObj->fUpdate = kTRUE;
    delete key;
}

void OnlineGUI::GetTreeVars(RootFileObject* fLocalRootFileObj)
{
    // Utility to find all of the variables (leaf's/branches) within a
    // Specified TTree and put them within the TreeVars vector.
    fLocalRootFileObj->TreeVars.clear();
    TObjArray *branchList;
    vector <TString> currentTree;

    for(UInt_t i=0; i<fLocalRootFileObj->RootTree.size(); i++) {
        currentTree.clear();
        branchList = fLocalRootFileObj->RootTree[i]->GetListOfBranches();
        TIter next(branchList);
        TBranch *brc;

        while((brc=(TBranch*)next())!=0) {
            TString found = brc->GetName();
            // Not sure if the line below is so smart...
            currentTree.push_back(found);
        }
        fLocalRootFileObj->TreeVars.push_back(currentTree);
    }
#ifdef DEBUG2
    for(UInt_t iTree=0; iTree<fLocalRootFileObj->TreeVars.size(); iTree++) {
        cout << "In Tree " << iTree << ": " << endl;
        for(UInt_t i=0; i<fLocalRootFileObj->TreeVars[iTree].size(); i++) {
            cout << fLocalRootFileObj->TreeVars[iTree][i] << endl;
        }
    }
#endif
}


void OnlineGUI::GetRootTree(RootFileObject* fLocalRootFileObj ) {
    // Utility to search a ROOT File for ROOT Trees
    // Fills the RootTree vector
    fLocalRootFileObj->RootTree.clear();

    list <TString> found;
    for(UInt_t i=0; i<fLocalRootFileObj->vfileObjects.size(); i++) {
#ifdef DEBUGGETROOTTREE
        cout << "Object = " << fLocalRootFileObj->vfileObjects[i].type <<
            "     Name = " << fLocalRootFileObj->vfileObjects[i].name << endl;
#endif
        if(fLocalRootFileObj->vfileObjects[i].type.Contains("TTree"))
            found.push_back(fLocalRootFileObj->vfileObjects[i].name);
    }

    // Remove duplicates, then insert into fLocalRootFileObj->RootTree
    found.unique();
    UInt_t nTrees = found.size();

    for(UInt_t i=0; i<nTrees; i++) {
        fLocalRootFileObj->RootTree.push_back( (TTree*) fLocalRootFileObj->RootFile->Get(found.front()) );
        found.pop_front();
    }
    // Initialize the TreeEntries vector
    fLocalRootFileObj->TreeEntries.clear();
    for(UInt_t i=0;i<fLocalRootFileObj->RootTree.size();i++) {
        fLocalRootFileObj->TreeEntries.push_back(0);
    }

}

UInt_t OnlineGUI::GetTreeIndex(TString var, RootFileObject* fLocalRootFileObj ) {
    // Utility to find out which Tree (in fRootFile.RootTree) has the specified
    // variable "var".  If the variable is a collection of Tree
    // variables (e.g. bcm1:lumi1), will only check the first
    // (e.g. bcm1).
    // Returns the correct index.  if not found returns an index 1
    // larger than fRootFile.RootTree.size()

    //  This is for 2d draws... look for the first only
    if(var.Contains(":")) {
        TString first_var = fConfig->SplitString(var,":")[0];
        var = first_var;
    }

    //  This is for variables with multiple dimensions.
    if(var.Contains("[")) {
        TString first_var = fConfig->SplitString(var,"[")[0];
        var = first_var;
    }

    for(UInt_t iTree=0; iTree<fLocalRootFileObj->TreeVars.size(); iTree++) {
        for(UInt_t ivar=0; ivar<fLocalRootFileObj->TreeVars[iTree].size(); ivar++) {
            if(var == fLocalRootFileObj->TreeVars[iTree][ivar]) return iTree;
        }
    }

    return fLocalRootFileObj->RootTree.size()+1;
}

UInt_t OnlineGUI::GetTreeIndexFromName(TString name, RootFileObject* fLocalRootFileObj) {
    // Called by TreeDraw().  Tries to find the Tree index provided the
    //  name.  If it doesn't match up, return a number that's one larger
    //  than the number of found trees.
    for(UInt_t iTree=0; iTree<fLocalRootFileObj->RootTree.size(); iTree++) {
        TString treename = fLocalRootFileObj->RootTree[iTree]->GetName();
        if(name == treename) {
            return iTree;
        }
    }

    return fLocalRootFileObj->RootTree.size()+1;
}

void OnlineGUI::MacroDraw(drawcommand command) {
    // Called by DoDraw(), this will make a call to the defined macro, and
    //  plot it in it's own pad.  One plot per macro, please.

    if(command.macro.IsNull()) {
        cout << "macro command doesn't contain a macro to execute" << endl;
        return;
    }

    if(doGolden) fRootFile.RootFile->cd();
    gROOT->Macro(command.macro);

}

void OnlineGUI::CanvasDraw(drawcommand command,UInt_t padnum) {
    // Called by DoDraw(), this will draw the TCanvas in the current Canvas

    TString rootversion = gROOT->GetVersion();
    TString slash = "/";
    TString rootversion_cut = (fConfig->SplitString(rootversion,slash))[0];
    Double_t ver_num = rootversion_cut.Atof();
    if(ver_num<5.16) {
        TString badstring = "TCanvas Drawing Unsupported in ROOT v"+rootversion;
        BadDraw(badstring);
        return;
    }

    TDirectory *fDir;
    if(command.directory.IsNull()) {
        fDir = 0;
    } else {
        fDir = (TDirectory*)fRootFile.RootFile->Get(command.directory);
    }

    if(fDir) fDir->cd();
    fRootFile.RootFile->cd();
    TCanvas *tcanv = (TCanvas*)fRootFile.RootFile->Get(command.objname);
    fCanvas->cd(padnum);
    tcanv->DrawClonePad();
    delete tcanv;

}

void OnlineGUI::GraphDraw(drawcommand command) {
    // Called by DoDraw(), this will draw the TCanvas in the current Canvas

    TString rootversion = gROOT->GetVersion();
    fRootFile.RootFile->cd();
    TDirectory *fDir;
    if(command.directory.IsNull()) {
        fDir = 0;
    } else {
        fDir = (TDirectory*)fRootFile.RootFile->Get(command.directory);
    }

    if(fDir) fDir->cd();
    TGraph *thisgraph = (TGraph*)fRootFile.RootFile->Get(command.objname);
    TString drawopt = command.type;
    if(drawopt.IsNull()) drawopt = "AP";
    if(!command.title.IsNull()) thisgraph->SetTitle(command.title);
    thisgraph->Draw(drawopt);

}


void OnlineGUI::DoDrawClear(RootFileObject* fLocalRootFileObj) {
    // Utility to grab the number of entries in each tree.  This info is
    // then used, if watching a file, to "clear" the TreeDraw
    // histograms, and begin looking at new data.
    for(UInt_t i=0; i<fLocalRootFileObj->TreeEntries.size(); i++) {
        fLocalRootFileObj->TreeEntries[i] = (Int_t) fLocalRootFileObj->RootTree[i]->GetEntries();
    }


}

void OnlineGUI::TimerUpdate() {
    // Called periodically by the timer, if "watchfile" is indicated
    // in the config.  Reloads the ROOT file, and updates the current page.
#ifdef DEBUG
    cout << "Update Now" << endl;
#endif

    if(fRootFile.RootFile->IsZombie() || (fRootFile.RootFile->GetSize() == -1)
            || (fRootFile.RootFile->ReadKeys()==0)) {
        cout << "New run not yet available.  Waiting..." << endl;
        fRootFile.RootFile->Close();
        delete fRootFile.RootFile;
        fRootFile.RootFile = 0;
        timer->Reset();
        timer->Disconnect();
        timer->Connect(timer,"Timeout()","OnlineGUI",this,"CheckRootFile()");
        return;
    }
    for(UInt_t i=0; i<fRootFile.RootTree.size(); i++) {
        fRootFile.RootTree[i]->Refresh();
    }
    DoDraw();
    timer->Reset();

}

void OnlineGUI::BadDraw(TString errMessage) {
    // Routine to display (in Pad) why a particular draw method has
    // failed.
    TPaveText *pt = new TPaveText(0.1,0.1,0.9,0.9,"brNDC");
    pt->SetBorderSize(3);
    pt->SetFillColor(10);
    pt->SetTextAlign(22);
    pt->SetTextFont(72);
    pt->SetTextColor(2);
    pt->AddText(errMessage.Data());
    pt->Draw();
    //   cout << errMessage << endl;

}


void OnlineGUI::CheckRootFile() {
    // Check the path to the rootfile (should follow symbolic links)
    // ... If found:
    //   Reopen new root file,
    //   Reconnect the timer to TimerUpdate()

    if(gSystem->AccessPathName(fConfig->GetRootFile())==0) {
        cout << "Found the new run" << endl;
        if(OpenRootFile()==0) {
            timer->Reset();
            timer->Disconnect();
            timer->Connect(timer,"Timeout()","OnlineGUI",this,"TimerUpdate()");
        }
    } else {
        TString rnBuff = "Waiting for run";
        fRunNumber->SetText(rnBuff.Data());
        hframe->Layout();
    }

}

Int_t OnlineGUI::OpenRootFile() {


    fRootFile.RootFile = new TFile(fConfig->GetRootFile(),"READ");
    if(fRootFile.RootFile->IsZombie() || (fRootFile.RootFile->GetSize() == -1)
            || (fRootFile.RootFile->ReadKeys()==0)) {
        cout << "New run not yet available.  Waiting..." << endl;
        fRootFile.RootFile->Close();
        delete fRootFile.RootFile;
        fRootFile.RootFile = 0;
        timer->Reset();
        timer->Disconnect();
        timer->Connect(timer,"Timeout()","OnlineGUI",this,"CheckRootFile()");
        return -1;
    }

    // Update the runnumber
    ObtainRunNumber();
    if(runNumber != 0) {
        TString rnBuff = "Run #";
        rnBuff += runNumber;
        fRunNumber->SetText(rnBuff.Data());
        hframe->Layout();
    }

    // Open the Root Trees.  Give a warning if it's not there..
    GetFileObjects(&fRootFile);
    if (fRootFile.fUpdate) { // Only do this stuff if there are valid keys
        GetRootTree(&fRootFile);
        GetTreeVars(&fRootFile);
        for(UInt_t i=0; i<fRootFile.RootTree.size(); i++) {
            if(fRootFile.RootTree[i]==0) {
                fRootFile.RootTree.erase(fRootFile.RootTree.begin() + i);
            }
        }
        DoDraw();
    } else {
        return -1;
    }
    return 0;

}

void OnlineGUI::HistDraw(drawcommand command) {
    // Called by DoDraw(), this will plot a histogram.

    Bool_t showGolden=kFALSE;
    if(doGolden) showGolden=kTRUE;
    Bool_t showStat=kTRUE;
    TString htitle;

    if(command.noshowgolden=="noshowgolden")
        showGolden = kFALSE;
    if(command.nostat=="nostat")
        showStat=kFALSE;
    htitle = command.title;

    TString type=command.type;
    TDirectory *fDir;

    if(command.directory.IsNull()) {
        fDir = 0;
    } else {
        fDir = (TDirectory*)fRootFile.RootFile->Get(command.directory);
    }

    // Determine dimensionality of histogram, then draw it
    if(command.objtype.Contains("TH1")) {
        // Operation for TH1
        fRootFile.RootFile->cd();
        if(fDir) fDir->cd();
        fRootFile.mytemp1d = (TH1D*)gDirectory->Get(command.objname);
        if(!fRootFile.mytemp1d) BadDraw(command.variable+" not found");
        if(fRootFile.mytemp1d->GetEntries()==0) {
            BadDraw("Empty Histogram");
        } else {
            if(showGolden) {
                fGoldenFile.RootFile->cd();
                if(fDir) fDir->cd();
                fGoldenFile.mytemp1d = (TH1D*)gDirectory->Get(command.objname);
                fGoldenFile.mytemp1d->SetLineColor(30);
                fGoldenFile.mytemp1d->SetFillColor(30);
                Int_t fillstyle=3027;
                if(fPrintOnly) fillstyle=3010;
                fGoldenFile.mytemp1d->SetFillStyle(fillstyle);
                fRootFile.mytemp1d->SetStats(showStat);
                fGoldenFile.mytemp1d->SetStats(kFALSE);
                fGoldenFile.mytemp1d->Draw();
                if(!htitle.IsNull()) fGoldenFile.mytemp1d->SetTitle(htitle);
                fRootFile.mytemp1d->Draw("sames"+type);
            } else {
                fRootFile.mytemp1d->SetStats(showStat);
                if(!htitle.IsNull()) fRootFile.mytemp1d->SetTitle(htitle);
                fRootFile.mytemp1d->Draw(type);

                // Force TH1s' y-axis to start at 1 if not a logy plot
                if(gStyle->GetOptLogy() == 0)
                    fRootFile.mytemp1d->SetMinimum(1);
            }
        }
        return;
    }

    if(command.objtype.Contains("TH2")) {
        // Operation for TH2
        if(!showGolden) fRootFile.RootFile->cd();
        if(fDir) fDir->cd();
        fRootFile.mytemp2d = (TH2D*)gDirectory->Get(command.objname);
        if(fRootFile.mytemp2d->GetEntries()==0) {
            BadDraw("Empty Histogram");
        } else {
            if(showGolden) {
                fGoldenFile.RootFile->cd();
                if(fDir) fDir->cd();
                fGoldenFile.mytemp2d = (TH2D*)gDirectory->Get(command.objname);
                fGoldenFile.mytemp2d->SetMarkerColor(2);
                fRootFile.mytemp2d->SetStats(showStat);
                fGoldenFile.mytemp2d->SetStats(kFALSE);
                fGoldenFile.mytemp2d->Draw(type);
                if(!htitle.IsNull()) fGoldenFile.mytemp2d->SetTitle(htitle);
                fRootFile.mytemp2d->Draw("sames"+type);
            } else {
                fRootFile.mytemp2d->SetStats(showStat);
                if(!htitle.IsNull()) fGoldenFile.mytemp2d->SetTitle(htitle);
                fRootFile.mytemp2d->Draw(type);
                fRootFile.mytemp2d->Draw("colz");
            }
        }
        return;
    }

    if(command.objtype.Contains("TH3")) {
        // Operation for TH3
        if(!showGolden) fRootFile.RootFile->cd();
        if(fDir) fDir->cd();
        fRootFile.mytemp3d = (TH3D*)gDirectory->Get(command.objname);
        if(fRootFile.mytemp3d->GetEntries()==0) {
            BadDraw("Empty Histogram");
        } else {
            fRootFile.mytemp3d->Draw();
            if(showGolden) {
                fGoldenFile.RootFile->cd();
                if(fDir) fDir->cd();
                fGoldenFile.mytemp3d = (TH3D*)gDirectory->Get(command.objname);
                fGoldenFile.mytemp3d->SetMarkerColor(2);
                fRootFile.mytemp3d->SetStats(showStat);
                fGoldenFile.mytemp3d->SetStats(kFALSE);
                fGoldenFile.mytemp3d->Draw(type);
                if(!htitle.IsNull()) fGoldenFile.mytemp3d->SetTitle(htitle);
                fRootFile.mytemp3d->Draw("sames"+type);
            } else {
                fRootFile.mytemp3d->SetStats(showStat);
                if(!htitle.IsNull()) fGoldenFile.mytemp3d->SetTitle(htitle);
                fRootFile.mytemp3d->Draw(type);
            }
        }
        return;
    }

}

drawcommand OnlineGUI::fileObject2command(drawcommand command, RootFileObject* fLocalRootFileObj) {
    // Fill in gaps in the drawcommand with items from the
    // vfileObject.  This elimates the need for duplicate code in
    // the different *Draw routines.
    for(UInt_t i=0; i<fLocalRootFileObj->vfileObjects.size(); i++) {
        if(fLocalRootFileObj->vfileObjects[i].name==command.variable ||
                fLocalRootFileObj->vfileObjects[i].title==command.variable ) {
            command.objname = fLocalRootFileObj->vfileObjects[i].name;
            command.objtitle = fLocalRootFileObj->vfileObjects[i].title;
            command.objtype = fLocalRootFileObj->vfileObjects[i].type;
            command.directory = fLocalRootFileObj->vfileObjects[i].directory;
#ifdef DEBUGf2command
            cout << command.directory << " "
                << command.objtype  << " "
                << command.objname
                << endl;
#endif
            return command;
        }
    }
    return command;


}

void OnlineGUI::TreeDraw(drawcommand command) {
    // Called by DoDraw(), this will plot a Tree Variable

    TString var = command.variable;
    Bool_t showGolden=kFALSE;
    if(doGolden) showGolden=kTRUE;

    Bool_t showStat=kTRUE;
    if(command.nostat=="nostat") showStat=kFALSE;

    //TObject *hobj;

    // Combine the cuts (definecuts and specific cuts)
    TCut cut = "";
    TString tempCut;
    if(!command.cut.IsNull()) {
        tempCut = command.cut;
        vector <TString> cutIdents = fConfig->GetCutIdent();
        for(UInt_t i=0; i<cutIdents.size(); i++) {
            if(tempCut.Contains(cutIdents[i])) {
                TString cut_found = (TString)fConfig->GetDefinedCut(cutIdents[i]);
                tempCut.ReplaceAll(cutIdents[i],cut_found);
            }
        }
        cut = (TCut)tempCut;
    }

    // Determine which Tree the variable comes from, then draw it.
    UInt_t iTree;
    if(command.treename.IsNull()) {
        iTree = GetTreeIndex(var,&fRootFile);
    } else {
        iTree = GetTreeIndexFromName(command.treename,&fRootFile);
    }
    TString drawopt = command.type;
    Int_t errcode=0;
    if(drawopt.IsNull() && var.Contains(":")) drawopt = "cont";
    if(drawopt=="scat") drawopt = "";

    fRootFile.RootFile->cd();
    if (iTree <= fRootFile.RootTree.size() ) {
        TObjArray* tok = var.Tokenize(">()");
        TString myvar   = ((TObjString*)tok->First())->GetString();
        TString hname   = "h";
        TString histdef = ((TObjString*)tok->Last())->GetString();
        if(tok->GetEntries() == 1) histdef = "";        // ie "var[0]"
        if(tok->GetEntries() == 2) {
            if(! histdef.Contains(",") ) {             // ie "var[0]>>h1"
                hname = histdef;
                histdef = "";
            }
        }
        if(tok->GetEntries() == 3) hname = ((TObjString*)tok->At(1))->GetString();  // ie "var[0]>>h1(100,0,100)"
        delete tok;
        TString tmp = var + tempCut;
        hname = Form("%s_%u",hname.Data(),tmp.Hash());      // unique id so caching histos works

        errcode=1;
        TObject *hobj  = gDirectory->Get(hname);
        if(hobj == NULL) {
            errcode = fRootFile.RootTree[iTree]->Draw(myvar+">>"+hname+"("+histdef+")",cut,drawopt,
                    1000000000,fRootFile.TreeEntries[iTree]);
            hobj = gDirectory->Get(hname);
        }
        TH1F *mainhist = (TH1F*)hobj;
        mainhist->Draw(drawopt);
        //tmp = myvar+">>"+hname+"("+histdef+")";
        //printf("hist: '%s',  opt: '%s',  \"%s\"\n", hname.Data(), drawopt.Data(),tmp.Data());

        if(errcode==-1) {
            BadDraw(var+" not found");
        } else if (errcode!=0) {
            TString htype = hobj->ClassName();
            if(! htype.Contains("TH1F") ) {
                //printf("skipping.\n");
                showGolden=kFALSE;
            }
            if(showGolden) {
                errcode=1;
                TString goldname = "gold"+hname;
                TH1F *goldhist = (TH1F*)gDirectory->Get(goldname);
                if(goldhist == NULL) {
                    goldhist = (TH1F*)mainhist->Clone(hname);
                    goldhist->SetName(goldname);
                    errcode = fGoldenFile.RootTree[iTree]->Project(goldname,myvar,cut);
                }
                if(errcode>0) {
                    goldhist->SetFillColor(30);
                    Int_t fillstyle=3027;
                    if(fPrintOnly) fillstyle=3010;
                    goldhist->SetFillStyle(fillstyle);
                    goldhist->Draw(drawopt);
                    mainhist->SetFillColor(0);
                    mainhist->SetFillStyle(1001);
                    mainhist->Draw("same");
                    if(!command.title.IsNull()) goldhist->SetTitle(command.title);
                    if(!showStat)               goldhist->SetStats(kFALSE);
                }
            } else {
                if(!command.title.IsNull()) mainhist->SetTitle(command.title);
                if(!showStat)               mainhist->SetStats(kFALSE);
            }
        } else {
            BadDraw("Empty Histogram");
        }
    } else {
        BadDraw(var+" not found");
        if (fConfig->IsMonitor()){
            // Maybe we missed it... look again.  I dont like the code
            // below... maybe I can come up with something better
            GetFileObjects(&fRootFile);
            GetRootTree(&fRootFile);
            GetTreeVars(&fRootFile);
        }
    }

}

void OnlineGUI::ObtainRunNumber()
{
    // Utility to obtain the runnumber through a helper macro
    //  "GetRunNumber.C"

    if (runNumber==0) runNumber = GetRunNumber();
#ifdef DEBUG
    cout << "Runnumber from file: " << runNumber << endl;
#endif
}

void OnlineGUI::PrintToFile()
{
    // Routine to print the current page to a File.
    //  A file dialog pop's up to request the file name.
    fCanvas = fEcanvas->GetCanvas();
    gStyle->SetPaperSize(20,24);
    static TString dir(".");
    TGFileInfo fi;
    const char *myfiletypes[] =
    { "All files","*",
        "PDF files","*.pdf",
        "PNG files","*.png",
        "GIF files","*.gif",
        "JPG files","*.jpg",
        0,               0 };
    fi.fFileTypes = myfiletypes;
    fi.fIniDir    = StrDup(dir.Data());
    cout << " INitiail = " << fi.fIniDir << endl;
    new TGFileDialog(gClient->GetRoot(), fMain, kFDSave, &fi);
    if(fi.fFilename!=NULL) {
        fCanvas->Print(fi.fFilename);
    }
}

void OnlineGUI::PrintPages() {
    // Routine to go through each defined page, and print the output to
    // a postscript file. (good for making sample histograms).

    // Open the RootFile
    //  unless we're watching a file.
    fRootFile.RootFile = new TFile(fConfig->GetRootFile(),"READ");
    if(!fRootFile.RootFile->IsOpen()) {
        cout << "ERROR:  rootfile: " << fConfig->GetRootFile()
            << " does not exist"
            << endl;
#ifdef STANDALONE
        gApplication->Terminate();
#else
        return;
#endif
    } else {
        fFileAlive = kTRUE;
        ObtainRunNumber();
        // Open the Root Trees.  Give a warning if it's not there..
        GetFileObjects(&fRootFile);
        GetRootTree(&fRootFile);
        GetTreeVars(&fRootFile);
        for(UInt_t i=0; i<fRootFile.RootTree.size(); i++) {
            if(fRootFile.RootTree[i]==0) {
                fRootFile.RootTree.erase(fRootFile.RootTree.begin() + i);
            }
        }

    }
    TString goldenfilename=fConfig->GetGoldenFile();
    if(!goldenfilename.IsNull()) {
        fGoldenFile.RootFile = new TFile(goldenfilename,"READ");
        if(!fGoldenFile.RootFile->IsOpen()) {
            cout << "ERROR: goldenrootfile: " << goldenfilename
                << " does not exist.  Oh well, no comparison plots."
                << endl;
            doGolden = kFALSE;
            fGoldenFile.RootFile=NULL;
        } else {
            // Open the Root Trees.  Give a warning if it's not there..
            GetFileObjects(&fGoldenFile);
            GetRootTree(&fGoldenFile);
            GetTreeVars(&fGoldenFile);
            for(UInt_t i=0; i<fGoldenFile.RootTree.size(); i++) {
                if(fGoldenFile.RootTree[i]==0) {
                    fGoldenFile.RootTree.erase(fGoldenFile.RootTree.begin() + i);
                }
            }
            doGolden = kTRUE;
        }
    } else {
        doGolden=kFALSE;
        fGoldenFile.RootFile=NULL;
    }

    // I'm not sure exactly how this works.  But it does.
    fCanvas = new TCanvas("fCanvas","trythis",850,1100);
    //   TCanvas *maincanvas = new TCanvas("maincanvas","whatever",850,1100);
    //   maincanvas->SetCanvas(fCanvas);
    TLatex *lt = new TLatex();

    TString plotsdir = fConfig->GetPlotsDir();
    Bool_t useJPG = kFALSE;
    if(!plotsdir.IsNull()) useJPG = kTRUE;

    //  TString filename = "summaryplots";
    TString ConfigStartAll=fConfig->GetConfigStart();
    TObjArray *tx= ConfigStartAll.Tokenize("/");
    TString temp=((TObjString *)(tx->At(tx->GetEntries()-1)))->String();
    Ssiz_t pos=temp.Index(".cfg");
    TString filename=temp(0,pos);
    if(runNumber!=0) {
        filename += "_";
        filename += runNumber;
    }
    if(useJPG) {
        filename.Prepend(plotsdir+"/");
        filename += "_pageXXXX.jpg";
    }
    else filename += ".pdf";

    TString pagehead = "Summary Plots";
    if(runNumber!=0) {
        pagehead += "(Run #";
        pagehead += runNumber;
        pagehead += ")";
    }
    pagehead += ": ";

    gStyle->SetPalette(1);
    gStyle->SetTitleX(0.5);
    gStyle->SetTitleY(0.95);
    gStyle->SetPadBorderMode(0);
    gStyle->SetHistLineColor(1);
    gStyle->SetHistFillColor(1);
    cout << filename << endl;
    if(!useJPG) fCanvas->Print(filename+"[");
    TString origFilename = filename;
    for(UInt_t i=0; i<fConfig->GetPageCount(); i++) {
        current_page=i;
        DoDraw();
        TString pagename = pagehead + fConfig->GetPageTitle(current_page);
        lt->SetTextSize(0.025);
        lt->DrawLatex(0.05,0.98,pagename);
        if(useJPG) {
            filename = origFilename;
            filename.ReplaceAll("XXXX",Form("%d",current_page));
            cout << "Printing page " << current_page
                << " to file = " << filename << endl;
        }
        fCanvas->Print(filename);
    }
    if(!useJPG) fCanvas->Print(filename+"]");
    //

    cout << "Writing histograms to root file '" << filename << "'." << endl;

    filename.Resize(filename.Last('.'));
    filename += ".root";
    TFile* hroot = new TFile(filename,"recreate");

    UInt_t pageCount = fConfig->GetPageCount();
    for (UInt_t iPage=0; iPage<pageCount; ++iPage) {
        UInt_t drawCount = fConfig->GetDrawCount(iPage);
        for (UInt_t iDraw=0; iDraw<drawCount; ++iDraw) {
            TString macro = fConfig->GetDrawCommand(iPage, iDraw).macro;
            if (macro != "") {
                TString histName = macro.Remove(0, macro.First('\"')+1);
                histName.Resize(macro.First('\"'));

                fRootFile.RootFile->cd();
                TH1F* h1f = dynamic_cast<TH1F*>(gDirectory->Get(histName));
                TH2F* h2f = dynamic_cast<TH2F*>(gDirectory->Get(histName));
                hroot->cd();

                if (h1f) h1f->Write();
                else if (h2f) h2f->Write();
                else cout << histName << " not found!" << endl;
            }
        }
    }

    hroot->Write();
    hroot->Close();
    delete hroot;

    cout << "Root file '" << filename << "' done." << endl;

    //
#ifdef STANDALONE
    gApplication->Terminate();
#endif
}

void OnlineGUI::MyCloseWindow()
{
    fMain->SendCloseMessage();
    cout << "OnlineGUI Closed." << endl;
    if(timer!=NULL) {
        timer->Stop();
        delete timer;
    }
    delete fPrint;
    delete fExit;
    delete fRunNumber;
    delete fPrev;
    delete fNext;
    delete wile;
    for(UInt_t i=0; i<fConfig->GetPageCount(); i++)
        delete fRadioPage[i];
    delete hframe;
    delete fEcanvas;
    delete fBottomFrame;
    delete vframe;
    delete fTopframe;
    delete fMain;
    if(fGoldenFile.RootFile!=NULL) delete fGoldenFile.RootFile;
    if(fRootFile.RootFile!=NULL) delete fRootFile.RootFile;
    delete fConfig;

#ifdef STANDALONE
    gApplication->Terminate();
#endif
}

void OnlineGUI::CloseGUI()
{
    // Routine to take care of the Exit GUI button
    fMain->SendCloseMessage();
}

OnlineGUI::~OnlineGUI()
{
    //  fMain->SendCloseMessage();
    if(timer!=NULL) {
        timer->Stop();
        delete timer;
    }
    delete fPrint;
    delete fExit;
    delete fRunNumber;
    delete fPrev;
    delete fNext;
    delete wile;
    for(UInt_t i=0; i<fConfig->GetPageCount(); i++)
        delete fRadioPage[i];
    delete hframe;
    delete fEcanvas;
    delete vframe;
    delete fBottomFrame;
    delete fTopframe;
    delete fMain;
    if(fGoldenFile.RootFile!=NULL) delete fGoldenFile.RootFile;
    if(fRootFile.RootFile!=NULL) delete fRootFile.RootFile;
    delete fConfig;
}

void online(TString type="standard",UInt_t run=0,Bool_t printonly=kFALSE,Bool_t rootonly=kFALSE)
{
    // "main" routine.  Run this at the ROOT commandline.

    if(printonly || rootonly) {
        if(!gROOT->IsBatch()) {
#ifdef STANDALONE
            gROOT->SetBatch();
#else
            cout << "Sorry... the print summary plots option only works "
                << "in BATCH mode." << endl;
            return;
#endif
        }
    }

    OnlineConfig *fconfig = new OnlineConfig(type);
    //    OnlineConfig *fconfig = new OnlineConfig("halla");

    if(!fconfig->ParseConfig()) {
#ifdef STANDALONE
        gApplication->Terminate();
#else
        return;
#endif
    }

    if(run!=0) fconfig->OverrideRootFile(run);

    new OnlineGUI(*fconfig,printonly,rootonly,run);

}

#ifdef STANDALONE
void Usage()
{
    cerr << "Usage: online [-r] [-f] [-P]"
        << endl;
    cerr << "Options:" << endl;
    cerr << "  -r : runnumber" << endl;
    cerr << "  -f : configuration file" << endl;
    cerr << "  -P : Only Print Summary Plots" << endl;
    cerr << "  -R : Only Rootfile " << endl;
    cerr << endl;

}

int main(int argc, char **argv)
{
    TString type="default";
    UInt_t run=0;
    Bool_t printonly=kFALSE;
    Bool_t rootonly=kFALSE;
    Bool_t showedUsage=kFALSE;

    TApplication theApp("App",&argc,argv,NULL,-1);

    for(Int_t i=1;i<theApp.Argc();i++)
    {
        TString sArg = theApp.Argv(i);
        if(sArg=="-f") {
            type = theApp.Argv(++i);
            cout << " File specifier: "
                <<  type << endl;
        } else if (sArg=="-r") {
            run = atoi(theApp.Argv(++i));
            cout << " Runnumber: "
                << run << endl;
        } else if (sArg=="-P") {
            printonly = kTRUE;
            cout <<  " PrintOnly" << endl;
        } else if (sArg=="-R") {
            rootonly = kTRUE;
            cout <<  " RootOnly" << endl;
        } else if (sArg=="-h") {
            if(!showedUsage) Usage();
            showedUsage=kTRUE;
            return 0;
        } else {
            cerr << "\"" << sArg << "\"" << " not recognized.  Ignored." << endl;
            if(!showedUsage) Usage();
            showedUsage=kTRUE;
        }
    }

    online(type,run,printonly,rootonly);
    theApp.Run();

    return 0;
}

#endif
