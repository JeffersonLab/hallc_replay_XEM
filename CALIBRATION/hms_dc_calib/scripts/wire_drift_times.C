#define wire_drift_times_cxx
#include "wire_drift_times.h"
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>
#define NPLANES 12

void wire_drift_times::Loop()
{
//   In a ROOT session, you can do:
//      Root > .L wire_drift_times.C
//      Root > wire_drift_times t
//      Root > t.GetEntry(12); // Fill t data members with entry number 12
//      Root > t.Show();       // Show values of entry 12
//      Root > t.Show(16);     // Read and show values of entry 16
//      Root > t.Loop();       // Loop on all entries
//

//     This is the loop skeleton where:
//    jentry is the global entry number in the chain
//    ientry is the entry number in the current Tree
//  Note that the argument to GetEntry must be:
//    jentry for TChain::GetEntry
//    ientry for TTree::GetEntry and TBranch::GetEntry
//
//       To read only selected branches, Insert statements like:
// METHOD1:
//    fChain->SetBranchStatus("*",0);  // disable all branches
//    fChain->SetBranchStatus("branchname",1);  // activate branchname
// METHOD2: replace line
//    fChain->GetEntry(jentry);       //read all branches
//by  b_branchname->GetEntry(ientry); //read only this branch
   if (fChain == 0) return;

   Long64_t nentries = fChain->GetEntriesFast();

//Read Run Number from txt file
int run_NUM;
TString f0 = "input_RUN.txt";
ifstream infile(f0);
infile >> run_NUM;

 TString run = Form("run%d", run_NUM);
//Declare plane names to loop over
TString plane_names[NPLANES]={"1x1", "1y1", "1u1", "1v1", "1y2", "1x2", "2x1", "2y1", "2u1", "2v1", "2y2", "2x2"};

//Declare a root file array to store individual DC cell drift times
TString root_file[NPLANES];
TFile *g[NPLANES];
   
int total_wires;  //integer to store total sense wires for a plane chosen by the user
     
   Long64_t nbytes = 0, nb = 0;
   
//Loop over all planes
for (int ip = 0; ip < NPLANES; ip++){

//Initialize a root file array to store individual DC cell drift times
root_file[ip] = "../root_files/" + run + "/hms_DC_"+plane_names[ip]+Form("_%d.root", run_NUM);
g[ip] = new TFile(root_file[ip], "RECREATE");
g[ip]->cd();

/*========================PLANES 1X1,1X2,2X1,2X2=====================================*/
	
	//If specific planes are encountered, treat them as follows:
	
	if(ip==0 || ip==5 || ip==6 || ip==11) {

	total_wires = 113;
	TH1F *cell_dt[113];    
    TH2F *wire_vs_dt = new TH2F("wire_vs_dt", "", 200., -50., 350., 113., 0.,113.);
    
    //Initialize wire drift time histograms
    for (int wirenum=1; wirenum<=total_wires; wirenum++){
	cell_dt[wirenum-1] = new TH1F(Form("wire_%d", wirenum), "", 200., -50., 350.);
     }
	
       //Loop over all entries (triggers or events)   
    for (Long64_t jentry=0; jentry<nentries; jentry++) {
      Long64_t ientry = LoadTree(jentry);
      if (ientry < 0) break;
      nb = fChain->GetEntry(jentry);   nbytes += nb;
      // if (Cut(ientry) < 0) continue;
 
 if (ip==0) {
    for (int i=0; i< Ndata_H_dc_1x1_wirenum; i++){
       wirenum = int(H_dc_1x1_wirenum[i]);
      //cout << " wire num: " << H_dc_1x1_wirenum[i] << endl;
	  //cout << "Time: " << H_dc_1x1_time[i] << endl;

      //Fill the Histograms
     cell_dt[wirenum-1]->Fill(H_dc_1x1_time[i]);
     wire_vs_dt->Fill(H_dc_1x1_time[i], H_dc_1x1_wirenum[i]);

				}
			}
							
	 if (ip==5) {
    for (int i=0; i< Ndata_H_dc_1x2_wirenum; i++){
       wirenum = int(H_dc_1x2_wirenum[i]);
      //cout << " wire num: " << H_dc_1x2_wirenum[i] << endl;
	  //cout << "Time: " << H_dc_1x2_time[i] << endl;

      //Fill the Histograms
     cell_dt[wirenum-1]->Fill(H_dc_1x2_time[i]);
     wire_vs_dt->Fill(H_dc_1x2_time[i], H_dc_1x2_wirenum[i]);

				}
			}						
			
		 if (ip==6) {
    for (int i=0; i< Ndata_H_dc_2x1_wirenum; i++){
       wirenum = int(H_dc_2x1_wirenum[i]);
      //cout << " wire num: " << H_dc_2x1_wirenum[i] << endl;
	  //cout << "Time: " << H_dc_2x1_time[i] << endl;

      //Fill the Histograms
     cell_dt[wirenum-1]->Fill(H_dc_2x1_time[i]);
     wire_vs_dt->Fill(H_dc_2x1_time[i], H_dc_2x1_wirenum[i]);

				}
			}		
			
		if (ip==11) {
    for (int i=0; i< Ndata_H_dc_2x2_wirenum; i++){
       wirenum = int(H_dc_2x2_wirenum[i]);
      //cout << " wire num: " << H_dc_2x2_wirenum[i] << endl;
	  //cout << "Time: " << H_dc_2x2_time[i] << endl;

      //Fill the Histograms
     cell_dt[wirenum-1]->Fill(H_dc_2x2_time[i]);
     wire_vs_dt->Fill(H_dc_2x2_time[i], H_dc_2x2_wirenum[i]);

				}
			}						
					
					
					
					}
					}
					
	/*PLANE 1U1, 1V1, 2U1, 2V1*/				
	//If specific planes are encountered, treat them as follows:
	if(ip==2 || ip==3 || ip==8 || ip==9) {

	total_wires = 107;
	TH1F *cell_dt[107];    
    TH2F *wire_vs_dt = new TH2F("wire_vs_dt", "", 200., -50., 350., 107., 0.,107.);
    
    //Initialize wire drift time histograms
    for (int wirenum=1; wirenum<=total_wires; wirenum++){
	cell_dt[wirenum-1] = new TH1F(Form("wire_%d", wirenum), "", 200., -50., 350.);
     }
	
       //Loop over all entries (triggers or events)   
    for (Long64_t jentry=0; jentry<nentries; jentry++) {
      Long64_t ientry = LoadTree(jentry);
      if (ientry < 0) break;
      nb = fChain->GetEntry(jentry);   nbytes += nb;
      // if (Cut(ientry) < 0) continue;
 
 if (ip==2) {
    for (int i=0; i< Ndata_H_dc_1u1_wirenum; i++){
       wirenum = int(H_dc_1u1_wirenum[i]);
      //cout << " wire num: " << H_dc_1u1_wirenum[i] << endl;
	  //cout << "Time: " << H_dc_1u1_time[i] << endl;

      //Fill the Histograms
     cell_dt[wirenum-1]->Fill(H_dc_1u1_time[i]);
     wire_vs_dt->Fill(H_dc_1u1_time[i], H_dc_1u1_wirenum[i]);

				}
			}
							
	 if (ip==3) {
    for (int i=0; i< Ndata_H_dc_1v1_wirenum; i++){
       wirenum = int(H_dc_1v1_wirenum[i]);
      //cout << " wire num: " << H_dc_1v1_wirenum[i] << endl;
	  //cout << "Time: " << H_dc_1v1_time[i] << endl;

      //Fill the Histograms
     cell_dt[wirenum-1]->Fill(H_dc_1v1_time[i]);
     wire_vs_dt->Fill(H_dc_1v1_time[i], H_dc_1v1_wirenum[i]);

				}
			}						
			
		 if (ip==8) {
    for (int i=0; i< Ndata_H_dc_2u1_wirenum; i++){
       wirenum = int(H_dc_2u1_wirenum[i]);
      //cout << " wire num: " << H_dc_2u1_wirenum[i] << endl;
	  //cout << "Time: " << H_dc_2u1_time[i] << endl;

      //Fill the Histograms
     cell_dt[wirenum-1]->Fill(H_dc_2u1_time[i]);
     wire_vs_dt->Fill(H_dc_2u1_time[i], H_dc_2u1_wirenum[i]);

				}
			}		
			
		if (ip==9) {
    for (int i=0; i< Ndata_H_dc_2v1_wirenum; i++){
       wirenum = int(H_dc_2v1_wirenum[i]);
      //cout << " wire num: " << H_dc_2v1_wirenum[i] << endl;
	  //cout << "Time: " << H_dc_2v1_time[i] << endl;

      //Fill the Histograms
     cell_dt[wirenum-1]->Fill(H_dc_2v1_time[i]);
     wire_vs_dt->Fill(H_dc_2v1_time[i], H_dc_2v1_wirenum[i]);

				}
			}						
					
					
					
					}
					}

	/*PLANE 1Y1, 1Y2, 2Y1, 2Y2*/				
	//If specific planes are encountered, treat them as follows:
	if(ip==1 || ip==4 || ip==7 || ip==10) {

	total_wires = 52;
	TH1F *cell_dt[52];    
    TH2F *wire_vs_dt = new TH2F("wire_vs_dt", "", 200., -50., 350., 52., 0.,52.);
    
    //Initialize wire drift time histograms
    for (int wirenum=1; wirenum<=total_wires; wirenum++){
	cell_dt[wirenum-1] = new TH1F(Form("wire_%d", wirenum), "", 200., -50., 350.);
     }
	
       //Loop over all entries (triggers or events)   
    for (Long64_t jentry=0; jentry<nentries; jentry++) {
      Long64_t ientry = LoadTree(jentry);
      if (ientry < 0) break;
      nb = fChain->GetEntry(jentry);   nbytes += nb;
      // if (Cut(ientry) < 0) continue;
 
 if (ip==1) {
    for (int i=0; i< Ndata_H_dc_1y1_wirenum; i++){
       wirenum = int(H_dc_1y1_wirenum[i]);
      //cout << " wire num: " << H_dc_1y1_wirenum[i] << endl;
	  //cout << "Time: " << H_dc_1y1_time[i] << endl;

      //Fill the Histograms
     cell_dt[wirenum-1]->Fill(H_dc_1y1_time[i]);
     wire_vs_dt->Fill(H_dc_1y1_time[i], H_dc_1y1_wirenum[i]);

				}
			}
							
	 if (ip==4) {
    for (int i=0; i< Ndata_H_dc_1y2_wirenum; i++){
       wirenum = int(H_dc_1y2_wirenum[i]);
      //cout << " wire num: " << H_dc_1y2_wirenum[i] << endl;
	  //cout << "Time: " << H_dc_1y2_time[i] << endl;

      //Fill the Histograms
     cell_dt[wirenum-1]->Fill(H_dc_1y2_time[i]);
     wire_vs_dt->Fill(H_dc_1y2_time[i], H_dc_1y2_wirenum[i]);

				}
			}						
			
		 if (ip==7) {
    for (int i=0; i< Ndata_H_dc_2y1_wirenum; i++){
       wirenum = int(H_dc_2y1_wirenum[i]);
      //cout << " wire num: " << H_dc_2y1_wirenum[i] << endl;
	  //cout << "Time: " << H_dc_2y1_time[i] << endl;

      //Fill the Histograms
     cell_dt[wirenum-1]->Fill(H_dc_2y1_time[i]);
     wire_vs_dt->Fill(H_dc_2y1_time[i], H_dc_2y1_wirenum[i]);

				}
			}		
			
		if (ip==10) {
    for (int i=0; i< Ndata_H_dc_2y2_wirenum; i++){
       wirenum = int(H_dc_2y2_wirenum[i]);
      //cout << " wire num: " << H_dc_2y2_wirenum[i] << endl;
	  //cout << "Time: " << H_dc_2y2_time[i] << endl;

      //Fill the Histograms
     cell_dt[wirenum-1]->Fill(H_dc_2y2_time[i]);
     wire_vs_dt->Fill(H_dc_2y2_time[i], H_dc_2y2_wirenum[i]);

				}
			}						
					
					
					
					}
					}

//Write wire drift time histos to file
g[ip]->Write();
	cout << "EVERYTHING OK in plane:" << ip << endl;	

}


     //     cout << "\r          \r" << (float)sensewire / total_wires * 100.0 << "%" << flush; 

}
