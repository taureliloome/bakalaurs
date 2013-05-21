/**
 *  @author    Jānis Knets
 *  @version   0.1
 *  @date      2013-04-14
 *  @copyright GNU Public License.
 *
 *  @brief     Mutator class.
 *  @details     This class is used to transform given blocks or their combination
 *             in to slightly new form of compilable code, thus creating new
 *             @see Organism
 * */

#ifndef _MUTATOR_H_
#define _MUTATOR_H_
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <iostream>
using namespace std;


class Mutator {
public:
    Mutator();
    Mutator(const string input);
    ~Mutator();
};

#endif /* _MUTATOR_H_ */
