TARGET:=$(shell gcc -dumpmachine)
LIBA:=-L.deps/$(TARGET)/lib -Lbuild/$(TARGET) -Lbuild/$(TARGET) -lscip
CXXFLAGS:= -std=c++2a -I.dep/$(TARGET)/include
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
MAIN := src/main.cpp

all: $(OBJD) $(PYOBJD) build/$(TARGET)/hftop build/$(TARGET)/libhf.so pybuild/$(TARGET)/libhf-py.so

build/$(TARGET)/hftop: src/main.cpp build/$(TARGET)/libhf.so
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LIBA) -Lbuild -lhf

build/$(TARGET)/libhf.so: $(LIBOBJ)
	$(CXX) -shared -Wl,-soname,$@ -o $@ $(LIBOBJ)  -lc -lscip

pybuild/$(TARGET)/libhf-py.so: $(LIBOBJ) $(PYOBJ)
	$(CXX) -shared -Wl,-soname,$@ -o $@ $(LIBOBJ)  -lc -lscip

$(PYOBJD)/%.o: $(SRCD)/py/%.cpp 
	$(CXX) -fPIC $(CXXFLAGS) $(PYFLAGS)  -c $< -o $@

$(OBJD)/%.o: $(SRCD)/lib/%.cpp 
	$(CXX) -fPIC $(CXXFLAGS)  -c $< -o $@

$(OBJD):
	mkdir -p build/$(TARGET)

$(PYOBJD):
	mkdir -p pybuild/$(TARGET)

clean:
	rm -rf build/$(TARGET)
	rm -rf pybuild/$(TARGET)

clean-all:
	rm -rf build/
