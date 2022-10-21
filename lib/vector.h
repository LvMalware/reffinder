#ifndef _VECTOR_H
#define _VECTOR_H

#include <stdlib.h>
#include <string.h>

#define MIN_ARRAY_SIZE 0x20

#define VECTOR_INDEX(vec, ind) \
    (ind < vec->size) ? vec->array[ind] : NULL

#define VECTOR_FIRST(vec) VECTOR_INDEX(vec, 0) 

#define VECTOR_LAST(vec) \
    (vec->size > 0) ? vec->array[vec->size - 1] : NULL

struct vector {
    size_t size;
    size_t real_size;
    void **array;
};

void * vector_pop(struct vector *);
struct vector * vector_init(size_t);
void vector_cleanup(struct vector *);
struct vector * vector_insert(struct vector *, void *);
struct vector * vector_remove_at(struct vector *, size_t);
struct vector * vector_insert_at(struct vector *, void *, size_t);

#endif /* vector.h */
