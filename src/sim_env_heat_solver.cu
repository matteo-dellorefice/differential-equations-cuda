#include "sim_env_heat.h"

__device__ float finite_diff_second_r(float *inbuf, unsigned int width, int r, int c, float dr)
{
    float num = inbuf[(c - 1) + r * width] - 2 * inbuf[c + r * width] + inbuf[(c + 1) + r * width];
    return num / (dr * dr);
}

__device__ float finite_diff_second_c(float *inbuf, unsigned int width, int r, int c, float dc)
{
    float num = inbuf[c + (r - 1) * width] - 2 * inbuf[c + r * width] + inbuf[c + (r + 1) * width];
    return num / (dc * dc);
}

__global__ void _heat_diffusion(float *inbuf, float *outbuf, 
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
    _heat_diffusion<<<grid, block>>>(in[0]->buffer, in[1]->buffer, 
        width, height, dx, dt, alpha);
}