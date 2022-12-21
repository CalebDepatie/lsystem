CC = gcc
CXX = g++

LINK_FLAGS = -lglfw -lGLEW -lGL -lGLU
CXXFLAGS = -Wall -std=c++23 -O3 $(LINK_FLAGS)
CCFLAGS = -Wall -std=c17 -O3

all: lsystem

lsystem: main.cpp shaders.o callbacks.o grammer.o graphics.o
	$(CXX) $(CXXFLAGS) -o lsystem main.cpp shaders.o callbacks.o grammer.o graphics.o

grammer.o: grammer.cpp grammer.hpp
	$(CXX) $(CXXFLAGS) -c -o grammer.o grammer.cpp

callbacks.o: callbacks.cpp callbacks.hpp
	$(CXX) $(CXXFLAGS) -c -o callbacks.o callbacks.cpp

shaders.o: shaders.cpp shaders.h
	$(CXX) $(CXXFLAGS) -c -o shaders.o shaders.cpp

graphics.o: graphics.cpp graphics.hpp
	$(CXX) $(CXXFLAGS) -c -o graphics.o graphics.cpp

clean:
	rm *.o
	rm asmt4
