#ifndef HDBUF_H
#define HDBUF_H

#include <cuda_runtime.h>

typedef struct {
    float *host;
    float *device;
    unsigned int size;
} hdbuf_t;

hdbuf_t hdbuf_create(unsigned int size);

void hdbuf_swap(hdbuf_t *b1, hdbuf_t *b2);

void hdbuf_free(hdbuf_t buf);

void hdbuf_memcpy(hdbuf_t buf, cudaMemcpyKind kind);

#endif