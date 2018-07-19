//Programma per individuare le vaiabili più significative con il taglio sulle masse.
// -- Selezione delgli eventi con tagli sulle masse invarianti
// -- Fattori di merito

using namespace std;

void histo_cutmass( TTree * tree_s, TTree * tree_b, TH1D * hs,  TH1D * hb, string var ){	

	//Dal TTree del background vengono selezionati gli eventi con:  
	// -- mww < 125 [GeV]/c^2
	// -- 124 < mbb < 126 [GeV]/c^2
	//Viene poi fatto un istogramma della pdf di ogni variabile scelta
 
	TTreeReader reader_s( tree_s );
	TTreeReader reader_b( tree_b );
	TTreeReaderValue<double> mbb(reader_b, "mww"); 
	TTreeReaderValue<double> mww(reader_b, "mbb"); 
	TTreeReaderValue<double> mjj_b(reader_b, "mjj");
	TTreeReaderValue<double> mjj_s(reader_s, "mjj");
	TTreeReaderValue<double>  variable_b( reader_b, var.c_str());
	TTreeReaderValue<double>  variable_s( reader_s, var.c_str());

	//Selezione eventi backgrond e scrittura degli istogrammi 
	while ( reader_b.Next() ) {
		if ( * mww <= 125. && * mbb >= 124. && * mbb <= 126.){
				hb->Fill(abs(* variable_b));
		}
	}	
	//Scrittura degli istogrammi del segnale
	while ( reader_s.Next() ) {
				hs->Fill(abs(* variable_s));			
	}
	return;
}

void setstack(TH1D * hs, TH1D * hb, THStack * stack ){	
	hb->SetFillStyle(3003);
	hs->SetFillStyle(3004);
    hs->SetFillColor(2);
	hb->SetFillColor(4);
	hs->SetLineColor(2);
	hb->SetLineColor(4);	
	stack->Add(hs, "S");
	stack->Add(hb, "S");
    stack->Draw("nostack");
	gPad->SetGrid(1,1);		
	TLegend *legend1 = new TLegend(0.8,0.2,0.98,0.38);
    legend1->AddEntry(hs,"Signal", "f");
    legend1->AddEntry(hb,"Background", "f");
    legend1->Draw("SAME");
	return;
}

