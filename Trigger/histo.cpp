
//Le grandezze di cui fare i grafici vanno passate sulla riga di comando.
//Per vedere i nomi con cui sono salvate nei TTree, consultare il file di lettura reading.cpp
 
// c++ -o histo histo.cpp `root-config --cflags --glibs`

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

using namespace std;

class TVirtualPad;

int main(int argc, char** argv){

	if (argc < 1){
		cout << "Usage: " << argv[0] << " q1 q2 q3 ... " << endl;
		return 1;
	}

	TApplication * Grafica = new TApplication("App", 0, 0);
	
	//Apro il file in cui sono contenuti gli eventi di segnale
	TFile * input = TFile::Open( "unweighted_events.root" );
 
	TCanvas * c = new TCanvas;
	TH1F * hs = new  TH1F("histo", "title", 1000, -100, 1500);
	string * var = new string; 

	//Salvo la variabile passata a terminale in una stringa	
	var[0] = argv[1];

	hs->SetFillStyle(3004);
	hs->SetFillColor(kAzure + 2);
	hs->SetLineColor(kBlack);

	//Lettura del tree contenuto nel file
	TTreeReader reader("tree", input);

	//Converto var[0] in un puntatore di tipo char come richiesto da TTreeReaderValue.h	
	TTreeReaderValue<double>  variable( reader, var[0].c_str() );
	
	//Scrittura dell'istogramma dai dati contenuti nel TTree
	while ( reader.Next() ) {
		hs->Fill(* variable);			
	}

	hs -> Draw();

	gPad->SetGrid(1,1);
	TLegend *legend1 = new TLegend(0.8,0.2,0.38, 0.7);
	legend1->AddEntry(hs,"Signal", "f");
	legend1->Draw();

	Grafica->Run();
	
	return 0;
}
