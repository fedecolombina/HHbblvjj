//I valori dei quattro tagli devono essere inseriti a terminale quando richiesto dal programma.
//Una volta applicati i tagli viene stampato a terminale il valore dell'efficienza, non vengono restituiti grafici.
//Il programma è stato costruito considerando quattro tagli sui jet, quindi tagli di tipo x > soglia.

//c++ fourJets.cpp -o fourJets `root-config --cflags --glibs`

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
	
	//Apro il file in cui sono contenuti gli eventi di segnale
	TFile * input = TFile::Open( "unweighted_events.root" );

	//Salvo le variabili passate a terminale in una stringa	
	double first_cut, second_cut, third_cut, fourth_cut;

	cout << "Inserisci il valore del primo taglio: " << endl;
	cin >> first_cut;

	cout << "Inserisci il valore del secondo taglio: " << endl;
	cin >> second_cut;

	cout << "Inserisci il valore del terzo taglio: " << endl;
	cin >> third_cut;

	cout << "Inserisci il valore del quarto taglio: " << endl;
	cin >> fourth_cut;

	//Metto le variabili passate a terminale in un vettore e lo ordino dal più piccolo al più grande
	vector<double> cuts {first_cut, second_cut, third_cut, fourth_cut};
	sort(cuts.begin(), cuts.end());	

	//Lettura del tree contenuto nel file
	TTreeReader reader("tree", input);

	//Converto var[0] in un puntatore di tipo char come richiesto da TTreeReaderValue.h	
	TTreeReaderArray<double>  first_jet( reader, "j1_pt");
	TTreeReaderArray<double>  second_jet( reader, "j2_pt");
	TTreeReaderArray<double>  third_jet( reader, "jb1_pt");
	TTreeReaderArray<double>  fourth_jet( reader, "jb2_pt");
	
	double first_survivors = 0;
	double second_survivors = 0;
	double third_survivors = 0;
	double fourth_survivors = 0;

	while (reader.Next()) {

		//Loop over the events
		for (int iEv = 0; iEv < first_jet.GetSize(); ++iEv) {

			//Applico il taglio più energetico
			if (first_jet[iEv] > cuts[3]) {
				first_survivors += 1;
			}

			if (second_jet[iEv] > cuts[3]) {
				second_survivors += 1;
			}

			if (third_jet[iEv] > cuts[3]) {
				third_survivors += 1;
			}

			if (fourth_jet[iEv] > cuts[3]) {
				fourth_survivors += 1;
			}

		}//end of loop over the events

	 }//end of while

	vector<double> surv {first_survivors, second_survivors, third_survivors, fourth_survivors};

	auto biggest = max_element(begin(surv), end(surv));
	double v_max = *biggest;

	double efficiency;

	//first if
	if (v_max == first_survivors) {
		efficiency = (v_max)/(1000000);
		
		second_survivors = 0;
		third_survivors = 0;
		fourth_survivors = 0;

		//Second cut
		while (reader.Next()) {

			//Loop over the events
			for (int iEv = 0; iEv < first_jet.GetSize(); ++iEv) {

				if (second_jet[iEv] > cuts[2] && second_jet[iEv] > cuts[3]) {
					second_survivors += 1;
				}

				if (third_jet[iEv] > cuts[2] && third_jet[iEv] > cuts[3]) {
					third_survivors += 1;
				}

				if (fourth_jet[iEv] > cuts[2] && fourth_jet[iEv] > cuts[3]) {
					fourth_survivors += 1;
				}

			}//end of loop over the events

		}//end of while	
	
		vector<double> s {second_survivors, third_survivors, fourth_survivors};

		auto big = max_element(begin(s), end(s));
		v_max = *big;	

		//second if
		if (v_max == second_survivors) {	

			efficiency *= (v_max)/(1000000);
	
			third_survivors = 0;
			fourth_survivors = 0;

			//Third cut
			while (reader.Next()) {

				//Loop over the events
				for (int iEv = 0; iEv < first_jet.GetSize(); ++iEv) {

					if (third_jet[iEv] > cuts[1] && third_jet[iEv] > cuts[2] && third_jet[iEv] > cuts[3]) {
						third_survivors += 1;
					}

					if (fourth_jet[iEv] > cuts[1] && fourth_jet[iEv] > cuts[2] && fourth_jet[iEv] > cuts[3]) {
						fourth_survivors += 1;
					}

				}//end of loop over the events

			}//end of while	

			//third if	
			if (third_survivors > fourth_survivors) {

				efficiency *= (third_survivors)/(1000000);
	
				fourth_survivors = 0;

				//Last cut
				while (reader.Next()) {

					//Loop over the events
					for (int iEv = 0; iEv < first_jet.GetSize(); ++iEv) {

						if (fourth_jet[iEv] > cuts[0] && fourth_jet[iEv] > cuts[1] && fourth_jet[iEv] > cuts[2] 						&& fourth_jet[iEv] > cuts[3]) {

							fourth_survivors += 1;
						}

					}//end of loop over the events

				}//end of while

			efficiency *= (fourth_survivors)/1000000;
	
			}//end of third if

			//third if
			if (third_survivors < fourth_survivors) {

				efficiency *= (fourth_survivors)/(1000000);
	
				third_survivors = 0;

				//Last cut
				while (reader.Next()) {

					//Loop over the events
					for (int iEv = 0; iEv < first_jet.GetSize(); ++iEv) {

						if (third_jet[iEv] > cuts[0] && third_jet[iEv] > cuts[1] && third_jet[iEv] > cuts[2] 							&& third_jet[iEv] > cuts[3]) {

							third_survivors += 1;
						}

					}//end of loop over the events

				}//end of while

			efficiency *= (third_survivors)/1000000;
	
			}//end of third if

		}//end of second if

		//second if
		if (v_max == third_survivors) {	

			efficiency *= (v_max)/(1000000);
	
			second_survivors = 0;
			fourth_survivors = 0;

			//Third cut
			while (reader.Next()) {

				//Loop over the events
				for (int iEv = 0; iEv < first_jet.GetSize(); ++iEv) {

					if (second_jet[iEv] > cuts[1] && second_jet[iEv] > cuts[2] && second_jet[iEv] > cuts[3]) {
						second_survivors += 1;
					}

					if (fourth_jet[iEv] > cuts[1] && fourth_jet[iEv] > cuts[2] && fourth_jet[iEv] > cuts[3]) {
						fourth_survivors += 1;
					}

				}//end of loop over the events

			}//end of while	

			//third if	
			if (second_survivors > fourth_survivors) {

				efficiency *= (second_survivors)/(1000000);
	
				fourth_survivors = 0;

				//Last cut
				while (reader.Next()) {

					//Loop over the events
					for (int iEv = 0; iEv < first_jet.GetSize(); ++iEv) {

						if (fourth_jet[iEv] > cuts[0] && fourth_jet[iEv] > cuts[1] && fourth_jet[iEv] > cuts[2] 						&& fourth_jet[iEv] > cuts[3]) {

							fourth_survivors += 1;
						}

					}//end of loop over the events

				}//end of while

			efficiency *= (fourth_survivors)/1000000;
	
			}//end of third if

			//third if
			if (second_survivors < fourth_survivors) {

				efficiency *= (fourth_survivors)/(1000000);
	
				second_survivors = 0;

				//Last cut
				while (reader.Next()) {

					//Loop over the events
					for (int iEv = 0; iEv < first_jet.GetSize(); ++iEv) {

						if (second_jet[iEv] > cuts[0] && second_jet[iEv] > cuts[1] && second_jet[iEv] > cuts[2] 						&& second_jet[iEv] > cuts[3]) {

							second_survivors += 1;
						}

					}//end of loop over the events

				}//end of while

			efficiency *= (second_survivors)/1000000;
	
			}//end of third if

		}//end of second if

		//second if
		if (v_max == fourth_survivors) {	

			efficiency *= (v_max)/(1000000);
	
			second_survivors = 0;
			third_survivors = 0;

			//Third cut
			while (reader.Next()) {

				//Loop over the events
				for (int iEv = 0; iEv < first_jet.GetSize(); ++iEv) {

					if (second_jet[iEv] > cuts[1] && second_jet[iEv] > cuts[2] && second_jet[iEv] > cuts[3]) {
						second_survivors += 1;
					}

					if (third_jet[iEv] > cuts[1] && third_jet[iEv] > cuts[2] && third_jet[iEv] > cuts[3]) {
						third_survivors += 1;
					}

				}//end of loop over the events

			}//end of while	

			//third if	
			if (second_survivors > third_survivors) {

				efficiency *= (second_survivors)/(1000000);
	
				third_survivors = 0;

				//Last cut
				while (reader.Next()) {

					//Loop over the events
					for (int iEv = 0; iEv < first_jet.GetSize(); ++iEv) {

						if (third_jet[iEv] > cuts[0] && third_jet[iEv] > cuts[1] && third_jet[iEv] > cuts[2] 							&& third_jet[iEv] > cuts[3]) {

							third_survivors += 1;
						}

					}//end of loop over the events

				}//end of while

			efficiency *= (third_survivors)/1000000;
	
			}//end of third if

			//third if
			if (second_survivors < third_survivors) {

				efficiency *= (third_survivors)/(1000000);
	
				second_survivors = 0;

				//Last cut
				while (reader.Next()) {

					//Loop over the events
					for (int iEv = 0; iEv < first_jet.GetSize(); ++iEv) {

						if (second_jet[iEv] > cuts[0] && second_jet[iEv] > cuts[1] && second_jet[iEv] > cuts[2] 						&& second_jet[iEv] > cuts[3]) {

							second_survivors += 1;
						}

					}//end of loop over the events

				}//end of while

			efficiency *= (second_survivors)/1000000;
	
			}//end of third if

		}//end of second if

	}//end of first if

	//first if
	if (v_max == second_survivors) {
		efficiency = (v_max)/(1000000);
		
		first_survivors = 0;
		third_survivors = 0;
		fourth_survivors = 0;

		//Second cut
		while (reader.Next()) {

			//Loop over the events
			for (int iEv = 0; iEv < first_jet.GetSize(); ++iEv) {

				if (first_jet[iEv] > cuts[2] && first_jet[iEv] > cuts[3]) {
					first_survivors += 1;
				}

				if (third_jet[iEv] > cuts[2] && third_jet[iEv] > cuts[3]) {
					third_survivors += 1;
				}

				if (fourth_jet[iEv] > cuts[2] && fourth_jet[iEv] > cuts[3]) {
					fourth_survivors += 1;
				}

			}//end of loop over the events

		}//end of while	
	
		vector<double> s {first_survivors, third_survivors, fourth_survivors};

		auto big = max_element(begin(s), end(s));
		v_max = *big;	

		//second if
		if (v_max == first_survivors) {	

			efficiency *= (v_max)/(1000000);
	
			third_survivors = 0;
			fourth_survivors = 0;

			//Third cut
			while (reader.Next()) {

				//Loop over the events
				for (int iEv = 0; iEv < first_jet.GetSize(); ++iEv) {

					if (third_jet[iEv] > cuts[1] && third_jet[iEv] > cuts[2] && third_jet[iEv] > cuts[3]) {
						third_survivors += 1;
					}

					if (fourth_jet[iEv] > cuts[1] && fourth_jet[iEv] > cuts[2] && fourth_jet[iEv] > cuts[3]) {
						fourth_survivors += 1;
					}

				}//end of loop over the events

			}//end of while	

			//third if	
			if (third_survivors > fourth_survivors) {

				efficiency *= (third_survivors)/(1000000);
	
				fourth_survivors = 0;

				//Last cut
				while (reader.Next()) {

					//Loop over the events
					for (int iEv = 0; iEv < first_jet.GetSize(); ++iEv) {

						if (fourth_jet[iEv] > cuts[0] && fourth_jet[iEv] > cuts[1] && fourth_jet[iEv] > cuts[2] 						&& fourth_jet[iEv] > cuts[3]) {

							fourth_survivors += 1;
						}

					}//end of loop over the events

				}//end of while

			efficiency *= (fourth_survivors)/1000000;
	
			}//end of third if

			//third if
			if (third_survivors < fourth_survivors) {

				efficiency *= (fourth_survivors)/(1000000);
	
				third_survivors = 0;

				//Last cut
				while (reader.Next()) {

					//Loop over the events
					for (int iEv = 0; iEv < first_jet.GetSize(); ++iEv) {

						if (third_jet[iEv] > cuts[0] && third_jet[iEv] > cuts[1] && third_jet[iEv] > cuts[2] 							&& third_jet[iEv] > cuts[3]) {

							third_survivors += 1;
						}

					}//end of loop over the events

				}//end of while

			efficiency *= (third_survivors)/1000000;
	
			}//end of third if

		}//end of second if

		//second if
		if (v_max == third_survivors) {	

			efficiency *= (v_max)/(1000000);
	
			first_survivors = 0;
			fourth_survivors = 0;

			//Third cut
			while (reader.Next()) {

				//Loop over the events
				for (int iEv = 0; iEv < first_jet.GetSize(); ++iEv) {

					if (first_jet[iEv] > cuts[1] && first_jet[iEv] > cuts[2] && first_jet[iEv] > cuts[3]) {
						first_survivors += 1;
					}

					if (fourth_jet[iEv] > cuts[1] && fourth_jet[iEv] > cuts[2] && fourth_jet[iEv] > cuts[3]) {
						fourth_survivors += 1;
					}

				}//end of loop over the events

			}//end of while	

			//third if	
			if (first_survivors > fourth_survivors) {

				efficiency *= (first_survivors)/(1000000);
	
				fourth_survivors = 0;

				//Last cut
				while (reader.Next()) {

					//Loop over the events
					for (int iEv = 0; iEv < first_jet.GetSize(); ++iEv) {

						if (fourth_jet[iEv] > cuts[0] && fourth_jet[iEv] > cuts[1] && fourth_jet[iEv] > cuts[2] 						&& fourth_jet[iEv] > cuts[3]) {

							fourth_survivors += 1;
						}

					}//end of loop over the events

				}//end of while

			efficiency *= (fourth_survivors)/1000000;
	
			}//end of third if

			//third if
			if (first_survivors < fourth_survivors) {

				efficiency *= (fourth_survivors)/(1000000);
	
				first_survivors = 0;

				//Last cut
				while (reader.Next()) {

					//Loop over the events
					for (int iEv = 0; iEv < first_jet.GetSize(); ++iEv) {

						if (first_jet[iEv] > cuts[0] && first_jet[iEv] > cuts[1] && first_jet[iEv] > cuts[2] 							&& first_jet[iEv] > cuts[3]) {

							first_survivors += 1;
						}

					}//end of loop over the events

				}//end of while

			efficiency *= (first_survivors)/1000000;
	
			}//end of third if

		}//end of second if

		//second if
		if (v_max == fourth_survivors) {	

			efficiency *= (v_max)/(1000000);
	
			first_survivors = 0;
			third_survivors = 0;

			//Third cut
			while (reader.Next()) {

				//Loop over the events
				for (int iEv = 0; iEv < first_jet.GetSize(); ++iEv) {

					if (first_jet[iEv] > cuts[1] && first_jet[iEv] > cuts[2] && first_jet[iEv] > cuts[3]) {
						first_survivors += 1;
					}

					if (third_jet[iEv] > cuts[1] && third_jet[iEv] > cuts[2] && third_jet[iEv] > cuts[3]) {
						third_survivors += 1;
					}

				}//end of loop over the events

			}//end of while	

			//third if	
			if (first_survivors > third_survivors) {

				efficiency *= (first_survivors)/(1000000);
	
				third_survivors = 0;

				//Last cut
				while (reader.Next()) {

					//Loop over the events
					for (int iEv = 0; iEv < first_jet.GetSize(); ++iEv) {

						if (third_jet[iEv] > cuts[0] && third_jet[iEv] > cuts[1] && third_jet[iEv] > cuts[2] 							&& third_jet[iEv] > cuts[3]) {

							third_survivors += 1;
						}

					}//end of loop over the events

				}//end of while

			efficiency *= (third_survivors)/1000000;
	
			}//end of third if

			//third if
			if (first_survivors < third_survivors) {

				efficiency *= (third_survivors)/(1000000);
	
				first_survivors = 0;

				//Last cut
				while (reader.Next()) {

					//Loop over the events
					for (int iEv = 0; iEv < first_jet.GetSize(); ++iEv) {

						if (first_jet[iEv] > cuts[0] && first_jet[iEv] > cuts[1] && first_jet[iEv] > cuts[2] 							&& first_jet[iEv] > cuts[3]) {

							first_survivors += 1;
						}

					}//end of loop over the events

				}//end of while

			efficiency *= (first_survivors)/1000000;
	
			}//end of third if

		}//end of second if

	}//end of first if
		
	//first if
	if (v_max == third_survivors) {
		efficiency = (v_max)/(1000000);
		
		second_survivors = 0;
		first_survivors = 0;
		fourth_survivors = 0;

		//Second cut
		while (reader.Next()) {

			//Loop over the events
			for (int iEv = 0; iEv < first_jet.GetSize(); ++iEv) {

				if (second_jet[iEv] > cuts[2] && second_jet[iEv] > cuts[3]) {
					second_survivors += 1;
				}

				if (first_jet[iEv] > cuts[2] && first_jet[iEv] > cuts[3]) {
					first_survivors += 1;
				}

				if (fourth_jet[iEv] > cuts[2] && fourth_jet[iEv] > cuts[3]) {
					fourth_survivors += 1;
				}

			}//end of loop over the events

		}//end of while	
	
		vector<double> s {second_survivors, first_survivors, fourth_survivors};

		auto big = max_element(begin(s), end(s));
		v_max = *big;	

		//second if
		if (v_max == second_survivors) {	

			efficiency *= (v_max)/(1000000);
	
			first_survivors = 0;
			fourth_survivors = 0;

			//Third cut
			while (reader.Next()) {

				//Loop over the events
				for (int iEv = 0; iEv < first_jet.GetSize(); ++iEv) {

					if (first_jet[iEv] > cuts[1] && first_jet[iEv] > cuts[2] && first_jet[iEv] > cuts[3]) {
						first_survivors += 1;
					}

					if (fourth_jet[iEv] > cuts[1] && fourth_jet[iEv] > cuts[2] && fourth_jet[iEv] > cuts[3]) {
						fourth_survivors += 1;
					}

				}//end of loop over the events

			}//end of while	

			//third if	
			if (first_survivors > fourth_survivors) {

				efficiency *= (first_survivors)/(1000000);
	
				fourth_survivors = 0;

				//Last cut
				while (reader.Next()) {

					//Loop over the events
					for (int iEv = 0; iEv < first_jet.GetSize(); ++iEv) {

						if (fourth_jet[iEv] > cuts[0] && fourth_jet[iEv] > cuts[1] && fourth_jet[iEv] > cuts[2] 						&& fourth_jet[iEv] > cuts[3]) {

							fourth_survivors += 1;
						}

					}//end of loop over the events

				}//end of while

			efficiency *= (fourth_survivors)/1000000;
	
			}//end of third if

			//third if
			if (first_survivors < fourth_survivors) {

				efficiency *= (fourth_survivors)/(1000000);
	
				first_survivors = 0;

				//Last cut
				while (reader.Next()) {

					//Loop over the events
					for (int iEv = 0; iEv < first_jet.GetSize(); ++iEv) {

						if (first_jet[iEv] > cuts[0] && first_jet[iEv] > cuts[1] && first_jet[iEv] > cuts[2] 							&& first_jet[iEv] > cuts[3]) {

							first_survivors += 1;
						}

					}//end of loop over the events

				}//end of while

			efficiency *= (first_survivors)/1000000;
	
			}//end of third if

		}//end of second if

		//second if
		if (v_max == first_survivors) {	

			efficiency *= (v_max)/(1000000);
	
			second_survivors = 0;
			fourth_survivors = 0;

			//Third cut
			while (reader.Next()) {

				//Loop over the events
				for (int iEv = 0; iEv < first_jet.GetSize(); ++iEv) {

					if (second_jet[iEv] > cuts[1] && second_jet[iEv] > cuts[2] && second_jet[iEv] > cuts[3]) {
						second_survivors += 1;
					}

					if (fourth_jet[iEv] > cuts[1] && fourth_jet[iEv] > cuts[2] && fourth_jet[iEv] > cuts[3]) {
						fourth_survivors += 1;
					}

				}//end of loop over the events

			}//end of while	

			//third if	
			if (second_survivors > fourth_survivors) {

				efficiency *= (second_survivors)/(1000000);
	
				fourth_survivors = 0;

				//Last cut
				while (reader.Next()) {

					//Loop over the events
					for (int iEv = 0; iEv < first_jet.GetSize(); ++iEv) {

						if (fourth_jet[iEv] > cuts[0] && fourth_jet[iEv] > cuts[1] && fourth_jet[iEv] > cuts[2] 						&& fourth_jet[iEv] > cuts[3]) {

							fourth_survivors += 1;
						}

					}//end of loop over the events

				}//end of while

			efficiency *= (fourth_survivors)/1000000;
	
			}//end of third if

			//third if
			if (second_survivors < fourth_survivors) {

				efficiency *= (fourth_survivors)/(1000000);
	
				second_survivors = 0;

				//Last cut
				while (reader.Next()) {

					//Loop over the events
					for (int iEv = 0; iEv < first_jet.GetSize(); ++iEv) {

						if (second_jet[iEv] > cuts[0] && second_jet[iEv] > cuts[1] && second_jet[iEv] > cuts[2] 						&& second_jet[iEv] > cuts[3]) {

							second_survivors += 1;
						}

					}//end of loop over the events

				}//end of while

			efficiency *= (second_survivors)/1000000;
	
			}//end of third if

		}//end of second if

		//second if
		if (v_max == fourth_survivors) {	

			efficiency *= (v_max)/(1000000);
	
			second_survivors = 0;
			first_survivors = 0;

			//Third cut
			while (reader.Next()) {

				//Loop over the events
				for (int iEv = 0; iEv < first_jet.GetSize(); ++iEv) {

					if (second_jet[iEv] > cuts[1] && second_jet[iEv] > cuts[2] && second_jet[iEv] > cuts[3]) {
						second_survivors += 1;
					}

					if (first_jet[iEv] > cuts[1] && first_jet[iEv] > cuts[2] && first_jet[iEv] > cuts[3]) {
						first_survivors += 1;
					}

				}//end of loop over the events

			}//end of while	

			//third if	
			if (second_survivors > first_survivors) {

				efficiency *= (second_survivors)/(1000000);
	
				first_survivors = 0;

				//Last cut
				while (reader.Next()) {

					//Loop over the events
					for (int iEv = 0; iEv < first_jet.GetSize(); ++iEv) {

						if (first_jet[iEv] > cuts[0] && first_jet[iEv] > cuts[1] && first_jet[iEv] > cuts[2] 							&& first_jet[iEv] > cuts[3]) {

							first_survivors += 1;
						}

					}//end of loop over the events

				}//end of while

			efficiency *= (first_survivors)/1000000;
	
			}//end of third if

			//third if
			if (second_survivors < first_survivors) {

				efficiency *= (first_survivors)/(1000000);
	
				second_survivors = 0;

				//Last cut
				while (reader.Next()) {

					//Loop over the events
					for (int iEv = 0; iEv < first_jet.GetSize(); ++iEv) {

						if (second_jet[iEv] > cuts[0] && second_jet[iEv] > cuts[1] && second_jet[iEv] > cuts[2] 						&& second_jet[iEv] > cuts[3]) {

							second_survivors += 1;
						}

					}//end of loop over the events

				}//end of while

			efficiency *= (second_survivors)/1000000;
	
			}//end of third if

		}//end of second if

	}//end of first if

	//first if
	if (v_max == fourth_survivors) {
		efficiency = (v_max)/(1000000);
		
		second_survivors = 0;
		third_survivors = 0;
		first_survivors = 0;

		//Second cut
		while (reader.Next()) {

			//Loop over the events
			for (int iEv = 0; iEv < first_jet.GetSize(); ++iEv) {

				if (second_jet[iEv] > cuts[2] && second_jet[iEv] > cuts[3]) {
					second_survivors += 1;
				}

				if (third_jet[iEv] > cuts[2] && third_jet[iEv] > cuts[3]) {
					third_survivors += 1;
				}

				if (first_jet[iEv] > cuts[2] && first_jet[iEv] > cuts[3]) {
					first_survivors += 1;
				}

			}//end of loop over the events

		}//end of while	
	
		vector<double> s {second_survivors, third_survivors, first_survivors};

		auto big = max_element(begin(s), end(s));
		v_max = *big;	

		//second if
		if (v_max == second_survivors) {	

			efficiency *= (v_max)/(1000000);
	
			third_survivors = 0;
			first_survivors = 0;

			//Third cut
			while (reader.Next()) {

				//Loop over the events
				for (int iEv = 0; iEv < first_jet.GetSize(); ++iEv) {

					if (third_jet[iEv] > cuts[1] && third_jet[iEv] > cuts[2] && third_jet[iEv] > cuts[3]) {
						third_survivors += 1;
					}

					if (first_jet[iEv] > cuts[1] && first_jet[iEv] > cuts[2] && first_jet[iEv] > cuts[3]) {
						first_survivors += 1;
					}

				}//end of loop over the events

			}//end of while	

			//third if	
			if (third_survivors > first_survivors) {

				efficiency *= (third_survivors)/(1000000);
	
				first_survivors = 0;

				//Last cut
				while (reader.Next()) {

					//Loop over the events
					for (int iEv = 0; iEv < first_jet.GetSize(); ++iEv) {

						if (first_jet[iEv] > cuts[0] && first_jet[iEv] > cuts[1] && first_jet[iEv] > cuts[2] 						&& first_jet[iEv] > cuts[3]) {

							first_survivors += 1;
						}

					}//end of loop over the events

				}//end of while

			efficiency *= (first_survivors)/1000000;
	
			}//end of third if

			//third if
			if (third_survivors < first_survivors) {

				efficiency *= (first_survivors)/(1000000);
	
				third_survivors = 0;

				//Last cut
				while (reader.Next()) {

					//Loop over the events
					for (int iEv = 0; iEv < first_jet.GetSize(); ++iEv) {

						if (third_jet[iEv] > cuts[0] && third_jet[iEv] > cuts[1] && third_jet[iEv] > cuts[2] 							&& third_jet[iEv] > cuts[3]) {

							third_survivors += 1;
						}

					}//end of loop over the events

				}//end of while

			efficiency *= (third_survivors)/1000000;
	
			}//end of third if

		}//end of second if

		//second if
		if (v_max == third_survivors) {	

			efficiency *= (v_max)/(1000000);
	
			second_survivors = 0;
			first_survivors = 0;

			//Third cut
			while (reader.Next()) {

				//Loop over the events
				for (int iEv = 0; iEv < first_jet.GetSize(); ++iEv) {

					if (second_jet[iEv] > cuts[1] && second_jet[iEv] > cuts[2] && second_jet[iEv] > cuts[3]) {
						second_survivors += 1;
					}

					if (first_jet[iEv] > cuts[1] && first_jet[iEv] > cuts[2] && first_jet[iEv] > cuts[3]) {
						first_survivors += 1;
					}

				}//end of loop over the events

			}//end of while	

			//third if	
			if (second_survivors > first_survivors) {

				efficiency *= (second_survivors)/(1000000);
	
				first_survivors = 0;

				//Last cut
				while (reader.Next()) {

					//Loop over the events
					for (int iEv = 0; iEv < first_jet.GetSize(); ++iEv) {

						if (first_jet[iEv] > cuts[0] && first_jet[iEv] > cuts[1] && first_jet[iEv] > cuts[2] 						&& first_jet[iEv] > cuts[3]) {

							first_survivors += 1;
						}

					}//end of loop over the events

				}//end of while

			efficiency *= (first_survivors)/1000000;
	
			}//end of third if

			//third if
			if (second_survivors < first_survivors) {

				efficiency *= (first_survivors)/(1000000);
	
				second_survivors = 0;

				//Last cut
				while (reader.Next()) {

					//Loop over the events
					for (int iEv = 0; iEv < first_jet.GetSize(); ++iEv) {

						if (second_jet[iEv] > cuts[0] && second_jet[iEv] > cuts[1] && second_jet[iEv] > cuts[2] 						&& second_jet[iEv] > cuts[3]) {

							second_survivors += 1;
						}

					}//end of loop over the events

				}//end of while

			efficiency *= (second_survivors)/1000000;
	
			}//end of third if

		}//end of second if

		//second if
		if (v_max == first_survivors) {	

			efficiency *= (v_max)/(1000000);
	
			second_survivors = 0;
			third_survivors = 0;

			//Third cut
			while (reader.Next()) {

				//Loop over the events
				for (int iEv = 0; iEv < first_jet.GetSize(); ++iEv) {

					if (second_jet[iEv] > cuts[1] && second_jet[iEv] > cuts[2] && second_jet[iEv] > cuts[3]) {
						second_survivors += 1;
					}

					if (third_jet[iEv] > cuts[1] && third_jet[iEv] > cuts[2] && third_jet[iEv] > cuts[3]) {
						third_survivors += 1;
					}

				}//end of loop over the events

			}//end of while	

			//third if	
			if (second_survivors > third_survivors) {

				efficiency *= (second_survivors)/(1000000);
	
				third_survivors = 0;

				//Last cut
				while (reader.Next()) {

					//Loop over the events
					for (int iEv = 0; iEv < first_jet.GetSize(); ++iEv) {

						if (third_jet[iEv] > cuts[0] && third_jet[iEv] > cuts[1] && third_jet[iEv] > cuts[2] 							&& third_jet[iEv] > cuts[3]) {

							third_survivors += 1;
						}

					}//end of loop over the events

				}//end of while

			efficiency *= (third_survivors)/1000000;
	
			}//end of third if

			//third if
			if (second_survivors < third_survivors) {

				efficiency *= (third_survivors)/(1000000);
	
				second_survivors = 0;

				//Last cut
				while (reader.Next()) {

					//Loop over the events
					for (int iEv = 0; iEv < first_jet.GetSize(); ++iEv) {

						if (second_jet[iEv] > cuts[0] && second_jet[iEv] > cuts[1] && second_jet[iEv] > cuts[2] 						&& second_jet[iEv] > cuts[3]) {

							second_survivors += 1;
						}

					}//end of loop over the events

				}//end of while

			efficiency *= (second_survivors)/1000000;
	
			}//end of third if

		}//end of second if

	}//end of first if
				

	cout << "L'efficienza totale vale: " << efficiency << endl;

	return 0;
}

