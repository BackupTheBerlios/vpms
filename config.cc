#include <iostream>
#include <iomanip>
#include "config.hh"
#include "MersenneTwister.h"

using namespace std;

extern MTRand vpms::random;
extern parameters vpms::p;

void Config::SetParam(configParams cfg, double value)
{
  switch (cfg) {
  
  case N:
    vpms::p.N = static_cast<unsigned long>(value);
    break;
  case B:
    vpms::p.B = value; 
    break;
  case R:
    vpms::p.R = static_cast<int>(value); 
    break;
  case M:
    vpms::p.M = value;
    break;
  case P:
    vpms::p.P = value;
    break;
  case T:
    vpms::p.T = static_cast<int>(value);
    break;
  case initGenome:
    vpms::p.initGenome = value;
    break;
  case randomSeed:
    vpms::p.randomSeed = static_cast<int>(value);
    vpms::random.seed(vpms::p.randomSeed);
    break;
  default:
    cerr << "Unknown config parameter: " << cfg << endl;
  }
    
}


ostream & operator<<(ostream & out,const Config & cfg) {
  out << setw(13) << "N = " << vpms::p.N  << endl;
  out << setw(13) << "B = " << vpms::p.B  << endl;
  out << setw(13) << "R = " << vpms::p.R  << endl;
  out << setw(13) << "T = " << vpms::p.T  << endl;
  out << setw(13) << "M = " << vpms::p.M  << endl;
  out << setw(13) << "VPMS::P = " << vpms::p.P  << endl;
  out << setw(13) << "initGenome = " << vpms::p.initGenome << endl;
  out << setw(13) << "randomSeed = " << vpms::p.randomSeed << endl;
  return out;
}


