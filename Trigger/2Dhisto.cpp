
//c++ 2Dhisto.cpp -o 2Dhisto `root-config --cflags --glibs`

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

	//Creo due istogrammi con lo stesso numero di bin su asse x e y
	TH2F *h_cumulative = new TH2F("hc","Title", x_nbin, x_min, x_max, x_nbin, y_min, y_max);
	TH2F *h_efficiency = new TH2F("he","Title", x_nbin, x_min, x_max, x_nbin, y_min, y_max);

	//Creo due istogrammi separati per le due variabili
	TH1 * hx = new  TH1F("hx", "title", x_nbin, x_min, x_max);
	TH1 * hy = new  TH1F("hy", "title", x_nbin, y_min, y_max);

	//Apro il file in cui sono contenuti gli eventi di segnale
	TFile * input = TFile::Open( "unweighted_events.root" );

	string * first_var = new string; 
	string * second_var = new string;

	//Salvo le variabili passate a terminale in una stringa	
	first_var[0] = argv[1];
	second_var[0] = argv[2];

	//Lettura del tree contenuto nel file
	TTreeReader reader("tree", input);

	//Converto var[0] in un puntatore di tipo char come richiesto da TTreeReaderValue.h	
	TTreeReaderValue<double>  first_variable( reader, first_var[0].c_str() );

	TTreeReaderValue<double>  second_variable( reader, second_var[0].c_str() );

	//Scrittura dell'istogramma dai dati contenuti nel TTree
	while ( reader.Next() ) {

		if (second_var[0]=="ele_eta"  || second_var[0]=="mu_eta") {

			h2->Fill(* first_variable, abs(*second_variable));
		
			hx -> Fill(*first_variable);
			hy -> Fill(abs(*second_variable));
		} 

		if (first_var[0]=="ele_eta" || first_var[0]=="mu_eta") {

			h2->Fill(abs(* first_variable), *second_variable);
	
			hx -> Fill(abs(*first_variable));
			hy -> Fill(*second_variable);
	
		}

		if (first_var[0]!="ele_eta" && second_var[0]!="ele_eta" && first_var[0]!="mu_eta" && second_var[0]!="mu_eta") {

			h2->Fill(* first_variable, *second_variable);
		
			hx -> Fill(*first_variable);
			hy -> Fill(*second_variable);
		}

	} //end of while

	double x_bin, y_bin, x_count, y_count;

	//Creo l'istogramma cumulativo 
	for (int k=0; k < x_nbin; ++k) {

		for (int i=0; i < x_nbin; ++i) {

			x_bin += hx -> GetBinContent(i); 
			h_cumulative -> SetBinContent(i, k, x_bin);
		}

		for (int j=0; j < x_nbin; ++j) {

			y_bin += hy -> GetBinContent(j);
			h_cumulative -> SetBinContent(k, j, y_bin);
		}
	}

	//Normalizzo l'istogramma

	double norm = 1/(h2->Integral()*x_nbin);
	h_cumulative -> Scale(norm);

	int num;

	cout << "Che tipo di taglio vuoi applicare all'asse x?" << endl;
	cout << "Digita 1 per x < soglia, 2 per x > soglia: " << endl;
	cin >> num;

	 if (num == 1) {

		int number;

		cout << "Che tipo di taglio vuoi applicare all'asse y?" << endl;
		cout << "Digita 1 per y < soglia, 2 per y > soglia: " << endl;

		cin >> number;

		if (number == 1) {
			//creo l'istogramma dell'efficienza per un taglio x > soglia e y > soglia
			for (int k=0; k < x_nbin; ++k) {

				for (int i=0; i < x_nbin; ++i) {

					x_count = h_cumulative -> GetBinContent(i, k);
					h_efficiency -> SetBinContent(i, k, x_count);		
				}


				for (int j=0; j < x_nbin; ++j) {

					y_count = h_cumulative -> GetBinContent(k, j);
					h_efficiency -> SetBinContent(k, j, y_count);
				}
			}
		}

		if (number == 2) {
		//creo l'istogramma dell'efficienza per un taglio x > soglia e y < soglia
			for (int k=0; k < x_nbin; ++k) {

				for (int i=0; i < x_nbin; ++i) {

					x_count = 1 - h_cumulative -> GetBinContent(i, k);
					h_efficiency -> SetBinContent(i, k, x_count);		
				}


				for (int j=0; j < x_nbin; ++j) {

					y_count = h_cumulative -> GetBinContent(k, j);
					h_efficiency -> SetBinContent(k, j, y_count);
				}
			}
		}
	}

	if (num == 2) {

		int number;

		cout << "Che tipo di taglio vuoi applicare all'asse y?" << endl;
		cout << "Digita 1 per y < soglia, 2 per y > soglia: " << endl;

		cin >> number;

		if (number == 1) {
			//creo l'istogramma dell'efficienza per un taglio x < soglia e y > soglia
			for (int k=0; k < x_nbin; ++k) {

				for (int i=0; i < x_nbin; ++i) {

					x_count = h_cumulative -> GetBinContent(i, k);
					h_efficiency -> SetBinContent(i, k, x_count);		
				}


				for (int j=0; j < x_nbin; ++j) {

					y_count = 1 - h_cumulative -> GetBinContent(k, j);
					h_efficiency -> SetBinContent(k, j, y_count);
				}
			}
		}

		if (number == 2) {
		//creo l'istogramma dell'efficienza per un taglio x < soglia e y < soglia
			for (int k=0; k < x_nbin; ++k) {

				for (int i=0; i < x_nbin; ++i) {

					x_count = 1 - h_cumulative -> GetBinContent(i, k);
					h_efficiency -> SetBinContent(i, k, x_count);		
				}


				for (int j=0; j < x_nbin; ++j) {

					y_count = 1 - h_cumulative -> GetBinContent(k, j);
					h_efficiency -> SetBinContent(k, j, y_count);
				}
			}
		}
	}
	//Rebin
	int comando;
	int rebin;

	cout << "Digita 1 se desideri ribinnare l'asse y: " << endl;
	cin >> comando;

	if (comando == 1) {
		cout << "Inserisci il numero di bin che vuoi unire: " << endl;
		cin >> rebin;
 
		h_efficiency->RebinY(rebin);
		double maximum = h_efficiency->GetBinContent(h_efficiency->GetMaximumBin());

		double normalize = 1/(maximum);
		h_efficiency -> Scale(normalize);
	}


	//Draw histogram together with its cumulative distribution
	c->Divide(1,2);

	c->cd(1);
	h2->Draw("COLor");

	c->cd(2);
	h_efficiency->Draw("COLor");

	c->Update();

	gr->Run();

	return 0;
}


