#include "caregiver.h"

Caregiver::Caregiver(){

}

Caregiver::Caregiver(const string input){
    inFile = fopen(input.c_str(),"r+");
}

Caregiver::~Caregiver(){
    if (inFile && feof(inFile)) {
        fclose(inFile);
    }
}
