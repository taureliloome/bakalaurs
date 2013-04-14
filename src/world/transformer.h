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


class Transformer {
    File inFile;
public:
    Transformer();
    Transformer(static String input);
    ~Transformer();
};

#endif /* _TRANSFORMER_H_ */
