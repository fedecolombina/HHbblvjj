
//Le grandezze di cui fare i grafici vanno passate sulla riga di comando.
//Per vedere i nomi con cui sono salvate nei TTree, consultare il file di lettura reading.cpp
 
// c++ -o cumulative cumulative.cpp `root-config --cflags --glibs`

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
	TH1 * hs = new  TH1D("histogram", "title", 1000, -100, 1500);
	string * var = new string; 

	//Salvo la variabile passata a terminale in una stringa	
	var[0] = argv[1];

	hs->SetFillStyle(3004);
	hs->SetFillColor(kRed);
	hs->SetLineColor(kBlack);

	//Lettura del tree contenuto nel file
	TTreeReader reader("tree", input);

	//Converto var[0] in un puntatore di tipo char come richiesto da TTreeReaderValue.h	
	TTreeReaderValue<double>  variable( reader, var[0].c_str() );
	
	//Scrittura dell'istogramma dai dati contenuti nel TTree
	while ( reader.Next() ) {
		hs->Fill(* variable);			
	}

	//Get the cumulative of hs
	TH1* hc = hs->GetCumulative();

	//Check that hc has the "right" contents
	double * integral = hs->GetIntegral();
	
	for (int i = 1; i <= hc->GetNbinsX(); ++i) {
 		assert(abs(integral[i] * hs->GetEntries() - hc->GetBinContent(i)) < 5);

	} 

	delete gROOT->FindObject("histogram");

	TH1 * h_efficiency = new  TH1D("histo", "title", 1000, -100, 1500);

	int N = hc->GetNbinsX();
	double v[N];

	for (int j = 0; j < N; ++j) {
	
		v[j] = hc->GetBinContent(j)/integral[j];
		h_efficiency -> Fill(v[j]);

	} 

	h_efficiency->Draw();

	Grafica->Run();
	
	return 0;
}
