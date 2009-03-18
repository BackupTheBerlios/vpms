#include <vector>
#include <list>
#include <map>

#include "model.hh"

using namespace std;

#ifndef STATS_H
#define STATS_H

class PStats {
  list<vector<unsigned int> * > pop_list;
  list<vector<unsigned int> * > mkill_list;
  const int gensize;
  
public:
  PStats();
  ~PStats();
  void Update(Environment *);  

};

template <class KType, class VType>
class TopRank {


  multimap<KType,VType> topgen;
  unsigned int capacity;
  unsigned int size;
  KType last;

public:
  TopRank(unsigned int n = 8) : capacity(n), size(0) {
    
  }
  void Put(KType key, VType val) {

    if(size < capacity) {
      topgen.insert(pair<KType,VType>(key,val));
      last=topgen.begin()->first;
      size++;
    }
    else {
      if(key > last) {
	topgen.erase(topgen.begin());
	topgen.insert(pair<KType,VType>(key,val));
	last=topgen.begin()->first;
      }
    }
  }

  multimap<KType,VType> GetRank() {
    return topgen;
  }


};

#endif
