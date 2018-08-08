
//c++ leptonJet.cpp -o leptonJet `root-config --cflags --glibs`

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
	
	//Open the file with the events
	TFile * input = TFile::Open( "TTrees.root" );

	double jet_pt_cut, lep_pt_cut;

	cout << "Inserisci il valore del taglio da applicare al momento trasverso del jet: " << endl;
	cin >> jet_pt_cut;

	cout << "Inserisci il valore del taglio da applicare al momento trasverso del leptone: " << endl;
	cin >> lep_pt_cut;

	int lep;

	cout << "Il leptone è un muone o un elettrone? " << endl;
	cout << "1: muone    2: elettrone" << endl;
	cin >> lep;

	//Reading the two TTrees in the .root file
	TTreeReader mu_reader("mu_tree", input);
	TTreeReader ele_reader("ele_tree", input);

	//Jet transverse momentum	
	TTreeReaderValue<double>  et_first_jet( ele_reader, "j1_pt");
	TTreeReaderValue<double>  et_second_jet( ele_reader, "j2_pt");
	TTreeReaderValue<double>  et_third_jet( ele_reader, "jb1_pt");
	TTreeReaderValue<double>  et_fourth_jet( ele_reader, "jb2_pt");

	TTreeReaderValue<double>  mt_first_jet( mu_reader, "j1_pt");
	TTreeReaderValue<double>  mt_second_jet( mu_reader, "j2_pt");
	TTreeReaderValue<double>  mt_third_jet( mu_reader, "jb1_pt");
	TTreeReaderValue<double>  mt_fourth_jet( mu_reader, "jb2_pt");

	//Leptons transverse momentum
	TTreeReaderValue<double>  electron_pt( ele_reader, "ele_pt");
	TTreeReaderValue<double>  muon_pt( mu_reader, "mu_pt");

	double survivors = 0;

	int iEv = 0;

	//Second case: a muon is produced
	if (lep == 1) {

		while (mu_reader.Next()) {

			//Searching jet with biggest momentum
			vector<double> mu_jets {*mt_first_jet, *mt_second_jet, *mt_third_jet, *mt_fourth_jet };
			sort(mu_jets.begin(), mu_jets.end());

			//No need to consider variables from ele_tree: the cut is an &&
			if (mu_jets[3] > jet_pt_cut && *muon_pt > lep_pt_cut) {

				survivors += 1;
			}

			//Count the events
			++iEv;

		} //end of while
			
	} //end of lep == 1

	//Second case: an electron is produced
	if (lep == 2) {

		while (ele_reader.Next()) {

			//Searching jet with biggest momentum
			vector<double> ele_jets {*et_first_jet, *et_second_jet, *et_third_jet, *et_fourth_jet };
			sort(ele_jets.begin(), ele_jets.end());

			//No need to consider variables from ele_tree: the cut is an &&
			if (ele_jets[3] > jet_pt_cut && *electron_pt > lep_pt_cut) {

				survivors += 1;
			}

			//Count the events
			++iEv;

		} //end of while
			
	} //end of lep == 2

	double tot_efficiency = survivors/1000000;

	double efficiency = survivors/iEv;

	cout << "Numero di eventi: " << iEv << endl;

	cout << "L'efficienza totale vale: " << tot_efficiency << endl;
	cout << "L'efficienza vale: " << efficiency << endl;

	return 0;
}

