//  c++ -o unit02 unit02.cpp JERreader.cc `root-config --libs --glibs --cflags`

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

  testing.drawResolFunction (2.1, 37.2) ;

  return 0 ;
}

