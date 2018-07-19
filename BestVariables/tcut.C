//Programma per individuare le vaiabili più significative con il taglio sulle masse.
// -- Selezione delgli eventi con tagli sulle masse invarianti
// -- Selezione del range migliore in base a S/sqrt(B) da dx e da sx

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

void tcut( ){	
	
	int N = 12;
	string * var = new string [N]; 
	string * tit = new string [N];

	//AGGIUNGERE LE VARIABILI DA PLOTTARE, modificare N
	var[0] = "deltaphi_bbljj";	
	var[1] = "deltar_bbljj";
	var[2] = "deltaphi_ljj";	
	var[3] = "deltar_ljj";
	var[4] = "deltaphi_bb";	
	var[5] = "deltar_bb";
	var[4] = "lep_pt";	
	var[5] = "lep_E";  
	var[6] = "ww_pt";	
	var[7] = "bb_pt";
	var[8] = "Htnu";  
	var[9] = "Ht";
	var[10] = "Ptm";
	var[11] = "deltaeta_bb"; 

	tit[0] = "#Delta#phi_{b#bar{b}ljj} distribution; #Delta#phi [rad];  Probabilty  ";
	tit[1] = "#DeltaR_{b#bar{b}ljj} distribution; #DeltaR [rad]; Probabilty  ";
	tit[2] = "#Delta#phi_{ljj} distribution; #Delta#phi [rad];  Probabilty ";
	tit[3] = "#DeltaR_{ljj} distribution; #DeltaR [rad]; Probabilty ";
	tit[4] = "p_{t_{lep}} distribution; p_{t_{lep}}; Probabilty";
	tit[5] = "E_{t_{lep}} distribution; E_{t_{lep}}; Probabilty";
	tit[6] = "p_{t_{WW}} distribution; p_{t_{WW}}; Probabilty";
	tit[7] = "p_{t_{b#bar{b}}} distribution; p_{t_{b#bar{b}}}; Probabilty";
	tit[8] = "H_{t_{#nu}} distribution; H_{t_{#nu}}; Probabilty";
	tit[9] = "H_{t} distribution; H_{t}; Probabilty";
	tit[10] = "p_{tm} distribution; p_{tm}; Probabilty";
	tit[11] = "#Delta#eta_{b#bar{b}} distribution; #Delta#eta_{b#bar{b}}; Probabilty";

	//LETTURA DEI TTree
    TFile * sinput = TFile::Open( "newHH.root" );
    TFile * binput = TFile::Open( "newttbar.root" );
    TTree * signal  = (TTree*)sinput->Get("tree");
    TTree * background  = (TTree*)binput->Get("tree");
	
	TCanvas ** c = new TCanvas * [N];

	for(int i = 0; i < N; i++){

		//INSERIRE RANGE DEGLI ISTOGRAMMI
		double min, max;
		int nbin;
		cout << "-------------------------------------------" << endl;
		cout << "Grandezza analizzata: " << var[i] <<endl;
		//cout << "Inserire range della grandezza da visualizzare: " << var[i] <<endl;
		//cout << "Min: "; cin >> min;
		//cout << "Max: "; cin >> max;
		//cout << "Nbin: "; cin >> nbin; 
		nbin=30;
		if( i < 4){
				if(i%2 == 0 ){ min=0; max=TMath::Pi();}
				if(i%2 != 0){ min=0; max=5;}
		}else{
			if( i==4 ){ min=0; max=200;}
			if( i==5 ){ min=0; max=200;}
			if( i==6 ){ min=0; max=300;}
			if( i==7 ){ min=0; max=350;}
			if( i==8 ){ min=100; max=1000;}
			if( i==9 ){ min=100; max=800;}
			if( i==10 ){ min=0; max=200;}
			if( i==11 ){ min=0; max=5;}
		}	
		string title = "Graph_" + var[i];
		c[i] = new TCanvas(Form("c%d",i), title.c_str() );	
		c[i]->Divide(1,3);
		c[i]->cd(1);

		//ISTOGRAMMI DELLE DISTIBUZIONI
		string title1 = "Background_" + var[i];
		string title2 = "Signal_" + var[i];
   		TH1D * hs = new TH1D( title2.c_str(), "Signal", nbin,min,max);
 		TH1D * hb = new TH1D( title1.c_str(), "Background", nbin,min,max); 
   		histo_cutmass(signal, background, hs, hb, var[i]);

   		//ISTOGRAMMA pdf
		THStack * pdf = new THStack("pdf", "Pdf histo");
   		TH1D * pdfs = new TH1D(Form("ps%d", i),"Signal Probability distribution", nbin,min,max);
		TH1D * pdfb = new TH1D(Form("pb%d", i),"Background Probability distribution", nbin,min,max);
    	for(int j = 1; j <= nbin; j++){
    		pdfs->SetBinContent(j, hs->GetBinContent(j)/hs->Integral());
			pdfb->SetBinContent(j, hb->GetBinContent(j)/hb->Integral());
		}
		setstack( pdfs, pdfb, pdf );
		pdf->SetTitle( tit[i].c_str() );
	
		TH1D * hsx = new TH1D(Form("S/sqrt(B)s %d", i),"T cut: #frac{S_{sx}}{#sqrt{B_{sx}}}", nbin, min, max);
		TH1D * hdx = new TH1D(Form("S/sqrt(B)d %d", i),"T cut: #frac{S_{dx}}{#sqrt{B_{dx}}}", nbin, min, max);
    	//TH1D * hsx = new TH1D(Form("S*Bs %d", i)," S_{sx} #times B_{dx}", nbin, min, max);
		//TH1D * hdx = new TH1D(Form("S*Bd %d", i)," S_{dx} #times B_{sx}", nbin, min, max);
		for(int j = 1; j <= nbin; j++){
			double s_sx = pdfs->Integral(1,j);
			double s_dx = pdfs->Integral(j, nbin);
			double b_sx = pdfb->Integral(1,j);
			double b_dx = pdfb->Integral(j, nbin );
			if ( b_sx != 0) hsx->SetBinContent(j, s_sx / sqrt(b_sx) );
			if ( b_dx != 0) hdx->SetBinContent(j, s_dx / sqrt(b_dx) );
			//hsx->SetBinContent(j, s_sx * b_dx );
			//hdx->SetBinContent(j, s_dx * b_sx );

		}
		c[i]->cd(2);
		gPad->SetGrid(1,1);
		gStyle->SetOptStat(0000);	
		hsx->SetLineWidth(3);
		hsx->SetLineColor(2);
		hsx->Draw("C");
		hsx->SetTitle(" T cut: #frac{S_{sx}}{#sqrt{B_{sx}}}; t; ");
		c[i]->cd(3);
		gPad->SetGrid(1,1);
		gStyle->SetOptStat(0000);	
		hdx->SetLineWidth(3);
		hdx->SetLineColor(2);
		hdx->Draw("C");
		hdx->SetTitle(" T cut: #frac{S_{dx}}{#sqrt{B_{dx}}}; t; ");

		//TCut selection 
		double max_dx=0, max_sx=0, maxmax;
		double t_sx=0, t_dx=0, t_max;
		bool dx=0, sx=0;	
		for( int i=1; i < nbin; i++ ){
			if( hsx->GetBinContent(i) >= max_sx){
				max_sx= hsx->GetBinContent(i);
				t_sx = hsx->GetBinCenter(i);
			}
 			if( hdx->GetBinContent(i) >= max_dx){
				 max_dx=hdx->GetBinContent(i);
				 t_dx = hdx->GetBinCenter(i);	
			}
		}
		double binsize = (max-min)/nbin; 
		cout << "max_t_dx: " << max_dx <<endl;
		cout << "max_t_sx: " << max_sx <<endl;
		cout << "REGIONE SELEZIONATA: " << t_dx  << "< x < " << t_sx << endl; 
		cout << "errore sulla selezione: " << binsize << endl; 
 /*		if( max_dx >= max_sx){
			dx=1;
			maxmax=max_dx; 
			t_max = t_dx;
			cout << " MASSIMO trovato da DESTRA: " << endl
				 << "Valore di S/sqrt(B): " << maxmax << endl
				 << "t cut: " << t_max << endl;
		}else{
			sx=1;
			maxmax=max_sx; 
			t_max = t_sx;
			cout << " MASSIMO trovato da SINISTRA: " << endl
				 << "Valore di S/sqrt(B): " << maxmax << endl
				 << "t cut: " << t_max << endl;
		}	
*/	}
	return;
}
