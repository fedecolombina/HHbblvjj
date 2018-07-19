//I valori dei quattro tagli devono essere inseriti a terminale quando richiesto dal programma.
//Una volta applicati i tagli viene stampato a terminale il valore dell'efficienza, non vengono restituiti grafici.
//Il programma è stato costruito considerando quattro tagli sui jet, quindi tagli di tipo x > soglia.

//c++ fourJets.cpp -o fourJets `root-config --cflags --glibs`

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
	
	//Apro il file in cui sono contenuti gli eventi di segnale
	TFile * input = TFile::Open( "unweighted_events.root" );

	//Salvo le variabili passate a terminale in una stringa	
	double first_cut, second_cut, third_cut, fourth_cut;

	cout << "Inserisci il valore del primo taglio: " << endl;
	cin >> first_cut;

	cout << "Inserisci il valore del secondo taglio: " << endl;
	cin >> second_cut;

	cout << "Inserisci il valore del terzo taglio: " << endl;
	cin >> third_cut;

	cout << "Inserisci il valore del quarto taglio: " << endl;
	cin >> fourth_cut;

	//Metto le variabili passate a terminale in un vettore e lo ordino dal più piccolo al più grande
	vector<double> cuts {first_cut, second_cut, third_cut, fourth_cut};
	sort(cuts.begin(), cuts.end());	

	//Lettura del tree contenuto nel file
	TTreeReader reader("tree", input);

	//Converto var[0] in un puntatore di tipo char come richiesto da TTreeReaderValue.h	
	TTreeReaderArray<double>  first_variable( reader, "j1_pt");
	TTreeReaderArray<double>  second_variable( reader, "j2_pt");
	TTreeReaderArray<double>  third_variable( reader, "jb1_pt");
	TTreeReaderArray<double>  fourth_variable( reader, "jb2_pt");

	double events;
	cout << "Inserisci il numero di eventi: " << endl;
	cin >> events;

	vector<double> first_jet(events);
	vector<double> second_jet(events);
	vector<double> third_jet(events);
	vector<double> fourth_jet(events);

	TH1F * h1 = new  TH1F("h1", "title", 1000, 0, 1000);
	TH1F * h2 = new  TH1F("h2", "title", 1000, 0, 1000);
	TH1F * h3 = new  TH1F("h3", "title", 1000, 0, 1000);
	TH1F * h4 = new  TH1F("h4", "title", 1000, 0, 1000);

	while (reader.Next()) {
		for (int iEv = 0; iEv < first_variable.GetSize(); ++iEv) {

			first_jet[iEv] = first_variable[iEv];
			second_jet[iEv] = second_variable[iEv];
			third_jet[iEv] = third_variable[iEv];
			fourth_jet[iEv] = fourth_variable[iEv];

			//Metto ciascuna distribuzione in un istogramma: mi servirà per i conteggi
			h1 -> Fill(first_jet[iEv]);
			h2 -> Fill(second_jet[iEv]);
			h3 -> Fill(third_jet[iEv]);
			h4 -> Fill(fourth_jet[iEv]);
			
		}
	}

	//Ordino gli elementi nei vettori (dal più piccolo al più grande)
	sort(first_jet.begin(), first_jet.end());
	sort(second_jet.begin(), second_jet.end());
	sort(third_jet.begin(), third_jet.end());
	sort(fourth_jet.begin(), fourth_jet.end());

	double efficiency[4];

	for (int iCut = 0; iCut < 4; ++iCut) {

		//per ogni array trovo l'indice dell'evento che corrisponde al taglio
		int iFirstJet = 0;
		int iSecondJet = 0; 
		int iThirdJet = 0; 
		int iFourthJet = 0;

		while (first_jet[iFirstJet] < cuts[iCut]) {

			iFirstJet += 1;
		} 

		while (second_jet[iSecondJet] < cuts[iCut]) {

			iSecondJet += 1;
		} 

		while (third_jet[iThirdJet] < cuts[iCut]) {

			iThirdJet += 1;
		} 

		while (fourth_jet[iFourthJet] < cuts[iCut]) {

			iFourthJet += 1;
		} 

		double v1, v2, v3, v4, v_max;

		v1 = h1 -> GetBinContent(iFirstJet);
		v2 = h2 -> GetBinContent(iSecondJet);
		v3 = h3 -> GetBinContent(iThirdJet);
		v4 = h4 -> GetBinContent(iFourthJet);

		vector<double> v {v1, v2, v3, v4};

    		auto biggest = max_element(begin(v), end(v));
    		v_max = *biggest;

		if (v_max == v1) {

			double cumulative = v1;

			for (int i = iFirstJet; i < 1000; ++i) {
				
				cumulative += h1 -> GetBinContent(i);		
			}

			efficiency[iCut] = cumulative/h1->Integral();
		}

		if (v_max == v2) {

			double cumulative = v2;

			for (int i = iSecondJet; i < 1000; ++i) {
				
				cumulative += h2 -> GetBinContent(i);		
			}

			efficiency[iCut] = cumulative/h2->Integral();
		}

		if (v_max == v3) {

			double cumulative = v3;

			for (int i = iThirdJet; i < 1000; ++i) {
				
				cumulative += h3 -> GetBinContent(i);		
			}

			efficiency[iCut] = cumulative/h3->Integral();
		}

		if (v_max == v4) {

			double cumulative = v4;

			for (int i = iFourthJet; i < 1000; ++i) {
				
				cumulative += h4 -> GetBinContent(i);		
			}

			efficiency[iCut] = cumulative/h4->Integral();
		}

		
	}

	double tot_efficiency = efficiency[0]*efficiency[1]*efficiency[2]*efficiency[3];

	cout << "L'efficienza totale vale: " << tot_efficiency << endl;


	return 0;
}

