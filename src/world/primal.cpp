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

bool Primal::getInputFileList(const char *path) {
    DIR *dir;
    string input;
    struct dirent *ent;
    if ((dir = opendir(path)) != NULL) {
        while ((ent = readdir(dir)) != NULL) {
            input.erase();
            if ( strcmp(ent->d_name,".") && strcmp(ent->d_name,"..")  ){
                input += path;
                input += '/';
                input += ent->d_name;
                debug3("%s ", input.c_str());
                files.push_front(input);
            }
        }
        closedir(dir);
    } else {
        perror("");
        return false;
    }
    return true;
}

transfer_t *Primal::fileListToMessage(size_t *size){
    *size = sizeof(transfer_t) * files.size();
    transfer_t *message = (transfer_t *) malloc(*size);
    int i = 0;
    for (list<string>::iterator it=files.begin(); it!=files.end(); ++it){
        strcpy(message[i].key, it->c_str());
        debug3("%s",message[i].key);
        i++;
    }
    return message;
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
