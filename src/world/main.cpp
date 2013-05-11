#include <iostream>
#include <stdio.h>
#include <string>

#include "communication.h"
#include "transformer.h"

static Communicator *communicator = NULL;

int main(int argv, char **argc) {
    /*
     Step 1 - Configuration using passed arguments
     +----------------------------+-------------------------+
     | Input parameters           | Requirement             |
     +----------------------------+-------------------------+
     | first file read            | obligatory              |
     | processes count            | obligatory              |
     | average life span          | obligatory              |
     | year loop count            | obligatory              |
     | result folder              | optional                |
     | log file                   | optional                |
     | resources_1	              | NOT_IMPLEMENTED         |
     +----------------------------+-------------------------+
     */
    if (argv < 2) {
        printf("incorrect usage not all arguments passed (%d < 2)\n", argv);
        return 1;
    }

    /*
     Step 2 - setup world
     Launch and initialize required processes.
     Open communication sockets.
     Setup primal database.
     */
    communicator = Communicator::getInstance();
    communicator->CreateListenSocket(argc[1]);
    bool ret = communicator->waitForConnection();
    if (!ret ){
        return 1;
    }
    //Transformer a(argc[2], MSG_DEBUG3);

    /*
     Step 3 - setup first generation
     First generation will be the first organism objects compiled, after this they only have to be managed with
     communication trough watchdog. Organisms shall spawn their own children.
     */

    /*
     Step 4 - sanity check
     Check that communication is initialized with all of the first generation organisms.
     Check database block count
     */

    /*
     Step 5 - work
     Make number of loops set up by user and give the results
     */
    communicator->communicate();
    delete communicator;
    return 0;
}

