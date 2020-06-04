/*  */
#include "queue.h"

void* InitQ(size_t size) {
    Queue *que = (Queue*) calloc(1, sizeof(Queue));
    if (que == NULL) {
        return NULL;
    }

    que->size = size;

    return (void*) que;
}

int PushQ(void *que, TArb child, TArb parent, int valid) {
    SimpleList *cell = (SimpleList*)calloc(1, sizeof(SimpleList));
    if (cell == NULL) {
        return 0;
    }

    cell->info = calloc(1, SIZE_QUE(que));
    if (cell->info == NULL) {
        return 0;
    }

    ((TArbP)cell->info)->node = child;
    ((TArbP)cell->info)->parent = parent;
    ((TArbP)cell->info)->valid = valid;

    cell->next = NULL;

    if (EMPTY_QUE(que)) {
        TAIL(que) = HEAD(que) = cell;

    } else {
        TAIL(que)->next = cell;
        TAIL(que) = cell;
    }

    return 1;
}

void PopQ(void *que) {
    if (EMPTY_QUE(que)) {
        return;
    }

    SimpleList *cell = HEAD(que);
    HEAD(que) = cell->next;
    free(cell->info);
    free(cell);

    if (HEAD(que) == NULL) {
        TAIL(que) = NULL;
    }
}

void DelQ(void **que) {
    if (*que != NULL) {
        while (!EMPTY_QUE(*que)) {
            PopQ(*que);
        }

        free(*que);
        *que = NULL;
    }
}
