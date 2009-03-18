#include <string>
#include <fstream>
#include <iostream>
#include <ctime>
#include "fileio.hh"
#include "compilation.hh"

using namespace std;

Logging::Logging() : logName("session.log"), logFile(logName.c_str()) {

    if(!logFile) {
      cerr <<"error: can not write to file: " << logName << endl;
    }

    logFile << "vpms version: " << __DATE__ << " " << __TIME__ << endl;
    logFile << "compilation flags: " << compilation_flags << endl << endl << flush;

}

Logging::~Logging() {
  (*this) << "session finished";
  logFile.close();
}

