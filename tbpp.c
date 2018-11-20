#include "tbpp.h"
#include "convolution.h"
#include <CL/cl.h>

static SAMPLE_SVP_NNIE_PARAM_S s_stTextNnieParam = {0};
static SAMPLE_SVP_NNIE_MODEL_S s_stSsdModel = {0};
static SAMPLE_SVP_NNIE_SSD_SOFTWARE_PARAM_S s_stSsdSoftwareParam = {0};
Convolution Layer;

void ConvLayerInit(Convolution *pLayer)
{
    pLayer->num_output  = 240;
    pLayer->num_output2 = 40;
    pLayer->kernel_w = 5;
    pLayer->kernel_h = 3;
    pLayer->dilation_w = 1;
    pLayer->dilation_h = 1;
    pLayer->stride_w = 1;
    pLayer->stride_h = 1;
    pLayer->pad_w = 2;
    pLayer->pad_h = 1;
    pLayer->bias_term = 1;

    pLayer->weight_data_size  = 3686400;
    pLayer->weight_data_size2 = 614400;
    pLayer->w = 24;
    pLayer->h = 24;
    pLayer->c = 1024;

    pLayer->PerData = (float*)malloc(280*24*24*sizeof(float));
    pLayer->PermuteData[0] = pLayer->PerData;
    pLayer->PermuteData[1] = pLayer->PermuteData[0] + 240*24*24;
    pLayer->output_temp = (float*)malloc(280*24*24*sizeof(float));
}

void ConvRelease(Convolution *pLayer)
{
    free(pLayer->PerData);
    free(pLayer->weight_data);
    free(pLayer->bias_data);
    free(pLayer->output_temp);
}

void Convload_model(const char *protopath, Convolution *pLayer)
{
    FILE* fp = fopen(protopath, "rb");
    int nread;
    pLayer->weight_data = (float*)malloc(sizeof(float) * (pLayer->weight_data_size + pLayer->weight_data_size2));
    pLayer->bias_data = (float*)malloc((pLayer->num_output + pLayer->num_output2) * sizeof(float));
    nread = fread(pLayer->weight_data, pLayer->weight_data_size * sizeof(float), 1, fp);
    nread = fread(pLayer->bias_data, pLayer->num_output * sizeof(float), 1, fp);
    nread = fread(pLayer->weight_data + pLayer->weight_data_size, pLayer->weight_data_size2 * sizeof(float), 1, fp);
    nread = fread(pLayer->bias_data + pLayer->num_output, pLayer->num_output2 * sizeof(float), 1, fp);
    fclose(fp);
}

void DKMultiClassDetectionEnd()
{
    ConvRelease(&Layer);
    SAMPLE_SVP_NNIE_Ssd_Deinit(&s_stTextNnieParam,&s_stSsdSoftwareParam,&s_stSsdModel);
    SAMPLE_COMM_SVP_CheckSysExit();
}

void DKMultiClassDetectionInit()
{
    HI_CHAR *pcModelName = "textboxes_fc7.wk";
    HI_U32 u32PicNum = 1;
    HI_S32 s32Ret = HI_SUCCESS;
    SAMPLE_SVP_NNIE_CFG_S   stNnieCfg = {0};

    /*Set configuration parameter*/
    stNnieCfg.u32MaxInputNum = u32PicNum; //max input image num in each batch
    stNnieCfg.u32MaxRoiNum = 0;
    stNnieCfg.aenNnieCoreId[0] = SVP_NNIE_ID_0;//set NNIE core

    /*Sys init*/
    SAMPLE_COMM_SVP_CheckSysInit();

    /*Ssd Load model*/
    SAMPLE_SVP_TRACE_INFO("Ssd Load model!\n");
    s32Ret = SAMPLE_COMM_SVP_NNIE_LoadModel(pcModelName,&s_stSsdModel);
    SAMPLE_SVP_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret,SSD_FAIL_0,SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error,SAMPLE_COMM_SVP_NNIE_LoadModel failed!\n");

    /*Ssd parameter initialization*/
    /*Ssd parameters are set in SAMPLE_SVP_NNIE_Ssd_SoftwareInit,
      if user has changed net struct, please make sure the parameter settings in
      SAMPLE_SVP_NNIE_Ssd_SoftwareInit function are correct*/
    SAMPLE_SVP_TRACE_INFO("Ssd parameter initialization!\n");
    s_stTextNnieParam.pstModel = &s_stSsdModel.stModel;
    // FIXME:
    s32Ret = SAMPLE_SVP_NNIE_Ssd_ParamInit(&stNnieCfg,&s_stTextNnieParam,&s_stSsdSoftwareParam);
    SAMPLE_SVP_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret,SSD_FAIL_0,SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error,SAMPLE_SVP_NNIE_Ssd_ParamInit failed!\n");
    
    ConvLayerInit(&Layer);
    Convload_model("nnie.bin", &Layer);

SSD_FAIL_0:
    if (s32Ret != HI_SUCCESS)
        DKMultiClassDetectionEnd();
}

