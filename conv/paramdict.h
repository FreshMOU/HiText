#include <stdio.h>

#ifndef PARAMDICT_H
#define PARAMDICT_H

#define MAX_PARAM 20
#define MAX_LAYER 5

struct paramdict{
    int loaded;
    union{
        int i ;
        float f;
    };
} params[MAX_LAYER][MAX_PARAM];

int load_param_dict(FILE* fp, int layer_idx);
int getInt(int layer_idx, int idx, int def);
float getFloat(int layer_idx, int idx, float def);

#endif