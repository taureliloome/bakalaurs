#include "analyzer.h"

Analyzer::Analyzer(){

}

Analyzer::Analyzer(static String input){
    inFile = fopen(input.c_str(),"r+");
}

Analyzer::~Analyzer(){
    if (inFile.is_open()) {
        fclose(inFile);
    }
}
