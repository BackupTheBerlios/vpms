#include <string>

using namespace std;

class Logging {
  
  string logName;
  ofstream logFile;
  
public:
  Logging();
  ~Logging();

  template <class MsgType>
  Logging & operator<<(MsgType);

};
