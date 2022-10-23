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

#include "http.h"

size_t write_callback(char *ptr, size_t size, size_t nmemb, void *userdata) {
    size_t length = size * nmemb; // size is always equal to 1
    struct string_buffer_s *sb = (struct string_buffer_s *) userdata;
    string_buffer_concat(sb, ptr, length);
    return length;
}

int http_get(char * const url, struct string_buffer_s * const result) {
    CURL *ceh = curl_easy_init();
    if (!ceh) return 0;
    curl_easy_setopt(ceh, CURLOPT_URL, url);
    curl_easy_setopt(ceh, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(ceh, CURLOPT_WRITEDATA, result);
    curl_easy_perform(ceh);
    curl_easy_cleanup(ceh);
    return 1;
}
