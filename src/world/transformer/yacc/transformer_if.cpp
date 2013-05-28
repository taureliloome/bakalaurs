#include "transformer_if.h"
extern char * strcpy(char * destination, const char * source);

TransformerIf::TransformerIf() :
        Messenger(MSG_INFO) {
    memset(&accBuff, 0, sizeof(0));
    memset(sendBuff, 0, sizeof(0));
    clearBufs();
}

TransformerIf::TransformerIf(msg_severity_t msg_lvl) :
        Messenger(msg_lvl) {
    memset(&accBuff, 0, sizeof(0));
    memset(sendBuff, 0, sizeof(0));
    clearBufs();
}

TransformerIf::~TransformerIf() {
    memset(&accBuff, 0, sizeof(0));
    memset(sendBuff, 0, sizeof(0));
    clearBufs();
}

void TransformerIf::addKey(const char *key, int debug) {
    debug3("Adding key %s %d", key, debug);
    addBuff(key, VAR_KEY);
}

void TransformerIf::addName(const char *name, int debug) {
    debug3("Adding name %s %d", name, debug);
    addBuff(name, VAR_NAME);
}

void TransformerIf::addVal(const char *val, int debug) {
    debug3("Adding value %s %d", val, debug);
    addBuff(val, VAR_VAL);
}

void TransformerIf::addBuff(const char * var, var_e type) {
    switch (type) {
    case VAR_KEY:
        strcpy(accBuff.key, var);
        break;

    case VAR_NAME:
        strcpy(accBuff.name, var);
        break;

    case VAR_VAL:
        strcpy(accBuff.val, var);
        break;
    }
}

void TransformerIf::addParam(const char *key, const char *name, const char *val, int debug) {
    addKey(key, debug);
    addName(name, debug);
    addVal(val, debug);
    addToBuff(debug, true);
}

void TransformerIf::addToBuff(int debug, bool clearKeyBuff) {
    if (strlen(accBuff.key) == 0) {
        info("nothing to add %d", debug);
    } else if (len < MSG_BUFFER_SIZE) {
        debug3("Adding to buffer %s %s %s | %d", accBuff.key, accBuff.name, accBuff.val, debug);
        if (strlen(accBuff.name) > 0 || strlen(accBuff.val) > 0) {
            sendBuff[len] = accBuff;
            len++;
            debug3("Added");
        }
    } else {
        error("Full buffer\n");
        return;
    }
    if (clearKeyBuff) {
        memset(accBuff.key, 0, MAX_KEY_LEN);
    }
    memset(accBuff.name, 0, MAX_NAME_LEN);
    memset(accBuff.val, 0, MAX_VAL_LEN);
}

void TransformerIf::clearBufs() {
    memset(&accBuff, 0, sizeof(accBuff));
}

transfer_t *TransformerIf::getBuffPtr() {
    return sendBuff;
}

size_t TransformerIf::getAndResetBuffLen() {
    size_t tmp = sizeof(transfer_t) * len;
    debug("size of sendBuff is %lu ( %lu", len, tmp);
    len = 0;
    return tmp;
}
