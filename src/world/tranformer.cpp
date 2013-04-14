#include "transformer.h"

Transformer::Transformer(){

}

Transformer::Transformer(static String input){
    inFile = fopen(input.c_str(),"r+");
}

Transformer::~Transformer(){
    if (inFile.is_open()) {
        fclose(inFile);
    }
}
