#include <map>
#include <iostream>


using namespace std;

#ifndef CONFIG_H
#define CONFIG_H

typedef enum {
  N,B,R,T,M,P,initGenome,rndSeed,error } configParams ;

class Config {

  const int cfgMax; 
  double * dconfig;

  void initialize() {
    dconfig[N] = 100;
    dconfig[B] = 1.2;
    dconfig[R] = 3;
    dconfig[T] = 1;
    dconfig[M] = 1.0;
    dconfig[P] = 0.0;
    dconfig[initGenome] = 0.5;
    dconfig[rndSeed] = 3456; 
  }



public:
  Config() : cfgMax(32) {
    dconfig = new double[cfgMax];
    initialize();
  }

  ~Config() { 
    delete [] dconfig;
  }


  double & operator[](configParams );

  friend  ostream & operator<<(ostream &  , const Config &);
};

#endif
