#include <cuda.h>
#include <cuda_runtime.h>
#include "heat_diffusion.h"

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

__global__ void _heat_diffusion(float *inbuf, float *outbuf, unsigned int width, unsigned int height) 
{
    int r = threadIdx.y + blockIdx.y * blockDim.y;
    int c = threadIdx.x + blockIdx.x * blockDim.x;

    if (r <= 0 || r >= height - 1 || c <= 0 || c >= width - 1) return;

    float alpha = 0.5;
    float dr = 0.1;
    float dc = 0.1;
    float dr2 = dr * dr;
    float dc2 = dc * dc;
    float dt = (dc2 * dr2 / (2. * alpha * (dc2 + dr2))) / 2.; 
    float laplacian = finite_diff_second_r(inbuf, width, r, c, dr) + 
        finite_diff_second_c(inbuf, width, r, c, dc);
    outbuf[c + r * width] = inbuf[c + r * width] + alpha * dt * laplacian;
}

void heat_diffusion(sim_env *env)
{
    dim3 block(32, 32);
    dim3 grid((env->width + block.x - 1) / block.x, (env->height + block.y - 1) / block.y);
    _heat_diffusion<<<grid, block>>>(env->in1.buffer, env->in2.buffer, env->width, env->height);
}