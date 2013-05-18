#include "transformer_if.h"
extern char * strcpy(char * destination, const char * source);
extern size_t strlen(const char * str);

TransformerIf::TransformerIf() :
        Messenger(MSG_INFO) {
    memset(buff, 0, MSG_BUFFER_SIZE);
    memset(len, 0, sizeof(size_t) * VAR_MAX);
    memset(varBuff, 0, sizeof(size_t) * (VAR_VAL + 1));
}

TransformerIf::TransformerIf(msg_severity_t msg_lvl) :
        Messenger(msg_lvl) {
    memset(buff, 0, MSG_BUFFER_SIZE);
    memset(len, 0, sizeof(size_t) * VAR_MAX);
    memset(varBuff, 0, sizeof(size_t) * (VAR_VAL + 1));
}

TransformerIf::~TransformerIf() {
}

void TransformerIf::addKey(const char *key) {
    debug3("Adding key %s",key);
    addBuff(key, VAR_KEY);
}

void TransformerIf::addName(const char *name) {
    addBuff(name, VAR_NAME);
}

void TransformerIf::addVal(const char *val) {
    addBuff(val, VAR_VAL);
}

void TransformerIf::addBuff(const char * var, var_e type) {
    if (var) {
        if (len[type] != 0) {
            error("length was not removed, possible data destruction");
        }
        len[type] = strlen(var);
        if (len[type] < VAR_BUFFER_SIZE && len[type] > 0) {
            strcpy(varBuff[type], var);
        }
    }
}

void TransformerIf::addParam(const char *key, const char *name, const char *val) {
    addKey(key);
    addName(name);
    addVal(val);
}

void TransformerIf::addToBuff() {
    if (len[VAR_KEY] + len[VAR_NAME] + len[VAR_VAL] + len[VAR_BUFF] < MSG_BUFFER_SIZE) {
        char *a = &(buff[len[VAR_BUFF]]);
        len[VAR_BUFF] += (len[VAR_KEY] + len[VAR_NAME] + len[VAR_VAL] + 4);
        sprintf(a, "<%s|%s|%s>", varBuff[VAR_KEY], varBuff[VAR_NAME], varBuff[VAR_VAL]);
    } else {
        fprintf(stderr, "Full buffer\n");
        return;
    }
    len[VAR_KEY] = len[VAR_NAME] = len[VAR_VAL] = 0;
}

void TransformerIf::itarate() {

}

char *TransformerIf::getBuffPtr() {
    return buff;
}

size_t TransformerIf::getAndResetBuffLen() {
    size_t tmp = len[VAR_BUFF];
    len[VAR_BUFF] = 0;
    return tmp;
}
