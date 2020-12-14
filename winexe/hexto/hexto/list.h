#ifndef __LIST_H__
#define __LIST_H__


#include "types.h"

typedef struct _buffer_t{
    U8              *buf;
    int             blen;
    int             dlen;
}buffer_t;

typedef struct _node_t{
    U8              *data;
    int             data_len;
    struct _node_t  *next;
}node_t;


typedef struct _list_t {
    node_t           *node;
    
    node_t          *head;
    node_t          *tail;
}list_t;


int list_init(list_t *l);


int list_add(list_t *l, buffer_t *b);


int list_free(list_t *l);


#endif