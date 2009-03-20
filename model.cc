/*
  VPMS - Penna Model Simulation
  
  Copyleft: <Mikolaj Sitarz> sitarz@gmail.com
  2009

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation,  version 3
  
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
   
  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <unordered_map>
#include <cstring>
#include <list>
#include <sstream>

#include "MersenneTwister.h"
#include "stats.hh"
#include "model.hh"
#include "vpms.hh"
#include "stats.hh"

using namespace std;

extern MTRand vpms::random;

namespace vpms {

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


  int calcPrValue(double v) {
    int n = static_cast<int>(v);
    if ( v - n > random() ) {
      n++;
    }
    return n;
  }

  inline void addPositiveMutations(genome *g, int n) {
    for(int i=1; i<=n; i++) {
      int pos = static_cast<int>(8*sizeof(genome) * random());
      setbit(g,pos,0);
    }
  }

  inline void addNegativeMutations(genome *g, int n) {
    for(int i=1; i<=n; i++) {
      int pos = static_cast<int>(8*sizeof(genome) * random());
      setbit(g,pos,1);
    }
  }

  inline genome *mutateGenome(const genome &g) {
    int nmutations=0;
    int pmutations=0;
    
    if(vpms::p.M > 0) {
      nmutations = calcPrValue(vpms::p.M);
    }
    if(vpms::p.P > 0) {
      pmutations = calcPrValue(vpms::p.P);
    }
    
    genome *newGenome = new genome(g);
    addNegativeMutations(newGenome,nmutations);
    addPositiveMutations(newGenome,pmutations);
    return newGenome;
  }

  
  string get_genome_repr(genome g) {
    ostringstream rep;
    for(int i=8*sizeof(g)-1; i>=0; i--) {
      rep << static_cast<int>(getbit(g,i));
    }
    rep << ends;
    return rep.str();
  }

}



Environment::Environment(): genomes(vpms::p.N) {
  nindividuals = 0;
  born = 0;
  vkilled = 0;
  mkilled = 0;
  tstep = 0;
}

Environment::~Environment() {
#if DEBUG_LEVEL > 0
  cerr << "Destroying environment..." ;
#endif

  unordered_map<genome, GenomeData *>::iterator iter;
  iter=genomes.begin();
  for(iter=genomes.begin(); iter != genomes.end(); iter++) {
    GenomeData *gd = iter->second;
    genomes.erase(iter);
    delete gd;
  } 
#if DEBUG_LEVEL > 0
  cerr << endl;
#endif

}


void Environment::Fill(unsigned int n) {
#ifdef DEBUG_LEVEL
  cerr << "filling environment with " << n << " individuals..." << endl;
#endif

  for(unsigned int i=1; i<=n; i++) {
    genome *g = new genome(0u);
    for(unsigned int j=0; j<8*sizeof(g); j++) {
      vpms::setbit(g,j,(vpms::random() < vpms::p.initGenome)?1:0);
    }
    this->AddIndividual(*g);

#if DEBUG_LEVEL > 1
    cerr << "adding genome: ";
    cerr << vpms::get_genome_repr(*g) << endl;
#endif
  }

  nindividuals = n;
}

void Environment::AddIndividual(genome g) {
  unordered_map<genome, GenomeData *>::iterator iter =  genomes.find(g);

  if(iter != genomes.end()) { // environment already contains genome
    GenomeData *gd = iter->second;
    gd->AddIndividual();
  }
  else {
    GenomeData *gd = new GenomeData(g);
    gd->AddIndividual();
    genomes[g] = gd;
  }
  nindividuals += 1;
}

unsigned int Environment::Clear() {
  unordered_map<genome, GenomeData *>::iterator iter;
  unsigned int count=0;
  for(iter=genomes.begin(); iter != genomes.end(); iter++) {
    GenomeData *gd = iter->second;
    if(gd->Size() == 0) {
#if DEBUG_LEVEL > 1
      cerr << "erasing genome: ";
      vpms::get_genome_repr(iter->first);
#endif
      count++;
      genomes.erase(iter);
      delete gd;
    }
  }
  return count;
}

void Environment::Step() {
  
  unordered_map<genome, GenomeData *>::iterator iter, iend;

  this->mkilled = 0;
  this->vkilled = 0;
  this->born = 0;

  double vfactor = static_cast<double>(this->nindividuals) / vpms::p.N;

  list<BornGenome *> waitingRoom;

  // run over genomes and perform aging, killing and calc birth
  iend = genomes.end();
  for(iter=genomes.begin(); iter != iend; iter++) {
    GenomeData *gd = iter->second;
    if(gd->Size() > 0) {
      unsigned int newborn = 0;
      mkilled += gd->MakeOlder();
      vkilled += gd->VerhulstKill(vfactor);
      newborn  = gd->GiveBirth();
      born    += newborn;      
    
      if (newborn > 0) {
	BornGenome *bornInfo = new BornGenome();
	bornInfo->nborn = newborn;
	bornInfo->baseGenome = iter->first;
	waitingRoom.push_back(bornInfo);
      }
    }
  }

  list<BornGenome *>::iterator listIter;
  // put new born to the population
  while(!waitingRoom.empty()) {
    listIter = waitingRoom.begin();
    BornGenome *bornInfo = (*listIter);
    waitingRoom.pop_front();
    
    for(unsigned int i=1; i<=bornInfo->nborn; i++) {
      genome *g = vpms::mutateGenome(bornInfo->baseGenome);
      AddIndividual(*g);
      delete g; g=NULL;
    }
    delete bornInfo; bornInfo=NULL;
  }
  
  // update number of individuals
  nindividuals -= (vkilled + mkilled);

  tstep += 1;
}

RuntimeParams Environment::Runtime() const {
  RuntimeParams rp;
  rp.nindividuals = nindividuals;
  rp.born = born;
  rp.mkilled = mkilled;
  rp.vkilled = vkilled;

  return rp;
}

unsigned int Environment::Time() const {
  return tstep;
}

double Environment::Diversity() const {
  // TODO 
  // 1.0 all individuals have the same genome
  // 0.0 there are no two individuals in population having the same genome

  // sredni rozmiar klastra dzielony przez liczbę osobników
  // przemysleć
  return 0.0;
}

// to be removed -  MortStructure() is sufficient
vector <unsigned int> Environment::PopStructure() {
  vector <unsigned int> pop(sizeof(int)*8,0u);
  unordered_map<genome, GenomeData *>::iterator iter, iend;
  iend = genomes.end();

  for(iter=genomes.begin(); iter != iend; iter++ ) {
    GenomeData *gd = iter->second;
    gd->UpdatePopStats(pop);
  }

  return pop;
}

/**
   Calculate mortality caused by genetic reasons
   for whole population.
   Method should be called after the "Step" is done.
 */
