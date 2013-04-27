#include "mutator.h"

Mutator::Mutator(){

}

Mutator::Mutator(const string input){
    inFile = fopen(input.c_str(),"r+");
}

Mutator::~Mutator(){
    if (inFile && feof(inFile)) {
        fclose(inFile);
    }
}
