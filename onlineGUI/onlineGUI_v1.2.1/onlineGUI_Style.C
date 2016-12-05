void onlineGUI_Style() {
  // These options are documented in the ROOT Reference Guide
  //  under the TStyle Class:
  // http://root.cern.ch/root/Reference.html

  gStyle->SetOptFit(0);
  gStyle->SetOptStat(1110);
  gStyle->SetPalette(1);
  gStyle->SetNdivisions(505);
  
  gStyle->SetCanvasColor(10);
  gStyle->SetPadTopMargin(.01);
  gStyle->SetPadLeftMargin(.15);
  gStyle->SetPadRightMargin(.05);
  gStyle->SetPadBottomMargin(.10);
  gStyle->SetTitleXOffset(0.8);
  gStyle->SetTitleYOffset(0.8);
  gStyle->SetLabelFont(42,"X");
  gStyle->SetLabelFont(42,"Y");

  gStyle->SetStatH(0.4);

  gStyle->SetPadTickX(1);
  gStyle->SetPadTickY(1);

  // prepare gStyle to be useful
  //   1 = solid
  //   2 = long dash (30 10)
  //   3 = dotted (4 8)
  //   4 = dash-dot (15 12 4 12)
  //   5 = short dash ( 15 15 )
  //   6 = dash-dot-dot 
  gStyle->SetLineStyleString(1,"[]");
  gStyle->SetLineStyleString(2,"[30 10]");
  gStyle->SetLineStyleString(3,"[4 8]");
  gStyle->SetLineStyleString(4,"[15 12 4 12]");
  gStyle->SetLineStyleString(5,"[15 15]");
  gStyle->SetLineStyleString(6,"[15 12 4 12 4 12]");
  gStyle->SetLabelSize(0.08,"X");
  gStyle->SetLabelSize(0.08,"Y");
  gStyle->SetNdivisions(505,"Y");
  gStyle->SetOptDate(0);
  gStyle->SetDateY(.98);
  gStyle->SetStripDecimals(kFALSE);

  gStyle->SetTitleSize(0.08,"T");

  gStyle->SetStatW(0.45);
}
