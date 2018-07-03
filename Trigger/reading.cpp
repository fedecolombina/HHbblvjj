//Il programma converte i file .lhe in file .root

// c++ -o reading reading.cpp `root-config --cflags --glibs`

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

	//Mi metto nella cartella dove si trova il file .lhe:
	const char * dirname;
	dirname = "/media/sf_Condivisa";
	chdir(dirname);

	//Il programma richiede che vengano inseriti a terminale il nome del file .root in cui 
	//si desidera vengano salvati i dati e il nome del file .lhe da convertire	
	if (argc < 3){
		cout << "Usage: " << argv[0] << " output.root file1.lhe.." << endl;
		return 1;
	}

	char* rootfile = argv[1];     
	TFile output( rootfile, "RECREATE");

	TTree* tree = new TTree("tree", "Background events");
    
	double lep_eta, lep_pt, lep_E, lep_Et, lep_phi;
	double n_eta, n_pt;
	double j1_eta, j1_pt, j1_phi, j2_eta, j2_pt, j2_phi;
	double b1_eta, b1_pt, b1_phi, b2_eta, b2_pt, b2_phi;
	double mww, mjj, mbb; 
	double ww_pt, j_pt, bb_pt;
	double j_Et, bb_Et, ww_Et;
	double Ht, Htnu, Ptm;
	double deltar_ljj, deltar_bbljj, deltar_bb;
	double deltaphi_ljj, deltaphi_bbljj, deltaphi_bb;
    
	TH1F *q1 = new TH1F("q1", "PGID of j1", 20, -10, 10);
	TH1F *q2 = new TH1F("q2", "PGID of j2", 20, -10, 10);   
	TH1F* l1 = new TH1F("l1", "PGID of lepton", 40, -20, 20);
	
	tree->Branch("lep_pt", &lep_pt);
	tree->Branch("lep_E", &lep_E);
	tree->Branch("lep_Et", &lep_Et);
	tree->Branch("n_pt", &n_pt);
	tree->Branch("j1_pt", &j1_pt);
	tree->Branch("j2_pt", &j2_pt);	
	tree->Branch("b1_pt", &b1_pt);
	tree->Branch("b2_pt", &b2_pt);

	tree->Branch("mjj", &mjj);
	tree->Branch("j_pt", &j_pt);
	tree->Branch("j_Et", &j_Et);
	tree->Branch("mww", &mww);
	tree->Branch("ww_pt", &ww_pt);
	tree->Branch("ww_Et", &ww_Et);
	tree->Branch("mbb", &mbb);
	tree->Branch("bb_pt", &bb_pt);
	tree->Branch("bb_Et", &bb_Et);
	
	tree->Branch("Ht", &Ht);
	tree->Branch("Htnu", &Htnu);
	tree->Branch("Ptm", &Ptm);

	tree->Branch("deltaphi_bb", &deltaphi_bb);
	tree->Branch("deltaphi_ljj", &deltaphi_ljj);
	tree->Branch("deltaphi_bbljj", &deltaphi_bbljj);

	tree->Branch("deltar_bb", &deltar_bb);
	tree->Branch("deltar_ljj", &deltar_ljj);
	tree->Branch("deltar_bbljj", &deltar_bbljj);
	
	long iEv = 0; //indice dell'evento
	int tau = 0; //variabile che controlla se vengono prodotte tau
  
	for (int i = 2; i < argc; i++){ //in questo caso ho solo i=3, il file .lhe

        	//Open a stream connected to an event file:
        	ifstream ifs(argv[i]);
        
        	//Create the Reader object:
        	LHEF::Reader reader(ifs);
        
        	//Now loop over all the events     
        	while ( reader.readEvent() ){
		
			iEv++;
            
			//vengono creati dei vettori in cui verranno sakvati.. 
	 		vector<int> charged_leptons; //elettroni e muoni
			vector<int> neutrinos; //neutrini
			vector<int> leptons; //leptoni (elettroni, muoni e neutrini)
			vector<int> quarks_jet; 
			vector<int> quarks_b; //quark beauty e antibeauty
			vector<int> jets; 
		    
			//TlorentVector of j and bosons w
			//ho solo w1, per come è fatto il programma non mi serve introdurre w2 
			//(quella che produce i leptoni) 
			TLorentzVector jet_j1_mom, jet_j2_mom, w1_mom;
			//TlorentVector of lepton and neutrino
			TLorentzVector lep_mom, nu_mom; 
			//TlorentVector of quarks beauty (b1) and antibeauty (b2)
			//bb indica la somma di b1 e b2
			TLorentzVector b1_mom, b2_mom, bb_mom; 
		    
			//Save quadrimomentum of all particles mapped with position in the event
			map<int, TLorentzVector> momenta;
		    
			//Save eta and pt of particles            
			//loop over particles in the event
			for (int iPart = 0 ; iPart < reader.hepeup.IDUP.size (); iPart++){

				//Saving info of final state particles only
				if (reader.hepeup.ISTUP.at (iPart) == 1){

					int ID = reader.hepeup.IDUP.at(iPart);

					//Il contenuto dei quadrivettori nel file .lhe viene trasferito
					//in un TLorentzVector:
					//PUP --> quadrimomentum                   
	 				TLorentzVector momentum(
					reader.hepeup.PUP.at (iPart).at (0), //px
					reader.hepeup.PUP.at (iPart).at (1), //py
					reader.hepeup.PUP.at (iPart).at (2), //pz
					reader.hepeup.PUP.at (iPart).at (3) //E
					);
	      
					//Il contenuto del TLorentzVector viene inserito nella map definita prima
					momenta[iPart] =  momentum;

					// eletrons, positrons, muon, antimuon 
					// Events with a Tau or an antitau are not considered
					if( abs(ID) == 11 || abs(ID) == 13 ){  

						// la funzione push_back inserisce un elemento in fondo al vettore
						charged_leptons.push_back(iPart);
						leptons.push_back(iPart);

						//ho il TLorentzVector di un leptone:						
						lep_mom = momentum;

						//si assegnano delle variabili alle informazioni contenute nel 
						//TLorentzVector:
						lep_eta = momentum.Eta();
						lep_pt = momentum.Pt(); //componente trasversale del momento
						lep_E = momentum.E(); //energia
						lep_Et = momentum.Et(); //componente trasversale dell'energia
						lep_phi = momentum.Phi();

						//viene creato un istogramma che confronterà il numero di muoni col 
						//numero di leptoni
						l1->Fill(ID);
					}

					// controllo se vengono prodotte tau: in caso affermativo l'evento non dovrà 
					// essere considerato
					if (abs(ID) == 15){ tau = 1;}

					// neutrinos
					if ( abs(ID) == 12 || abs(ID) == 14 ){  
		               
						neutrinos.push_back(iPart);
						leptons.push_back(iPart);

						nu_mom = momentum;

						n_eta = momentum.Eta();
						n_pt = momentum.Pt(); //il neutrino ha m=0
					}
			
					// quark beauty 
					if ( ID == 5 ){  
		                
						quarks_b.push_back(iPart);

						b1_mom = momentum;

						b1_eta = momentum.Eta();
						b1_pt = momentum.Pt();
						b1_phi = momentum.Phi();
					}
				
					// quark antibeauty 
					if ( ID == -5 ){  
					
						quarks_b.push_back(iPart);

						b2_mom = momentum;

						b2_eta = momentum.Eta();
						b2_pt = momentum.Pt();
						b2_phi = momentum.Phi();
					}
		            
					// Other quarks in final state are from the jet 
					if ( abs(ID) < 7 && abs(ID) != 5 ){  
				
						quarks_jet.push_back(iPart);
						jets.push_back(iPart);
					}   
				}// info of final state particles
			}// end of loop over particles	
			
			// mass and pt of the two beauty           
			mbb = ( b1_mom + b2_mom ).M(); //massa invariante 
			bb_pt = ( b1_mom + b2_mom ).Pt();
			bb_Et = ( b1_mom + b2_mom ).Et();    			
		
			// The j jet are saved looking at the element of the jet vector.
			// Events with a DOUBLE JETS or NO JET are not saved..
			// Quindi posso avere massimo 2 jet. Tau deve essere uguale a zero altrimenti
			// non considero neanche l'evento.   
			if(jets.size() > 0 && jets.size() < 3 && tau == 0){      

				for (int i=0; i<jets.size(); i += 2){
		               	
					jet_j1_mom = momenta[jets[i]];
					jet_j2_mom = momenta[jets[i+1]];

					j1_pt = jet_j1_mom.Pt();
		    			j1_eta = jet_j1_mom.Eta();
					j1_phi = jet_j1_mom.Phi();

		   			j2_pt = jet_j2_mom.Pt();
		    			j2_eta = jet_j2_mom.Eta();
					j2_phi = jet_j2_mom.Phi();
						
					//valori totali dei due jet
					mjj = (jet_j1_mom + jet_j2_mom).M();
		   			j_pt = (jet_j1_mom + jet_j2_mom).Pt();
					j_Et = (jet_j1_mom + jet_j2_mom).Et();

					//bosons mass and momentum, the neutrinos are not measured  
					//perchè i due jet sono prodotti da w e i leptoni da un'altra w
		    			mww = ( jet_j1_mom + jet_j2_mom + lep_mom ).M();
					ww_pt = ( jet_j1_mom + jet_j2_mom + lep_mom ).Pt();

					//flavour of jets are saved in an histogram
					q1->Fill(reader.hepeup.IDUP.at(jets[i]));
		    			q2->Fill(reader.hepeup.IDUP.at(jets[i+1]));
						
					// saving Ht, Htnu, Ptm
					Ht = b1_pt + b2_pt + j1_pt + j2_pt + lep_pt;
					Htnu = b1_pt + b2_pt + j1_pt + j2_pt + lep_pt + n_pt; //somma scalare (>0)  
					Ptm = ( b1_mom + b2_mom + jet_j1_mom + jet_j2_mom + lep_mom ).Pt(); // somma vettoriale

					//Angolar variables 
					deltaphi_bb = b1_mom.DeltaPhi( b2_mom );
					deltar_bb = b1_mom.DeltaR( b2_mom );
					deltaphi_ljj = lep_mom.DeltaPhi( jet_j1_mom + jet_j2_mom );
					deltar_ljj = lep_mom.DeltaR( jet_j1_mom + jet_j2_mom );

					bb_mom = b1_mom + b2_mom;
					//la somma dei due jet mi darà w
					w1_mom = jet_j1_mom + jet_j2_mom; 

					deltaphi_bbljj = bb_mom.DeltaPhi( lep_mom + w1_mom);
					deltar_bbljj = bb_mom.DeltaR( lep_mom + w1_mom);
					
					tree->Fill();
				}//end of jet construction 
		
			//Altrimenti l'evento non viene salvato 	
			} else { 
				tau = 0; 
			}     
		}//end of loop over events        
        
		ifs.close();
	}//end of loop over files

	l1->Write();
 	q1->Write();
	q2->Write();

	tree->Write();

	output.Close();    

	return 0;
}
