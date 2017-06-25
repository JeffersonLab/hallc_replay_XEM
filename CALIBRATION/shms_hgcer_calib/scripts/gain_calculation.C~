#include <sstream>
#include <TGraphErrors.h>

Double_t exponential(Double_t *x, Double_t *par)
{
  Double_t result = par[0]*pow(x[0],par[1]);
  return result;
}

void gain_calculation()
{
  struct input_t
  {
    Int_t voltage;
    Int_t index;
    float constant;
  };
  
  input_t input;

  string datafile;
  datafile = "/home/rambrose/hallc_replay/CALIBRATION/shms_hgcer_calib/Final Scripts/Constants.dat";

  FILE *fp = fopen(datafile.c_str(),"r");
  Double_t const_err = 5;
  //Double_t pC = 0.006;
  Double_t pC = 0.01953125;

  char line[50];
  TGraphErrors *gr_1 = new TGraphErrors();
  TGraphErrors *gr_2 = new TGraphErrors();
  TGraphErrors *gr_3 = new TGraphErrors();
  TGraphErrors *gr_4 = new TGraphErrors();
  
  while (fgets(&line[0],50,fp))
    {
      if (line[0] == '#') continue;
      else
	{
	  sscanf(&line[0],"%i%i%f",&input.voltage,&input.index,&input.constant);
	  if (input.index == 1)
	    {
	      gr_1->SetPoint(gr_1->GetN(),input.voltage,(input.constant * pC)/(0.0000001602));
	      gr_1->SetPointError(gr_1->GetN()-1,0,(const_err * pC)/(0.0000001602));
	    }
	  if (input.index == 2)
	    {
	      gr_2->SetPoint(gr_2->GetN(),input.voltage,(input.constant * pC)/(0.0000001602));
	      gr_2->SetPointError(gr_2->GetN()-1,0,(const_err * pC)/(0.0000001602));
	    }
	  if (input.index == 3)
	    {
	      gr_3->SetPoint(gr_3->GetN(),input.voltage,(input.constant * pC)/(0.0000001602));
	      gr_3->SetPointError(gr_3->GetN()-1,0,(const_err * pC)/(0.0000001602));
	    }
	  if (input.index == 4)
	    {
	      gr_4->SetPoint(gr_4->GetN(),input.voltage,(input.constant * pC)/(0.0000001602));
	      gr_4->SetPointError(gr_4->GetN()-1,0,(const_err * pC)/(0.0000001602));
	    }
	}  
    }
  fclose(fp);

  TF1 *e1 = new TF1("e1",exponential,2000,2400,2);
  e1->SetParNames("Coefficient","Exponent");
  e1->SetParameters(1*pow(10,-28), 10.5);
  TF1 *e2 = new TF1("e2",exponential,2000,2400,2);
  e2->SetParNames("Coefficient","Exponent");
  e2->SetParameters(1*pow(10,-28), 10.5);
  TF1 *e3 = new TF1("e3",exponential,2000,2400,2);
  e3->SetParNames("Coefficient","Exponent");
  e3->SetParameters(1*pow(10,-28), 10.5);
  TF1 *e4 = new TF1("e4",exponential,2000,2400,2);
  e4->SetParNames("Coefficient","Exponent");
  e4->SetParameters(1*pow(10,-28), 10.5);

  TCanvas *c0 = new TCanvas("c0","Gain vs. Voltage");
  gr_1->SetName("gr_1");
  gr_1->SetMarkerColor(kMagenta);
  gr_1->SetMarkerStyle(22);
  gr_1->SetMarkerSize(1);
  e1->SetLineColor(kMagenta);
  gr_1->Fit("e1","RMEQ");
  //gr_1->Draw("AP");
  gr_2->SetName("gr_2");
  gr_2->SetMarkerColor(kBlue);
  gr_2->SetMarkerStyle(22);
  gr_2->SetMarkerSize(1);
  e2->SetLineColor(kBlue);
  gr_2->Fit("e2","RMEQ");
  //gr_2->Draw("APsame");
  gr_3->SetName("gr_3");
  gr_3->SetMarkerColor(kCyan);
  gr_3->SetMarkerStyle(22);
  gr_3->SetMarkerSize(1);
  e3->SetLineColor(kCyan);
  gr_3->Fit("e3","RMEQ");
  //gr_3->Draw("APsame");
  gr_4->SetName("gr_4");
  gr_4->SetMarkerColor(kGreen);
  gr_4->SetMarkerStyle(22);
  gr_4->SetMarkerSize(1);
  e4->SetLineColor(kGreen);
  gr_4->Fit("e4","RMEQ");
  //gr_4->Draw("same");
  TMultiGraph *mg = new TMultiGraph();
  mg->SetTitle("Voltage vs Gain for HGC PMTs");
  mg->Add(gr_1);
  mg->Add(gr_2);
  mg->Add(gr_3);
  mg->Add(gr_4);
  mg->Draw("AP");
  mg->GetXaxis()->SetTitle("Voltage");
  mg->GetYaxis()->SetTitle("Gain");
  auto legend = new TLegend(0.6,0.7,0.8,0.9);
  legend->AddEntry("gr_1","PMT 1","ep");
  legend->AddEntry("gr_2","PMT 2","ep");
  legend->AddEntry("gr_3","PMT 3","ep");
  legend->AddEntry("gr_4","PMT 4","ep");
  legend->Draw();
  gPad->Modified();
  
  cout << "Calibration values are:  (Constant & Exponent)" << endl;
  cout << "PMT1: " << e1->GetParameter(0) << "   " << e1->GetParameter(1) << " +/- " << e1->GetParError(1) << endl;
  cout << "PMT2: " << e2->GetParameter(0) << "   " << e2->GetParameter(1) << " +/- " << e2->GetParError(1) << endl;
  cout << "PMT3: " << e3->GetParameter(0) << "   " << e3->GetParameter(1) << " +/- " << e3->GetParError(1) <<endl;
  cout << "PMT4: " << e4->GetParameter(0) << "   " << e4->GetParameter(1) << " +/- " << e4->GetParError(1) <<endl;


}
