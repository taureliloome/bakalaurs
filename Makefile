ROOT    = src
CC      = gcc
CPP     = g++
CFLAGS  = -g3
VERSION=$(shell git rev-parse HEAD)

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

#YACC
YACC	 = $(ROOT)/world/transformer/yacc/main.y

#FOLDERS WITH HEADERS
INCLUDE   = -I$(ROOT)/organism/
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
	./build/world.bin
	
precompile:
	$(CC) -E ./tasks/hello.c > ./tasks/hello.pre
	
lex: mkdir
	flex $(LEX)
	$(CC) lex.yy.c -o ./build/lexer.bin -DLEX -ll
	rm lex.yy.c


lex_yacc: mkdir
	
	echo "=================================================================="
	echo "= BUILD                 $(VERSION) ="
	echo "= TARGET                                                lex_yacc ="
	echo "=================================================================="
	flex $(LEX)
	yacc -d $(YACC)
	#$(CC) --debug --verbose lex.yy.c y.tab.c -o ./build/lexer.bin $(INCLUDE)
	$(CC) lex.yy.c y.tab.c -o ./build/lexer.bin $(INCLUDE) -DYACC -ll
	rm lex.yy.c y.tab.c y.tab.h

test:
	test -f ./build/lexer.bin && cat ./tasks/hello.c | ./build/lexer.bin	

clean:
	rm -rf *.o  ./build
	rm -rf *.o  ./results
	
clean_lex:
	rm ./build/lexer.bin lex.yy.c y.tab.c y.tab.h

clean_all: clean clean_lex
	rm -rf *.out *~ 