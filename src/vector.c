/*
 * Copyright 2022 Lucas V. Araujo (https://github.com/lvmalware)
 *
 * This file is part of RefFinder.
 * RefFinder is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or (at your
 * option) any later version.
 *
 * RefFinder is distributed in the hope that it will be useful,but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with RefFinder. If not, see <https://www.gnu.org/licenses/>.
 */

#include "vector.h"

static inline int vector_ensure_space(struct param_vector_s * const vec, size_t space_needed) {
    if (vec->capacity <= space_needed) {
        vec->capacity <<= 1;
        void *temp = realloc(vec->array, vec->capacity * sizeof(vec->array[0]));
        if (temp == NULL) return 0;
        vec->array = temp;
    }
    return 1;
}

static inline void vector_shrink_space(struct param_vector_s * const vec) {
    if (vec->capacity > (vec->size << 2) && vec->capacity > MIN_ARRAY_SIZE) {
        vec->capacity >>= 1;
        vec->array = realloc(vec->array, vec->capacity * sizeof(vec->array[0]));
    }
}

static inline void get_param_clean(struct get_param_s * const p) {
    free(p->name);
    free(p->value);
    p->value = p->name = NULL;
}

void param_vector_cleanup(struct param_vector_s * const vec) {
    if (vec == NULL) return;
    size_t i;
    for (i = 0; i < vec->size; i ++)
        get_param_clean(&(vec->array[i]));
    free(vec->array);
    vec->array = NULL;
    vec->size  = vec->capacity = 0;
}

void param_vector_init(struct param_vector_s * const vec) {
    if (vec == NULL) return;
    vec->array = calloc(MIN_ARRAY_SIZE, sizeof(vec->array[0]));
    if (vec->array == NULL) return;
    vec->size = 0;
    vec->capacity = MIN_ARRAY_SIZE;
}

void param_vector_insert(struct param_vector_s * const vec,
                         char * const param_name, size_t name_len,
                         char * const param_value, size_t value_len)
{
    if (!vec->array || !vector_ensure_space(vec, vec->size + 1)) return;
    if ((vec->array[vec->size].name = strndup(param_name, name_len)) == NULL)
        return;
    if ((vec->array[vec->size].value = strndup(param_value, value_len)) == NULL) {
        free(vec->array[vec->size].name);
        return;
    }
    vec->size ++;
}
