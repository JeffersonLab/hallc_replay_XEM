/**********************************************************************
 * This is a root macro that can be run in root as:
 *             .x makeAlignmentPlots.C
 *
 * This will output the plots that are needed to check 
 * the alignment of the drift chambers. The global parameters 
 * must be the same as those used in the script alignDC.py. 
 * The input files are in the output_align folder and the path here 
 * should be modified to get to them. This outputs a file, 
 * offsets.txt which must be used for the next iteration to align 
 * the drift chambers. (Take a total of about 2-4 iterations.)
 *
 *
 * Author: Holly Szumila, hszumila@jlab.org
 * Date: 27 September 2017
 *
 *********************************************************************/


//define some globals:
float xposStep = 0.001;
float yposStep = 0.001;
float xposInit = -0.05;
float yposInit = -0.05;
float xposMax = 0.05;
float yposMax = 0.05;


void makeAlignmentPlots(){
 
  std::string pdf_file_name_A = "output_alignment_plots.pdf";
  std::string pdf_file_name_R = "output_residual_plots.pdf";
  gStyle->SetOptStat(0);

  const int npoints = 1000;
  const int nplanes = 12;
  gROOT->SetBatch(kTRUE);

  //plots by plane and by shift
  TCanvas *cx[nplanes][npoints];
  TCanvas *cy[nplanes][npoints];
  
  //store the minimum shift value for x,y
  double shiftValue[nplanes][2];

  //loop plane interested in
  for (int npl=0; npl<nplanes; npl++){
    double xdelta[npoints];
    double ydelta[npoints];
    double xmean[npoints];
    double ymean[npoints];
    int nx=0;
    //loop x position
    for (int xposShort=-50; xposShort<50; xposShort+=1){
      float xpos = (float)xposShort/1000;
      //for (float xpos=xposInit; xpos<xposMax; xpos+=xposStep){ 
      //open the file
      //if (xpos == -0.0){xpos = abs(xpos);}
      cout<<"xpos:\t"<<xpos<<endl;
      TFile *f = new TFile(Form("hallc_replay/CALIBRATION/shms_dc_calib/output_align/shms_replay_%d_xpos_%.3f.root",npl,xpos));
      double sumMean = 0.0;
      xdelta[nx] = xpos;
      TF1 *fgaus[nplanes];

      //load the histograms
      TH1F *h_res[nplanes];
      h_res[0] = (TH1F*)f->Get("pdc1u1_residuals");
      h_res[1] = (TH1F*)f->Get("pdc1u2_residuals");
      h_res[2] = (TH1F*)f->Get("pdc1x1_residuals");
      h_res[3] = (TH1F*)f->Get("pdc1x2_residuals");  
      h_res[4] = (TH1F*)f->Get("pdc1v1_residuals"); 
      h_res[5] = (TH1F*)f->Get("pdc1v2_residuals");
      h_res[6] = (TH1F*)f->Get("pdc2u1_residuals"); 
      h_res[7] = (TH1F*)f->Get("pdc2u2_residuals");
      h_res[8] = (TH1F*)f->Get("pdc2x1_residuals");
      h_res[9] = (TH1F*)f->Get("pdc2x2_residuals");
      h_res[10] = (TH1F*)f->Get("pdc2v1_residuals");
      h_res[11] = (TH1F*)f->Get("pdc2v2_residuals");
      
      cx[npl][nx] = new TCanvas(Form("cx_%d_%d",npl,nx),Form("cx_%d_%d",npl,nx),800,800);
      cx[npl][nx]->Divide(4,3);
      //get the histogram of the relevant plane and fit
      for (int ii=0; ii<nplanes; ii++){
	cx[npl][nx]->cd(ii+1);
	h_res[ii]->Draw();
	fgaus[ii] = new TF1(Form("fg_%d",ii), "gaus",h_res[ii]->GetMean()-0.1,h_res[ii]->GetMean()+0.1);
	h_res[ii]->Fit(Form("fg_%d",ii),"RQ");
	gStyle->SetOptFit(111);
	if (ii==0){
	  TPad *newpad=new TPad("newpad","a transparent pad",0,0,1,1);
	  newpad->SetFillStyle(4000);
	  newpad->Draw();
	  newpad->cd();
	  TPaveLabel *title = new TPaveLabel(0.1,0.88,0.9,0.92,Form("Plane_%d, x-shift_%.3f",npl, xpos));
	  title->SetFillColor(16);
	  title->SetTextFont(52);
	  title->Draw();
	}
	sumMean += abs(fgaus[ii]->GetParameter(1));
      }
      
      cx[npl][nx]->Update();
      cx[npl][nx]->Print((pdf_file_name_R+"(").c_str());
      cx[npl][nx]->Close();
      xmean[nx] = sumMean;
      nx++;
      f->Close();
    } //end xpos loop
    
    int ny=0;
    //loop the y position
    for (int yposShort=-50; yposShort<50; yposShort+=1){
      float ypos = (float)yposShort/1000;
    //for (float ypos=yposInit; ypos<yposMax; ypos+=yposStep){ 
      //if (ypos == -0.0){ypos = abs(ypos);}
      //open the file
      TFile *f = new TFile(Form("hallc_replay/CALIBRATION/shms_dc_calib/output_align/shms_replay_%d_ypos_%.3f.root",npl,ypos));
      double sumMean = 0.0;
      ydelta[ny] = ypos;
      TF1 *fgaus[nplanes]; 
      
      //load the histograms
      TH1F *h_res[nplanes];
      h_res[0] = (TH1F*)f->Get("pdc1u1_residuals");
      h_res[1] = (TH1F*)f->Get("pdc1u2_residuals");
      h_res[2] = (TH1F*)f->Get("pdc1x1_residuals");
      h_res[3] = (TH1F*)f->Get("pdc1x2_residuals");  
      h_res[4] = (TH1F*)f->Get("pdc1v1_residuals"); 
      h_res[5] = (TH1F*)f->Get("pdc1v2_residuals");
      h_res[6] = (TH1F*)f->Get("pdc2u1_residuals"); 
      h_res[7] = (TH1F*)f->Get("pdc2u2_residuals");
      h_res[8] = (TH1F*)f->Get("pdc2x1_residuals");
      h_res[9] = (TH1F*)f->Get("pdc2x2_residuals");
      h_res[10] = (TH1F*)f->Get("pdc2v1_residuals");
      h_res[11] = (TH1F*)f->Get("pdc2v2_residuals");
      
      cy[npl][ny] = new TCanvas(Form("cy_%d_%d",npl,ny),Form("cy_%d_%d",npl,ny),800,800);
      cy[npl][ny]->Divide(4,3);
      //get the histogram of the relevant plane and fit
      for (int ii=0; ii<nplanes; ii++){
	cy[npl][ny]->cd(ii+1);
	h_res[ii]->Draw();
	fgaus[ii] = new TF1(Form("fg_%d",ii), "gaus",h_res[ii]->GetMean()-0.1,h_res[ii]->GetMean()+0.1);
	h_res[ii]->Fit(Form("fg_%d",ii),"RQ");
	gStyle->SetOptFit(111);
	if (ii==0){
	  TPad *newpad=new TPad("newpad","a transparent pad",0,0,1,1);
	  newpad->SetFillStyle(4000);
	  newpad->Draw();
	  newpad->cd();
	  TPaveLabel *title = new TPaveLabel(0.1,0.88,0.9,0.92,Form("Plane_%d, y-shift_%.3f",npl, ypos));
	  title->SetFillColor(16);
	  title->SetTextFont(52);
	  title->Draw();
	}
	sumMean += abs(fgaus[ii]->GetParameter(1));
      }    
      cy[npl][ny]->Update();
      cy[npl][ny]->Print((pdf_file_name_R+"(").c_str());
      cy[npl][ny]->Close();
      ymean[ny] = sumMean;
      ny++;
      f->Close();
    }//end ypos loop
    
    
    //cut down arrays to correct size:
    double xdeltaCut[nx];
    double xmeanCut[nx];
    int filledx = 0;
    for (int jj=0; jj<nx; jj++){
      xdeltaCut[jj] = xdelta[jj];
      xmeanCut[jj] = xmean[jj];
      if (jj!=0){
	if (xmeanCut[jj]>xmeanCut[jj-1] && filledx==0){
	  shiftValue[npl][0] = xdeltaCut[jj-1];
	  filledx++;
	}
      }
    }
    
    //plot the x position vs the reference
    TCanvas *cc = new TCanvas("cc","",800,400);
    cc->Divide(2,1);
    cc->cd(1);

    TGraph *gx = new TGraph(nx,xdeltaCut,xmeanCut);
    gx->SetMarkerStyle(20);
    gx->SetMarkerColor(kRed);
    gx->SetTitle(Form("Tweak x position, plane %d",npl+1));
    gx->GetYaxis()->SetTitle("Sum of | residual means | [cm]");
    gx->GetYaxis()->SetTitleOffset(1.2);
    gx->GetXaxis()->SetTitle("x position tweak [cm]");
    gx->Draw("ap");
    
    //cut down arrays to correct size:
    double ydeltaCut[nx];
    double ymeanCut[nx];
    int filledy = 0;
    for (int jj=0; jj<ny; jj++){
      ydeltaCut[jj] = ydelta[jj];
      ymeanCut[jj] = ymean[jj];
      if (jj!=0){
	if (ymeanCut[jj]>ymeanCut[jj-1] && filledy==0){
	  shiftValue[npl][1] = ydeltaCut[jj-1];
	  filledy++;
	}
      }
    }
    
    //plot the y position vs the reference
    cc->cd(2);
    TGraph *gy = new TGraph(ny,ydeltaCut,ymeanCut);
    gy->SetTitle(Form("Tweak y position, plane %d",npl+1));
    gy->GetYaxis()->SetTitle("Sum of | residual means | [cm]");
    gy->GetYaxis()->SetTitleOffset(1.2);
    gy->GetXaxis()->SetTitle("y position tweak [cm]");
    gy->SetMarkerStyle(20);
    gy->SetMarkerColor(kBlue);
    gy->Draw("ap");
    cc->Update();
    cc->Print((pdf_file_name_A+"(").c_str());
    cc->Close();
  }//end loop through planes

  FILE *foffsets = fopen("offsets.txt","w");
  fprintf(foffsets,"plane\t x-shift[cm]\t y-shift[cm]\n");
  for (int kk=0;kk<nplanes; kk++){
    fprintf(foffsets,Form("%d\t %.4f\t %.4f\n",kk+1,shiftValue[kk][0],shiftValue[kk][1]));
  }
  
  TCanvas *cexit = new TCanvas("cexit","",1);
  cexit->Print((pdf_file_name_A+")").c_str());
  cexit->Print((pdf_file_name_R+")").c_str());
}
