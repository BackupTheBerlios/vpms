CXX=g++
CXXFLAGS=-g -Wall -std=c++0x -DDEBUG_LEVEL=2
LFLAGS=`guile-config link`

SRC      := model.cc bitstring.cc vpms.cc guile.cc config.cc
HEADERS  := $(SRC:%.cc=%.hh)
OBJECTS  := $(SRC:%.cc=%.o)
TARGET   := vpms

$(TARGET): $(OBJECTS)
	$(CXX) $(LFLAGS) -o $@ $^

.SUFFIXES: .o .cc

.cc.o: $(HEADERS)
	$(CXX) $(CXXFLAGS) -c $<

.PHONY: clean

clean:
	rm -f *.o *~ $(TARGET)
