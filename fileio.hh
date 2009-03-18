#include <string>
#include <fstream>

using namespace std;

#ifndef FILEIO_H
#define FILEIO_H

class Logging {
  
  string logName;
  ofstream logFile;
  
public:
  Logging();
  ~Logging();

  template <class MsgType>
  Logging & operator<<(MsgType);

};

template <class MsgType>
Logging & Logging::operator<<(MsgType message) {

  time_t tt;
  time(&tt); 
  string strtime(ctime(&tt));
  strtime.erase(strtime.find("\n"),1);

  if(logFile) {
    logFile << "[" << strtime << "]: ";
    logFile << message << endl << flush;
  }
  cout << message << endl << flush;

  return *this;
}


#endif
