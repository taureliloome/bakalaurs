#include "caregiver.h"

Caregiver::Caregiver(){

}

Caregiver::Caregiver(static String input){
    inFile = fopen(input.c_str(),"r+");
}

Caregiver::~Caregiver(){
    if (inFile.is_open()) {
        fclose(inFile);
    }
}
