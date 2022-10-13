void tyler_quickscript2(){
  vector<int> deg8;
  deg8.push_back(17255);
  deg8.push_back(17256);
  deg8.push_back(17257);
  deg8.push_back(17259);
  deg8.push_back(17268);
  deg8.push_back(17269);
  deg8.push_back(17270);
  deg8.push_back(17277);
  deg8.push_back(17279);
  deg8.push_back(17282);
  deg8.push_back(17287);
  deg8.push_back(17288);
  deg8.push_back(17289);
  deg8.push_back(17290);
  deg8.push_back(17292);
  deg8.push_back(17293);
  deg8.push_back(17294);
  deg8.push_back(17295);
  deg8.push_back(17296);
  deg8.push_back(17297);
  deg8.push_back(17298);
  deg8.push_back(17299);

  vector<int> deg85;
  deg85.push_back(17300);
  deg85.push_back(17301);
  deg85.push_back(17302);
  deg85.push_back(17303);
  deg85.push_back(17304);
  deg85.push_back(17305);
  deg85.push_back(17306);
  deg85.push_back(17307);
  deg85.push_back(17308);
  deg85.push_back(17309);
  deg85.push_back(17310);
  deg85.push_back(17311);
  deg85.push_back(17312);

  auto c = new TCanvas();
  for(int i=0; i<deg8.size(); i++){
    auto f = new TFile(Form("UTIL_XEM/paddle_plots_out/paddle_cut_plots_%d_S1X8_S2X7.root", deg8[i]));
    auto h = (TH1D*) f->Get("hS1Xgoodhits");
    h->Draw();
    c->Print(Form("UTIL_XEM/paddle_plots_out/%dS1X.png",deg8[i]));
    auto h2 = (TH1D*) f->Get("hS2Xgoodhits");
    h2->Draw();
    c->Print(Form("UTIL_XEM/paddle_plots_out/%dS2X.png",deg8[i]));
  }
  for(int i=0; i<deg85.size(); i++){
    auto f = new TFile(Form("UTIL_XEM/paddle_plots_out/paddle_cut_plots_%d_S1X6_S2X8.root", deg85[i]));
    auto h = (TH1D*) f->Get("hS1Xgoodhits");
    h->Draw();
    c->Print(Form("UTIL_XEM/paddle_plots_out/%dS1X.png",deg85[i]));
    auto h2 = (TH1D*) f->Get("hS2Xgoodhits");
    h2->Draw();
    c->Print(Form("UTIL_XEM/paddle_plots_out/%dS2X.png",deg85[i]));
  }
}
  
