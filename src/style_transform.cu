
/**
viridis = matplotlib.colormaps['viridis'].resampled(9)
color_list = np.array(viridis.colors) * 255.
color_list = color_list.astype(int)
*/

__device__ unsigned int viridis[] = {
     68,   1,  84,
     71,  44, 123,
     58,  82, 139,
     44, 114, 142,
     32, 144, 140,
     40, 174, 127,
     94, 201,  97,
    173, 220,  48,
    253, 231,  36
};

__global__ void _transform_1channel(float *in, unsigned int *out, unsigned int width, unsigned int height)
{
    int r = threadIdx.y + blockIdx.y * blockDim.y;
    int c = threadIdx.x + blockIdx.x * blockDim.x;

    if (r >= height || c >= width) return;

    float val = in[c + r * width] * 8.;
    unsigned int lev_lo = (unsigned int) floorf(val);
    unsigned int lev_hi = (unsigned int) fminf(lev_lo + 1., 8.);
    float frac_lo = val - truncf(val);
    float frac_hi = 1. - frac_lo;

    unsigned int _r = ((unsigned int) (frac_hi * viridis[3 * lev_lo] + frac_lo * viridis[3 * lev_hi]));
    unsigned int _g = ((unsigned int) (frac_hi * viridis[3 * lev_lo + 1] + frac_lo * viridis[3 * lev_hi + 1])) << 8;
    unsigned int _b = ((unsigned int) (frac_hi * viridis[3 * lev_lo + 2] + frac_lo * viridis[3 * lev_hi + 2])) << 16;

    unsigned int color = 0xFF000000 | _b | _g | _r;

    // unsigned int color = 0;
    // color |= 0xFF000000; // alpha
    // color |= 0x00FF0000; // blue
    // color |= 0x0000FF00; // green
    // color |= 0x000000FF; // red

    out[c + r * width] = color;
}

void transform_1channel(float *in, unsigned int *out, unsigned int width, unsigned int height)
{
    dim3 block(32, 32);
    dim3 grid((width + block.x - 1) / block.x, (height + block.y - 1) / block.y);
    _transform_1channel<<<grid, block>>>(in, out, width, height);
}