#include <iostream>
#include <numeric>
#include <unordered_map>
#include <set>

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


  list<vector<double> *>::const_iterator it2, iend2;
  iend2 = gen_list.end();
  for(it2=gen_list.begin(); it2 != iend2; it2++) {
    delete *it2;
  }

  list<AutoHistogram<unsigned int, unsigned int> * >::const_iterator iterm, iendm;

  iendm  = hist_list.end();
  for(iterm=hist_list.begin(); iterm != iendm; iterm++) {
    delete *iterm;
  }

}

void PStats::Update(Environment *env) {
  vector <unsigned int> * pop  = new vector<unsigned int>(gensize,0u);
  vector <unsigned int> * mkill= new vector<unsigned int>(gensize,0u);
  AutoHistogram <unsigned int, unsigned int> * hist = new AutoHistogram<unsigned int, unsigned int>();
  vector <double> * genstr = new vector<double>(env->GenomeStructure());

  env->UpdateStats(pop,mkill,hist);

  pop_list.push_back(pop);
  mkill_list.push_back(mkill);
  hist_list.push_back(hist);
  gen_list.push_back(genstr);

  time_list.push_back(env->Time());
  
}

namespace vpms {
  int calcCommonMaxAvailable(list<vector<unsigned int> * > series) {
    list<vector<unsigned int> *>::const_iterator it;
    
    // if there is no entries at list, just return
    if(series.begin() == series.end() ) {
      return 0;
    }

    int maxav = (*series.begin())->size();
    
    for(it=series.begin(); it != series.end(); it++) {
      vector <unsigned int> v = *(*it) ;
      int currmax = 0;
      for(int i=v.size()-1; i>=0; i--) {
	if(v[i] != 0) {
	  currmax = i+1;
	  break;
	}
      }
      if(currmax < maxav) {
	maxav = currmax;
      }
    }

    return maxav;
  }
}

map<unsigned, pair<double,double> > PStats::GetAvgClusters() {
  
  SimpleMoments moments;
  
  for(list<AutoHistogram<unsigned int, unsigned int> * >::const_iterator it =
	hist_list.begin(); it != hist_list.end(); it++) {

    unordered_map<unsigned int, unsigned int> clusters = (*(*it)).GetClusterData();
    for(unordered_map<unsigned, unsigned>::const_iterator it2 = clusters.begin();
	it2 != clusters.end(); it2++) {
      moments.Put(it2->first, it2->second);
    }
  }
  
  return moments.GetMoments(hist_list.size());

}

map<double, double> PStats::GetAvgClustersHistogram(int nslices) {
  map<unsigned, pair<double, double> > clusters = this->GetAvgClusters();

  map<unsigned,double> ahist;

    for(int i=1; i<=nslices; i++) {
      ahist[i] = 0;
    }

  unsigned maxkey = max_element(clusters.begin(),clusters.end())->first;
  double factor = static_cast<double>(maxkey)/nslices;

  for( map<unsigned, pair<double,double> >::const_iterator it = clusters.begin();
       it != clusters.end(); it++) {
    ahist[ ceil((it->first)/factor) ] += (it->second).first;
  }

  map<double,double> shist;

  for(map<unsigned,double>::const_iterator it = ahist.begin();
      it != ahist.end(); it++) {
    double newkey = static_cast<double>(it->first) * maxkey / static_cast<double>(nslices);
    shist[ newkey ] = it->second;
  }

  return shist;
}


vector<pair<double,double> > PStats::GetAvgGenome() {
  vector<SimpleMoments> moments((*gen_list.begin())->size());
  
  for( list<vector<double> * >::const_iterator it = gen_list.begin();
       it != gen_list.end(); it++) {
    vector<double> current = **it;
    for(unsigned i=0; i < current.size(); i++) {
      moments[i].Put(current[i]);
    }
  }

  vector<pair<double, double> > retstats(moments.size());

  for(unsigned i=0; i<moments.size(); i++) {
    retstats[i] = moments[i].GetMoments();
  }
  
  return retstats;
}



vector<pair<double,double> > PStats::GetAvgPopulation() {
  
  int maxAvail = vpms::calcCommonMaxAvailable(pop_list);
  vector<pair<double, double> > pop(maxAvail,pair<double,double>(0,0));
  list<vector<unsigned int> * >::const_iterator it,itend;
  vector<SimpleMoments> moments(maxAvail);


  itend=pop_list.end();
  for(it=pop_list.begin(); it != itend; it++) {
    for(int i=0; i<maxAvail; i++) {
      moments[i].Put((**it)[i]);
    }

  }

  vector<pair<double, double> > retstats(maxAvail);

  for(int i=0; i<maxAvail; i++) {
    retstats[i] = moments[i].GetMoments();
  }


  return retstats;
}

vector<pair<double,double> > PStats::GetAvgMortality() {

  int maxAvail = vpms::calcCommonMaxAvailable(pop_list);
  vector<SimpleMoments> moments(maxAvail);
  list<vector<unsigned int> * >::const_iterator it1, it2;

  for(it1=mkill_list.begin(), it2=pop_list.begin();
      it1 != mkill_list.end() && it2 != pop_list.end();
      it1++, it2++) {

    for(int i=0; i<maxAvail; i++) {
      double mkill = static_cast<double>((*(*it1))[i]);
      unsigned pop   = (*(*it2))[i];
      moments[i].Put(mkill/pop);
    }
    
  }
  vector<pair<double, double> > retstats(maxAvail);
  
  for(int i=0; i<maxAvail; i++) {
    retstats[i] = moments[i].GetMoments();
  }

  return retstats;

}

list<unsigned int> PStats::GetTimeList() {
  return time_list;
}


void SimpleMoments::Put(double d) {
  data.push_back(d);
  squares.push_back(d*d);
}

void SimpleMoments::Put(double k, double v) {
  if(data_map.find(k) != data_map.end()) {
    data_map[k] += v;
    squa_map[k] += v*v;
  }
  else {
    data_map[k] = v;
    squa_map[k] = v;
  }
}

map<unsigned, pair<double, double> > SimpleMoments::GetMoments(int scale) {
  map<unsigned, pair<double, double> > moments;

  for(map<double,double>::const_iterator it = data_map.begin();
      it != data_map.end(); it++) {
    double sum = it->second;
    double sumsq = squa_map[it->first];
    double avg = sum / scale;
    double sig = sqrt( sumsq/scale - avg*avg );
    moments[it->first] = pair<double,double>(avg,sig);
  }

  return moments;
}

pair<double,double> SimpleMoments::GetMoments() {

  pair<double, double> moments;

  double sum   = accumulate(data.begin(), data.end(), 0.0);
  double sumsq = accumulate(squares.begin(), squares.end(), 0.0);
  int length = data.size();

  double avg = sum/length;
  double sig = sqrt( sumsq/length - avg*avg );

  moments.first  = avg;
  moments.second = sig;

  return moments;
}
