LIBA=-lscip
CXXFLAGS=-g -std=c++2a

SRCD := src
OBJD := build
SRC  := $(wildcard $(SRCD)/*.cpp)
OBJ  := $(patsubst $(SRCD)/%.cpp,$(OBJD)/%.o,$(SRC))

$(OBJD)/%.o: $(SRCD)/%.cpp $(OBJD)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

build/main: $(OBJ)
	g++ $(CXXFLAGS) $^ -o $@ $(LIBA)

build:
	mkdir -p build

clean:
	rm -rf build/
