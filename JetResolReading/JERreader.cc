#include "JERreader.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include "TCanvas.h"
#include "TFile.h"


void 
dumpColumn (
    vector<float> * output_v, 
    const vector<vector<float> > & input_v, 
    int column
  )
{
  for (int i = 0 ; i < input_v.size () ; ++i)
    output_v->push_back (input_v.at (i).at (column)) ;
  return ;
}


//PG ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- 


void 
getBinLimits (
    vector<float> * output_v, 
    const vector<vector<float> > & input_v, 
    int firstColumn
  )
{
  dumpColumn (output_v, input_v, firstColumn) ;
  dumpColumn (output_v, input_v, firstColumn+1) ;
  sort (output_v->begin (), output_v->end ()) ;
  vector<float>::iterator it;
  it = unique (output_v->begin (), output_v->end ()) ;
  output_v->resize (distance (output_v->begin (),it));
}


//PG ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- 


//PG constructor of the jet energy resolution reader
//PG reads the input file and saves values in a container,
//PG to return the right resolution value as a function of eta and pt of the jet
JERreader::JERreader (string inputFileName)
  {
    m_rho_ref = 30. ;
    ifstream inputFile (inputFileName) ;
    string oneLine ;
    
    //PG read header line and forget about it
    //PG assuming it has the form 
    //PG {2    JetEta    Rho    1    JetPt    sqrt([0]*abs([0])/(x*x)+[1]*[1]*pow(x,[3])+[2]*[2])    Resolution}
    getline (inputFile, oneLine) ;
    
    //PG load file content in a NxM matrix of float
    vector<vector<float> > fileContent ;
    vector<float> dummy (11, 0.) ;
    while (getline (inputFile, oneLine))
      {
        stringstream iss (oneLine) ;
        for (int i = 0 ; i < 11 ; ++i)
          {
             string parola ;
             iss >> parola ;
             dummy[i] = atof (parola.c_str ()) ;
          }
        fileContent.push_back (dummy) ;  
      }

    vector<float> etaVals ;
    getBinLimits (&etaVals, fileContent, 0) ;

    vector<float> rhoVals ;
    getBinLimits (&rhoVals, fileContent, 2) ;

    vector<float> ptVals ;
    getBinLimits (&ptVals, fileContent, 5) ;

    m_par_0 = new TH2F ("m_par_0", "m_par_0", 
                        etaVals.size ()-1, &etaVals[0], 
                        rhoVals.size ()-1, &rhoVals[0]) ; 

    m_par_1 = new TH2F ("m_par_1", "m_par_1", 
                        etaVals.size ()-1, &etaVals[0], 
                        rhoVals.size ()-1, &rhoVals[0]) ;

    m_par_2 = new TH2F ("m_par_2", "m_par_2", 
                        etaVals.size ()-1, &etaVals[0], 
                        rhoVals.size ()-1, &rhoVals[0]) ;

    m_par_3 = new TH2F ("m_par_3", "m_par_3", 
                        etaVals.size ()-1, &etaVals[0], 
                        rhoVals.size ()-1, &rhoVals[0]) ;

    for (int i = 0 ; i < fileContent.size () ; ++i)
      {
        m_par_0->Fill (fileContent.at (i).at (0),   //eta
                       fileContent.at (i).at (2),   //rho
                       fileContent.at (i).at (7)) ; //param_0

        m_par_1->Fill (fileContent.at (i).at (0),   //eta
                       fileContent.at (i).at (2),   //rho
                       fileContent.at (i).at (8)) ; //param_1

        m_par_2->Fill (fileContent.at (i).at (0),   //eta
                       fileContent.at (i).at (2),   //rho
                       fileContent.at (i).at (9)) ; //param_2

        m_par_3->Fill (fileContent.at (i).at (0),   //eta
                       fileContent.at (i).at (2),   //rho
                       fileContent.at (i).at (10)) ; //param_3
      }

    m_resolution = new TF1 ("resolution", "sqrt([0]*abs([0])/(x*x)+[1]*[1]*pow(x,[3])+[2]*[2])",
                            ptVals.at (0), ptVals.at (ptVals.size ()-1));

    return ;
  }


