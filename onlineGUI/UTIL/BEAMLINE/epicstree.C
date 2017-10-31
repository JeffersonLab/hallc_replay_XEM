void epicsTree(TString leafname)
{

  TFile *f = new TFile("../ROOTfiles/shms_replay_production_452_1000000.root");
  TTree *E = (TTree*)gDirectory->Get("E");

  E->Draw(leafname);

}
