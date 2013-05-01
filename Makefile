ROOT    = src
CC      = cc
CPP      = g++
CFLAGS  = -g3

#ORGANISM FILES
ORGANISM_CPP  = $(ROOT)/organism/organism.cpp
ORGANISM_CPP += $(ROOT)/helpers/messenger/messenger.cpp
ORGANISM_CPP += $(ROOT)/organism/main.cpp

ORGANISM_H  = $(ROOT)/organism/organism.h
ORGANISM_H += $(ROOT)/helpers/messenger/messenger.h


#WORLD FILES
WORLD_CPP  = $(ROOT)/world/analyzer.cpp
WORLD_CPP += $(ROOT)/world/caregiver.cpp
WORLD_CPP += $(ROOT)/world/judge.cpp
WORLD_CPP += $(ROOT)/world/mutator.cpp
WORLD_CPP += $(ROOT)/world/primal.cpp
WORLD_CPP += $(ROOT)/world/transformer/transformer.cpp
WORLD_CPP += $(ROOT)/world/watchdog.cpp
WORLD_CPP += $(ROOT)/world/main.cpp
WORLD_CPP += $(ROOT)/helpers/messenger/messenger.cpp

WORLD_H  = $(ROOT)/world/analyzer.h
WORLD_H += $(ROOT)/world/caregiver.h
WORLD_H += $(ROOT)/world/judge.h
WORLD_H += $(ROOT)/world/mutator.h
WORLD_H += $(ROOT)/world/primal.h
WORLD_H += $(ROOT)/world/transformer/transformer.h
WORLD_H += $(ROOT)/world/watchdog.h
WORLD_H += $(ROOT)/helpers/messenger/messenger.h

#LEX
LEX      = $(ROOT)/world/transformer/lex/main.l

#FOLDERS WITH HEADERS
INCLUDE  = -I$(ROOT)/organism/
INCLUDE  += -I$(ROOT)/helpers/messenger/
INCLUDE  += -I$(ROOT)/world/transformer/
INCLUDE  += -I$(ROOT)/world/

all: mkdir organism world

mkdir:
	test -d ./build || mkdir ./build
	test -d ./results || mkdir ./results

organism: mkdir
	$(CPP) $(CFLAGS) $(ORGANISM_CPP) -o ./build/organism.bin $(ORGANISM_H) $(INCLUDE) $(LIBS)
	./build/organism.bin
	
world: mkdir
	$(CPP) $(CFLAGS) $(WORLD_CPP) -o ./build/world.bin $(WORLD_H) $(INCLUDE) $(LIBS)
	./build/worldjhs.bin
	
lex: mkdir
	flex $(LEX)
	yacc -d ./src/world/transformer/yacc/main.y
	$(CC) lex.yy.c y.tab.c -o ./build/lexer.bin $(INCLUDE)
	rm lex.yy.c

clean:
	rm -rf *.o  ./build
	rm -rf *.o  ./results
	
clean_lex:
	rm ./build/lexer.bin

clean_all: clean
	rm -rf *.out *~ 