//PG ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- 


void 
JERreader::dumpMaps ()
  {
    TCanvas c1 ;
    m_par_0->Draw ("colz") ;
    c1.Print ("par_0.pdf", "pdf") ; 
    m_par_1->Draw ("colz") ;
    c1.Print ("par_1.pdf", "pdf") ; 
    m_par_2->Draw ("colz") ;
    c1.Print ("par_2.pdf", "pdf") ; 
    m_par_3->Draw ("colz") ;
    c1.Print ("par_3.pdf", "pdf") ; 
    TFile outfile ("maps.root", "recreate") ;
    outfile.cd () ;
    m_par_0->Write () ;
    m_par_1->Write () ;
    m_par_2->Write () ;
    m_par_3->Write () ;
    outfile.Close () ;
    return ;
  }




//PG ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- 


void 
JERreader::drawResolFunction (float eta, float rho)
{
  Int_t eta_bin = m_par_0->GetXaxis ()->FindBin (eta) ;
  Int_t rho_bin = m_par_0->GetYaxis ()->FindBin (rho) ;

  float val_0 = m_par_0->GetBinContent (eta_bin, rho_bin) ;
  float val_1 = m_par_1->GetBinContent (eta_bin, rho_bin) ;
  float val_2 = m_par_2->GetBinContent (eta_bin, rho_bin) ;
  float val_3 = m_par_3->GetBinContent (eta_bin, rho_bin) ;

  m_resolution->SetParameter (0, val_0) ;
  m_resolution->SetParameter (1, val_1) ;
  m_resolution->SetParameter (2, val_2) ;
  m_resolution->SetParameter (3, val_3) ;

  TCanvas c1 ;
  m_resolution->Draw () ;
  c1.Print ("resolFunction.pdf", "pdf") ;
  return ;
}


//PG ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- 


void
JERreader::changeRho (float rho) 
{
  m_rho_ref = rho ;
}


//PG ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- 


float
JERreader::resolution (float eta, float pt)
{
  Int_t eta_bin = m_par_0->GetXaxis ()->FindBin (eta) ;
  Int_t rho_bin = m_par_0->GetYaxis ()->FindBin (m_rho_ref) ;

  float val_0 = m_par_0->GetBinContent (eta_bin, rho_bin) ;
  float val_1 = m_par_1->GetBinContent (eta_bin, rho_bin) ;
  float val_2 = m_par_2->GetBinContent (eta_bin, rho_bin) ;
  float val_3 = m_par_3->GetBinContent (eta_bin, rho_bin) ;

  m_resolution->SetParameter (0, val_0) ;
  m_resolution->SetParameter (1, val_1) ;
  m_resolution->SetParameter (2, val_2) ;
  m_resolution->SetParameter (3, val_3) ;

  return m_resolution->Eval (pt) ;
}


//PG ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- 


float
JERreader::resolution (float eta, float rho, float pt)
{
  Int_t eta_bin = m_par_0->GetXaxis ()->FindBin (eta) ;
  Int_t rho_bin = m_par_0->GetYaxis ()->FindBin (rho) ;

  float val_0 = m_par_0->GetBinContent (eta_bin, rho_bin) ;
  float val_1 = m_par_1->GetBinContent (eta_bin, rho_bin) ;
  float val_2 = m_par_2->GetBinContent (eta_bin, rho_bin) ;
  float val_3 = m_par_3->GetBinContent (eta_bin, rho_bin) ;

  m_resolution->SetParameter (0, val_0) ;
  m_resolution->SetParameter (1, val_1) ;
  m_resolution->SetParameter (2, val_2) ;
  m_resolution->SetParameter (3, val_3) ;

  return m_resolution->Eval (pt) ;
}


//PG ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- 


JERreader::~JERreader ()
  {
     delete m_resolution ;
     delete m_par_0 ;
     delete m_par_1 ;
     delete m_par_2 ;
     delete m_par_3 ;
     return ;
  }


