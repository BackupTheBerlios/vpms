#include <string>
#include <fstream>

using namespace std;

#ifndef FILEIO_H
#define FILEIO_H

class Logging {
  
  string logName;
  ofstream logFile;
  bool displayOnScreen;
  
public:
  Logging();
  ~Logging();
  void SetScreenDisplay(bool);

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
    logFile << message << endl;
  }
  
  if(displayOnScreen) {
    cout << message << endl << flush;
  }

  return *this;
}


#endif
