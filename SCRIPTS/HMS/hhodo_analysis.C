
// Custom analysis of the hodoscope individual channels
// Author: Eric Pooser, pooser@jlab.org, 01/06/2017
// Script adapted from hcana/examples/hhodrawhists.C

//#include "TF1.h"

#define NPLANES  4
#define NSIDES   2
#define NSIGNALS 2
#define MAXBARS  16

TString  SPECTROMETER = "H";
TString  DETECTOR     = "hod";

TString  plane_names[NPLANES] = {"1x", "1y", "2x", "2y"};
Int_t    nbars[NPLANES]       = {16, 10, 16, 10};
TString  sides[NSIDES]        = {"neg", "pos"};
TString  signals[NSIGNALS]    = {"adc", "tdc"};

static const Double_t ADC_MIN   = 0.0;
static const Double_t ADC_MAX   = 25000.0;
static const Int_t    ADC_NBINS = 2500;

static const Double_t TDC_MIN   = 0.0;
static const Double_t TDC_MAX   = 8000.0;
static const Int_t    TDC_NBINS = 800;

Int_t    nhits[NPLANES][NSIDES][NSIGNALS];
Double_t paddles[NPLANES][NSIDES][NSIGNALS][MAXBARS];
Double_t values[NPLANES][NSIDES][NSIGNALS][MAXBARS];

TH1F *h[NPLANES*NSIDES*NSIGNALS*MAXBARS];
TFile *rif, *rof;

TTree *T;

TString base_name, ndata_name, padlist_name, vallist_name;

Int_t nbins, hmin, hmax, hindex, hindex_base;

TString ibarname, title, name;

Int_t ibar;
Double_t val;

void hhodo_analysis(Int_t RunNumber=0, Int_t MaxEvent=0) {

  // Get RunNumber and MaxEvent if not provided.
  if(RunNumber == 0) {
    cout << "Enter a Run Number (-1 to exit): ";
    cin >> RunNumber;
    if( RunNumber<=0 ) return;
  }
  if(MaxEvent == 0) {
    cout << "\nNumber of Events to analyze: ";
    cin >> MaxEvent;
    if(MaxEvent == 0) {
      cerr << "...Invalid entry\n";
      exit;
    }
  } 

  // Define root in/out files
  rif  = new TFile(Form("ROOTfiles/hhodo_htrig_replay_%d.root", RunNumber), "READ");
  rof = new TFile(Form("ROOTfiles/hhodo_analysis_%d.root", RunNumber), "RECREATE");
  rif->cd();
  
  // Acquire tree from root in file
  T = new TTree();
  T = (TTree*) rif->Get("T");

  for(Int_t iplane = 0; iplane < NPLANES; iplane++) {
    for(Int_t iside = 0; iside < NSIDES; iside++) {
      for(Int_t isignal = 0; isignal < NSIGNALS; isignal++) {

	base_name = SPECTROMETER + "." + DETECTOR + "." +
	  plane_names[iplane] + "." + sides[iside] + signals[isignal];
	ndata_name   = "Ndata." + base_name + "pad";
	padlist_name = base_name + "pad";
	vallist_name = base_name + "val";

	// Set branch addresses
	T->SetBranchAddress(ndata_name,   &nhits[iplane][iside][isignal]);
	T->SetBranchAddress(padlist_name, &paddles[iplane][iside][isignal][0]);
	T->SetBranchAddress(vallist_name, &values[iplane][iside][isignal][0]);

	// Create histograms
	// ADC and TDC histogram for each
	if(signals[isignal] == "adc")
	  nbins = ADC_NBINS; hmin = ADC_MIN; hmax = ADC_MAX;
	if(signals[isignal] == "tdc")
	  nbins = TDC_NBINS; hmin = TDC_MIN; hmax = TDC_MAX;
	
	rof->cd();
	for(Int_t ibar = 0; ibar < nbars[iplane]; ibar++) {
	  hindex = iplane*NSIDES*NSIGNALS*MAXBARS
	    + iside*NSIGNALS*MAXBARS + isignal*MAXBARS + ibar;
	  ibarname = Form("%d", ibar + 1);
	  title = "h" + plane_names[iplane] + ibarname + sides[iside]
	    + " " + signals[isignal];
	  name = "h"; name += Form("%d", hindex);
	  //  cout << name << " " << title << " " << nbins << " " << hmin << " "
	  //       << hmax << endl;
	  h[hindex] = new TH1F(name, title, nbins, hmin, hmax);
	}
	rif->cd();

      }  // Signal loop
    }  // Side loop
  }  // Plane loop

  // Loop over the events, filling the histograms
  for(Int_t ievent = 0, N = T->GetEntries(); ievent < N; ievent++) {
    T->GetEntry(ievent);
    for(Int_t iplane = 0; iplane < NPLANES; iplane++){
      for(Int_t iside = 0;iside < NSIDES; iside++) {
	for(Int_t isignal = 0;isignal < NSIGNALS; isignal++) {

	  hindex_base = iplane*NSIDES*NSIGNALS*MAXBARS
	    + iside*NSIGNALS*MAXBARS + isignal*MAXBARS;

	  for(Int_t ihit = 0; ihit < nhits[iplane][iside][isignal]; ihit++) {
	    ibar = TMath::Nint(paddles[iplane][iside][isignal][ihit]) - 1;
	    val = values[iplane][iside][isignal][ihit];
	    hindex = hindex_base + ibar;
	    h[hindex]->Fill(val);
	  }  // Hit loop
	}  // Signal loop
      }  // Side loop
    }  // Plane loop

    // Display or save the histograms

  }

  // Write the root out file
  rof->cd(); rof->Write();
  
}
