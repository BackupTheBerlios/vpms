#include <unordered_map>
#include <map>
#include <vector>

#include "vpms.hh"

using namespace std;

#ifndef MODEL_H
#define MODEL_H

typedef struct  {
  
  unsigned int nindividuals;
  unsigned int born;
  unsigned int mkilled;
  unsigned int vkilled;
  
} RuntimeParams;


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

  void UpdatePopStats(vector<unsigned int> &) const;
  void UpdateMortStats(vector<unsigned int> &, vector<unsigned int> &) const;


};


class Environment {
  //  unordered_map<genome,GenomeData , hash<genome>, eqgen> genomes;

  unordered_map<genome,GenomeData *> genomes;
  unsigned int nindividuals;
  unsigned int born;
  unsigned int vkilled;
  unsigned int mkilled;
  unsigned int tstep;

  void AddIndividual(genome);

  struct BornGenome {
    unsigned int nborn;
    genome baseGenome;
  };

public:
  Environment();
  ~Environment();
  void Fill(unsigned int n);
  void Step();
  unsigned int Clear();
 
  unsigned int Size() const {
    return nindividuals;
  }

  vector<unsigned int> PopStructure() ;
  vector<double> MortStructure() ;
  void UpdateStats(vector<unsigned int> *, vector<unsigned int> *) ;
  RuntimeParams Runtime() const;
  unsigned int Time() const;
  double Diversity() const;
  
  multimap<unsigned int, genome> GetTopRank(int n = 8);
  
  // TODO genpool histogram
};

#endif

