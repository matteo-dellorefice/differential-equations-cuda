#include "sim_env_wave.h"
#include "finite_differences.h"

__global__ void wave_kernel(float *prev, float *curr, float *next, 
    unsigned int width, unsigned int height,
    float dx, float dt, float _c)
{
    int r = threadIdx.y + blockIdx.y * blockDim.y;
    int c = threadIdx.x + blockIdx.x * blockDim.x;

    if (r <= 0 || r >= height - 1 || c <= 0 || c >= width - 1) return;
    
    float laplacian = finite_diff_second_r(curr, width, r, c, dx) + 
        finite_diff_second_c(curr, width, r, c, dx);
    next[c + r * width] = 2 * curr[c + r * width] - prev[c + r * width] + _c * _c * dt * dt * laplacian;
}

void sim_env_wave::solver()
{
    dim3 block(32, 32);
    dim3 grid((width + block.x - 1) / block.x, (height + block.y - 1) / block.y);
    wave_kernel<<<grid, block>>>(in[0]->buffer, in[1]->buffer, in[2]->buffer,
        width, height, dx, dt, c);
}