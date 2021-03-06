#include <unordered_map>
#include <map>
#include <vector>
#include <cstdio>

#include "vpms.hh"
#include "config.hh"


using namespace std;

#ifndef MODEL_H
#define MODEL_H

typedef struct  {
  
    unsigned int nindividuals;
    unsigned int born;
    unsigned int mkilled;
    unsigned int vkilled;
  
} RuntimeParams;

namespace vpms {
    string get_genome_repr(genome g);
}


template<class KType, class VType> class AutoHistogram;

class GenomeData {
    
    unsigned char maxage;
    unsigned int nindividuals;
    unsigned int *ages;
  

public:
    
    GenomeData(genome g);
    GenomeData(FILE*);
    ~GenomeData();
    void  AddIndividual(unsigned int = 0);
    unsigned int MakeOlder();
    unsigned int VerhulstKill(double);
    unsigned int GiveBirth();
    unsigned int Size() const;
    unsigned int SizeAboveAge(int) const;
    unsigned int Born() const;
    
    void UpdatePopStats(vector<unsigned int> &) const;
    void UpdateMortStats(vector<unsigned int> &, vector<unsigned int> &) const;
    inline int CountMaxAge(genome g);
    
    void Write(FILE*);

};


class Environment {

    struct BornGenome {
        unsigned int nborn;
        genome baseGenome;
    };

 
    unordered_map<genome,GenomeData *> genomes;
    unsigned int nindividuals;
    unsigned int born;
    unsigned int vkilled;
    unsigned int mkilled;
    unsigned int tstep;

    void AddIndividual(genome);

  
public:
    Environment();
    Environment(FILE*);
    ~Environment();
    void Write(FILE*);

    void Fill(unsigned int n);
    void Step();
    unsigned int Clear();
 
    unsigned int Size() const {
        return nindividuals;
    }

    vector<unsigned int> PopStructure() ;
    vector<double> MortStructure() ;
    vector<double> GenomeStructure();

    void UpdateStats(vector<unsigned int> *, vector<unsigned int> *, AutoHistogram<unsigned int, unsigned int> *) ;
    RuntimeParams Runtime() const;
    unsigned int Time() const;
    double Diversity() const;
    unsigned int GetNumOfGenomes(int) const;

    multimap<unsigned int, genome> GetTopRank(int n = 8);
    map<unsigned int, unsigned int> GetClusters();
    map<unsigned int, unsigned int> GetClustersHistogram(int slices = 10);
  
  
};

#endif

