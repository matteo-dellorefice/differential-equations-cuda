
#include <stdio.h>
#include <math.h>
#include <cuda_runtime.h>

#include "hdbuf.cuh"
#include "heat.cuh"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "std_image_write.h"

#include "imgui.h"

#define WIDTH 800
#define HEIGHT 600

void hdbuf_to_imgbuf(hdbuf_t buf, char *imgbuf) 
{
    for (int r = 0; r < HEIGHT; r++) {
        for (int c = 0; c < WIDTH; c++) {
            imgbuf[c + r * WIDTH] = 255 * buf.host[c + r * WIDTH];
        }
    }
}

int _main() 
{
    // ImGui::CreateContext();
    ImGui::ShowDemoWindow();
    stbi_write_png_compression_level = 0;

    hdbuf_t in = hdbuf_create(WIDTH * HEIGHT);
    hdbuf_t out = hdbuf_create(WIDTH * HEIGHT);

    char *imgbuf = (char *) calloc(WIDTH * HEIGHT, sizeof(char));

    int col_center = WIDTH / 2;
    int row_center = HEIGHT / 2;
    float radius = HEIGHT / 2;
    for (int r = 0; r < HEIGHT; r++) {
        for (int c = 0; c < WIDTH; c++) {
            float dist = sqrt((r - row_center) * (r - row_center) + (c - col_center) * (c - col_center));
            if (dist < radius) {
                in.host[c + r * WIDTH] = 1.;
            }
        }
    }

    hdbuf_memcpy(in, cudaMemcpyHostToDevice);

    hdbuf_to_imgbuf(in, imgbuf);
    stbi_write_png("input.png", WIDTH, HEIGHT, 1, imgbuf, WIDTH);

    for (int i = 0; i < 100; i++) {
        heat(in, out, WIDTH, HEIGHT);
        hdbuf_swap(&in, &out);
    }
    
    hdbuf_memcpy(out, cudaMemcpyDeviceToHost);
    
    hdbuf_to_imgbuf(out, imgbuf);
    stbi_write_png("output.png", WIDTH, HEIGHT, 1, imgbuf, WIDTH);

    hdbuf_free(in);
    hdbuf_free(out);

    cudaError_t err = cudaGetLastError();
    const char *errStr = cudaGetErrorString(err);

    printf("CUDA last error %d: %s\n", err, errStr);

    return 0;
}