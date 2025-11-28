#pragma once

#include <stdio.h>
#include <stddef.h>
#include <stdint.h>

typedef struct
{
    uint8_t *buffer;            /* raw byte buffer */
    size_t element_size;        /* size of each element */
    size_t capacity;            /* number of elements the buffer holds */
    size_t head;                /* write index */
    size_t tail;                /* read index */
    size_t count;               /* number of elements currenty stored */
} ring_buffer_t;

int ring_buffer_init(ring_buffer_t *rb, size_t capacity, size_t element_size);

int ring_buffer_free(ring_buffer_t *rb);

int ring_buffer_push(ring_buffer_t *rb, const void *item);

int ring_buffer_pop(ring_buffer_t *rb, void *out_item);

int ring_buffer_is_empty(ring_buffer_t *rb);

int ring_buffer_is_full(ring_buffer_t *rb);
