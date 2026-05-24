#ifndef SIM_ENV_H
#define SIM_ENV_H

#include "device_buffer.h"
#include "cuda_gl_resource.h"
#include "sim_params.h"

struct sim_env
{
    sim_params *params;

    unsigned int step;
    device_buffer<float> in1;
    device_buffer<float> in2;
    device_buffer<unsigned int> out;
    cuda_gl_resource res;

    sim_env(sim_params *params);
    
    void run(unsigned int times);

    void swap();

    void render();
};

#endif