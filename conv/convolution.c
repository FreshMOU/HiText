#include "convolution.h"
#include <string.h>
#include <math.h>

void permuteInt(const int *src, int *dst, int w, int h, int channel)
{
    int step = channel*h;
    int outstep = w*h;

    for (int q=0; q<channel; q++)
    {
        int* outptr = dst + q * outstep;

        for (int i = 0; i < h; i++)
        {
            for (int j = 0; j < w; j++)
            {
                const int* ptr = src + j * step;

                outptr[i*w + j] = ptr[i*channel + q];
            }
        }
    }
}

int Int2Float(const int *input, float *output, int size)
{
    for (int i=0; i<size; i++)
    {
        output[i] = (float)input[i];
    }
    return 0;
}

int Float2Int(float *input, int *output, int size)
{
    for (int i=0; i<size; i++)
    {
        output[i] = (int)input[i];
    }
    return 0;
}

// v即为填充的pad，默认为0
void copy_make_border(const int *src, int *dst, int top, int bottom, int left, int right, int c, float v, Convolution *conv)
{
    int w = conv->w + left + right;
    int h = conv->h + top + bottom;

    if (w == conv->w && h == conv->h)
    {
        memcpy(dst, src, w*h*c*sizeof(int));
        return;
    }

    int step = w * h;
    int instep = conv->w * conv->h;
    #pragma omp parallel for
    for (int i=0; i<c; i++)
    {
        const int* ptr = src + instep * i;//.data;
        int* outptr = dst + step * i;//.data;

        int y = 0;
        // fill top
        for (; y < top; y++)
        {
            int x = 0;
            for (; x < w; x++)
            {
                outptr[x] = v;
            }
            outptr += w;
        }
        // fill center
        for (; y < (top + conv->h); y++)
        {
            int x = 0;
            for (; x < left; x++)
            {
                outptr[x] = v;
            }
            if (conv->w < 12)
            {
                for (; x < (left + conv->w); x++)
                {
                    outptr[x] = ptr[x - left];
                }
            }
            else
            {
                memcpy(outptr + left, ptr, conv->w * sizeof(int));
                x += conv->w;
            }
            for (; x < w; x++)
            {
                outptr[x] = v;
            }
            ptr += conv->w;
            outptr += w;
        }
        // fill bottom
        for (; y < h; y++)
        {
            int x = 0;
            for (; x < w; x++)
            {
                outptr[x] = v;
            }
            outptr += w;
        }
    }
}

int load_param_conv(Convolution *conv, int layer_idx, int w, int h, int c)
{
    conv->num_output = getInt(layer_idx, 0, 0);
    conv->kernel_w = getInt(layer_idx, 1, 0);
    conv->kernel_h = getInt(layer_idx, 11, conv->kernel_w);
    conv->dilation_w = getInt(layer_idx, 2, 1);
    conv->dilation_h = getInt(layer_idx, 12, conv->dilation_w);
    conv->stride_w = getInt(layer_idx, 3, 1);
    conv->stride_h = getInt(layer_idx, 13, conv->stride_w);
    conv->pad_w = getInt(layer_idx, 4, 0);
    conv->pad_h = getInt(layer_idx, 14, conv->pad_w);
    conv->bias_term = getInt(layer_idx, 5, 0);
    conv->weight_data_size = getInt(layer_idx, 6, 0);
    conv->w = w;
    conv->h = h;
    conv->c = c;

    //fprintf(stderr, "Convolution: %d %d %d\n", conv->num_output, conv->kernel_w, conv->stride_w);

    return 0;
}
        

int load_model_conv(FILE *binfp, Convolution *conv)
{
    int nread = 0;
    conv->weight_data = (float*)malloc(sizeof(float) * conv->weight_data_size);

    // raw data
    nread = fread(conv->weight_data, conv->weight_data_size * sizeof(float), 1, binfp);
    if (nread != 1)
    {
        fprintf(stderr, "ModelBin read weight_data failed %d\n", nread);
        return -100;
    }

    conv->bias_data = (float*)malloc(conv->num_output * sizeof(float));
    if (conv->bias_term)
    {
        nread = fread(conv->bias_data, conv->num_output * sizeof(float), 1, binfp);
        if (nread != 1)
        {
            fprintf(stderr, "ModelBin read bias_data failed %d\n", nread);
            return -100;
        }
    }
    //fprintf(stderr, "bias: %f\n", conv->bias_data[0]);

    return 0;
}

