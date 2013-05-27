#include <iostream>
#include <stdio.h>
#include <string>

#include "communication.h"
#include "transformer.h"

static Communicator *communicator = NULL;
static Primal *primal = NULL;

int main(int argv, char **argc) {
    /*
     Step 1 - Configuration using passed arguments
     +----------------------------+-------------------------+
     | Input parameters           | Requirement             |
     +----------------------------+-------------------------+
     | input data folder          | obligatory              |
     | processes count            | obligatory [DISABLED]   |
     | average life span          | obligatory [DISABLED]   |
     | year loop count            | obligatory [DISABLED]   |
     | result folder              | optional   [DISABLED]   |
     | log file                   | optional   [DISABLED]   |
     +----------------------------+-------------------------+
     */
    if (argv < 2 ) {
        printf("incorrect usage: incorrect amount of arguments (%d < 2)\n", argv);
        return 1;
    }

    /*
     Step 2 - setup world
     Launch and initialize required processes.
     Open communication sockets.
     Setup primal database.
     */
    communicator = Communicator::getInstance(true,"WORLD");
    primal = Primal::getInstance(MSG_DEBUG3);
    Transformer transformer(MSG_DEBUG3);

    communicator->CreateListenSocket(argc[1]);
    communicator->setTransformer(&transformer);

    primal->getInputFileList(argc[2]);

    /*
     Step 3 - setup first generation [DISABLED]
     First generation will be the first organism objects compiled, after this they only have to be managed with
     communication trough watchdog. Organisms shall spawn their own children.
     */

    /*
     Step 4 - sanity check [DISABLED]
     Check that communication is initialized with all of the first generation organisms.
     Check database block count
     */

    /*
     Step 5 - work
     Make number of loops set up by user and give the results
     */
    do{
        bool ret = communicator->waitForConnection();
        if (!ret ){
            return 1;
        }
    }while(communicator->getClientCount() > 0 );
    delete communicator;
    return 0;
}

