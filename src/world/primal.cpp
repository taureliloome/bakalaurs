#include "primal.h"

bool Primal::instanceFlag = false;
Primal* Primal::self = NULL;

static int compare(const void *left, const void *right) {
    return (Primal::getInstance())->_compare(left, right);
}

static void destroy(void *ptr) {
    (Primal::getInstance())->_destroy(ptr);
}

Primal *Primal::getInstance(msg_severity_t msg_lvl) {
    if (!instanceFlag) {
        self = new Primal(msg_lvl);
        instanceFlag = true;
    }
    return self;
}

Primal::Primal(msg_severity_t msg_lvl) :
        Messenger(msg_lvl) {
    primal = NULL;
    initTree();
}

Primal::~Primal() {
    avl_free_tree(primal);
}

bool Primal::initTree() {
    if (!primal) {
        primal = avl_alloc_tree(compare, destroy);
    }
    if (!primal) {
        error("Unable to create primal database, kill me now");
        return false;
    }
    return true;
}

void Primal::insert(nucleotide_t *insert) {
    avl_insert(primal, insert);
}

void Primal::_destroy(void *ptr) {
    nucleotide_t *nucleotide = (nucleotide_t *) ptr;
    if (nucleotide && nucleotide->val != NULL) {
        debug3("Removing element %p", nucleotide);
        free(nucleotide);
    }
    free(nucleotide);
}

int Primal::_compare(const void *left, const void *right) {
    if (((nucleotide_t *) left)->id > ((nucleotide_t *) right)->id)
        return -1;
    if (((nucleotide_t *) left)->id < ((nucleotide_t *) right)->id)
        return 1;
    return 0;
}
