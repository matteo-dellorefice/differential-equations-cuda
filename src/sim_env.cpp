#include "sim_env.h"
#include "style_transform.h"

#include <chrono>

sim_env::sim_env(unsigned int width, unsigned int height, float dx, float dt) :
    width(width),
    height(height),
    dx(dx),
    dt(dt),
    step(0),
    in(),
    out(width * height),
    res(width, height)
{ }

void sim_env::run(unsigned int times)
{
    for (int i = 0; i < times; i++) {
        solver();
        swap();
        step++;
    }
}
