#include "primal.h"

Primal::Primal(){

}

Primal::Primal(static String input){
    inFile = fopen(input.c_str(),"r+");
}

Primal::~Primal(){
    if (inFile.is_open()) {
        fclose(inFile);
    }
}
