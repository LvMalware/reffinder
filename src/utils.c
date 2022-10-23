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

#include "utils.h"

static inline void generate_string_probe(char * const probe) {
    size_t i;
    for (i = 0; i < PROBE_STR_LEN; i ++)
        sprintf(&(probe[i]), "%x", rand() & 0xf);
}

int get_param_from_url(char * const url, struct param_vector_s * const params,
                                  struct string_buffer_s * const mod) {
    size_t start = strcspn(url, "?");
    if (url[start] == '\0') return 0;

    char probe[PROBE_STR_LEN];
    string_buffer_concat(mod, url, start + 1);
    char *ptr = &(url[start]);
    while (ptr) {
        char *next = strchr(++ptr, '&');
        size_t len = strcspn(ptr, "=&");
        if (len > 0) {
            generate_string_probe(probe);
            param_vector_insert(params, ptr, len, probe, PROBE_STR_LEN);
            if (params->size > 1) string_buffer_concat(mod, "&", 0x1);
            string_buffer_concat(mod, ptr, len);
            string_buffer_concat(mod, "=", 0x1);
            string_buffer_concat(mod, probe, PROBE_STR_LEN);
        }
        ptr = next;
    }
    return params->size;
}

int get_reflected_params(char * const url, struct param_vector_s * const p) {
    struct string_buffer_s mod;
    struct string_buffer_s body;
    struct param_vector_s params;
    string_buffer_init(&mod);
    string_buffer_init(&body);
    param_vector_init(&params);

    size_t i;
    if (get_param_from_url(url, &params, &mod) && http_get(mod.buff, &body)) {
        for (i = 0; i < params.size; i ++) {
            if (strstr(body.buff, params.array[i].value) != NULL) {
                param_vector_insert(p, params.array[i].name,
                                    strlen(params.array[i].name),
                                    params.array[i].value,
                                    strlen(params.array[i].value));
            }
        }
    }

    string_buffer_finish(&mod);
    string_buffer_finish(&body);
    param_vector_cleanup(&params);
    return p->size;
}
