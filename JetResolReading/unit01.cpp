//  c++ -o unit01 unit01.cpp JERreader.cc `root-config --libs --glibs --cflags`

#include "JERreader.h"

using namespace std ;

int main (int argc, char **argv)
{
  if (argc < 2) 
    {
      cerr << "JER file missing" << endl ;
      exit (1) ;
    }    
  JERreader testing (argv[1]) ; 
  testing.dumpMaps () ;
  cout << testing.resolution (1.1, 125) << endl ;
  return 0 ;
}

