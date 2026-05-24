#ifndef HEAT_DIFFUSION_H
#define HEAT_DIFFUSION_H

#include "sim_env.h"

// void heat_diffusion(sim_env *env);

struct heat_diffusion_params: virtual public sim_params
{
    float alpha;

    heat_diffusion_params(unsigned int width, unsigned int height, 
        float dx, float alpha);

    void solver(sim_env *env) override;

    float max_dt() override;
};

#endif