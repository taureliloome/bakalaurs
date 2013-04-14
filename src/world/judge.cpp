#include "judge.h"

Judge::Judge(){

}

Judge::Judge(static String input){
    inFile = fopen(input.c_str(),"r+");
}

Judge::~Judge(){
    if (inFile.is_open()) {
        fclose(inFile);
    }
}