int forward_conv(const int *bottom_blob, float *top_blob, Convolution *conv)
{
    // int *bottom_blob;
    // bottom_blob = (int*)malloc(conv->w*conv->h*conv->c*sizeof(int));
    // permuteInt(bottom_blob_in, bottom_blob, conv->w, conv->h, conv->c);

    // convolv with NxN kernel
    // value = value + bias

    //fprintf(stderr, "\nw: %d, h: %d, c:%d , kernel_w: %d, kernel_h: %d\n", bottom_blob.w, bottom_blob.h, bottom_blob.c, kernel_w, kernel_h);

    //fprintf(stderr, "Convolution input %d x %d  pad = %d %d  ksize=%d %d  stride=%d %d\n", w, h, pad_w, pad_h, kernel_w, kernel_h, stride_w, stride_h);

    const int kernel_extent_w = conv->dilation_w * (conv->kernel_w - 1) + 1;
    const int kernel_extent_h = conv->dilation_h * (conv->kernel_h - 1) + 1;

    int w = conv->w + conv->pad_w + conv->pad_w;
    int h = conv->h + conv->pad_h + conv->pad_h;
    int channel = conv->c;
    int *bottom_blob_bordered;
    bottom_blob_bordered = (int*)malloc(w*h*channel*sizeof(int));
    memset(bottom_blob_bordered, 0, w*h*channel*sizeof(int));
    copy_make_border(bottom_blob, bottom_blob_bordered, conv->pad_h, conv->pad_h, conv->pad_w, conv->pad_w, channel, 0.f, conv);


    int outw = (w - kernel_extent_w) / conv->stride_w + 1;
    int outh = (h - kernel_extent_h) / conv->stride_h + 1;
    //fprintf(stderr, "w: %d, h: %d, ker_w: %d, ker_h: %d, stride_w: %d, stride_h: %d\n", w,h,kernel_extent_w, kernel_extent_h, stride_w, stride_h);

    //top_blob提前分配好内存，就不在forward中分配了。
    // top_blob = (float*)malloc(outw * outh * num_output * sizeof(float));
    // memset(top_blob, 0, outw * outh * num_output * sizeof(float));

    const int maxk = conv->kernel_w * conv->kernel_h;

    // kernel offsets
    int* space_ofs;
    space_ofs = (int*)malloc(maxk*sizeof(int));
    memset(space_ofs, 0, maxk*sizeof(int));
    {
        int p1 = 0;
        int p2 = 0;
        int gap = w * conv->dilation_h - conv->kernel_w * conv->dilation_w;
        for (int i = 0; i < conv->kernel_h; i++)
        {
            for (int j = 0; j < conv->kernel_w; j++)
            {
                space_ofs[p1] = p2;
                p1++;
                p2 += conv->dilation_w;
            }
            p2 += gap;
        }
    }

    int step = w * h;
    int outstep = outw * outh;
    // num_output
    #pragma omp parallel for
    for (int p=0; p<conv->num_output; p++)
    {
        float* outptr = top_blob + outstep * p;

        for (int i = 0; i < outh; i++)
        {
            for (int j = 0; j < outw; j++)
            {
                float sum = 0.f;

                if (conv->bias_term)
                    sum = conv->bias_data[p];


                const float* kptr = (const float*)conv->weight_data + maxk * channel * p;

                // channels
                for (int q=0; q<channel; q++)
                {
                    const int* m = bottom_blob_bordered + step * q;
                    const int* sptr = m + i*conv->stride_h * w + j*conv->stride_w;

                    for (int k = 0; k < maxk; k++) // 29.23
                    {
                        float val = sptr[ space_ofs[k] ]; // 20.72
                        float w = kptr[k];
                        sum += val * w; // 41.45
                    }

                    kptr += maxk;
                }

                outptr[j] = sum;
            }

            outptr += outw;
        }
    }
    fprintf(stderr, "forward end.\n");

    return 0;
}

