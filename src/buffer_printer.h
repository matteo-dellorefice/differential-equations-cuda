
#pragma once

#include "buffer.h"

void print_gaussian(const device_buffer<float>& buf1, const device_buffer<float>& buf2, unsigned int width, unsigned int height, float ampl, float sigmax, float sigmay, unsigned int x0, unsigned int y0);

void print_random_gaussian(const device_buffer<float>& buf1, const device_buffer<float>& buf2, unsigned int width, unsigned int height);