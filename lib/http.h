#ifndef _HTTP_H
#define _HTTP_H

#include <curl/curl.h>

#include "buffer.h"

int http_get(char * const, struct string_buffer_s * const);

#endif /* http.h */
