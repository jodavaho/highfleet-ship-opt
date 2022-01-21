LIBA=-lscip
CXXFLAGS=-g -std=c++2a -I/usr/include/python3.8

SRCD := src
OBJD := build
SRC  := $(wildcard $(SRCD)/*.cpp)
OBJ  := $(patsubst $(SRCD)/%.cpp,$(OBJD)/%.o,$(SRC))

build/main: $(OBJ)
	g++ $(CXXFLAGS) -o $@ $^ $(LIBA)

$(OBJD)/%.o: $(SRCD)/%.cpp $(OBJD)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

build:
	mkdir -p build

clean:
	rm -rf build/
