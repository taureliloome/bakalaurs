#include "judge.h"

Judge::Judge(){

}

Judge::Judge(const string input){
    inFile = fopen(input.c_str(),"r+");
}

Judge::~Judge(){
    if (inFile && feof(inFile)) {
        fclose(inFile);
    }
}
