LIBA=-lscip
CCF=-g -std=c++2a

build/main: src/main.cpp src/modules.hpp build
	g++ $(CCF) $< -o $@ $(LIBA)

build:
	mkdir -p build

clean:
	rm -rf build/
