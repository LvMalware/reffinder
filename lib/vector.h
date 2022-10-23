#ifndef _VECTOR_H
#define _VECTOR_H

#include <stdlib.h>
#include <string.h>
#include "memdebug.h"

#define MIN_ARRAY_SIZE 0x20

struct get_param_s {
    char *name;
    char *value;
};

struct param_vector_s {
    size_t size;
    size_t capacity;
    struct get_param_s *array;
};

void param_vector_init(struct param_vector_s * const);
void param_vector_cleanup(struct param_vector_s * const);
void param_vector_insert(struct param_vector_s * const,
                   char * const, size_t, char * const, size_t);

#endif /* vector.h */
