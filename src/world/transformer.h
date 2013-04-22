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
using namespace std;

typedef struct nucleotide_s{
    char name[29];
    unsigned int id;
    size_t len;
    void *data;
}nucleotide_t;

class Transformer: Messenger{
    FILE *inFile;
public:
    Transformer();
    Transformer(const string input);
    Transformer(const char *input, msg_severity_t msg_lvl);
    ~Transformer();
    void transform();
};

#endif /* _TRANSFORMER_H_ */
