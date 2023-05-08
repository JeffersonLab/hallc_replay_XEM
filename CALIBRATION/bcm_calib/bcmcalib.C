
#include "bcmcalib.h"


void bcmcalib( Int_t run_number = 20277){

/// Some style for the plots
gStyle->SetOptFit(0);
gStyle->SetOptStat(0);
gStyle->SetPalette(1);
gStyle->SetNdivisions(505);

cout << "Run number: " << run_number << endl;

Double_t g_unser = 1/4011.23;
//The scalers tree seem to be TSP
// === Information of the trees
TChain *TSP = LoadRun(run_number,"TSP");
Double_t bcm1rate, bcm2rate, bcm4arate, bcm4brate, bcm4crate;
Double_t unserrate;

TSP->SetBranchAddress("P.Unser.scalerRate", &unserrate);
TSP->SetBranchAddress("P.BCM1.scalerRate", &bcm1rate);
TSP->SetBranchAddress("P.BCM2.scalerRate", &bcm2rate);
TSP->SetBranchAddress("P.BCM4A.scalerRate", &bcm4arate);
TSP->SetBranchAddress("P.BCM4B.scalerRate", &bcm4brate);
TSP->SetBranchAddress("P.BCM4C.scalerRate", &bcm4crate);

Long64_t ev = TSP->GetEntries();
cout << "Number of Entries in TSP: " << ev << endl;

Double_t prevun, evun, unmean;
prevun = evun = unmean = 0; 

Int_t entryini, entryfin;
entryini = entryfin = 0;

std::vector<Double_t> mean_unser, meam_unsercur,  mean_bcm1, mean_bcm2, mean_bcm4a, mean_bcm4b, mean_bcm4c, mean_off;
std::vector<Double_t> SD_unser,  SD_bcm1, SD_bcm2, SD_bcm4a, SD_bcm4b, SD_bcm4c;
std::vector<Int_t> entry_initial, entry_final;

 std::vector<Double_t> unsertemp, unsertemp1, bcm1temp, bcm2temp, bcm4atemp, bcm4btemp, bcm4ctemp;
 Int_t count = 0;
//=========Reading all evens in the tree ========//
for (Int_t j=0; j <= ev; j++){
 TSP->GetEntry(j);
 evun = unserrate;
 //===== Add if they are in a range of +/- 10000 ==== //
 if(fabs(evun - prevun) < 10000){
   unsertemp.push_back(unserrate);
   bcm1temp.push_back(bcm1rate);
   bcm2temp.push_back(bcm2rate);
   bcm4atemp.push_back(bcm4arate);
   bcm4btemp.push_back(bcm4brate);
   bcm4ctemp.push_back(bcm4crate);
   entryfin =j;
   count++; 

   // if(unserrate < 700000){
   // bcm1temp.push_back(bcm1rate);
   // bcm2temp.push_back(bcm2rate);
      
   // }
  
 }

 
 //===== Reset if it is outside the range=============//
 else{
   //=====Store if at least 20 entries were in range ======//
   if(count > 20){   
    entry_initial.push_back(entryini);
    entry_final.push_back(entryfin);
    mean_unser.push_back(computeMean(unsertemp));
    
    // mean_unser.push_back(computeMean(unsertemp1));

    mean_bcm1.push_back(computeMean(bcm1temp));
    mean_bcm2.push_back(computeMean(bcm2temp));
    mean_bcm4a.push_back(computeMean(bcm4atemp));
    mean_bcm4b.push_back(computeMean(bcm4btemp));
    mean_bcm4c.push_back(computeMean(bcm4ctemp));
    SD_unser.push_back(calculateSD(unsertemp));
    SD_bcm1.push_back(calculateSD(bcm1temp));
    SD_bcm2.push_back(calculateSD(bcm2temp));
    SD_bcm4a.push_back(calculateSD(bcm4atemp));
    SD_bcm4b.push_back(calculateSD(bcm4btemp));
    SD_bcm4c.push_back(calculateSD(bcm4ctemp));

    // if(unserrate < 700000){
    // mean_bcm1.push_back(computeMean(bcm1temp));
    // mean_bcm2.push_back(computeMean(bcm2temp));
    // SD_bcm1.push_back(calculateSD(bcm1temp));
    // SD_bcm2.push_back(calculateSD(bcm2temp));
    // }
  
   }  

    unsertemp.clear(); bcm1temp.clear();  bcm2temp.clear(); 
    bcm4atemp.clear(); bcm4btemp.clear(); bcm4ctemp.clear();

   // unsertemp1.clear();
   // SD_unser.clear(); SD_bcm1.clear(); SD_bcm2.clear();
   // Results in some 0 error bars
   // SD_bcm4a.clear(); SD_bcm4b.clear(); SD_bcm4c.clear();
   // Results in some 0 error bars
    entryini =j;  
    count = 0;
 }

 prevun = unserrate;

}


//=====Make plots ===============
TCanvas *canv1 = new TCanvas("canv1","Unser", 1600,800);
canv1->Print(Form("BCM_%i.pdf[",run_number));

 Double_t ymin = *min_element(mean_unser.begin(), mean_unser.end()) - 100000;
 Double_t ymax = *max_element(mean_unser.begin(), mean_unser.end()) + 100000;
TH2F *unser = new TH2F("unser", "", ev/3, 0,ev,500,ymin,ymax);

TSP->Draw("P.Unser.scalerRate:Entry$>>unser","","goff");
unser->SetLineColor(2);
unser->SetLineWidth(5);
unser->Draw("BOX");
unser->GetXaxis()->SetTitle("Entry Number");
unser->GetYaxis()->SetTitle("Unser Rate");

TLine *l1;


//====== Draw Lines of means ===============================//
for (int k = 0; k<mean_unser.size(); k++){
  l1 = new TLine(entry_initial[k], mean_unser[k], entry_final[k], mean_unser[k]);
  l1->SetLineColor(1);
  l1->SetLineWidth(7);
  l1->Draw();
}

//======= Subtracting offset ============//

std::vector<Float_t> u_freq,  bcm1_freq, bcm2_freq, bcm4a_freq, bcm4b_freq, bcm4c_freq;
std::vector<Float_t> u_sd,  bcm1_sd, bcm2_sd, bcm4a_sd, bcm4b_sd, bcm4c_sd;
Float_t o_avg, u_avg;

Int_t u_size = mean_unser.size();
// Double_t bkg = 480000;
 /*if(run_number >18600){
   bkg = 600000;
 }
  if(run_number >19700){
   bkg = 730000;
 }
 */
 Double_t bkg = *min_element(mean_unser.begin(), mean_unser.end()) + 10000;
for (int k = 0; k<u_size; k++){
  if(mean_unser[k] > bkg && k > 0 && k< u_size-1){
    if( mean_unser[k-1] < bkg && mean_unser[k+1] < bkg ){
      o_avg = (mean_unser[k-1] + mean_unser[k+1])/2;
      u_avg = mean_unser[k] - o_avg;
      u_freq.push_back(u_avg*g_unser);
      bcm1_freq.push_back(mean_bcm1[k]);
      bcm2_freq.push_back(mean_bcm2[k]);
      bcm4a_freq.push_back(mean_bcm4a[k]);
      bcm4b_freq.push_back(mean_bcm4b[k]);
      bcm4c_freq.push_back(mean_bcm4c[k]);
      u_sd.push_back(0.16);
      bcm1_sd.push_back(SD_bcm1[k]);     
      bcm2_sd.push_back(SD_bcm2[k]);     
      bcm4a_sd.push_back(SD_bcm4a[k]);     
      bcm4b_sd.push_back(SD_bcm4b[k]);     
      bcm4c_sd.push_back(SD_bcm4c[k]);   

      // if(unserrate < 700000){
      // bcm1_freq.push_back(mean_bcm1[k]);
      // bcm2_freq.push_back(mean_bcm2[k]); 
      // bcm1_sd.push_back(SD_bcm1[k]);     
      // bcm2_sd.push_back(SD_bcm2[k]); 
      // }
    }
  }	
}

//transform(u_freq.begin(), u_freq.end(), u_freq.begin(), [g_unser](Double_t &c){return c*g_unser;});

canv1->Print(Form("BCM_%i.pdf",run_number));




//======= BCM1 ================================================//
TCanvas *Canvbcm1 = new TCanvas("Canvbcm1","BCM1",1600,800);
Canvbcm1->Divide(1,3);
Canvbcm1->cd(1);



 std::vector<Float_t>  unserbcm1, u_sdbcm1, bcm1_final,  bcm1_sd_final;
 for (Int_t i=0; i < u_freq.size(); i++){
   if (u_freq[i] < 55){
     unserbcm1.push_back(u_freq[i]);
     u_sdbcm1.push_back(u_sd[i]);
     bcm1_final.push_back(bcm1_freq[i]);
     bcm1_sd_final.push_back(bcm1_sd[i]);
   }
}
 TGraphErrors *gr1_2 = new TGraphErrors(unserbcm1.size(),unserbcm1.data(),bcm1_final.data(),u_sdbcm1.data(),bcm1_sd_final.data());
gr1_2->SetMarkerStyle(20);
gr1_2->SetTitle("");
gr1_2->GetYaxis()->SetTitle("BCM1 Frequency [Hz]");
gr1_2->GetYaxis()->SetTitleSize(0.06);
gr1_2->GetYaxis()->SetLabelSize(0.06);
gr1_2->GetXaxis()->SetTitle("Unser Current [#muA]");
gr1_2->GetXaxis()->SetTitleSize(0.055);
gr1_2->GetXaxis()->SetLabelSize(0.0555);
gr1_2->SetMarkerColor(1);
gr1_2->SetMarkerStyle(23);
gr1_2->SetMarkerSize(1.5);
//gr1_2->GetYaxis()->SetRangeUser(0,70);
gr1_2->Draw("AP");
gr1_2->Fit("pol1","q");
 TF1 *fit1 = gr1_2->GetFunction("pol1");
  gr1_2->GetFunction("pol1")->SetLineColor(2);
 // fit1->Draw("same");
Double_t g1, g1er, o1, o1er;
g1 = fit1->GetParameter(1); g1er= fit1->GetParError(1);
o1 = fit1->GetParameter(0); o1er= fit1->GetParError(0);

//Creates Correlation and Covariance Matrix
TFitResultPtr r1 = gr1_2->Fit("pol1", "S");

TMatrixD corr1 = r1->GetCorrelationMatrix();

TMatrixD cov1 = r1->GetCovarianceMatrix();


cout << "bcm1 gain: " << g1 << " +/- " << g1er << " bcm1 offset: " << o1 << " +/- " << o1er << endl;

//Prints Correlation Matrix
corr1.Print();

//Prints Covariance Matrix
cov1.Print();



Canvbcm1->cd(2);
std::vector<Float_t>  residual1, residual1er;
std::vector<Float_t>  relresidual1, relresidual1er;
 double residual_1, unresidual_1, db1_1, db2_1, dx_1, x_1, b1_1, relresidual_1,unrelresidual_1;
 residual_1=unresidual_1=db1_1=db2_1=dx_1=x_1=b1_1=relresidual_1,unrelresidual_1=0;

 for (Int_t i  = 0; i < bcm1_freq.size(); i++){
    if (u_freq[i] < 55){

 residual_1=(((bcm1_freq[i] - o1)/g1) - u_freq[i]);

 dx_1=sqrt(pow(bcm1_sd[i],2)+pow(o1er,2));
 x_1=bcm1_freq[i]-o1;
 //dx_1 =0
 b1_1=(bcm1_freq[i] - o1)/g1;
 db1_1=sqrt(pow(dx_1/x_1,2)+pow(g1er/g1,2));
 
 db2_1=u_sd[i];
 residual1.push_back(residual_1);

 unresidual_1=sqrt(pow(db1_1,2)+pow(db2_1,2));
  
 residual1er.push_back(unresidual_1);

//relative Residuals
  relresidual_1=(((bcm1_freq[i] - o1)/g1) - u_freq[i])/((bcm1_freq[i] - o1)/g1);
  relresidual1.push_back(relresidual_1);
  unrelresidual_1=sqrt(pow(unresidual_1/residual_1,2)+pow(db1_1/b1_1,2))*abs(relresidual_1);
  relresidual1er.push_back(unrelresidual_1);
  
  //cout << unresidual_1 << endl;
    }
}


 
TGraphErrors *gr1_3 = new TGraphErrors(unserbcm1.size(),unserbcm1.data(),residual1.data(), u_sdbcm1.data(), residual1er.data()); 
gr1_3->SetMarkerStyle(20);
gr1_3->SetTitle("");
gr1_3->GetYaxis()->SetTitle("Fit - Unser current");
gr1_3->GetYaxis()->SetTitleSize(0.06);
gr1_3->GetYaxis()->SetLabelSize(0.06);
gr1_3->GetXaxis()->SetTitle("Unser Current [#muA]");
gr1_3->GetXaxis()->SetTitleSize(0.055);
gr1_3->GetXaxis()->SetLabelSize(0.0555);
gr1_3->SetMarkerColor(7);
gr1_3->SetMarkerStyle(21);
gr1_3->SetMarkerSize(1.35);
gr1_3->Draw("AP");
Double_t xmin = *min_element(unserbcm1.begin(), unserbcm1.end());
Double_t xmax= *max_element(unserbcm1.begin(), unserbcm1.end());
TLine *l3 = new TLine(xmin, 0, xmax, 0);
l3->SetLineColor(2);
l3->SetLineWidth(2);
l3->Draw();
 
//relative Residuals
Canvbcm1->cd(3);


TGraphErrors *gr1_4 = new TGraphErrors(unserbcm1.size(),unserbcm1.data(),relresidual1.data(), u_sdbcm1.data(), relresidual1er.data());
gr1_4->SetMarkerStyle(20);
gr1_4->SetTitle("");
gr1_4->GetYaxis()->SetTitle("(Fit - Unser current)/Fit");
gr1_4->GetYaxis()->SetTitleSize(0.06);
gr1_4->GetYaxis()->SetLabelSize(0.06);
gr1_4->GetXaxis()->SetTitle("Unser Current [#muA]");
gr1_4->GetXaxis()->SetTitleSize(0.055);
gr1_4->GetXaxis()->SetLabelSize(0.0555);
gr1_4->SetMarkerColor(3);
gr1_4->SetMarkerStyle(20);
gr1_4->SetMarkerSize(1.35);
gr1_4->Draw("AP");
l3->Draw();


 Canvbcm1->Print(Form("BCM_%i.pdf",run_number));




//======= BCM2 ================================================//
TCanvas *Canvbcm2 = new TCanvas("Canvbcm2","BCM2",1600,800);
Canvbcm2->Divide(1,3);
Canvbcm2->cd(1);

 std::vector<Float_t> unserbcm2, u_sdbcm2, bcm2_final,  bcm2_sd_final;
 for (Int_t i=0; i < u_freq.size(); i++){
   if (u_freq[i] < 55){
     unserbcm2.push_back(u_freq[i]);
     u_sdbcm2.push_back(u_sd[i]);
     bcm2_final.push_back(bcm2_freq[i]);
     bcm2_sd_final.push_back(bcm2_sd[i]);
   }
}

 TGraphErrors *gr2_2 = new TGraphErrors(unserbcm2.size(),unserbcm2.data(),bcm2_final.data(),u_sdbcm2.data(),bcm2_sd_final.data());
gr2_2->SetMarkerStyle(20);
gr2_2->SetTitle("");
gr2_2->GetYaxis()->SetTitle("BCM2 Frequency [Hz]");
gr2_2->GetYaxis()->SetTitleSize(0.06);
gr2_2->GetYaxis()->SetLabelSize(0.06);
gr2_2->GetXaxis()->SetTitle("Unser Current [#muA]");
gr2_2->GetXaxis()->SetTitleSize(0.055);
gr2_2->GetXaxis()->SetLabelSize(0.0555);
gr2_2->SetMarkerColor(1);
gr2_2->SetMarkerStyle(23);
gr2_2->SetMarkerSize(1.5);
//gr1_2->GetYaxis()->SetRangeUser(0,70);
gr2_2->Draw("AP");
gr2_2->Fit("pol1","q");
TF1 *fit2 = gr2_2->GetFunction("pol1");
 gr2_2->SetLineColor(2);
Double_t g2, g2er, o2, o2er;
g2 = fit2->GetParameter(1); g2er= fit2->GetParError(1);
o2 = fit2->GetParameter(0); o2er= fit2->GetParError(0);


//Creates Correlation and Covariance Matrix
TFitResultPtr r2 = gr2_2->Fit("pol1", "S");

TMatrixD corr2 = r2->GetCorrelationMatrix();

TMatrixD cov2 = r2->GetCovarianceMatrix();

cout << "bcm2 gain: " << g2 << " +/- " << g2er << " bcm2 offset: " << o2 << " +/- " << o2er << endl;

//Prints Correlation Matrix
corr2.Print();

//Prints Covariance Matrix
cov2.Print();


Canvbcm2->cd(2);
std::vector<Float_t>  residual2, residual2er;
std::vector<Float_t>  relresidual2, relresidual2er;
 double residual_2, unresidual_2, db1_2, db2_2, dx_2, x_2, b1_2, relresidual_2 ,unrelresidual_2;
 residual_2=unresidual_2=db1_2=db2_2=dx_2=x_2=b1_2=relresidual_2,unrelresidual_2=0;

for (Int_t i  = 0 ; i < bcm2_freq.size(); i++){
   if (u_freq[i] < 55){
 residual_2=(((bcm2_freq[i] - o2)/g2) - u_freq[i]);

 dx_2=sqrt(pow(bcm2_sd[i],2)+pow(o2er,2));
 x_2=bcm2_freq[i]-o2;
 //dx_2=0
 b1_2=(bcm2_freq[i] - o2)/g2;
 db1_2=sqrt(pow(dx_2/x_2,2)+pow(g2er/g2,2));
 
 db2_2=u_sd[i];
 residual2.push_back(residual_2);

 unresidual_2=sqrt(pow(db1_2,2)+pow(db2_2,2));

 residual2er.push_back(unresidual_2);

//relative Residuals
  relresidual_2=(((bcm2_freq[i] - o2)/g2) - u_freq[i])/((bcm2_freq[i] - o2)/g2);
  relresidual2.push_back(relresidual_2);
  unrelresidual_2=sqrt(pow(unresidual_2/residual_2,2)+pow(db1_2/b1_2,2))*abs(relresidual_2);
  relresidual2er.push_back(unrelresidual_2);
   }
}

TGraphErrors *gr2_3 = new TGraphErrors(unserbcm2.size(),unserbcm2.data(),residual2.data(), u_sdbcm2.data(), residual2er.data()); 
gr2_3->SetMarkerStyle(20);
gr2_3->SetTitle("");
gr2_3->GetYaxis()->SetTitle("Fit - Unser current");
gr2_3->GetYaxis()->SetTitleSize(0.06);
gr2_3->GetYaxis()->SetLabelSize(0.06);
gr2_3->GetXaxis()->SetTitle("Unser Current [#muA]");
gr2_3->GetXaxis()->SetTitleSize(0.055);
gr2_3->GetXaxis()->SetLabelSize(0.0555);
gr2_3->SetMarkerColor(7);
gr2_3->SetMarkerStyle(21);
gr2_3->SetMarkerSize(1.35);
gr2_3->Draw("AP");
 Double_t xmin2 = *min_element(unserbcm2.begin(), unserbcm2.end());
 Double_t xmax2= *max_element(unserbcm2.begin(), unserbcm2.end());
TLine *l4 = new TLine(xmin2, 0, xmax2, 0);;
l4->SetLineColor(2);
l4->SetLineWidth(2);
l4->Draw();
 
//relative Residuals
Canvbcm2->cd(3);

TGraphErrors *gr2_4 = new TGraphErrors(unserbcm2.size(),unserbcm2.data(),relresidual2.data(), u_sdbcm2.data(), relresidual2er.data());
gr2_4->SetMarkerStyle(20);
gr2_4->SetTitle("");
gr2_4->GetYaxis()->SetTitle("(Fit - Unser current)/Fit");
gr2_4->GetYaxis()->SetTitleSize(0.06);
gr2_4->GetYaxis()->SetLabelSize(0.06);
gr2_4->GetXaxis()->SetTitle("Unser Current [#muA]");
gr2_4->GetXaxis()->SetTitleSize(0.055);
gr2_4->GetXaxis()->SetLabelSize(0.0555);
gr2_4->SetMarkerColor(3);
gr2_4->SetMarkerStyle(20);
gr2_4->SetMarkerSize(1.35);
gr2_4->Draw("AP");
l4->Draw();


 Canvbcm2->Print(Form("BCM_%i.pdf",run_number));



//======= BCM4A ================================================//
TCanvas *Canvbcm4a = new TCanvas("Canvbcm4a","BCM4A",1600,800);
Canvbcm4a->Divide(1,3);
Canvbcm4a->cd(1);

TGraphErrors *gr4a_2 = new TGraphErrors(u_freq.size(),u_freq.data(),bcm4a_freq.data(),u_sd.data(),bcm4a_sd.data());
gr4a_2->SetMarkerStyle(20);
gr4a_2->SetTitle("");
gr4a_2->GetYaxis()->SetTitle("BCM4A Frequency [Hz]");
gr4a_2->GetYaxis()->SetTitleSize(0.06);
gr4a_2->GetYaxis()->SetLabelSize(0.06);
gr4a_2->GetXaxis()->SetTitle("Unser Current [#muA]");
gr4a_2->GetXaxis()->SetTitleSize(0.055);
gr4a_2->GetXaxis()->SetLabelSize(0.0555);
gr4a_2->SetMarkerColor(1);
gr4a_2->SetMarkerStyle(23);
gr4a_2->SetMarkerSize(1.5);
//gr1_2->GetYaxis()->SetRangeUser(0,70);
gr4a_2->Draw("AP");
gr4a_2->Fit("pol1","q");
TF1 *fit4a = gr4a_2->GetFunction("pol1");
 gr4a_2->SetLineColor(2);
Double_t g4a, g4aer, o4a, o4aer;
g4a = fit4a->GetParameter(1); g4aer= fit4a->GetParError(1);
o4a = fit4a->GetParameter(0); o4aer= fit4a->GetParError(0);


//Creates Correlation and Covariance Matrix
TFitResultPtr r3 = gr4a_2->Fit("pol1", "S");

TMatrixD corr4a = r3->GetCorrelationMatrix();

TMatrixD cov4a = r3->GetCovarianceMatrix();

cout << "bcm4a gain: " << g4a << " +/- " << g4aer << " bcm4a offset: " << o4a << " +/- " << o4aer << endl;

//Prints Correlation Matrix
corr4a.Print();

//Prints Covariance Matrix
cov4a.Print();



Canvbcm4a->cd(2);

std::vector<Float_t>  residual4a, residual4aer;
std::vector<Float_t>  relresidual4a, relresidual4aer;
 double residual_4a, unresidual_4a, db1_4a, db2_4a, dx_4a, x_4a, b1_4a, relresidual_4a,unrelresidual_4a;
 residual_4a=unresidual_4a=db1_4a=db2_4a=dx_4a=x_4a=b1_4a=relresidual_4a,unrelresidual_4a=0;

for (Int_t i  = 0 ; i < bcm4a_freq.size(); i++){
  
 residual_4a=(((bcm4a_freq[i] - o4a)/g4a) - u_freq[i]);

 dx_4a=sqrt(pow(bcm4a_sd[i],2)+pow(o4aer,2));
 x_4a=bcm4a_freq[i]-o4a;
 //dx_4a=0
 b1_4a=(bcm4a_freq[i] - o4a)/g4a;
 db1_4a=sqrt(pow(dx_4a/x_4a,2)+pow(g4aer/g4a,2));
 
 db2_4a=u_sd[i];
 residual4a.push_back(residual_4a);

 unresidual_4a=sqrt(pow(db1_4a,2)+pow(db2_4a,2));

 residual4aer.push_back(unresidual_4a);

  //relative Residuals
  relresidual_4a=(((bcm4a_freq[i] - o4a)/g4a) - u_freq[i])/((bcm4a_freq[i] - o4a)/g4a);
  relresidual4a.push_back(relresidual_4a);
  unrelresidual_4a=sqrt(pow(unresidual_4a/residual_4a,2)+pow(db1_4a/b1_4a,2))*abs(relresidual_4a);
  relresidual4aer.push_back(unrelresidual_4a);
}


TGraphErrors *gr4a_3 = new TGraphErrors(u_freq.size(),u_freq.data(),residual4a.data(), u_sd.data(), residual4aer.data()); 
gr4a_3->SetMarkerStyle(20);
gr4a_3->SetTitle("");
gr4a_3->GetYaxis()->SetTitle("Fit - Unser current");
gr4a_3->GetYaxis()->SetTitleSize(0.06);
gr4a_3->GetYaxis()->SetLabelSize(0.06);
gr4a_3->GetXaxis()->SetTitle("Unser Current [#muA]");
gr4a_3->GetXaxis()->SetTitleSize(0.055);
gr4a_3->GetXaxis()->SetLabelSize(0.0555);
gr4a_3->SetMarkerColor(7);
gr4a_3->SetMarkerStyle(21);
gr4a_3->SetMarkerSize(1.35);
gr4a_3->Draw("AP");
 Double_t xmin3 = *min_element(u_freq.begin(), u_freq.end());
 Double_t xmax3= *max_element(u_freq.begin(), u_freq.end());
TLine *l5 = new TLine(xmin3, 0, xmax3, 0);
l5->SetLineColor(2);
l5->SetLineWidth(2);
l5->Draw();
 
//relative Residuals
Canvbcm4a->cd(3);

TGraphErrors *gr4a_4 = new TGraphErrors(u_freq.size(),u_freq.data(),relresidual4a.data(), u_sd.data(), relresidual4aer.data());
gr4a_4->SetMarkerStyle(20);
gr4a_4->SetTitle("");
gr4a_4->GetYaxis()->SetTitle("(Fit - Unser current)/Fit");
gr4a_4->GetYaxis()->SetTitleSize(0.06);
gr4a_4->GetYaxis()->SetLabelSize(0.06);
gr4a_4->GetXaxis()->SetTitle("Unser Current [#muA]");
gr4a_4->GetXaxis()->SetTitleSize(0.055);
gr4a_4->GetXaxis()->SetLabelSize(0.0555);
gr4a_4->SetMarkerColor(3);
gr4a_4->SetMarkerStyle(20);
gr4a_4->SetMarkerSize(1.35);
gr4a_4->Draw("AP");
l5->Draw();



 Canvbcm4a->Print(Form("BCM_%i.pdf",run_number));





//======= BCM4B ================================================//
TCanvas *Canvbcm4b = new TCanvas("Canvbcm4b","BCM4B",1600,800);
Canvbcm4b->Divide(1,3);
Canvbcm4b->cd(1);

TGraphErrors *gr4b_2 = new TGraphErrors(u_freq.size(),u_freq.data(),bcm4b_freq.data(),u_sd.data(),bcm4b_sd.data());
gr4b_2->SetMarkerStyle(20);
gr4b_2->SetTitle("");
gr4b_2->GetYaxis()->SetTitle("BCM4B Frequency [Hz]");
gr4b_2->GetYaxis()->SetTitleSize(0.06);
gr4b_2->GetYaxis()->SetLabelSize(0.06);
gr4b_2->GetXaxis()->SetTitle("Unser Current [#muA]");
gr4b_2->GetXaxis()->SetTitleSize(0.055);
gr4b_2->GetXaxis()->SetLabelSize(0.0555);
gr4b_2->SetMarkerColor(1);
gr4b_2->SetMarkerStyle(23);
gr4b_2->SetMarkerSize(1.5);
//gr1_2->GetYaxis()->SetRangeUser(0,70);
gr4b_2->Draw("AP");
gr4b_2->Fit("pol1","q");
TF1 *fit4b = gr4b_2->GetFunction("pol1");
 gr4b_2->SetLineColor(2);
Double_t g4b, g4ber, o4b, o4ber;
g4b = fit4b->GetParameter(1); g4ber= fit4b->GetParError(1);
o4b = fit4b->GetParameter(0); o4ber= fit4b->GetParError(0);


//Creates Correlation and Covariance Matrix
TFitResultPtr r4 = gr4b_2->Fit("pol1", "S");

TMatrixD corr4b = r4->GetCorrelationMatrix();

TMatrixD cov4b = r4->GetCovarianceMatrix();

cout << "bcm4b gain: " << g4b << " +/- " << g4ber << " bcm4b offset: " << o4b << " +/- " << o4ber << endl;

//Prints Correlation Matrix
corr4b.Print();

//Prints Covariance Matrix
cov4b.Print();


Canvbcm4b->cd(2);
std::vector<Float_t>  residual4b, residual4ber;
std::vector<Float_t>  relresidual4b, relresidual4ber;
 double residual_4b, unresidual_4b, db1_4b, db2_4b, dx_4b, x_4b, b1_4b, relresidual_4b,unrelresidual_4b;
 residual_4b=unresidual_4b=db1_4b=db2_4b=dx_4b=x_4b=b1_4b=relresidual_4b,unrelresidual_4b=0;

for (Int_t i  = 0 ; i < bcm4b_freq.size(); i++){

 residual_4b=(((bcm4b_freq[i] - o4b)/g4b) - u_freq[i]);

 dx_4b=sqrt(pow(bcm4b_sd[i],2)+pow(o4ber,2));
 x_4b=bcm4b_freq[i]-o4b;
 //dx_4b=0
 b1_4b=(bcm4b_freq[i] - o4b)/g4b;
 db1_4b=sqrt(pow(dx_4b/x_4b,2)+pow(g4ber/g4b,2));
 
 db2_4b=u_sd[i];
 residual4b.push_back(residual_4b);

 unresidual_4b=sqrt(pow(db1_4b,2)+pow(db2_4b,2));

 residual4ber.push_back(unresidual_4b);

 //relative Residuals
  relresidual_4b=(((bcm4b_freq[i] - o4b)/g4b) - u_freq[i])/((bcm4b_freq[i] - o4b)/g4b);
  relresidual4b.push_back(relresidual_4b);
  unrelresidual_4b=sqrt(pow(unresidual_4b/residual_4b,2)+pow(db1_4b/b1_4b,2))*abs(relresidual_4b);
  relresidual4ber.push_back(unrelresidual_4b);

}

TGraphErrors *gr4b_3 = new TGraphErrors(u_freq.size(),u_freq.data(),residual4b.data(), u_sd.data(), residual4ber.data()); 
gr4b_3->SetMarkerStyle(20);
gr4b_3->SetTitle("");
gr4b_3->GetYaxis()->SetTitle("Fit - Unser current");
gr4b_3->GetYaxis()->SetTitleSize(0.06);
gr4b_3->GetYaxis()->SetLabelSize(0.06);
gr4b_3->GetXaxis()->SetTitle("Unser Current [#muA]");
gr4b_3->GetXaxis()->SetTitleSize(0.055);
gr4b_3->GetXaxis()->SetLabelSize(0.0555);
gr4b_3->SetMarkerColor(7);
gr4b_3->SetMarkerStyle(21);
gr4b_3->SetMarkerSize(1.35);
gr4b_3->Draw("AP");
TLine *l6 = new TLine(xmin3, 0, xmax3, 0);
l6->SetLineColor(2);
l6->SetLineWidth(2);
l6->Draw();
 
//relative Residuals
Canvbcm4b->cd(3);

TGraphErrors *gr4b_4 = new TGraphErrors(u_freq.size(),u_freq.data(),relresidual4b.data(), u_sd.data(), relresidual4ber.data());
gr4b_4->SetMarkerStyle(20);
gr4b_4->SetTitle("");
gr4b_4->GetYaxis()->SetTitle("(Fit - Unser current)/Fit");
gr4b_4->GetYaxis()->SetTitleSize(0.06);
gr4b_4->GetYaxis()->SetLabelSize(0.06);
gr4b_4->GetXaxis()->SetTitle("Unser Current [#muA]");
gr4b_4->GetXaxis()->SetTitleSize(0.055);
gr4b_4->GetXaxis()->SetLabelSize(0.0555);
gr4b_4->SetMarkerColor(3);
gr4b_4->SetMarkerStyle(20);
gr4b_4->SetMarkerSize(1.35);
gr4b_4->Draw("AP");
l6->Draw();



Canvbcm4b->Print(Form("BCM_%i.pdf",run_number));




//======= BCM4C ================================================//
TCanvas *Canvbcm4c = new TCanvas("Canvbcm4c","BCM4C",1600,800);
Canvbcm4c->Divide(1,3);
Canvbcm4c->cd(1);

TGraphErrors *gr4c_2 = new TGraphErrors(u_freq.size(),u_freq.data(),bcm4c_freq.data(),u_sd.data(),bcm4c_sd.data());
gr4c_2->SetMarkerStyle(20);
gr4c_2->SetTitle("");
gr4c_2->GetYaxis()->SetTitle("BCM4C Frequency [Hz]");
gr4c_2->GetYaxis()->SetTitleSize(0.06);
gr4c_2->GetYaxis()->SetLabelSize(0.06);
gr4c_2->GetXaxis()->SetTitle("Unser Current [#muA]");
gr4c_2->GetXaxis()->SetTitleSize(0.055);
gr4c_2->GetXaxis()->SetLabelSize(0.0555);
gr4c_2->SetMarkerColor(1);
gr4c_2->SetMarkerStyle(23);
gr4c_2->SetMarkerSize(1.5);
//gr1_2->GetYaxis()->SetRangeUser(0,70);
gr4c_2->Draw("AP");
gr4c_2->Fit("pol1","q");
TF1 *fit4c = gr4c_2->GetFunction("pol1");
 gr4c_2->SetLineColor(2);
Double_t g4c, g4cer, o4c, o4cer;
g4c = fit4c->GetParameter(1); g4cer= fit4c->GetParError(1);
o4c = fit4c->GetParameter(0); o4cer= fit4c->GetParError(0);


//Creates Correlation and Covariance Matrix
TFitResultPtr r5 = gr4c_2->Fit("pol1", "S");

TMatrixD corr4c = r5->GetCorrelationMatrix();

TMatrixD cov4c = r5->GetCovarianceMatrix();

cout << "bcm4c gain: " << g4c << " +/- " << g4cer << " bcm4c offset: " << o4c << " +/- " << o4cer << endl;

//Prints Correlation Matrix
corr4c.Print();

//Prints Covariance Matrix
cov4c.Print();


Canvbcm4c->cd(2);
std::vector<Float_t>  residual4c, residual4cer;
std::vector<Float_t>  relresidual4c, relresidual4cer;
double residual_4c, unresidual_4c, db1_4c, db2_4c, dx_4c, x_4c, b1_4c, relresidual_4c,unrelresidual_4c;
 residual_4c=unresidual_4c=db1_4c=db2_4c=dx_4c=x_4c=b1_4c=relresidual_4c,unrelresidual_4c=0;

for (Int_t i  = 0 ; i < bcm4c_freq.size(); i++){

 residual_4c=(((bcm4c_freq[i] - o4c)/g4c) - u_freq[i]);

 dx_4c=sqrt(pow(bcm4c_sd[i],2)+pow(o4cer,2));
 x_4c=bcm4c_freq[i]-o4c;
 //dx_4c=0
 b1_4c=(bcm4c_freq[i] - o4c)/g4c; 
 db1_4c=sqrt(pow(dx_4c/x_4c,2)+pow(g4cer/g4c,2));
 
 db2_4c=u_sd[i];
 residual4c.push_back(residual_4c);

 unresidual_4c=sqrt(pow(db1_4c,2)+pow(db2_4c,2));

 residual4cer.push_back(unresidual_4c);

 //relative Residuals
  relresidual_4c=(((bcm4c_freq[i] - o4c)/g4c) - u_freq[i])/((bcm4c_freq[i] - o4c)/g4c);
  relresidual4c.push_back(relresidual_4c);
  unrelresidual_4c=sqrt(pow(unresidual_4c/residual_4c,2)+pow(db1_4c/b1_4c,2))*abs(relresidual_4c);
  relresidual4cer.push_back(unrelresidual_4c);
}

TGraphErrors *gr4c_3 = new TGraphErrors(u_freq.size(),u_freq.data(),residual4c.data(), u_sd.data(), residual4cer.data()); 
gr4c_3->SetMarkerStyle(20);
gr4c_3->SetTitle("");
gr4c_3->GetYaxis()->SetTitle("Fit - Unser current");
gr4c_3->GetYaxis()->SetTitleSize(0.06);
gr4c_3->GetYaxis()->SetLabelSize(0.06);
gr4c_3->GetXaxis()->SetTitle("Unser Current [#muA]");
gr4c_3->GetXaxis()->SetTitleSize(0.055);
gr4c_3->GetXaxis()->SetLabelSize(0.0555);
gr4c_3->SetMarkerColor(7);
gr4c_3->SetMarkerStyle(21);
gr4c_3->SetMarkerSize(1.5);
gr4c_3->Draw("AP");
TLine *l7 = new TLine(xmin3, 0, xmax3, 0);
l7->SetLineColor(2);
l7->SetLineWidth(2);
l7->Draw();
 
//relative Residuals
Canvbcm4c->cd(3);


TGraphErrors *gr4c_4 = new TGraphErrors(u_freq.size(),u_freq.data(),relresidual4c.data(), u_sd.data(), relresidual4cer.data());
gr4c_4->SetMarkerStyle(20);
gr4c_4->SetTitle("");
gr4c_4->GetYaxis()->SetTitle("(Fit - Unser current)/Fit");
gr4c_4->GetYaxis()->SetTitleSize(0.06);
gr4c_4->GetYaxis()->SetLabelSize(0.06);
gr4c_4->GetXaxis()->SetTitle("Unser Current [#muA]");
gr4c_4->GetXaxis()->SetTitleSize(0.055);
gr4c_4->GetXaxis()->SetLabelSize(0.0555);
gr4c_4->SetMarkerColor(3);
gr4c_4->SetMarkerStyle(20);
gr4c_4->SetMarkerSize(1.35);
gr4c_4->Draw("AP");
l7->Draw();


 Canvbcm4c->Print(Form("BCM_%i.pdf",run_number));
Canvbcm4c->Print(Form("BCM_%i.pdf]",run_number));




}








 
