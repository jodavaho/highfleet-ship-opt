TARGET:=$(shell gcc -dumpmachine)
ARCH:=x86_64 #TODO: fix to scrape from TARGET

CXX := $(TARGET)-g++
CC  := $(TARGET)-gcc

LIBA:=-L.deps/$(TARGET)/lib -Lbuild/$(TARGET)  -lz -lgmp
CXXFLAGS:= -Isrc -std=c++2a -I.deps/$(TARGET)/include -DNO_CONFIG_HEADER

SRCD := src
OBJD := build/$(TARGET)

# Lib outputs
LIBSRC  := $(wildcard $(SRCD)/lib/*.cpp)
LIBOBJ  := $(patsubst $(SRCD)/lib/%.cpp,$(OBJD)/%.o,$(LIBSRC))
SCIPOBJ := .deps/$(TARGET)/lib/libscip.a .deps/$(TARGET)/lib/libsoplex.a
LIB := build/$(TARGET)/libhf.so

# Main outputs
MAIN := src/main.cpp src/opts.cpp
EXE := build/$(TARGET)/hftop

PYOBJD := build/lib.linux-$(ARCH)-3.8
PYLIB := $(PYOBJD)/hfopt.cpython-38-$(TARGET).so

all: $(OBJD) $(EXE) $(LIB)  $(PYLIB)

$(EXE): $(MAIN) $(LIB) $(SCIPOBJ)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LIBA) -lhf

$(LIB): $(LIBOBJ)
	$(CXX) -shared -Wl,-soname,$@ -o $@ $(LIBOBJ) -lc 

$(OBJD)/%.o: $(SRCD)/lib/%.cpp 
	$(CXX) -fPIC $(CXXFLAGS)  -c $< -o $@

$(OBJD):
	mkdir -p build/$(TARGET)

$(SCIPOBJ):
	@echo "You need to build $@ ... see README.md"

$(PYLIB): $(LIB)
	$(shell python3 build_py.py build)
	cp src/py/*.py bu

clean:
	rm -rf build/$(TARGET)

clean-all:
	rm -rf build/
