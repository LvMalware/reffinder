#ifndef _UTILS_H
#define _UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "http.h"
#include "buffer.h"
#include "vector.h"
#include "memdebug.h"

#define PROBE_STR_LEN 0xa

int get_reflected_params(char * const, struct param_vector_s * const);
int get_param_from_url(char * const, struct param_vector_s * const,
                       struct string_buffer_s * const);

#endif /* utils.h */