void permute(float *src, float *dst, int type, int w, int h, int channel)
{
    // 0 (W,H,C) -> (C,W,H) (0,1,2)->(2,0,1)
    // 1 (C,H,W) -> (W,H,C) (0,1,2)->(2,1,0)
    // 2 (W,H,C) -> (H,W,C) (0,1,2)->(1,0,2)
    // 3 (W,H,C) -> (W,C,H) (0,1,2)->(2,0,1)
    if (type == 0)
    {
        int step = w*h;
        int outstep = channel*w;
        #pragma omp parallel for
        for (int q=0; q<h; q++)
        {
            //fprintf(stderr, "q: %d\n", q);
            float* outptr = dst + q * outstep;

            for (int i = 0; i < w; i++)
            {
                for (int j = 0; j < channel; j++)
                {
                    const float* ptr = src + j * step + w * q;

                    outptr[i*channel + j] = ptr[i];
                }
            }
        }
    }
    else if (type == 1)
    {
        int step = channel*h;
        int outstep = w*h;

        for (int q=0; q<h; q++)
        {
            float* outptr = dst + q * outstep;

            for (int i = 0; i < channel; i++)
            {
                for (int j = 0; j < w; j++)
                {
                    const float* ptr = src + j * step + channel * q;

                    outptr[i*channel + j] = ptr[i];
                }
            }
        }
    }
    else if (type == 2)
    {
        int step = w*h;
        int outstep = h*w;

        // #pragma omp parallel for
        for (int q=0; q<channel; q++)
        {
            float* outptr = dst + q * outstep;
            const float* ptr = src + q * step;

            for (int i = 0; i < w; i++)
            {
                for (int j = 0; j < h; j++)
                {
                    outptr[h*i + j] = ptr[j*w + i];
                }
            }
        }
    }
    else if (type == 3)
    {
        int step = w*h;
        int outstep = w*channel;

        // #pragma omp parallel for
        for (int q=0; q<h; q++)
        {
            float* outptr = dst + q*outstep;

            for (int i = 0; i < channel; i++)
            {
                const float* ptr = src + i*step + w*q;

                for (int j = 0; j < w; j++)
                {
                    outptr[i*w + j] = ptr[j];
                }
            }
        }
    }
    
}

