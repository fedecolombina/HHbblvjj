//  c++ -o unit03 unit03.cpp JERreader.cc `root-config --libs --glibs --cflags`

#include "JERreader.h"
#include "TGraph.h"
#include "TCanvas.h"

using namespace std ;

int main (int argc, char **argv)
{
  if (argc < 2) 
    {
      cerr << "JER file missing" << endl ;
      exit (1) ;
    }    
  JERreader testing (argv[1]) ; 

  float rho[4] = {40, 30., 20., 10.} ;
  float pts[13] = {10., 15., 20., 30., 40., 50., 60., 70., 80., 90., 100., 110., 120.} ;

  TGraph * trends[4] ;
  for (int i = 0 ; i < 4 ; ++i)
    {
      float resolutions[13] ;
      for (int j = 0 ; j < 13 ; ++j)
        {
          resolutions[j] = testing.resolution (0., rho[i], pts[j]) ;
        }
      trends[i] = new TGraph (13, pts, resolutions) ;
    }

  TCanvas c1 ;
  string options = "ALP" ;
  for (int i = 0 ; i < 4 ; ++i)
    {
      trends[i]->SetLineWidth (2) ;
      trends[i]->SetLineColor (2*i) ;
      trends[i]->SetMarkerStyle (20+i) ;
      trends[i]->SetMarkerColor (2*i) ;
      trends[i]->Draw (options.c_str ()) ;
      options = "LP" ;
    }

  c1.Print ("trends.pdf", "pdf") ;

  return 0 ;
}

