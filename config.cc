#include <iostream>
#include <iomanip>
#include "config.hh"

using namespace std;


double & Config::operator[](configParams cfg)
{
    
  if (cfg < cfgMax) {
    return dconfig[cfg];
  }
  else {
    cerr << "Unknown config parameter: " << cfg << endl;
    return dconfig[error];
  }
  
}



ostream & operator<<(ostream & out,const Config & cfg) {
  out << setw(13) << "N = " << cfg.dconfig[N] << endl;
  out << setw(13) << "B = " << cfg.dconfig[B] << endl;
  out << setw(13) << "R = " << cfg.dconfig[R] << endl;
  out << setw(13) << "T = " << cfg.dconfig[T] << endl;
  out << setw(13) << "M = " << cfg.dconfig[M] << endl;
  out << setw(13) << "P = " << cfg.dconfig[P] << endl;
  out << setw(13) << "initGenome = " << cfg.dconfig[initGenome] << endl;
  out << setw(13) << "rndSeed = " << cfg.dconfig[rndSeed] << endl;
  return out;
}


