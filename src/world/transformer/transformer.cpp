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

int Transformer::transform(transfer_t *msg, size_t total) {
    if (msg) {
        size_t rd = 0;
        nucleotide_t *nucleotide = NULL;
        char key[NUCLEOTIDE_NAME_MAX_LEN] = { '\0' };
        char name[NUCLEOTIDE_NAME_MAX_LEN] = { '\0' };
        char val[NUCLEOTIDE_NAME_MAX_LEN] = { '\0' };
        total /= sizeof(transfer_t);
        while (rd < total) {
            rd++;
            debug3("%s %s %s", msg[rd].key, msg[rd].name, msg[rd].val);

            nucleotide = createNucleotide(msg[rd].key, msg[rd].name, msg[rd].val);
            if (nucleotide) {
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

const char *Transformer::subtypeToStr(nucleotide_type_e type, nucleotide_u subtype) {
    switch (type) {
    case NUCLEO_TYPE_BASE:
        return baseToStr(subtype.base);
        break;
    case NUCLEO_TYPE_CONTROL:
        return controlToStr(subtype.control);
        break;
    case NUCLEO_TYPE_LOOP:
        return loopToStr(subtype.loop);
        break;
    case NUCLEO_TYPE_JUMP:
        return jumpToStr(subtype.jump);
        break;
    }
    return "UNDEFINED";
}

const char *Transformer::typeToStr(nucleotide_type_e type) {
    switch (type) {
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

const char *Transformer::baseToStr(nucleotide_base_e base) {
    switch (base) {
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
    case NUCLEO_BASE_STRING:
        return "STRING";
        break;
    }
    return "UNDEFINED";
}

const char *Transformer::controlToStr(nucleotide_control_e control) {
    switch (control) {
    case NUCLEO_CONTROL_FUNCTION:
        return "FUNCTION";
        break;
    case NUCLEO_CONTROL_IF:
        return "IF";
        break;
    case NUCLEO_CONTROL_ELIF:
        return "ELSE IF";
        break;
    case NUCLEO_CONTROL_ELSE:
        return "ELSE";
        break;
    case NUCLEO_CONTROL_SWITCH:
        return "SWITCH";
        break;
    }
    return "UNDEFINED";
}

const char *Transformer::loopToStr(nucleotide_loop_e loop) {
    switch (loop) {
    case NUCLEO_LOOP_DO:
        return "DO WHILE";
        break;
    case NUCLEO_LOOP_WHILE:
        return "WHILE";
        break;
    case NUCLEO_LOOP_FOR:
        return "FOR";
        break;
    }
    return "UNDEFINED";
}

const char *Transformer::jumpToStr(nucleotide_jump_e jump) {
    switch (jump) {
    case NUCLEO_JUMP_RETURN:
        return "RETURN";
        break;
    case NUCLEO_JUMP_BREAK:
        return "BREAK";
        break;
    case NUCLEO_JUMP_CONTINUE:
        return "CONTINUE";
        break;
    case NUCLEO_JUMP_GOTO:
        return "GOTO";
        break;
    }
    return "UNDEFINED";
}

nucleotide_type_e Transformer::strToType(const char *type, uint32_t *subtype) {
    *subtype = 0;
    *subtype = strToBase(type);
    if (*subtype != NUCLEO_BASE_UNDEFINED) {
        return NUCLEO_TYPE_BASE;
    }
    *subtype = strToControl(type);
    if (*subtype != NUCLEO_CONTROL_UNDEFINED) {
        return NUCLEO_TYPE_CONTROL;
    }
    *subtype = strToLoop(type);
    if (*subtype != NUCLEO_LOOP_UNDEFINED) {
        return NUCLEO_TYPE_LOOP;
    }
    *subtype = strToLoop(type);
    if (*subtype != NUCLEO_JUMP_UNDEFINED) {
        return NUCLEO_TYPE_JUMP;
    }
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
    } else if (!strcmp(base, "string")) {
        return NUCLEO_BASE_STRING;
    }
    return NUCLEO_BASE_UNDEFINED;
}

nucleotide_control_e Transformer::strToControl(const char *control) {
    if (!strcmp(control, "func_srt")) {
        return NUCLEO_CONTROL_FUNCTION;
    } else if (!strcmp(control, "if")) {
        return NUCLEO_CONTROL_IF;
    } else if (!strcmp(control, "elif")) {
        return NUCLEO_CONTROL_ELIF;
    } else if (!strcmp(control, "else")) {
        return NUCLEO_CONTROL_ELSE;
    } else if (!strcmp(control, "switch")) {
        return NUCLEO_CONTROL_SWITCH;
    }
    return NUCLEO_CONTROL_UNDEFINED;
}

nucleotide_loop_e Transformer::strToLoop(const char *loop) {
    if (!strcmp(loop, "do_while")) {
        return NUCLEO_LOOP_DO;
    } else if (!strcmp(loop, "while")) {
        return NUCLEO_LOOP_WHILE;
    } else if (!strcmp(loop, "for")) {
        return NUCLEO_LOOP_FOR;
    }
    return NUCLEO_LOOP_UNDEFINED;
}

nucleotide_jump_e Transformer::strToJump(const char *jump) {
    if (!strcmp(jump, "return")) {
        return NUCLEO_JUMP_RETURN;
    } else if (!strcmp(jump, "break")) {
        return NUCLEO_JUMP_BREAK;
    } else if (!strcmp(jump, "continue")) {
        return NUCLEO_JUMP_CONTINUE;
    } else if (!strcmp(jump, "goto")) {
        return NUCLEO_JUMP_GOTO;
    }
    return NUCLEO_JUMP_UNDEFINED;
}

nucleotide_t *Transformer::createNucleotide(const char *type, const char *name, const char *val) {
    nucleotide_t *nucleotide = (nucleotide_t *) malloc(sizeof(nucleotide));
    if (nucleotide == NULL)
        return NULL;
    uint32_t subtype;
    nucleotide->type = strToType(type, &subtype);
    strcpy(nucleotide->name, name);
    nucleotide->subtype.raw = subtype;

    if (val) {
        switch (nucleotide->type) {
        case NUCLEO_TYPE_BASE:
            switch (nucleotide->subtype.base) {
            case NUCLEO_BASE_STRING:
                strcpy(nucleotide->subvalues.base.str, val);
                break;
            case NUCLEO_BASE_VOID:
                break;
            case NUCLEO_BASE_SIGNED:
                sscanf(val, "%d", &nucleotide->subvalues.base.sg);
                break;
            case NUCLEO_BASE_INT:
                sscanf(val, "%d", &nucleotide->subvalues.base.i);
                break;
            case NUCLEO_BASE_CHAR:
                sscanf(val, "%c", &nucleotide->subvalues.base.c);
                break;
            case NUCLEO_BASE_FLOAT:
                sscanf(val, "%f", &nucleotide->subvalues.base.f);
                break;
            case NUCLEO_BASE_DOUBLE:
                sscanf(val, "%lf", &nucleotide->subvalues.base.d);
                break;
            case NUCLEO_BASE_UNSIGNED:
                sscanf(val, "%u", &nucleotide->subvalues.base.usg);
                break;
            case NUCLEO_BASE_BOOL:
                if (strcmp("true", val))
                    nucleotide->subvalues.base.b = true;
                else
                    nucleotide->subvalues.base.b = false;
                break;
            }
            break;
        }
    }
    return nucleotide;
}

void Transformer::nucleotideToStr(nucleotide_t *nucleotide) {
    info(
            "\n=============================\n" "Type\t\t%s(%d)\n" "Subtype\t\t%s(%d)\nName\t\t%s\nValue\t\t%x\n" "=============================\n",

            typeToStr(nucleotide->type), nucleotide->type,
            subtypeToStr(nucleotide->type, nucleotide->subtype), nucleotide->subtype.base,
            nucleotide->name, nucleotide->subtype.raw);
}
