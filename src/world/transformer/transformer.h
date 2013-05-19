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
#include <iostream>
#include "messenger.h"
#include "world_types.h"
using namespace std;

class Transformer: Messenger{
public:
    Transformer();
    Transformer(msg_severity_t msg_lvl);
    ~Transformer();
    int transform(const char *msg);

    void nucleotideToStr(nucleotide_t *nucleotide);

    const char *typeToStr(nucleotide_type_e type);
    const char *baseToStr(nucleotide_base_e base);

    nucleotide_type_e strToType(const char *type, uint32_t *subtype);
    nucleotide_base_e strToBase(const char *base);
    nucleotide_control_e strToControl(const char *control);
    nucleotide_loop_e strToLoop(const char *loop);
    nucleotide_jump_e strToJump(const char *jump);

    nucleotide_t *createNucleotide(const char *type, const char *name, const char *val);
};

#endif /* _TRANSFORMER_H_ */
