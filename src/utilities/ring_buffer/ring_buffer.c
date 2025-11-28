#include "ring_buffer.h"
#include "common_def.h"

#include <string.h>

int ring_buffer_init(ring_buffer_t *rb, size_t capacity, size_t element_size)
{
    if (!rb || capacity == 0 || element_size == 0) return ERROR; 
    
    if (capacity > 1024) capacity = 1024; /* max cap is 1024 items */

    rb->buffer = calloc(1, capacity * element_size);
    if (!rb->buffer) return ERROR;

    rb->element_size = element_size; 
    rb->capacity = capacity; 
    rb->head = 0; 
    rb->tail = 0; 
    rb->count = 0;

    return 0;
}

int ring_buffer_free(ring_buffer_t *rb)
{
    if (!rb || !rb->buffer) return ERROR; 
    free(rb->buffer);
    rb->buffer = NULL;
    rb->count = rb->capacity = rb->element_size = 0;

    return OK;
}

int ring_buffer_push(ring_buffer_t *rb, const void *item)
{
    if (!rb || !rb->buffer || !item) return ERROR; 
    
    if (ring_buffer_is_full(rb))
    {
        fprintf(stderr, "[RING_BUF]: ERROR ring buffer is full\n");
        return ERROR;
    }

    uint8_t *dest = rb->buffer + (rb->head * rb->element_size);
    memcpy(dest, item, rb->element_size);

    rb->head = (rb->head + 1) % rb->capacity; 
    rb->count++;

    return OK;
}

int ring_buffer_pop(ring_buffer_t *rb, void *out_item)
{
    if (!rb || !rb->buffer || !out_item) return ERROR;

    if (ring_buffer_is_empty(rb))
    {
        fprintf(stderr, "[RING_BUF]: ring buffer is empty\n");
        return ERROR;
    }

    const uint8_t *src = rb->buffer + (rb->tail * rb->element_size);
    memcpy(out_item, src, rb->element_size);

    rb->tail = (rb->tail + 1) % rb->capacity;
    rb->count--;

    return OK;
}

int ring_buffer_is_empty(ring_buffer_t *rb)
{
    if (!rb) return ERROR; 
    return rb->count == 0;
}

int ring_buffer_is_full(ring_buffer_t *rb)
{
    if (!rb) return ERROR; 
    return rb->count == rb->capacity;
}