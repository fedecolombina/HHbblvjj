//This program converts .lhe into .root files

//c++ -o reading reading.cpp `root-config --cflags --glibs`

#include "LHEF.h"
#include <iostream>
#include "TH1.h"
#include "TFile.h"
#include "TTree.h"
#include "TCanvas.h"
#include "TString.h"
#include <fstream>
#include <algorithm>
#include "TLorentzVector.h"
#include <vector>

using namespace std;

int main(int argc, char** argv){

	//Change directory to find the .lhe file:
	const char * dirname;
	dirname = "/home/osboxes/Downloads";
	chdir(dirname);

	//Insert the file's name to run the program	
	if (argc < 3){
		cout << "Usage: " << argv[0] << " output.root file1.lhe.." << endl;
		return 1;
	}

	char* rootfile = argv[1];     
	TFile output( rootfile, "RECREATE");

	TTree* tree = new TTree("tree", "Events");
    
	//Muons
	double mu_eta, mu_pt;

	//Electrons
	double ele_eta, ele_pt;

	//Invariant mass of the lepton-neutrino system
	double ele_n_mass, mu_n_mass;

	//Jets from quarks beauty and antibeauty
	double jb1_eta, jb1_pt, jb2_eta, jb2_pt;

	//Jets from any other quark
	double j1_eta, j1_pt, j2_eta, j2_pt;

	//Energy released and missing enegy
	double HT, MET;

	//Average
	double av_jj, av_bb, av_jb11, av_jb12, av_jb21, av_jb22, av_max;

	//Muons
	tree->Branch("mu_pt", &mu_pt);
	tree->Branch("mu_eta", &mu_eta);

	//Electrons
	tree->Branch("ele_pt", &ele_pt);
	tree->Branch("ele_eta", &ele_eta);

	//Invariant mass if the lepton-neutrino system
	tree->Branch("ele_n_mass", &ele_n_mass);
	tree->Branch("mu_n_mass", &mu_n_mass);	

	//Jets from quarks beauty and antibeauty
	tree->Branch("jb1_pt", &jb1_pt);
	tree->Branch("jb1_eta", &jb1_eta);
	tree->Branch("jb2_pt", &jb2_pt);
	tree->Branch("jb2_eta", &jb2_eta);	

	//Jets from any other quark
	tree->Branch("j1_pt", &j1_pt);
	tree->Branch("j1_eta", &j1_eta);
	tree->Branch("j2_pt", &j2_pt);
	tree->Branch("j2_eta", &j2_eta);

	//Energy released and missing enegy	
	tree->Branch("HT", &HT);
	tree->Branch("MET", &MET);

	//Average
	tree->Branch("av_max", &av_max);
	
	long iEv = 0; 
	int tau = 0;
  
	for (int i = 2; i < argc; i++){ 

        	//Open a stream connected to an event file:
        	ifstream ifs(argv[i]);
        
        	//Create the Reader object:
        	LHEF::Reader reader(ifs);
        
        	//Now loop over all the events     
        	while ( reader.readEvent() ){

			//Muon
			mu_pt = 0;
			mu_eta = 0;

			//Electron
			ele_eta = 0;
			ele_pt = 0;

			//Invariant mass of the lepton-neutrino system
			ele_n_mass = 0;
			mu_n_mass = 0;
	
			//Jets from quarks beauty and antibeauty
			jb1_pt = 0;
			jb1_eta = 0;
			jb2_pt = 0;
			jb2_eta	= 0;

			//Jets from any other quark
			j1_pt = 0;
			j1_eta = 0;
			j2_pt = 0;
			j2_eta = 0;	

			//Energy released and missing enegy	
			HT = 0;
			MET = 0;
 
			//Count the events		
			iEv++;
		    
			//TlorentVector of electrons, muons and neutrinos
			TLorentzVector ele_mom, n_mom, mu_mom; 

			//TlorentVector of jets from quarks beauty (b1) and antibeauty (b2)
			TLorentzVector jb1_mom, jb2_mom;

			//TlorentVector of the other jets
			TLorentzVector j1_mom, j2_mom; 

			//Jets are also saved in a vector
			vector<int> jets;

			//Save quadrimomentum of all particles mapped with position in the event
			map <int, TLorentzVector> momenta;
		    
			//Save eta and pt of particles            
			//Loop over particles in the event
			for (int iPart = 0 ; iPart < reader.hepeup.IDUP.size (); iPart++){

				//Saving info of final state particles only
				if (reader.hepeup.ISTUP.at (iPart) == 1){

					int ID = reader.hepeup.IDUP.at(iPart);

					//Information in the .lhe file are saved in a TLorentzVector:  
					//PUP --> quadrimomentum                   
	 				TLorentzVector momentum(
					reader.hepeup.PUP.at (iPart).at (0), //px
					reader.hepeup.PUP.at (iPart).at (1), //py
					reader.hepeup.PUP.at (iPart).at (2), //pz
					reader.hepeup.PUP.at (iPart).at (3) //E
					);

					//Save momentum
					momenta[iPart] = momentum;

					//Eletrons and positrons 
					//Events with a Tau or an antitau are not considered
					if( abs(ID) == 11){  

						//The TLorentzVector belongs to an electron:						
						ele_mom = momentum;

						ele_eta = momentum.Eta();
						ele_pt = momentum.Pt(); //Trasversal momentum

					}

					//Muons and antimuons
					if( abs(ID) == 13){  

						mu_mom = momentum;

						mu_eta = momentum.Eta();
						mu_pt = momentum.Pt();   

					}

					//Checking whether tau and antitau are producted: these events are not considered
					if (abs(ID) == 15){ tau = 1;}

					//Neutrinos (no mass)
					if ( abs(ID) == 12 || abs(ID) == 14 ){

						n_mom = momentum;

						//Saving MET (neutrino transverse momentum)
						MET = momentum.Pt(); 
					}
			
					//Quark beauty 
					if ( ID == 5 ){  

						jb1_mom = momentum;

						jb1_eta = momentum.Eta();
						jb1_pt = momentum.Pt();
					}
				
					//Quark antibeauty 
					if ( ID == -5 ){  

						jb2_mom = momentum;

						jb2_eta = momentum.Eta();
						jb2_pt = momentum.Pt();
					}

					//Other quarks in final state are from the jet 
					if ( abs(ID) < 7 && abs(ID) != 5 ){ 
 
						//The function push_back adds an element to the vector		
						jets.push_back(iPart);
					} 
		            
	
				}
			}//End of loop over particles	
			
			//The j jet are saved looking at the element of the jet vector   
			if(jets.size() > 0 && jets.size() < 3 && tau == 0){      

				for (int i=0; i<jets.size(); i += 2){

					j1_mom = momenta[jets[i]];
					j2_mom = momenta[jets[i+1]];

					j1_pt = j1_mom.Pt();
		    			j1_eta = j1_mom.Eta();

		   			j2_pt = j2_mom.Pt();
		    			j2_eta = j2_mom.Eta();

					//Saving average
					av_jj = (j1_pt + j2_pt)/2;
					av_bb = (jb1_pt + jb2_pt)/2;

					av_jb11 = (j1_pt + jb1_pt)/2;
					av_jb12 = (j1_pt + jb2_pt)/2;
					av_jb22 = (j2_pt + jb2_pt)/2;
					av_jb21 = (j2_pt + jb1_pt)/2;

					vector<double> av {av_jj, av_bb, av_jb11, av_jb12, av_jb21, av_jb22};

    					auto biggest = max_element(begin(av), end(av));
    					av_max = *biggest;

				}//End of jet construction 

			//Saving HT
			HT = jb1_pt + jb2_pt + j1_pt + j2_pt + mu_pt + ele_pt;

			//Saving invariant masses
			if (mu_pt != 0) {

				mu_n_mass = (mu_mom + n_mom).M();
			}

			if (ele_pt != 0) {

				ele_n_mass = (ele_mom + n_mom).M();
			}

			tree -> Fill();
		
			//Otherwise the event is not saved 	
			} else { 
				tau = 0; 
			}  	
   
		}//End of loop over events       
        
		ifs.close();
	}//End of loop over files

	tree->Write();

	output.Close();    

	return 0;
}
