#include <time.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdarg.h>
#include <pthread.h>

#include "http.h"
#include "utils.h"
#include "queue.h"
#include "buffer.h"
#include "vector.h"
#include "memdebug.h"

#define MAX_THREADS 10000

pthread_mutex_t print_mutex;

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

struct task_info {
    struct queue *queue;
    FILE *output;
};

void *worker_func(void *tinfo) {
    struct task_info *task = (struct task_info *) tinfo;
    pthread_t tid = pthread_self();
    void *data = NULL;
    struct queue *queue = task->queue;
    FILE *output = task->output;
    struct param_vector_s params;

    size_t i;

    (void)tid;
    (void)output;
    while ((data = queue_pop(queue)) != NULL) {
        param_vector_init(&params);
        //
        //
        if (get_reflected_params((char *) data, &params) > 0) {
            pthread_mutex_lock(&print_mutex);
            fprintf(output, "{\"url\":\"%s\",\"params\":", (char *) data);
            for (i = 0; i < params.size; i ++) {
                fprintf(output, "%c\"%s\"", i == 0 ? '[' : ',', params.array[i].name);
            }
            fprintf(output, "]}\n");
            pthread_mutex_unlock(&print_mutex);
        }
        //
        //
        free(data);
        param_vector_cleanup(&params);
    }
    return data;
}

int main(int argc, char *argv[]) {
    srand(time(NULL));
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

    pthread_mutex_init(&print_mutex, NULL);
    pthread_t *threads = calloc(nthreads, sizeof(*threads));

    struct task_info task = { queue_init(), log_stdout };

    size_t i;
    for (i = 0; i < nthreads; i ++) {
        pthread_create(&threads[i], NULL, worker_func, &task);
    }

    while (!feof(urls_stdin)) {
        char *line = NULL;
        size_t size = 0;
        getline(&line, &size, urls_stdin);
        if (size > 1) {
            line[strcspn(line, "\n")] = '\0';
            queue_push(task.queue, line);
        } else {
            free(line);
        }
    }

    fclose(urls_stdin);
    queue_end(task.queue);

    for (i = 0; i < nthreads; i ++) {
        pthread_join(threads[i], NULL);
    }

    free(threads);
    queue_cleanup(task.queue, NULL);
    fclose(log_stdout);

    return 0;
}
