// UserScript.C
//
// Helper macro to build additional histograms

void UserScript()
{

  const UInt_t NPLANES  = 4;
  const UInt_t NSIDES   = 2;
  const UInt_t MAXBARS  = 100;  // FIX ME: Parameter should be 21
  const UInt_t NSIGNALS = 2;
  const UInt_t NADCSIGNALS = 7;
  const UInt_t NTDCSIGNALS = 2;

  const TString SPECTROMETER = "P";
  const TString DETECTOR = "hod";

  const TString plane_names[NPLANES] = {"1x", "1y", "2x", "2y"};
  const UInt_t  nbars[NPLANES] = {13, 13, 14, 21};
  const TString sides[NSIDES] = {"neg", "pos"};
  const TString signals[NSIGNALS] = {"Adc", "Tdc"};
  const TString adc_signals[NADCSIGNALS] = {"Ped", "PedRaw", "PulseInt", "PulseIntRaw", 
					    "PulseAmp", "PulseAmpRaw", "PulseTimeRaw"};
  const TString tdc_signals[NTDCSIGNALS] = {"Time", "TimeRaw"};
  
  const Double_t ADCPED_MIN   = 400.0;
  const Double_t ADCPED_MAX   = 600.0;
  const Int_t    ADCPED_NBINS = 200;

  const Double_t ADCPEDRAW_MIN   = 1700.0;
  const Double_t ADCPEDRAW_MAX   = 2300.0;
  const Int_t    ADCPEDRAW_NBINS = 600;

  const Double_t ADCINT_MIN   = 0.0;
  const Double_t ADCINT_MAX   = 25000.0;
  const Int_t    ADCINT_NBINS = 2500;

  const Double_t ADCINTRAW_MIN   = 0.0;
  const Double_t ADCINTRAW_MAX   = 25000.0;
  const Int_t    ADCINTRAW_NBINS = 2500;

  const Double_t ADCAMP_MIN   = 0.0;
  const Double_t ADCAMP_MAX   = 4100.0;
  const Int_t    ADCAMP_NBINS = 410;

  const Double_t ADCAMPRAW_MIN   = 0.0;
  const Double_t ADCAMPRAW_MAX   = 4100.0;
  const Int_t    ADCAMPRAW_NBINS = 410;

  const Double_t ADCPTRAW_MIN   = 500.0;
  const Double_t ADCPTRAW_MAX   = 2500.0;
  const Int_t    ADCPTRAW_NBINS = 200;

  const Double_t TDC_MIN   = -1800.0;
  const Double_t TDC_MAX   = -600.0;
  const Int_t    TDC_NBINS = 120;

  const Double_t RAWTDC_MIN   = 4000.0;
  const Double_t RAWTDC_MAX   = 7000.0;
  const Int_t    RAWTDC_NBINS = 300;

  Int_t nadchits[NPLANES][NSIDES][NSIGNALS];
  Int_t ntdchits[NPLANES][NSIDES][NSIGNALS];

  Double_t adc_paddles[NPLANES][NSIDES][NSIGNALS][MAXBARS];
  Double_t tdc_paddles[NPLANES][NSIDES][NSIGNALS][MAXBARS];

  Double_t ped_values[NPLANES][NSIDES][NADCSIGNALS][MAXBARS];
  Double_t pedraw_values[NPLANES][NSIDES][NADCSIGNALS][MAXBARS];

  Double_t int_values[NPLANES][NSIDES][NADCSIGNALS][MAXBARS];
  Double_t intraw_values[NPLANES][NSIDES][NADCSIGNALS][MAXBARS];
  
  Double_t amp_values[NPLANES][NSIDES][NADCSIGNALS][MAXBARS];
  Double_t ampraw_values[NPLANES][NSIDES][NADCSIGNALS][MAXBARS];
  
  Double_t ptraw_values[NPLANES][NSIDES][NADCSIGNALS][MAXBARS];

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
	    if (adc_signals[iadcsignal] == "PulseInt") {   
	      TString intlist_name = adc_base_name+"PulseInt";
	      T->SetBranchAddress(intlist_name, &int_values[ip][iside][iadcsignal][0]);
	    }
	    if (adc_signals[iadcsignal] == "PulseIntRaw") {
	      TString intrawlist_name = adc_base_name+"PulseIntRaw";
	      T->SetBranchAddress(intrawlist_name, &intraw_values[ip][iside][iadcsignal][0]);
	    }
	    if (adc_signals[iadcsignal] == "PulseAmp") {   
	      TString amplist_name = adc_base_name+"PulseAmp";
	      T->SetBranchAddress(amplist_name, &amp_values[ip][iside][iadcsignal][0]);
	    }
	    if (adc_signals[iadcsignal] == "PulseAmpRaw") {
	      TString amprawlist_name = adc_base_name+"PulseAmpRaw";
	      T->SetBranchAddress(amprawlist_name, &ampraw_values[ip][iside][iadcsignal][0]);
	    }
	    if (adc_signals[iadcsignal] == "PulseTimeRaw") {
	      TString ptrawlist_name = adc_base_name+"PulseTimeRaw";
	      T->SetBranchAddress(ptrawlist_name, &ptraw_values[ip][iside][iadcsignal][0]);
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

	      if (adc_signals[iadcsignal] == "Ped") {
		UInt_t   nadcbins = ADCPED_NBINS;
		Double_t hadcmin  = ADCPED_MIN;
		Double_t hadcmax  = ADCPED_MAX;
	      }
	      if (adc_signals[iadcsignal] == "PedRaw") {
		UInt_t   nadcbins = ADCPEDRAW_NBINS;
		Double_t hadcmin  = ADCPEDRAW_MIN;
		Double_t hadcmax  = ADCPEDRAW_MAX;
	      }
	      if (adc_signals[iadcsignal] == "PulseInt") {
		UInt_t   nadcbins = ADCINT_NBINS;
		Double_t hadcmin  = ADCINT_MIN;
		Double_t hadcmax  = ADCINT_MAX;
	      }
	      if (adc_signals[iadcsignal] == "PulseIntRaw") {
		UInt_t   nadcbins = ADCINTRAW_NBINS;
		Double_t hadcmin  = ADCINTRAW_MIN;
		Double_t hadcmax  = ADCINTRAW_MAX;
	      }
	      if (adc_signals[iadcsignal] == "PulseAmp") {
		UInt_t   nadcbins = ADCAMP_NBINS;
		Double_t hadcmin  = ADCAMP_MIN;
		Double_t hadcmax  = ADCAMP_MAX;
	      }
	      if (adc_signals[iadcsignal] == "PulseAmpRaw") {
		UInt_t   nadcbins = ADCAMPRAW_NBINS;
		Double_t hadcmin  = ADCAMPRAW_MIN;
		Double_t hadcmax  = ADCAMPRAW_MAX;
	      }
	      if (adc_signals[iadcsignal] == "PulseTimeRaw") {
		UInt_t   nadcbins = ADCPTRAW_NBINS;
		Double_t hadcmin  = ADCPTRAW_MIN;
		Double_t hadcmax  = ADCPTRAW_MAX;
	      }
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

	      if (tdc_signals[itdcsignal] == "TimeRaw") {
		UInt_t   ntdcbins = RAWTDC_NBINS;
		Double_t htdcmin  = RAWTDC_MIN;
		Double_t htdcmax  = RAWTDC_MAX;
	      }
	      if (tdc_signals[itdcsignal] == "Time") {
		UInt_t   ntdcbins = TDC_NBINS;
		Double_t htdcmin  = TDC_MIN;
		Double_t htdcmax  = TDC_MAX;
	      }
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

     for(UInt_t ip = 0; ip < NPLANES; ip++) {
      for(UInt_t iside = 0; iside < NSIDES; iside++) {
	for(UInt_t isignal = 0; isignal < NSIGNALS; isignal++) {
     
	  if(signals[isignal] == "Adc") {
	    for (UInt_t iadcsignal = 0; iadcsignal < NADCSIGNALS; iadcsignal++) {

	      UInt_t hadcindex_base = ip*NSIDES*NADCSIGNALS*MAXBARS
		+iside*NADCSIGNALS*MAXBARS+iadcsignal*MAXBARS;

	      for(Int_t adc_ihit = 0; adc_ihit < nadchits[ip][iside][isignal]; adc_ihit++) {

		UInt_t ibar = TMath::Nint(adc_paddles[ip][iside][isignal][adc_ihit]) - 1;

		if (adc_signals[iadcsignal] == "Ped")
		  Double_t pedval = ped_values[ip][iside][iadcsignal][adc_ihit];
		if (adc_signals[iadcsignal] == "PedRaw")  
		  Double_t pedrawval = pedraw_values[ip][iside][iadcsignal][adc_ihit];
		if (adc_signals[iadcsignal] == "PulseInt")
		  Double_t intval = int_values[ip][iside][iadcsignal][adc_ihit];
		if (adc_signals[iadcsignal] == "PulseIntRaw")  
		  Double_t intrawval = intraw_values[ip][iside][iadcsignal][adc_ihit];
		if (adc_signals[iadcsignal] == "PulseAmp")
		  Double_t ampval = amp_values[ip][iside][iadcsignal][adc_ihit];
		if (adc_signals[iadcsignal] == "PulseAmpRaw")  
		  Double_t amprawval = ampraw_values[ip][iside][iadcsignal][adc_ihit];
		if (adc_signals[iadcsignal] == "PulseTimeRaw")  
		  Double_t ptrawval = ptraw_values[ip][iside][iadcsignal][adc_ihit];
		
		UInt_t hadcindex = hadcindex_base + ibar;

		// cout << "hadcindex = " << hadcindex << endl;
		
		if (hadcindex < NPLANES*NSIDES*NADCSIGNALS*MAXBARS) {
		  if (adc_signals[iadcsignal] == "Ped") {
		    // cout << "pedval = " << pedval << endl;
		    hadc[hadcindex]->Fill(pedval);
		  }
		  if (adc_signals[iadcsignal] == "PedRaw") {
		    // cout << "pedrawval = " << pedrawval << endl;
		    hadc[hadcindex]->Fill(pedrawval);
		  }
		  if (adc_signals[iadcsignal] == "PulseInt") {
		    // cout << "intval = " << intval << endl;
		    hadc[hadcindex]->Fill(intval);
		  }
		  if (adc_signals[iadcsignal] == "PulseIntRaw") {
		    // cout << "intrawval = " << intrawval << endl;
		    hadc[hadcindex]->Fill(intrawval);
		  }
		  if (adc_signals[iadcsignal] == "PulseAmp") {
		    // cout << "ampval = " << ampval << endl;
		    hadc[hadcindex]->Fill(ampval);
		  }
		  if (adc_signals[iadcsignal] == "PulseAmpRaw") {
		    // cout << "amprawval = " << amprawval << endl;
		    hadc[hadcindex]->Fill(amprawval);
		  }
		  if (adc_signals[iadcsignal] == "PulseTimeRaw") {
		    // cout << "ptrawval = " << ptrawval << endl;
		    hadc[hadcindex]->Fill(ptrawval);
		  }
		}
	      }
	    }
	  }

	  if(signals[isignal] == "Tdc") {
	    for (UInt_t itdcsignal = 0; itdcsignal < NTDCSIGNALS; itdcsignal++) {

	      UInt_t htdcindex_base = ip*NSIDES*NTDCSIGNALS*MAXBARS
		+iside*NTDCSIGNALS*MAXBARS+itdcsignal*MAXBARS;

	      for(Int_t tdc_ihit = 0; tdc_ihit < ntdchits[ip][iside][isignal]; tdc_ihit++) {

		UInt_t ibar = TMath::Nint(tdc_paddles[ip][iside][isignal][tdc_ihit]) - 1;

		Double_t tdcval = tdc_values[ip][iside][itdcsignal][tdc_ihit];
		Double_t tdcrawval = tdcraw_values[ip][iside][itdcsignal][tdc_ihit];
		UInt_t htdcindex = htdcindex_base + ibar;

		if (tdc_signals[itdcsignal] == "Time") {
		  // cout << "Time = " << tdcval << endl;
		  htdc[htdcindex]->Fill(tdcval);
		}
		if (tdc_signals[itdcsignal] == "TimeRaw") {
		  // cout << "RawTime = " << tdcrawval << endl;
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

void rawdraw_phodo(TString histname) {

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

