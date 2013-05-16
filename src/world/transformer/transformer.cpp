#include "transformer.h"

Transformer::Transformer(): Messenger(MSG_INFO) {
}

Transformer::Transformer(msg_severity_t msg_lvl) : Messenger(msg_lvl) {
    //TODO: transform given file to nucleotide
    transform();
}

Transformer::~Transformer(){
}

void Transformer::transform(){
    printf("it works!\n");
}
