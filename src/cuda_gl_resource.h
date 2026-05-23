
#include <GLFW/glfw3.h> 
#include <cuda_gl_interop.h>

struct cuda_gl_resource 
{
    GLuint texture_id;
    struct cudaGraphicsResource* cuda_resource;
    unsigned int width;
    unsigned int height;

    cuda_gl_resource(unsigned int width, unsigned int height);
    ~cuda_gl_resource();

    void transfer(unsigned int *device_buffer);

};