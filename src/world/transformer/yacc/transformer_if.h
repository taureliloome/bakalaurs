/**
 *  @author    Jānis Knets
 *  @version   0.1
 *  @date      2013-04-14
 *  @copyright GNU Public License.
 *
 *  @brief     Judge class.
 *  @details   This class is used to bufferize and prepare lex\yacc traversed data
 *              in to one message and later send it to world
 * */

#ifndef _TRANSFORMER_IF_H_
#define _TRANSFORMER_IF_H_
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <iostream>
#include "messenger.h"
using namespace std;

typedef enum{
    YACC_MSG_FULL = 0,
    YACC_MSG_PART,
    YACC_MSG_END,
    YACC_MSG_MAX
}yacc_msg_e;

/* Enumerator order affects sizes of varBuff in TransforerIf ! */
typedef enum{
    VAR_NAME = 0,
    VAR_KEY,
    VAR_VAL,
    VAR_BUFF,
    VAR_MAX
}var_e;

class TransformerIf: Messenger{
    static const size_t MSG_BUFFER_SIZE = 1024;

    size_t len;

    transfer_t sendBuff[MSG_BUFFER_SIZE];
    transfer_t accBuff;
public:
    TransformerIf();
    TransformerIf(msg_severity_t msg_lvl);
    ~TransformerIf();

    void clearBufs();

    void addParam(const char *key, const char *name, const char *val, int debug);
    void addKey(const char *key, int debug);
    void addName(const char *name, int debug);
    void addVal(const char *val, int debug);
    void addBuff(const char * var, var_e type);

    void addToBuff(int debug, bool clearKeyBuff = true);
    void itarate();
    transfer_t *getBuffPtr();
    size_t getAndResetBuffLen();
};

#endif /* _TRANSFORMER_IF_H_ */
