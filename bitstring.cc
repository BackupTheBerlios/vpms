#include <iostream>
#include "vpms.hh"
#include "MersenneTwister.h"

using namespace std;

extern MTRand random;


extern parameters p;


// get bit at position "p" from char "x"
unsigned char getbit(genome x, unsigned char p) {
  return (x >> p) & ~((~0) << 1);
}

// set bit "value" at position "p" in char "c"
void setbit(genome *x, unsigned char p, unsigned char value) {
  if(value)
    *x = (value << p) | *x ;
  else {
    *x = ~( (~( (~0) << 1 )) << p) & *x ;
  }
}

int count_max_age(genome g) {
  int nmut=0;
  for(unsigned int i=0; i<8*sizeof(genome); i++) {
    nmut += getbit(g,i);
    if(nmut >= p.T) {
      return i+1;
    }
  }
  return 8*sizeof(genome);
}

  


void print_genome(genome g) {
  for(int i=8*sizeof(g)-1; i>=0; i--) {
    cerr << static_cast<int>(getbit(g,i));
  }
  cerr << endl;
}

void addPositiveMutations(genome *g, int n) {
  for(int i=1; i<=n; i++) {
    int pos = static_cast<int>(8*sizeof(genome) * random());
    setbit(g,pos,0);
  }
}

void addNegativeMutations(genome *g, int n) {
  for(int i=1; i<=n; i++) {
    int pos = static_cast<int>(8*sizeof(genome) * random());
    setbit(g,pos,1);
  }
}
