#include "sim_env.h"
#include "style_transform.h"
#include "heat_diffusion.h"

#include <stdio.h>
#include <thread>
#include <GLFW/glfw3.h> 

sim_env::sim_env(unsigned int width, unsigned int height) :
    step(0), 
    width(width), 
    height(height),
    in1(width * height),
    in2(width * height),
    out(width * height),
    res(width, height)
{ }

void sim_env::run(unsigned int times)
{
    for (int i = 0; i < times; i++) {
        heat_diffusion(this);
        swap();
        step++;
        // glfwPostEmptyEvent();
    }
    // std::thread t([=]() {
        
    // });

    // t.detach();
}

void sim_env::swap()
{
    float *tmp = in1.buffer;
    in1.buffer = in2.buffer;
    in2.buffer = tmp;
}

void sim_env::render() 
{
    transform_1channel(in1.buffer, out.buffer, width, height);
    res.transfer(out.buffer);
}