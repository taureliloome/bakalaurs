ROOT    = /home/janisknets/Projects/bakdarbs/src
CC      = g++
CFLAGS  = -g3

FILES    = $(ROOT)/organism/organism.cpp
FILES   += $(ROOT)/organism/main.cpp

INCLUDE = $(ROOT)/organism/organism.h

all: mkdir organism

mkdir:
	test -d ./build || mkdir ./build
	test -d ./results || mkdir ./results

organism: mkdir
	$(CC) $(CFLAGS) $(FILES) -o ./build/organism.bin $(INCLUDE) $(LIBS)

clean:
	rm -rf *.o  ./build
	rm -rf *.o  ./results

clean_all: clean
	rm -rf *.out *~ 