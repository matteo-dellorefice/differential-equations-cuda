#include "hdbuf.cuh"

#include <stdlib.h>

// TODO: error checking

hdbuf_t hdbuf_create(unsigned int size) 
{
    float *host = (float*) calloc(size, sizeof(float));
    float *device;

    cudaMalloc((void**) &device, size * sizeof(float));

    return (hdbuf_t) {
        host,
        device,
        size
    };
}

void hdbuf_swap(hdbuf_t *b1, hdbuf_t *b2)
{
    hdbuf_t tmp = *b1;

    *b1 = *b2;
    *b2 = tmp;
}

void hdbuf_free(hdbuf_t buf)
{
    free(buf.host);
    cudaFree(buf.device);
}

void hdbuf_memcpy(hdbuf_t buf, cudaMemcpyKind kind)
{
    void *dst, *src;
    
    switch (kind) {
    case cudaMemcpyHostToDevice:
        dst = (void*) buf.device;
        src = (void*) buf.host;
        break;
    case cudaMemcpyDeviceToHost:
    default:
        dst = (void*) buf.host;
        src = (void*) buf.device;
    }

    cudaMemcpy(dst, src, buf.size * sizeof(buf.host[0]), kind);
}