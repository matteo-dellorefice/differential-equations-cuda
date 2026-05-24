#ifndef SIM_PARAMS_H
#define SIM_PARAMS_H
#include <stdio.h>

struct sim_env;

struct sim_params
{
    unsigned int width;
    unsigned int height;
    float dx;
    float dt;

    sim_params(unsigned int width, unsigned int height, float dx, float dt) :
        width(width),
        height(height),
        dx(dx),
        dt(dt)
    { }

    virtual void solver(sim_env *env) = 0;

    virtual float max_dt() = 0;
};

#endif
