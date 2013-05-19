#include "transformer.h"
extern int strcmp(const char * str1, const char * str2);

Transformer::Transformer() :
        Messenger(MSG_INFO) {
}

Transformer::Transformer(msg_severity_t msg_lvl) :
        Messenger(msg_lvl) {
}

Transformer::~Transformer() {
}

int Transformer::transform(const char *msg) {
    if (msg) {
        printf("MSG:\n"
                "=============================\n"
                "%s\n"
                "=============================\n", (char*) msg);
        size_t read = 0;
        size_t total = strlen(msg);
        nucleotide_t *nucleotide = NULL;
        char key[NUCLEOTIDE_NAME_MAX_LEN] = {'\0'};
        char name[NUCLEOTIDE_NAME_MAX_LEN]= {'\0'};
        char val[NUCLEOTIDE_NAME_MAX_LEN]= {'\0'};
        while (read < total) {
            sscanf(msg, "<%s|%s|%s>", key, name, val);
            read += strlen(key) + strlen(name) + strlen(val) + 4;
            nucleotide = createNucleotide(key, name, val);
            if (nucleotide){
                nucleotideToStr(nucleotide);
                free(nucleotide);
            } else {
                error("no nucleotide allocated");
                return 1;
            }
        }
        return 0;
    } else {
        error("No message to transform was supplied");
    }
    return 1;
}

void Transformer::nucleotideToStr(nucleotide_t *nucleotide){
    info("Nucleotide %p\n"
         "=============================\n"
         "Type\t\t %s\n"
         "Subtype\t\t%s\n"
         "=============================",
         nucleotide,
         typeToStr(nucleotide->type),
         baseToStr(nucleotide->subtype.base) );
}

const char *Transformer::typeToStr(nucleotide_type_e type){
    switch (type){
    case NUCLEO_TYPE_BASE:
        return "BASE";
        break;
    case NUCLEO_TYPE_CONTROL:
        return "CONTROL";
        break;
    case NUCLEO_TYPE_LOOP:
        return "LOOP";
        break;
    case NUCLEO_TYPE_JUMP:
        return "JUMP";
        break;
    }
    return "UNDEFINED";
}

const char *Transformer::baseToStr(nucleotide_base_e base){
    switch (base){
    case NUCLEO_BASE_VOID:
        return "BASE";
        break;
    case NUCLEO_BASE_CHAR:
        return "CONTROL";
        break;
    case NUCLEO_BASE_SHORT:
        return "SHORT";
        break;
    case NUCLEO_BASE_INT:
        return "INT";
        break;
    case NUCLEO_BASE_LONG:
        return "LONG";
        break;
    case NUCLEO_BASE_FLOAT:
        return "FLOAT";
        break;
    case NUCLEO_BASE_DOUBLE:
        return "DOUBLE";
        break;
    case NUCLEO_BASE_SIGNED:
        return "SIGNED";
        break;
    case NUCLEO_BASE_UNSIGNED:
        return "UNSIGNED";
        break;
    case NUCLEO_BASE_BOOL:
        return "BOOL";
        break;
    }
    return "UNDEFINED";
}

nucleotide_type_e Transformer::strToType(const char *type, uint32_t *subtype) {
    *subtype = strToBase(type);
    if (*subtype != NUCLEO_BASE_UNDEFINED)
        return NUCLEO_TYPE_BASE;
    *subtype = strToControl(type);
    if (*subtype != NUCLEO_BASE_UNDEFINED)
        return NUCLEO_TYPE_CONTROL;
    *subtype = strToLoop(type);
    if (*subtype != NUCLEO_BASE_UNDEFINED)
        return NUCLEO_TYPE_LOOP;
    *subtype = strToJump(type);
    if (*subtype != NUCLEO_BASE_UNDEFINED)
        return NUCLEO_TYPE_JUMP;
    return NUCLEO_TYPE_UNDEFINED;
}

nucleotide_base_e Transformer::strToBase(const char *base) {
    if (!strcmp(base, "void")) {
        return NUCLEO_BASE_VOID;
    } else if (!strcmp(base, "char")) {
        return NUCLEO_BASE_CHAR;
    } else if (!strcmp(base, "short")) {
        return NUCLEO_BASE_SHORT;
    } else if (!strcmp(base, "int")) {
        return NUCLEO_BASE_INT;
    } else if (!strcmp(base, "long")) {
        return NUCLEO_BASE_LONG;
    } else if (!strcmp(base, "float")) {
        return NUCLEO_BASE_FLOAT;
    } else if (!strcmp(base, "signed")) {
        return NUCLEO_BASE_DOUBLE;
    } else if (!strcmp(base, "double")) {
        return NUCLEO_BASE_SIGNED;
    } else if (!strcmp(base, "unsigned")) {
        return NUCLEO_BASE_UNSIGNED;
    } else if (!strcmp(base, "bool")) {
        return NUCLEO_BASE_BOOL;
    }
    return NUCLEO_BASE_UNDEFINED;
}

nucleotide_control_e Transformer::strToControl(const char *control) {
    return NUCLEO_CONTROL_UNDEFINED;
}

nucleotide_loop_e Transformer::strToLoop(const char *loop) {
    return NUCLEO_LOOP_UNDEFINED;
}

nucleotide_jump_e Transformer::strToJump(const char *jump) {
    return NUCLEO_JUMP_UNDEFINED;
}

nucleotide_t *Transformer::createNucleotide(const char *type, const char *name, const char *val) {
    nucleotide_t *nucleotide = (nucleotide_t *) malloc(sizeof(nucleotide));
    if (nucleotide == NULL)
        return NULL;
    uint32_t subtype;
    nucleotide->type = strToType(type, &subtype);
    nucleotide->subtype.raw = subtype;
    return nucleotide;
}
