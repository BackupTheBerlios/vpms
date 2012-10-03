CXX=g++
#CXXFLAGS=-g -Wall -std=c++0x -pg
#CXXFLAGS=-g -Wall -std=c++0x -DDEBUG_LEVEL=2
#CXXFLAGS=-g -Wall -std=c++0x -DDEBUG_LEVEL=1
<<<<<<< local
CXXFLAGS=-Wall -std=c++0x -O2 -march=native -fomit-frame-pointer
=======
CXXFLAGS=-Wall -std=c++0x -O2 -march=native -mtune=native -fomit-frame-pointer
>>>>>>> other
LFLAGS=`guile-config link`

SRC      := model.cc vpms.cc guile.cc config.cc stats.cc fileio.cc
HEADERS  := $(SRC:%.cc=%.hh)
OBJECTS  := $(SRC:%.cc=%.o)
TARGET   := vpms



$(TARGET): $(OBJECTS)
	$(CXX) -o $@ $^ $(LFLAGS) 

.SUFFIXES: .o .cc

fileio.o: fileio.cc $(HEADERS)
	@echo "const char* compilation_flags=\"" $(CXXFLAGS) "\";" > compilation.hh
	$(CXX) $(CXXFLAGS) -c $<

.cc.o: $(HEADERS) flags
	$(CXX) $(CXXFLAGS) -c $<

.PHONY: clean

clean:
	rm -f *.o *.gch *~ $(TARGET)
