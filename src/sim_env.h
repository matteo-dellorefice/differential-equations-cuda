#ifndef SIM_ENV_H
#define SIM_ENV_H

#include "device_buffer.h"
#include "cuda_gl_resource.h"

#include <vector>
#include <memory> // std::unique_ptr

struct sim_env
{
    unsigned int width;
    unsigned int height;
    float dx;
    float dt;

    unsigned int step;
    std::vector<std::unique_ptr<device_buffer<float>>> in;
    device_buffer<unsigned int> out;
    cuda_gl_resource res;

    sim_env(unsigned int width, unsigned int height, float dx, float dt);
    
    void run(unsigned int times);

    virtual void render() = 0;

    virtual void solver() = 0;

    virtual void swap() = 0;

    virtual float max_dt() = 0;
};

#endif