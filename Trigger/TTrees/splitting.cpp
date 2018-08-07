//This program divides the TTree written with reading.cpp in two smaller TTrees: the first one contains the events where 
//a muon is produced. The other one is for the electrons. 

//c++ -o splitting splitting.cpp `root-config --cflags --glibs`

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

int main(int argc, char** argv){

	//Insert the file's name to run the program	
	if (argc < 2){
		cout << "Usage: " << argv[0] << " output.root " << endl;
		return 1;
	}

	
	//Opening the file with the events 
	TFile * input = TFile::Open( "unweighted_events.root" );

	//Muons
	double mt_mu_eta, mt_mu_pt;

	//Electrons
	double et_ele_eta, et_ele_pt;

	//Jets from quarks beauty and antibeauty
	double mt_jb1_eta, mt_jb1_pt, mt_jb2_eta, mt_jb2_pt;
	double et_jb1_eta, et_jb1_pt, et_jb2_eta, et_jb2_pt;

	//Jets from any other quark
	double mt_j1_eta, mt_j1_pt, mt_j2_eta, mt_j2_pt;
	double et_j1_eta, et_j1_pt, et_j2_eta, et_j2_pt;

	//Energy released and missing enegy
	double mt_HT, mt_MET;
	double et_HT, et_MET;

	//Average
	double mt_av_max;
	double et_av_max;

	char* rootfile = argv[1];     
	TFile output( rootfile, "RECREATE");
	
	//Creating two different ttrees for muons and electrons
	TTree* mu_tree = new TTree("mu_tree", "mu_Events");
	TTree* ele_tree = new TTree("ele_tree", "ele_Events");

	//Muons
	mu_tree->Branch("mu_pt", &mt_mu_pt);
	mu_tree->Branch("mu_eta", &mt_mu_eta);

	//Electrons
	ele_tree->Branch("ele_pt", &et_ele_pt);
	ele_tree->Branch("ele_eta", &et_ele_eta);
	
	//Jets from quarks beauty and antibeauty
	mu_tree->Branch("jb1_pt", &mt_jb1_pt);
	mu_tree->Branch("jb1_eta", &mt_jb1_eta);
	mu_tree->Branch("jb2_pt", &mt_jb2_pt);
	mu_tree->Branch("jb2_eta", &mt_jb2_eta);

	ele_tree->Branch("jb1_pt", &et_jb1_pt);
	ele_tree->Branch("jb1_eta", &et_jb1_eta);
	ele_tree->Branch("jb2_pt", &et_jb2_pt);
	ele_tree->Branch("jb2_eta", &et_jb2_eta);	

	//Jets from any other quark
	mu_tree->Branch("j1_pt", &mt_j1_pt);
	mu_tree->Branch("j1_eta", &mt_j1_eta);
	mu_tree->Branch("j2_pt", &mt_j2_pt);
	mu_tree->Branch("j2_eta", &mt_j2_eta);

	ele_tree->Branch("j1_pt", &et_j1_pt);
	ele_tree->Branch("j1_eta", &et_j1_eta);
	ele_tree->Branch("j2_pt", &et_j2_pt);
	ele_tree->Branch("j2_eta", &et_j2_eta);

	//Energy released and missing enegy	
	mu_tree->Branch("HT", &mt_HT);
	mu_tree->Branch("MET", &mt_MET);

	ele_tree->Branch("HT", &et_HT);
	ele_tree->Branch("MET", &et_MET);

	//Average
	mu_tree->Branch("av_max", &mt_av_max);

	ele_tree->Branch("av_max", &et_av_max);

	//Reading the tree 
	TTreeReader reader("tree", input);
	
	//Creating an array for every variable:
	//Leptons
	TTreeReaderValue<double>  mu_pt_value( reader, "mu_pt" );
	TTreeReaderValue<double>  ele_pt_value( reader, "ele_pt" );
	TTreeReaderValue<double>  mu_eta_value( reader, "mu_eta" );
	TTreeReaderValue<double>  ele_eta_value( reader, "ele_eta" );

	//Jets
	TTreeReaderValue<double>  jb1_eta_value( reader, "jb1_eta" );
	TTreeReaderValue<double>  jb1_pt_value( reader, "jb1_pt" );
	TTreeReaderValue<double>  jb2_eta_value( reader, "jb2_eta" );
	TTreeReaderValue<double>  jb2_pt_value( reader, "jb2_pt" );

	TTreeReaderValue<double>  j1_eta_value( reader, "j1_eta" );
	TTreeReaderValue<double>  j1_pt_value( reader, "j1_pt" );
	TTreeReaderValue<double>  j2_eta_value( reader, "j2_eta" );
	TTreeReaderValue<double>  j2_pt_value( reader, "j2_pt" );

	//Energy released and missing enegy
	TTreeReaderValue<double>  HT_value( reader, "HT" );
	TTreeReaderValue<double>  MET_value( reader, "MET" );

	//Average
	TTreeReaderValue<double>  av_max_value( reader, "av_max" );

	//Loop over the events
	while (reader.Next()) {
			
		//First case: an electron has been produced
		if (*mu_pt_value == 0) {

				//Electrons
				et_ele_pt = *ele_pt_value; 
				et_ele_eta = *ele_eta_value;

				//Jets
				et_jb1_eta = *jb1_eta_value; 
				et_jb1_pt = *jb1_pt_value; 
				et_jb2_eta = *jb2_eta_value;
				et_jb2_pt = *jb2_pt_value;

				et_j1_eta = *j1_eta_value;
				et_j1_pt = *j1_pt_value;
				et_j2_eta = *j2_eta_value;
				et_j2_pt = *j2_pt_value; 

				//Energy released and missing enegy
				et_HT = *HT_value;
				et_MET = *MET_value;

				//Average
				et_av_max = *av_max_value; 

				//Now fill the tree
				ele_tree->Fill();
		}

		//Second case: a muon has been produced
		if (*ele_pt_value == 0) {

				//Electrons
				mt_mu_pt = *mu_pt_value; 
				mt_mu_eta = *mu_eta_value;

				//Jets
				mt_jb1_eta = *jb1_eta_value; 
				mt_jb1_pt = *jb1_pt_value; 
				mt_jb2_eta = *jb2_eta_value;
				mt_jb2_pt = *jb2_pt_value;

				mt_j1_eta = *j1_eta_value;
				mt_j1_pt = *j1_pt_value;
				mt_j2_eta = *j2_eta_value;
				mt_j2_pt = *j2_pt_value; 

				//Energy released and missing enegy
				mt_HT = *HT_value;
				mt_MET = *MET_value;

				//Average
				mt_av_max = *av_max_value; 

				//Now fill the tree
				mu_tree->Fill();
		}


	}//end of loop over the events

	mu_tree->Write();
	ele_tree->Write();

	output.Close();   

	return 0;
}

					


