ROOT    = /home/janisknets/Projects/bakdarbs/src
CC      = g++
CFLAGS  = -g3

FILES    = $(ROOT)/organism/organism.cpp
FILES   += $(ROOT)/helpers/messenger.cpp
FILES   += $(ROOT)/organism/main.cpp

HEADERS  = $(ROOT)/organism/organism.h
HEADERS += $(ROOT)/helpers/messenger.h

INCLUDE  = -I$(ROOT)/organism/
INCLUDE  = -I$(ROOT)/helpers/

all: mkdir organism

mkdir:
	test -d ./build || mkdir ./build
	test -d ./results || mkdir ./results

organism: mkdir
	$(CC) $(CFLAGS) $(FILES) -o ./build/organism.bin $(HEADERS) $(INCLUDE) $(LIBS)
	./build/organism.bin

clean:
	rm -rf *.o  ./build
	rm -rf *.o  ./results

clean_all: clean
	rm -rf *.out *~ 