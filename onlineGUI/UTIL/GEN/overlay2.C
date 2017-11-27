/*******************************
 * This macro overlays two histograms.
 * The label1 and label2 are optional
 * and can be used to label the
 * different histograms as they are 
 * drawn.
 *
 * hszumila@jlab.org
 *******************************/

void overlay2(TString hist1name, TString hist2name, TString label1="", TString label2="", TString htitle="", Bool_t golden=false,TString samestr=""){
  TH1F* H1;
  TH1F* H2;
  
  H1 = (TH1F*) gDirectory->Get(hist1name);
  H2 = (TH1F*) gDirectory->Get(hist2name);
  if (H1 && H2) {
  if(golden){
    H1->SetLineColor(30);
    H1->SetFillColor(30);
    H1->SetFillStyle(3027);
    H2->SetLineColor(46);
    H2->SetFillColor(46);
    H2->SetFillStyle(3027);
  } else {
    H1->SetLineColor(4);
    H1->SetLineWidth(2);
    H2->SetLineWidth(2);
    H2->SetLineColor(2);
    H1->SetMinimum(1);
    H2->SetMinimum(1);
    H1->SetStats(0);
    H2->SetStats(0);
    H1->SetTitle(htitle);
    H2->SetTitle(htitle);
    H1->GetXaxis()->SetTitleSize(0.035);
    H1->GetYaxis()->SetTitleSize(0.035);
    H2->GetXaxis()->SetTitleSize(0.035);
    H2->GetYaxis()->SetTitleSize(0.035);
    H1->GetXaxis()->SetLabelSize(0.03);
    H1->GetYaxis()->SetLabelSize(0.03);
    H2->GetXaxis()->SetLabelSize(0.03);
    H2->GetYaxis()->SetLabelSize(0.03);
  }
 
  double xpos = H1->GetXaxis()->GetBinCenter(H1->GetXaxis()->GetFirst()+4);
  double ypos = 0.95*H1->GetMaximum();

  TText* t1 = new TText(xpos,0.4*ypos,label1);
  t1->SetTextColor(4);
  TText* t2 = new TText(xpos,0.5*ypos,label2);
  t2->SetTextColor(2);
  
  if(golden){
    if(!samestr.Contains("sames")) samestr += "sames";
    H1->Draw(samestr);
    H2->Draw(samestr);
  } else {
    if(!golden && !samestr.Contains("sames")){
      // pick which hist to draw first:
      if(H1->GetMaximum() > H2->GetMaximum()){
	H1->Draw(samestr);
	if(!samestr.Contains("sames")) samestr += "sames";
	H2->Draw(samestr);
	t1->Draw();
	t2->Draw();
      } else {
	H2->Draw(samestr);
	if(!samestr.Contains("sames")) samestr += "sames";
	H1->Draw(samestr);
	t1->Draw();
	t2->Draw();
      }
    } else {
      H1->Draw(samestr);
      if(!samestr.Contains("sames")) samestr += "sames";
      H2->Draw(samestr);
      t1->Draw();
      t2->Draw();

    H2->GetXaxis()->SetTitleOffset(.55);
    H2->GetXaxis()->SetTitleSize(0.075);
    H2->GetYaxis()->SetTitleOffset(.75);
    H2->GetYaxis()->SetTitleSize(0.075);
    }
  }
  } else {
    if (!H1) cout << " Histogram " << hist1name << " does not exist" << endl;
    if (!H2) cout << " Histogram " << hist2name << " does not exist" << endl;
  }
}
