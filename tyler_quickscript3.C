void plot_them(TString targ, double angle, vector<int> runs, vector<TString> leg, vector<double> Q, vector<int> PS, int rebin=0){
  int s1=8;
  int s2=7;
  if(angle==8.5){
    s1=6;
    s2=8;
  }
  auto c = new TCanvas();
  c->SetLogy();
  auto l = new TLegend(0.5, 0.7, 0.9, 0.9);
  for(int i=0; i<runs.size(); i++){
    auto f = new TFile(Form("UTIL_XEM/paddle_plots_out/paddle_cut_plots_%d_S1X%d_S2X%d.root", runs[i], s1, s2));
    auto h = (TH1D*) f->Get("hx");
    if(rebin!=0){
      h->Rebin(rebin);
    }
    h->SetTitle(Form("x_{bj} %s %1.1fdeg (PS and Q Scaled)", targ.Data(), angle));
    h->GetYaxis()->SetTitle("Charge and PS normalized Yield");
    h->SetLineColor(i+1);
    h->SetMarkerStyle(24+i);
    h->SetMarkerColor(i+1);
    if(i>=4){
      h->SetLineColor(i+3);
      h->SetMarkerColor(i+3);
    }
    h->Scale(PS[i]/Q[i]);
    if(i==0){
      h->Draw();
    }else{
      h->Draw("same");
    }
    l->AddEntry(h, leg[i], "pl");
  }
  l->Draw();
  if(rebin==0){
    c->Print(Form("UTIL_XEM/paddle_plots_out/%s_%1.1fdeg.png", targ.Data(),angle));
  }else{
    c->Print(Form("UTIL_XEM/paddle_plots_out/%s_%1.1fdeg_rebin%d.png", targ.Data(),angle,rebin));
  }
}

void plot_2Nnorm(TString targ, double angle, vector<int> runs, vector<TString> leg, vector<double> Q, vector<int> PS, int rebin=0){
  int s1=8;
  int s2=7;
  if(angle==8.5){
    s1=6;
    s2=8;
  }
  auto c = new TCanvas();
  c->SetLogy();
  auto l = new TLegend(0.5, 0.7, 0.9, 0.9);
  for(int i=0; i<runs.size(); i++){
    auto f = new TFile(Form("UTIL_XEM/paddle_plots_out/paddle_cut_plots_%d_S1X%d_S2X%d.root", runs[i], s1, s2));
    auto h = (TH1D*) f->Get("hx");
    h->SetTitle(Form("x_{bj} %s %1.1fdeg (2N Scaled)", targ.Data(), angle));
    h->GetYaxis()->SetTitle("Normalized Yield");
    h->SetLineColor(i+1);
    h->SetMarkerStyle(24+i);
    h->SetMarkerColor(i+1);
    if(i>=4){
      h->SetLineColor(i+3);
      h->SetMarkerColor(i+3);
    }
    double count2N = h->Integral(131,200);
    h->Scale(100000/count2N);
    if(rebin!=0){
      h->Rebin(rebin);
    }
    if(i==0){
      h->Draw();
    }else{
      h->Draw("same");
    }
    l->AddEntry(h, leg[i], "pl");
  }
  l->Draw();
  if(rebin==0){
    c->Print(Form("UTIL_XEM/paddle_plots_out/%s_%1.1fdeg_2Nscale.png", targ.Data(),angle));
  }else{
    c->Print(Form("UTIL_XEM/paddle_plots_out/%s_%1.1fdeg_2Nscale_rebin%d.png", targ.Data(),angle,rebin));
  }
}

