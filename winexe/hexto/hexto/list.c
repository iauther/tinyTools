
#include "list.h"


static node_t *node_init(buffer_t *b)
{
    node_t *n;
    
    n = (node_t*)malloc(sizeof(node_t));
    if(!n) {
        return NULL;
    }
    n->data = (U8*)malloc(b->dlen);
    if(!n->data) {
        free(n);
        return NULL;
    }
    memcpy(n->data, b->buf, b->dlen);
    n->data_len = b->dlen;
	n->next = NULL;
    
    return n;
}

static int node_free(node_t **n)
{
	if(!n) {
		return -1;
	}

	if (*n) free((*n)->data);
    free(*n);
    
    return 0;
}


int list_init(list_t *l)
{
    if(!l) {
        return -1;
    }
    
    l->head = l->tail = l->node = NULL;
    
    return 0;

}


int list_add(list_t *l, buffer_t *b)
{
    node_t *n;
    
    if(!l || !b || !b->buf || !b->dlen) {
        return -1;
    }
    
    n = node_init(b);
    if(!n) {
        return -1;
    }

	if (l->tail) {
		l->tail->next = n;
		l->tail = n;
	}
	else {
		l->head = l->node = l->tail = n;
	}
    
    return 0;
}


int list_free(list_t *l)
{
	node_t *n,*tmp;
    if(!l) {
        return -1;
    }
    
	n = l->node;
    while(n) {
		tmp = n->next;
		node_free(&n);
		n = tmp;
    }
    
    return 0;
}

