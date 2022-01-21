LIBA=-lscip
CXXFLAGS=-g -std=c++2a -I/usr/include/python3.8

SRCD := src
OBJD := build
LIBSRC  := $(wildcard $(SRCD)/lib/*.cpp)
LIBOBJ  := $(patsubst $(SRCD)/lib/%.cpp,$(OBJD)/lib/%.o,$(LIBSRC))
MAIN := src/main.cpp

build/main: build/hf.so $(MAIN) $(OBJD)
	g++ $(CXXFLAGS) -o $@ $^ $(LIBA) 

build/hf.so: $(LIBOBJ) 
	g++ -shared -Wl,-soname,libhf.so -o $@ $^ -lc

$(LIBOBJ): $(LIBSRC)
	$(CXX) $(CXXFLAGS) -fPIC -c -o $@ $<

$(OBJD):
	mkdir -p build/lib
	mkdir -p build/bin

clean:
	rm -rf build/
