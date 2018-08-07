//Il programma restituisce l'efficienza di un trigger in cui viene applicato un taglio sul momento trasverso di un jet.
//E' previsto solo il caso x > threshold; viene mostrato l'istogramma dei conteggi e quello dell'efficienza.

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
	TFile * input = TFile::Open( "TTrees.root" );

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

	//Lettura dei tree contenuti nel file
	TTreeReader ele_reader("ele_tree", input);
	TTreeReader mu_reader("mu_tree", input);

	//Accesso alle variabili contenute nei ttree	
	TTreeReaderValue<double>  et_first_jet( ele_reader, "j1_pt");
	TTreeReaderValue<double>  et_second_jet( ele_reader, "j2_pt");
	TTreeReaderValue<double>  et_third_jet( ele_reader, "jb1_pt");
	TTreeReaderValue<double>  et_fourth_jet( ele_reader, "jb2_pt");

	TTreeReaderValue<double>  mt_first_jet( mu_reader, "j1_pt");
	TTreeReaderValue<double>  mt_second_jet( mu_reader, "j2_pt");
	TTreeReaderValue<double>  mt_third_jet( mu_reader, "jb1_pt");
	TTreeReaderValue<double>  mt_fourth_jet( mu_reader, "jb2_pt");

	//Loop over the events
	while (mu_reader.Next()) {

		//Ordino i pt dal più piccolo al più grande
		vector<double> mt_jets {*mt_first_jet, *mt_second_jet, *mt_third_jet, *mt_fourth_jet};
		sort(mt_jets.begin(), mt_jets.end());

		hs->Fill(mt_jets[3]);

	}//end of loop over the events

	while (ele_reader.Next()) {

		//Ordino i pt dal più piccolo al più grande
		vector<double> et_jets {*et_first_jet, *et_second_jet, *et_third_jet, *et_fourth_jet};
		sort(et_jets.begin(), et_jets.end());

		hs->Fill(et_jets[3]);

	}//end of loop over the events

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

	double efficiency = h_efficiency->GetBinContent(cut);

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

