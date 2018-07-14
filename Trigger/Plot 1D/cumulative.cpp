
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

int main(int argc, char** argv){

	TApplication * Grafica = new TApplication("App", 0, 0);
	TCanvas * c = new TCanvas;

	if (argc < 1){
		cout << "Usage: " << argv[0] << " q1 q2 q3 ... " << endl;
		return 1;
	}
	
	int nbin, max, min;
	
	cout << "Inserisci il numero di bin: " << endl;
	cin >> nbin;

	cout << "Inserisci il punto in cui far iniziare l'istogramma: " << endl;
	cin >> min;

	cout << "Inseirsci il punto in cui far terminare l'istogramma: " << endl;
	cin >> max; 
	
	//Apro il file in cui sono contenuti gli eventi di segnale
	TFile * input = TFile::Open( "unweighted_events.root" );
 
	TH1 * hs = new  TH1F("histogram", "title", nbin, min, max);
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
	hs->SetLineColor(kBlack);
	hs->GetXaxis()->SetTitleOffset(1.2);
	hs->GetYaxis()->SetTitle("Counts");

	//Get the cumulative of hs
	TH1* hc = hs->GetCumulative();

	double integral = hs->Integral();

	TH1F * h1_efficiency = new  TH1F("h1_efficiency", "title", nbin, min, max);
	TH1F * h2_efficiency = new  TH1F("h2_efficiency", "title", nbin, min, max);

	int N = hc->GetNbinsX();
	double v[N];

	for (int i = 0; i < N; ++i) {
	
		v[i] = hc->GetBinContent(i)/integral;
		h1_efficiency -> SetBinContent(i, v[i]);

	} 

	for (int j = 0; j < N; ++j) {
	
		v[j] = 1 - hc->GetBinContent(j)/integral;
		h2_efficiency -> SetBinContent(j, v[j]);

	}

	h1_efficiency->SetLineColor(kBlue);
	h1_efficiency->GetXaxis()->SetTitleOffset(1.2);
	h1_efficiency->GetYaxis()->SetTitle("Efficiency");

	h2_efficiency->SetLineColor(kGreen);
	h2_efficiency->GetXaxis()->SetTitleOffset(1.2);
	h2_efficiency->GetYaxis()->SetTitle("Efficiency");

	//Stampo a terminale i valori dei due istogrammi in corrispondenza della soglia
	int threshold;
	double counts, efficiency;

	cout << "Inserisci il valore della soglia: " << endl;
	cin >> threshold;

	efficiency = h1_efficiency -> GetBinContent(threshold);
	counts = hs -> GetBinContent(threshold);

	//L'efficienza segue una distribuzione binomiale
	double sigma = sqrt( efficiency * (1-efficiency) / 1000000 );  
	
	cout << "I conteggi in corrispondenza della soglia sono: " << counts << endl;
	cout << "Il valore dell'efficienza (x < threshold) in corrispondenza della soglia vale: " 
	<< efficiency << " +/- " << sigma << endl;
	cout << "Il valore dell'efficienza (x > threshold) in corrispondenza della soglia vale: " 
	<< h2_efficiency -> GetBinContent(threshold) << " +/- " << sigma << endl;

	TLegend *legend = new TLegend(0.8,0.2,0.98,0.38);

	//Draw histogram together with its cumulative distribution
	c->Divide(1,2);
	c->cd(1);
	hs->Draw();
	c->cd(2);
	h1_efficiency->Draw();
	h2_efficiency->Draw("same");
	h1_efficiency->SetStats(kFALSE);
	h2_efficiency->SetStats(kFALSE);
	legend->AddEntry(h1_efficiency,"x < threshold", "l");
	legend->AddEntry(h2_efficiency,"x > threshold", "l");
	legend->Draw("SAME");
	c->Update();

	Grafica->Run();
	
	return 0;
}
