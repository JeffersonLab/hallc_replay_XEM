#include <TFile.h>
#include <TTree.h>

#include "ScalerCalib.h"

using namespace std;

ofstream outfile;

//_________________________________

ScalerCalib::ScalerCalib(string name) :
  fName(name), fPrintFlag(DEFAULT)
{

  filename = "dummyname";

}

//_________________________________

ScalerCalib::~ScalerCalib()
{

}

//_________________________________

int ScalerCalib::Run()
{

  FillContainer();

  if(evnum.size() < 1)
    {
      cout << "ERROR: empty container! Check the input file" << endl;
      return -1;
    }

  int pos = filename.find("scalers_");
  runstr = (filename.substr(pos+8)).substr(0,4);

  ofilename = "bcmcurrent_" + runstr + ".param";
  outfile.open(ofilename.c_str());

  outfile << "num_scal_reads = " << evnum.size();
  outfile << "\n" << "\n";
  outfile << "scal_read_bcm1_current = ";
  PrintContainer(bcm1);

  outfile << "\n" << "\n";
  outfile << "scal_read_bcm2_current = ";
  PrintContainer(bcm2);

  if(fPrintFlag == ALL)
    {
      outfile << "\n" << "\n";
      outfile << "scal_read_bcm4a_current = ";
      PrintContainer(bcm4a);

      outfile << "\n" << "\n";
      outfile << "scal_read_bcm4b_current = ";
      PrintContainer(bcm4b);

      outfile << "\n" << "\n";
      outfile << "scal_read_bcm17_current = ";
      PrintContainer(bcm17);
    }
  
  //  outfile.close();

  return 0;
  
}

//_________________________________

int ScalerCalib::PrintContainer(ScalerContainer sc)
{
  
  if(sc.size() < 1)
    if(evnum.size() < 1)
      {
	cout << "ERROR: empty container!" << endl;
	return -1;
      }

  for(SCIterator i = sc.begin(); i != sc.end()-1; ++i)
    {
      outfile << *i << ", ";
    }
  
  outfile << sc.back();

  /*
  stringstream sstr;
  string result;

  std::copy(sc.begin(), sc.end()-1, ofstream_iterator<double>(sstr, ", "));
  sstr << sc.end();  

  result = sstr.str();
  outfile << result;

  result = "";
  sstr.str("");
  sstr.clear();
  */

  return 0;
}

//_________________________________


int ScalerCalib::FillContainer()
{

  ClearContainers();

  TFile* file = new TFile(filename.c_str());
  if(!file->IsOpen())
    {
      cout << "ERROR: " << filename.c_str() << " is not open!" << endl;
      return -1;      
    }
  
  if(! file->GetListOfKeys()->Contains("TSH") )
    {
      cout << "ERROR: Couldn't find the Scaler Tree"  << endl;
      return -1;      
    }

  TTree* T = (TTree*)file->Get("TSH");  

  double evnumber;
  double bcm1_current;
  double bcm2_current;
  double bcm4a_current;
  double bcm4b_current;
  double bcm17_current;

  string bname[5];
  string sarray[5] = {".BCM1.scalerCurrent",
		      ".BCM2.scalerCurrent", 
		      ".BCM4A.scalerCurrent",
		      ".BCM4B.scalerCurrent",
		      ".BCM17.scalerCurrent"};

  for(int i=0; i<5; i++)
    {
      bname[i] = "";
      bname[i] += fName;
      bname[i] += sarray[i];
    }

  T->SetBranchAddress("evNumber",       &evnumber);
  T->SetBranchAddress(bname[0].c_str(), &bcm1_current);
  T->SetBranchAddress(bname[1].c_str(), &bcm2_current);
  T->SetBranchAddress(bname[2].c_str(), &bcm4a_current);
  T->SetBranchAddress(bname[3].c_str(), &bcm4b_current);
  T->SetBranchAddress(bname[4].c_str(), &bcm17_current);

  Long64_t nentries = T->GetEntries();
  for(Long64_t ientry=0; ientry<nentries; ientry++)
    {
      T->GetEntry(ientry);

      evnum.push_back(evnumber);
      bcm1.push_back(bcm1_current);
      bcm2.push_back(bcm2_current);
      bcm4a.push_back(bcm4a_current);
      bcm4b.push_back(bcm4b_current);
      bcm17.push_back(bcm17_current);

    }

  file->Close();

  return 0;
}

//_________________________________

void ScalerCalib::ClearContainers()
{

  evnum.clear();
  bcm1.clear();
  bcm2.clear();
  bcm4a.clear();
  bcm4b.clear();
  bcm17.clear();

}

ClassImp(ScalerCalib)
