
//c++ three_cuts.cpp -o three_cuts `root-config --cflags --glibs`

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
#include "TTreeReaderArray.h"
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

	TH2F *h_cut = new TH2F("h_cut","Title", x_nbin, x_min, x_max, y_nbin, y_min, y_max);
	TH2F *cut_cumulative = new TH2F("cumulative","Title", x_nbin, x_min, x_max, y_nbin, y_min, y_max);

	//Apro il file in cui sono contenuti gli eventi di segnale
	TFile * input = TFile::Open( "unweighted_events.root" );

	string * first_var = new string; 
	string * second_var = new string;
	string * third_var = new string;

	//Salvo le variabili passate a terminale in una stringa	
	first_var[0] = argv[1];
	second_var[0] = argv[2];
	third_var[0] = argv[3];

	//Lettura del tree contenuto nel file
	TTreeReader reader("tree", input);

	//Converto var[0] in un puntatore di tipo char come richiesto da TTreeReaderValue.h	
	TTreeReaderArray<double>  first_variable( reader, first_var[0].c_str() );
	TTreeReaderArray<double>  second_variable( reader, second_var[0].c_str() );
	TTreeReaderArray<double>  third_variable( reader, third_var[0].c_str() );

	//Applico il terzo taglio
	double third_cut;

	cout << "Inserisci il valore del terzo taglio: " << endl;
	cin >> third_cut;

	//Scrittura dell'istogramma dai dati contenuti nel TTree
	while ( reader.Next() ) {

		for (int iEv = 0; iEv < first_variable.GetSize(); ++iEv) {

			if (second_var[0]=="ele_eta"  || second_var[0]=="mu_eta") {

				h2->Fill(first_variable[iEv], abs(second_variable[iEv]));

			} 

			if (first_var[0]=="ele_eta" || first_var[0]=="mu_eta") {

				h2->Fill(abs(first_variable[iEv]), second_variable[iEv]);
			}

			if (first_var[0]!="ele_eta" && second_var[0]!="ele_eta" && first_var[0]!="mu_eta" && second_var[0]!="mu_eta") {

				h2->Fill(first_variable[iEv], second_variable[iEv]);
			}

			if (third_variable[iEv] >= third_cut) {

				h_cut -> Fill(first_variable[iEv], second_variable[iEv]);
			}
		}	
	} 

	//Creo l'istogramma cumulativo
	int x_cut, y_cut;

	cout << "Che tipo di taglio vuoi fare sull'asse x?" << endl;	
	cout << "(1: x < soglia    2: x > soglia)" << endl;
	cin >> x_cut;

	cout << "Che tipo di taglio vuoi fare sull'asse y?" << endl;	
	cout << "(1: y < soglia    2: y > soglia)" << endl;
	cin >> y_cut;

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

	//Creo l'istogramma cumulativo per h_cut
	if ( x_cut == 1 && y_cut == 1 ) {

		for (int i=1; i <= x_nbin; ++i ) {
	
			for (int j=1; j <= y_nbin; ++j) {

				double sum = 0;
		
				for (int z=1; z <= i; ++z) {

					for (int t=1; t <= j; ++t) {

						sum += h_cut -> GetBinContent(z,t);
					}
				}	

				cut_cumulative -> SetBinContent(i, j, sum);
			}

		}
	}

	if ( x_cut == 2 && y_cut == 1 ) {

		for (int i=1; i <= x_nbin; ++i ) {
	
			for (int j=1; j <= y_nbin; ++j) {

				double sum = 0;
		
				for (int z=x_nbin; z >= i; --z) {

					for (int t=1; t <= j; ++t) {

						sum += h_cut -> GetBinContent(z,t);
					}
				}	

				cut_cumulative -> SetBinContent(i, j, sum);
			}

		}
	}

	if ( x_cut == 1 && y_cut == 2 ) {

		for (int i=1; i <= x_nbin; ++i ) {
	
			for (int j=1; j <= y_nbin; ++j) {

				double sum = 0;
		
				for (int z=1; z <= i; ++z) {

					for (int t=y_nbin; t >= j; --t) {

						sum += h_cut -> GetBinContent(z,t);
					}
				}	

				cut_cumulative -> SetBinContent(i, j, sum);
			}

		}
	}

	if ( x_cut == 2 && y_cut == 2 ) {

		for (int i=1; i <= x_nbin; ++i ) {
	
			for (int j=1; j <= y_nbin; ++j) {

				double sum = 0;
		
				for (int z=x_nbin; z >= i; --z) {

					for (int t=y_nbin; t >= j; --t) {

						sum += h_cut -> GetBinContent(z,t);
					}
				}	

				cut_cumulative -> SetBinContent(i, j, sum);
			}

		}
	} 

	cut_cumulative -> Scale(norm); 

	//Stampo a terminale i valori dei due istogrammi in corrispondenza della soglia
	double x_threshold, y_threshold;
	double counts, efficiency;

	cout << "Inserisci il valore della soglia sull'asse x: " << endl;
	cin >> x_threshold;

	cout << "Inserisci il valore della soglia sull'asse y: " << endl;
	cin >> y_threshold;

	double x_length = x_max - x_min;
	double y_length = y_max - y_min;

	double x_var = ( x_nbin / x_length ) * x_threshold;
	double y_var = ( y_nbin / y_length ) * y_threshold; 

	efficiency = cut_cumulative -> GetBinContent(x_var, y_var);
	counts = h_cut -> GetBinContent(x_var, y_var);

	cout << "I conteggi in corrispondenza dei due tagli sono: " << counts << endl;
	cout << "L'efficienza in corrispondenza dei due tagli vale: " << efficiency << endl;

	//Draw histogram together with its cumulative distribution
	c->Divide(2,2);

	c->cd(1);
	h2->Draw("COLor");

	c->cd(2);
	h_cumulative->SetStats(kFALSE);
	h_cumulative->Draw("COLor");

	c->cd(3);
	h_cut->Draw("COLor");

	c->cd(4);
	cut_cumulative->SetStats(kFALSE);
	cut_cumulative->Draw("COLor");

	c->Update();

	gr->Run();

	return 0;
}

