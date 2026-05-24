#ifndef SIM_ENV_WAVE_H
#define SIM_ENV_WAVE_H

#include "sim_env.h"

struct sim_env_wave : virtual public sim_env
{
    float c; // wave speed [m]

    sim_env_wave(unsigned int width, unsigned int height, float dx, float dt, float alpha);

    void render() override;
    void solver() override;
    void swap() override;
    float max_dt() override;
};

#endif