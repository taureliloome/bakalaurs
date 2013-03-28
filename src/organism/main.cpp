#include <stdio.h>
#include "organism.h"
using namespace std;

int main(int argc, const char **argv){
	Organism a(MSG_DEBUG3);
	if ( argc >= 2 ){
		a.code(argv[1]);
	}
	a.code("./src/organism/hello_world.c");
	return 0;
}
