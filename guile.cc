#include <libguile.h>
#include <iostream>
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

SCM read_config() {

  SCM lsymbol = scm_c_lookup("config");
  SCM list = scm_variable_ref(lsymbol);
  int lsize = scm_to_uint(scm_length(list)); 
  SCM retVal = scm_from_bool(1);

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
    
    putConfig(N,         string("N"),string(varName),varValue);
    putConfig(B,         string("B"),string(varName),varValue);
    putConfig(R,         string("R"),string(varName),varValue);
    putConfig(T,         string("T"),string(varName),varValue);
    putConfig(M,         string("M"),string(varName),varValue);
    putConfig(P,         string("P"),string(varName),varValue);
    putConfig(initGenome,string("initGenome"),string(varName),varValue);
    putConfig(rndSeed,   string("rndSeed"),string(varName),varValue);
   }
   cout << cfg << endl;
   initialize();
   return retVal;
}

SCM make_environment(SCM param) {

  delete env;
  env = new Environment();
  unsigned int size = scm_to_uint(param);
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

SCM show_warranty() {
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
  scm_c_primitive_load ("init.scm");
  scm_shell (argc, argv);
}

void init_guile(int argc, char **argv) {
  scm_boot_guile (argc, argv, vpms_main, 0);
}