#ifdef ARM_NEON
int forward_conv_arm(const int *bottom_blob, float *top_blob, Convolution *conv)
{
    const int kernel_extent_w = conv->dilation_w * (conv->kernel_w - 1) + 1;
    const int kernel_extent_h = conv->dilation_h * (conv->kernel_h - 1) + 1;

    int w = conv->w + conv->pad_w + conv->pad_w;
    int h = conv->h + conv->pad_h + conv->pad_h;
    int channel = conv->c;
    float *bottom_blob_bordered;
    int *bottom_blob_float;
    bottom_blob_bordered = (float*)malloc(w*h*channel*sizeof(float));
    bottom_blob_float = (int*)malloc(conv->w*conv->h*channel*sizeof(int));
    memset(bottom_blob_bordered, 0, w*h*channel*sizeof(float));
    memset(bottom_blob_float, 0, w*h*channel*sizeof(int));
    
    copy_make_border(bottom_blob, bottom_blob_float, conv->pad_h, conv->pad_h, conv->pad_w, conv->pad_w, channel, 0.f, conv);
    Int2Float(bottom_blob_float, bottom_blob_bordered, conv->w*conv->h*channel);

    int outw = (w - kernel_extent_w) / conv->stride_w + 1;
    int outh = (h - kernel_extent_h) / conv->stride_h + 1;

    int step = w * h;
    int outstep = outw * outh;
    #pragma omp parallel for
    for (int p=0; p<conv->num_output; p++)
    {
        float* outptr = top_blob + outstep * p;
        float sum = 0.f;

        if (conv->bias_term)
            sum = conv->bias_data[p];

        for (int st=0; st < outstep; st++)
            outptr[st] = sum;

        for (int q=0; q<channel; q++)
        {
            float* outptr1 = outptr;

            const float* img0 = bottom_blob_bordered + q * step;

            const float* kernel0 = conv->weight_data + p*channel*15  + q*15;

            const float* r0 = img0;
            const float* r1 = img0 + w;
            const float* r2 = img0 + w*2;

            float32x4_t _k0123 = vld1q_f32(kernel0);
            float32x4_t _k4567 = vld1q_f32(kernel0+4);
            float32x4_t _k891011 = vld1q_f32(kernel0+8);
            float32x4_t _k12131415 = vld1q_f32(kernel0+12);

            for (int i = 0; i < outh; i++)
            {
                // 因为现在刚好是4的整数倍，所以不需要写remain的计算。
                int nn = outw >> 2;
                // int remain = outw & 3;

                for (; nn>0; nn--)
                {
                    float32x4_t _sum = vld1q_f32(outptr1);

                    float32x4_t _r00 = vld1q_f32(r0);
                    float32x4_t _r04 = vld1q_f32(r0 + 4);
                    float32x4_t _r01 = vextq_f32(_r00, _r04, 1);
                    float32x4_t _r02 = vextq_f32(_r00, _r04, 2);
                    float32x4_t _r03 = vextq_f32(_r00, _r04, 3);

                    float32x4_t _r10 = vld1q_f32(r1);
                    float32x4_t _r14 = vld1q_f32(r1 + 4);
                    float32x4_t _r11 = vextq_f32(_r10, _r14, 1);
                    float32x4_t _r12 = vextq_f32(_r10, _r14, 2);
                    float32x4_t _r13 = vextq_f32(_r10, _r14, 3);

                    float32x4_t _r20 = vld1q_f32(r2);
                    float32x4_t _r24 = vld1q_f32(r2 + 4);
                    float32x4_t _r21 = vextq_f32(_r20, _r24, 1);
                    float32x4_t _r22 = vextq_f32(_r20, _r24, 2);
                    float32x4_t _r23 = vextq_f32(_r20, _r24, 3);

                    _sum = vfmaq_laneq_f32(_sum, _r00, _k0123, 0);
                    _sum = vfmaq_laneq_f32(_sum, _r01, _k0123, 1);
                    _sum = vfmaq_laneq_f32(_sum, _r02, _k0123, 2);
                    _sum = vfmaq_laneq_f32(_sum, _r03, _k0123, 3);
                    _sum = vfmaq_laneq_f32(_sum, _r04, _k4567, 0);

                    _sum = vfmaq_laneq_f32(_sum, _r10, _k4567, 1);
                    _sum = vfmaq_laneq_f32(_sum, _r11, _k4567, 2);
                    _sum = vfmaq_laneq_f32(_sum, _r12, _k4567, 3);
                    _sum = vfmaq_laneq_f32(_sum, _r13, _k891011, 0);
                    _sum = vfmaq_laneq_f32(_sum, _r14, _k891011, 1);

                    _sum = vfmaq_laneq_f32(_sum, _r20, _k891011, 2);
                    _sum = vfmaq_laneq_f32(_sum, _r21, _k891011, 3);
                    _sum = vfmaq_laneq_f32(_sum, _r22, _k12131415, 0);
                    _sum = vfmaq_laneq_f32(_sum, _r23, _k12131415, 1);
                    _sum = vfmaq_laneq_f32(_sum, _r24, _k12131415, 2);

                    vst1q_f32(outptr1, _sum);

                    r0 += 4;
                    r1 += 4;
                    r2 += 4;
                    outptr1 += 4;
                }
                r0 += 4;
                r1 += 4;
                r2 += 4;
            }
        }   
    }
    //fprintf(stderr, "forward end.\n");

    return 0;
}

