
#include "sim_env_heat.h"
#include "style_transform.h"

#include <algorithm> // std::min

sim_env_heat::sim_env_heat(unsigned int width, unsigned int height, float dx, float dt, float alpha) :
    sim_env(width, height, dx, dt),
    alpha(alpha)
{
    in.push_back(device_buffer<float>(width * height));
    in.push_back(device_buffer<float>(width * height));
    this->dt = std::min(dt, max_dt()); // TODO this can go in the base class?
}

void sim_env_heat::render()
{
    transform_1channel(&in[0], &out, width, height);
    res.transfer(&out);
}

void sim_env_heat::swap()
{
    in[0].swap(in[1]);
}

float sim_env_heat::max_dt()
{
    return (dx * dx) / (4 * alpha); // TODO check CFL condition
}