/*
  VPMS - Penna Model Simulation
  
  Copyleft 2009: <Mikolaj Sitarz> sitarz@gmail.com

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

#include <libguile.h>
#include <iostream>
#include <iomanip>
#include <sstream>

#include "vpms.hh"
#include "config.hh"
#include "model.hh"
#include "fileio.hh"
#include "stats.hh"

using namespace std;

namespace vpms {
  extern Environment * env;
  extern Config cfg;
  extern Logging log;
  extern PStats *stats;
}

SCM vectorpd2list(const vector<pair<double, double> > &v) {
  SCM plist = scm_list(SCM_EOL);

  for(unsigned int i=0; i<v.size(); i++) {
    SCM entry = scm_list_1(scm_list_3(
				      scm_uint2num(i), 
				      scm_double2num(v[i].first),  
				      scm_double2num(v[i].second) ));
    plist = scm_append(scm_list_2(plist,entry));
  }
  
  return plist;
}

void logTime(string message, clock_t start, clock_t stop) {
  ostringstream out;
  out << message << ": ";
  out << (static_cast<double>(stop-start)/CLOCKS_PER_SEC) << " sec.";
  vpms::log << out.str();
}


inline void putConfig(configParams par, string expected, string name, double value) {
  if(expected == name) {
    vpms::cfg.SetParam(par,value);
  }
}

inline void saveConfig(SCM key, SCM val) {

  scm_dynwind_begin ((scm_t_dynwind_flags)0);
  char *varName = scm_to_locale_string(scm_symbol_to_string(key));
  double varValue = scm_to_double(val);
  scm_dynwind_free (varName); // collect pointer for further clean-up

  putConfig(N,         string("N"),string(varName),varValue);
  putConfig(B,         string("B"),string(varName),varValue);
  putConfig(R,         string("R"),string(varName),varValue);
  putConfig(T,         string("T"),string(varName),varValue);
  putConfig(M,         string("M"),string(varName),varValue);
  putConfig(P,         string("P"),string(varName),varValue);
  putConfig(initGenome,string("initGenome"),string(varName),varValue);
  putConfig(randomSeed,   string("randomSeed"),string(varName),varValue);

  scm_dynwind_end ();
}

void throw_exception(string symbol, string description) {
  scm_throw(scm_str2symbol(symbol.c_str()),
	    scm_list_1(scm_from_locale_string(description.c_str())));
}

extern "C" SCM config(SCM list) {

  if(scm_is_false(scm_list_p(list))) {
    cout << vpms::cfg << endl;
  }
  else {

    SCM element;
    do {
      element = scm_car(list);
      list    = scm_cdr(list);
 
      SCM key = scm_car(element);
      SCM val = scm_cadr(element);
      
      saveConfig(key, val);
      
    }  while(scm_is_false(scm_null_p(list) )) ;
    
    ostringstream msg;
    msg << "set up config to: " << endl << vpms::cfg;
    vpms::log << msg.str();
  }
 
  return scm_from_bool(1);
}


void check_environment() {
  if (vpms::env == NULL) {
    throw_exception("environment-not-created","Environment not created yet!");
  }
}

void check_avg() {
  if(vpms::stats == NULL) {
    throw_exception("stats-not-calculated","Statistics were not calculated yet");
  }
}

extern "C" SCM get_state() {

  check_environment();

  RuntimeParams rp = vpms::env->Runtime();
  SCM scm_nindividuals =  scm_list_2(
				   scm_str2symbol("nindividuals") ,
				   scm_uint2num(rp.nindividuals));

  SCM scm_born  =  scm_list_2(
			    scm_str2symbol("born") ,
			    scm_uint2num(rp.born));

  SCM scm_mkilled =  scm_list_2(
			      scm_str2symbol("mkilled") ,
			      scm_uint2num(rp.mkilled));
  SCM scm_vkilled =  scm_list_2(
			      scm_str2symbol("vkilled") ,
			      scm_uint2num(rp.vkilled));

  return scm_list_4(scm_nindividuals, scm_born, scm_mkilled, scm_vkilled);

}

extern "C" SCM get_population() {
  check_environment();
  vector<unsigned int> pop = vpms::env->PopStructure();

  SCM plist = scm_list(SCM_EOL);

  for(unsigned int i=0; i<pop.size(); i++) {
    SCM entry = scm_list_1(scm_list_2(scm_uint2num(i), scm_uint2num(pop[i]) ));
    plist = scm_append(scm_list_2(plist, entry));
  }
  
  return plist;
}

extern "C" SCM get_mortality() {
  check_environment();
  vector<double> mort = vpms::env->MortStructure();
  SCM plist = scm_list(SCM_EOL);

  for(unsigned int i=0; i<mort.size(); i++) {
    SCM entry = scm_list_1(scm_list_2(scm_uint2num(i), scm_double2num(mort[i])));
    plist = scm_append(scm_list_2(plist,entry));
  }
  return plist;
}

extern "C" SCM get_genome_ranking(SCM number) {
  check_environment();
  int rankSize=8;

  if(scm_is_true(scm_integer_p(number))) {
    rankSize = scm_to_int(number);
  }

  time_t cstart=clock();
  multimap<unsigned int, genome> result = vpms::env->GetTopRank(rankSize);
  time_t cstop=clock();

  ostringstream buf;
  buf << "calculated top genome rank for " << rankSize << " genomes";
  logTime(buf.str(),cstart,cstop);

  multimap<unsigned int, genome>::const_reverse_iterator iter;

  SCM retlist = scm_list(SCM_EOL);

  for(iter=result.rbegin(); iter != result.rend(); iter++ ) {
    unsigned int nindividuals = iter->first;
    string strGenome = vpms::get_genome_repr(iter->second);

    SCM current = scm_list_2(scm_int2num(nindividuals),scm_from_locale_string(strGenome.c_str()));

    retlist = scm_append(scm_list_2(retlist, scm_list_1(current)));
							
  }
  return retlist;
}

extern "C" SCM get_genome_distribution() {
  check_environment();

  time_t cstart=clock();
  vector<double> gens = vpms::env->GenomeStructure();
  time_t cstop=clock();
  logTime("obtained genome structure",cstart,cstop);
  
  SCM retlist = scm_list(SCM_EOL);
  for (unsigned int i=0; i<gens.size(); i++) {
    SCM current = scm_list_2(scm_uint2num(i),scm_double2num(gens[i]));
    retlist = scm_append(scm_list_2(retlist,scm_list_1(current)));
  }

  return retlist;
}

extern "C" SCM get_cluster_data() {
  check_environment();

  map<unsigned int, unsigned int> clusters =  vpms::env->GetClusters();
  map<unsigned int, unsigned int>::const_iterator iter, itend;
  SCM retlist = scm_list(SCM_EOL);

  itend = clusters.end();
  for(iter=clusters.begin(); iter != itend; iter++){
    SCM current = scm_list_2(scm_uint2num(iter->first),scm_uint2num(iter->second));
    retlist = scm_append(scm_list_2(retlist,scm_list_1(current)));
  }
  

  return retlist;
}

extern "C" SCM get_cluster_histogram(SCM param) {
  check_environment();

  int size = 10;
  if(scm_is_true(scm_integer_p(param))) {
    size = scm_to_uint(param);
  }

  map<unsigned int, unsigned int> clusters =  vpms::env->GetClustersHistogram(size);
  map<unsigned int, unsigned int>::const_iterator iter, itend;
  SCM retlist = scm_list(SCM_EOL);

  itend = clusters.end();
  for(iter=clusters.begin(); iter != itend; iter++){
    SCM current = scm_list_2(scm_uint2num(iter->first),scm_uint2num(iter->second));
    retlist = scm_append(scm_list_2(retlist,scm_list_1(current)));
  }
  

  return retlist;
}

extern "C" SCM get_time() {
  check_environment();
  unsigned int time = vpms::env->Time();
  return scm_uint2num(time);
}

extern "C" SCM do_step(SCM param,SCM calc_stats) {
  check_environment();
  
  SCM ret_val = scm_from_bool(1);
  unsigned int nsteps = 1;
  clock_t cstart, cstop;

  bool do_stats=false;

  if(scm_is_true(scm_boolean_p(calc_stats))) {
    do_stats = scm_to_bool(calc_stats);
  }

  if(do_stats) {
    delete vpms::stats;
    vpms::stats = new PStats();
  }

  if (scm_is_true(scm_integer_p(param))){
    nsteps = scm_to_uint(param);
  }

  cstart=clock();
  unsigned int i;
  for(i=1; i<=nsteps; i++) {
    vpms::env->Step();
    if(do_stats) {
      vpms::stats->Update(vpms::env);
    }
    if(vpms::env->Size() <= 0) {
      cerr << endl << " *** Population extinct! *** " << endl;
      throw_exception("population-extinct","Population extinct");
      break;
    }
    
  }
  cstop=clock();

  ostringstream msg;
  msg << "done " << i-1 << " step(s) ";
  if(do_stats) {
    msg << " + average calculations";
  }
  logTime(msg.str(),cstart,cstop);
  
  return ret_val;
}

extern "C" SCM get_avg_population() {
  check_environment();
  check_avg();
  return vectorpd2list(vpms::stats->GetAvgPopulation());
}

extern "C" SCM get_avg_times() {
  check_environment();
  check_avg();

  list<unsigned int> times = vpms::stats->GetTimeList();
  list<unsigned int>::const_iterator iter;

  SCM retlist = scm_list(SCM_EOL);

  for(iter=times.begin(); iter != times.end(); iter++){
    SCM current = scm_uint2num(*iter);
    retlist = scm_append(scm_list_2(retlist,scm_list_1(current)));
  }
  return retlist;
}

extern "C" SCM clear_environment() {
  clock_t cstart, cstop;

  check_environment();

  cstart=clock();
  unsigned int count = vpms::env->Clear();
  cstop=clock();
 
  ostringstream msg;
  msg << "cleared environment, removed " << count << " empty genotypes";
  logTime(msg.str(),cstart,cstop);

  return scm_from_uint(count);
}

extern "C" SCM make_environment(SCM param) {

  clock_t cstart = clock();
  delete vpms::env;
  delete vpms::stats;
  vpms::env = NULL;
  vpms::stats = NULL;
  clock_t cstop = clock();

  logTime("deleted old environment (if any)",cstart,cstop);

  vpms::env = new Environment();
  unsigned int size = scm_to_uint(param);

  if(size >= vpms::p.N) {
    ostringstream info;
    info << "Environment fill can not be greater or equal capacity N=" << vpms::p.N;
    throw_exception("incorrect-fill-size", info.str());
  }

  cstart = clock();
  vpms::env->Fill(size);
  cstop = clock();
  
  ostringstream msg;
  msg << "Created environment with " << size << " individuals";
  logTime(msg.str(),cstart,cstop);

  return scm_from_bool(1);
}

extern "C" SCM set_logging(SCM loggingb) {
  bool blog = scm_is_true(loggingb);
  vpms::log.SetScreenDisplay(blog);
  return scm_from_bool(loggingb);
}

extern "C" void show_welcome() {
  cout << endl;
  cout << "VPMS  Copyright (C) 2009  Mikolaj Sitarz" << endl;
  cout << "This program comes with ABSOLUTELY NO WARRANTY; for details type `(warranty)'." << endl;
  cout << "This is free software, and you are welcome to redistribute it" << endl;
  cout << "under certain conditions; see LICENSE file details." << endl << endl;
}

extern "C" SCM show_warranty() {
  cout << endl;
  cout << "THERE IS NO WARRANTY FOR THE PROGRAM, TO THE EXTENT PERMITTED BY" << endl;
  cout << "APPLICABLE LAW.  EXCEPT WHEN OTHERWISE STATED IN WRITING THE COPYRIGHT" << endl;
  cout << "HOLDERS AND/OR OTHER PARTIES PROVIDE THE PROGRAM \"AS IS\" WITHOUT WARRANTY" << endl;
  cout << "OF ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING, BUT NOT LIMITED TO," << endl;
  cout << "THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR" << endl;
  cout << "PURPOSE.  THE ENTIRE RISK AS TO THE QUALITY AND PERFORMANCE OF THE PROGRAM" << endl;
  cout << "IS WITH YOU.  SHOULD THE PROGRAM PROVE DEFECTIVE, YOU ASSUME THE COST OF" << endl;
  cout << "ALL NECESSARY SERVICING, REPAIR OR CORRECTION." << endl << endl;
  return scm_from_bool(1);
}

void vpms_main (void *closure, int argc, char **argv)
{
  show_welcome();

  scm_c_define_gsubr("config",0,1,0,(SCM (*)()) config);
  scm_c_define_gsubr("create-environment",1,0,0,(SCM (*)() ) make_environment);
  scm_c_define_gsubr("optimize-environment",0,0,0,clear_environment);
  scm_c_define_gsubr("do-step",0,2,0,(SCM (*)())do_step);

  scm_c_define_gsubr("get-state",0,0,0, get_state);
  scm_c_define_gsubr("get-population",0,0,0,get_population);
  scm_c_define_gsubr("get-mortality",0,0,0,get_mortality);
  scm_c_define_gsubr("get-genome-ranking",0,1,0,(SCM (*)()) get_genome_ranking);
  scm_c_define_gsubr("get-time",0,0,0, get_time);
  scm_c_define_gsubr("get-genome-distribution",0,0,0, get_genome_distribution);
  scm_c_define_gsubr("get-clusters-histogram",0,1,0,(SCM (*)()) get_cluster_histogram);
  scm_c_define_gsubr("_vpms-get-clusters",0,0,0, get_cluster_data);

  scm_c_define_gsubr("get-avg-population",0,0,0,get_avg_population);
  scm_c_define_gsubr("get-avg-times",0,0,0,get_avg_times);

  scm_c_define_gsubr("logging",1,0,0,(SCM (*)()) set_logging);
  scm_c_define_gsubr("warranty",0,0,0, show_warranty);

  scm_c_primitive_load ("init.scm");
  scm_shell (argc, argv);
}

void init_guile(int argc, char **argv) {
  scm_boot_guile (argc, argv, vpms_main, 0);
}
