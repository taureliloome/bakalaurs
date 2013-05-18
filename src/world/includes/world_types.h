#ifndef __WORLD_TYPES_H__
#define __WORLD_TYPES_H__

typedef enum {
    NUCLEO_SINGLE = 0,
    NUCLEO_IF,
    NUCLEO_ELIF,
    NUCLEO_ELSE,
    NUCLEO_FUNC,
    NUCLEO_MAX
} nucleotide_type_e;

typedef struct nucleotide_s{
    char file[32];              // Name of the source code file
    char name[32];              // Variable name.
    nucleotide_type_e type;     // Type of nucleotide see @ref nucleotide_type_e
    uint64_t id;                // Id of current child, these are used to compare, balance and find nucleotides

    nucleotide_t *parent;       // Parent block, NULL if it's main();
    nucleotide_t *child;        // First child block
    nucleotide_t *sibling;      // Sigling blocks to current, sorted and ordered, NULL if main();

    nucleotide_t *statement;    // Statement for this block to be active, NULL if not _IF or _ELIF

    size_t len;
    char *val;
}nucleotide_t;

#endif /* __WORLD_TYPES_H__ */
