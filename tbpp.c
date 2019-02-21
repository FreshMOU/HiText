#include "tbpp.h"

static SAMPLE_SVP_NNIE_PARAM_S s_stTextNnieParam = {0};
static SAMPLE_SVP_NNIE_MODEL_S s_stSsdModel = {0};
static SAMPLE_SVP_NNIE_SSD_SOFTWARE_PARAM_S s_stSsdSoftwareParam = {0};

static SAMPLE_SVP_NNIE_PARAM_S s_stHandNnieParam = {0};
static SAMPLE_SVP_NNIE_MODEL_S s_stHandModel = {0};
static SAMPLE_SVP_NNIE_SSD_SOFTWARE_PARAM_S s_stHandSoftwareParam = {0};

void DKMultiClassDetectionEnd()
{
    SAMPLE_SVP_NNIE_Ssd_Deinit(&s_stTextNnieParam,&s_stSsdSoftwareParam,&s_stSsdModel);
    SAMPLE_SVP_NNIE_Ssd_Deinit(&s_stHandNnieParam,&s_stHandSoftwareParam,&s_stHandModel);
    SAMPLE_COMM_SVP_CheckSysExit();
}

void DKMultiClassDetectionInit()
{
    HI_CHAR *hand_pcModelName = "ssd_hand.wk";
    HI_U32 u32PicNum = 1;
    HI_S32 s32Ret = HI_SUCCESS;
    SAMPLE_SVP_NNIE_CFG_S  hand_stNnieCfg = {0};

    /*Set configuration parameter*/
    hand_stNnieCfg.u32MaxInputNum = u32PicNum; //max input image num in each batch
    hand_stNnieCfg.u32MaxRoiNum = 0;
    hand_stNnieCfg.aenNnieCoreId[0] = SVP_NNIE_ID_1;//set NNIE core

    /*Sys init*/
    SAMPLE_COMM_SVP_CheckSysInit();

    /*Ssd Load model*/
    SAMPLE_SVP_TRACE_INFO("Ssd_Hnad Load model!\n");
    s32Ret = SAMPLE_COMM_SVP_NNIE_LoadModel(hand_pcModelName,&s_stHandModel);
    SAMPLE_SVP_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret,SSD_FAIL_0,SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error,SAMPLE_COMM_SVP_NNIE_LoadModel failed!\n");

    /*Ssd parameter initialization*/
    /*Ssd parameters are set in SAMPLE_SVP_NNIE_Ssd_SoftwareInit,
      if user has changed net struct, please make sure the parameter settings in
      SAMPLE_SVP_NNIE_Ssd_SoftwareInit function are correct*/
    SAMPLE_SVP_TRACE_INFO("Ssd_Hand parameter initialization!\n");
    s_stHandNnieParam.pstModel = &s_stHandModel.stModel;
    // FIXME:
    s32Ret = HAND_SVP_NNIE_Ssd_ParamInit(&hand_stNnieCfg,&s_stHandNnieParam,&s_stHandSoftwareParam);
    SAMPLE_SVP_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret,SSD_FAIL_0,SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error,SAMPLE_SVP_NNIE_Ssd_ParamInit failed!\n");

    SAMPLE_SVP_TRACE_INFO("DKMultiClassDetectionInit start\n");
    HI_CHAR *pcModelName = "textboxes_fc7.wk";
    //HI_U32 u32PicNum = 1;
    //HI_S32 s32Ret = HI_SUCCESS;
    SAMPLE_SVP_NNIE_CFG_S   stNnieCfg = {0};

    /*Set configuration parameter*/
    stNnieCfg.u32MaxInputNum = u32PicNum; //max input image num in each batch
    stNnieCfg.u32MaxRoiNum = 0;
    stNnieCfg.aenNnieCoreId[0] = SVP_NNIE_ID_0;//set NNIE core

    /*Sys init*/
    //SAMPLE_COMM_SVP_CheckSysInit();

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

SSD_FAIL_0:
    if (s32Ret != HI_SUCCESS)
        DKMultiClassDetectionEnd();
}

