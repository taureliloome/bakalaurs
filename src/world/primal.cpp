#include "primal.h"

bool Primal::instanceFlag = false;
Primal* Primal::self = NULL;

static int compare(const void *left, const void *right) {
    return (Primal::getInstance())->_compare((nucleotide_t *)left, (nucleotide_t *)right);
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
            if (strcmp(ent->d_name, ".") && strcmp(ent->d_name, "..")) {
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

transfer_t *Primal::fileListToMessage(size_t *size) {
    *size = sizeof(transfer_t) * files.size();
    transfer_t *message = (transfer_t *) malloc(*size);
    int i = 0;
    for (list<string>::iterator it = files.begin(); it != files.end(); ++it) {
        strcpy(message[i].key, it->c_str());
        debug3("%s", message[i].key);
        i++;
    }
    return message;
}

void Primal::insert(nucleotide_t *insert) {
    avl_insert(primal, insert);
}

void Primal::update(nucleotide_t *search, nucleotide_type_e type, uint32_t subtype) {
    avl_node_t *node = avl_search(primal, search);
    if (node) {
        nucleotide_t *update = (nucleotide_t *) node->item;
        update->nucleobase[0].raw = ((uint64_t)((type << 5) + subtype ) << 56);
    }
}

nucleotide_t *Primal::search(nucleotide_t* nucleotide) {
    avl_node_t *tmp = avl_search(primal, nucleotide);
    if (tmp)
        return (nucleotide_t *) (tmp)->item;
    return NULL;
}

void Primal::iterate(const avl_node_t *parent, avl_iterate_cb_t *iterate_cb) {
    if (parent->left)
        iterate(parent->left, iterate_cb);
    iterate_cb((nucleotide_t *) parent->item);
    if (parent->right)
        iterate(parent->right, iterate_cb);
}

void Primal::_destroy(void *ptr) {
    nucleotide_t *nucleotide = (nucleotide_t *) ptr;
    if (nucleotide && nucleotide->nucleobase != NULL) {
        debug3("Removing element %p", nucleotide);
        free(nucleotide->nucleobase);
    }
    free(nucleotide);
}

int Primal::_compare(const nucleotide_t *left, const nucleotide_t *right) {
    debug3("comparing %s and %s", left->name, right->name);
    size_t len = right->nucleobase_count > left->nucleobase_count ? left->nucleobase_count : right->nucleobase_count  ;
    return memcmp(left->nucleobase, right->nucleobase, len * sizeof(uint64_t));
}
