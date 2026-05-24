#include "sim_env_heat.h"
#include "finite_differences.h"

__global__ void heat_kernel(float *inbuf, float *outbuf, 
    unsigned int width, unsigned int height,
    float dx, float dt, float alpha)
{
    int r = threadIdx.y + blockIdx.y * blockDim.y;
    int c = threadIdx.x + blockIdx.x * blockDim.x;

    if (r <= 0 || r >= height - 1 || c <= 0 || c >= width - 1) return;
    
    float laplacian = finite_diff_second_r(inbuf, width, r, c, dx) + 
        finite_diff_second_c(inbuf, width, r, c, dx);
    outbuf[c + r * width] = inbuf[c + r * width] + alpha * dt * laplacian;
}

void sim_env_heat::solver()
{
    dim3 block(32, 32);
    dim3 grid((width + block.x - 1) / block.x, (height + block.y - 1) / block.y);
    heat_kernel<<<grid, block>>>(in[0]->buffer, in[1]->buffer, 
        width, height, dx, dt, alpha);
}