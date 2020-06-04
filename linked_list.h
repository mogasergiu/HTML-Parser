/*  */
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#ifndef LINKED_LIST
#define LINKED_LIST

typedef struct sll {
    struct sll *next;
    void *info;
} SimpleList;

#endif
