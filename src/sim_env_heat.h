#ifndef SIM_ENV_HEAT_H
#define SIM_ENV_HEAT_H

#include "sim_env.h"

struct sim_env_heat : virtual public sim_env
{
    float alpha; // thermal diffusivity [m*m/s]

    sim_env_heat(unsigned int width, unsigned int height, float dx, float dt, float alpha);

    void render() override;
    void solver() override;
    void swap() override;
    float max_dt() override;
};

#endif