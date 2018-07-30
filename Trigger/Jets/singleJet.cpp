
//c++ singleJet.cpp -o singleJet `root-config --cflags --glibs`

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

int main (int argc, char** argv) {

	TApplication * Grafica = new TApplication("App", 0, 0);
	TCanvas * c = new TCanvas;
	
	//Apro il file in cui sono contenuti gli eventi di segnale
	TFile * input = TFile::Open( "unweighted_events.root" );

	int nbin, max, min;
	
	cout << "Inserisci il numero di bin: " << endl;
	cin >> nbin;

	cout << "Inserisci il punto in cui far iniziare l'istogramma: " << endl;
	cin >> min;

	cout << "Inseirsci il punto in cui far terminare l'istogramma: " << endl;
	cin >> max; 

	TH1 * hs = new  TH1F("histogram", "title", nbin, min, max);

	double cut;

	cout << "Inserisci il valore del taglio: " << endl;
	cin >> cut;

	//Lettura del tree contenuto nel file
	TTreeReader reader("tree", input);

	//Converto var[0] in un puntatore di tipo char come richiesto da TTreeReaderValue.h	
	TTreeReaderArray<double>  first_jet( reader, "j1_pt");
	TTreeReaderArray<double>  second_jet( reader, "j2_pt");
	TTreeReaderArray<double>  third_jet( reader, "jb1_pt");
	TTreeReaderArray<double>  fourth_jet( reader, "jb2_pt");
	
	double survivors = 0;

	while (reader.Next()) {

		//Loop over the events
		for (int iVar = 0; iVar < first_jet.GetSize(); ++iVar) {

			//Ordino i pt dal più piccolo al più grande
			vector<double> jets {first_jet[iVar], second_jet[iVar], third_jet[iVar], fourth_jet[iVar] };
			sort(jets.begin(), jets.end());

			hs->Fill(jets[3]);

			//Richiedo che il pt più grande passi il cut più grande e così via
			if (jets[3] > cut) {
				survivors += 1;

			}

		}//end of loop over the events

	 }//end of while

	hs->SetFillStyle(3004);
	hs->SetFillColor(kRed);
	hs->SetLineColor(kBlack);
	hs->GetXaxis()->SetTitleOffset(1.2);
	hs->GetYaxis()->SetTitle("Counts");
	hs->GetXaxis()->SetTitle("p_{T}");

	//Get the cumulative of hs
	TH1* hc = hs->GetCumulative();

	double integral = hs->Integral();

	TH1F * h_efficiency = new  TH1F("h_efficiency", "title", nbin, min, max);

	int N = hc->GetNbinsX();
	double v[N];

	for (int i = 0; i < N; ++i) {
	
		v[i] = 1 - hc->GetBinContent(i)/integral;
		h_efficiency -> SetBinContent(i, v[i]);

	}

	h_efficiency->SetLineColor(kBlue);
	h_efficiency->GetXaxis()->SetTitleOffset(1.2);
	h_efficiency->GetYaxis()->SetTitle("Efficiency");
	h_efficiency->GetXaxis()->SetTitle("p_{T}");

	double efficiency = survivors/1000000;

	cout << "L'efficienza vale: " << efficiency << endl;

	//Draw histogram together with its cumulative distribution
	c->Divide(1,2);
	c->cd(1);
	hs->Draw();
	c->cd(2);
	h_efficiency->Draw();
	h_efficiency->SetStats(kFALSE);
	c->Update();

	Grafica->Run();

	return 0;
}

