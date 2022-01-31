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
CXXFLAGS:= -I$(SRCD) -std=c++2a -I$(INCS) -DNO_CONFIG_HEADER -Wall -Werror -Wpedantic

# libhf.so outputs
LIBSRC  := $(wildcard $(SRCD)/lib/*.cpp)
LIBOBJ  := $(patsubst $(SRCD)/lib/%.cpp,$(OBJD)/%.o,$(LIBSRC))
SCIPOBJ := .deps/$(TARGET)/lib/libscip.a .deps/$(TARGET)/lib/libsoplex.a
LIB := build/$(TARGET)/libhf.so
SLIB := build/$(TARGET)/libhf.a

# main (hfopt) outputs
MAIN := src/main.cpp src/opts.cpp
EXE := build/$(TARGET)/hfopt

# Some python outputs
PYSRCD := src/lib/py
PYOBJD := build/lib.linux-$(ARCH)-3.8
PYSRC := $(wildcard $(PYSRCD)/*.cpp)
PYLIB := $(PYOBJD)/hfopt.cpython-38-$(TARGET).so

all: $(OBJD) $(EXE) $(LIB) $(PYLIB)

# Main executable with static linkeage
$(EXE): $(MAIN) $(SLIB) $(SCIPOBJ)
	$(CXX) $(CXXFLAGS) $^ $(SLIB) -o $@ $(LIBA) 

# Main library, mostly for python?
$(LIB): $(SCIPOBJ) $(LIBOBJ) 
	$(CXX) -shared -Wl,-soname,$@ -o $@ $^ $(LIBA)

# Static version 
$(SLIB): $(LIBOBJ)
	ar -crs $(@) $(SCIPOBJ) $(LIBOBJ)

# For any object file x, build it from src/lib/x.cpp
$(OBJD)/%.o: $(SRCD)/lib/%.cpp 
	$(CXX) -fPIC $(CXXFLAGS)  -c $< -o $@

# To build the output directory, just mkdir
$(OBJD):
	mkdir -p build/$(TARGET)

# If we're missing an input library (external), just nag user
$(SCIPOBJ):
	@echo "You need to build $@ ... see README.md"

# To build python library, link in libhf.so and build the thing from src/py/*
# Not sure if libhf.a or libhf.so will work ... testing
$(PYLIB): $(SLIB) src/py/hfopt.py $(PYSRC)
	cp $(LIB) $(DEPS)/
	cp $(SLIB) $(DEPS)/
	python3 build_py.py build  -j4 
	cp src/py/*.py $(PYOBJD)
	cp $(LIB) $(PYOBJD)
	cp $(DEPS)/libscip.so $(PYOBJD)

clean:
	rm -rf build/$(TARGET)/*
	rm -rf $(PYOBJD)/*

clean-all:
	rm -rf build/
