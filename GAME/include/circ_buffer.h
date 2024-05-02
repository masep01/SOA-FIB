#ifndef _LINUX_CIRC_BUFFER_H
#define _LINUX_CIRC_BUFFER_H

#define MAXLEN_BUFFER 1024

typedef struct circ_buffer{
    char buffer[MAXLEN_BUFFER];
    int head;
    int tail;
} circ_buffer;

/* 
    Function: Initializes buffer [b]
    Returns:
        -
*/
void init_circ_buffer(struct circ_buffer *b);

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