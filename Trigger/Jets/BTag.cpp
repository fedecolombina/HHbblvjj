
//c++ BTag.cpp -o BTag `root-config --cflags --glibs`

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
	double first_cut, second_cut;

	cout << "Inserisci il valore del taglio sul jet: " << endl;
	cin >> first_cut;

	cout << "Inserisci il valore del taglio sulla seconda variabile: " << endl;
	cin >> second_cut;	

	//Lettura del tree contenuto nel file
	TTreeReader reader("tree", input);

	TTreeReaderArray<double>  first_jet( reader, "jb1_pt");
	TTreeReaderArray<double>  second_jet( reader, "jb2_pt");

	//In questo caso la seconda variabile è il momento trasverso del muone
	TTreeReaderArray<double>  muon( reader, "mu_pt");
	
	double survivors = 0;

	while (reader.Next()) {

		//Loop over the events
		for (int iVar = 0; iVar < first_jet.GetSize(); ++iVar) {

			//Ordino i pt dal più piccolo al più grande
			vector<double> jets {first_jet[iVar], second_jet[iVar] };
			sort(jets.begin(), jets.end());

			//Richiedo che il pt più grande passi il cut più grande e così via
			if (jets[1] > first_cut && muon[iVar] > second_cut ) {
				survivors += 1;
			}

		}//end of loop over the events

	 }//end of while

	double efficiency = survivors/1000000;

	cout << "L'efficienza vale: " << efficiency << endl;

	return 0;
}
