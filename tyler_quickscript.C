#include "UTIL_XEM/paddle_cut_plots.C"

void tyler_quickscript(){
  vector<int> deg8;
  //deg8.push_back(17255);
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

  for(int i=0; i<deg8.size(); i++){
    paddle_cut_plots(deg8[i],8,7);
  }
  for(int i=0; i<deg85.size(); i++){
    paddle_cut_plots(deg85[i],6,8);
  }
}
  
