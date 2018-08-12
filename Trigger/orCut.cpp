
// c++ -o orCut orCut.cpp `root-config --cflags --glibs`

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

	int couple;

	cout << "Scegli la coppia di trigger da studiare: " << endl;
	cout << "1: HLT_IsoMu24_v* e HLT_Ele32_WPTight_Gsf_v*" << endl;
	cout << "2: HLT_IsoMu24_v* e HLT_Ele28_eta2p1_WPTight_Gsf_HT150_v*" << endl;
	cout << "3: HLT_IsoMu24_v* e HLT_Ele30_eta2p1_WPTight_Gsf_CentralPFJet35_EleCleaned_v*" << endl;

	cin >> couple;

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

	//First case: two variables
	if (couple == 1) {

		//One cut is supposed to be applied on events from ele_tree; the other one is for events saved in mu_tree
		double eleCut, muCut;

		//Considering the trigger list, the two variable must be "ele_pt" and "mu_pt"
		cout << "Inserisci il valore del taglio da applicare a mu_pt: " << endl;
		cin >> muCut;

		cout << "Inserisci il valore del taglio da applicare ad ele_pt: " << endl;
		cin >> eleCut;

		while ( mu_reader.Next() ) {

			//First trigger
			if (*mu_pt_value > muCut) {

				survivors += 1;

			}

		iEv += 1;

		}

		while ( ele_reader.Next() ) {

			//Second trigger: the intersection is empty 
			if (*ele_pt_value > eleCut) {

				survivors += 1;
			}

		iEv += 1;

		}

	} //end of couple == 1

	//Second case: four variables
	if (couple == 2) {

		//Considering the trigger list:
		double eleCut, muCut, htCut, etaCut;

		cout << "Inserisci il valore del taglio da applicare a mu_pt: " << endl;
		cin >> muCut;

		cout << "Inserisci il valore del taglio da applicare ad ele_pt: " << endl;
		cin >> eleCut;

		cout << "Inserisci il valore del taglio da applicare ad ele_eta: " << endl;
		cin >> etaCut;

		cout << "Inserisci il valore del taglio da applicare ad HT: " << endl;
		cin >> htCut;

		while ( ele_reader.Next() ) {

			//Cross-trigger: 
			if (*ele_pt_value > eleCut && abs(*ele_eta_value) < etaCut && *et_HT_value > htCut ) {

				survivors += 1;
			}

		iEv += 1;

		}

		//The intersection is empty: events with muons and events with electrons are saved in two different trees 
		//(also HT)
		while ( mu_reader.Next() ) {

			//Second trigger 
			if (*mu_pt_value > muCut) {

				survivors += 1;
			}

		iEv += 1;

		}


	} //end of couple == 2

	//Third case: five variables
	if (couple == 3) {

		//Considering the trigger list:
		double muCut, ele_pt_cut, jet_pt_cut, ele_eta_cut, jet_eta_cut; 

		cout << "Inserisci il valore del taglio da applicare a mu_pt: " << endl;
		cin >> muCut;

		cout << "Inserisci il valore del taglio da applicare ad ele_pt: " << endl;
		cin >> ele_pt_cut;

		cout << "Inserisci il valore del taglio da applicare ad ele_eta: " << endl;
		cin >> ele_eta_cut;

		cout << "Inserisci il valore del taglio da applicare a jet_pt: " << endl;
		cin >> jet_pt_cut;

		cout << "Inserisci il valore del taglio da applicare a jet_eta: " << endl;
		cin >> jet_eta_cut;

		//Cross-trigger
		while (ele_reader.Next()) {

			//Ordering the pt values from the jets
			vector<double> et_jets {*et_first_jet, *et_second_jet, *et_third_jet, *et_fourth_jet };
			sort(et_jets.begin(), et_jets.end());

			if(et_jets[3] == *et_first_jet) {

				if (et_jets[3] > jet_pt_cut && abs(*et_first_eta) < jet_eta_cut && *ele_pt_value > ele_pt_cut 
				&& abs(*ele_eta_value) < ele_eta_cut) {

					survivors += 1;
				}
			}

			if(et_jets[3] == *et_second_jet) {

				if (et_jets[3] > jet_pt_cut && abs(*et_second_eta) < jet_eta_cut && *ele_pt_value > ele_pt_cut 
				&& abs(*ele_eta_value) < ele_eta_cut) {

					survivors += 1;
				}
			}

			if(et_jets[3] == *et_third_jet) {

				if (et_jets[3] > jet_pt_cut && abs(*et_third_eta) < jet_eta_cut && *ele_pt_value > ele_pt_cut 
				&& abs(*ele_eta_value) < ele_eta_cut) {

					survivors += 1;
				}
			}

			if(et_jets[3] == *et_fourth_jet) {

				if (et_jets[3] > jet_pt_cut && abs(*et_fourth_eta) < jet_eta_cut && *ele_pt_value > ele_pt_cut 
				&& abs(*ele_eta_value) < ele_eta_cut) {

					survivors += 1;
				}
			}

			iEv += 1;
		}

		while ( mu_reader.Next() ) {

			//Second trigger: the intersection is empty 
			if (*mu_pt_value > muCut) {

				survivors += 1;
			}

			iEv += 1;

		}	

	} //end of couple == 3

	double efficiency = survivors/iEv;

	cout << "Numero di eventi: " << iEv << endl;
	cout << "Eventi sopravvissuti: " << survivors << endl;
	cout << "Efficienza: " << efficiency << endl;

	return 0;
}



