float8 convolution_5x1_8(float16 input, float k1, float k2, float k3, float k4, float k5)
{
    float8 value1 = input.s01234567;
    float8 value2 = input.s12345678;
    float8 value3 = input.s23456789;
    float8 value4 = input.s3456789a;
    float8 value5 = input.s456789ab;

    return value1 * (float8)k1 + value2 * (float8)k2 + value3 * (float8)k3 + value4 * (float8)k4 + value5 * (float8)k5;
}

__kernel void convolution_5x3(__global float *src, __global float *k, __global float *dst, int c)
{
    int out_channel, w, h;
    int k_offset;
    int src_offset;
    float16 input1, input2, input3, inK;
    float8 out;


    w = get_global_id(0);
    h = get_global_id(1);
    out_channel = get_global_id(2);

    k_offset = out_channel * 15 * 1024;
    src_offset = h * 28 + c * 28 * 26;
    out = vload8(w, dst + h * 24 + out_channel * 24 * 24);

    input1 = vload16(w, src+src_offset-8*w);
    input2 = vload16(w, src+src_offset+28-8*w);
    input3 = vload16(w, src+src_offset+2*28-8*w);
    inK  = vload16(c, k + k_offset -c);
    out += convolution_5x1_8(input1, inK.s0, inK.s1, inK.s2, inK.s3, inK.s4);
    out += convolution_5x1_8(input2, inK.s5, inK.s6, inK.s7, inK.s8, inK.s9);
    out += convolution_5x1_8(input3, inK.sa, inK.sb, inK.sc, inK.sd, inK.se);

    vstore8(out, w, dst + h * 24 + out_channel * 24 * 24);
}