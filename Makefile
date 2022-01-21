LIBA:=-lscip
CXXFLAGS:= -std=c++2a 
PYFLAGS:= -I/usr/include/python3.8

SRCD := src
OBJD := build
LIBSRC  := $(wildcard $(SRCD)/lib/*.cpp)
LIBOBJ  := $(patsubst $(SRCD)/lib/%.cpp,$(OBJD)/%.o,$(LIBSRC))
MAIN := src/main.cpp

build/hftop: src/main.cpp build/libhf.so
	g++ $(CXXFLAGS) $^ -o $@ $(LIBA) -Lbuild -lhf

build/libhf.so: $(LIBOBJ)
	g++ -shared -Wl,-soname,$@ -o $@ $(LIBOBJ)  -lc -lscip

$(OBJD)/%.o: $(SRCD)/lib/%.cpp 
	g++ -fPIC $(CXXFLAGS)  -c $< -o $@

clean:
	rm -rf build/
