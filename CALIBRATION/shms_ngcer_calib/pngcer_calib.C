/*
Cameron Cotton - 01/10/2023
Used to get the calibration constants for the SHMS Noble Gas Cerenkov PMTs.

Sample Usage:
root -l pngcer_calib.C
*/

#include <ROOT/RDataFrame.hxx>
#include <TChain.h>
#include <TCanvas.h>
#include <TF1.h>
#include <TLine.h>

int pngcer_calib() {
	// Input file format
	std::string replay_file_form = "../../ROOTfiles/SHMS/CALIBRATION/shms_replay_cer_%i_-1.root";

	// Taking user input
	std::string raw_run_input;
	std::vector<int> run_list;
	bool valid_runs = true;
	do {
		std::cout << "Please Enter Run Numbers for Calibation, Space Separated: ";
		std::getline(std::cin, raw_run_input);
		std::istringstream iss(raw_run_input);
		int run_num;
		run_list.clear();
		while (iss >> run_num) run_list.push_back(run_num);
		for (int run_number : run_list){
			if (run_number == 0){
				std::cout << "Ending Program...\n\n";
				return 1;
			}
			std::string replay_file = Form(replay_file_form.c_str(),run_number);
			if (FILE *file = fopen(replay_file.c_str(), "r")) { // check if file can be found
				fclose(file);
				std::cout << "Was able to open file at " << replay_file << "\n";
			} 
			else {
				std::cout << "Could not open file at " <<  replay_file 
				          << "\nDoes it exist? Try again or enter 0 to exit...\n\n";
				valid_runs = false;
				break;
			}
		}
	} 	while (!valid_runs);
	
	// Setting up RDataFrame
	ROOT::EnableImplicitMT(8);
	TChain chain("T");
	for(int run_number : run_list){
		chain.Add(Form(replay_file_form.c_str(), run_number));
		std::cout << "Adding run " << run_number << " to the chain...\n";
	}
	ROOT::RDataFrame df(chain);

	// Defining Cut Parameters
	int adcMultCut = 1;
	double xmin = 2;
	double ymin = 2;
	double xmax = 25;
	double ymax = 25;
	double emin = 0.7;
	double emax = 2.0;
	double dpmin = -10;
	double dpmax = 22;

	// Plotting Calorimeter distribution for entire dataset
	ROOT::RDF::TH1DModel m_etottracknorm ("h_etottracknorm","P.cal.etottracknorm Cuts on All Events", 300, 0, 3);
	auto h_etottracknorm = df.Filter(Form("P.cal.etottracknorm > 0.01 && P.gtr.dp > %.1f && P.gtr.dp < %.1f"
										  ,dpmin,dpmax)).Histo1D(m_etottracknorm,"P.cal.etottracknorm");

	// New RDataFrame with general good electron cuts applied
	auto df2 = df.Filter(Form("P.cal.etottracknorm > %.1f && P.cal.etottracknorm < %.1f && P.gtr.dp > %.1f"
							  "&& P.gtr.dp < %.1f",emin,emax,dpmin,dpmax));
	
	// Making Cerenkov position and pulse integral plots for each PMT
	ROOT::RDF::TH1DModel m_pmt1_int ("h_pmt1_int","PMT 1 goodAdcPulseInt Distribution", 80, 1, 161);
	ROOT::RDF::TH2DModel m_pmt1_pos ("h_pmt1_pos","PMT 1 Cerenkov Position Cuts", 60, -30, 30, 60, -30, 30);
	auto h_pmt1_int = df2.Filter(Form("P.ngcer.goodAdcMult[0] == %i && P.ngcer.goodAdcMult[1] == 0"
	                                  "&& P.ngcer.goodAdcMult[2] == 0 && P.ngcer.goodAdcMult[3] == 0"
									  "&& P.ngcer.xAtCer > %.1f && P.ngcer.xAtCer < %.1f" 
									  "&& P.ngcer.yAtCer > %.1f && P.ngcer.yAtCer < %.1f"
									  ,adcMultCut, xmin, xmax, ymin, ymax)).Histo1D(m_pmt1_int,"P.ngcer.goodAdcPulseInt");
	auto h_pmt1_pos = df2.Filter(Form("P.ngcer.goodAdcMult[0] == %i && P.ngcer.goodAdcMult[1] == 0"
	                                  "&& P.ngcer.goodAdcMult[2] == 0 && P.ngcer.goodAdcMult[3] == 0"
									  ,adcMultCut)).Histo2D(m_pmt1_pos,"P.ngcer.xAtCer","P.ngcer.yAtCer");
	
	ROOT::RDF::TH1DModel m_pmt2_int ("h_pmt2_int","PMT 2 goodAdcPulseInt Distribution", 80, 1, 161);
	ROOT::RDF::TH2DModel m_pmt2_pos ("h_pmt2_pos","PMT 2 Cerenkov Position Cuts", 60, -30, 30, 60, -30, 30);
	auto h_pmt2_int = df2.Filter(Form("P.ngcer.goodAdcMult[0] == 0 && P.ngcer.goodAdcMult[1] == %i"
	                                  "&& P.ngcer.goodAdcMult[2] == 0 && P.ngcer.goodAdcMult[3] == 0"
									  "&& P.ngcer.xAtCer > %.1f && P.ngcer.xAtCer < %.1f"
									  "&& P.ngcer.yAtCer < -1*%.1f && P.ngcer.yAtCer > -1*%.1f"
									  ,adcMultCut, xmin, xmax, ymin, ymax)).Histo1D(m_pmt2_int,"P.ngcer.goodAdcPulseInt");
	auto h_pmt2_pos = df2.Filter(Form("P.ngcer.goodAdcMult[0] == 0 && P.ngcer.goodAdcMult[1] == %i"
	                                  "&& P.ngcer.goodAdcMult[2] == 0 && P.ngcer.goodAdcMult[3] == 0"
									  ,adcMultCut)).Histo2D(m_pmt2_pos,"P.ngcer.xAtCer","P.ngcer.yAtCer");

	ROOT::RDF::TH1DModel m_pmt3_int ("h_pmt3_int","PMT 3 goodAdcPulseInt Distribution", 80, 1, 161);
	ROOT::RDF::TH2DModel m_pmt3_pos ("h_pmt3_pos","PMT 3 Cerenkov Position Cuts", 60, -30, 30, 60, -30, 30);
	auto h_pmt3_int = df2.Filter(Form("P.ngcer.goodAdcMult[0] == 0 && P.ngcer.goodAdcMult[1] == 0"
	                                  "&& P.ngcer.goodAdcMult[2] == %i && P.ngcer.goodAdcMult[3] == 0"
									  "&& P.ngcer.xAtCer < -1*%.1f && P.ngcer.xAtCer > -1*%.1f"
									  "&& P.ngcer.yAtCer > %.1f && P.ngcer.yAtCer < %.1f"
									  ,adcMultCut, xmin, xmax, ymin, ymax)).Histo1D(m_pmt3_int,"P.ngcer.goodAdcPulseInt");
	auto h_pmt3_pos = df2.Filter(Form("P.ngcer.goodAdcMult[0] == 0 && P.ngcer.goodAdcMult[1] == 0"
	                                  "&& P.ngcer.goodAdcMult[2] == %i && P.ngcer.goodAdcMult[3] == 0"
									  ,adcMultCut)).Histo2D(m_pmt3_pos,"P.ngcer.xAtCer","P.ngcer.yAtCer");

	ROOT::RDF::TH1DModel m_pmt4_int ("h_pmt4_int","PMT 4 goodAdcPulseInt Distribution", 80, 1, 161);
	ROOT::RDF::TH2DModel m_pmt4_pos ("h_pmt4_pos","PMT 4 Cerenkov Position Cuts", 60, -30, 30, 60, -30, 30);
	auto h_pmt4_int = df2.Filter(Form("P.ngcer.goodAdcMult[0] == 0 && P.ngcer.goodAdcMult[1] == 0"
									  "&& P.ngcer.goodAdcMult[2] == 0 && P.ngcer.goodAdcMult[3] == %i"
									  "&& P.ngcer.xAtCer < -1*%.1f && P.ngcer.xAtCer > -1*%.1f"
									  "&& P.ngcer.yAtCer < -1*%.1f && P.ngcer.yAtCer > -1*%.1f"
									  ,adcMultCut, xmin, xmax, ymin, ymax)).Histo1D(m_pmt4_int,"P.ngcer.goodAdcPulseInt");
	auto h_pmt4_pos = df2.Filter(Form("P.ngcer.goodAdcMult[0] == 0 && P.ngcer.goodAdcMult[1] == 0"
									  "&& P.ngcer.goodAdcMult[2] == 0 && P.ngcer.goodAdcMult[3] == %i"
									  ,adcMultCut)).Histo2D(m_pmt4_pos,"P.ngcer.xAtCer","P.ngcer.yAtCer");

	// Fitting pulse integral distribution for each PMT to determine calibration constant
	TCanvas* c1 = new TCanvas("c1", "Cerenkov Calibration", 1200, 1200);
	c1->Divide(2,2);
	c1->cd(1);
	TF1* f1 = new TF1("f1","[0]*TMath::Power(([1]/[2]),(x/[2]))*(TMath::Exp(-([1]/[2])))/TMath::Gamma((x/[2])+1)",30,70);
	f1->SetParameters(2000,50,3);
	h_pmt1_int->Fit(f1,"R");
	double yscale1 = f1->GetParameter(0);
	double mean1 = f1->GetParameter(1);
	double xscale1 = f1->GetParameter(2); // this is the calibration constant
	h_pmt1_int->SetTitle("PMT 1 Cerenkov Calibration Poisson Fit; Pulse Integral");
	auto h_pmt1_int_clone = h_pmt1_int->DrawClone();
	
	c1->cd(2);
	TF1* f2 = new TF1("f2","[0]*TMath::Power(([1]/[2]),(x/[2]))*(TMath::Exp(-([1]/[2])))/TMath::Gamma((x/[2])+1)",30,70);
	f2->SetParameters(2000,50,3);
	h_pmt2_int->Fit(f2,"R");
	double yscale2 = f2->GetParameter(0);
	double mean2 = f2->GetParameter(1);
	double xscale2 = f2->GetParameter(2); // this is the calibration constant
	h_pmt2_int->SetTitle("PMT 2 Cerenkov Calibration Poisson Fit; Pulse Integral");
	auto h_pmt2_int_clone = h_pmt2_int->DrawClone();
	
	c1->cd(3);
	TF1* f3 = new TF1("f3","[0]*TMath::Power(([1]/[2]),(x/[2]))*(TMath::Exp(-([1]/[2])))/TMath::Gamma((x/[2])+1)",30,70);
	f3->SetParameters(2000,50,3);
	h_pmt3_int->Fit(f3,"R");
	double yscale3 = f3->GetParameter(0);
	double mean3 = f3->GetParameter(1);
	double xscale3 = f3->GetParameter(2); // this is the calibration constant
	h_pmt3_int->SetTitle("PMT 3 Cerenkov Calibration Poisson Fit; Pulse Integral");
	auto h_pmt3_int_clone = h_pmt3_int->DrawClone();
	
	c1->cd(4);
	TF1* f4 = new TF1("f4","[0]*TMath::Power(([1]/[2]),(x/[2]))*(TMath::Exp(-([1]/[2])))/TMath::Gamma((x/[2])+1)",30,70);
	f4->SetParameters(2000,50,3);
	h_pmt4_int->Fit(f4,"R");
	double yscale4 = f4->GetParameter(0);
	double mean4 = f4->GetParameter(1);
	double xscale4 = f4->GetParameter(2); // this is the calibration constant
	h_pmt4_int->SetTitle("PMT 4 Cerenkov Calibration Poisson Fit; Pulse Integral");
	auto h_pmt4_int_clone = h_pmt4_int->DrawClone();
	
	// Drawing Cerenkov position distribution histograms
	TCanvas* c2 = new TCanvas("c2", "Cerenkov Position Cuts", 1200, 1200);
	c2->Divide(2,2);

	c2->cd(1);
	h_pmt1_pos->SetTitle("PMT 1 Cerenkov Position Cuts; xAtCer; yAtCer");
	auto h_pmt1_pos_clone = h_pmt1_pos->DrawClone("COLZ");
	TLine *l1 = new TLine(xmin,ymin,xmax,ymin);
    l1->SetLineColor(kRed);
    l1->Draw();
    TLine *l2 = new TLine(xmin,ymax,xmax,ymax);
    l2->SetLineColor(kRed);
    l2->Draw();
    TLine *l3 = new TLine(xmin,ymin,xmin,ymax);
    l3->SetLineColor(kRed);
    l3->Draw();
    TLine *l4 = new TLine(xmax,ymin,xmax,ymax);
    l4->SetLineColor(kRed);
    l4->Draw();
    
	c2->cd(2);
	h_pmt2_pos->SetTitle("PMT 2 Cerenkov Position Cuts; xAtCer; yAtCer");
	auto h_pmt2_pos_clone = h_pmt2_pos->DrawClone("COLZ");
	TLine *l5 = new TLine(xmin,-ymin,xmax,-ymin);
    l5->SetLineColor(kRed);
    l5->Draw();
    TLine *l6 = new TLine(xmin,-ymax,xmax,-ymax);
    l6->SetLineColor(kRed);
    l6->Draw();
    TLine *l7 = new TLine(xmin,-ymin,xmin,-ymax);
    l7->SetLineColor(kRed);
    l7->Draw();
    TLine *l8 = new TLine(xmax,-ymin,xmax,-ymax);
    l8->SetLineColor(kRed);
    l8->Draw();
	
	c2->cd(3);
	h_pmt3_pos->SetTitle("PMT 3 Cerenkov Position Cuts; xAtCer; yAtCer");
	auto h_pmt3_pos_clone = h_pmt3_pos->DrawClone("COLZ");
	TLine *l9 = new TLine(-xmin,ymin,-xmax,ymin);
    l9->SetLineColor(kRed);
    l9->Draw();
    TLine *l10 = new TLine(-xmin,ymax,-xmax,ymax);
    l10->SetLineColor(kRed);
    l10->Draw();
    TLine *l11 = new TLine(-xmin,ymin,-xmin,ymax);
    l11->SetLineColor(kRed);
    l11->Draw();
    TLine *l12 = new TLine(-xmax,ymin,-xmax,ymax);
    l12->SetLineColor(kRed);
    l12->Draw();
	
	c2->cd(4);
	h_pmt4_pos->SetTitle("PMT 4 Cerenkov Position Cuts; xAtCer; yAtCer");
	auto h_pmt4_pos_clone = h_pmt4_pos->DrawClone("COLZ");
	TLine *l13 = new TLine(-xmin,-ymin,-xmax,-ymin);
    l13->SetLineColor(kRed);
    l13->Draw();
    TLine *l14 = new TLine(-xmin,-ymax,-xmax,-ymax);
    l14->SetLineColor(kRed);
    l14->Draw();
    TLine *l15 = new TLine(-xmin,-ymin,-xmin,-ymax);
    l15->SetLineColor(kRed);
    l15->Draw();
    TLine *l16 = new TLine(-xmax,-ymin,-xmax,-ymax);
    l16->SetLineColor(kRed);
    l16->Draw();
	
	// Drawing calorimeter distribution histogram
	TCanvas* c3 = new TCanvas("c3", "P.cal.etottracknorm Cuts", 1200, 1200);
	auto h_etottracknorm_clone = h_etottracknorm->DrawClone();
	c3->Update();
	TLine *l17 = new TLine(emin,0,emin,c3->GetUymax());
    l17->SetLineColor(kRed);
    l17->Draw();
    TLine *l18 = new TLine(emax,0,emax,c3->GetUymax());
    l18->SetLineColor(kRed);
    l18->Draw();
	
	// Printing calibration constants to terminal
	std::cout << "1/PMT1 Calibration Constant: " << xscale1 << std::endl;
	std::cout << "1/PMT2 Calibration Constant: " << xscale2 << std::endl;
	std::cout << "1/PMT3 Calibration Constant: " << xscale3 << std::endl;
	std::cout << "1/PMT4 Calibration Constant: " << xscale4 << std::endl;
	
	return 0;
}
