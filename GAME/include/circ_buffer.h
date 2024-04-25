#ifndef _LINUX_CIRC_BUFFER_H
#define _LINUX_CIRC_BUFFER_H

struct circ_buffer{
    char * const buffer;
    const int maxlen;
    int head;
    int tail;
};

#define INIT_CIRC_BUFFER(b, len) \
    char * tmp[len];             \
    circ_buffer b = {            \
        .buffer = tmp,           \
        .head = 0,               \
        .tail = 0,               \
        .maxlen = len            \
    }

/* 
    Function: Pushes [data] in buffer [b]
    Returns:
        0: [data] was pushed succesfully.
       -1: [b] is full.
*/
int push_circ_buffer(struct circ_buffer* b, char data);

/* 
    Function: Pops an element and returns it in [data]
    Returns:
        0: The element in [data] was popped succesfully.
       -1: [b] is empty.
*/
int pop_circ_buffer(struct circ_buffer* b, char *data);

#endif