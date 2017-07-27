// UserScript.C
//
// Helper macro to build additional histograms

void UserScript()
{

  const UInt_t NPLANES  = 4;
  const UInt_t NSIDES   = 2;
  const UInt_t MAXBARS  = 16;
  const UInt_t NSIGNALS = 2;
  const UInt_t NADCSIGNALS = 2;
  const UInt_t NTDCSIGNALS = 2;

  const TString SPECTROMETER = "H";
  const TString DETECTOR = "hod";

  const TString plane_names[NPLANES] = {"1x", "1y", "2x", "2y"};
  const UInt_t  nbars[NPLANES] = {16, 10, 16, 10};
  const TString sides[NSIDES] = {"neg", "pos"};
  const TString signals[NSIGNALS] = {"Adc", "Tdc"};
  const TString adc_signals[NADCSIGNALS] = {"Ped", "PedRaw"};
  const TString tdc_signals[NTDCSIGNALS] = {"Time", "TimeRaw"};
  
  const Double_t ADC_MIN    = 0.0;
  const Double_t ADC_MAX    = 25000.0;
  const Int_t    ADC_NBINS  = 2500;
  const Double_t TDC_MIN    = -8000.0;
  const Double_t TDC_MAX    = 8000.0;
  const Int_t    TDC_NBINS  = 1600;

  Int_t nadchits[NPLANES][NSIDES][NSIGNALS];
  Int_t ntdchits[NPLANES][NSIDES][NSIGNALS];

  Double_t adc_paddles[NPLANES][NSIDES][NSIGNALS][MAXBARS];
  Double_t tdc_paddles[NPLANES][NSIDES][NSIGNALS][MAXBARS];

  Double_t ped_values[NPLANES][NSIDES][NADCSIGNALS][MAXBARS];
  Double_t pedraw_values[NPLANES][NSIDES][NADCSIGNALS][MAXBARS];

  Double_t tdc_values[NPLANES][NSIDES][NTDCSIGNALS][MAXBARS];
  Double_t tdcraw_values[NPLANES][NSIDES][NTDCSIGNALS][MAXBARS];

  TH1F* hadc[NPLANES*NSIDES*NADCSIGNALS*MAXBARS];
  TH1F* htdc[NPLANES*NSIDES*NTDCSIGNALS*MAXBARS];

  Int_t adc_ihit, tdc_ihit;

  TTree *T=(TTree*)gDirectory->Get("T");

  for(UInt_t ip = 0; ip < NPLANES; ip++) {
    for(UInt_t iside = 0; iside < NSIDES; iside++) {
      for(UInt_t isignal = 0; isignal < NSIGNALS; isignal++) {

	if(signals[isignal] == "Adc") {  
	  TString adc_base_name = SPECTROMETER+"."+DETECTOR+"."+
	    plane_names[ip]+"."+sides[iside]+signals[isignal];
	  TString adc_ndata_name = "Ndata."+adc_base_name+"Counter";
	  T->SetBranchAddress(adc_ndata_name, &nadchits[ip][iside][isignal]);
	  TString adc_paddle_name = adc_base_name+"Counter";
	  T->SetBranchAddress(adc_paddle_name, &adc_paddles[ip][iside][isignal][0]);
	  for (UInt_t iadcsignal = 0; iadcsignal < NADCSIGNALS; iadcsignal++) {
	    if (adc_signals[iadcsignal] == "Ped") {
	      TString pedlist_name = adc_base_name+"Ped";
	      T->SetBranchAddress(pedlist_name, &ped_values[ip][iside][iadcsignal][0]);
	    }
	    if (adc_signals[iadcsignal] == "PedRaw") {
	      TString pedrawlist_name = adc_base_name+"PedRaw";
	      T->SetBranchAddress(pedrawlist_name, &pedraw_values[ip][iside][iadcsignal][0]);
	    }
	  }
	}

	if(signals[isignal] == "Tdc") {  
	  TString tdc_base_name = SPECTROMETER+"."+DETECTOR+"."+
	    plane_names[ip]+"."+sides[iside]+signals[isignal];
	  TString tdc_ndata_name = "Ndata."+tdc_base_name+"Counter";
	  T->SetBranchAddress(tdc_ndata_name, &ntdchits[ip][iside][isignal]);
	  TString tdc_paddle_name = tdc_base_name+"Counter";
	  T->SetBranchAddress(tdc_paddle_name, &tdc_paddles[ip][iside][isignal][0]);
	  for (UInt_t itdcsignal = 0; itdcsignal < NTDCSIGNALS; itdcsignal++) {
	    if (tdc_signals[itdcsignal] == "Time") {
	      TString tdclist_name = tdc_base_name+"Time";
	      T->SetBranchAddress(tdclist_name, &tdc_values[ip][iside][itdcsignal][0]);
	    }
	    if (tdc_signals[itdcsignal] == "TimeRaw") {
	      TString tdcrawlist_name = tdc_base_name+"TimeRaw";
	      T->SetBranchAddress(tdcrawlist_name, &tdcraw_values[ip][iside][itdcsignal][0]);
	    }
	  }
	}

	// Create histograms
	// ADC and TDC histogram for each 
	if(signals[isignal] == "Adc") {
	  UInt_t nadcbins  = ADC_NBINS;
	  Double_t hadcmin = ADC_MIN;
	  Double_t hadcmax = ADC_MAX;
	} 
	if(signals[isignal] == "Tdc") {
	  UInt_t ntdcbins  = TDC_NBINS;
	  Double_t htdcmin = TDC_MIN;
	  Double_t htdcmax = TDC_MAX;
	}
	
	if(signals[isignal] == "Adc") {
	  for (UInt_t iadcsignal = 0; iadcsignal < NADCSIGNALS; iadcsignal++) {
	    for(UInt_t ibar = 0; ibar < nbars[ip]; ibar++) {
	      UInt_t hadcindex = ip*NSIDES*NADCSIGNALS*MAXBARS
		+iside*NADCSIGNALS*MAXBARS+iadcsignal*MAXBARS+ibar;
	      TString iadcbarname = Form("%d",ibar+1);
	      TString adctitle= "hadc"+plane_names[ip]+iadcbarname+sides[iside]
		+" "+adc_signals[iadcsignal];
	      TString adcname="uhadc"+plane_names[ip]+iadcbarname+sides[iside]+
		adc_signals[iadcsignal];
	      
	      hadc[hadcindex]=new TH1F(adcname, adctitle, nadcbins, hadcmin, hadcmax);
	    }
	  }
	}

	if(signals[isignal] == "Tdc") {
	  for (UInt_t itdcsignal = 0; itdcsignal < NTDCSIGNALS; itdcsignal++) {
	    for(UInt_t ibar = 0; ibar < nbars[ip]; ibar++) {
	      UInt_t htdcindex = ip*NSIDES*NTDCSIGNALS*MAXBARS
		+iside*NTDCSIGNALS*MAXBARS+itdcsignal*MAXBARS+ibar;
	      TString itdcbarname = Form("%d",ibar+1);
	      TString tdctitle= "htdc"+plane_names[ip]+itdcbarname+sides[iside]
		+" "+tdc_signals[itdcsignal];
	      TString tdcname="uhtdc"+plane_names[ip]+itdcbarname+sides[iside]+
		tdc_signals[itdcsignal];
	  
	      htdc[htdcindex]=new TH1F(tdcname, tdctitle, ntdcbins, htdcmin, htdcmax);
	    }
	  }
	}
	
      }
    }
  }

  // Loop over the events, filling the histograms
  for(UInt_t iev = 0, N = T->GetEntries(); iev < N; iev++) {
    T->GetEntry(iev);

    cout << "**************************" << endl;
    cout << "Event Number = " << iev << endl;
    cout << "**************************" << endl;

    for(UInt_t ip = 0; ip < NPLANES; ip++){
      for(UInt_t iside = 0; iside < NSIDES; iside++) {
	for(UInt_t isignal = 0; isignal < NSIGNALS; isignal++) {
     
	  if(signals[isignal] == "Adc") {
	    for (UInt_t iadcsignal = 0; iadcsignal < NADCSIGNALS; iadcsignal++) {

	      UInt_t hadcindex_base = ip*NSIDES*NADCSIGNALS*MAXBARS
		+iside*NADCSIGNALS*MAXBARS+iadcsignal*MAXBARS;

	      cout << "============= ADC =============" << endl;
	      cout << "ip = " << ip << ", iside = " << iside << ", iadcsignal = " << iadcsignal << ", hadcindex_base = " << hadcindex_base << endl;

	      cout << "nadchits = " << nadchits[ip][iside][isignal] << endl;

 	      for(Int_t adc_ihit = 0; adc_ihit < nadchits[ip][iside][isignal]; adc_ihit++) {

		//UInt_t ibar = TMath::Nint(paddles[ip][iside][isignal][adc_ihit]) - 1;
		UInt_t ibar = TMath::Nint(adc_paddles[ip][iside][isignal][adc_ihit]) - 1;

		cout << "ibar = " << ibar << ", adc_ihit = " << adc_ihit << endl;

		cout << "level 1" << endl;
			
		if (adc_signals[iadcsignal] == "Ped") {  
		  Double_t pedval = ped_values[ip][iside][iadcsignal][adc_ihit];
		}

		cout << "level 2" << endl;

		if (adc_signals[iadcsignal] == "PedRaw") { 
		  Double_t pedrawval = pedraw_values[ip][iside][iadcsignal][adc_ihit];
		}
		
		cout << "level 3" << endl;

		// if (hadcindex_base == 0 && ibar == 0) UInt_t hadcindex = 1;
		// else UInt_t hadcindex = hadcindex_base + ibar;

		UInt_t hadcindex = hadcindex_base + ibar;

		cout << "hadcindex = " << hadcindex << endl;

		if (adc_signals[iadcsignal] == "Ped") {
		  cout << "pedval = " << pedval << endl;
		  hadc[hadcindex]->Fill(pedval);
		}
		if (adc_signals[iadcsignal] == "PedRaw") {
		  cout << "pedrawval = " << pedrawval << endl;
		  hadc[hadcindex]->Fill(pedrawval);
		}

	      }
	    }
	  }

	  if(signals[isignal] == "Tdc") {
	    for (UInt_t itdcsignal = 0; itdcsignal < NTDCSIGNALS; itdcsignal++) {

	      UInt_t htdcindex_base = ip*NSIDES*NTDCSIGNALS*MAXBARS
		+iside*NTDCSIGNALS*MAXBARS+itdcsignal*MAXBARS;

	      cout << "============= TDC =============" << endl;
	      cout << "ip = " << ip << ", iside = " << iside << ", itdcsignal = " << itdcsignal << ", htdcindex_base = " << htdcindex_base << endl;

	      cout << "ntdchits = " << ntdchits[ip][iside][isignal] << endl;

	      for(Int_t tdc_ihit = 0; tdc_ihit < ntdchits[ip][iside][isignal]; tdc_ihit++) {

		UInt_t ibar = TMath::Nint(tdc_paddles[ip][iside][isignal][tdc_ihit]) - 1;

		cout << "ibar = " << ibar << endl;

		Double_t tdcval = tdc_values[ip][iside][itdcsignal][tdc_ihit];
		Double_t tdcrawval = tdcraw_values[ip][iside][itdcsignal][tdc_ihit];
		UInt_t htdcindex = htdcindex_base + ibar;

		cout << "htdcindex = " << htdcindex << endl;

		if (tdc_signals[itdcsignal] == "Time") {
		  cout << "Time = " << tdcval << endl;
		  htdc[htdcindex]->Fill(tdcval);
		}
		if (tdc_signals[itdcsignal] == "TimeRaw") {
		  cout << "RawTime = " << tdcrawval << endl;
		  htdc[htdcindex]->Fill(tdcrawval);
		}
	      }
	    }
	  }

	}
      }
    }
  }

  return;

}

void rawdraw(TString histname) {

  TH1F* h;

  h = (TH1F*) gDirectory->Get(histname);
  if(!h) {
    UserScript();
    h = (TH1F*) gDirectory->Get(histname);
    if(!h) {
      cout << "User histogram " << histname << " not found" << endl;
      exit(1);
    }
  }
  h->Draw();
}

