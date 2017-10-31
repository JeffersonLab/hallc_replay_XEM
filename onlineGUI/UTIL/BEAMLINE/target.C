void target(TString histoname)
{

TFile *f = new TFile("../ROOTfiles/shms_replay_production_452_1000000.root");

  const Int_t n = 3;
  
  Int_t i;
  Int_t j;

  Double_t z[n] = {3708.2, 2249.6, 1293}; //positions from target in mm

  TTree *E = (TTree*)gDirectory->Get("E");
  auto nevents = E->GetEntries();

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

 Double_t theta_xz[nevents]; // angle between x and z at target
 Double_t theta_yz[nevents]; // angle between y and z at target

 // offsets from Gaskell's survey values
  const Double_t AXSOF = -2.84;
  const Double_t AYSOF = -0.21;
  const Double_t BXSOF = -2.88;
  const Double_t BYSOF = -0.20;
  const Double_t CXSOF = -3.26;
  const Double_t CYSOF = -0.045;

 //offsets of BPM 07B calculated from line drawn between RAW-SOF BPM A and C points from run 452 with 500000 events
 const Double_t BXos = 0.929; 
 const Double_t BYos = -0.588;

//get values of x,y position for each BPM per event

 for(i=0; i<nevents; i++){

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

  TF1 *line = new TF1("line", "[0]*x+[1]",-10.0, 10.0);
  line->SetParameters(10000,10000);

   for(i=0; i<nevents; i++){
     xE[i] = new Double_t[n];
     yE[i] = new Double_t[n];
  }

   for(i=0; i<nevents; i++){
     for(j=0; j<1; j++){ 
      xE[i][j] = AX[i]; //BPM A x positions per event to feed into TGraph array
      yE[i][j] = AY[i]; //BPM A y positions per event to feed into TGraph array
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

    grX[i] = new TGraph(n,xE[i],z);
    grX[i]->Fit("line");
    
    slopeX[i] = line->GetParameter(0);
    intX[i] = line->GetParameter(1);
    
    BeamX[i] = (-1)*(intX[i]/slopeX[i]); //x position of the beam at the target

    theta_xz[i] = TMath::Tan((BeamX[i])/z[0]); //x angle of beam relative to z axis

  }

TGraph *grY[nevents];

  for(i=0; i<nevents; i++){

    grY[i] = new TGraph(n,yE[i],z);
    
    grY[i]->Fit("line");
    
    slopeY[i] = line->GetParameter(0);
    intY[i] = line->GetParameter(1);
    
    BeamY[i] = (-1)*(intY[i]/slopeY[i]); //y position of the beam at the target

    theta_yz[i] = TMath::Tan((BeamY[i])/z[0]); //y angle of beam relative to z axis

  }

 
  TH1D *BeamXt = new TH1D("BeamXt","Beam X Position at Target",100, -12.0, 12.0);
  TH1D *BeamYt = new TH1D("BeamYt","Beam Y Position at Target",100, -12.0, 12.0);

  TH1D *BeamXAt = new TH1D("BeamXAt","Beam X Angle at Target",100, -12.0, 12.0);
  TH1D *BeamYAt = new TH1D("BeamYAt","Beam Y Angle at Target",100, -12.0, 12.0);

  TH1D* h1;

  if(histoname.Contains("Xt")){
    h1 = (TH1D*) gDirectory->Get("BeamXt");
    for (i=0; i<nevents; i++) {
      h1->Fill(BeamX[i]);
   }
   h1->GetXaxis()->SetTitle("Beam X Position (mm)");
   h1->Draw();
 }

  if(histoname.Contains("Yt")){
    h1 = (TH1D*) gDirectory->Get("BeamYt");
    for (i=0; i<nevents; i++) {
      h1->Fill(BeamY[i]);
   }
   h1->GetXaxis()->SetTitle("Beam Y Position (mm)");
   h1->Draw();
 }

 if(histoname.Contains("XAt")){
    h1 = (TH1D*) gDirectory->Get("BeamXAt");
    for (i=0; i<nevents; i++) {
      theta_xz[nevents];
      h1->Fill(theta_xz[i]*1000); //convert to mrad
   }
   h1->GetXaxis()->SetTitle("Beam X Angle (mrad)");
   h1->Draw();
 }

  if(histoname.Contains("YAt")){
    h1 = (TH1D*) gDirectory->Get("BeamYAt");
    for (i=0; i<nevents; i++) {
      theta_xz[nevents];
      h1->Fill(theta_yz[i]*1000); //convert to mrad
   }
   h1->GetXaxis()->SetTitle("Beam Y Angle (mrad)");
   h1->Draw();
 }


}

