#include <iostream>
#include <numeric>
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
	  currmax = i;
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

vector<pair<double,double> > PStats::GetAvgPopulation() {
  
  int maxAvail = vpms::calcCommonMaxAvailable(pop_list);
  vector<pair<double, double> > pop(maxAvail,pair<double,double>(0,0));
  list<vector<unsigned int> * >::const_iterator it,itend;
  vector<SimpleMoments> moments(maxAvail);


  itend=pop_list.end();
  for(it=pop_list.begin(); it != itend; it++) {
    for(int i=0; i<maxAvail; i++) {
      moments[i].Put((*(*it))[i]);
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

pair<double,double> SimpleMoments::GetMoments() {
  list<double>::iterator it_data, it_squares;
  pair<double, double> moments;

  double sum   = accumulate(data.begin(), data.end(), 0.0);
  double sumsq = accumulate(squares.begin(), squares.end(), 0.0);
  int length = data.size();

  moments.first  = sum/length;
  moments.second = sqrt((sum*sum - sumsq)/length);

  return moments;
}
