
#ifndef VPMS_H
#define VPMS_H

void initialize();

typedef unsigned int genome;

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

#endif
