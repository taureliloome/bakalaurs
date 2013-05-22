#include "primal.h"

bool Primal::instanceFlag = false;
Primal* Primal::self = NULL;

static int compare(const void *left, const void *right)
{
    return (Primal::getInstance())->_compare(left, right);
}

static void destroy(void *ptr)
{
    (Primal::getInstance())->_destroy(ptr);
}

Primal *Primal::getInstance(){
    if (!instanceFlag) {
        self = new Primal();
        instanceFlag = true;
    }
    return self;
}

Primal::Primal()
{
    primal = NULL;
    avl_init_tree(primal, compare, destroy);
}

Primal::~Primal()
{
    avl_free_tree(primal);
}

void Primal::_destroy(void *ptr)
{
    nucleotide_t *nucleotide = (nucleotide_t *) ptr;
    if (nucleotide->val != NULL) {
        free(nucleotide->val);
    }
    /* Do the children have to be removed, or just marked for removal on next iteration?
    if (nucleotide->type == NUCLEO_TYPE_CONTROL) {
        _destroy(nucleotide->subvalues.control.child);
    } else if (nucleotide->type == NUCLEO_TYPE_LOOP) {
        _destroy(nucleotide->subvalues.loop.child);
    }
    */
    free(nucleotide);
}

int Primal::_compare(const void *left, const void *right)
{
    if (((nucleotide_t *) left)->id > ((nucleotide_t *) right)->id)
        return -1;
    if (((nucleotide_t *) left)->id < ((nucleotide_t *) right)->id)
        return 1;
    return 0;
}
