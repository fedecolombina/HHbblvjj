
//c++ centralJet.cpp -o centralJet `root-config --cflags --glibs`

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
	TFile * input = TFile::Open( "TTrees.root" );

	//Salvo le variabili passate a terminale in una stringa	
	double jet_pt_cut, jet_eta_cut, lep_pt_cut, lep_eta_cut;

	cout << "Inserisci il valore del taglio da applicare al momento trasverso del jet: " << endl;
	cin >> jet_pt_cut;

	cout << "Inserisci il valore del taglio da applicare alla pseudorapidità del jet: " << endl;
	cin >> jet_eta_cut;

	cout << "Inserisci il valore del taglio da applicare al momento trasverso del leptone: " << endl;
	cin >> lep_pt_cut;

	cout << "Inserisci il valore del taglio da applicare alla pseudorapidità del leptone: " << endl;
	cin >> lep_eta_cut;

	int lep;

	cout << "Il leptone è un muone o un elettrone? " << endl;
	cout << "1: muone    2: elettrone" << endl;
	cin >> lep;

	//Lettura del tree contenuto nel file
	TTreeReader ele_reader("ele_tree", input);
	TTreeReader mu_reader("mu_tree", input);

	//Jet transverse momentum	
	TTreeReaderValue<double>  et_first_jet( ele_reader, "j1_pt");
	TTreeReaderValue<double>  et_second_jet( ele_reader, "j2_pt");
	TTreeReaderValue<double>  et_third_jet( ele_reader, "jb1_pt");
	TTreeReaderValue<double>  et_fourth_jet( ele_reader, "jb2_pt");

	TTreeReaderValue<double>  mt_first_jet( mu_reader, "j1_pt");
	TTreeReaderValue<double>  mt_second_jet( mu_reader, "j2_pt");
	TTreeReaderValue<double>  mt_third_jet( mu_reader, "jb1_pt");
	TTreeReaderValue<double>  mt_fourth_jet( mu_reader, "jb2_pt");

	//Jet pseudorapidity
	TTreeReaderValue<double>  et_first_eta( ele_reader, "j1_eta");
	TTreeReaderValue<double>  et_second_eta( ele_reader, "j2_eta");
	TTreeReaderValue<double>  et_third_eta( ele_reader, "jb1_eta");
	TTreeReaderValue<double>  et_fourth_eta( ele_reader, "jb2_eta");

	TTreeReaderValue<double>  mt_first_eta( mu_reader, "j1_eta");
	TTreeReaderValue<double>  mt_second_eta( mu_reader, "j2_eta");
	TTreeReaderValue<double>  mt_third_eta( mu_reader, "jb1_eta");
	TTreeReaderValue<double>  mt_fourth_eta( mu_reader, "jb2_eta");

	//Leptons transverse momentum
	TTreeReaderValue<double>  electron_pt( ele_reader, "ele_pt");
	TTreeReaderValue<double>  muon_pt( mu_reader, "mu_pt");

	//Leptons pseudorapidity
	TTreeReaderValue<double>  electron_eta( ele_reader, "ele_eta");
	TTreeReaderValue<double>  muon_eta( mu_reader, "mu_eta");
	
	double survivors = 0;

	int iEv = 0;
	
	//Se ho un muone:
	if (lep == 1) {

		while (mu_reader.Next()) {

			++iEv;

			//Ordino i pt dal più piccolo al più grande
			vector<double> mt_jets {*mt_first_jet, *mt_second_jet, *mt_third_jet, *mt_fourth_jet };
			sort(mt_jets.begin(), mt_jets.end());

			if(mt_jets[3] == *mt_first_jet) {

				if (mt_jets[3] > jet_pt_cut && abs(*mt_first_eta) < jet_eta_cut && *muon_pt > lep_pt_cut 
				&& abs(*muon_eta) < lep_eta_cut) {

					survivors += 1;
				}
			}

			if(mt_jets[3] == *mt_second_jet) {

				if (mt_jets[3] > jet_pt_cut && abs(*mt_second_eta) < jet_eta_cut && *muon_pt > lep_pt_cut 
				&& abs(*muon_eta) < lep_eta_cut) {

					survivors += 1;
				}
			}

			if(mt_jets[3] == *mt_third_jet) {

				if (mt_jets[3] > jet_pt_cut && abs(*mt_third_eta) < jet_eta_cut && *muon_pt > lep_pt_cut 
				&& abs(*muon_eta) < lep_eta_cut) {

					survivors += 1;
				}
			}

			if(mt_jets[3] == *mt_fourth_jet) {

				if (mt_jets[3] > jet_pt_cut && abs(*mt_fourth_eta) < jet_eta_cut && *muon_pt > lep_pt_cut 
				&& abs(*muon_eta) < lep_eta_cut) {

					survivors += 1;
				}
			}
		}		
	} //end of lep == 1

	//Se ho un elettrone:
	if (lep == 2) {

		while (ele_reader.Next()) {

			++iEv;

			//Ordino i pt dal più piccolo al più grande
			vector<double> et_jets {*et_first_jet, *et_second_jet, *et_third_jet, *et_fourth_jet };
			sort(et_jets.begin(), et_jets.end());

			if(et_jets[3] == *et_first_jet) {

				if (et_jets[3] > jet_pt_cut && abs(*et_first_eta) < jet_eta_cut && *electron_pt > lep_pt_cut 
				&& abs(*electron_eta) < lep_eta_cut) {

					survivors += 1;
				}
			}

			if(et_jets[3] == *et_second_jet) {

				if (et_jets[3] > jet_pt_cut && abs(*et_second_eta) < jet_eta_cut && *electron_pt > lep_pt_cut 
				&& abs(*electron_eta) < lep_eta_cut) {

					survivors += 1;
				}
			}

			if(et_jets[3] == *et_third_jet) {

				if (et_jets[3] > jet_pt_cut && abs(*et_third_eta) < jet_eta_cut && *electron_pt > lep_pt_cut 
				&& abs(*electron_eta) < lep_eta_cut) {

					survivors += 1;
				}
			}

			if(et_jets[3] == *et_fourth_jet) {

				if (et_jets[3] > jet_pt_cut && abs(*et_fourth_eta) < jet_eta_cut && *electron_pt > lep_pt_cut 
				&& abs(*electron_eta) < lep_eta_cut) {

					survivors += 1;
				}
			}
		}		
	} //end of lep == 2

			
	double tot_efficiency = survivors/1000000;
	double efficiency = survivors/iEv;

	cout << "L'efficienza vale: " << efficiency << endl;
	cout << "L'efficienza totale vale: " << tot_efficiency << endl;

	return 0;
}

