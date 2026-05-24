#include "sim_env.h"
#include "sim_params.h"
#include "style_transform.h"
#include <stdio.h>
sim_env::sim_env(sim_params *params) :
    params(params),
    step(0),
    in1(params->width * params->height),
    in2(params->width * params->height),
    out(params->width * params->height),
    res(params->width, params->height)
{ }

void sim_env::run(unsigned int times)
{
    for (int i = 0; i < times; i++) {
        params->solver(this);
        swap();
        step++;
    }
    // print
}

void sim_env::swap()
{
    float *tmp = in1.buffer;
    in1.buffer = in2.buffer;
    in2.buffer = tmp;
}

void sim_env::render() 
{
    transform_1channel(in1.buffer, out.buffer, params->width, params->height);
    res.transfer(out.buffer);
}