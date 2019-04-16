#ifndef CONVOLUTION_H
#define CONVOLUTION_H

#include "paramdict.h"
#include <stdlib.h>
#include <stdio.h>

#ifdef ARM_NEON
#include <arm_neon.h>
#endif

#ifdef OPENCL
#include <CL/cl.h>
#endif

typedef struct convolution{
    int num_output;
    int num_output2;
    int kernel_w;
    int kernel_h;
    int dilation_w;
    int dilation_h;
    int stride_w;
    int stride_h;
    int pad_w;
    int pad_h;
    int bias_term;

    int weight_data_size;
    int weight_data_size2;
    int w;
    int h;
    int c;

    // model
    float *weight_data;
    float *bias_data;
    float *PerData;
    float *PermuteData[2];
    float *output_temp;
}Convolution;

int load_param_conv(Convolution *conv, int layer_idx, int w, int h, int c);

int load_model_conv(FILE *binfp, Convolution *conv);

int forward_conv(const int *bottom_blob, float *top_blob, Convolution *conv);

void permute(float *src, float *dst, int type, int w, int h, int channel);

#ifdef ARM_NEON
int forward_conv_arm(const int *bottom_blob, float *top_blob, Convolution *conv);
int forward_conv_compile(const int *bottom_blob, float *top_blob, Convolution *conv);
#endif

#ifdef OPENCL
int forward_conv_cl(const int *bottom_blob, float *top_blob, Convolution *conv, cl_command_queue command_queue, cl_context context, cl_kernel kernel);
#endif

int Float2Int(float *input, int *output, int size);

#endif // LAYER_CONVOLUTION_H
