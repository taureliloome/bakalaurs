/**
 *  @author    Jānis Knets
 *  @version   0.1
 *  @date      2013-04-14
 *  @copyright GNU Public License.
 *
 *  @brief     Organism main.
 *  @details   This is used to determine the range of correct answer from accepted scope.
 *             Gives the marks and promotes organisms to next generation.
 *  @author    Jānis Knets
 *  @version   0.1
 *  @date      2013-04-14
 *  @copyright GNU Public License.
 * */

#include <stdio.h>
#include <string.h>
#include "organism.h"
#include "communication.h"

int main(int argc, const char **argv){
	Organism a(MSG_DEBUG3);
	if ( argc >= 2 ){
		a.code(argv[1]);
	}
	a.code("./src/organism/hello_world.c");
	return 0;
}

