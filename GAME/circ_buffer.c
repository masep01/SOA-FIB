#include <circ_buffer.h>

int push_circ_buffer(struct circ_buffer* b, char data){

    /* Next points where head will point after this write. */
    int next = b->head + 1;

    /* Next has reached maxlen, we restart HEAD */
    if(b->maxlen <= next) next = 0;

    /* Buffer is full */
    if(next == b->tail) return -1;

    b->buffer[b->head] = data;          /* Push element      */
    b->head = next;                     /* Move head pointer */
    return 0;
}

int pop_circ_buffer(struct circ_buffer* b, char *data){
   
    /* Tail and Head are equal, so we don't have any data. Buffer is empty */
    if(b->head == b->tail) return -1;

    /* Next points where head will point after this write. */
    int next = b->tail + 1;

    /* Next has reached maxlen, we restart HEAD */
    if(b->maxlen <= next) next = 0;

    *data = b->buffer[b->tail];         /* Read data from tail pointer */
    b->tail = next;                     /* Move tail                   */
    return 0;
}