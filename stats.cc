#include <iostream>
#include "stats.hh"
#include "model.hh"

using namespace std;

PStats::PStats(): gensize(sizeof(genome)*8) {}

PStats::~PStats() {
  list<vector<unsigned int> * >::const_iterator iter, iend;

  iend = pop_list.end();
  for(iter=pop_list.begin(); iter != iend; iter++) {
    delete *iter;
  }

  iend = mkill_list.end();
  for(iter=mkill_list.begin(); iter != iend; iter++) {
    delete *iter;
  }


  iend = gen_list.end();
  for(iter=gen_list.begin(); iter != iend; iter++) {
    delete *iter;
  }

  list<map<unsigned int, unsigned int> * >::const_iterator iterm, iendm;

  iendm  = clust_list.end();
  for(iterm=clust_list.begin(); iterm != iendm; iterm++) {
    delete *iterm;
  }

}

void PStats::Update(Environment *env) {
  vector <unsigned int> * pop  = new vector<unsigned int>(gensize,0u);
  vector <unsigned int> * mkill= new vector<unsigned int>(gensize,0u);
  env->UpdateStats(pop,mkill);

  pop_list.push_back(pop);
  mkill_list.push_back(mkill);
  
  

}



