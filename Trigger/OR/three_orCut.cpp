
// c++ -o three_orCut three_orCut.cpp `root-config --cflags --glibs`

#include <iostream>
#include "TH1.h"
#include "TCanvas.h"
#include "TFile.h"
#include "TTree.h"
#include "TTreeReader.h"
#include "TTreeReaderValue.h"
#include <TStyle.h>
#include <TMath.h>
#include <TF1.h>
#include <TPaveStats.h>
#include <TLegend.h>
#include <THStack.h>
#include <TApplication.h>
#include <fstream>
#include <TLatex.h>
#include <TBrowser.h>
#include <cassert>
#include <cmath>

using namespace std;

int main(int argc, char** argv){ 

	int number;

	cout << "Scegli i tre trigger da studiare: " << endl;
	cout << "1: HLT_IsoMu24_v*, HLT_Ele32_WPTight_Gsf_v* e HLT_Ele28_eta2p1_WPTight_Gsf_HT150_v*" << endl;
	cout << "2: HLT_IsoMu24_v*, HLT_Ele32_WPTight_Gsf_v* e HLT_Ele30_eta2p1_WPTight_Gsf_CentralPFJet35_EleCleaned_v*" << endl;

	cin >> number;

	//Open the .root file with the events
	TFile * input = TFile::Open( "TTrees.root" );

	//Read both TTrees in the file
	TTreeReader ele_reader("ele_tree", input);
	TTreeReader mu_reader("mu_tree", input);

	//Access to the values
	TTreeReaderValue<double>  mu_pt_value( mu_reader, "mu_pt");
	TTreeReaderValue<double>  ele_pt_value( ele_reader, "ele_pt" );
	TTreeReaderValue<double>  ele_eta_value( ele_reader, "ele_eta");
	TTreeReaderValue<double>  et_HT_value( ele_reader, "HT" );

	//Jet transverse momentum	
	TTreeReaderValue<double>  et_first_jet( ele_reader, "j1_pt");
	TTreeReaderValue<double>  et_second_jet( ele_reader, "j2_pt");
	TTreeReaderValue<double>  et_third_jet( ele_reader, "jb1_pt");
	TTreeReaderValue<double>  et_fourth_jet( ele_reader, "jb2_pt");

	//Jet pseudorapidity
	TTreeReaderValue<double>  et_first_eta( ele_reader, "j1_eta");
	TTreeReaderValue<double>  et_second_eta( ele_reader, "j2_eta");
	TTreeReaderValue<double>  et_third_eta( ele_reader, "jb1_eta");
	TTreeReaderValue<double>  et_fourth_eta( ele_reader, "jb2_eta");

	double survivors = 0;
	int iEv = 0;

	if (number == 1) {
		
		double ele_first_cut, ele_second_cut, muCut, etaCut, htCut;

		//Considering the trigger list, the two variable must be "ele_pt" and "mu_pt"
		cout << "Inserisci il valore del taglio da applicare a mu_pt: " << endl;
		cin >> muCut;

		cout << "Inserisci il valore del primo taglio da applicare ad ele_pt (valore più grande): " << endl;
		cin >> ele_first_cut;

		cout << "Inserisci il valore del secondo taglio da applicare ad ele_pt (valore più basso): " << endl;
		cin >> ele_second_cut;

		cout << "Inserisci il valore del taglio da applicare ad ele_eta: " << endl;
		cin >> etaCut;

		cout << "Inserisci il valore del taglio da applicare ad HT: " << endl;
		cin >> htCut;

		while ( mu_reader.Next() ) {

			//First trigger: intersection with the other two triggers is empty
			if (*mu_pt_value > muCut) {

				survivors += 1;
			}

			iEv += 1;

		}

		while ( ele_reader.Next() ) {

			//Second trigger
			if (*ele_pt_value > ele_first_cut) {

				survivors += 1;
			}

			//Cross-trigger: this time the intersection is not empty (*ele_pt_value < ele_first_cut)
			if (*ele_pt_value > ele_second_cut && *ele_pt_value < ele_first_cut && abs(*ele_eta_value) < etaCut 
			&& *et_HT_value > htCut ) {

				survivors += 1;
			}

			iEv += 1;

		}
	} //end of number == 1

	if (number == 2) {

		double ele_first_cut, ele_second_cut, muCut, jet_pt_cut, ele_eta_cut, jet_eta_cut; 

		cout << "Inserisci il valore del taglio da applicare a mu_pt: " << endl;
		cin >> muCut;

		cout << "Inserisci il valore del primo taglio da applicare ad ele_pt (valore più grande): " << endl;
		cin >> ele_first_cut;

		cout << "Inserisci il valore del secondo taglio da applicare ad ele_pt (valore più basso): " << endl;
		cin >> ele_second_cut;

		cout << "Inserisci il valore del taglio da applicare ad ele_eta: " << endl;
		cin >> ele_eta_cut;

		cout << "Inserisci il valore del taglio da applicare a jet_pt: " << endl;
		cin >> jet_pt_cut;

		cout << "Inserisci il valore del taglio da applicare a jet_eta: " << endl;
		cin >> jet_eta_cut;

		while ( mu_reader.Next() ) {

			//First trigger: intersection with the other two triggers is empty
			if (*mu_pt_value > muCut) {

				survivors += 1;
			}

			iEv += 1;

		}

		while ( ele_reader.Next() ) {

			//Second trigger
			if (*ele_pt_value > ele_first_cut) {

				survivors += 1;
			}

			//Cross-trigger: this time the intersection is not empty (*ele_pt_value < ele_first_cut)
			//Ordering the pt values from the jets
			vector<double> et_jets {*et_first_jet, *et_second_jet, *et_third_jet, *et_fourth_jet };
			sort(et_jets.begin(), et_jets.end());

			if(et_jets[3] == *et_first_jet) {

				if (et_jets[3] > jet_pt_cut && abs(*et_first_eta) < jet_eta_cut && *ele_pt_value > ele_second_cut 
				&& *ele_pt_value < ele_first_cut && abs(*ele_eta_value) < ele_eta_cut) {

					survivors += 1;
				}
			}

			if(et_jets[3] == *et_second_jet) {

				if (et_jets[3] > jet_pt_cut && abs(*et_second_eta) < jet_eta_cut && *ele_pt_value > ele_second_cut 
				&& *ele_pt_value < ele_first_cut && abs(*ele_eta_value) < ele_eta_cut) {

					survivors += 1;
				}
			}

			if(et_jets[3] == *et_third_jet) {

				if (et_jets[3] > jet_pt_cut && abs(*et_third_eta) < jet_eta_cut && *ele_pt_value > ele_second_cut 
				&& *ele_pt_value < ele_first_cut && abs(*ele_eta_value) < ele_eta_cut) {

					survivors += 1;
				}
			}

			if(et_jets[3] == *et_fourth_jet) {

				if (et_jets[3] > jet_pt_cut && abs(*et_fourth_eta) < jet_eta_cut && *ele_pt_value > ele_second_cut 
				&& *ele_pt_value < ele_first_cut && abs(*ele_eta_value) < ele_eta_cut) {

					survivors += 1;
				}
			}

			iEv += 1;
		}		

	} //end of number == 2

	double efficiency = survivors/iEv;

	cout << "Numero di eventi: " << iEv << endl;
	cout << "Eventi sopravvissuti: " << survivors << endl;
	cout << "Efficienza: " << efficiency << endl;

	return 0;
}

