#ifndef SIM_ENV_H
#define SIM_ENV_H

#include "device_buffer.h"
#include "cuda_gl_resource.h"

struct sim_env
{
    unsigned int step;
    unsigned int width;
    unsigned int height;

    device_buffer<float> in1;
    device_buffer<float> in2;
    device_buffer<unsigned int> out;
    cuda_gl_resource res;

    sim_env(unsigned int width, unsigned int height);
    
    void run(unsigned int times);
    void swap();
    void render();

    GLuint get_texture_id();
};

#endif