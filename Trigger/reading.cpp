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

using namespace std;

int main(int argc, char** argv){

	//Change directory to find the .lhe file:
	const char * dirname;
	dirname = "/media/sf_Condivisa";
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
	double mu_eta, mu_pt, mu_E, mu_Et;

	//Electrons
	double ele_eta, ele_pt, ele_E, ele_Et;

	//Neutrinos
	double n_eta, n_pt;

	//Jets from quarks beauty and antibeauty
	double jb1_eta, jb1_pt, jb2_eta, jb2_pt;

	//Jets from any other quark
	double j1_eta, j1_pt, j2_eta, j2_pt;

	//Energy released and missing enegy
	double HT, MET;

	//Total values from the jets
	double mjj, jj_Et, jj_pt;
	double mbb, bb_Et, bb_pt;

	//Muons
	tree->Branch("mu_pt", &mu_pt);
	tree->Branch("mu_E", &mu_E);
	tree->Branch("mu_Et", &mu_Et);
	tree->Branch("mu_eta", &mu_eta);

	//Electrons
	tree->Branch("ele_pt", &ele_pt);
	tree->Branch("ele_E", &ele_E);
	tree->Branch("ele_Et", &ele_Et);
	tree->Branch("ele_eta", &ele_eta);
	
	//Neutrinos
	tree->Branch("n_pt", &n_pt);
	tree->Branch("n_eta", &n_eta);
	
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

	//Total values from the jets
	tree->Branch("mjj", &mjj);
	tree->Branch("jj_pt", &jj_pt);
	tree->Branch("jj_Et", &jj_Et);
	tree->Branch("mbb", &mbb);
	tree->Branch("bb_pt", &bb_pt);
	tree->Branch("bb_Et", &bb_Et);
	
	long iEv = 0; 
	int tau = 0;
  
	for (int i = 2; i < argc; i++){ 

        	//Open a stream connected to an event file:
        	ifstream ifs(argv[i]);
        
        	//Create the Reader object:
        	LHEF::Reader reader(ifs);
        
        	//Now loop over all the events     
        	while ( reader.readEvent() ){

			//Count the events		
			iEv++;
		    
			//TlorentVector of electrons, muons and neutrinos
			TLorentzVector ele_mom, n_mom, mu_mom; 

			//TlorentVector of jets from quarks beauty (b1) and antibeauty (b2)
			TLorentzVector jb1_mom, jb2_mom, bb_mom;

			//TlorentVector of the other jets
			TLorentzVector j1_mom, j2_mom, jj_mom; 

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
						ele_E = momentum.E(); //Energy
						ele_Et = momentum.Et(); //Trasversal Energy

					}

					//Muons and antimuons
					if( abs(ID) == 13){  

						mu_mom = momentum;

						mu_eta = momentum.Eta();
						mu_pt = momentum.Pt(); 
						mu_E = momentum.E(); 
						mu_Et = momentum.Et(); 

					}

					//Checking whether tau and antitau are producted: these events are not considered
					if (abs(ID) == 15){ tau = 1;}

					//Neutrinos (no mass)
					if ( abs(ID) == 12 || abs(ID) == 14 ){

						n_mom = momentum;

						n_eta = momentum.Eta();
						n_pt = momentum.Pt(); 
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

			//Mass, Pt and Et of the two jets from quarks beauty 
			bb_mom = jb1_mom + jb2_mom;
          
			mbb = ( jb1_mom + jb2_mom ).M();
			bb_pt = ( jb1_mom + jb2_mom ).Pt();
			bb_Et = ( jb1_mom + jb2_mom ).Et();  
		
			//The j jet are saved looking at the element of the jet vector   
			if(jets.size() > 0 && jets.size() < 3 && tau == 0){      

				for (int i=0; i<jets.size(); i += 2){

					j1_mom = momenta[jets[i]];
					j2_mom = momenta[jets[i+1]];

					j1_pt = j1_mom.Pt();
		    			j1_eta = j1_mom.Eta();

		   			j2_pt = j2_mom.Pt();
		    			j2_eta = j2_mom.Eta();

					//Mass, Pt and Et of the two jets from the other quarks
					jj_mom = j1_mom + j2_mom;

					mjj = (j1_mom + j2_mom).M();
           				jj_pt = (j1_mom + j2_mom).Pt();
					jj_Et = (j1_mom + j2_mom).Et();

					//The neutrinos are not measured: saving MET
					MET = n_pt;
						
					//Saving HT
					HT = jb1_pt + jb2_pt + j1_pt + j2_pt + mu_pt + ele_pt;
					
				}//End of jet construction 

			tree->Fill(); 
		
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
