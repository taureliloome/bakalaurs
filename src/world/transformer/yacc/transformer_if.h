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
    static const size_t VAR_BUFFER_SIZE = 128;

    size_t len[VAR_MAX];

    char buff[MSG_BUFFER_SIZE];
    char varBuff[VAR_VAL + 1][VAR_BUFFER_SIZE];
public:
    TransformerIf();
    TransformerIf(msg_severity_t msg_lvl);
    ~TransformerIf();

    void addParam(const char *key, const char *name, const char *val);
    void addKey(const char *key);
    void addName(const char *name);
    void addVal(const char *val);
    void addBuff(const char * var, var_e type);

    void addToBuff();
    void itarate();
    char *getBuffPtr();
    size_t getAndResetBuffLen();
};

#endif /* _TRANSFORMER_IF_H_ */