void plot_2Nnorm_ratio(TString targ, double angle, vector<int> runs, vector<TString> leg, vector<double> Q, vector<int> PS, int rebin=0){
  int s1=8;
  int s2=7;
  if(angle==8.5){
    s1=6;
    s2=8;
  }
  auto c = new TCanvas();
  c->SetLogy();
  auto l = new TLegend(0.5, 0.1, 0.9, 0.3);
  auto f0 = new TFile(Form("UTIL_XEM/paddle_plots_out/paddle_cut_plots_%d_S1X%d_S2X%d.root", runs[0], s1, s2));
  auto h0 = (TH1D*) f0->Get("hx");
  double count2N0 = h0->Integral(131,200);
  h0->Scale(100000/count2N0);
  if(rebin!=0){
    h0->Rebin(rebin);
  }
  h0->SetDirectory(0);
  for(int i=0; i<runs.size(); i++){
    auto f = new TFile(Form("UTIL_XEM/paddle_plots_out/paddle_cut_plots_%d_S1X%d_S2X%d.root", runs[i], s1, s2));
    auto h = (TH1D*) f->Get("hx");
    h->SetTitle(Form("x_{bj} %s %1.1fdeg (2N Scaled ratio to All On)", targ.Data(), angle));
    h->GetYaxis()->SetTitle("Normalized Yield");
    h->SetLineColor(i+1);
    h->SetMarkerStyle(24+i);
    h->SetMarkerColor(i+1);
    if(i>=4){
      h->SetLineColor(i+3);
      h->SetMarkerColor(i+3);
    }
    double count2N = h->Integral(131,200);
    h->Scale(100000/count2N);
    if(rebin!=0){
      h->Rebin(rebin);
    }
    h->Divide(h0);
    if(i==0){
      h->Draw("hist p");
    }else{
      h->Draw("hist p same");
    }
    l->AddEntry(h, leg[i], "pl");
  }
  l->Draw();
  if(rebin==0){
    c->Print(Form("UTIL_XEM/paddle_plots_out/%s_%1.1fdeg_2Nscale_ratio.png", targ.Data(),angle));
  }else{
    c->Print(Form("UTIL_XEM/paddle_plots_out/%s_%1.1fdeg_2Nscale_ratio_rebin%d.png", targ.Data(),angle,rebin));
  }
}

void plot_2Nnorm_ratio_lin(TString targ, double angle, vector<int> runs, vector<TString> leg, vector<double> Q, vector<int> PS, int rebin=0){
  int s1=8;
  int s2=7;
  if(angle==8.5){
    s1=6;
    s2=8;
  }
  auto c = new TCanvas();
  //c->SetLogy();
  auto l = new TLegend(0.5, 0.1, 0.9, 0.3);
  auto f0 = new TFile(Form("UTIL_XEM/paddle_plots_out/paddle_cut_plots_%d_S1X%d_S2X%d.root", runs[0], s1, s2));
  auto h0 = (TH1D*) f0->Get("hx");
  double count2N0 = h0->Integral(131,200);
  h0->Scale(100000/count2N0);
  if(rebin!=0){
    h0->Rebin(rebin);
  }
  h0->SetDirectory(0);
  h0->GetYaxis()->SetRangeUser(0.5,1.1);
  for(int i=0; i<runs.size(); i++){
    auto f = new TFile(Form("UTIL_XEM/paddle_plots_out/paddle_cut_plots_%d_S1X%d_S2X%d.root", runs[i], s1, s2));
    auto h = (TH1D*) f->Get("hx");
    h->SetTitle(Form("x_{bj} %s %1.1fdeg (2N Scaled ratio to All On)", targ.Data(), angle));
    h->GetYaxis()->SetTitle("Normalized Yield");
    h->GetYaxis()->SetRangeUser(0.5,1.1);
    h->SetLineColor(i+1);
    h->SetMarkerStyle(24+i);
    h->SetMarkerColor(i+1);
    if(i==4 || i==5){
      h->SetLineColor(i+3);
      h->SetMarkerColor(i+3);
    }
    double count2N = h->Integral(131,200);
    h->Scale(100000/count2N);
    if(rebin!=0){
      h->Rebin(rebin);
    }
    h->Divide(h0);
    if(i==0){
      h->Draw("hist p");
    }else{
      h->Draw("hist p same");
    }
    l->AddEntry(h, leg[i], "pl");
  }
  l->Draw();
  if(rebin==0){
    c->Print(Form("UTIL_XEM/paddle_plots_out/%s_%1.1fdeg_2Nscale_ratio_lin.png", targ.Data(),angle));
  }else{
    c->Print(Form("UTIL_XEM/paddle_plots_out/%s_%1.1fdeg_2Nscale_ratio_lin_rebin%d.png", targ.Data(),angle,rebin));
  }
}

