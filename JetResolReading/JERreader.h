#ifndef jerreader
#define jerreader

#include <string>
#include <iostream>
#include "TF1.h"
#include "TH2F.h"

using namespace std ;

class JERreader
{
  public:

    JERreader (string) ;
    ~JERreader () ;
    float resolution (float eta, float pt) ;
    float resolution (float eta, float rho, float pt) ;
    void changeRho (float rho) ;
    void dumpMaps () ;
    void drawResolFunction (float eta, float rho) ;

  private:

    TF1 * m_resolution ;
    int m_etaBins ; 
    int m_rhoBins ; 
    float m_rho_ref ;
    TH2F * m_par_0 ;
    TH2F * m_par_1 ;
    TH2F * m_par_2 ;
    TH2F * m_par_3 ;
}; 

#endif
