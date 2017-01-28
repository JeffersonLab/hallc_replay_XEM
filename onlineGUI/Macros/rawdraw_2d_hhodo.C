// UserScript.C
//
// Helper macro to build additional histograms

void UserScript()
{

  const UInt_t NPLANES  = 4;
  const UInt_t NSIDES   = 2;
  const UInt_t MAXBARS  = 16;  // FIX ME: Parameter should be 16
  const UInt_t NSIGNALS = 2;
  const UInt_t NADCSIGNALS = 7;
  const UInt_t NTDCSIGNALS = 2;
  const UInt_t MAXHITS=16*16;

  const TString SPECTROMETER = "H";
  const TString DETECTOR = "hod";

  const TString plane_names[NPLANES] = {"1x", "1y", "2x", "2y"};
  const UInt_t  nbars[NPLANES] = {16, 10, 16, 10};
  const TString sides[NSIDES] = {"neg", "pos"};
  const TString signals[NSIGNALS] = {"Adc", "Tdc"};
  const TString adc_signals[NADCSIGNALS] = {"Ped", "PedRaw", "PulseInt", "PulseIntRaw", 
					    "PulseAmp", "PulseAmpRaw", "PulseTimeRaw"};
  const TString tdc_signals[NTDCSIGNALS] = {"Time", "TimeRaw"};
  
  Double_t adcbit_to_pC = 1.0/(50*4096)*4*1000. ;// 1V/(50 Ohms)/4096*4ns* = .020 pC
  cout << adcbit_to_pC << endl;
  Int_t nadchits[NPLANES][NSIDES][NSIGNALS];
  Int_t ntdchits[NPLANES][NSIDES][NSIGNALS];

  Double_t adc_paddles[NPLANES][NSIDES][NSIGNALS][MAXHITS];
  Double_t tdc_paddles[NPLANES][NSIDES][NSIGNALS][MAXHITS];

  Double_t ped_values[NPLANES][NSIDES][NADCSIGNALS][MAXHITS];
  Double_t pedraw_values[NPLANES][NSIDES][NADCSIGNALS][MAXHITS];

  Double_t int_values[NPLANES][NSIDES][NADCSIGNALS][MAXHITS];
  Double_t intraw_values[NPLANES][NSIDES][NADCSIGNALS][MAXHITS];
  
  Double_t amp_values[NPLANES][NSIDES][NADCSIGNALS][MAXHITS];
  Double_t ampraw_values[NPLANES][NSIDES][NADCSIGNALS][MAXHITS];
  
  Double_t ptraw_values[NPLANES][NSIDES][NADCSIGNALS][MAXHITS];

  Double_t tdc_values[NPLANES][NSIDES][NTDCSIGNALS][MAXHITS];
  Double_t tdcraw_values[NPLANES][NSIDES][NTDCSIGNALS][MAXHITS];

  TH2F* htdc_adc[NPLANES*NSIDES*MAXBARS];
  TH2F* htdc_tdc[NPLANES*MAXBARS];
  TH2F* hadc_adc[NPLANES*MAXBARS];
  TH2F* hadcint_adcint[NPLANES*MAXBARS];
  TH2F* hatdc_atdc[NPLANES*MAXBARS];
  TH2F* hadc_atdc[NPLANES*NSIDES*MAXBARS];
  Int_t adc_ihit, tdc_ihit;

  TTree *T=(TTree*)gDirectory->Get("T");
  //
  for(UInt_t ip = 0; ip < NPLANES; ip++) {
	    for(UInt_t ibar = 0; ibar < nbars[ip]; ibar++) {
	      TString i2dbarname = Form("%d",ibar+1);
	      TString h2dttitle= "TDC 0.1ns/chan "+plane_names[ip]+i2dbarname+"; Neg ; Pos ";
	      TString h2dtname="uh2dtdc"+plane_names[ip]+i2dbarname;
              htdc_tdc[ip*MAXBARS+ibar]= new TH2F(h2dtname,h2dttitle,100,-1000,-500,100,-1000,-500.);
	    }
  }
  //
  //
  for(UInt_t ip = 0; ip < NPLANES; ip++) {
	    for(UInt_t ibar = 0; ibar < nbars[ip]; ibar++) {
	      TString i2dbarname = Form("%d",ibar+1);
	      TString h2dttitle= "ADC Peak Amp "+plane_names[ip]+i2dbarname+"; Neg (mV); Pos (mV)";
	      TString h2dtname="uh2dadcpeak"+plane_names[ip]+i2dbarname;
	      //             hadc_adc[ip*MAXBARS+ibar]= new TH2F(h2dtname,h2dttitle,500,-200,5800,500,-200,5800.);
	                 hadc_adc[ip*MAXBARS+ibar]= new TH2F(h2dtname,h2dttitle,160,0,800,160,0,800.);
	      TString h2dttitle= "ADC Int Amp "+plane_names[ip]+i2dbarname+"; Neg (pC); Pos (pC)";
	      TString h2dtname="uh2dadcint"+plane_names[ip]+i2dbarname;
	                 hadcint_adcint[ip*MAXBARS+ibar]= new TH2F(h2dtname,h2dttitle,50,0,100,50,0,100.);
	    }
  }
  //
  //
  for(UInt_t ip = 0; ip < NPLANES; ip++) {
	    for(UInt_t ibar = 0; ibar < nbars[ip]; ibar++) {
	      TString i2dbarname = Form("%d",ibar+1);
	      TString h2dttitle= "h2dadctdc"+plane_names[ip]+i2dbarname;
	      TString h2dtname="uh2dadctdc"+plane_names[ip]+i2dbarname;
              hatdc_atdc[ip*MAXBARS+ibar]= new TH2F(h2dtname,h2dttitle,100,1000,2000,100,1000,2000.);
	    }
  }
  //
  for(UInt_t ip = 0; ip < NPLANES; ip++) {
    for(UInt_t iside = 0; iside < NSIDES; iside++) {
	    for(UInt_t ibar = 0; ibar < nbars[ip]; ibar++) {
	      UInt_t h2dindex = ip*NSIDES*MAXBARS+iside*MAXBARS+ibar;
	      TString i2dbarname = Form("%d",ibar+1);
	      TString h2dtitle= "ADC (pc) vs TDC (0.1ns/chan) "+plane_names[ip]+i2dbarname+sides[iside];
	      TString h2dname="uh2d"+plane_names[ip]+i2dbarname+sides[iside];
	      TString h2datitle= "h2dadcAtdc"+plane_names[ip]+i2dbarname+sides[iside];
	      TString h2daname="uh2dadcAtdc"+plane_names[ip]+i2dbarname+sides[iside];
              if ( h2dindex < NPLANES*NSIDES*MAXBARS) {
              htdc_adc[h2dindex]= new TH2F(h2dname,h2dtitle,200,-1000,-500,50,0.,100.);
              hadc_atdc[h2dindex]= new TH2F(h2daname,h2datitle,50,0,100.,300,1000,2000.);
	      //	      cout << " index = " << h2dindex << " " << h2dname << endl;
              } else {
                cout << " index = " << h2dindex << " too large" << endl;
	      }
	    }
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

      }
    }
  }

  // Loop over the events, filling the histograms
  cout << " looping over data " << endl;
  for(UInt_t iev = 0, N = T->GetEntries(); iev < N; iev++) {
    if (iev%10000==0) cout << " iev = " << iev << endl;
    T->GetEntry(iev);
    //
    Double_t tdcbar_save=-1;
        for(UInt_t ip = 0; ip < NPLANES; ip++) {
	  //       cout << iev << " " << ip << " " << ntdchits[ip][0][1]<< " " << ntdchits[ip][1][1] << endl;
		for(Int_t tdc1_ihit = 0; tdc1_ihit < ntdchits[ip][0][1]; tdc1_ihit++)  {
 		UInt_t tdc1bar = TMath::Nint(tdc_paddles[ip][0][1][tdc1_ihit]) - 1;
		Double_t tdc1val = tdc_values[ip][0][0][tdc1_ihit];
		for(Int_t tdc2_ihit = 0; tdc2_ihit < ntdchits[ip][1][1]; tdc2_ihit++)  {
		  UInt_t tdc2bar = TMath::Nint(tdc_paddles[ip][1][1][tdc2_ihit]) - 1;
		  Double_t tdc2val =tdc_values[ip][1][0][tdc2_ihit];
		  //                  cout << iev << " " << tdc1_ihit << " " << tdc1val << " " << tdc1bar << " tdc 2 = " << tdc2_ihit << " " << tdc2val   << " " << tdc2bar << endl;
		  if ( tdc2bar == tdc1bar) htdc_tdc[ip*MAXBARS+tdc1bar]->Fill(tdc1val,tdc2val);
                  if ( ip==3 && tdc2bar == tdc1bar) tdcbar_save=tdc2bar;
		}
		}
	}
        for(UInt_t ip = 0; ip < NPLANES; ip++) {
		//
		for(Int_t adc1_ihit = 0; adc1_ihit < nadchits[ip][0][0]; adc1_ihit++)  {
 		UInt_t adc1bar = TMath::Nint(adc_paddles[ip][0][0][adc1_ihit]) - 1;
		Double_t adc1val = int_values[ip][0][2][adc1_ihit]*adcbit_to_pC;
		Double_t adc1pval = amp_values[ip][0][4][adc1_ihit];
		Double_t adc1tval = ptraw_values[ip][0][6][adc1_ihit];
		for(Int_t adc2_ihit = 0; adc2_ihit < nadchits[ip][1][0]; adc2_ihit++)  {
		  UInt_t adc2bar = TMath::Nint(adc_paddles[ip][1][0][adc2_ihit]) - 1;
		  Double_t adc2val =int_values[ip][1][2][adc2_ihit]*adcbit_to_pC;
		  Double_t adc2pval =amp_values[ip][1][4][adc2_ihit];
		  Double_t adc2tval =ptraw_values[ip][1][6][adc2_ihit];
		  //            cout << iev << " " << adc1_ihit << " " << adc1tval << " " << adc1bar << " adc 2 = " << adc2_ihit << " " << adc2tval   << " " << adc2bar << endl;
		  if ( adc2bar == adc1bar ) hadc_adc[ip*MAXBARS+adc1bar]->Fill(adc1pval*.244*2,adc2pval*.244*2);
		  if ( adc2bar == adc1bar ) hadcint_adcint[ip*MAXBARS+adc1bar]->Fill(adc1val,adc2val);
		  if ( adc2bar == adc1bar) hatdc_atdc[ip*MAXBARS+adc1bar]->Fill(adc1tval,adc2tval);
		}
		}
	}
    //

     //
     for(UInt_t ip = 0; ip < NPLANES; ip++) {
      for(UInt_t iside = 0; iside < NSIDES; iside++) {
	//
	      UInt_t hindex_base = ip*NSIDES*MAXBARS+iside*MAXBARS;
		for(Int_t tdc1_ihit = 0; tdc1_ihit < ntdchits[ip][iside][1]; tdc1_ihit++)  {
 		UInt_t tdc1bar = TMath::Nint(tdc_paddles[ip][iside][1][tdc1_ihit]) - 1;
		Double_t tdc1val = tdc_values[ip][0][0][tdc1_ihit];
		for(Int_t adc2_ihit = 0; adc2_ihit < nadchits[ip][iside][0]; adc2_ihit++)  {
		  UInt_t adc2bar = TMath::Nint(adc_paddles[ip][iside][0][adc2_ihit]) - 1;
		  Double_t adc2val =int_values[ip][iside][2][adc2_ihit];
		  //                  cout << iev << " " << tdc1_ihit << " " << tdc1val << " " << tdc1bar << " tdc 2 = " << tdc2_ihit << " " << tdc2val   << " " << tdc2bar << endl;
		  if (tdc1bar==adc2bar) htdc_adc[hindex_base+adc2bar]->Fill(tdc1val,adc2val*adcbit_to_pC);
		}
		}
	//
      }
     }
    
  }
  return;
}

void rawdraw_2d_hhodo(TString histname) {

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
  TLine *pline = new TLine(0,30.,800.,30.);
  TLine *pline2 = new TLine(30,0.,30.,800.);
  // cout << histname.Contains("adcpeak") << endl;
  h->SetStats(0);
  h->Draw("colz");
  if (histname.Contains("adcpeak")) {
    pline->Draw();
    pline->SetLineColor(2);
    pline->SetLineWidth(3);
    pline2->Draw();
    pline2->SetLineColor(2);
    pline2->SetLineWidth(3);
    h->GetXaxis()->SetTitleOffset(.6);
    h->GetXaxis()->SetTitleSize(0.08);
    h->GetYaxis()->SetTitleOffset(.6);
    h->GetYaxis()->SetTitleSize(0.08);
  }
  if (histname.Contains("adcint")) {
    h->GetXaxis()->SetTitleOffset(.6);
    h->GetXaxis()->SetTitleSize(0.08);
    h->GetYaxis()->SetTitleOffset(.6);
    h->GetYaxis()->SetTitleSize(0.08);
  }
}

