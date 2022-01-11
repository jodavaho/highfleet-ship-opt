LIBA=-lscip
CCF=-g

build/main: src/main.cpp build
	g++ $(CCF) $< -o $@ $(LIBA)

build:
	mkdir -p build

clean:
	rm -rf build/
