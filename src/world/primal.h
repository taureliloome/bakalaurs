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
#include <string>
#include <list>
#include <iostream>
#include "messenger.h"
#include "world_types.h"
#include <dirent.h>

extern "C" {
#include <avl.h>
}
using namespace std;

typedef void (avl_iterate_cb_t)(nucleotide_t *ret);

class Primal: Messenger {
    static bool instanceFlag;
    static Primal *self;
    avl_tree_t *primal;
    list<string> files;
    Primal(msg_severity_t msg_lvl = MSG_INFO);
public:
    static Primal *getInstance(msg_severity_t msg_lvl = MSG_INFO);
    ~Primal();
    bool initTree();
    bool getInputFileList(const char *path);
    transfer_t *fileListToMessage(size_t *size);
    int _compare(const nucleotide_t *left, const nucleotide_t *right);
    void _destroy(void *ptr);

    void iterate(const avl_node_t *parent, avl_iterate_cb_t *iterate_cb);

    void insert(nucleotide_t *insert);
    void update(nucleotide_t *search, nucleotide_type_e type, uint32_t subtype);
    nucleotide_t *search(nucleotide_t* nucleotide);
};

#endif /* _PRIMAL_H_ */
