#include "TextExample.h"


int w[4] = {48, 48, 24, 24};
int h[4] = {48, 48, 24, 24};
int c[4] = {512, 512, 1024, 1024};

/* layers有 4 层分别如下
*  0 -> conv4_3_norm_mbox_loc
*  1 -> conv4_3_norm_mbox_conf
*  2 -> fc_mbox_loc
*  3 -> fc_mbox_conf
*/

int load_param_fp(const char *protopath, Convolution *layers)
{
    FILE* fp = fopen(protopath, "rb");
    if (!fp)
    {
        fprintf(stderr, "fopen %s failed\n", protopath);
        return -1;
    }

    int layer_idx = 0;
    while (!feof(fp))
    {
        int nscan = 0;

        char layer_type[256];
        char layer_name[256];
        int bottom_count = 0;
        int top_count = 0;
        nscan = fscanf(fp, "%256s %256s %d %d", layer_type, layer_name, &bottom_count, &top_count);
        if (nscan != 4)
        {
            continue;
        }

        if (!strcmp(layer_type, "Convolution"))
        {
            // layer specific params
            int pdlr = load_param_dict(fp, layer_idx);
            if (pdlr != 0)
            {
                fprintf(stderr, "ParamDict load_param failed\n");
                continue;
            }

            int lr = load_param_conv(layers+layer_idx, layer_idx, w[layer_idx+2], h[layer_idx+2], c[layer_idx+2]);
            if (lr != 0)
            {
                fprintf(stderr, "layer load_param failed\n");
                return -1;
            }
            layer_idx++;
        }
    }
    fclose(fp);

    return 0;
}

int load_model_fp(const char *protopath, Convolution *layers)
{
    FILE* fp = fopen(protopath, "rb");
    if (!fp)
    {
        fprintf(stderr, "fopen %s failed\n", protopath);
        return -1;
    }

    for (int i=0; i<2; i++)
    {
        load_model_conv(fp, layers+i);
    }

    fclose(fp);
    return 0;
}


