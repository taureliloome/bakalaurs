#include "mutator.h"

Mutator::Mutator(){

}

Mutator::Mutator(static String input){
    inFile = fopen(input.c_str(),"r+");
}

Mutator::~Mutator(){
    if (inFile.is_open()) {
        fclose(inFile);
    }
}
