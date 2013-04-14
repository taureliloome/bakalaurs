#include "watchdog.h"

Watchdog::Watchdog(){

}

Watchdog::Watchdog(static String input){
    inFile = fopen(input.c_str(),"r+");
}

Watchdog::~Watchdog(){
    if (inFile.is_open()) {
        fclose(inFile);
    }
}