vector <double> Environment::MortStructure()  {
  vector <unsigned int> pop(sizeof(int)*8,0u);
  vector <unsigned int> mkill(sizeof(int)*8,0u);

  unordered_map<genome, GenomeData *>::iterator iter, iend;
  iend = genomes.end();
  for(iter=genomes.begin(); iter != iend; iter++) {
    GenomeData *gd = iter->second;
    if(gd->Size() > 0) {
      gd->UpdateMortStats(pop,mkill);
    }
  }
  
  vector<double> mortality(pop.size(),0.0);
  for(unsigned int i=0; i<pop.size(); i++) {
    if(pop[i] != 0) {
      mortality[i] = static_cast<double>(mkill[i]) / pop[i];
    }
    else {
      mortality[i] = 0.0;
    }
  }

  return mortality;
}

void Environment::UpdateStats(vector<unsigned int> * pop, vector<unsigned int> * mkill)  {

  unordered_map<genome, GenomeData *>::iterator iter, iend;
  /*  iend = genomes.end();
  for(iter=genomes.begin(); iter != genomes.end(); iter++) {
    GenomeData *gd = iter->second;
    if(gd->Size() > 0) {
      gd->UpdateMortStats(*pop,*mkill);
    }
    } */
}

multimap<unsigned int, genome> Environment::GetTopRank(int n)  {
  unordered_map<genome, GenomeData *>::iterator iter, iend;

  TopRank<unsigned int, genome> rank(n);


  iend = genomes.end();
  for(iter=genomes.begin(); iter != genomes.end(); iter++) {
    GenomeData *gd = iter->second;
    if(gd->Size() > 0) {
      rank.Put(gd->Size(),iter->first);
    }
  }
  
  return rank.GetRank();

}

