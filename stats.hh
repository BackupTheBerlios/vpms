#include <vector>
#include <list>
#include <map>
#include <unordered_map>
#include <cmath>
#include <algorithm>

#include "model.hh"

using namespace std;

#ifndef STATS_H
#define STATS_H

template<class KType,class VType> class AutoHistogram;

class PStats {
    list<vector<unsigned int> * > pop_list;
    list<vector<unsigned int> * > mkill_list;
    list<vector<double> * > gen_list;
    list<AutoHistogram<unsigned int, unsigned int> * > hist_list;
    list<unsigned int> time_list;

    const int gensize;
  
public:
    PStats();
    ~PStats();
    void Update(Environment *);  
    vector<pair<double,double> > GetAvgPopulation();
    vector<pair<double,double> > GetAvgMortality();
    vector<pair<double,double> > GetAvgGenome();
    map<double,double> GetAvgClustersHistogram(int);
    map<unsigned, pair<double,double> > GetAvgClusters();
    list<unsigned int> GetTimeList();

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

template <class KType, class VType>
class AutoHistogram {
    unordered_map<KType,VType> hist;
  
public:
    void Put(KType key,VType val) {    
        typename unordered_map<KType,VType>::iterator iter;

        iter = hist.find(key);
        if(iter != hist.end()) {
            hist[key] += val;
        }
        else {
            hist[key] = val;
        }
    }

    KType GetMaxKey() {
        return max_element(hist.begin(),hist.end())->first;
    }


    unordered_map<KType,VType> GetClusterData() {
        return hist;
    }
  
    map<KType,VType> Generate(int nslices) {
        map<KType,VType> ahist;

        for(int i=1; i<=nslices; i++) {
            ahist[i] = 0;
        }

        KType maxkey = max_element(hist.begin(),hist.end())->first;
        double factor = static_cast<double>(maxkey)/nslices;

        typename unordered_map<KType,VType>::iterator iter, itend;
        itend=hist.end();
        for(iter=hist.begin(); iter != itend; iter++) {
            ahist[ ceil((iter->first)/factor) ] += iter->second;
        }
    
        return ahist;
    }



};

class SimpleMoments {
    list<double> data;
    list<double> squares;

    map<double, double> data_map;
    map<double, double> squa_map;

public:
    void Put(double);
    void Put(double, double);
    pair<double, double> GetMoments();
    map<unsigned, pair<double, double> > GetMoments(int);

};

#endif
