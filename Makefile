LIBA=-lscip
all: build/
	g++ src/main.cpp -o build/main $(LIBA)

build/:
	mkdir -p build

clean:
	rm -rf build/