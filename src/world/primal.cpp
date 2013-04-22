#include "primal.h"

Primal::Primal(){

}

Primal::Primal(const string input){
    inFile = fopen(input.c_str(),"r+");
}

Primal::~Primal(){
    if (inFile && feof(inFile)) {
        fclose(inFile);
    }
}
