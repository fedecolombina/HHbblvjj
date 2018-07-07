
//Le grandezze di cui fare i grafici vanno passate sulla riga di comando.
//Per vedere i nomi con cui sono salvate nei TTree, consultare il file di lettura reading.cpp
 
// c++ -o acumulative acumulative.cpp `root-config --cflags --glibs`

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

	TApplication * Grafica = new TApplication("App", 0, 0);
	TCanvas * c = new TCanvas;

	if (argc < 1){
		cout << "Usage: " << argv[0] << " q1 q2 q3 ... " << endl;
		return 1;
	}
	
	//Apro il file in cui sono contenuti gli eventi di segnale
	TFile * input = TFile::Open( "unweighted_events.root" );
 
	TH1 * hs = new  TH1F("histogram", "title", 1000, -100, 1500);
	string * var = new string; 

	//Salvo la variabile passata a terminale in una stringa	
	var[0] = argv[1];

	//Lettura del tree contenuto nel file
	TTreeReader reader("tree", input);

	//Converto var[0] in un puntatore di tipo char come richiesto da TTreeReaderValue.h	
	TTreeReaderValue<double>  variable( reader, var[0].c_str() );
	
	//Scrittura dell'istogramma dai dati contenuti nel TTree
	while ( reader.Next() ) {
		hs->Fill(* variable);			
	}

	hs->SetFillStyle(3004);
	hs->SetFillColor(kRed);


	//Get the cumulative of hs
	TH1* hc = hs->GetCumulative();

	//Check that hc has the "right" contents
	double integral = hs->Integral();

	TH1F * h_efficiency = new  TH1F("histo", "title", 1000, -100, 1500);

	int N = hc->GetNbinsX();
	double v[N];

	for (int j = 0; j < N; ++j) {
	
		v[j] = 1 - hc->GetBinContent(j)/integral;
		h_efficiency -> SetBinContent(j, v[j]);

	} 

	h_efficiency->SetFillStyle(3004);
	h_efficiency->SetFillColor(kBlue);

	//Draw histogram together with its cumulative distribution
  
	c->Divide(1,2);
	c->cd(1);
	hs->Draw();
	c->cd(2);
	h_efficiency->Draw();
	c->Update();

	Grafica->Run();
	
	return 0;
}
