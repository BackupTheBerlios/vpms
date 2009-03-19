#include <map>
#include <iostream>
#include "MersenneTwister.h"


using namespace std;

#ifndef CONFIG_H
#define CONFIG_H

typedef enum {
  N,B,R,T,M,P,initGenome,randomSeed,error } configParams ;

typedef struct {

  unsigned long N;
  double B;
  int R;
  double M;
  double P;
  int T;

  double initGenome;
  int randomSeed;

} parameters ;


namespace vpms {
  extern MTRand random;
  extern parameters p;
}



class Config {


  void initialize() {
    vpms::p.N = 10000;
    vpms::p.B = 1.2;
    vpms::p.R = 3;
    vpms::p.T = 1;
    vpms::p.M = 1.0;
    vpms::p.P = 0.05;
    vpms::p.initGenome = 0.5;
    vpms::p.randomSeed = 3456; 
    vpms::random.seed(vpms::p.randomSeed);
  }



public:
  Config() {
    initialize();
  }

  void SetParam(configParams, double);

  friend  ostream & operator<<(ostream &  , const Config &);
};

#endif