DKSMultiDetectionRes DKMultiClassDetectionProcess(char * imgfilename)
{
    DKSMultiDetectionRes DetectRes = {0};
    SAMPLE_SVP_NNIE_INPUT_DATA_INDEX_S stInputDataIdx = {0};
    SAMPLE_SVP_NNIE_PROCESS_SEG_INDEX_S stProcSegIdx = {0};
    HI_S32 s32Ret = HI_SUCCESS;
    HI_FLOAT f32PrintResultThresh = 0.2f;

    struct timeval tv_begin, tv_end;
    gettimeofday(&tv_begin,NULL);
    /*Fill src data*/
    SAMPLE_SVP_TRACE_INFO("Ssd start!\n");
    stInputDataIdx.u32SegIdx = 0;
    stInputDataIdx.u32NodeIdx = 0;
    s32Ret = SAMPLE_SVP_NNIE_FillSrcData(imgfilename, &s_stTextNnieParam, &stInputDataIdx);
    SAMPLE_SVP_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret,SSD_FAIL_0,SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error,SAMPLE_SVP_NNIE_FillSrcData failed!\n");

    /*NNIE process(process the 0-th segment)*/
    stProcSegIdx.u32SegIdx = 0;
    s32Ret = SAMPLE_SVP_NNIE_Forward(&s_stTextNnieParam,&stInputDataIdx,&stProcSegIdx,HI_TRUE);
    SAMPLE_SVP_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret,SSD_FAIL_0,SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error,SAMPLE_SVP_NNIE_Forward failed!\n");


    /*software process*/
    /*if user has changed net struct, please make sure SAMPLE_SVP_NNIE_Ssd_GetResult
     function's input datas are correct*/
    s32Ret = SAMPLE_SVP_NNIE_Ssd_GetResult(&s_stTextNnieParam,&s_stSsdSoftwareParam, &Layer);
    SAMPLE_SVP_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret,SSD_FAIL_0,SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error,SAMPLE_SVP_NNIE_Ssd_GetResult failed!\n");
    
    gettimeofday(&tv_end, NULL);
    double total_time = (double) (tv_end.tv_sec - tv_begin.tv_sec)*1000 + (double)(tv_end.tv_usec - tv_begin.tv_usec)/1000;
    //fprintf(stderr, "begin - end: %d\n", (int)tv_begin.tv_sec - (int)tv_end.tv_sec);
    SAMPLE_SVP_TRACE_INFO("%.5f ms\n", total_time);

     SAMPLE_SVP_TRACE_INFO("Ssd result:\n");
    (void)TextBoxes_plusplus_Result(&DetectRes, &s_stSsdSoftwareParam.stDstScore,
        &s_stSsdSoftwareParam.stDstRoi, &s_stSsdSoftwareParam.stClassRoiNum,f32PrintResultThresh);

    return DetectRes;


SSD_FAIL_0:
    if (s32Ret != HI_SUCCESS)
        DKMultiClassDetectionEnd();
}

int tbpp_sample(char* a, char* b)
{
    SAMPLE_SVP_NNIE_Ssd(a, b);
    return 0;
}