int forward_conv_compile(const int *bottom_blob, float *top_blob, Convolution *conv)
{
    //fprintf(stderr, "compile start\n");
    const int kernel_extent_w = conv->dilation_w * (conv->kernel_w - 1) + 1;
    const int kernel_extent_h = conv->dilation_h * (conv->kernel_h - 1) + 1;

    int w = conv->w + conv->pad_w + conv->pad_w;
    int h = conv->h + conv->pad_h + conv->pad_h;
    int channel = conv->c;
    float *bottom_blob_bordered;
    int *bottom_blob_float;
    bottom_blob_bordered = (float*)malloc(w*h*channel*sizeof(float));
    bottom_blob_float = (int*)malloc(w*h*channel*sizeof(int));
    memset(bottom_blob_bordered, 0, w*h*channel*sizeof(float));
    memset(bottom_blob_float, 0, w*h*channel*sizeof(int));
    
    copy_make_border(bottom_blob, bottom_blob_float, conv->pad_h, conv->pad_h, conv->pad_w, conv->pad_w, channel, 0.f, conv);
    Int2Float(bottom_blob_float, bottom_blob_bordered, w*h*channel);

    int outw = (w - kernel_extent_w) / conv->stride_w + 1;
    int outh = (h - kernel_extent_h) / conv->stride_h + 1;

    int step = w * h;
    int outstep = outw * outh;
    #pragma omp parallel for
    for (int p=0; p<conv->num_output; p++)
    {
        float* outptr = top_blob + outstep * p;
        float sum = 0.f;

        if (conv->bias_term)
            sum = conv->bias_data[p];

        for (int st=0; st < outstep; st++)
            outptr[st] = sum;

        for (int q=0; q<channel; q++)
        {
            float* outptr1 = outptr;
            float* outptr2 = outptr + outw;

            const float* img0 = bottom_blob_bordered + q * step;

            const float* kernel0 = conv->weight_data + p*channel*15  + q*15;

            const float* r0 = img0;
            const float* r1 = img0 + w;
            const float* r2 = img0 + w*2;
            const float* r3 = img0 + w*3;

            float32x4_t _k0123 = vld1q_f32(kernel0);
            float32x4_t _k4567 = vld1q_f32(kernel0+4);
            float32x4_t _k891011 = vld1q_f32(kernel0+8);
            float32x4_t _k12131415 = vld1q_f32(kernel0+12);

            for (int i = 0; i+1 < outh; i+=2)
            {
                // 因为现在刚好是4的整数倍，所以不需要写remain的计算。
                int nn = outw >> 2;
                // int remain = outw & 3;

                if (nn > 0)
                {
                asm volatile(
                    // v11 = rx1 / rx3
                    // v12 = rx2
                    // v13 v14 = intermediate sum register

                    "prfm       pldl1keep, [%1, #128]          \n"
                    "ld1        {v7.4s}, [%1]                  \n"// v7 = out

                    "0:                                        \n"

                    "prfm       pldl1keep, [%2, #128]          \n"
                    "ld1        {v8.4s}, [%2]                  \n"// v8 = out2
                    // r1
                    "prfm       pldl1keep, [%4, #256]          \n"
                    "ld1        {v9.4s, v10.4s}, [%4]          \n"// v9 v10 = r10 r14
                    "add        %4, %4, #16                    \n" 

                    "ext        v11.16b, v9.16b, v10.16b, #4   \n" //r11
                    "fmul       v13.4s, v9.4s, %15.s[1]        \n"

                    "fmul       v14.4s, v9.4s, %14.s[0]        \n"
                    "fmla       v8.4s, v11.4s, %14.s[1]        \n"

                    "ext        v12.16b, v9.16b, v10.16b, #8   \n" //r12
                    "fmla       v7.4s,  v11.4s, %15.s[2]       \n"

                    "ext        v11.16b, v9.16b, v10.16b, #12  \n" //r13
                    "fmla       v13.4s, v12.4s, %15.s[3]       \n"

                    "fmla       v7.4s,  v11.4s, %16.s[0]       \n"
                    "fmla       v14.4s, v12.4s, %14.s[2]       \n"
                    "fmla       v8.4s,  v11.4s, %14.s[3]       \n"
                    "fmla       v14.4s, v10.4s, %15.s[0]       \n"

                    "prfm       pldl1keep, [%5, #256]          \n"

                    "fmla       v13.4s, v10.4s, %16.s[1]       \n"

                    // r2
                    "ld1        {v9.4s, v10.4s}, [%5]          \n"// v9 v10 = r20 r24
                    "add        %5, %5, #16                    \n"

                    "ext        v11.16b, v9.16b, v10.16b, #4   \n" //r21
                    "fmla       v7.4s,  v9.4s, %16.s[2]        \n"        

                    "fmla       v8.4s,  v9.4s,  %15.s[1]       \n"
                    "fmla       v14.4s, v10.4s, %16.s[1]       \n"     

                    "ext        v12.16b, v9.16b, v10.16b, #8   \n" //r22
                    "fmla       v13.4s, v11.4s, %16.s[3]       \n"
                    
                    "fmla       v8.4s,  v11.4s, %15.s[2]       \n"
                    "fmla       v14.4s, v12.4s, %15.s[3]       \n"

                    "ext        v11.16b, v9.16b, v10.16b, #12  \n" //r23
                    "fmla       v7.4s,  v12.4s, %17.s[0]       \n"

                    "fmla       v8.4s,  v11.4s, %16.s[0]       \n"

                    "fmla       v13.4s, v11.4s, %17.s[1]       \n"
                    "fmla       v7.4s,  v10.4s, %17.s[2]       \n"

                    "prfm       pldl1keep, [%6, #256]          \n"
                    // r3
                    "ld1        {v9.4s, v10.4s}, [%6]          \n"// v9 v10 = r30 r34
                    "add        %6, %6, #16                    \n"

                    "ext        v11.16b, v9.16b, v10.16b, #4   \n" //r31
                    "fmla       v14.4s,  v9.4s, %16.s[2]       \n"

                    "fmla       v8.4s,  v10.4s, %17.s[2]       \n"
                    "fmla       v14.4s, v11.4s, %16.s[3]       \n"

                    "ext        v12.16b, v9.16b, v10.16b, #8   \n" //r32
                    "fmla       v8.4s,  v12.4s,  %17.s[0]      \n"

                    "ext        v11.16b, v9.16b, v10.16b, #12  \n" //r33
                    "fmla       v14.4s, v11.4s,  %17.s[1]      \n"

                    "fadd       v8.4s, v8.4s, v14.4s           \n"

                    "st1        {v8.4s}, [%2], #16             \n"

                    "prfm       pldl1keep, [%3, #256]          \n"

                    // r0 and r5
                    "ld1        {v9.4s, v10.4s}, [%3]          \n"// v9 v10 = r00 r04
                    "add        %3, %3, #16                    \n"//16x8/32=4 -> r0 += 4

                    "ext        v11.16b, v9.16b, v10.16b, #4   \n" //r01
                    "fmla       v13.4s, v11.4s, %14.s[1]       \n"

                    "ext        v12.16b, v9.16b, v10.16b, #8   \n" //r02
                    "fmla       v7.4s, v12.4s, %14.s[2]        \n"

                    "ext        v11.16b, v9.16b, v10.16b, #12  \n" //r03
                    "fmla       v13.4s, v11.4s, %14.s[3]       \n"

                    "fmla       v7.4s,  v9.4s,  %14.s[0]       \n"
                    "fmla       v13.4s, v10.4s, %15.s[0]       \n"

                    "fadd       v7.4s, v7.4s, v13.4s           \n"

                    "st1        {v7.4s}, [%1], #16             \n"

                    "prfm       pldl1keep, [%1, #128]          \n"
                    "ld1        {v7.4s}, [%1]                  \n"// v7 = out

                    "subs       %w0, %w0, #1                   \n"
                    "bne        0b                             \n"
                    : "=r"(nn),         // %0
                      "=r"(outptr1),     // %1
                      "=r"(outptr2),    // %2
                      "=r"(r0),         // %3
                      "=r"(r1),         // %4
                      "=r"(r2),         // %5
                      "=r"(r3)
                    : "0"(nn),
                      "1"(outptr1),
                      "2"(outptr2),
                      "3"(r0),
                      "4"(r1),
                      "5"(r2),
                      "6"(r3),
                      "w"(_k0123),      // %14
                      "w"(_k4567),      // %15
                      "w"(_k891011),    // %16
                      "w"(_k12131415)  // %17
                    : "cc", "memory", "v7", "v8", "v9", "v10", "v11", "v12", "v13", "v14"
                );
                }
                r0 += 4 + w;
                r1 += 4 + w;
                r2 += 4 + w;
                r3 += 4 + w;

                outptr1 += outw;
                outptr2 += outw;
            }
        }   
    }
    //fprintf(stderr, "forward end.\n");
    //fprintf(stderr, "address: %d, bordered: %d, bottom: %d, top: %d\n", bottom_blob_float, bottom_blob_bordered, bottom_blob, top_blob);
    free(bottom_blob_float);
    bottom_blob_float = NULL;
    free(bottom_blob_bordered);
    bottom_blob_bordered = NULL;

    return 0;
}
#endif