void histo_ang( ){	
	
	int N = 6;
	string * var = new string [N]; 
	string * tit = new string [N];

	//AGGIUNGERE LE VARIABILI DA PLOTTARE, modificare N
	var[0] = "deltaphi_bbljj";	
	var[1] = "deltar_bbljj";
	var[2] = "deltaphi_ljj";	
	var[3] = "deltar_ljj";
	var[4] = "deltaphi_bb";	
	var[5] = "deltar_bb";

	tit[0] = "#Delta#phi_{b#bar{b}ljj} distribution; #Delta#phi [rad];  Probabilty  ";
	tit[1] = "#DeltaR_{b#bar{b}ljj} distribution; #DeltaR [rad]; Probabilty  ";
	tit[2] = "#Delta#phi_{ljj} distribution; #Delta#phi [rad];  Probabilty ";
	tit[3] = "#DeltaR_{ljj} distribution; #DeltaR [rad]; Probabilty ";
	tit[4] = "#Delta#phi_{b#bar{b}} distribution; #Delta#phi [rad];  Probabilty ";
	tit[5] = "#DeltaR_{b#bar{b}} distribution; #DeltaR [rad];  Probabilty ";

	//LETTURA DEI TTree
    TFile * sinput = TFile::Open( "newHH.root" );
    TFile * binput = TFile::Open( "newttbar.root" );
    TTree * signal  = (TTree*)sinput->Get("tree");
    TTree * background  = (TTree*)binput->Get("tree");
	

	TCanvas ** c = new TCanvas * [N];
	TH1D *** h = new  TH1D ** [N];


	for(int i = 0; i < N; i++){

		//INSERIRE RANGE DEGLI ISTOGRAMMI
		double min, max;
		int nbin;
		cout << "Grandezza visulizzata: " << var[i] <<endl;
		//cout << "Inserire range della grandezza da visualizzare: " << var[i] <<endl;
		//cout << "Min: "; cin >> min;
		//cout << "Max: "; cin >> max;
		//cout << "Nbin: "; cin >> nbin; 
		
		nbin=100;
		if(i%2 == 0 ){ min=0; max=TMath::Pi();}
		if(i%2 != 0){ min=0; max=7;}
		string title = "Graph_" + var[i];

		h[i] = new TH1D * [8];
		c[i] = new TCanvas(Form("c%d",i), title.c_str() );	
		c[i]->Divide(2,2);
		c[i]->cd(1);

		//ISTOGRAMMI DELLE DISTIBUZIONI
		string title1 = "Background_" + var[i];
		string title2 = "Signal_" + var[i];
   		h[i][1] = new TH1D( title2.c_str(), "Signal", nbin,min,max);
 		h[i][2] = new TH1D( title1.c_str(), "Background", nbin,min,max); 
   		histo_cutmass(signal, background, h[i][1], h[i][2], var[i]);

   		//ISTOGRAMMA pdf
		THStack * pdf = new THStack("pdf", "Pdf histo");
   		h[i][3] = new TH1D(Form("ps%d", i),"Signal Probability distribution", nbin,min,max);
		h[i][4] = new TH1D(Form("pb%d", i),"Background Probability distribution", nbin,min,max);
    	for(int j = 1; j <= nbin; j++){
    		h[i][3]->SetBinContent(j, h[i][1]->GetBinContent(j)/h[i][1]->Integral());
			h[i][4]->SetBinContent(j, h[i][2]->GetBinContent(j)/h[i][2]->Integral());
		}
		setstack( h[i][3], h[i][4], pdf );
		pdf->SetTitle(tit[i].c_str() );
		//cout<<"Total probability signal in this range : "<< h[i][3]->Integral() <<"\n"; 
		//cout<<"Total probability background in this range: "<< h[i][4]->Integral() <<"\n"; 

		//FATTORI diMERITO:
		// -- S_sx*B_dx
		// -- s_sx/sqrt(B_sx)
		// -- (S_dx * N_S)/( S_dx * N_S + B_sx * N_B)

		h[i][5] = new TH1D(Form("S*B %d", i)," S_{sx} #bullet B_{dx}", nbin, min, max);
		h[i][6] = new TH1D(Form("S/sqrt(B) %d", i),"T cut: #frac{S_{sx}}{#sqrt{B_{sx}}}", nbin, min, max);
		h[i][7] = new TH1D(Form("Purity %d", i),"Purity: #frac{S_{dx} #bullet N_{S}}{ S_{dx}#bullet N_{S} + B_{sx} #bullet N_{B}} ", nbin, min, max);
    	for(int j = 1; j <= nbin; j++){
			double s_sx = h[i][3]->Integral(1,j);
			double s_dx = h[i][3]->Integral(j, nbin);
			double b_sx = h[i][4]->Integral(1,j);
			double b_dx = h[i][4]->Integral(j, nbin );
			double purity_s = s_dx*h[i][1]->Integral() / (s_dx*h[i][1]->Integral()+b_sx*h[i][2]->Integral());
	   		h[i][5]->SetBinContent(j, s_sx * b_dx);
			if ( b_sx != 0) h[i][7]->SetBinContent(j, s_sx / sqrt(b_sx) );
			h[i][6]->SetBinContent(j, purity_s);
		}
		for (int j = 0; j < 3; j++){
			c[i]->cd(2+j);
			gPad->SetGrid(1,1);
			gStyle->SetOptStat(0000);	
			h[i][5+j]->SetLineWidth(3);
			h[i][5+j]->SetLineColor(2);
			h[i][5+j]->Draw("C");
			if (j == 0 ) h[i][5]->SetTitle(" Significance level #times Power: S_{sx}B_{dx} ; t; ");
    		if (j == 1 ) h[i][7]->SetTitle(" T cut: #frac{S_{sx}}{#sqrt{B_{sx}}}; t; ");
			if (j == 2 ) h[i][6]->SetTitle(" Signal Purity:  #frac{S_{dx}N_{S}}{ S_{dx}N_{S} + B_{sx}N_{B}} ; t ; ");	
		}
	}
	return;
}
