#include <stdio.h>
#include <unistd.h>
#include <stdarg.h>
#include <pthread.h>

#include "queue.h"
#include "vector.h"

#define MAX_THREADS 10000

void fatal_error(const char *format, ...) {
    va_list ap;
    va_start(ap, format);
    vfprintf(stderr, format, ap);
    exit(-1);
}

void help_message(char *prog) {
    printf(
            "%s - description\n\n"
            "Usage: %s [option(s)]\n"
            "Options:\n"
            "   -h          Show this help message and exit.\n"
            "   -i          Input file with URLs\n"
            "   -o          Output file to log results\n"
            "   -t          Number of concurrent threads\n",
            prog, prog
    );
    exit(0);
}

void *worker_func(void *queue) {
    void *data;
    pthread_t tid = pthread_self();
    while ((data = queue_pop(queue)) != NULL) {
        printf("[Thread %lu] %s\n", tid, (char *) data);
        free(data);
    }
    return data;
}

int main(int argc, char *argv[]) {
    int opt;
    unsigned nthreads = 10;
    FILE * urls_stdin = stdin;
    FILE * log_stdout = stdout;
    while ((opt = getopt(argc, argv, "i:o:t:h")) != -1) {
        switch (opt) {
            case 'i':
                urls_stdin = fopen(optarg, "r");
                if (!urls_stdin)
                    fatal_error("Can't open %s for reading\n", optarg);
                break;
            case 'o':
                log_stdout = fopen(optarg, "w");
                if (!log_stdout)
                    fatal_error("Can't open %s for writing\n", optarg);
                break;
            case 't':
                nthreads = strtoul(optarg, NULL, 10);
                if (nthreads < 1 || nthreads > MAX_THREADS)
                    fatal_error("Invalid value to -t: '%s'\n", optarg);
                break;
            case 'h':
                help_message(argv[0]);
                break;
            default:
                break;
        }
    }

    pthread_t *threads = calloc(nthreads, sizeof(*threads));

    struct queue *queue = queue_init();

    size_t i;
    for (i = 0; i < nthreads; i ++) {
        pthread_create(&threads[i], NULL, worker_func, queue);
    }

    while (!feof(urls_stdin)) {
        char *line = NULL;
        size_t size = 0;
        getline(&line, &size, urls_stdin);
        if (size > 1) {
            line[strcspn(line, "\n")] = '\0';
            queue_push(queue, line);
        } else {
            free(line);
        }
    }

    queue_end(queue);

    for (i = 0; i < nthreads; i ++) {
        pthread_join(threads[i], NULL);
    }

    free(threads);

    return 0;
}
