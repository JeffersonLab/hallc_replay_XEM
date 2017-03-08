// UserScript.C
//
// Helper macro to build additional histograms

void UserScript()
{

  const UInt_t NPLANES  = 4;
  const UInt_t NSIDES   = 2;
  const UInt_t MAXBARS  = 16;  
  const UInt_t MAXHITS  = 16*16;  
  const UInt_t NSIGNALS = 2;
  const UInt_t NADCSIGNALS = 1;
  const UInt_t NTDCSIGNALS = 3;

  const TString SPECTROMETER = "H";
  const TString DETECTOR = "hod";

  const TString plane_names[NPLANES] = {"1x", "1y", "2x", "2y"};
  const TString plane_check_names[NPLANES] = {"1y", "1x", "2y", "2x"};
  const UInt_t  nbars[NPLANES] = {16, 10, 16, 10};
  const UInt_t  nbars_check[NPLANES] = {10, 16, 10, 16};
  const TString sides[NSIDES] = {"GoodNeg", "GoodPos"};
  const TString signals[NSIGNALS] = {"Adc", "Tdc"};
  const TString adc_signals[NADCSIGNALS] = {"PulseInt"};
  const Double_t adc_min[NADCSIGNALS] = {0};
  const Double_t adc_max[NADCSIGNALS] = {5000};
  const Int_t adc_nbins[NADCSIGNALS] = {500};
  const TString tdc_signals[NTDCSIGNALS] = {"Chan", "TimeCorr", "TimeTOFCorr"};
  const Double_t tdc_min[NTDCSIGNALS] = {0.,0.,0.};
  const Double_t tdc_max[NTDCSIGNALS] = {4000.,50.,50.};
  const Int_t tdc_nbins[NTDCSIGNALS] = {100,500,500};
 
  Double_t adcbit_to_pC = 1.0/(50*4096)*4*1000. ;// 1V/(50 Ohms)/4096*4ns* = .020 pC
  Double_t adcpeak_to_mV = 1.0/4096.*1000;// 1.0V per 4096 bits
  Double_t fac_for_disc[4]= {2.0,2.,2.,2.};// Factor to have ADC at FADC match signal at discriminatir assuming split of 2/3 to 1/3( 2Y plane has linear FI/FO not a splitter)
 
  Int_t nhits[NPLANES];
  Double_t adc_tdc_ymin[4]={20.,20.,20.,20.};
  Double_t adc_tdc_ymax[4]={45.,45.,45.,45.};
  Int_t nhits_temp;
  Double_t paddles[NPLANES][MAXHITS];


  Double_t adc_values[NPLANES][NSIDES][NADCSIGNALS][MAXHITS];
  Double_t tdc_values[NPLANES][NSIDES][NTDCSIGNALS][MAXHITS];


  //
  TH2F* htdc_tdc[NPLANES*MAXBARS];
  TH2F* hadc_adc[NPLANES*MAXBARS];
  TH2F* hadc_adc_good[NPLANES*MAXBARS];
  TH2F* hadc_tdc[NPLANES*MAXBARS][2];
  /*
  make histogram of plane ADC versus position in perpendicalur plane
   */
  TH2F* hadc_dis[NPLANES*MAXBARS][2];
  Int_t dis_bin[4]={10,16,10,16};
  Double_t dis_min[4];
  Double_t dis_max[4];
  Double_t dis_center_pad1[4]={33.75,-56.25,33.75,-56.25};
  Double_t dis_sign[4]={-1.,1.,-1.,1.};
  Double_t dis_space=7.5;
  //
  TH2F*   hpadx;
  TH2F*   hpadxy1;
  TH2F*   hpady;
  hpadx= new TH2F("hpadx",";paddle X1;paddle X2",16,1,17,16,1,17);
  hpadxy1= new TH2F("hpadxy1",";paddle X1;paddle Y1",16,1,17,10,1,11);
  hpady= new TH2F("hpady",";paddle Y1;paddle Y2",10,1,11,10,1,11);
  //
  /*
Histograms for Neg and Pos PMT efficiencies as function of paddle in other perpendicular plane
    Each X1 paddle efficiency as function of Y1 paddle.
    Each Y1 paddle efficiency as function of X1 paddle.
    Each X2 paddle efficiency as function of Y2 paddle.
    Each Y2 paddle efficiency as function of X2 paddle.
Tree has data in which at least one PMT had a good TDC time. 
If the other PMT did not have a good time then its TDC =-999.
For each event
Set good_should=kTRUE if  nhits==1 all planes.
Loop through all hits in each plane ip=0,3
  If good_should=kTRUE and tdc_neg_val>0 good_neg[ip][bar]=kTRUE 
  If good_should=kTRUE and tdc_pos_val>0 good_neg[ip][bar]=kTRUE 
endloop
Loop through all planes and all bars in that plane ib=0,nbars[ip]
  if good_pos[ip][ib] then negative end should have had a hit
         fill "should" neg PMT histogram with perpendicular paddle  
  if good_pos[ip][ib] and good_neg[ip][ib] 
         fill "did"  neg PMT  histogram with perpendicular paddle  
  if good_neg[ip][ib] then positive end should have had a hit
         fill "should" pos PMT histogram with perpendicular paddle  
  if good_pos[ip][ib] and good_neg[ip][ib] 
         fill "did"  pos PMT  histogram with perpendicular paddle 
end event
Efficiency is ratio of did/should which is calculated using hodo_efficiency.C   
   */
  TH1F* hgood_neg_should[4][16];
  TH1F* hgood_pos_should[4][16];
  TH1F* hgood_neg_did[4][16];
  TH1F* hgood_pos_did[4][16];
  //
  Int_t adc_ihit, tdc_ihit;

  TTree *T=(TTree*)gDirectory->Get("T");
  //
  for(UInt_t ip = 0; ip < NPLANES; ip++) {
	    for(UInt_t ibar = 0; ibar < nbars[ip]; ibar++) {
	      TString i2dbarname = Form("%d",ibar+1);
	      TString h2dttitle= "TDC 1ns/chan "+plane_names[ip]+i2dbarname+"; Neg ; Pos ";
	      TString h2dtname="uh2dtdc"+plane_names[ip]+i2dbarname;
              htdc_tdc[ip*MAXBARS+ibar]= new TH2F(h2dtname,h2dttitle,200,25,75,200,25,75.);
		TString h2dnegtitle= "Neg PMT Pad  "+plane_names[ip]+i2dbarname+"; plane "+plane_check_names[ip]+" paddles; Counts neg ";
	        TString h2dnegname="uhdidneg"+plane_names[ip]+i2dbarname;
                 hgood_neg_did[ip][ibar]= new TH1F(h2dnegname,h2dnegtitle,nbars_check[ip],1,nbars_check[ip]+1);
		TString h2dnegtitle= "Neg PMT Pad  "+plane_names[ip]+i2dbarname+"; plane "+plane_check_names[ip]+" paddles; Counts neg ";
	        TString h2dnegname="uhshouldneg"+plane_names[ip]+i2dbarname;
                 hgood_neg_should[ip][ibar]= new TH1F(h2dnegname,h2dnegtitle,nbars_check[ip],1,nbars_check[ip]+1);
		TString h2dpostitle= "Pos PMT Pad  "+plane_names[ip]+i2dbarname+"; plane "+plane_check_names[ip]+" paddles;Counts pos ";
	        TString h2dposname="uhdidpos"+plane_names[ip]+i2dbarname;
                 hgood_pos_did[ip][ibar]= new TH1F(h2dposname,h2dpostitle,nbars_check[ip],1,nbars_check[ip]+1);
		TString h2dpostitle="Pos PMT Pad  "+plane_names[ip]+i2dbarname+"; plane "+plane_check_names[ip]+" paddles;Counts pos ";
	        TString h2dposname="uhshouldpos"+plane_names[ip]+i2dbarname;
                 hgood_pos_should[ip][ibar]= new TH1F(h2dposname,h2dpostitle,nbars_check[ip],1,nbars_check[ip]+1);
 	    }
  }
  //
  //
  for(UInt_t ip = 0; ip < NPLANES; ip++) {
	    for(UInt_t ibar = 0; ibar < nbars[ip]; ibar++) {
	      TString i2dbarname = Form("%d",ibar+1);
	      TString h2dttitle= "ADC Int Amp "+plane_names[ip]+i2dbarname+"; Neg (pC); Pos (pC)";
	      TString h2dtname="uh2dadcint"+plane_names[ip]+i2dbarname;
	      hadc_adc[ip*MAXBARS+ibar]= new TH2F(h2dtname,h2dttitle,50,0,100,50,0,100.);
	      TString h2dttitle= "Good TDC ADC Int Amp "+plane_names[ip]+i2dbarname+"; Neg (pC); Pos (pC)";
	      TString h2dtname="uh2dadcintgtdc"+plane_names[ip]+i2dbarname;
	      hadc_adc_good[ip*MAXBARS+ibar]= new TH2F(h2dtname,h2dttitle,50,0,100,50,0,100.);
	      TString h2dttitle= "Neg PMT Pad "+plane_names[ip]+i2dbarname+"; Adc (pC);  Time (ns)";
	      TString h2dtname="uh2dadcvtdcneg"+plane_names[ip]+i2dbarname;
	      hadc_tdc[ip*MAXBARS+ibar][0]= new TH2F(h2dtname,h2dttitle,50,0,100,50,adc_tdc_ymin[ip],adc_tdc_ymax[ip]);
	      TString h2dttitle= "Pos PMT Pad "+plane_names[ip]+i2dbarname+"; Adc (pC);  Time (ns)";
	      TString h2dtname="uh2dadcvtdcpos"+plane_names[ip]+i2dbarname;
	      hadc_tdc[ip*MAXBARS+ibar][1]= new TH2F(h2dtname,h2dttitle,50,0,100,50,adc_tdc_ymin[ip],adc_tdc_ymax[ip]);
	      TString h2dttitle= "Neg PMT Pad "+plane_names[ip]+i2dbarname+"; Pos (cm); Adc (pC)";
	      TString h2dtname="uh2dadcvdisneg"+plane_names[ip]+i2dbarname;
	      hadc_dis[ip*MAXBARS+ibar][0]= new TH2F(h2dtname,h2dttitle,dis_bin[ip],dis_sign[ip]*dis_center_pad1[ip]-dis_space/2.,-dis_sign[ip]*dis_center_pad1[ip]+dis_space/2.,50,0,100);
	      TString h2dttitle= "Pos PMT Pad "+plane_names[ip]+i2dbarname+"; Pos (cm); Adc (pC)";
	      TString h2dtname="uh2dadcvdispos"+plane_names[ip]+i2dbarname;
	      hadc_dis[ip*MAXBARS+ibar][1]= new TH2F(h2dtname,h2dttitle,dis_bin[ip],dis_sign[ip]*dis_center_pad1[ip]-dis_space/2.,-dis_sign[ip]*dis_center_pad1[ip]+dis_space/2.,50,0,100);
	    }
  }
  // set up branches

  for(UInt_t ip = 0; ip < NPLANES; ip++) {
	  TString base2_name = SPECTROMETER+"."+DETECTOR+"."+plane_names[ip];
	  TString ndata_name = "Ndata."+base2_name+".GoodPaddle";
	  T->SetBranchAddress(ndata_name, &nhits[ip]);
	  TString paddle_name = base2_name+".GoodPaddle";
	  T->SetBranchAddress(paddle_name, &paddles[ip][0]);
	  //s          cout << ip << " " << paddle_name << endl;
    for(UInt_t iside = 0; iside < NSIDES; iside++) {
      for(UInt_t isignal = 0; isignal < NSIGNALS; isignal++) {

	if(signals[isignal] == "Adc") {  
	  TString adc_base_name = SPECTROMETER+"."+DETECTOR+"."+
	    plane_names[ip]+"."+sides[iside]+signals[isignal];
	  for (UInt_t iadcsignal = 0; iadcsignal < NADCSIGNALS; iadcsignal++) {
	    TString adclist_name = adc_base_name+adc_signals[iadcsignal];
	    T->SetBranchAddress(adclist_name, &adc_values[ip][iside][iadcsignal][0]);
	  }
	}
	
	if(signals[isignal] == "Tdc") {  
	  TString tdc_base_name = SPECTROMETER+"."+DETECTOR+"."+
	    plane_names[ip]+"."+sides[iside]+signals[isignal];
	  for (UInt_t itdcsignal = 0; itdcsignal < NTDCSIGNALS; itdcsignal++) {
	    TString tdclist_name = tdc_base_name+tdc_signals[itdcsignal];
	      T->SetBranchAddress(tdclist_name, &tdc_values[ip][iside][itdcsignal][0]);
	    }
	}
      } // isginal
    } //iside
  } // ip
  // Loop over the events, filling the histograms
  Bool_t good_should;
  Double_t good_pad[4];
  Double_t good_adc[4][2];
  Int_t check_plane[4]={1,0,3,2};
  Bool_t good_neg[4][16],good_pos[4][16];
  for(UInt_t iev = 0, N = T->GetEntries(); iev < N; iev++) {
    T->GetEntry(iev);
    if (iev%10000==0) cout << " iev = " << iev << endl;
     // get events with x paddle number equal and y paddle equal
    //
       good_should=kFALSE;
      for(UInt_t ip = 0; ip < NPLANES; ip++){ 
     for(UInt_t ib = 0; ib < nbars[ip]; ib++){ 
       good_neg[ip][ib]=kFALSE;
       good_pos[ip][ib]=kFALSE;
     }
     }
    if (nhits[0]==1&&nhits[1]==1&&nhits[2]==1&&nhits[3]==1) good_should = kTRUE;
      for(UInt_t ip = 0; ip < NPLANES; ip++){ 
	//	 cout << iev << " " << ip << " " << nhits[ip]<< endl;
		for(Int_t ihit = 0; ihit < nhits[ip]; ihit++)  {
 		UInt_t bar = TMath::Nint(paddles[ip][ihit]) - 1;
		Double_t tdc_neg_val = tdc_values[ip][0][1][ihit];
		Double_t tdc_pos_val =tdc_values[ip][1][1][ihit];
		Double_t adc_neg_val = adc_values[ip][0][0][ihit]*adcbit_to_pC;
		Double_t adc_pos_val = adc_values[ip][1][0][ihit]*adcbit_to_pC;
		//		cout << " " << ihit << " " << bar << " "  << tdc_neg_val << endl;
		htdc_tdc[ip*MAXBARS+bar]->Fill(tdc_neg_val,tdc_pos_val);
		//		if (tdc_neg_val > 0 && tdc_pos_val >0) hadc_adc[ip*MAXBARS+bar]->Fill(adc_neg_val,adc_pos_val);
		hadc_adc[ip*MAXBARS+bar]->Fill(adc_neg_val,adc_pos_val);
		if (tdc_pos_val>0 && tdc_neg_val>0 ) {
                  hadc_adc_good[ip*MAXBARS+bar]->Fill(adc_neg_val,adc_pos_val);
		  hadc_tdc[ip*MAXBARS+bar][0]->Fill(adc_neg_val,tdc_neg_val);
		  hadc_tdc[ip*MAXBARS+bar][1]->Fill(adc_pos_val,tdc_pos_val);
		}
		good_pad[ip]=bar+1;
                good_adc[ip][0]=adc_neg_val;
                good_adc[ip][1]=adc_pos_val;
                if (good_should && tdc_neg_val>0 ) good_neg[ip][bar]=kTRUE;
                if (good_should && tdc_pos_val>0 ) good_pos[ip][bar]=kTRUE;
		}
	}
      //
     if (good_should   ) {
     for(UInt_t ip = 0; ip < NPLANES; ip++){ 
       //       cout << " ip = " << ip << " " << good_should<< endl;
	 //	 cout << " pad =  " << good_pad[check_plane[ip]] << " check_plane = " << check_plane[ip] << endl;
          for(UInt_t ib = 0; ib < nbars[ip]; ib++){ 
	    //	    cout << ib << " " << good_neg[ip][ib] << " " << good_pos[ip][ib] << endl;
          if (good_pos[ip][ib]) hgood_neg_should[ip][ib]->Fill(good_pad[check_plane[ip]]);
          if (good_neg[ip][ib]) hgood_pos_should[ip][ib]->Fill(good_pad[check_plane[ip]]);
          if (good_neg[ip][ib] && good_pos[ip][ib]) {
               hgood_neg_did[ip][ib]->Fill(good_pad[check_plane[ip]]);
               hgood_pos_did[ip][ib]->Fill(good_pad[check_plane[ip]]);
               hadc_dis[ip*MAXBARS+ib][0]->Fill(dis_center_pad1[ip]+dis_sign[ip]*dis_space*(good_pad[check_plane[ip]]-1),good_adc[ip][0]);
               hadc_dis[ip*MAXBARS+ib][1]->Fill(dis_center_pad1[ip]+dis_sign[ip]*dis_space*(good_pad[check_plane[ip]]-1),good_adc[ip][1]);
	  }
 	  }
      }
     }
  } // loop over entries
    //
  return;
}



