ROOT    = src
CC      = gcc
CPP     = g++
CFLAGS  = -g3
VERSION=$(shell git rev-parse HEAD)

#LIBS
LIBS  = -lpthread
LIBS += -lavl


#ORGANISM FILES
ORGANISM_CPP  = $(ROOT)/organism/organism.cpp
ORGANISM_CPP += $(ROOT)/helpers/messenger/messenger.cpp
ORGANISM_CPP += $(ROOT)/helpers/communication/communication.cpp
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
WORLD_CPP += $(ROOT)/helpers/communication/communication.cpp

WORLD_O  = analyzer.o
WORLD_O += caregiver.o
WORLD_O += judge.o
WORLD_O += mutator.o
WORLD_O += primal.o
WORLD_O += transformer.o
WORLD_O += watchdog.o
WORLD_O += main.o
WORLD_O += messenger.o
WORLD_O += communication.o

WORLD_H  = $(ROOT)/world/analyzer.h
WORLD_H += $(ROOT)/world/caregiver.h
WORLD_H += $(ROOT)/world/judge.h
WORLD_H += $(ROOT)/world/mutator.h
WORLD_H += $(ROOT)/world/primal.h
WORLD_H += $(ROOT)/world/transformer/transformer.h
WORLD_H += $(ROOT)/world/watchdog.h
WORLD_H += $(ROOT)/helpers/messenger/messenger.h

#YACC FILES
YACC_C  = lex.yy.c 
YACC_C += y.tab.c
YACC_C += $(ROOT)/helpers/messenger/messenger.cpp
YACC_C += $(ROOT)/helpers/communication/communication.cpp
YACC_C += $(ROOT)/world/transformer/transformer.cpp
YACC_C += $(ROOT)/world/transformer/yacc/transformer_if.cpp
YACC_C += $(ROOT)/world/primal.cpp

#LEX
LEX      = $(ROOT)/world/transformer/lex/main.l

#YACC
YACC	 = $(ROOT)/world/transformer/yacc/main.y

#INCLUDES
INCLUDE   = -I$(ROOT)/organism/
INCLUDE  += -I$(ROOT)/helpers/messenger/
INCLUDE  += -I$(ROOT)/helpers/communication/
INCLUDE  += -I$(ROOT)/world/transformer/yacc/
INCLUDE  += -I$(ROOT)/world/transformer/
INCLUDE  += -I$(ROOT)/world/includes/
INCLUDE  += -I$(ROOT)/world/

all: mkdir organism world

mkdir:
	test -d ./build || mkdir ./build
	test -d ./results || mkdir ./results

organism: mkdir
	$(CPP) $(CFLAGS) $(ORGANISM_CPP) -o ./build/organism.bin $(ORGANISM_H) $(INCLUDE) $(LIBS)
	./build/organism.bin
	
#Objects for world
analyzer.o :  $(ROOT)/world/analyzer.cpp
	$(CPP) -g -c $(ROOT)/world/analyzer.cpp $(ROOT)/world/analyzer.h $(INCLUDE)
	
caregiver.o :  $(ROOT)/world/caregiver.cpp
	$(CPP) -g -c $(ROOT)/world/caregiver.cpp $(ROOT)/world/caregiver.h $(INCLUDE)
	
primal.o :  $(ROOT)/world/primal.cpp
	$(CPP) -g -c $(ROOT)/world/primal.cpp $(ROOT)/world/primal.h $(INCLUDE)
	
judge.o :  $(ROOT)/world/judge.cpp
	$(CPP) -g -c $(ROOT)/world/judge.cpp $(ROOT)/world/judge.h $(INCLUDE)
	
transformer.o :  $(ROOT)/world/transformer/transformer.cpp
	$(CPP) -g -c $(ROOT)/world/transformer/transformer.cpp $(ROOT)/world/transformer/transformer.h $(INCLUDE)
	
watchdog.o :  $(ROOT)/world/watchdog.cpp
	$(CPP) -g -c $(ROOT)/world/watchdog.cpp $(ROOT)/world/watchdog.h $(INCLUDE)
	
main.o :  $(ROOT)/world/main.cpp
	$(CPP) -g -c $(ROOT)/world/main.cpp $(INCLUDE)
	
mutator.o	 :  $(ROOT)/world/mutator.cpp
	$(CPP) -g -c $(ROOT)/world/mutator.cpp $(ROOT)/world/mutator.h $(INCLUDE)
	
messenger.o :  $(ROOT)/helpers/messenger/messenger.cpp
	$(CPP) -g -c $(ROOT)/helpers/messenger/messenger.cpp $(ROOT)/helpers/messenger/messenger.h $(INCLUDE)
	
communication.o	 :  $(ROOT)/helpers/communication/communication.cpp
	$(CPP) -g -c $(ROOT)/helpers/communication/communication.cpp $(ROOT)/helpers/communication/communication.h $(INCLUDE)
	
world: $(WORLD_O) mkdir
	$(CPP) $(CFLAGS) -o ./build/world.bin $(WORLD_O) $(LIBS)
	
run_world: world
	./build/world.bin 1337 ./tasks/hello.c
	
precompile:
	$(CC) -E ./tasks/hello.c > ./tasks/hello.pre
	
lex: mkdir
	flex $(LEX)
	$(CPP) lex.yy.c -o ./build/lexer.bin -DLEX -ll
	rm lex.yy.c


yacc: mkdir
	
#	"=================================================================="
#	"= BUILD                 $(VERSION) ="
#	"= TARGET                                                lex_yacc ="
#	"=================================================================="
	flex $(LEX)
	yacc -d $(YACC)
	$(CPP) $(YACC_C) -o ./build/lexer.bin $(INCLUDE) -DYACC -ll $(LIBS)
	rm lex.yy.c y.tab.c y.tab.h

test:
	test -f ./build/lexer.bin && cat ./tasks/hello.c | ./build/lexer.bin	

clean:
	rm -rf *.o  ./build
	rm -rf *.o  ./results
	rm -rf src/world/transformer/yacc/*.h.gch 
	rm -rf src/world/transformer/*.h.gch 
	rm -rf src/world/*.h.gch
	rm -rf src/helpers/communication/communication.h.gch
	rm -rf src/helpers/messenger/messenger.h.gch
	
clean_lex:
	rm ./build/lexer.bin lex.yy.c y.tab.c y.tab.h

clean_all: clean clean_lex
	rm -rf *.out *~ 