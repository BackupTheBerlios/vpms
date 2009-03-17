#include <string>
#include <fstream>
#include <iostream>
#include "fileio.hh"

using namespace std;

Logging::Logging() : logName("session.log"), logFile(logName.c_str()) {

  if(!logFile) {
    cerr << "error: can not write to file: " << logName << endl;
  }
}

Logging::~Logging() {
  logFile.close();
}

template <class MsgType>
Logging & Logging::operator<<(MsgType message) {
  logFile << message << flush;
  return *this;
}
