//Read one 1D histogram from rootfile
void shms_ped(Int_t runNumber, Int_t eventNumber)
{

  const Int_t run        = runNumber;
  const Int_t nevents    = eventNumber;
  const Int_t goldRunNum = 1757;
  const Int_t numHistos  = 5;     //Number of histos

  // Define golden run in which to compare current run to
   TFile *f = new TFile(Form("ROOTfiles/shms_replay_production_%d_50000.root", goldRunNum));
  // TFile *f = new TFile(Form("ROOTfiles/shms_coin_replay_production_%d_50000.root", goldRunNum));
  ofstream myfile;
  myfile.open("outSHMS.txt"); //To store pedestals differences
  //cout<<"________________________________________________________________"<<endl;
 myfile <<"___________________________________________________________________________"<<endl;
  myfile <<"SHMS PEDESTAL GOLDEN  RUN NUMBER =" "  "<<goldRunNum<<endl;
  myfile <<"SHMS TRIGGER SUM PPED RUN NUMBER =" "  "<<run<<endl;
  myfile <<"___________________________________________________________________________"<<endl;
  myfile <<"shms_sum_pped"<< "\t\t\t"<<"Mean_Golden"<<"\t"<<"Mean"<<"\t\t"<<"Mean_Diff"<<endl;
  myfile <<"___________________________________________________________________________"<<endl;


  // myfile<< "shms_sum_pped   " << "Mean_Golden"<<"\t"<<"Mean"<<"\t\t"<<"Mean_Diff"<<endl;
 


  // cout<<"Channel "<<"   Bar"<<"\t"<<"Variable"<<"\t"<<"Mean_Golden"<<"\t"<<"Mean "<<"\t"<<"Mean_Diff"<<endl;
  //  cout<<"________________________________________________________________"<<endl;

  TH1D *histG[numHistos];
  TH1D *hist[numHistos];
  Int_t   channel[5] = {1,2,3,4,5}; //(see hallc_replay/MAPS/SHMS/DETEC/TRIG/ptrig.map)
  TString variables[numHistos] = {"pHGCER", "pNGCER", "pPSHWR", "pHG_MOD", "pNG_MOD"};
  TString histos[numHistos]    = {"ptrig_phgc_sum_pped", "ptrig_pngc_sum_pped", "ptrig_prshwr_sum_pped", "ptrig_phgc_sum_MOD_pped", "ptrig_pngc_sum_MOD_pped"};
 
  Int_t plane=1;
  Int_t bar [5]={2,3,4,6,7}; //variables are corresponding to these bars
 
  // For Goldern

  double meanG[numHistos],    rmsG[numHistos];
  double mean[numHistos],     rms[numHistos];
  double meanDiff[numHistos], rmsDiff[numHistos];
  double eff_rms[numHistos];

  for (int i = 0; i < numHistos-2 ; i++)
    {
      histG[i] = (TH1D*)gDirectory->Get(histos[i]);
      // cout << "histG[i] = " << histG[i] << endl;
      if (histG[i]) {
  	//	histG[i]->Draw();
      	meanG[i]=histG[i]->GetMean();
      	rmsG[i]=histG[i]->GetRMS();
      }
      else 
  	{	
  	  cout << "****************ATTENTION REQUIRED *****************************" << endl;
  	  cout << "************SHMS_TRIGGER_PULSE_PEDESTALS_HAVE PROBLEMS *********" << endl;
  	  cout << "************PLEASE LOOK AT SHMS_TRIGGER_SUM_PPED_HISTOGRAMS*****" << endl;    
  	}
    }
  //  no golden
  // TFile *f1= new TFile(Form("../../../../ROOTfiles/shms_replay_production_%d_%d.root",run,nevents));
  TFile *f1= new TFile(Form("ROOTfiles/shms_coin_replay_production_%d_%d.root",run,nevents));

  for (int i = 0; i < numHistos-2 ; i++)
    {
      hist[i] = (TH1D*)gDirectory->Get(histos[i]);
      // cout << "mean = " << hist[i]->GetMean() << endl;
      mean[i]=hist[i]->GetMean();
      rms[i]=hist[i]->GetRMS();
    }

  for (int i = 0; i < numHistos-2 ; i++)
    {
      meanDiff[i]=meanG[i]-mean[i];
      eff_rms[i] = sqrt(pow(rmsG[i], 2)+pow(rms[i], 2));
    }


  for (int i = 0; i < numHistos-2 ; i++)
    {
      // cout <<channel[i]<<""<<"\t"<< bar[i]<<"\t"<<variables[i]<<"\t"<<"\t"<<meanG[i]<<"\t\t"<<mean[i]<<"\t\t"<<meanDiff[i]<<endl;
      myfile<<histos[i]<<"\t"<<"\t"<<meanG[i]<<"\t\t"<<mean[i]<<"\t\t"<<meanDiff[i]<<endl;//to store values in .txt file

    }
 
  for (int i = 0; i < numHistos-2 ; i++)
    {
      if(abs(meanDiff[i])>2.5) //The following message will appear in terminal if Pedestal mean differnce is greater than 2.5
	{
	  
cout<<"_____________________________________________________________________"<<endl;
	  cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!Attention Needed !!!!!!!!!!!!!!!!!!!!!" << endl;
	  cout<<"_____________________________________________________________________"<<endl;
	 
	  cout << "--------------------See ROC2 --- SLOT 14---------------------------" << endl;//hallc_replay/MAPS/HMS/DETEC/TRIG/htrig.map
	  cout << "-----SEE THE PROBLEMATIC CHANNELS FOR SHMS TRIGGER SUM PPED---------"<<endl;
	  cout<<  "THE FOLLOWING FIGURES ARE THE VALUES OF SHMS TRIGGER PULSE PEDESTALS"<<endl; 
	  cout<<  " FOR SHMS  HEAVY GAS CERENKOV SUM, NOBEL GAS CERENKOV SUM AND SHWER SUM"<<endl;
	  cout << "NOTE----> MEAN VALUE OF PULSE PEDESTALS OF HMS GOLDEN RUN = MG-----"<<endl;
          cout<<  "NOTE----> MEAN FOR CURRENT RUN  = M  AND THEIR DIFFERNCE = MD------"<<endl;
          cout << "---------------ABSOLUTE VALUE OF MD should be < 2.5----------------"<< endl;
	  cout<< "------------BUT HERE YOU SEE THE MG VALUE(S) > 2.5-----------------"<<endl;
	  cout << "-----------------PLEASE CONSULT WITH  SHIFT LEADER-----------------" <<endl;
          cout<<"_____________________________________________________________________"<<endl;
	  cout << "!!!!!!!!!!!!!!!!!Attention Needed !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << endl;
	  cout << setprecision(4) << fixed <<"chan"<<" "<< channel[i] << " " <<"bar"<<" "<< bar[i] 
	       << "\t" <<  variables[i] << "\t" <<"MG"<<"  "<<meanG[i] <<"  " 
	       <<"M"<<"  "<< mean[i] <<" " <<"MD"<<" "<< meanDiff[i] << endl;//MG=mean golden,M = mean current, MD = mean diff
          cout<<"_____________________________________________________________________"<<endl;

       
	}
    }
  myfile <<"___________________________________________________________________________"<<endl;

  myfile.close();

  // TCanvas *c2 = new TCanvas();
    
  // double x[numHistos] ={1,2,3,4,5};
  
  
  // TGraphErrors *gr = new TGraphErrors(numHistos-2, x, meanDiff, 0, eff_rms); 

  // c2->cd(1);
  // gr->GetXaxis()->SetTitle(" SHMS HGC    NGC    PRSHWR");//     PHGC_SUM     PNGC_SUM");
  // gr->GetYaxis()->SetTitle("PED_MEAN_DIFF");
  // gr->SetTitle("SHMS PED HGC NGC PRSHWR Golden (1757)_vs_current_Run");
  // gr->SetMarkerColor(2);
  // gr->SetMarkerSize(1);
  // gr->SetMarkerStyle(3);
  // gr->Draw("AP*");

 
  // TLegend *leg = new TLegend(0.20,0.9,0.45,0.7);
  // leg->AddEntry(gr ,"1 = PHGC","p");
  // leg->AddEntry(gr ,"2 = PNGC","p");
  // leg->AddEntry(gr,"3 = PRSHWR","p");
  // //leg->AddEntry(gr ,"2 = PHGC_SUM","p");
  // // leg->AddEntry(gr,"3 = PNGC_SUM","p");


 
  // leg->Draw();
   
  
 
  // c2->Update();

  
   
}
