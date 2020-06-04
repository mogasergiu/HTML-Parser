/*  */
#include "lib.h"

#ifndef QUEUE
#define QUEUE

#define HEAD(que) (((Queue*)que)->head)
#define TAIL(que) (((Queue*)que)->tail)
#define SIZE_QUE(que) (((Queue*)que)->size)
#define EMPTY_QUE(que) (HEAD(que) == NULL)

typedef struct sll {
    struct sll *next;
    void *info;
} SimpleList;

typedef struct {
    size_t size;
    SimpleList *head, *tail;
} Queue;

void* InitQ(size_t size);

int PushQ(void *que, TArb child, TArb parent, int valid);

void PopQ(void *que);

void DelQ(void **que);

#endif
