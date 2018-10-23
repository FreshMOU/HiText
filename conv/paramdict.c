#include "paramdict.h"

static int vstr_is_float(const char vstr[16])
{
    // look ahead for determine isfloat
    for (int j=0; j<16; j++)
    {
        if (vstr[j] == '\0')
            break;

        if (vstr[j] == '.')
            return 1;
    }

    return 0;
}

int getInt(int layer_idx, int idx, int def)
{
    return params[layer_idx][idx].loaded ? params[layer_idx][idx].i : def;
}

float getFloat(int layer_idx, int idx, float def)
{
    return params[layer_idx][idx].loaded ? params[layer_idx][idx].f : def;
}

int load_param_dict(FILE* fp, int layer_idx)
{
    for (int i = 0; i < MAX_PARAM; i++)
    {
        params[layer_idx][i].loaded = 0;
        params[layer_idx][i].i = 0;
    }
    
    fprintf(stderr, "load_param_dict\n");
    // parse each key=value pair
    int id = 0;
    while (fscanf(fp, "%d=", &id) == 1)
    {
        char vstr[16];
        int nscan = fscanf(fp, "%15s", vstr);
        if (nscan != 1)
        {
            fprintf(stderr, "ParamDict read value fail\n");
            return -1;
        }

        int is_float = vstr_is_float(vstr);

        if (is_float)
            nscan = sscanf(vstr, "%f", &params[layer_idx][id].f);
        else
            nscan = sscanf(vstr, "%d", &params[layer_idx][id].i);
        if (nscan != 1)
        {
            fprintf(stderr, "ParamDict parse value fail\n");
            return -1;
        }

        params[layer_idx][id].loaded = 1;
    }

    return 0;
}