void plot_epics(TString graphname)

{

  TFile *f = new TFile("../ROOTfiles/shms_replay_production_452_1000000.root");
  
  TTree *E = (TTree*)gDirectory->Get("E");
  auto nevents = E->GetEntries();

  TH1D* hAX;
  hAX = new TH1D("hAX","BPMA X",100, -5.0,0.0);

  TH1D* hAY;
  hAY = new TH1D("hAY","BPMA Y",100, 0.0,6.0);

  TH1D* hBX;
  hBX = new TH1D("hBX","BPMB X",100, -5.0,0.0);

  TH1D* hBY;
  hBY = new TH1D("hBY","BPMB Y",100, 0.0,6.0);

  TH1D* hCX;
  hCX = new TH1D("hCX","BPMC X",100, -5.0,0.0);

  TH1D* hCY;
  hCY = new TH1D("hCY","BPMC Y",100, 0.0,6.0);

 // offsets from Gaskell's survey values
  const Double_t AXSOF = -2.84;
  const Double_t AYSOF = -0.21;
  const Double_t BXSOF = -2.88;
  const Double_t BYSOF = -0.20;
  const Double_t CXSOF = -3.26;
  const Double_t CYSOF = -0.045;

 // extra offsets of BPM 07B calculated from line drawn between RAW-SOF BPM A and C points from run 452 with 500000 events
  const Double_t BXos = 0.929; 
  const Double_t BYos = -0.588;

//get mean values of x,y position for each BPM for the run
  E->Draw("IPM3H07A.XRAW>>hAX");
  Double_t meanAX = hAX->GetMean() - AXSOF;
  cout << "Mean A XPOS = " << meanAX << endl;

  E->Draw("IPM3H07A.YRAW>>hAY");
  Double_t meanAY = hAY->GetMean() - AYSOF;
  cout << "Mean A YPOS = " << meanAY << endl;

  E->Draw("IPM3H07B.XRAW>>hBX");
  Double_t meanBX = hBX->GetMean() - BXSOF - BXos;
  cout << "Mean B XPOS = " << meanBX << endl;

  E->Draw("IPM3H07B.YRAW>>hBY");
  Double_t meanBY = hBY->GetMean() - BYSOF - BYos;
  cout << "Mean B YPOS = " << meanBY << endl;

  E->Draw("IPM3H07C.XRAW>>hCX");
  Double_t meanCX = hCX->GetMean() - CXSOF;
  cout << "Mean C XPOS = " << meanCX << endl;

  E->Draw("IPM3H07C.YRAW>>hCY");
  Double_t meanCY = hCY->GetMean() - CYSOF;
  cout << "Mean C YPOS = " << meanCY << endl;

  Double_t BPMX[3] = {meanAX, meanBX, meanCX};
  Double_t BPMY[3] = {meanAY, meanBY, meanCY};
  Double_t BPMZ[3] = {3708.2, 2249.6, 1293};

  TF1 *line = new TF1("line", "[0]*x+[1]",-10.0, 10.0);
  line->SetParameters(10000,1000);
  
  TGraph *XZmean;
  XZmean = new TGraph(3,BPMX,BPMZ);
  XZmean->SetName("XZmean");
  gDirectory->GetList()->Add(XZmean); //need to add TGraph to current directory
 
  TGraph *YZmean;
  YZmean = new TGraph(3,BPMY,BPMZ);
  YZmean->SetName("YZmean");
  gDirectory->GetList()->Add(YZmean);
 
  TGraph* grmean;

   if(graphname == "XZmean"){
    grmean = (TGraph*) gDirectory->Get("XZmean");

    grmean->GetXaxis()->SetTitle("X Position (mm)");
    grmean->GetYaxis()->SetTitle("Z Position (mm)");

    grmean->SetTitle("BPM Mean X for Run");
    grmean->GetXaxis()->SetTitleOffset(1.25);
    grmean->GetYaxis()->SetTitleOffset(1.50);

    grmean->Draw("AP*");
    grmean->Fit("line");

    Double_t slopeX = line->GetParameter(0);
    Double_t intX = line->GetParameter(1);

    Double_t meanBeamX = (-1)*intX/slopeX;

    cout << "The mean beam X position is: " << meanBeamX << endl;

  }

   if(graphname == "YZmean"){
    grmean = (TGraph*) gDirectory->Get("YZmean");

    grmean->GetXaxis()->SetTitle("Y Position (mm)");
    grmean->GetYaxis()->SetTitle("Z Position (mm)");

    grmean->SetTitle("BPM Mean Y for Run");
    grmean->GetXaxis()->SetTitleOffset(1.25);
    grmean->GetYaxis()->SetTitleOffset(1.50);

    grmean->Draw("AP*");
    grmean->Fit("line");

    Double_t slopeY = line->GetParameter(0);
    Double_t intY = line->GetParameter(1);

    Double_t meanBeamY = (-1)*intY/slopeY;

    cout << "The mean beam Y position is: " << meanBeamY << endl;

  }

  Double_t AX[nevents];
  Double_t AY[nevents];
  Double_t BX[nevents];
  Double_t BY[nevents];
  Double_t CX[nevents];
  Double_t CY[nevents];

  Double_t *xE[nevents];
  Double_t *yE[nevents];

  Double_t slopeX[nevents];
  Double_t slopeY[nevents];

  Double_t intX[nevents];
  Double_t intY[nevents];

  Double_t BeamX[nevents];
  Double_t BeamY[nevents];

  //get values of x,y position for each BPM per event

  for(Int_t i=0; i<nevents; i++){

   E->GetEntry(i);

   AX[i] = E->GetLeaf("IPM3H07A.XRAW")->GetValue(0) - AXSOF;
   cout << "A XPOS Event " << i << " = " << AX[i] << endl;

   AY[i] = E->GetLeaf("IPM3H07A.YRAW")->GetValue(0) - AYSOF;
   cout << "A YPOS Event " << i << " = " << AY[i] << endl;

   BX[i] = E->GetLeaf("IPM3H07B.XRAW")->GetValue(0) - BXSOF - BXos;
   cout << "B XPOS Event " << i << " = " << BX[i] << endl;

   BY[i] = E->GetLeaf("IPM3H07B.YRAW")->GetValue(0) - BYSOF - BYos;
   cout << "B YPOS Event " << i << " = " << BY[i] << endl;

   CX[i] = E->GetLeaf("IPM3H07C.XRAW")->GetValue(0) - CXSOF;
   cout << "C XPOS Event " << i << " = " << CX[i] << endl;

   CY[i] = E->GetLeaf("IPM3H07C.YRAW")->GetValue(0) - CYSOF;
   cout << "C YPOS Event " << i << " = " << CY[i] << endl;

  }
 
  Int_t i;
  Int_t j;
  Int_t n = 3;

  for(i=0; i<nevents; i++){
     xE[i] = new Double_t[n];
     yE[i] = new Double_t[n];
  }

   for(i=0; i<nevents; i++){
     for(j=0; j<1; j++){ 
      xE[i][j] = AX[i];
      yE[i][j] = AY[i];
     }
     for(j=1; j<2; j++){
      xE[i][j] = BX[i];
      yE[i][j] = BY[i];
     }
     for(j=2; j<3; j++){
      xE[i][j] = CX[i];
      yE[i][j] = CY[i];
     }
   }

   TGraph *grX[nevents];

  for(i=0; i<nevents; i++){

    grX[i] = new TGraph(n,xE[i],BPMZ);
    grX[i]->SetName(TString::Format("grXZe%d",i));
    gDirectory->GetList()->Add(grX[i]);
  }

   TGraph *grY[nevents];

  for(i=0; i<nevents; i++){

    grY[i] = new TGraph(n,yE[i],BPMZ);
    grY[i]->SetName(TString::Format("grYZe%d",i));
    gDirectory->GetList()->Add(grY[i]);
  }

  for (i=0; i<nevents; i++){

    if(graphname == (TString::Format("grXZe%d",i))){
      grmean = (TGraph*) gDirectory->Get(TString::Format("grXZe%d",i));

      grmean->GetXaxis()->SetTitle("X Position (mm)");
      grmean->GetYaxis()->SetTitle("Z Position (mm)");

      grmean->SetTitle(TString::Format("BPM X Pos Event %d",i));
      grmean->GetXaxis()->SetTitleOffset(1.25);
      grmean->GetYaxis()->SetTitleOffset(1.50);

      grmean->Draw("AP*");
      grmean->Fit("line");
    }

     if(graphname == (TString::Format("grYZe%d",i))){
      grmean = (TGraph*) gDirectory->Get(TString::Format("grYZe%d",i));

      grmean->GetXaxis()->SetTitle("Y Position (mm)");
      grmean->GetYaxis()->SetTitle("Z Position (mm)");

      grmean->SetTitle(TString::Format("BPM Y Pos Event %d",i));
      grmean->GetXaxis()->SetTitleOffset(1.25);
      grmean->GetYaxis()->SetTitleOffset(1.50);

      grmean->Draw("AP*");
      grmean->Fit("line");
    }

  }

}

