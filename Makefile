TARGET:=$(shell gcc -dumpmachine)
#TODO: fix to scrape from TARGET
ARCH:=x86_64

CXX := $(TARGET)-g++
CC  := $(TARGET)-gcc

# Input and output dirs
SRCD := src
OBJD := build/$(TARGET)

# External dependencies, pre-built
INCS:=.deps/$(TARGET)/include
DEPS:=.deps/$(TARGET)/lib

# Linking commands
LIBA:=-L$(DEPS) -Lbuild/$(TARGET)  -lz -lgmp

# Build flags
CXXFLAGS:= -I$(SRCD) -std=c++2a -I$(INCS) -DNO_CONFIG_HEADER

# libhf.so outputs
LIBSRC  := $(wildcard $(SRCD)/lib/*.cpp)
LIBOBJ  := $(patsubst $(SRCD)/lib/%.cpp,$(OBJD)/%.o,$(LIBSRC))
SCIPOBJ := .deps/$(TARGET)/lib/libscip.a .deps/$(TARGET)/lib/libsoplex.a
LIB := build/$(TARGET)/libhf.so
SLIB := build/$(TARGET)/libhf.a

# main (hfopt) outputs
MAIN := src/main.cpp src/opts.cpp
EXE := build/$(TARGET)/hftop

# Some python outputs
PYOBJD := build/lib.linux-$(ARCH)-3.8
PYLIB := $(PYOBJD)/hfopt.cpython-38-$(TARGET).so

all: $(OBJD) $(EXE) $(LIB) $(PYLIB)

$(EXE): $(MAIN) $(LIB) $(SCIPOBJ)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LIBA) -lhf

$(LIB): $(LIBOBJ)
	$(CXX) -shared -Wl,-soname,$@ -o $@ $(SCIPOBJ) $(LIBOBJ) 

$(SLIB): $(LIBOBJ)
	ar -crs $(@) $(SCIPOBJ) $(LIBOBJ)

# For any object file x, build it from src/lib/x.cpp
$(OBJD)/%.o: $(SRCD)/lib/%.cpp 
	$(CXX) -fPIC $(CXXFLAGS)  -c $< -o $@

# TO build the output directory, just mkdir
$(OBJD):
	mkdir -p build/$(TARGET)

# If we're missing an input library (external), just nag user
$(SCIPOBJ):
	@echo "You need to build $@ ... see README.md"

# To build python library, link in libhf.so and build the thing from src/py/*
$(PYLIB): $(SLIB)
	cp $(SLIB) $(DEPS)/
	python3 build_py.py build  -j4 
	cp src/py/*.py $(PYOBJD)
	cp $(LIB) $(PYOBJD)

clean:
	rm -rf build/$(TARGET)
	rm -rf $(PYOBJD)

clean-all:
	rm -rf build/
