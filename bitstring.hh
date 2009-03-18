#include "vpms.hh"

using namespace std;

#ifndef BITSTRING_H
#define BITSTRING_H
int count_max_age(genome g);
int setbit(genome *, unsigned char, unsigned char);
void print_genome(genome);
string get_genome_repr(genome);

void addPositiveMutations(genome *, int);
void addNegativeMutations(genome *, int);
#endif