void tyler_quickscript3(){
  gStyle->SetOptStat(0);
  vector<int> c12_8_runs;
  vector<TString> c12_8_leg;
  vector<double> c12_8_Q;
  vector<int> c12_8_PS;

  c12_8_runs.push_back(17270);
  c12_8_leg.push_back("ALL ON (PS 65) (EL 119kHz)");
  c12_8_Q.push_back(26.92);
  c12_8_PS.push_back(65);

  c12_8_runs.push_back(17279);
  c12_8_leg.push_back("S1X 1-7 off, S2X 1-6 off (PS 33) (EL 47.5kHz)");
  c12_8_Q.push_back(25.981);
  c12_8_PS.push_back(33);

  c12_8_runs.push_back(17288);
  c12_8_leg.push_back("S1X 1-8 off, S2X 1-6 off (PS 33) (EL 47.8kHz)");
  c12_8_Q.push_back(27.676);
  c12_8_PS.push_back(33);

  c12_8_runs.push_back(17290);
  c12_8_leg.push_back("S1X 1-7 off, S2X 1-7 off (PS 17) (EL 32.2kHz)");
  c12_8_Q.push_back(21.538);
  c12_8_PS.push_back(17);

  c12_8_runs.push_back(17297);
  c12_8_leg.push_back("S1X 1-7 off, S2X 1-8 off (PS 33) (EL 25.4kHz) (long beam down)");
  c12_8_Q.push_back(13.9);
  c12_8_PS.push_back(33);

  c12_8_runs.push_back(17298);
  c12_8_leg.push_back("S1X 1-8 off, S2X 1-7 off (PS 33) (EL 35.1kHz)");
  c12_8_Q.push_back(28.383);
  c12_8_PS.push_back(33);

  vector<int> ld2_8_runs;
  vector<TString> ld2_8_leg;
  vector<double> ld2_8_Q;
  vector<int> ld2_8_PS;

  ld2_8_runs.push_back(17268);
  ld2_8_leg.push_back("ALL ON (PS 129) (3/4 instead of ELREAL) (EL 350kHz)");
  ld2_8_Q.push_back(24.308);
  ld2_8_PS.push_back(129);

  ld2_8_runs.push_back(17282);
  ld2_8_leg.push_back("S1X 1-7 off, S2X 1-6 off (PS 65) (EL 156kHz)");
  ld2_8_Q.push_back(28.027);
  ld2_8_PS.push_back(65);

  ld2_8_runs.push_back(17292);
  ld2_8_leg.push_back("S1X 1-7 off, S2X 1-7 off (PS 65) (EL 118kHz)");
  ld2_8_Q.push_back(23.969);
  ld2_8_PS.push_back(65);

  ld2_8_runs.push_back(17293);
  ld2_8_leg.push_back("S1X 1-8 off, S2X 1-7 off (PS 65) (EL 114kHz)");
  ld2_8_Q.push_back(25.081);
  ld2_8_PS.push_back(65);

  ld2_8_runs.push_back(17295);
  ld2_8_leg.push_back("S1X 1-7 off, S2X 1-8 off (PS 33) (EL 97.5kHz)");
  ld2_8_Q.push_back(29.675);
  ld2_8_PS.push_back(33);

  vector<int> c12_85_runs;
  vector<TString> c12_85_leg;
  vector<double> c12_85_Q;
  vector<int> c12_85_PS;

  c12_85_runs.push_back(17301);
  c12_85_leg.push_back("ALL ON (PS 33) (EL 77.7kHz)");
  c12_85_Q.push_back(26.644);
  c12_85_PS.push_back(33);

  c12_85_runs.push_back(17302);
  c12_85_leg.push_back("S1X 1-5 off, S2X 1-7 off (PS 17) (EL 33.7kHz)");
  c12_85_Q.push_back(24.642);
  c12_85_PS.push_back(17);

  c12_85_runs.push_back(17305);
  c12_85_leg.push_back("S1X 1-6 off, S2X 1-8 off (PS 9) (EL 24.8kHz)");
  c12_85_Q.push_back(29.779);
  c12_85_PS.push_back(9);

  c12_85_runs.push_back(17306);
  c12_85_leg.push_back("S1X 1-7 off, S2X 1-8 off (PS 9) (EL 17.3kHz)");
  c12_85_Q.push_back(16.313);
  c12_85_PS.push_back(9);

  vector<int> ld2_85_runs;
  vector<TString> ld2_85_leg;
  vector<double> ld2_85_Q;
  vector<int> ld2_85_PS;

  ld2_85_runs.push_back(17312);
  ld2_85_leg.push_back("ALL ON (PS 129) (EL 240kHz)");
  ld2_85_Q.push_back(30.104);
  ld2_85_PS.push_back(129);

  ld2_85_runs.push_back(17310);
  ld2_85_leg.push_back("S1X 1-5 off, S2X 1-7 off (PS 65) (EL 111kHz)");
  ld2_85_Q.push_back(27.056);
  ld2_85_PS.push_back(65);

  ld2_85_runs.push_back(17309);
  ld2_85_leg.push_back("S1X 1-7 off, S2X 1-8 off (PS 33) (EL 76.3kHz)");
  ld2_85_Q.push_back(23.490);
  ld2_85_PS.push_back(33);

  plot_them("C12", 8, c12_8_runs, c12_8_leg, c12_8_Q, c12_8_PS, 5);
  plot_them("LD2", 8, ld2_8_runs, ld2_8_leg, ld2_8_Q, ld2_8_PS, 5);
  plot_them("C12", 8.5, c12_85_runs, c12_85_leg, c12_85_Q, c12_85_PS, 5);
  plot_them("LD2", 8.5, ld2_85_runs, ld2_85_leg, ld2_85_Q, ld2_85_PS, 5);

  plot_2Nnorm("C12", 8, c12_8_runs, c12_8_leg, c12_8_Q, c12_8_PS, 5);
  plot_2Nnorm("LD2", 8, ld2_8_runs, ld2_8_leg, ld2_8_Q, ld2_8_PS, 5);
  plot_2Nnorm("C12", 8.5, c12_85_runs, c12_85_leg, c12_85_Q, c12_85_PS, 5);
  plot_2Nnorm("LD2", 8.5, ld2_85_runs, ld2_85_leg, ld2_85_Q, ld2_85_PS, 5);

  plot_2Nnorm_ratio("C12", 8, c12_8_runs, c12_8_leg, c12_8_Q, c12_8_PS, 5);
  plot_2Nnorm_ratio("LD2", 8, ld2_8_runs, ld2_8_leg, ld2_8_Q, ld2_8_PS, 5);
  plot_2Nnorm_ratio("C12", 8.5, c12_85_runs, c12_85_leg, c12_85_Q, c12_85_PS, 5);
  plot_2Nnorm_ratio("LD2", 8.5, ld2_85_runs, ld2_85_leg, ld2_85_Q, ld2_85_PS, 5);

  plot_2Nnorm_ratio_lin("C12", 8, c12_8_runs, c12_8_leg, c12_8_Q, c12_8_PS, 5);
  plot_2Nnorm_ratio_lin("LD2", 8, ld2_8_runs, ld2_8_leg, ld2_8_Q, ld2_8_PS, 5);
  plot_2Nnorm_ratio_lin("C12", 8.5, c12_85_runs, c12_85_leg, c12_85_Q, c12_85_PS, 5);
  plot_2Nnorm_ratio_lin("LD2", 8.5, ld2_85_runs, ld2_85_leg, ld2_85_Q, ld2_85_PS, 5);
}

