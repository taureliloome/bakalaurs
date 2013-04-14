/**
 *  @author    Jānis Knets
 *  @version   0.1
 *  @date      2013-04-14
 *  @copyright GNU Public License.
 *
 *  @brief     Analyzer class.
 *  @details   This class is used to analyze passed blocks of code passed by
 *              @see Transformer class and comparing them to blocks that are
 *              stored in @see Primal class.
 * */

#ifndef _ANALYZER_H_
#define _ANALYZER_H_
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <iostream>


class Analyzer {
    File inFile;
public:
    Analyzer();
    Analyzer(static String input);
    ~Analyzer();
private:
    void findBlock();
};

#endif /* _ANALYZER_H_ */
