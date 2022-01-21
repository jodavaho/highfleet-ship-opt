LIBA:=-lscip
CXXFLAGS:= -std=c++2a 
PYFLAGS:= -I/usr/include/python3.8

SRCD := src
OBJD := build
PYOBJD := pybuild
PYSRC   := $(wildcard $(SRCD)/py/*.cpp)
PYOBJ   := $(patsubst $(SRCD)/py/%.cpp,$(PYOBJD)/%.o,$(PYSRC))
LIBSRC  := $(wildcard $(SRCD)/lib/*.cpp)
LIBOBJ  := $(patsubst $(SRCD)/lib/%.cpp,$(OBJD)/%.o,$(LIBSRC))
MAIN := src/main.cpp

all: $(OBJD) $(PYOBJD) build/hftop build/libhf.so pybuild/libhf-py.so

build/hftop: src/main.cpp build/libhf.so
	g++ $(CXXFLAGS) $^ -o $@ $(LIBA) -Lbuild -lhf

build/libhf.so: $(LIBOBJ)
	g++ -shared -Wl,-soname,$@ -o $@ $(LIBOBJ)  -lc -lscip

pybuild/libhf-py.so: $(LIBOBJ) $(PYOBJ)
	g++ -shared -Wl,-soname,$@ -o $@ $(LIBOBJ)  -lc -lscip

$(PYOBJD)/%.o: $(SRCD)/py/%.cpp 
	g++ -fPIC $(CXXFLAGS) $(PYFLAGS)  -c $< -o $@

$(OBJD)/%.o: $(SRCD)/lib/%.cpp 
	g++ -fPIC $(CXXFLAGS)  -c $< -o $@

$(OBJD):
	mkdir -p build

$(PYOBJD):
	mkdir -p pybuild

clean:
	rm -rf build/
	rm -rf pybuild/