DKSMultiDetectionRes DKMultiClassDetectionProcess(char * imgfilename, float f32PrintResultThresh)
{
    DKSMultiDetectionRes DetectRes = {0};
    SAMPLE_SVP_NNIE_INPUT_DATA_INDEX_S stInputDataIdx = {0};
    SAMPLE_SVP_NNIE_PROCESS_SEG_INDEX_S stProcSegIdx = {0};
    HI_S32 s32Ret = HI_SUCCESS;
    //HI_FLOAT f32PrintResultThresh = 0.5f;

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
    s32Ret = SAMPLE_SVP_NNIE_Ssd_GetResult(&s_stTextNnieParam,&s_stSsdSoftwareParam);
    SAMPLE_SVP_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret,SSD_FAIL_0,SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error,SAMPLE_SVP_NNIE_Ssd_GetResult failed!\n");
    
    gettimeofday(&tv_end, NULL);
    double total_time = (double) (tv_end.tv_sec - tv_begin.tv_sec)*1000 + (double)(tv_end.tv_usec - tv_begin.tv_usec)/1000;
    //fprintf(stderr, "begin - end: %d\n", (int)tv_begin.tv_sec - (int)tv_end.tv_sec);
    SAMPLE_SVP_TRACE_INFO("%.5f ms\n", total_time);
    //SAMPLE_SVP_TRACE_INFO("%f\n", f32PrintResultThresh);
    SAMPLE_SVP_TRACE_INFO("Text result:\n");
    (void)TextBoxes_plusplus_Result(&DetectRes, &s_stSsdSoftwareParam.stDstScore,
        &s_stSsdSoftwareParam.stDstRoi, &s_stSsdSoftwareParam.stClassRoiNum,f32PrintResultThresh);

    return DetectRes;


SSD_FAIL_0:
    if (s32Ret != HI_SUCCESS)
        DKMultiClassDetectionEnd();
}

DKSMultiDetectionRes DKHandProcess(char * imgfilename, float f32PrintResultThresh)
{
    DKSMultiDetectionRes DetectRes = {0};
    SAMPLE_SVP_NNIE_INPUT_DATA_INDEX_S stInputDataIdx = {0};
    SAMPLE_SVP_NNIE_PROCESS_SEG_INDEX_S stProcSegIdx = {0};
    HI_S32 s32Ret = HI_SUCCESS;
    //HI_FLOAT f32PrintResultThresh = 0.5f;

    /*Fill src data*/
    SAMPLE_SVP_TRACE_INFO("Ssd_hand start!\n");
    stInputDataIdx.u32SegIdx = 0;
    stInputDataIdx.u32NodeIdx = 0;
    s32Ret = SAMPLE_SVP_NNIE_FillSrcData(imgfilename, &s_stHandNnieParam, &stInputDataIdx);
    SAMPLE_SVP_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret,SSD_FAIL_0,SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error,SAMPLE_SVP_NNIE_FillSrcData failed!\n");

    /*NNIE process(process the 0-th segment)*/
    SAMPLE_SVP_TRACE_INFO("Hand forward start!\n");
    stProcSegIdx.u32SegIdx = 0;
    s32Ret = SAMPLE_SVP_NNIE_Forward(&s_stHandNnieParam,&stInputDataIdx,&stProcSegIdx,HI_TRUE);
    SAMPLE_SVP_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret,SSD_FAIL_0,SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error,SAMPLE_SVP_NNIE_Forward failed!\n");

    SAMPLE_SVP_TRACE_INFO("get Hand result start!\n");
    /*software process*/
    /*if user has changed net struct, please make sure SAMPLE_SVP_NNIE_Ssd_GetResult
     function's input datas are correct*/
    s32Ret = Hand_SVP_NNIE_Ssd_GetResult(&s_stHandNnieParam,&s_stHandSoftwareParam);
    SAMPLE_SVP_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret,SSD_FAIL_0,SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error,SAMPLE_SVP_NNIE_Ssd_GetResult failed!\n");
     SAMPLE_SVP_TRACE_INFO("Hand result:\n");
    (void)SSD_Hand_Result(&DetectRes, &s_stHandSoftwareParam.stDstScore,
        &s_stHandSoftwareParam.stDstRoi, &s_stHandSoftwareParam.stClassRoiNum,f32PrintResultThresh);

    return DetectRes;


SSD_FAIL_0:
    if (s32Ret != HI_SUCCESS)
        DKMultiClassDetectionEnd();
}