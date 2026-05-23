
__global__ void _transform_1channel(float *in, unsigned int *out, unsigned int width, unsigned int height)
{
    int r = threadIdx.y + blockIdx.y * blockDim.y;
    int c = threadIdx.x + blockIdx.x * blockDim.x;

    if (r >= height || c >= width) return;

    unsigned int v = (unsigned int) (in[c + r * width] * 255.);
    v += (v << 8) + (v << 16) + (v << 24);

    out[c + r * width] = v;
}

void transform_1channel(float *in, unsigned int *out, unsigned int width, unsigned int height)
{
    dim3 block(32, 32);
    dim3 grid((width + block.x - 1) / block.x, (height + block.y - 1) / block.y);
    _transform_1channel<<<grid, block>>>(in, out, width, height);
}