#include "analyzer.h"

Analyzer::Analyzer(){

}

Analyzer::Analyzer(const string input){
    inFile = fopen(input.c_str(),"r+");
}

Analyzer::~Analyzer(){
    if (inFile && feof(inFile)) {
        fclose(inFile);
    }
}
