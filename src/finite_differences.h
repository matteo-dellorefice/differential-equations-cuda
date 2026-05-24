#ifndef FINITE_DIFFERENCES_H
#define FINITE_DIFFERENCES_H

__device__ float finite_diff_second_r(float *inbuf, unsigned int width, int r, int c, float dr);
__device__ float finite_diff_second_c(float *inbuf, unsigned int width, int r, int c, float dc);

#endif