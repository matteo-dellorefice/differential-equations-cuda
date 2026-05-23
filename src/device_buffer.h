#ifndef DEVICE_BUFFER_2D
#define DEVICE_BUFFER_2D

#include <cuda_runtime.h>

template<typename T>
struct device_buffer
{
    T* buffer;

    device_buffer(unsigned int size)
    {
        cudaMalloc((void**) &this->buffer, size * sizeof(T));
    }

    ~device_buffer()
    {
        cudaFree(this->buffer);
    }
};

#endif