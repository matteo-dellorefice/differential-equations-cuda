#include "finite_differences.h"

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
