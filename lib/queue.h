#ifndef _QUEUE_H
#define _QUEUE_H

#include <stdlib.h>
#include <pthread.h>

struct queue_item {
    void *data;
    struct queue_item *next;
};

struct queue {
    int end;
    size_t size;
    pthread_mutex_t mutex;
    pthread_cond_t cond_get;
    pthread_cond_t cond_put;
    struct queue_item *first;
    struct queue_item *last;
};

struct queue *queue_init(void);
void queue_end(struct queue *);
void *queue_pop(struct queue *);
struct queue *queue_push(struct queue *, void *);
void queue_cleanup(struct queue *, void (*cb)(void *));

#endif /* queue.h */
