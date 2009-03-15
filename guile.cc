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

using namespace std;

extern Config cfg;
extern parameters p;
extern Environment *env;


inline void putConfig(configParams par, string expected, string name, double value) {
  if(expected == name) {
    cfg[par] = value;
  }
}

void throw_exception(string symbol, string description) {
  scm_throw(scm_str2symbol(symbol.c_str()),
	    scm_list_1(scm_from_locale_string(description.c_str())));
}

extern "C" SCM read_config() {

  SCM lsymbol = scm_c_lookup("config");
  SCM list = scm_variable_ref(lsymbol);
  int lsize = scm_to_uint(scm_length(list)); 
  SCM retVal = scm_from_bool(1);



  scm_dynwind_begin ((scm_t_dynwind_flags)0);

   for(int i=0; i<lsize; i++) {
    SCM sublist = scm_list_ref(list,scm_from_int(i));
    int ssize = scm_to_uint(scm_length(sublist));
    if(ssize != 2) {
      cerr << "Warning, incorrect config format" << endl;
      retVal = scm_from_bool(0);
    }
    
    SCM refName  = scm_list_ref(sublist,scm_from_int(0));
    SCM refValue = scm_list_ref(sublist,scm_from_int(1));



    char *varName = scm_to_locale_string(scm_symbol_to_string(refName));
    double varValue = scm_to_double(refValue);
    scm_dynwind_free (varName); // collect pointer for further clean-up
    
    putConfig(N,         string("N"),string(varName),varValue);
    putConfig(B,         string("B"),string(varName),varValue);
    putConfig(R,         string("R"),string(varName),varValue);
    putConfig(T,         string("T"),string(varName),varValue);
    putConfig(M,         string("M"),string(varName),varValue);
    putConfig(P,         string("P"),string(varName),varValue);
    putConfig(initGenome,string("initGenome"),string(varName),varValue);
    putConfig(rndSeed,   string("rndSeed"),string(varName),varValue);



   }

   scm_dynwind_end ();

   cout << cfg << endl;
   initialize();
   return retVal;
}

void check_environment() {
  if (env == NULL) {
    throw_exception("environment-not-created","Environment not created yet!");
  }
}

extern "C" SCM get_state() {

  check_environment();

  RuntimeParams rp = env->Runtime();
  SCM scm_nindividuals =  scm_cons(
				   scm_str2symbol("nindividuals") ,
				   scm_uint2num(rp.nindividuals));

  SCM scm_born  =  scm_cons(
			    scm_str2symbol("born") ,
			    scm_uint2num(rp.born));

  SCM scm_mkilled =  scm_cons(
			      scm_str2symbol("mkilled") ,
			      scm_uint2num(rp.mkilled));
  SCM scm_vkilled =  scm_cons(
			      scm_str2symbol("vkilled") ,
			      scm_uint2num(rp.vkilled));

  return scm_list_4(scm_nindividuals, scm_born, scm_mkilled, scm_vkilled);

}

extern "C" SCM print_state() {

  check_environment();
  RuntimeParams rp = env->Runtime();

  cout << endl;
  cout << setw(15) << "individuals = " << rp.nindividuals << endl;
  cout << setw(15) << "born = " << rp.born << endl;
  cout << setw(15) << "mkilled = " << rp.mkilled << endl;
  cout << setw(15) << "vkilled = " << rp.vkilled << endl;
  cout << flush;

  return scm_from_bool(1);
}

extern "C" SCM get_population() {
  check_environment();
  vector<unsigned int> pop = env->PopStructure();

  SCM plist = scm_list(SCM_EOL);

  for(unsigned int i=0; i<pop.size(); i++) {
    plist = scm_append(scm_list_2(plist, scm_list_1(scm_uint2num(pop[i]))));
  }
  
  return plist;
}

extern "C" SCM get_mortality() {
  check_environment();
  vector<double> mort = env->MortStructure();
  SCM plist = scm_list(SCM_EOL);

  for(unsigned int i=0; i<mort.size(); i++) {
    plist = scm_append(scm_list_2(plist,scm_list_1(scm_double2num(mort[i]))));
  }
  return plist;
}

extern "C" SCM do_step(SCM param) {
  check_environment();
  unsigned int nsteps = 1;
  
  if (scm_is_true(scm_integer_p(param))){
    nsteps = scm_to_uint(param);
  }

  for(unsigned int i=1; i<=nsteps; i++) {
    env->Step();
    if(env->Size() <= 0) {
      cerr << "Population extinct!" << endl;
      break;
    }
    
  }
  
  return scm_from_bool(1);
}

extern "C" SCM make_environment(SCM param) {

  delete env;
  env = new Environment();
  unsigned int size = scm_to_uint(param);

  if(size >= p.N) {
    ostringstream info;
    info << "Environment fill can not be greater or equal capacity N=" << p.N << ends;
    throw_exception("incorrect-fill-size", info.str());
  }

  env->Fill(size);

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
  scm_c_define_gsubr("read-config",0,0,0,read_config);
  scm_c_define_gsubr("create-environment",1,0,0,(SCM (*)() ) make_environment);
  scm_c_define_gsubr("warranty",0,0,0, show_warranty);
  scm_c_define_gsubr("get-state",0,0,0, get_state);
  scm_c_define_gsubr("print-state",0,0,0,print_state);
  scm_c_define_gsubr("get-population",0,0,0,get_population);
  scm_c_define_gsubr("get-mortality",0,0,0,get_mortality);
  scm_c_define_gsubr("do-step",0,1,0,(SCM (*)())do_step);
  scm_c_primitive_load ("init.scm");
  scm_shell (argc, argv);
}

void init_guile(int argc, char **argv) {
  scm_boot_guile (argc, argv, vpms_main, 0);
}
