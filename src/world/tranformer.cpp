#include "transformer.h"

Transformer::Transformer(): Messenger(MSG_INFO) {

}

Transformer::Transformer(static String input): Messenger(MSG_INFO) {
    inFile = fopen(input.c_str(),"r+");
}

Transformer::Transformer(static char *input, msg_severity_t msg_lvl) : Messenger(msg_lvl) {
    inFile = fopen(input,"r+");
    //TODO: transform given file to nucleotide
    transform();
}

Transformer::~Transformer(){
    if (inFile.is_open()) {
        fclose(inFile);
    }
}