#ifdef OPENCL
int forward_conv_cl(const int *bottom_blob, float *top_blob, Convolution *conv, cl_command_queue command_queue, cl_context context, cl_kernel kernel)
{
    const int kernel_extent_w = conv->dilation_w * (conv->kernel_w - 1) + 1;
    const int kernel_extent_h = conv->dilation_h * (conv->kernel_h - 1) + 1;

    int w = conv->w + conv->pad_w + conv->pad_w;
    int h = conv->h + conv->pad_h + conv->pad_h;
    int channel = conv->c;
    float *bottom_blob_bordered;
    int *bottom_blob_float;
    bottom_blob_bordered = (float*)malloc(w*h*channel*sizeof(float));
    bottom_blob_float = (int*)malloc(w*h*channel*sizeof(int));
    memset(bottom_blob_bordered, 0, w*h*channel*sizeof(int));
    memset(bottom_blob_float, 0, w*h*channel*sizeof(int));
    
    copy_make_border(bottom_blob, bottom_blob_float, conv->pad_h, conv->pad_h, conv->pad_w, conv->pad_w, channel, 0.f, conv);
    Int2Float(bottom_blob_float, bottom_blob_bordered, w*h*channel);
    int outw = (w - kernel_extent_w) / conv->stride_w + 1;
    int outh = (h - kernel_extent_h) / conv->stride_h + 1;

    int i,j;
    for (i = 0; i < 280; i++ )
    {
        for (j = 0; j < 24*24; j++)
            top_blob[i*24*24 + j] = conv->bias_data[i];
    }

    cl_int ret;

    cl_mem in_mem_obj  = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                                        (w*h*1024) * sizeof(float), bottom_blob_bordered, &ret);
    cl_mem weight_mem_obj  = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                                        5*3*1024*280 * sizeof(float), conv->weight_data, &ret);
    cl_mem out_mem_obj = clCreateBuffer(context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR,
                                        outw*outh*280*sizeof(float) , top_blob, &ret);
    void *mapped_memory;
    
    ret = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void *)&in_mem_obj);
    ret = clSetKernelArg(kernel, 1, sizeof(cl_mem), (void *)&weight_mem_obj);
    ret = clSetKernelArg(kernel, 2, sizeof(cl_mem), (void *)&out_mem_obj);

    // Execute the OpenCL kernel on the list
    size_t global_item_size[3] = {3, 24, 280}; // Process the entire lists
    size_t local_item_size[3] = {1, 2, 14}; // Divide work items into groups of 64
    for (i = 0; i < 1024; i++)
    {
        ret = clSetKernelArg(kernel, 3, sizeof(int), (void *)&i);
        ret = clEnqueueNDRangeKernel(command_queue, kernel, 3, NULL,
                                 global_item_size, local_item_size, 0, NULL, NULL);
        //ret = clEnqueueReadBuffer(command_queue, out_mem_obj, CL_TRUE, 0, outw*outh*240*sizeof(float), top_blob, 0, NULL, NULL);

        //clFinish(command_queue);
    }

    mapped_memory = clEnqueueMapBuffer(command_queue, out_mem_obj, CL_TRUE, CL_MAP_READ | CL_MAP_WRITE, 
                            0, outw*outh*280*sizeof(float), 0, NULL, NULL, &ret);
    memcpy(top_blob, mapped_memory, outw*outh*280*sizeof(float));
    ret = clEnqueueUnmapMemObject(command_queue, out_mem_obj, mapped_memory, 0, NULL, NULL);

    ret = clFlush(command_queue);
    ret = clFinish(command_queue);
    ret = clReleaseMemObject(in_mem_obj);
    ret = clReleaseMemObject(weight_mem_obj);
    ret = clReleaseMemObject(out_mem_obj);


    return 0;
}
#endif