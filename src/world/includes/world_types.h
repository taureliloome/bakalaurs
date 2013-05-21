#ifndef __WORLD_TYPES_H__
#define __WORLD_TYPES_H__
#include <stdint.h>

#define MAX_KEY_LEN  8
#define MAX_NAME_LEN 64
#define MAX_VAL_LEN 64
#define NUCLEOTIDE_NAME_MAX_LEN MAX_NAME_LEN
#define FILE_NAME_MAX_LEN 64

typedef enum {
    NUCLEO_TYPE_BASE = 0,
    NUCLEO_TYPE_CONTROL,
    NUCLEO_TYPE_LOOP,
    NUCLEO_TYPE_JUMP,
    NUCLEO_TYPE_UNDEFINED
} nucleotide_type_e;

typedef enum {
    /* defined types */
    NUCLEO_BASE_VOID = 0,
    NUCLEO_BASE_CHAR,
    NUCLEO_BASE_SHORT,
    NUCLEO_BASE_INT,
    NUCLEO_BASE_LONG,
    NUCLEO_BASE_FLOAT,
    NUCLEO_BASE_DOUBLE,
    NUCLEO_BASE_SIGNED,
    NUCLEO_BASE_UNSIGNED,
    NUCLEO_BASE_BOOL,
    NUCLEO_BASE_STRING,
    NUCLEO_BASE_UNDEFINED
} nucleotide_base_e;

typedef enum {
    NUCLEO_CONTROL_FUNCTION = 0,
    NUCLEO_CONTROL_IF,
    NUCLEO_CONTROL_ELIF,
    NUCLEO_CONTROL_ELSE,
    NUCLEO_CONTROL_SWITCH,
    NUCLEO_CONTROL_UNDEFINED
} nucleotide_control_e;

typedef enum {
    NUCLEO_LOOP_DO = 0, NUCLEO_LOOP_WHILE, NUCLEO_LOOP_FOR, NUCLEO_LOOP_UNDEFINED
} nucleotide_loop_e;

typedef enum {
    NUCLEO_JUMP_RETURN = 0,
    NUCLEO_JUMP_BREAK,
    NUCLEO_JUMP_CONTINUE,
    NUCLEO_JUMP_GOTO,
    NUCLEO_JUMP_UNDEFINED
} nucleotide_jump_e;

typedef struct transfer_s {
    char key[MAX_KEY_LEN];
    char name[MAX_NAME_LEN];
    char val[MAX_VAL_LEN];
} transfer_t;

typedef union {
    nucleotide_type_e type;
    nucleotide_base_e base;
    nucleotide_control_e control;
    nucleotide_loop_e loop;
    nucleotide_jump_e jump;
    uint32_t raw;
} nucleotide_u;

typedef union {
    char c;
    short s;
    int i;
    long l;
    float f;
    double d;
    signed sg;
    unsigned usg;
    bool b;
    char str[MAX_VAL_LEN];
}nucleotide_base_u;

typedef struct nucleotide_s {
    char file[FILE_NAME_MAX_LEN];                   // Name of the source code file
    char name[NUCLEOTIDE_NAME_MAX_LEN];             // Variable name.
    nucleotide_type_e type;                         // Type of nucleotide see @ref nucleotide_type_e
    nucleotide_u subtype;
    union {
        nucleotide_base_u base;
        struct {
            struct nucleotide_s *statement; // Statement for this block to be active, NULL if not _IF or _ELIF
            struct nucleotide_s *child;                             // First child block
        } control;
        struct {
            struct nucleotide_s *statement; // Statement for this block to be active, NULL if not _IF or _ELIF
            struct nucleotide_s *child;                             // First child block
        } loop;
        struct {
            struct nucleotide_s *jump;
            struct nucleotide_s *ret;
        } ret;
    }subvalues;
    uint64_t id;     // Id of current child, these are used to compare, balance and find nucleotides

    nucleotide_s *parent;                           // Parent block, NULL if it's main();
    nucleotide_s *sibling;         // Sibling blocks to current, sorted and ordered, NULL if main();

    size_t len;
    char *val;
} nucleotide_t;

#endif /* __WORLD_TYPES_H__ */
