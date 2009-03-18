CXX=g++
#CXXFLAGS=-g -Wall -std=c++0x -DDEBUG_LEVEL=2
#CXXFLAGS=-g -Wall -std=c++0x -DDEBUG_LEVEL=1
CXXFLAGS=-Wall -std=c++0x -O2 -march=i686 -fomit-frame-pointer
LFLAGS=`guile-config link`

SRC      := model.cc bitstring.cc vpms.cc guile.cc config.cc stats.cc fileio.cc
HEADERS  := $(SRC:%.cc=%.hh)
OBJECTS  := $(SRC:%.cc=%.o)
TARGET   := vpms



$(TARGET): $(OBJECTS)
	$(CXX) $(LFLAGS) -o $@ $^

.SUFFIXES: .o .cc

fileio.o: fileio.cc $(HEADERS)
	@echo "string compilation_flags=\"" $(CXXFLAGS) "\";" > compilation.hh
	$(CXX) $(CXXFLAGS) -c $<

.cc.o: $(HEADERS) flags
	$(CXX) $(CXXFLAGS) -c $<

.PHONY: clean

clean:
	rm -f *.o *~ $(TARGET)
