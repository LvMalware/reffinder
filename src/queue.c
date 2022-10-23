#include "queue.h"

struct queue *queue_init(void) {
    struct queue *q = malloc(1 * sizeof(*q));
    *q = (struct queue) {
        0,
        0,
        PTHREAD_MUTEX_INITIALIZER,
        PTHREAD_COND_INITIALIZER,
        PTHREAD_COND_INITIALIZER,
        NULL,
        NULL
    };
    return q;
}

void queue_end(struct queue *q) {
    if (!q) return;
    pthread_mutex_lock(&q->mutex);
    q->end = 1;
    pthread_cond_broadcast(&q->cond_put);
    pthread_mutex_unlock(&q->mutex);
}

void *queue_pop(struct queue *q) {
    if (!q) return NULL;

    pthread_mutex_lock(&q->mutex);
    while (q->size == 0) {
        if (q->end) {
            pthread_mutex_unlock(&q->mutex);
            return NULL;
        }
        pthread_cond_wait(&q->cond_put, &q->mutex);
    }
        
    struct queue_item *first = q->first;
    q->first = first->next;
    if (q->first == NULL) q->last = NULL;
    q->size --;

    pthread_cond_signal(&q->cond_get);
    pthread_mutex_unlock(&q->mutex);

    void *data = first->data;
    free(first);
    return data;
}

void queue_cleanup(struct queue *q, void (*cb)(void *)) {
    if (!q) return;
    struct queue_item *next, *item = q->first;
    while (item) {
        next = item->next;
        if (cb) cb(item->data);
        free(item);
        item = next;
    }
    free(q);
}

struct queue *queue_push(struct queue *q, void *data) {
    if (!q) q = queue_init();
    struct queue_item *item = malloc(1 * sizeof(*item));
    item->data = data;
    item->next = NULL;
    pthread_mutex_lock(&q->mutex);
    while (q->size > QUEUE_MAX_PENDING) pthread_cond_wait(&q->cond_get, &q->mutex);
    if (q->last == NULL) {
        q->first = item;
    } else {
        q->last->next = item;
    }
    q->last = item;
    q->size ++;
    pthread_cond_broadcast(&q->cond_put);
    pthread_mutex_unlock(&q->mutex);
    return q;
}
