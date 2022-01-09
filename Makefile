all: build/
	gcc src/main.c -o build/main

build/:
	mkdir -p build

clean:
	rm -rf build/
