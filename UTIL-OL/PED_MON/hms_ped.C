//Read one 1D histogram from rootfile
void hms_ped(Int_t runNumber, Int_t eventNumber)
{
  const Int_t run          = runNumber;
  const  Int_t nevents     = eventNumber;
  const Int_t goldRunNum   = 1292;
  const Int_t numHistos  = 3;       //Number of histos

  // Define golden run in which to compare current run to
  
  TFile *f = new TFile(Form("ROOTfiles/hms_replay_production_%d_50000.root", goldRunNum));


  ofstream myfile;
  myfile.open("outHMS.txt"); //To store pedestals differences
  myfile <<"___________________________________________________________________________"<<endl;
  myfile <<"HMS PEDESTAL GOLDEN  RUN NUMBER =" "  "<<goldRunNum<<endl;
  myfile <<"HMS TRIGGER  PPED  RUN   NUMBER =" "  "<<run<<endl;
  myfile <<"___________________________________________________________________________"<<endl;
  myfile <<"hms_pped"<< "\t\t\t"<<"Mean_Golden"<<"\t"<<"Mean"<<"\t\t"<<"Mean_Diff"<<endl;
  myfile <<"___________________________________________________________________________"<<endl;


  
  TH1D *histG[numHistos];
  TH1D *hist[numHistos];
  Int_t channel [3]={13,12,15};//(see hallc_replay/MAPS/HMS/DETEC/TRIG/htrig.map)
  TString variables[numHistos]={"hSHWR","hPSHWR","hCER"};
  TString histos[numHistos]={"htrig_hshwr_adc_pped",  "htrig_hpreShwr_adc_pped", "htrig_hcersum_adc_pped" }; 
 
  Int_t plane=1;
  Int_t bar [3]={6,5,8}; //variables are corresponding to these bars

  //For Golden

  double meanG[numHistos],rmsG[numHistos];
  double mean[numHistos],rms[numHistos];
  double meanDiff[numHistos],rmsDiff[numHistos];
  double eff_rms[numHistos];

  for (int i = 0; i<numHistos ; i++)
    {
      histG[i] = (TH1D*)gDirectory->Get(histos[i]);
      //histG[i] = (TH1D*)gDirectory->GetList()->FindObject(branches[i]);
      if (histG[i]) {

	meanG[i]=histG[i]->GetMean();
	rmsG[i]=histG[i]->GetRMS();
      }
      else
	{

	  cout << "****************ATTENTION REQUIRED *****************************" << endl;
	  cout << "************HMS_TRIGGER_PULSE_PEDESTALS_HAVE PROBLEMS *********" << endl;
	  cout << "************PLEASE LOOK AT HMS_TRIGGER_SUM_PPED_HISTOGRAMS*****" << endl;
     

	}
    }
 
  
  // no golden

  TFile *f1= new TFile(Form("ROOTfiles/hms_replay_production_%d_%d.root",run,nevents));

  for (int i = 0; i<numHistos ; i++)
    {
      hist[i] = (TH1D*)gDirectory->Get(histos[i]);
      mean[i]=hist[i]->GetMean();
      rms[i]=hist[i]->GetRMS();
    }

  for (int i = 0; i<numHistos ; i++)
    {
      meanDiff[i]=meanG[i]-mean[i];
      eff_rms[i] = sqrt(pow(rmsG[i], 2)+pow(rms[i], 2));
    }



  for (int i = 0; i<numHistos ; i++)
    {
      //open this cout if we want to print values in terminal
      // cout <<channel[i]<<""<<"\t"<< bar[i]<<"\t"<<variables[i]<<"\t"<<"\t"<<meanG[i]<<"\t\t"<<mean[i]<<"\t\t"<<meanDiff[i]<<endl;
      myfile<<histos[i]<<"\t"<<"\t"<<meanG[i]<<"\t\t"<<mean[i]<<"\t\t"<<meanDiff[i]<<endl;//to stores values in .txt file


    }
  
  for (int i = 0; i<numHistos ; i++)
    {
      if(abs(meanDiff[i])>2.5) // //The following message will appear in terminal if Pedestal mean differnce is greater than 2.5
	{
	  cout<<"_____________________________________________________________________"<<endl;
	  cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!Attention Needed !!!!!!!!!!!!!!!!!!!!!" << endl;
	  cout<<"_____________________________________________________________________"<<endl;
	 
	  cout << "--------------------See ROC1 --- SLOT 17---------------------------" << endl;//hallc_replay/MAPS/HMS/DETEC/TRIG/htrig.map
	  cout << "-----SEE THE PROBLEMATIC CHANNELS FOR HMS TRIGGER SUM PPED---------"<<endl;
	  cout<<  "THE FOLLOWING FIGURES ARE THE VALUES OF HMS TRIGGER PULSE PEDESTALS"<<endl; 
	  cout<<  "------------- FOR SHWER SUM, PRESHWER SUM AND CERENKOV SUM---------"<<endl;
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
  myfile <<"____________________________________________________________________________"<<endl;

  myfile.close();

  // TCanvas *c2 = new TCanvas();
    
  // double x[numHistos] ={1,2,3};
  
  
  // TGraphErrors *gr = new TGraphErrors(numHistos, x, meanDiff, 0, eff_rms); 

  // c2->cd(1);
  // gr->GetXaxis()->SetTitle(" HMS SHWR    PRESHWR  CERENKOV");
  // gr->GetYaxis()->SetTitle("HMS PED_MEAN_DIFF");
  // gr->SetTitle("HMS PED SHWR PRESHWR CER Golden_vs_current_Run");
  // gr->SetMarkerColor(2);
  // gr->SetMarkerSize(1);
  // gr->SetMarkerStyle(3);
  // gr->Draw("AP*");

  //  TLegend *leg = new TLegend(0.2,0.9,0.5,0.7);
  //  leg->AddEntry(gr ,"1 = Pshwr","p");
  //  leg->AddEntry(gr ,"2 = Pprshwr","p");
  //  leg->AddEntry(gr ,"3 = Pcer","p");
 


  // leg->Draw();
   

 
  // c2->Update();


  
}
