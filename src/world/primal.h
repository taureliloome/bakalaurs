/**
 *  @author    Jānis Knets
 *  @version   0.1
 *  @date      2013-04-14
 *  @copyright GNU Public License.
 *
 *  @brief     Primal class.
 *  @details     This class is used to store and categorize different widely used
 *             blocks of code in a database like fashion. From this class @see
 *             Mutator takes building blocks for new mutations.
 * */

#ifndef _PRIMAL_H_
#define _PRIMAL_H_
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <iostream>
#include "world_types.h"

extern "C"{
#include <avl.h>
}
using namespace std;


class Primal {
    static bool instanceFlag;
    static Primal *self;
    avl_tree_t *primal;
    Primal();
public:
    static Primal *getInstance();
    ~Primal();
    int _compare(const void *left, const void *right);
    void _destroy(void *ptr);
};

#endif /* _PRIMAL_H_ */
