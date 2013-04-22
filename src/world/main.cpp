#include <iostream>
#include <stdio.h>
#include <string>

#include "transformer.h"

int main(int argv, char **argc){
    //TODO: update param checks
    if ( argv < 1 ){
        printf("incorrect usage not enoght args passed (%d < 2)\n", argv);
        return 1;
    }

    Transformer a(argc[1],MSG_DEBUG3);
    return 0;
}


