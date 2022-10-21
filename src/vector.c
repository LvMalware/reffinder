#include "vector.h"

struct vector * vector_init(size_t initial_size) {
    if (initial_size < MIN_ARRAY_SIZE) initial_size = MIN_ARRAY_SIZE;
    struct vector * vec = malloc(1 * sizeof(*vec));
    *vec = (struct vector) {
        0,
        initial_size,
        calloc(initial_size, sizeof(*vec->array))
    };
    return vec;
}

void vector_cleanup(struct vector *vec) {
    if (!vec) return;
    if (vec->array) free(vec->array);
    free(vec);
}

static inline void vector_ensure_space (struct vector *vec, size_t space_needed) {
    if (vec->real_size <= space_needed) {
        vec->real_size <<= 1;
        vec->array = realloc(vec->array, vec->real_size * sizeof(vec->array[0]));
    }
}

static inline void vector_shrink_space (struct vector *vec) {
    if (vec->real_size > (vec->size << 2) && vec->real_size > MIN_ARRAY_SIZE) {
        vec->real_size >>= 1;
        vec->array = realloc(vec->array, vec->real_size * sizeof(vec->array[0]));
    }
}

struct vector * vector_insert(struct vector *vec, void * val) {
    if (!vec) vec = vector_init(0);
    vector_ensure_space(vec, vec->size + 1);
    vec->array[vec->size ++] = val;
    return vec;
}

struct vector * vector_insert_at(struct vector * vec, void * val, size_t index) {
    if (!vec) vec = vector_init(0);
    vector_ensure_space(vec, vec->size + 1);
    if (index < vec->size) {
        memmove(&vec->array[index + 1],
                &vec->array[index],
                (vec->size - index) * sizeof(vec->array[0]));
        vec->array[index] = val;
    } else {
        vec->array[vec->size] = val;
    }
    vec->size ++;
    return vec;
}

struct vector * vector_remove_at(struct vector * vec, size_t index) {
    if (vec && index < vec->size) {
        memmove(&vec->array[index],
                &vec->array[index - 1],
                (vec->size - index) * sizeof(vec->array[0]));
        vec->size --;
        vector_shrink_space(vec);
    }
    return vec;
}

void * vector_pop(struct vector * vec) {
    if (!vec || vec->size == 0) return NULL;
    return vec->array[-- vec->size];
}

