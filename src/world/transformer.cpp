#include "transformer.h"

Transformer::Transformer(): Messenger(MSG_INFO) {

}

Transformer::Transformer(const string input): Messenger(MSG_INFO) {
    inFile = fopen(input.c_str(),"r+");
}

Transformer::Transformer(const char *input, msg_severity_t msg_lvl) : Messenger(msg_lvl) {
    inFile = fopen(input,"r+");
    //TODO: transform given file to nucleotide
    transform();
}

Transformer::~Transformer(){
    if (inFile && feof(inFile)) {
        fclose(inFile);
    }
}

void Transformer::transform(){
    printf("it works!\n");
}
