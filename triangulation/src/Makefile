CC=g++
WFLAGS=-Wall -Wextra -pedantic -Werror
OFLAFS=-O3

all : trToTGA TGAtoTGA

clean :
	rm -f *~ trToTGA *.o

TGAtoTGA : Picture.o Common.o
	$(CC) $(OFLAGS) $(WFLAGS) Picture.o Common.o TGAtoTGA.cpp -o TGAtoTGA

trToTGA : triangleToPic.cpp Triangle.hpp Common.o Picture.o
	$(CC) $(OFLAGS) $(WFLAGS) Picture.o Common.o triangleToPic.cpp -o trToTGA

Picture.o : Picture.hpp Picture.cpp Triangle.hpp Common.hpp
	$(CC) $(OFLAGS) $(WFLAGS) Picture.cpp -c -o Picture.o

Common.o : Common.hpp Common.cpp
	$(CC) $(OFLAGS) $(WFLAGS) Common.cpp -c -o Common.o