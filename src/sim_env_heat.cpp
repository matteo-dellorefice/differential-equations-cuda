
#include "sim_env_heat.h"
#include "style_transform.h"

#include <algorithm> // std::min

sim_env_heat::sim_env_heat(unsigned int width, unsigned int height, float dx, float dt, float alpha) :
    sim_env(width, height, dx, dt),
    alpha(alpha)
{
    in.push_back(std::unique_ptr<device_buffer<float>>(new device_buffer<float>(width * height)));
    in.push_back(std::unique_ptr<device_buffer<float>>(new device_buffer<float>(width * height)));
    this->dt = std::min(dt, max_dt()); // TODO this can go in the base class?
}

void sim_env_heat::render()
{
    transform_1channel(in[0]->buffer, out.buffer, width, height);
    res.transfer(out.buffer);
}

void sim_env_heat::swap()
{
    float *tmp = in[0]->buffer;
    in[0]->buffer = in[1]->buffer;
    in[1]->buffer = tmp;
}

float sim_env_heat::max_dt()
{
    return (dx * dx) / (4 * alpha);
}