map<unsigned int, unsigned int> Environment::GetClusters(){
  AutoHistogram<unsigned int, unsigned int> hist;

  { // collect data
    unordered_map<genome,GenomeData *>::iterator iter, itend;
    itend = genomes.end();
    for(iter=genomes.begin(); iter != itend; iter++) {
      unsigned int size = iter->second->Size();
      if(size > 0) {
	hist.Put(size,size);
      }
    }
  }

  unordered_map<unsigned int, unsigned int> raw_clusters = hist.GetClusterData();
  unordered_map<unsigned int, unsigned int>::iterator iter, itend;
  map<unsigned int, unsigned int> sorted_map;

  itend=raw_clusters.end();
  for(iter=raw_clusters.begin(); iter != itend; iter++) {
    sorted_map[iter->first] = iter->second;
  }
  return sorted_map;
}

map<unsigned int, unsigned int> Environment::GetClustersHistogram(int nslices) {
  AutoHistogram<unsigned int, unsigned int> hist;
  

  unordered_map<genome,GenomeData *>::iterator iter, itend;
  itend = genomes.end();
  for(iter=genomes.begin(); iter != itend; iter++) {
    unsigned int size = (*(iter->second)).Size();
    if(size > 0) {
      hist.Put(size,size);
    }
  }



  return hist.Generate(nslices);
}

// =============================================================

inline int GenomeData::CountMaxAge(genome g) {
  int nmut=0;
  for(unsigned int i=0; i<8*sizeof(genome); i++) {
    nmut += vpms::getbit(g,i);
    if(nmut >= vpms::p.T) {
      return i+1;
    }
  }
  return 8*sizeof(genome);
}

GenomeData::GenomeData(genome g) {
  maxage=CountMaxAge(g);
  ages = new unsigned int[maxage];
  memset(ages,0u,maxage*sizeof(unsigned int));
  nindividuals = 0u;
}

GenomeData::~GenomeData() {
  delete[] ages;
#ifdef DEBUG_LEVEL
  cerr << nindividuals << " " ;
#endif
}

void GenomeData::AddIndividual(unsigned int age) {
  ages[age]++;
  nindividuals++;
}

// make individuals older and perform genetic death
// @return number of genetically killed individuals
unsigned int GenomeData::MakeOlder() { 
  unsigned int nkilled = ages[maxage-1];
  nindividuals -= nkilled;
  for(int i=maxage-1; i > 0; i--) {
    ages[i] = ages[i-1];
  }
  ages[0] = 0;
  return nkilled;
}

unsigned int GenomeData::VerhulstKill(double vfactor) {
  unsigned int nkilled = 0;

  for(unsigned int a=1; a < maxage; a++) { // do not kill with a=0
    unsigned int curinds = ages[a];
    for(unsigned int i=1; i <= curinds; i++) {
      if(vpms::random() < vfactor) {
	 ages[a]--;
	 nkilled++;
      }
    }
  }
  nindividuals -= nkilled;
  return nkilled;
}

unsigned int GenomeData::GiveBirth() {
  unsigned int nborn = 0;
  for(unsigned int a = vpms::p.R; a < maxage; a++) {
    for(unsigned int i=1; i<=ages[a]; i++) {
      nborn += vpms::calcPrValue(vpms::p.B);
    }
  }
  return nborn;
}

unsigned int GenomeData::Size() const {
  return nindividuals;
}



void GenomeData::UpdatePopStats(vector<unsigned int> &pop) const {
  for(int i=0; i<maxage; i++) {
    pop[i] += ages[i];
  }
}

void GenomeData::UpdateMortStats(vector<unsigned int> &pop, vector<unsigned int> &mkill) const {
  for(int i=0; i<maxage; i++) {
    pop[i] += ages[i];
  }
  mkill[maxage-1] += ages[maxage-1];
}
