#include "cuda_gl_resource.h"
#include <stdlib.h>
cuda_gl_resource::cuda_gl_resource(unsigned int width, unsigned int height) 
{
    this->width = width;
    this->height = height;

    glGenTextures(1, &this->texture_id);
    glBindTexture(GL_TEXTURE_2D, this->texture_id);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, 
        GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    cudaGraphicsGLRegisterImage(&this->cuda_resource, this->texture_id, 
        GL_TEXTURE_2D, cudaGraphicsRegisterFlagsNone);
}

cuda_gl_resource::~cuda_gl_resource()
{
    cudaGraphicsUnregisterResource(this->cuda_resource);
    glDeleteTextures(1, &this->texture_id);
}

void cuda_gl_resource::transfer(unsigned int *device_buffer) 
{
    cudaArray *mapped_array;
    cudaGraphicsMapResources(1, &this->cuda_resource, 0);
    cudaGraphicsSubResourceGetMappedArray(&mapped_array, this->cuda_resource, 0, 0);
    // TODO: cudaMemcpyToArray is deprecated.
    // Use cudaMemcpy2DToArray:
    // https://developer.nvidia.com/w/compute/DevZone/docs/html/C/doc/html/group__CUDART__MEMORY_g1cc6e4eb2a5e0cd2bebbc8ebb4b6c46f.html
    // https://forums.developer.nvidia.com/t/cudamemcpytoarray-is-deprecated/71385/9
    // cudaMemcpyToArray(mapped_array, 0, 0, device_buffer, this->width * this->height * sizeof(float), cudaMemcpyDeviceToDevice);
    cudaMemcpy2DToArray(mapped_array, 0, 0, device_buffer, this->width * sizeof(unsigned int), 
        this->width * sizeof(unsigned int), this->height, cudaMemcpyDeviceToDevice);
    cudaGraphicsUnmapResources(1, &this->cuda_resource, 0); 
}