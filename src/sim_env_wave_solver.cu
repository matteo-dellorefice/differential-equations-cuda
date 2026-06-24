#include "sim_env_wave.h"
#include "finite_differences.h"

#define SHM_IDX(x, y, stride) ((x) + 1 + ((y) + 1) * stride)
#define GLM_IDX(x, y, stride) ((x) + (y) * (stride))

__global__ void wave_kernel(float *prev, float *curr, float *next, 
    unsigned int width, unsigned int height,
    float dx, float dt, float _c)
{
    extern __shared__ float shm[];

    int r = threadIdx.y + blockIdx.y * blockDim.y;
    int c = threadIdx.x + blockIdx.x * blockDim.x;

    int tix = threadIdx.x;
    int tiy = threadIdx.y;
    int str = blockDim.x + 2;

    if (r <= 0 || r >= height - 1 || c <= 0 || c >= width - 1) {
        shm[SHM_IDX(tix, tiy, str)] = 0.;
        return;
    }

    shm[SHM_IDX(tix, tiy, str)] = curr[GLM_IDX(c, r, width)];

    if (tix == 0) {
        shm[SHM_IDX(tix - 1, tiy, str)] = curr[GLM_IDX(c - 1, r, width)];
    }
    if (tix == blockDim.x - 1) {
        shm[SHM_IDX(tix + 1, tiy, str)] = curr[GLM_IDX(c + 1, r, width)];
    }
    if (tiy == 0) {
        shm[SHM_IDX(tix, tiy - 1, str)] = curr[GLM_IDX(c, r - 1, width)];
    }
    if (tiy == blockDim.y - 1) {
        shm[SHM_IDX(tix, tiy + 1, str)] = curr[GLM_IDX(c, r + 1, width)];
    }

    __syncthreads();

    float second_deriv_x = 
        (shm[SHM_IDX(tix - 1, tiy, str)] - 2 * shm[SHM_IDX(tix, tiy, str)] + shm[SHM_IDX(tix + 1, tiy,str)]) / (dx * dx);
    float second_deriv_y = 
        (shm[SHM_IDX(tix, tiy - 1, str)] - 2 * shm[SHM_IDX(tix, tiy, str)] + shm[SHM_IDX(tix, tiy + 1, str)]) / (dx * dx);

    next[c + r * width] = 2 * shm[SHM_IDX(tix, tiy, str)] - prev[c + r * width] + _c * _c * dt * dt * (second_deriv_x + second_deriv_y);

    next[c + r * width] = 2 * shm[SHM_IDX(tix, tiy, str)] - prev[c + r * width] + _c * _c * dt * dt * (second_deriv_x + second_deriv_y);
}

void sim_env_wave::solver()
{
    dim3 block(32, 16);
    dim3 grid((width + block.x - 1) / block.x, (height + block.y - 1) / block.y);
    size_t shmem_size = (block.x + 2) * (block.y + 2) * sizeof(float);
    wave_kernel<<<grid, block, shmem_size>>>(&in[0], &in[1], &in[2], width, height, dx, dt, c);
}