void gooddraw_2d_hhodo(TString histname) {

  TH1F* h1d;
  TH2F* h2d;

  h1d = (TH1F*) gDirectory->Get(histname);
  h2d = (TH2F*) gDirectory->Get(histname);
  if(!h1d && !h2d) {
    UserScript();
    h1d = (TH1F*) gDirectory->Get(histname);
    h2d = (TH2F*) gDirectory->Get(histname);
    if(!h1d || !h2d) {
      cout << "User histogram " << histname << " not found" << endl;
      exit(1);
    }
  }
  if (h2d) {
  h2d->SetStats(0);
  h2d->Draw("colz");
    h2d->GetXaxis()->SetTitleOffset(.6);
    h2d->GetXaxis()->SetTitleSize(0.08);
    h2d->GetYaxis()->SetTitleOffset(.6);
    h2d->GetYaxis()->SetTitleSize(0.08);
      }
  else {
    cout << "1d  drawing   " << histname << endl;
  h1d->SetStats(0);
  h1d->Draw();
    h1d->GetXaxis()->SetTitleOffset(.6);
    h1d->GetXaxis()->SetTitleSize(0.08);
    h1d->GetYaxis()->SetTitleOffset(.6);
    h1d->GetYaxis()->SetTitleSize(0.08);
  }
}

