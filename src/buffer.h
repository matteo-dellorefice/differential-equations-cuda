
#pragma once

#include <cuda_runtime.h>
#include <vector>
#include <ostream>

#include <utility>
#include <algorithm>
#include <memory>
#include <iostream>


template<typename T>
class host_buffer;

template<typename T>
class device_buffer
{
    struct cuda_deleter
    {
        void operator()(void *p) const 
        {
            if (p != nullptr) cudaFree(p);
        }
    };

    size_t size_;
    std::unique_ptr<T, cuda_deleter> buffer_;

    public:
    // Default constructor
    device_buffer(size_t size) : size_(size) {
        T *buf;
        cudaMalloc(&buf, size * sizeof(T));
        buffer_ = std::unique_ptr<T, cuda_deleter>(buf);
    }

    // Copy constructor
    device_buffer(const device_buffer& other) :
        device_buffer(other.size_)
    {
        cudaMemcpy(buffer_.get(), other.buffer_.get(), size_ * sizeof(T), cudaMemcpyDeviceToDevice);
    }

    device_buffer(const host_buffer<T>& other) :
        device_buffer(other.size())
    {
        cudaMemcpy(buffer_.get(), &other, size_ * sizeof(T), cudaMemcpyHostToDevice);
    }

    void swap(device_buffer& other) noexcept 
    {
        std::swap(buffer_, other.buffer_);
        std::swap(size_, other.size_);
    }

    device_buffer& operator=(device_buffer other) noexcept 
    {
        swap(other);
        return *this;
    }

    size_t size() const { 
        return size_;
    }

    T* operator&() const {
        return buffer_.get();
    }
};

template<typename T>
class host_buffer
{
private:
    size_t size_;
    std::vector<T> buffer_;

public:
    host_buffer(size_t size) :
        size_(size),
        buffer_(size)
    { }

    host_buffer(const device_buffer<T>& other) :
        host_buffer(other.size())
    {
        cudaMemcpy(&buffer_[0], &other, size_ * sizeof(T), cudaMemcpyDeviceToHost);
    }

    host_buffer& operator=(const device_buffer<T>& other) 
    {
        cudaMemcpy(&buffer_[0], &other, size_ * sizeof(T), cudaMemcpyDeviceToHost);

        return *this;
    } 

    size_t size() const {
        return size_;
    }

    T* operator&() {
        return &buffer_[0];
    }

    friend std::ostream& operator<<(std::ostream &os, const host_buffer<T>& b)
    {
        for (int i = 0; i < b.size_; i++) {
            os << b.buffer_[i] << " ";
        }

        return os;
    }
};

