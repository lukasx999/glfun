CXX=clang++
CXXFLAGS=-Wall -Wextra -ggdb -std=c++23 -pedantic
LIBS=-lglfw -lGL

all: cube

cube: main.o shader.o
	$(CXX) $(CXXFLAGS) $(LIBS) $^ -o $@

%.o: %.cc Makefile shader.hh
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm *.o cube

.PHONY: clean
