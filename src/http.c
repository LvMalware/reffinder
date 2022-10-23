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
