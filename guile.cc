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
#include "bitstring.hh"

using namespace std;

extern Config cfg;
extern parameters p;
extern Environment * env;

namespace glocal {
  Logging log;
  
};

void logTime(string message, clock_t start, clock_t stop) {
  ostringstream out;
  out << message << ": ";
  out << (static_cast<double>(stop-start)/CLOCKS_PER_SEC) << " sec.";
  glocal::log << out.str();
}


inline void putConfig(configParams par, string expected, string name, double value) {
  if(expected == name) {
    cfg[par] = value;
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
  putConfig(rndSeed,   string("rndSeed"),string(varName),varValue);

  scm_dynwind_end ();
}

void throw_exception(string symbol, string description) {
  scm_throw(scm_str2symbol(symbol.c_str()),
	    scm_list_1(scm_from_locale_string(description.c_str())));
}

extern "C" SCM config(SCM list) {

  if(scm_is_false(scm_list_p(list))) {
    cout << cfg << endl;
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
    initialize();
    
    ostringstream msg;
    msg << "set up config to: " << endl << cfg;
    glocal::log << msg.str();
  }
 
  return scm_from_bool(1);
}


void check_environment() {
  if (env == NULL) {
    throw_exception("environment-not-created","Environment not created yet!");
  }
}

extern "C" SCM get_state() {

  check_environment();

  RuntimeParams rp = env->Runtime();
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
  vector<unsigned int> pop = env->PopStructure();

  SCM plist = scm_list(SCM_EOL);

  for(unsigned int i=0; i<pop.size(); i++) {
    SCM entry = scm_list_1(scm_list_2(scm_uint2num(i), scm_uint2num(pop[i]) ));
    plist = scm_append(scm_list_2(plist, entry));
  }
  
  return plist;
}

extern "C" SCM get_mortality() {
  check_environment();
  vector<double> mort = env->MortStructure();
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
  multimap<unsigned int, genome> result = env->GetTopRank(rankSize);
  time_t cstop=clock();

  ostringstream buf;
  buf << "calculated top genome rank for " << rankSize << " genomes";
  logTime(buf.str(),cstart,cstop);

  multimap<unsigned int, genome>::reverse_iterator iter;

  SCM retlist = scm_list(SCM_EOL);

  for(iter=result.rbegin(); iter != result.rend(); iter++ ) {
    unsigned int nindividuals = iter->first;
    string strGenome = get_genome_repr(iter->second);

    SCM current = scm_list_2(scm_int2num(nindividuals),scm_from_locale_string(strGenome.c_str()));

    retlist = scm_append(scm_list_2(retlist, scm_list_1(current)));
							
  }
  return retlist;
}

extern "C" SCM get_time() {
  check_environment();
  unsigned int time = env->Time();
  return scm_uint2num(time);
}

extern "C" SCM do_step(SCM param) {
  check_environment();
  
  SCM ret_val = scm_from_bool(1);
  unsigned int nsteps = 1;
  clock_t cstart, cstop;

  if (scm_is_true(scm_integer_p(param))){
    nsteps = scm_to_uint(param);
  }

  cstart=clock();
  unsigned int i;
  for(i=1; i<=nsteps; i++) {
    env->Step();
    if(env->Size() <= 0) {
      cerr << "Population extinct!" << endl;
      ret_val = scm_from_bool(0);
      break;
    }
    
  }
  cstop=clock();

  ostringstream msg;
  msg << "done " << i-1 << " step(s) ";
  logTime(msg.str(),cstart,cstop);
  
  return ret_val;
}

extern "C" SCM clear_environment() {
  clock_t cstart, cstop;

  check_environment();

  cstart=clock();
  unsigned int count = env->Clear();
  cstop=clock();
 
  ostringstream msg;
  msg << "cleared environment, removed " << count << " empty genotypes";
  logTime(msg.str(),cstart,cstop);

  return scm_from_uint(count);
}

extern "C" SCM make_environment(SCM param) {

  clock_t cstart = clock();
  delete env;
  clock_t cstop = clock();

  logTime("deleted old environment (if any)",cstart,cstop);

  env = new Environment();
  unsigned int size = scm_to_uint(param);

  if(size >= p.N) {
    ostringstream info;
    info << "Environment fill can not be greater or equal capacity N=" << p.N;
    throw_exception("incorrect-fill-size", info.str());
  }

  cstart = clock();
  env->Fill(size);
  cstop = clock();
  
  ostringstream msg;
  msg << "Created environment with " << size << " individuals";
  logTime(msg.str(),cstart,cstop);

  return scm_from_bool(1);
}

void show_welcome() {
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
  scm_c_define_gsubr("warranty",0,0,0, show_warranty);
  scm_c_define_gsubr("get-state",0,0,0, get_state);
  scm_c_define_gsubr("get-population",0,0,0,get_population);
  scm_c_define_gsubr("get-mortality",0,0,0,get_mortality);
  scm_c_define_gsubr("get-genome-ranking",0,1,0,(SCM (*)()) get_genome_ranking);
  scm_c_define_gsubr("get-time",0,0,0, get_time);
  scm_c_define_gsubr("do-step",0,1,0,(SCM (*)())do_step);
  scm_c_define_gsubr("optimize-environment",0,0,0,clear_environment);
  scm_c_primitive_load ("init.scm");
  scm_shell (argc, argv);
}

void init_guile(int argc, char **argv) {
  scm_boot_guile (argc, argv, vpms_main, 0);
}
