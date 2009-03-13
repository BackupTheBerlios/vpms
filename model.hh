#include <unordered_map>
#include "vpms.hh"

using namespace std;

#ifndef MODEL_H
#define MODEL_H

class GenomeData {
  unsigned char maxage;
  unsigned int nindividuals;
  unsigned int *ages;

public:
  GenomeData(genome g);
  ~GenomeData();
  void  AddIndividual(unsigned int = 0);
  unsigned int MakeOlder();
  unsigned int VerhulstKill(double);
  unsigned int GiveBirth();
  unsigned int Size() const;
  unsigned int Born() const;

};


class Environment {
  //  unordered_map<genome,GenomeData , hash<genome>, eqgen> genomes;

  unordered_map<genome,GenomeData *> genomes;
  unsigned int nindividuals;

  void AddIndividual(genome);

  struct BornGenome {
    unsigned int nborn;
    genome baseGenome;
  };

public:
  Environment();
  ~Environment();
  void Fill(unsigned int n);
  void Clear();
  void Step();
};

#endif

