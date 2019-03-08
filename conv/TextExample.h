#include <stdio.h>
#include "paramdict.h"
#include "convolution.h"
#include <string.h>

int load_param_fp(const char *protopath, Convolution *layers, int w, int h);

int load_model_fp(const char *protopath, Convolution *layers);
