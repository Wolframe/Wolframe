CXX ?= g++

CXXFLAGS = -g -O0 $(shell python3-config --cflags)
LDFLAGS = $(shell python3-config --ldflags)
LIBS = $(shell python3-config --libs)

all: embed

embed.o: embed.cpp
	$(CXX) -c -o embed.o $(CXXFLAGS) embed.cpp

embed: embed.o
	$(CXX) -o embed $(LDFLAGS) embed.o $(LIBS)

clean:
	-rm *.o
	-rm embed

test: all
	./embed my
