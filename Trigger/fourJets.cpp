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
	TTreeReaderArray<double>  first_jet( reader, "j1_pt");
	TTreeReaderArray<double>  second_jet( reader, "j2_pt");
	TTreeReaderArray<double>  third_jet( reader, "jb1_pt");
	TTreeReaderArray<double>  fourth_jet( reader, "jb2_pt");
	
	double survivors = 0;

	while (reader.Next()) {

		//Loop over the events
		for (int iEv = 0; iEv < first_jet.GetSize(); ++iEv) {

			//Ordino i pt dal più piccolo al più grande
			vector<double> jets {first_jet[iEv], second_jet[iEv], third_jet[iEv], fourth_jet[iEv] };
			sort(jets.begin(), jets.end());

			//Richiedo che il pt più grande passi il cut più grande e così via
			if (jets[3] > cuts[3] && jets[2] > cuts[2] && jets[1] > cuts[1] && jets[0] > cuts[0]) {
				survivors += 1;
			}

		}//end of loop over the events

	 }//end of while

	double efficiency = survivors/first_jet.GetSize();

	return 0;
}

