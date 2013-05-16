#include "transformer.h"

Transformer::Transformer(): Messenger(MSG_INFO) {
}

Transformer::Transformer(msg_severity_t msg_lvl) : Messenger(msg_lvl) {
}

Transformer::~Transformer(){
}

int Transformer::transform(const char *msg){
    if ( msg ){
        printf("MSG:\n"
                "=============================\n"
                "%s\n"
                "=============================\n", (char*) msg);
        return 0;
    } else {
        error("No message to transform was supplied");
    }
    return 1;
}
