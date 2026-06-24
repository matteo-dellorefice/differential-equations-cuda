
#include "sim_env_wave.h"
#include "style_transform.h"

sim_env_wave::sim_env_wave(unsigned int width, unsigned int height, float dx, float dt, float c) :
    sim_env(width, height, dx, dt),
    c(c)
{
    in.push_back(device_buffer<float>(width * height));
    in.push_back(device_buffer<float>(width * height));
    in.push_back(device_buffer<float>(width * height));
    // Courant-Friedrichs-Lewy condition: c <= dx / dt
    this->dt = std::min(dt, max_dt());
}

void sim_env_wave::render()
{
    transform_1channel(&(in[1]), &out, width, height);
    res.transfer(&out);
}

void sim_env_wave::swap()
{
    in[0].swap(in[1]);
    in[1].swap(in[2]);
}

float sim_env_wave::max_dt()
{
    return (dx * dx) / (4 * c);
}