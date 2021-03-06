/**
 *  @author    Jānis Knets
 *  @version   0.1
 *  @date      2013-04-14
 *  @copyright GNU Public License.
 *
 *  @brief     Judge class.
 *  @details   This class is used to transform give source code file in to primal
 *              blocks that are recognized by other modules. Transformation of
 *              two differently written algorithm realizations should transform
 *              to an equal representation.
 * */

#ifndef _TRANSFORMER_H_
#define _TRANSFORMER_H_
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <stack>
#include <iostream>
#include "messenger.h"
#include "primal.h"
#include "world_types.h"
using namespace std;

class Transformer: Messenger{
    uint32_t nodeIds;
    stack<nucleotide_t *> backtrace;
    Primal *primal;
    nucleotide_t *prev;
    nucleotide_t root;
    bool isParam;
public:
    Transformer();
    Transformer(msg_severity_t msg_lvl);
    ~Transformer();
    int transform(transfer_t *msg, size_t len);
    nucleotide_t *createNucleotide(const char *type, const char *name, const char *val);

    void nucleotideToStr(nucleotide_t *nucleotide);
    void printFiles(bool print);
    void printChildren(nucleotide_t *parent, size_t depth);

    const char *subtypeToStr(nucleobase_u data);
    const char *typeToStr(nucleobase_u type);
    const char *baseToStr(nucleotide_base_e base);
    const char *controlToStr(nucleotide_control_e control);
    const char *loopToStr(nucleotide_loop_e loop);
    const char *jumpToStr(nucleotide_jump_e jump);
    const char *supportToStr(nucleotide_support_e support);
    const char *assignsToStr(nucleotide_assigns_e assign);
    const char *compareToStr(nucleotide_compare_e compare);
    const char *operatorToStr(nucleotide_operator_e oper);

    nucleotide_type_e strToType(const char *type, nucleotide_u *subtype);
    nucleotide_base_e strToBase(const char *base);
    nucleotide_control_e strToControl(const char *control);
    nucleotide_loop_e strToLoop(const char *loop);
    nucleotide_jump_e strToJump(const char *jump);
    nucleotide_support_e strToSupport(const char *support);
    nucleotide_assigns_e strToAssigns(const char *assigns);
    nucleotide_compare_e strToCompare(const char *compare);
    nucleotide_operator_e strToOperator(const char *oper);

    bool createNucleobase(nucleotide_t *nucleo, nucleotide_type_e type, nucleotide_u subtype);
    bool attachNucleobase(nucleotide_t *parent, nucleotide_t *child);
    void printNucleobase(nucleotide_t *nucleo);

};

#endif /* _TRANSFORMER_H_ */
