#include "watchdog.h"

Watchdog::Watchdog(){

}

Watchdog::Watchdog(const string input){
    inFile = fopen(input.c_str(),"r+");
}

Watchdog::~Watchdog(){
    if (feof(inFile)) {
        fclose(inFile);
    }
}
