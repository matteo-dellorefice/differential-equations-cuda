
#include "buffer_printer.h"

#include <random>

__global__ void print_gaussian_kernel(float *buf, unsigned int width, unsigned int height, float ampl, float sigmax, float sigmay, unsigned int x0, unsigned int y0)
{
    int r = threadIdx.y + blockIdx.y * blockDim.y;
    int c = threadIdx.x + blockIdx.x * blockDim.x;

    if (r <= 0 || r >= height - 1 || c <= 0 || c >= width - 1)
        return;
    
    float x_comp = powf(c - x0, 2) / (2 * sigmax * sigmax);
    float y_comp = powf(r - y0, 2) / (2 * sigmay * sigmay);
    float value = ampl * expf(-1. * (x_comp + y_comp));
    float curr = buf[c + r * width];

    buf[c + r * width] = fmaxf(curr, value);
}

void print_gaussian(const device_buffer<float>& buf, unsigned int width, unsigned int height, float ampl, float sigmax, float sigmay, unsigned int x0, unsigned int y0)
{
    dim3 blk(32, 16);
    dim3 grid((width + blk.x - 1) / blk.x, (height + blk.y - 1) / blk.y);

    print_gaussian_kernel<<<grid, blk>>>(&buf, width, height, ampl, sigmax, sigmay, x0, y0);
}

void print_random_gaussian(const device_buffer<float>& buf1, const device_buffer<float>& buf2, unsigned int width, unsigned int height)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution dist(0., 1.);

    unsigned int x0 = (unsigned int) floor(dist(gen) * width);
    unsigned int y0 = (unsigned int) floor(dist(gen) * height);

    float ampl = dist(gen) * 5.;
    float sigma = dist(gen) * 10.;

    print_gaussian(buf1, width, height, ampl, sigma, sigma, x0, y0);
    print_gaussian(buf2, width, height, ampl, sigma, sigma, x0, y0);
}