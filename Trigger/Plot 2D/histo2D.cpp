//The program creates 2D histograms (counts and efficiency) for triggers with two cuts. 

//c++ histo2D.cpp -o histo2D `root-config --cflags --glibs`

#include <iostream>
#include <cstdlib>
#include <TCanvas.h>
#include <TApplication.h>
#include <cmath>
#include <TStyle.h>
#include <TF1.h>
#include <TH1F.h>
#include <fstream>
#include <TMath.h>
#include <TH2F.h>
#include <TPad.h>
#include <TRandom.h>
#include <TLatex.h>
#include "TH1.h"
#include "TFile.h"
#include "TTree.h"
#include "TTreeReader.h"
#include "TTreeReaderValue.h"
#include <TPaveStats.h>
#include <TLegend.h>
#include <THStack.h>
#include <string>
#include <cstring>
#include <stdio.h>
#include <limits>

using namespace std;

int main (int argc, char** argv) {

	if (argc < 2){
			cout << "Usage: " << argv[0] << " q1 q2 q3 ... " << endl;
			return 1;
	}

	int x_nbin, x_max, x_min;
	int y_nbin, y_max, y_min;

	cout << "Inserisci il numero di bin per la prima variabile: " << endl;
	cin >> x_nbin;

	cout << "Inserisci il punto di inizio per la prima variabile: " << endl;
	cin >> x_min;

	cout << "Inseirsci il punto di fine per la prima variabile: " << endl;
	cin >> x_max; 

	cout << "Inserisci il numero di bin per la seconda variabile: " << endl;
	cin >> y_nbin;

	cout << "Inserisci il punto di inizio per la seconda variabile: " << endl;
	cin >> y_min;

	cout << "Inseirsci il punto di fine per la seconda variabile: " << endl;
	cin >> y_max; 

	TApplication * gr = new TApplication("grafica", 0, NULL);
	TCanvas *c = new TCanvas("c","c",600,400);

	TH2F *h2 = new TH2F("histo","Title", x_nbin, x_min, x_max, y_nbin, y_min, y_max);
	TH2F *h_cumulative = new TH2F("hc","Title", x_nbin, x_min, x_max, y_nbin, y_min, y_max);

	//Open the .root file with the events
	TFile * input = TFile::Open( "TTrees.root" );

	string * first_var = new string; 
	string * second_var = new string;

	//Save the variables in a string	
	first_var[0] = argv[1];
	second_var[0] = argv[2];

	//Reading the two TTrees in the .root file 
	TTreeReader ele_reader("ele_tree", input);
	TTreeReader mu_reader("mu_tree", input);

	//Take values from both TTrees
	TTreeReaderValue<double>  mt_first_variable( mu_reader, first_var[0].c_str() );
	TTreeReaderValue<double>  et_first_variable( ele_reader, first_var[0].c_str() );

	TTreeReaderValue<double>  mt_second_variable( mu_reader, second_var[0].c_str() );
	TTreeReaderValue<double>  et_second_variable( ele_reader, second_var[0].c_str() );

	//Histograms
	//Electron transverse momentum
	if (first_var[0] == "ele_pt" && second_var[0] == "ele_eta") {

		//Loop over the events
		while ( ele_reader.Next() ) {

			h2->Fill(*et_first_variable, abs(*et_second_variable));

		}//end of loop over the events 
	}

	if (first_var[0] == "ele_pt" && second_var[0] != "ele_eta") {

		//Loop over the events
		while ( ele_reader.Next() ) {

			h2->Fill(*et_first_variable, *et_second_variable);

		}//end of loop over the event
	}

	//Muon transverse momentum
	if (first_var[0] == "mu_pt" && second_var[0] == "mu_eta") {

		//Loop over the events
		while ( mu_reader.Next() ) {

			h2->Fill(*mt_first_variable, abs(*mt_second_variable));

		}//end of loop over the events 
	}

	if (first_var[0] == "mu_pt" && second_var[0] != "mu_eta") {

		//Loop over the events
		while ( mu_reader.Next() ) {

			h2->Fill(*mt_first_variable, *mt_second_variable);

		}//end of loop over the event
	}

	//Other variables
	if (first_var[0]!="mu_pt" && first_var[0]!="ele_pt") {

		while ( mu_reader.Next() ) {

			h2->Fill(*mt_first_variable, *mt_second_variable);

		}

		while ( ele_reader.Next() ) {

			h2->Fill(*et_first_variable, *et_second_variable);

		}	
	}

	//Create the cumulative distribution
	int x_cut, y_cut;

	cout << "Che tipo di taglio vuoi fare sull'asse x?" << endl;	
	cout << "(1: x < soglia    2: x > soglia)" << endl;
	cin >> x_cut;

	cout << "Che tipo di taglio vuoi fare sull'asse y?" << endl;	
	cout << "(1: y < soglia    2: y > soglia)" << endl;
	cin >> y_cut;

	//Considering all the combinations
	if ( x_cut == 1 && y_cut == 1 ) {

		for (int i=1; i <= x_nbin; ++i ) {
	
			for (int j=1; j <= y_nbin; ++j) {

				double sum = 0;
		
				for (int z=1; z <= i; ++z) {

					for (int t=1; t <= j; ++t) {

						sum += h2 -> GetBinContent(z,t);
					}
				}	

				h_cumulative -> SetBinContent(i, j, sum);
			}

		}
	}

	if ( x_cut == 2 && y_cut == 1 ) {

		for (int i=1; i <= x_nbin; ++i ) {
	
			for (int j=1; j <= y_nbin; ++j) {

				double sum = 0;
		
				for (int z=x_nbin; z >= i; --z) {

					for (int t=1; t <= j; ++t) {

						sum += h2 -> GetBinContent(z,t);
					}
				}	

				h_cumulative -> SetBinContent(i, j, sum);
			}

		}
	}

	if ( x_cut == 1 && y_cut == 2 ) {

		for (int i=1; i <= x_nbin; ++i ) {
	
			for (int j=1; j <= y_nbin; ++j) {

				double sum = 0;
		
				for (int z=1; z <= i; ++z) {

					for (int t=y_nbin; t >= j; --t) {

						sum += h2 -> GetBinContent(z,t);
					}
				}	

				h_cumulative -> SetBinContent(i, j, sum);
			}

		}
	}

	if ( x_cut == 2 && y_cut == 2 ) {

		for (int i=1; i <= x_nbin; ++i ) {
	
			for (int j=1; j <= y_nbin; ++j) {

				double sum = 0;
		
				for (int z=x_nbin; z >= i; --z) {

					for (int t=y_nbin; t >= j; --t) {

						sum += h2 -> GetBinContent(z,t);
					}
				}	

				h_cumulative -> SetBinContent(i, j, sum);
			}

		}
	}

	double norm = 1/(h2->Integral());
	h_cumulative -> Scale(norm);

	//Efficiency
	double x_threshold, y_threshold;
	double efficiency;

	cout << "Inserisci il valore della soglia sull'asse x: " << endl;
	cin >> x_threshold;

	cout << "Inserisci il valore della soglia sull'asse y: " << endl;
	cin >> y_threshold;

	double x_length = x_max - x_min;
	double y_length = y_max - y_min;

	double x_var = ( x_nbin / x_length ) * x_threshold;
	double y_var = ( y_nbin / y_length ) * y_threshold; 

	efficiency = h_cumulative -> GetBinContent(x_var, y_var);

	cout << "L'efficienza in corrispondenza dei due tagli vale: " << efficiency << endl;

	if (first_var[0]=="mu_pt" || second_var[0]=="mu_pt" || first_var[0]=="ele_pt" || second_var[0]=="ele_pt") {

		double tot_efficiency = efficiency * h2->GetEntries() / 1000000;

		cout << "L'efficieza totale vale: " << tot_efficiency << endl;
	}

	//Draw histogram together with its cumulative distribution
	c->Divide(1,2);

	c->cd(1);
	h2->Draw("COLor");

	c->cd(2);
	h_cumulative->SetStats(kFALSE);
	h_cumulative->Draw("COLor");

	c->Update();

	gr->Run();

	return 0;
}
