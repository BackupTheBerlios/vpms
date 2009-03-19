#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
#include <ctime>
#include "fileio.hh"
#include "compilation.hh"
#include "model.hh"

using namespace std;

namespace vpms {
  extern Environment *env;
}

void Logging::SetScreenDisplay(bool dscreen) {
  displayOnScreen=dscreen;
}

Logging::Logging() : logName("session.log"), logFile(logName.c_str()), displayOnScreen(true) {

    if(!logFile) {
      cerr <<"error: can not write to file: " << logName << endl;
    }

    logFile << "vpms version: " << __DATE__ << " " << __TIME__ << endl;
    logFile << "compilation flags: " << compilation_flags << endl << endl << flush;

}

Logging::~Logging() {
  ostringstream buf;
  buf << "session finished" << endl;
  (*this) << buf.str();
  logFile.close();
}

