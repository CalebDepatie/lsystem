CC = gcc
CXX = g++

LINK_FLAGS = -lglfw -lGLEW -lGL -lGLU
CXXFLAGS = -Wall -std=c++23 -O3 $(LINK_FLAGS)
CCFLAGS = -Wall -std=c17 -O3

all: asmt4

asmt4: main.cpp shaders.o callbacks.o grammer.o
	$(CXX) $(CXXFLAGS) -o asmt4 main.cpp shaders.o callbacks.o grammer.o

grammer.o: grammer.cpp grammer.hpp
	$(CXX) $(CXXFLAGS) -c -o grammer.o grammer.cpp

callbacks.o: callbacks.cpp callbacks.hpp
	$(CXX) $(CXXFLAGS) -c -o callbacks.o callbacks.cpp

shaders.o: shaders.cpp shaders.h
	$(CXX) $(CXXFLAGS) -c -o shaders.o shaders.cpp

clean:
	rm *.o
	rm asmt4
