
//c++ fourCuts.cpp -o fourCuts `root-config --cflags --glibs`

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
	double first_cut, second_cut, third_cut, mjj_cut, MET_cut;

	cout << "Inserisci il valore del primo taglio sul jet: " << endl;
	cin >> first_cut;

	cout << "Inserisci il valore del secondo taglio sul jet: " << endl;
	cin >> second_cut;

	cout << "Inserisci il valore del taglio sulla MET: " << endl;
	cin >> MET_cut;

	cout << "Inserisci il valore del taglio sulla massa invariante dei jet: " << endl;
	cin >> mjj_cut;

	//Metto le variabili passate a terminale in un vettore e lo ordino dal più piccolo al più grande
	vector<double> cuts {first_cut, second_cut};
	sort(cuts.begin(), cuts.end());	

	//Lettura del tree contenuto nel file
	TTreeReader reader("tree", input);

	//Jet	
	TTreeReaderArray<double>  first_jet( reader, "j1_pt");
	TTreeReaderArray<double>  second_jet( reader, "j2_pt");
	TTreeReaderArray<double>  third_jet( reader, "jb1_pt");
	TTreeReaderArray<double>  fourth_jet( reader, "jb2_pt");

	//Invariant masses
	TTreeReaderArray<double>  jj_mass( reader, "mjj");
	TTreeReaderArray<double>  bb_mass( reader, "mbb");
	TTreeReaderArray<double>  jb11_mass( reader, "mjb11");
	TTreeReaderArray<double>  jb12_mass( reader, "mjb12");	
	TTreeReaderArray<double>  jb21_mass( reader, "mjb21");
	TTreeReaderArray<double>  jb22_mass( reader, "mjb22");

	//MET
	TTreeReaderArray<double>  MET_value( reader, "MET");
	
	double survivors = 0;

	while (reader.Next()) {

		//Loop over the events
		for (int iVar = 0; iVar < first_jet.GetSize(); ++iVar) {

			//Ordino i pt dal più piccolo al più grande
			vector<double> jets {first_jet[iVar], second_jet[iVar], third_jet[iVar], fourth_jet[iVar]};
			sort(jets.begin(), jets.end());

			//Stessa cosa per la massa invariante
			vector<double> mjj_array { bb_mass[iVar], jj_mass[iVar], jb11_mass[iVar], jb12_mass[iVar], jb21_mass[iVar], 
			jb22_mass[iVar] };

			sort(mjj_array.begin(), mjj_array.end());

			//Richiedo che il pt più grande passi il cut più grande e così via
			if (jets[3] > cuts[1] && jets[2] > cuts[0] && mjj_array[5] > mjj_cut && MET_value[iVar] > MET_cut) {

				survivors += 1;
			}

		}

	 }//end of while

	double efficiency = survivors/1000000;

	cout << "L'efficienza vale: " << efficiency << endl;

	return 0;
}
