TARGET:=$(shell gcc -dumpmachine)
LIBA:=-L.deps/$(TARGET)/lib -Lbuild/$(TARGET)  -lz -lgmp
CXXFLAGS:= -std=c++2a -I.deps/$(TARGET)/include -DNO_CONFIG_HEADER
PYFLAGS:= -I/usr/include/python3.8

CXX := $(TARGET)-g++
CC  := $(TARGET)-gcc

SRCD := src
OBJD := build/$(TARGET)
PYOBJD := pybuild/$(TARGET)
PYSRC   := $(wildcard $(SRCD)/py/*.cpp)
PYOBJ   := $(patsubst $(SRCD)/py/%.cpp,$(PYOBJD)/%.o,$(PYSRC))
LIBSRC  := $(wildcard $(SRCD)/lib/*.cpp)
LIBOBJ  := $(patsubst $(SRCD)/lib/%.cpp,$(OBJD)/%.o,$(LIBSRC))
SCIPOBJ := .deps/$(TARGET)/lib/libscip.a .deps/$(TARGET)/lib/libsoplex.a
MAIN := src/main.cpp

all: $(OBJD) $(PYOBJD) build/$(TARGET)/hftop build/$(TARGET)/libhf.so pybuild/$(TARGET)/libhf-py.so

build/$(TARGET)/hftop: src/main.cpp build/$(TARGET)/libhf.so $(SCIPOBJ)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LIBA) -lhf

build/$(TARGET)/libhf.so: $(LIBOBJ)
	$(CXX) -shared -Wl,-soname,$@ -o $@ $(LIBOBJ) -lc 

pybuild/$(TARGET)/libhf-py.so: $(LIBOBJ) $(PYOBJ)
	$(CXX) -shared -Wl,-soname,$@ -o $@ $(LIBOBJ)  -lc 

$(PYOBJD)/%.o: $(SRCD)/py/%.cpp 
	$(CXX) -fPIC $(CXXFLAGS) $(PYFLAGS)  -c $< -o $@

$(OBJD)/%.o: $(SRCD)/lib/%.cpp 
	$(CXX) -fPIC $(CXXFLAGS)  -c $< -o $@

$(OBJD):
	mkdir -p build/$(TARGET)

$(PYOBJD):
	mkdir -p pybuild/$(TARGET)

$(SCIPOBJ):
	@echo "You need to build $@ ... see README.md"

clean:
	rm -rf build/$(TARGET)
	rm -rf pybuild/$(TARGET)

clean-all:
	rm -rf build/
