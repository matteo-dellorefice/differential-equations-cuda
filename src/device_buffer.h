#ifndef DEVICE_BUFFER_2D
#define DEVICE_BUFFER_2D

#include <cuda_runtime.h>

template<typename T>
struct device_buffer
{
    unsigned int size;
    T* buffer;

    device_buffer(unsigned int size) : 
        size(size)
    {
        cudaMalloc((void**) &this->buffer, size * sizeof(T));
    }

    ~device_buffer()
    {
        cudaFree(this->buffer);
    }
};

#endif