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
/*
int main(int argc, const char **argv){
	Organism a(MSG_DEBUG3);
	if ( argc >= 2 ){
		a.code(argv[1]);
	}
	a.code("./src/organism/hello_world.c");
	return 0;
}
*/

static Communicator *self = NULL;
int main()
{
    self = Communicator::getInstance();
    /*
    FILE *fd = fopen("./server.out", "w+");
    if ( fd )
        setOutputType(fd);
    */
    if ( !self->ConnectToServer("127.0.0.1","1337") ){
        delete self;
        return 0;
    }
    char buf[] = "HELLO\n";
    self->SendMessage(0, buf, strlen(buf), 0);
    //yyparse();
    //TODO: Enable when required sends messages to the list server
    //submitDirSrv(argv);
    delete self;
    return 0;
}
