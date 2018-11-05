#include"sample_svp_nnie_software.h"

#ifdef __cplusplus    // If used by C++ code,
extern "C" {          // we need to export the C interface
#endif

static HI_FLOAT s_af32ExpCoef[10][16] = {
    {1.0f, 1.00024f, 1.00049f, 1.00073f, 1.00098f, 1.00122f, 1.00147f, 1.00171f, 1.00196f, 1.0022f, 1.00244f, 1.00269f, 1.00293f, 1.00318f, 1.00342f, 1.00367f},
    {1.0f, 1.00391f, 1.00784f, 1.01179f, 1.01575f, 1.01972f, 1.02371f, 1.02772f, 1.03174f, 1.03578f, 1.03984f, 1.04391f, 1.04799f, 1.05209f, 1.05621f, 1.06034f},
    {1.0f, 1.06449f, 1.13315f, 1.20623f, 1.28403f, 1.36684f, 1.45499f, 1.54883f, 1.64872f, 1.75505f, 1.86825f, 1.98874f, 2.117f, 2.25353f, 2.39888f, 2.55359f},
    {1.0f, 2.71828f, 7.38906f, 20.0855f, 54.5981f, 148.413f, 403.429f, 1096.63f, 2980.96f, 8103.08f, 22026.5f, 59874.1f, 162755.0f, 442413.0f, 1.2026e+006f, 3.26902e+006f},
    {1.0f, 8.88611e+006f, 7.8963e+013f, 7.01674e+020f, 6.23515e+027f, 5.54062e+034f, 5.54062e+034f, 5.54062e+034f, 5.54062e+034f, 5.54062e+034f, 5.54062e+034f, 5.54062e+034f, 5.54062e+034f, 5.54062e+034f, 5.54062e+034f, 5.54062e+034f},
    {1.0f, 0.999756f, 0.999512f, 0.999268f, 0.999024f, 0.99878f, 0.998536f, 0.998292f, 0.998049f, 0.997805f, 0.997562f, 0.997318f, 0.997075f, 0.996831f, 0.996588f, 0.996345f},
    {1.0f, 0.996101f, 0.992218f, 0.98835f, 0.984496f, 0.980658f, 0.976835f, 0.973027f, 0.969233f, 0.965455f, 0.961691f, 0.957941f, 0.954207f, 0.950487f, 0.946781f, 0.94309f},
    {1.0f, 0.939413f, 0.882497f, 0.829029f, 0.778801f, 0.731616f, 0.687289f, 0.645649f, 0.606531f, 0.569783f, 0.535261f, 0.502832f, 0.472367f, 0.443747f, 0.416862f, 0.391606f},
    {1.0f, 0.367879f, 0.135335f, 0.0497871f, 0.0183156f, 0.00673795f, 0.00247875f, 0.000911882f, 0.000335463f, 0.00012341f, 4.53999e-005f, 1.67017e-005f, 6.14421e-006f, 2.26033e-006f, 8.31529e-007f, 3.05902e-007f},
    {1.0f, 1.12535e-007f, 1.26642e-014f, 1.42516e-021f, 1.60381e-028f, 1.80485e-035f, 2.03048e-042f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f}
};

/*****************************************************************************
* Prototype :   SVP_NNIE_QuickExp
* Description : this function is used to quickly get exp result
* Input :     HI_S32    s32Value           [IN]   input value
*
*
*
*
* Output :
* Return Value : HI_FLOAT: output value.
* Spec :
* Calls :
* Called By :
* History:
*
* 1. Date : 2017-11-10
* Author :
* Modification : Create
*
*****************************************************************************/
static HI_FLOAT SVP_NNIE_QuickExp( HI_S32 s32Value )
{
    if( s32Value & 0x80000000 )
    {
        s32Value = ~s32Value + 0x00000001;
        return s_af32ExpCoef[5][s32Value & 0x0000000F] * s_af32ExpCoef[6][(s32Value>>4) & 0x0000000F] * s_af32ExpCoef[7][(s32Value>>8) & 0x0000000F] * s_af32ExpCoef[8][(s32Value>>12) & 0x0000000F] * s_af32ExpCoef[9][(s32Value>>16) & 0x0000000F ];
    }
    else
    {
        return s_af32ExpCoef[0][s32Value & 0x0000000F] * s_af32ExpCoef[1][(s32Value>>4) & 0x0000000F] * s_af32ExpCoef[2][(s32Value>>8) & 0x0000000F] * s_af32ExpCoef[3][(s32Value>>12) & 0x0000000F] * s_af32ExpCoef[4][(s32Value>>16) & 0x0000000F ];
    }
}

/*****************************************************************************
* Prototype :   SVP_NNIE_SoftMax
* Description : this function is used to do softmax
* Input :     HI_FLOAT*         pf32Src           [IN]   the pointer to source data
*             HI_U32             u32Num           [IN]   the num of source data
*
*
*
*
* Output :
* Return Value : HI_SUCCESS: Success;Error codes: Failure.
* Spec :
* Calls :
* Called By :
* History:
*
* 1. Date : 2017-11-10
* Author :
* Modification : Create
*
*****************************************************************************/
static HI_S32 SVP_NNIE_SoftMax( HI_FLOAT* pf32Src, HI_U32 u32Num)
{
    HI_FLOAT f32Max = 0;
    HI_FLOAT f32Sum = 0;
    HI_U32 i = 0;

    for(i = 0; i < u32Num; ++i)
    {
        if(f32Max < pf32Src[i])
        {
            f32Max = pf32Src[i];
        }
    }

    for(i = 0; i < u32Num; ++i)
    {
        pf32Src[i] = (HI_FLOAT)SVP_NNIE_QuickExp((HI_S32)((pf32Src[i] - f32Max)*SAMPLE_SVP_NNIE_QUANT_BASE));
        f32Sum += pf32Src[i];
    }

    for(i = 0; i < u32Num; ++i)
    {
        pf32Src[i] /= f32Sum;
    }
    return HI_SUCCESS;
}


/*****************************************************************************
* Prototype :   SVP_NNIE_SSD_SoftMax
* Description : this function is used to do softmax for SSD
* Input :       HI_S32*           pf32Src          [IN]   the pointer to input array
*               HI_S32            s32ArraySize     [IN]   the array size
*               HI_S32*           ps32Dst          [OUT]  the pointer to output array
*
*
*
*
* Output :
* Return Value : void
* Spec :
* Calls :
* Called By :
* History:
*
* 1. Date : 2017-03-10
* Author :
* Modification : Create
*
*****************************************************************************/
static HI_S32 SVP_NNIE_SSD_SoftMax(HI_S32* ps32Src, HI_S32 s32ArraySize, HI_S32* ps32Dst)
{
    /***** define parameters ****/
    HI_S32 s32Max = 0;
    HI_S32 s32Sum = 0;
    HI_S32 i = 0;
    for (i = 0; i < s32ArraySize; ++i)
    {
        if (s32Max < ps32Src[i])
        {
            s32Max = ps32Src[i];
        }
    }
    for (i = 0; i < s32ArraySize; ++i)
    {
        ps32Dst[i] = (HI_S32)(SAMPLE_SVP_NNIE_QUANT_BASE* exp((HI_FLOAT)(ps32Src[i] - s32Max) / SAMPLE_SVP_NNIE_QUANT_BASE));
        s32Sum += ps32Dst[i];
    }
    for (i = 0; i < s32ArraySize; ++i)
    {
        ps32Dst[i] = (HI_S32)(((HI_FLOAT)ps32Dst[i] / (HI_FLOAT)s32Sum) * SAMPLE_SVP_NNIE_QUANT_BASE);
    }
    return HI_SUCCESS;
}

static HI_S32 SVP_NNIE_SSD_SoftMax_Float(float* ps32Src, HI_S32 s32ArraySize, HI_S32* ps32Dst)
{
    /***** define parameters ****/
    float s32Max = 0;
    float s32Sum = 0;
    HI_S32 i = 0;
    for (i = 0; i < s32ArraySize; ++i)
    {
        if (s32Max < ps32Src[i])
        {
            s32Max = ps32Src[i];
        }
    }
    for (i = 0; i < s32ArraySize; ++i)
    {
        ps32Dst[i] = (HI_S32)(SAMPLE_SVP_NNIE_QUANT_BASE* exp((HI_FLOAT)(ps32Src[i] - s32Max) / SAMPLE_SVP_NNIE_QUANT_BASE));
        s32Sum += ps32Dst[i];
    }
    for (i = 0; i < s32ArraySize; ++i)
    {
        ps32Dst[i] = (HI_S32)(((HI_FLOAT)ps32Dst[i] / (HI_FLOAT)s32Sum) * SAMPLE_SVP_NNIE_QUANT_BASE);
    }
    return HI_SUCCESS;
}


/*****************************************************************************
* Prototype :   SVP_NNIE_Argswap
* Description : this function is used to exchange array data
* Input :       HI_FLOAT*           pf32Src1          [IN]   the pointer to the first array
*               HI_S32*             ps32Src2          [OUT]  the pointer to the second array
*
*
*
*
* Output :
* Return Value : void
* Spec :
* Calls :
* Called By :
* History:
*
* 1. Date : 2017-03-10
* Author :
* Modification : Create
*
*****************************************************************************/
static void SVP_NNIE_Argswap(HI_S32* ps32Src1, HI_S32* ps32Src2)
{
    HI_U32 i = 0;
    HI_S32 u32Tmp = 0;
    for( i = 0; i < SAMPLE_SVP_NNIE_PROPOSAL_WIDTH; i++ )
    {
        u32Tmp = ps32Src1[i];
        ps32Src1[i] = ps32Src2[i];
        ps32Src2[i] = u32Tmp;
    }
}


/*****************************************************************************
* Prototype :   SVP_NNIE_NonRecursiveArgQuickSort
* Description : this function is used to do quick sort
* Input :       HI_S32*             ps32Array         [IN]   the array need to be sorted
*               HI_S32              s32Low            [IN]   the start position of quick sort
*               HI_S32              s32High           [IN]   the end position of quick sort
*               SAMPLE_SVP_NNIE_STACK_S *  pstStack   [IN]   the buffer used to store start positions and end positions
*
*
*
*
* Output :
* Return Value : HI_SUCCESS: Success;Error codes: Failure.
* Spec :
* Calls :
* Called By :
* History:
*
* 1. Date : 2017-03-10
* Author :
* Modification : Create
*
*****************************************************************************/
static HI_S32 SVP_NNIE_NonRecursiveArgQuickSort(HI_S32* ps32Array,
    HI_S32 s32Low, HI_S32 s32High, SAMPLE_SVP_NNIE_STACK_S *pstStack,HI_U32 u32MaxNum)
{
    HI_S32 i = s32Low;
    HI_S32 j = s32High;
    HI_S32 s32Top = 0;
    HI_S32 s32KeyConfidence = ps32Array[SAMPLE_SVP_NNIE_PROPOSAL_WIDTH * s32Low + 12];
    pstStack[s32Top].s32Min = s32Low;
    pstStack[s32Top].s32Max = s32High;

    while(s32Top > -1)
    {
        s32Low = pstStack[s32Top].s32Min;
        s32High = pstStack[s32Top].s32Max;
        i = s32Low;
        j = s32High;
        s32Top--;

        s32KeyConfidence = ps32Array[SAMPLE_SVP_NNIE_PROPOSAL_WIDTH * s32Low + 12];

        while(i < j)
        {
            while((i < j) && (s32KeyConfidence > ps32Array[j * SAMPLE_SVP_NNIE_PROPOSAL_WIDTH + 12]))
            {
                j--;
            }
            if(i < j)
            {
                SVP_NNIE_Argswap(&ps32Array[i*SAMPLE_SVP_NNIE_PROPOSAL_WIDTH], &ps32Array[j*SAMPLE_SVP_NNIE_PROPOSAL_WIDTH]);
                i++;
            }

            while((i < j) && (s32KeyConfidence < ps32Array[i*SAMPLE_SVP_NNIE_PROPOSAL_WIDTH + 12]))
            {
                i++;
            }
            if(i < j)
            {
                SVP_NNIE_Argswap(&ps32Array[i*SAMPLE_SVP_NNIE_PROPOSAL_WIDTH], &ps32Array[j*SAMPLE_SVP_NNIE_PROPOSAL_WIDTH]);
                j--;
            }
        }

        if(s32Low <= u32MaxNum)
        {
                if(s32Low < i-1)
                {
                    s32Top++;
                    pstStack[s32Top].s32Min = s32Low;
                    pstStack[s32Top].s32Max = i-1;
                }

                if(s32High > i+1)
                {
                    s32Top++;
                    pstStack[s32Top].s32Min = i+1;
                    pstStack[s32Top].s32Max = s32High;
                }
        }
    }
    return HI_SUCCESS;
}


/*****************************************************************************
* Prototype :   SVP_NNIE_Overlap
* Description : this function is used to calculate the overlap ratio of two proposals
* Input :     HI_S32              s32XMin1          [IN]   first input proposal's minimum value of x coordinate
*               HI_S32              s32YMin1          [IN]   first input proposal's minimum value of y coordinate of first input proposal
*               HI_S32              s32XMax1          [IN]   first input proposal's maximum value of x coordinate of first input proposal
*               HI_S32              s32YMax1          [IN]   first input proposal's maximum value of y coordinate of first input proposal
*               HI_S32              s32XMin1          [IN]   second input proposal's minimum value of x coordinate
*               HI_S32              s32YMin1          [IN]   second input proposal's minimum value of y coordinate of first input proposal
*               HI_S32              s32XMax1          [IN]   second input proposal's maximum value of x coordinate of first input proposal
*               HI_S32              s32YMax1          [IN]   second input proposal's maximum value of y coordinate of first input proposal
*             HI_FLOAT            *pf32IoU          [INOUT]the pointer of the IoU value
*
*
* Output :
* Return Value : HI_FLOAT f32Iou.
* Spec :
* Calls :
* Called By :
* History:
*
* 1. Date : 2017-03-10
* Author :
* Modification : Create
*
*****************************************************************************/
static HI_S32 SVP_NNIE_Overlap(HI_S32 s32XMin1, HI_S32 s32YMin1, HI_S32 s32XMax1, HI_S32 s32YMax1, HI_S32 s32XMin2,
    HI_S32 s32YMin2, HI_S32 s32XMax2, HI_S32 s32YMax2,  HI_S32* s32AreaSum, HI_S32* s32AreaInter)
{
    /*** Check the input, and change the Return value  ***/
    HI_S32 s32Inter = 0;
    HI_S32 s32Total = 0;
    HI_S32 s32XMin = 0;
    HI_S32 s32YMin = 0;
    HI_S32 s32XMax = 0;
    HI_S32 s32YMax = 0;
    HI_S32 s32Area1 = 0;
    HI_S32 s32Area2 = 0;
    HI_S32 s32InterWidth = 0;
    HI_S32 s32InterHeight = 0;

    s32XMin = SAMPLE_SVP_NNIE_MAX(s32XMin1, s32XMin2);
    s32YMin = SAMPLE_SVP_NNIE_MAX(s32YMin1, s32YMin2);
    s32XMax = SAMPLE_SVP_NNIE_MIN(s32XMax1, s32XMax2);
    s32YMax = SAMPLE_SVP_NNIE_MIN(s32YMax1, s32YMax2);

    s32InterWidth = s32XMax - s32XMin + 1;
    s32InterHeight = s32YMax - s32YMin + 1;

    s32InterWidth = ( s32InterWidth >= 0 ) ? s32InterWidth : 0;
    s32InterHeight = ( s32InterHeight >= 0 ) ? s32InterHeight : 0;

    s32Inter = s32InterWidth * s32InterHeight;
    s32Area1 = (s32XMax1 - s32XMin1 + 1) * (s32YMax1 - s32YMin1 + 1);
    s32Area2 = (s32XMax2 - s32XMin2 + 1) * (s32YMax2 - s32YMin2 + 1);

    s32Total = s32Area1 + s32Area2 - s32Inter;

    *s32AreaSum = s32Total;
    *s32AreaInter = s32Inter;
    return HI_SUCCESS;
}

/*****************************************************************************
* Prototype :   SVP_NNIE_FilterLowScoreBbox
* Description : this function is used to remove low score bboxes, in order to speed-up Sort & RPN procedures.
* Input :      HI_S32*         ps32Proposals     [IN]   proposals
*              HI_U32          u32NumAnchors     [IN]   input anchors' num
*              HI_U32          u32FilterThresh   [IN]   rpn configuration
*              HI_U32*         u32NumAfterFilter [OUT]  output num of anchors after low score filtering
*
*
*
*
* Output :
* Return Value : HI_SUCCESS: Success;Error codes: Failure.
* Spec :
* Calls :
* Called By :
* History:
*
* 1. Date : 2017-03-10
* Author :
* Modification : Create
*
*****************************************************************************/
static HI_S32 SVP_NNIE_FilterLowScoreBbox(HI_S32* ps32Proposals, HI_U32 u32AnchorsNum,
    HI_U32 u32FilterThresh, HI_U32* u32NumAfterFilter)
{
    HI_U32 u32ProposalCnt = u32AnchorsNum;
    HI_U32 i = 0;

    if( u32FilterThresh > 0 )
    {
        for( i = 0; i < u32AnchorsNum; i++ )
        {
            if( ps32Proposals[SAMPLE_SVP_NNIE_PROPOSAL_WIDTH * i + 4 ] < (HI_S32)u32FilterThresh )
            {
                ps32Proposals[SAMPLE_SVP_NNIE_PROPOSAL_WIDTH * i + 5 ] = 1;
            }
        }

        u32ProposalCnt = 0;
        for( i = 0; i < u32AnchorsNum; i++ )
        {
            if( 0 == ps32Proposals[ SAMPLE_SVP_NNIE_PROPOSAL_WIDTH * i + 5 ] )
            {
                ps32Proposals[ SAMPLE_SVP_NNIE_PROPOSAL_WIDTH * u32ProposalCnt ] = ps32Proposals[ SAMPLE_SVP_NNIE_PROPOSAL_WIDTH * i ];
                ps32Proposals[ SAMPLE_SVP_NNIE_PROPOSAL_WIDTH * u32ProposalCnt + 1 ] = ps32Proposals[ SAMPLE_SVP_NNIE_PROPOSAL_WIDTH * i + 1 ];
                ps32Proposals[ SAMPLE_SVP_NNIE_PROPOSAL_WIDTH * u32ProposalCnt + 2 ] = ps32Proposals[ SAMPLE_SVP_NNIE_PROPOSAL_WIDTH * i + 2 ];
                ps32Proposals[ SAMPLE_SVP_NNIE_PROPOSAL_WIDTH * u32ProposalCnt + 3 ] = ps32Proposals[ SAMPLE_SVP_NNIE_PROPOSAL_WIDTH * i + 3 ];
                ps32Proposals[ SAMPLE_SVP_NNIE_PROPOSAL_WIDTH * u32ProposalCnt + 4 ] = ps32Proposals[ SAMPLE_SVP_NNIE_PROPOSAL_WIDTH * i + 4 ];
                ps32Proposals[ SAMPLE_SVP_NNIE_PROPOSAL_WIDTH * u32ProposalCnt + 5 ] = ps32Proposals[ SAMPLE_SVP_NNIE_PROPOSAL_WIDTH * i + 5 ];
                u32ProposalCnt++;
            }
        }
    }
    *u32NumAfterFilter = u32ProposalCnt;
    return HI_SUCCESS;
}
/*****************************************************************************
* Prototype :   SVP_NNIE_NonMaxSuppression
* Description : this function is used to do non maximum suppression
* Input :       HI_S32*           ps32Proposals     [IN]   proposals
*               HI_U32            u32AnchorsNum     [IN]   anchors num
*               HI_U32            u32NmsThresh      [IN]   non maximum suppression threshold
*               HI_U32            u32MaxRoiNum      [IN]  The max roi num for the roi pooling
*
*
*
*
* Output :
* Return Value : HI_SUCCESS: Success;Error codes: Failure.
* Spec :
* Calls :
* Called By :
* History:
*
* 1. Date : 2017-03-10
* Author :
* Modification : Create
*
*****************************************************************************/
static HI_S32 SVP_NNIE_NonMaxSuppression( HI_S32* ps32Proposals, HI_U32 u32AnchorsNum,
    HI_U32 u32NmsThresh,HI_U32 u32MaxRoiNum)
{
    /****** define variables *******/
    HI_S32 s32XMin1 = 0;
    HI_S32 s32YMin1 = 0;
    HI_S32 s32XMax1 = 0;
    HI_S32 s32YMax1 = 0;
    HI_S32 s32XMin2 = 0;
    HI_S32 s32YMin2 = 0;
    HI_S32 s32XMax2 = 0;
    HI_S32 s32YMax2 = 0;
    HI_S32 s32AreaTotal = 0;
    HI_S32 s32AreaInter = 0;
    HI_U32 i = 0;
    HI_U32 j = 0;
    HI_U32 u32Num = 0;
    HI_BOOL bNoOverlap  = HI_TRUE;
    for (i = 0; i < u32AnchorsNum && u32Num < u32MaxRoiNum; i++)
    {
        if( ps32Proposals[SAMPLE_SVP_NNIE_PROPOSAL_WIDTH*i+13] == 0 )
        {
            u32Num++;
            s32XMin1 =  ps32Proposals[SAMPLE_SVP_NNIE_PROPOSAL_WIDTH*i];
            s32YMin1 =  ps32Proposals[SAMPLE_SVP_NNIE_PROPOSAL_WIDTH*i+1];
            s32XMax1 =  ps32Proposals[SAMPLE_SVP_NNIE_PROPOSAL_WIDTH*i+2];
            s32YMax1 =  ps32Proposals[SAMPLE_SVP_NNIE_PROPOSAL_WIDTH*i+3];
            for(j= i+1;j< u32AnchorsNum; j++)
            {
                if( ps32Proposals[SAMPLE_SVP_NNIE_PROPOSAL_WIDTH*j+13] == 0 )
                {
                    s32XMin2 = ps32Proposals[SAMPLE_SVP_NNIE_PROPOSAL_WIDTH*j];
                    s32YMin2 = ps32Proposals[SAMPLE_SVP_NNIE_PROPOSAL_WIDTH*j+1];
                    s32XMax2 = ps32Proposals[SAMPLE_SVP_NNIE_PROPOSAL_WIDTH*j+2];
                    s32YMax2 = ps32Proposals[SAMPLE_SVP_NNIE_PROPOSAL_WIDTH*j+3];
                    bNoOverlap = (s32XMin2>s32XMax1)||(s32XMax2<s32XMin1)||(s32YMin2>s32YMax1)||(s32YMax2<s32YMin1);
                    if(bNoOverlap)
                    {
                        continue;
                    }
                    (void)SVP_NNIE_Overlap(s32XMin1, s32YMin1, s32XMax1, s32YMax1, s32XMin2, s32YMin2, s32XMax2, s32YMax2, &s32AreaTotal, &s32AreaInter);
                    if(s32AreaInter*SAMPLE_SVP_NNIE_QUANT_BASE > ((HI_S32)u32NmsThresh*s32AreaTotal))
                    {
                        if( ps32Proposals[SAMPLE_SVP_NNIE_PROPOSAL_WIDTH*i+12] >= ps32Proposals[SAMPLE_SVP_NNIE_PROPOSAL_WIDTH*j+12] )
                        {
                            ps32Proposals[SAMPLE_SVP_NNIE_PROPOSAL_WIDTH*j+13] = 1;
                        }
                        else
                        {
                            ps32Proposals[SAMPLE_SVP_NNIE_PROPOSAL_WIDTH*i+13] = 1;
                        }
                    }
                }
            }
        }
    }
    return HI_SUCCESS;
}

/*****************************************************************************
* Prototype :   SVP_NNIE_Ssd_PriorBoxForward
* Description : this function is used to get SSD priorbox
* Input :     HI_U32 u32PriorBoxWidth            [IN] prior box width
*             HI_U32 u32PriorBoxHeight           [IN] prior box height
*             HI_U32 u32OriImWidth               [IN] input image width
*             HI_U32 u32OriImHeight              [IN] input image height
*             HI_U32 f32PriorBoxMinSize          [IN] prior box min size
*             HI_U32 u32MinSizeNum               [IN] min size num
*             HI_U32 f32PriorBoxMaxSize          [IN] prior box max size
*             HI_U32 u32MaxSizeNum               [IN] max size num
*             HI_BOOL bFlip                      [IN] whether do Flip
*             HI_BOOL bClip                      [IN] whether do Clip
*             HI_U32  u32InputAspectRatioNum     [IN] aspect ratio num
*             HI_FLOAT af32PriorBoxAspectRatio[] [IN] aspect ratio value
*             HI_FLOAT f32PriorBoxStepWidth      [IN] prior box step width
*             HI_FLOAT f32PriorBoxStepHeight     [IN] prior box step height
*             HI_FLOAT f32Offset                 [IN] offset value
*             HI_S32   as32PriorBoxVar[]         [IN] prior box variance
*             HI_S32*  ps32PriorboxOutputData    [OUT] output reslut
*
* Output :
* Return Value : HI_SUCCESS: Success;Error codes: Failure.
* Spec :
* Calls :
* Called By :
* History:
*
* 1. Date : 2017-11-10
* Author :
* Modification : Create
*
*****************************************************************************/
static HI_S32 SVP_NNIE_Ssd_PriorBoxForward(HI_U32 u32PriorBoxWidth,
    HI_U32 u32PriorBoxHeight, HI_U32 u32OriImWidth, HI_U32 u32OriImHeight,
    HI_FLOAT* pf32PriorBoxMinSize, HI_U32 u32MinSizeNum, HI_FLOAT* pf32PriorBoxMaxSize,
    HI_U32 u32MaxSizeNum, HI_BOOL bFlip, HI_BOOL bClip, HI_U32 u32InputAspectRatioNum,
    HI_FLOAT af32PriorBoxAspectRatio[],HI_FLOAT f32PriorBoxStepWidth,
    HI_FLOAT f32PriorBoxStepHeight,HI_FLOAT f32Offset,HI_S32 as32PriorBoxVar[],
    HI_S32* ps32PriorboxOutputData)
{
    HI_U32 u32AspectRatioNum = 0;
    HI_U32 u32Index = 0;
    HI_FLOAT af32AspectRatio[SAMPLE_SVP_NNIE_SSD_ASPECT_RATIO_NUM] = { 0 };
    HI_U32 u32NumPrior = 0;
    HI_FLOAT f32CenterX = 0;
    HI_FLOAT f32CenterY = 0;
    HI_FLOAT f32CenterOffsetY = 0;
    HI_FLOAT f32BoxHeight = 0;
    HI_FLOAT f32BoxWidth = 0;
    HI_FLOAT f32MaxBoxWidth = 0;
    HI_U32 i = 0;
    HI_U32 j = 0;
    HI_U32 n = 0;
    HI_U32 h = 0;
    HI_U32 w = 0;
    // SAMPLE_SVP_CHECK_EXPR_RET((HI_TRUE == bFlip && u32InputAspectRatioNum >
    //     (SAMPLE_SVP_NNIE_SSD_ASPECT_RATIO_NUM-1)/2),HI_INVALID_VALUE,SAMPLE_SVP_ERR_LEVEL_ERROR,
    //     "Error,when bFlip is true, u32InputAspectRatioNum(%d) can't be greater than %d!\n",
    //     u32InputAspectRatioNum, (SAMPLE_SVP_NNIE_SSD_ASPECT_RATIO_NUM-1)/2);
    // SAMPLE_SVP_CHECK_EXPR_RET((HI_FALSE == bFlip && u32InputAspectRatioNum >
    //     (SAMPLE_SVP_NNIE_SSD_ASPECT_RATIO_NUM-1)),HI_INVALID_VALUE,SAMPLE_SVP_ERR_LEVEL_ERROR,
    //     "Error,when bFlip is false, u32InputAspectRatioNum(%d) can't be greater than %d!\n",
    //     u32InputAspectRatioNum, (SAMPLE_SVP_NNIE_SSD_ASPECT_RATIO_NUM-1));

    // generate aspect_ratios
    u32AspectRatioNum = 0;
    af32AspectRatio[0] = 1;
    u32AspectRatioNum++;
    for (i = 0; i < u32InputAspectRatioNum; i++)
    {
        af32AspectRatio[u32AspectRatioNum++] = af32PriorBoxAspectRatio[i];
        if (bFlip)
        {
            af32AspectRatio[u32AspectRatioNum++] = 1.0f / af32PriorBoxAspectRatio[i];
        }
    }
    u32NumPrior = (u32MinSizeNum * u32AspectRatioNum + u32MaxSizeNum) * 2; //textboxes*2

    u32Index = 0;
    for (h = 0; h < u32PriorBoxHeight; h++)
    {
        for (w = 0; w < u32PriorBoxWidth; w++)
        {
            f32CenterX = (w + f32Offset) * f32PriorBoxStepWidth;
            f32CenterY = (h + f32Offset) * f32PriorBoxStepHeight;
            f32CenterOffsetY = (h + 1.0) * f32PriorBoxStepHeight;
            for (n = 0; n < u32MinSizeNum; n++)
            {
                /*** first prior ***/
                f32BoxHeight = pf32PriorBoxMinSize[n];
                f32BoxWidth = pf32PriorBoxMinSize[n];
                ps32PriorboxOutputData[u32Index++] = (HI_S32)(f32CenterX - f32BoxWidth * SAMPLE_SVP_NNIE_HALF);
                ps32PriorboxOutputData[u32Index++] = (HI_S32)(f32CenterY - f32BoxHeight * SAMPLE_SVP_NNIE_HALF);
                ps32PriorboxOutputData[u32Index++] = (HI_S32)(f32CenterX + f32BoxWidth * SAMPLE_SVP_NNIE_HALF);
                ps32PriorboxOutputData[u32Index++] = (HI_S32)(f32CenterY + f32BoxHeight * SAMPLE_SVP_NNIE_HALF);

                if (1)
                {
                    ps32PriorboxOutputData[u32Index++] = (HI_S32)(f32CenterX - f32BoxWidth * SAMPLE_SVP_NNIE_HALF);
                    ps32PriorboxOutputData[u32Index++] = (HI_S32)(f32CenterOffsetY - f32BoxHeight * SAMPLE_SVP_NNIE_HALF);
                    ps32PriorboxOutputData[u32Index++] = (HI_S32)(f32CenterX + f32BoxWidth * SAMPLE_SVP_NNIE_HALF);
                    ps32PriorboxOutputData[u32Index++] = (HI_S32)(f32CenterOffsetY + f32BoxHeight * SAMPLE_SVP_NNIE_HALF);
                }

                /*** second prior ***/
                if(u32MaxSizeNum>0)
                {
                    f32MaxBoxWidth = sqrt(pf32PriorBoxMinSize[n] * pf32PriorBoxMaxSize[n]);
                    f32BoxHeight = f32MaxBoxWidth;
                    f32BoxWidth = f32MaxBoxWidth;
                    ps32PriorboxOutputData[u32Index++] = (HI_S32)(f32CenterX - f32BoxWidth * SAMPLE_SVP_NNIE_HALF);
                    ps32PriorboxOutputData[u32Index++] = (HI_S32)(f32CenterY - f32BoxHeight * SAMPLE_SVP_NNIE_HALF);
                    ps32PriorboxOutputData[u32Index++] = (HI_S32)(f32CenterX + f32BoxWidth * SAMPLE_SVP_NNIE_HALF);
                    ps32PriorboxOutputData[u32Index++] = (HI_S32)(f32CenterY + f32BoxHeight * SAMPLE_SVP_NNIE_HALF);

                    if (1)
                    {
                        ps32PriorboxOutputData[u32Index++] = (HI_S32)(f32CenterX - f32BoxWidth * SAMPLE_SVP_NNIE_HALF);
                        ps32PriorboxOutputData[u32Index++] = (HI_S32)(f32CenterOffsetY - f32BoxHeight * SAMPLE_SVP_NNIE_HALF);
                        ps32PriorboxOutputData[u32Index++] = (HI_S32)(f32CenterX + f32BoxWidth * SAMPLE_SVP_NNIE_HALF);
                        ps32PriorboxOutputData[u32Index++] = (HI_S32)(f32CenterOffsetY + f32BoxHeight * SAMPLE_SVP_NNIE_HALF);
                    }
                }
                /**** rest of priors, skip AspectRatio == 1 ****/
                for (i = 1; i < u32AspectRatioNum; i++)
                {
                    f32BoxWidth = (HI_FLOAT)(pf32PriorBoxMinSize[n] * sqrt( af32AspectRatio[i] ));
                    f32BoxHeight = (HI_FLOAT)(pf32PriorBoxMinSize[n]/sqrt( af32AspectRatio[i] ));
                    ps32PriorboxOutputData[u32Index++] = (HI_S32)(f32CenterX - f32BoxWidth * SAMPLE_SVP_NNIE_HALF);
                    ps32PriorboxOutputData[u32Index++] = (HI_S32)(f32CenterY - f32BoxHeight * SAMPLE_SVP_NNIE_HALF);
                    ps32PriorboxOutputData[u32Index++] = (HI_S32)(f32CenterX + f32BoxWidth * SAMPLE_SVP_NNIE_HALF);
                    ps32PriorboxOutputData[u32Index++] = (HI_S32)(f32CenterY + f32BoxHeight * SAMPLE_SVP_NNIE_HALF);

                    if (1)
                    {
                        ps32PriorboxOutputData[u32Index++] = (HI_S32)(f32CenterX - f32BoxWidth * SAMPLE_SVP_NNIE_HALF);
                        ps32PriorboxOutputData[u32Index++] = (HI_S32)(f32CenterOffsetY - f32BoxHeight * SAMPLE_SVP_NNIE_HALF);
                        ps32PriorboxOutputData[u32Index++] = (HI_S32)(f32CenterX + f32BoxWidth * SAMPLE_SVP_NNIE_HALF);
                        ps32PriorboxOutputData[u32Index++] = (HI_S32)(f32CenterOffsetY + f32BoxHeight * SAMPLE_SVP_NNIE_HALF);
                    }
                }
            }
        }
    }
    /************ clip the priors' coordidates, within [0, u32ImgWidth] & [0, u32ImgHeight] *************/
    if (bClip)
    {
        for (i = 0; i < (HI_U32)(u32PriorBoxWidth * u32PriorBoxHeight * SAMPLE_SVP_NNIE_COORDI_NUM*u32NumPrior / 2); i++)
        {
            ps32PriorboxOutputData[2 * i] = SAMPLE_SVP_NNIE_MIN((HI_U32)SAMPLE_SVP_NNIE_MAX(ps32PriorboxOutputData[2 * i], 0), u32OriImWidth);
            ps32PriorboxOutputData[2 * i + 1] = SAMPLE_SVP_NNIE_MIN((HI_U32)SAMPLE_SVP_NNIE_MAX(ps32PriorboxOutputData[2 * i + 1], 0), u32OriImHeight);
        }
    }
    /*********************** get var **********************/
    for (h = 0; h < u32PriorBoxHeight; h++)
    {
        for (w = 0; w < u32PriorBoxWidth; w++)
        {
            for (i = 0; i < u32NumPrior; i++)
            {
                for (j = 0; j < SAMPLE_SVP_NNIE_COORDI_NUM; j++)
                {
                    ps32PriorboxOutputData[u32Index++] = (HI_S32)as32PriorBoxVar[j];
                }
            }
        }
    }
    return HI_SUCCESS;
}

/*****************************************************************************
* Prototype :   SVP_NNIE_Ssd_SoftmaxForward
* Description : this function is used to do SSD softmax
* Input :     HI_U32 u32SoftMaxInHeight          [IN] softmax input height
*             HI_U32 au32SoftMaxInChn[]          [IN] softmax input channel
*             HI_U32 u32ConcatNum                [IN] concat num
*             HI_U32 au32ConvStride[]            [IN] conv stride
*             HI_U32 u32SoftMaxOutWidth          [IN] softmax output width
*             HI_U32 u32SoftMaxOutHeight         [IN] softmax output height
*             HI_U32 u32SoftMaxOutChn            [IN] softmax output channel
*             HI_S32* aps32SoftMaxInputData[]    [IN] softmax input data
*             HI_S32* ps32SoftMaxOutputData      [OUT]softmax output data
*
*
* Output :
* Return Value : HI_SUCCESS: Success;Error codes: Failure.
* Spec :
* Calls :
* Called By :
* History:
*
* 1. Date : 2017-11-10
* Author :
* Modification : Create
*
*****************************************************************************/
static HI_S32 SVP_NNIE_Ssd_SoftmaxForward(HI_U32 u32SoftMaxInHeight,
    HI_U32 au32SoftMaxInChn[], HI_U32 u32ConcatNum, HI_U32 au32ConvStride[],
    HI_U32 u32SoftMaxOutWidth, HI_U32 u32SoftMaxOutHeight, HI_U32 u32SoftMaxOutChn,
    HI_S32* aps32SoftMaxInputData[], HI_S32* ps32SoftMaxOutputData, float* PermuteData[])
{
    HI_S32* ps32InputData = NULL;
    HI_S32* ps32OutputTmp = NULL;
    float * ps32InputDataFloat = NULL;
    HI_U32 u32OuterNum = 0;
    HI_U32 u32InnerNum = 0;
    HI_U32 u32InputChannel = 0;
    HI_U32 i = 0;
    HI_U32 u32ConcatCnt = 0;
    HI_S32 s32Ret = 0;
    HI_U32 u32Stride = 0;
    HI_U32 u32Skip = 0;
    HI_U32 u32Left = 0;
    ps32OutputTmp = ps32SoftMaxOutputData;
    int OutputCount = 0;
    for (u32ConcatCnt = 0; u32ConcatCnt < u32ConcatNum; u32ConcatCnt++)
    {
        if (u32ConcatCnt == 1)
        {
            ps32InputDataFloat = PermuteData[1];
            u32Stride = au32ConvStride[u32ConcatCnt];       //48 32 16 16 16 16
            u32InputChannel = au32SoftMaxInChn[u32ConcatCnt];   // 92160
            u32OuterNum = u32InputChannel / u32SoftMaxInHeight; // 48080
            u32InnerNum = u32SoftMaxInHeight;       // 2
            u32Skip = u32Stride / u32InnerNum;      //Skip = 24 16 8 8 8 8, Left = 0
            u32Left = u32Stride % u32InnerNum;        // do softmax
            for (i = 0; i < u32OuterNum; i++)
            {
                s32Ret = SVP_NNIE_SSD_SoftMax_Float(ps32InputDataFloat, (HI_S32)u32InnerNum, ps32OutputTmp);
                if ((i + 1) % u32Skip == 0)
                {
                    ps32InputDataFloat += u32Left;
                }
                ps32InputDataFloat += u32InnerNum;
                ps32OutputTmp += u32InnerNum;
                OutputCount += u32InnerNum;
            }
        }
        else
        {
            ps32InputData = aps32SoftMaxInputData[u32ConcatCnt];
            u32Stride = au32ConvStride[u32ConcatCnt];       //48 32 16 16 16 16
            u32InputChannel = au32SoftMaxInChn[u32ConcatCnt];   // 92160
            u32OuterNum = u32InputChannel / u32SoftMaxInHeight; // 48080
            u32InnerNum = u32SoftMaxInHeight;       // 2
            u32Skip = u32Stride / u32InnerNum;      //Skip = 24 16 8 8 8 8, Left = 0
            u32Left = u32Stride % u32InnerNum;        // do softmax
            for (i = 0; i < u32OuterNum; i++)
            {
                s32Ret = SVP_NNIE_SSD_SoftMax(ps32InputData, (HI_S32)u32InnerNum,ps32OutputTmp);
                if ((i + 1) % u32Skip == 0)
                {
                    ps32InputData += u32Left;
                }
                ps32InputData += u32InnerNum;
                ps32OutputTmp += u32InnerNum;
            }
        }
    }

    //fprintf(stderr, "OutputCount: %d\n", OutputCount);
    return s32Ret;
}

/*****************************************************************************
* Prototype :   SVP_NNIE_Ssd_DetectionOutForward
* Description : this function is used to get detection result of SSD
* Input :     HI_U32 u32ConcatNum            [IN] SSD concat num
*             HI_U32 u32ConfThresh           [IN] confidence thresh
*             HI_U32 u32ClassNum             [IN] class num
*             HI_U32 u32TopK                 [IN] Topk value
*             HI_U32 u32KeepTopK             [IN] KeepTopK value
*             HI_U32 u32NmsThresh            [IN] NMS thresh
*             HI_U32 au32DetectInputChn[]    [IN] detection input channel
*             HI_S32* aps32AllLocPreds[]     [IN] Location prediction
*             HI_S32* aps32AllPriorBoxes[]   [IN] prior box
*             HI_S32* ps32ConfScores         [IN] confidence score
*             HI_S32* ps32AssistMemPool      [IN] assist buffer
*             HI_S32* ps32DstScoreSrc        [OUT] result of score
*             HI_S32* ps32DstBboxSrc         [OUT] result of Bbox
*             HI_S32* ps32RoiOutCntSrc       [OUT] result of the roi num of each class
*
*
* Output :
* Return Value : HI_SUCCESS: Success;Error codes: Failure.
* Spec :
* Calls :
* Called By :
* History:
*
* 1. Date : 2017-11-10
* Author :
* Modification : Create
*
*****************************************************************************/
static HI_S32 SVP_NNIE_Ssd_DetectionOutForward(HI_U32 u32ConcatNum,
    HI_U32 u32ConfThresh,HI_U32 u32ClassNum, HI_U32 u32TopK, HI_U32 u32KeepTopK, HI_U32 u32NmsThresh,
    HI_U32 au32DetectInputChn[], HI_S32* aps32AllLocPreds[], HI_S32* aps32AllPriorBoxes[],
    HI_S32* ps32ConfScores, HI_S32* ps32AssistMemPool, HI_S32* ps32DstScoreSrc,
    HI_S32* ps32DstBboxSrc, HI_S32* ps32RoiOutCntSrc, float* PermuteData[])
{
    /************* check input parameters ****************/
    /******** define variables **********/
    HI_S32* ps32LocPreds = NULL;
    HI_S32* ps32PriorBoxes = NULL;
    HI_S32* ps32PriorVar = NULL;
    HI_S32* ps32AllDecodeBoxes = NULL;
    HI_S32* ps32DstScore = NULL;
    HI_S32* ps32DstBbox = NULL;
    HI_S32* ps32ClassRoiNum = NULL;
    HI_U32 u32RoiOutCnt = 0;
    HI_S32* ps32SingleProposal = NULL;
    HI_S32* ps32AfterTopK = NULL;
    SAMPLE_SVP_NNIE_STACK_S* pstStack = NULL;
    HI_U32 u32PriorNum = 0;
    HI_U32 u32NumPredsPerClass = 0;
    HI_FLOAT f32PriorWidth = 0;
    HI_FLOAT f32PriorHeight = 0;
    HI_FLOAT f32PriorCenterX = 0;
    HI_FLOAT f32PriorCenterY = 0;
    HI_FLOAT f32DecodeBoxCenterX = 0;
    HI_FLOAT f32DecodeBoxCenterY = 0;
    HI_FLOAT f32DecodeBoxWidth = 0;
    HI_FLOAT f32DecodeBoxHeight = 0;
    HI_U32 u32SrcIdx = 0;
    HI_U32 u32AfterFilter = 0;
    HI_U32 u32AfterTopK = 0;
    HI_U32 u32KeepCnt = 0;
    HI_U32 i = 0;
    HI_U32 j = 0;
    HI_U32 u32Offset = 0;
    HI_S32 s32Ret = HI_SUCCESS;
    u32PriorNum = 0;

    float* ps32LocPredsFloat = NULL;
    for (i = 0; i < u32ConcatNum; i++)
    {
        u32PriorNum += au32DetectInputChn[i] / SAMpLE_SVP_NNIE_POLYGON;
    }
    //prepare for Assist MemPool
    ps32AllDecodeBoxes = ps32AssistMemPool;
    ps32SingleProposal = ps32AllDecodeBoxes + u32PriorNum * SAMpLE_SVP_NNIE_POLYGON;
    ps32AfterTopK = ps32SingleProposal + SAMPLE_SVP_NNIE_PROPOSAL_WIDTH * u32PriorNum;
    pstStack = (SAMPLE_SVP_NNIE_STACK_S*)(ps32AfterTopK + u32PriorNum * SAMPLE_SVP_NNIE_PROPOSAL_WIDTH);
    u32SrcIdx = 0;
    for (i = 0; i < u32ConcatNum; i++)
    {
        //fprintf(stderr, "u32Idx:  %d\n", u32SrcIdx);
        if ( i == 1 )
        {
            ps32LocPredsFloat = PermuteData[0];
            u32NumPredsPerClass = au32DetectInputChn[i] / SAMpLE_SVP_NNIE_POLYGON;
            /********** get Prior Bboxes ************/
            ps32PriorBoxes = aps32AllPriorBoxes[i];
            ps32PriorVar = ps32PriorBoxes + u32NumPredsPerClass*SAMPLE_SVP_NNIE_COORDI_NUM;
            for (j = 0; j < u32NumPredsPerClass; j++)
            {
                //printf("ps32PriorBoxes start***************\n");
                f32PriorWidth = (HI_FLOAT)(ps32PriorBoxes[j*SAMPLE_SVP_NNIE_COORDI_NUM+2] - ps32PriorBoxes[j*SAMPLE_SVP_NNIE_COORDI_NUM]);
                f32PriorHeight = (HI_FLOAT)(ps32PriorBoxes[j*SAMPLE_SVP_NNIE_COORDI_NUM+3] - ps32PriorBoxes[j*SAMPLE_SVP_NNIE_COORDI_NUM + 1]);
                f32PriorCenterX = (ps32PriorBoxes[j*SAMPLE_SVP_NNIE_COORDI_NUM+2] + ps32PriorBoxes[j*SAMPLE_SVP_NNIE_COORDI_NUM])*SAMPLE_SVP_NNIE_HALF;
                f32PriorCenterY = (ps32PriorBoxes[j*SAMPLE_SVP_NNIE_COORDI_NUM+3] + ps32PriorBoxes[j*SAMPLE_SVP_NNIE_COORDI_NUM+1])*SAMPLE_SVP_NNIE_HALF;

                f32DecodeBoxCenterX = ((HI_FLOAT)ps32PriorVar[j*SAMPLE_SVP_NNIE_COORDI_NUM]/SAMPLE_SVP_NNIE_QUANT_BASE)*
                    ((HI_FLOAT)ps32LocPredsFloat[j*SAMpLE_SVP_NNIE_POLYGON]/SAMPLE_SVP_NNIE_QUANT_BASE)*f32PriorWidth+f32PriorCenterX;

                f32DecodeBoxCenterY = ((HI_FLOAT)ps32PriorVar[j*SAMPLE_SVP_NNIE_COORDI_NUM+1]/SAMPLE_SVP_NNIE_QUANT_BASE)*
                    ((HI_FLOAT)ps32LocPredsFloat[j*SAMpLE_SVP_NNIE_POLYGON+1]/SAMPLE_SVP_NNIE_QUANT_BASE)*f32PriorHeight+f32PriorCenterY;

                f32DecodeBoxWidth = exp(((HI_FLOAT)ps32PriorVar[j*SAMPLE_SVP_NNIE_COORDI_NUM+2]/SAMPLE_SVP_NNIE_QUANT_BASE)*
                    ((HI_FLOAT)ps32LocPredsFloat[j*SAMpLE_SVP_NNIE_POLYGON+2]/SAMPLE_SVP_NNIE_QUANT_BASE))*f32PriorWidth;

                f32DecodeBoxHeight = exp(((HI_FLOAT)ps32PriorVar[j*SAMPLE_SVP_NNIE_COORDI_NUM+3]/SAMPLE_SVP_NNIE_QUANT_BASE)*
                    ((HI_FLOAT)ps32LocPredsFloat[j*SAMpLE_SVP_NNIE_POLYGON+3]/SAMPLE_SVP_NNIE_QUANT_BASE))*f32PriorHeight;

                //printf("ps32PriorBoxes end***************\n");

                ps32AllDecodeBoxes[u32SrcIdx++] = (HI_S32)(f32DecodeBoxCenterX - f32DecodeBoxWidth * SAMPLE_SVP_NNIE_HALF);
                ps32AllDecodeBoxes[u32SrcIdx++] = (HI_S32)(f32DecodeBoxCenterY - f32DecodeBoxHeight * SAMPLE_SVP_NNIE_HALF);
                ps32AllDecodeBoxes[u32SrcIdx++] = (HI_S32)(f32DecodeBoxCenterX + f32DecodeBoxWidth * SAMPLE_SVP_NNIE_HALF);
                ps32AllDecodeBoxes[u32SrcIdx++] = (HI_S32)(f32DecodeBoxCenterY + f32DecodeBoxHeight * SAMPLE_SVP_NNIE_HALF);
                /* polygon */
                ps32AllDecodeBoxes[u32SrcIdx++] = (HI_S32)(ps32PriorVar[j*SAMPLE_SVP_NNIE_COORDI_NUM]/SAMPLE_SVP_NNIE_QUANT_BASE)*
                    (HI_FLOAT)(ps32LocPredsFloat[j*SAMpLE_SVP_NNIE_POLYGON+ 4]/SAMPLE_SVP_NNIE_QUANT_BASE)*f32PriorWidth +ps32PriorBoxes[j*SAMPLE_SVP_NNIE_COORDI_NUM];
                ps32AllDecodeBoxes[u32SrcIdx++] = (HI_S32)(ps32PriorVar[j*SAMPLE_SVP_NNIE_COORDI_NUM+1]/SAMPLE_SVP_NNIE_QUANT_BASE)*
                    (HI_FLOAT)(ps32LocPredsFloat[j*SAMpLE_SVP_NNIE_POLYGON+ 5]/SAMPLE_SVP_NNIE_QUANT_BASE)*f32PriorHeight+ps32PriorBoxes[j*SAMPLE_SVP_NNIE_COORDI_NUM+1];
                ps32AllDecodeBoxes[u32SrcIdx++] = (HI_S32)(ps32PriorVar[j*SAMPLE_SVP_NNIE_COORDI_NUM]/SAMPLE_SVP_NNIE_QUANT_BASE)*
                    (HI_FLOAT)(ps32LocPredsFloat[j*SAMpLE_SVP_NNIE_POLYGON+ 6]/SAMPLE_SVP_NNIE_QUANT_BASE)*f32PriorWidth +ps32PriorBoxes[j*SAMPLE_SVP_NNIE_COORDI_NUM+2];
                ps32AllDecodeBoxes[u32SrcIdx++] = (HI_S32)(ps32PriorVar[j*SAMPLE_SVP_NNIE_COORDI_NUM+1]/SAMPLE_SVP_NNIE_QUANT_BASE)*
                    (HI_FLOAT)(ps32LocPredsFloat[j*SAMpLE_SVP_NNIE_POLYGON+ 7]/SAMPLE_SVP_NNIE_QUANT_BASE)*f32PriorHeight+ps32PriorBoxes[j*SAMPLE_SVP_NNIE_COORDI_NUM+1];
                ps32AllDecodeBoxes[u32SrcIdx++] = (HI_S32)(ps32PriorVar[j*SAMPLE_SVP_NNIE_COORDI_NUM]/SAMPLE_SVP_NNIE_QUANT_BASE)*
                    (HI_FLOAT)(ps32LocPredsFloat[j*SAMpLE_SVP_NNIE_POLYGON+ 8]/SAMPLE_SVP_NNIE_QUANT_BASE)*f32PriorWidth +ps32PriorBoxes[j*SAMPLE_SVP_NNIE_COORDI_NUM+2];
                ps32AllDecodeBoxes[u32SrcIdx++] = (HI_S32)(ps32PriorVar[j*SAMPLE_SVP_NNIE_COORDI_NUM+1]/SAMPLE_SVP_NNIE_QUANT_BASE)*
                    (HI_FLOAT)(ps32LocPredsFloat[j*SAMpLE_SVP_NNIE_POLYGON+ 9]/SAMPLE_SVP_NNIE_QUANT_BASE)*f32PriorHeight+ps32PriorBoxes[j*SAMPLE_SVP_NNIE_COORDI_NUM+3];
                ps32AllDecodeBoxes[u32SrcIdx++] = (HI_S32)(ps32PriorVar[j*SAMPLE_SVP_NNIE_COORDI_NUM]/SAMPLE_SVP_NNIE_QUANT_BASE)*
                    (HI_FLOAT)(ps32LocPredsFloat[j*SAMpLE_SVP_NNIE_POLYGON+10]/SAMPLE_SVP_NNIE_QUANT_BASE)*f32PriorWidth +ps32PriorBoxes[j*SAMPLE_SVP_NNIE_COORDI_NUM];
                ps32AllDecodeBoxes[u32SrcIdx++] = (HI_S32)(ps32PriorVar[j*SAMPLE_SVP_NNIE_COORDI_NUM+1]/SAMPLE_SVP_NNIE_QUANT_BASE)*
                    (HI_FLOAT)(ps32LocPredsFloat[j*SAMpLE_SVP_NNIE_POLYGON+11]/SAMPLE_SVP_NNIE_QUANT_BASE)*f32PriorHeight+ps32PriorBoxes[j*SAMPLE_SVP_NNIE_COORDI_NUM+3];
            }
        }
        else
        {
            /********** get loc predictions ************/
            ps32LocPreds = aps32AllLocPreds[i];
            u32NumPredsPerClass = au32DetectInputChn[i] / SAMpLE_SVP_NNIE_POLYGON;
            /********** get Prior Bboxes ************/
            ps32PriorBoxes = aps32AllPriorBoxes[i];
            ps32PriorVar = ps32PriorBoxes + u32NumPredsPerClass*SAMPLE_SVP_NNIE_COORDI_NUM;
            for (j = 0; j < u32NumPredsPerClass; j++)
            {
                //printf("ps32PriorBoxes start***************\n");
                f32PriorWidth = (HI_FLOAT)(ps32PriorBoxes[j*SAMPLE_SVP_NNIE_COORDI_NUM+2] - ps32PriorBoxes[j*SAMPLE_SVP_NNIE_COORDI_NUM]);
                f32PriorHeight = (HI_FLOAT)(ps32PriorBoxes[j*SAMPLE_SVP_NNIE_COORDI_NUM+3] - ps32PriorBoxes[j*SAMPLE_SVP_NNIE_COORDI_NUM + 1]);
                f32PriorCenterX = (ps32PriorBoxes[j*SAMPLE_SVP_NNIE_COORDI_NUM+2] + ps32PriorBoxes[j*SAMPLE_SVP_NNIE_COORDI_NUM])*SAMPLE_SVP_NNIE_HALF;
                f32PriorCenterY = (ps32PriorBoxes[j*SAMPLE_SVP_NNIE_COORDI_NUM+3] + ps32PriorBoxes[j*SAMPLE_SVP_NNIE_COORDI_NUM+1])*SAMPLE_SVP_NNIE_HALF;

                f32DecodeBoxCenterX = ((HI_FLOAT)ps32PriorVar[j*SAMPLE_SVP_NNIE_COORDI_NUM]/SAMPLE_SVP_NNIE_QUANT_BASE)*
                    ((HI_FLOAT)ps32LocPreds[j*SAMpLE_SVP_NNIE_POLYGON]/SAMPLE_SVP_NNIE_QUANT_BASE)*f32PriorWidth+f32PriorCenterX;

                f32DecodeBoxCenterY = ((HI_FLOAT)ps32PriorVar[j*SAMPLE_SVP_NNIE_COORDI_NUM+1]/SAMPLE_SVP_NNIE_QUANT_BASE)*
                    ((HI_FLOAT)ps32LocPreds[j*SAMpLE_SVP_NNIE_POLYGON+1]/SAMPLE_SVP_NNIE_QUANT_BASE)*f32PriorHeight+f32PriorCenterY;

                f32DecodeBoxWidth = exp(((HI_FLOAT)ps32PriorVar[j*SAMPLE_SVP_NNIE_COORDI_NUM+2]/SAMPLE_SVP_NNIE_QUANT_BASE)*
                    ((HI_FLOAT)ps32LocPreds[j*SAMpLE_SVP_NNIE_POLYGON+2]/SAMPLE_SVP_NNIE_QUANT_BASE))*f32PriorWidth;

                f32DecodeBoxHeight = exp(((HI_FLOAT)ps32PriorVar[j*SAMPLE_SVP_NNIE_COORDI_NUM+3]/SAMPLE_SVP_NNIE_QUANT_BASE)*
                    ((HI_FLOAT)ps32LocPreds[j*SAMpLE_SVP_NNIE_POLYGON+3]/SAMPLE_SVP_NNIE_QUANT_BASE))*f32PriorHeight;

                //printf("ps32PriorBoxes end***************\n");

                ps32AllDecodeBoxes[u32SrcIdx++] = (HI_S32)(f32DecodeBoxCenterX - f32DecodeBoxWidth * SAMPLE_SVP_NNIE_HALF);
                ps32AllDecodeBoxes[u32SrcIdx++] = (HI_S32)(f32DecodeBoxCenterY - f32DecodeBoxHeight * SAMPLE_SVP_NNIE_HALF);
                ps32AllDecodeBoxes[u32SrcIdx++] = (HI_S32)(f32DecodeBoxCenterX + f32DecodeBoxWidth * SAMPLE_SVP_NNIE_HALF);
                ps32AllDecodeBoxes[u32SrcIdx++] = (HI_S32)(f32DecodeBoxCenterY + f32DecodeBoxHeight * SAMPLE_SVP_NNIE_HALF);
                /* polygon */
                ps32AllDecodeBoxes[u32SrcIdx++] = (HI_S32)(ps32PriorVar[j*SAMPLE_SVP_NNIE_COORDI_NUM]/SAMPLE_SVP_NNIE_QUANT_BASE)*
                    (HI_FLOAT)(ps32LocPreds[j*SAMpLE_SVP_NNIE_POLYGON+ 4]/SAMPLE_SVP_NNIE_QUANT_BASE)*f32PriorWidth +ps32PriorBoxes[j*SAMPLE_SVP_NNIE_COORDI_NUM];
                ps32AllDecodeBoxes[u32SrcIdx++] = (HI_S32)(ps32PriorVar[j*SAMPLE_SVP_NNIE_COORDI_NUM+1]/SAMPLE_SVP_NNIE_QUANT_BASE)*
                    (HI_FLOAT)(ps32LocPreds[j*SAMpLE_SVP_NNIE_POLYGON+ 5]/SAMPLE_SVP_NNIE_QUANT_BASE)*f32PriorHeight+ps32PriorBoxes[j*SAMPLE_SVP_NNIE_COORDI_NUM+1];
                ps32AllDecodeBoxes[u32SrcIdx++] = (HI_S32)(ps32PriorVar[j*SAMPLE_SVP_NNIE_COORDI_NUM]/SAMPLE_SVP_NNIE_QUANT_BASE)*
                    (HI_FLOAT)(ps32LocPreds[j*SAMpLE_SVP_NNIE_POLYGON+ 6]/SAMPLE_SVP_NNIE_QUANT_BASE)*f32PriorWidth +ps32PriorBoxes[j*SAMPLE_SVP_NNIE_COORDI_NUM+2];
                ps32AllDecodeBoxes[u32SrcIdx++] = (HI_S32)(ps32PriorVar[j*SAMPLE_SVP_NNIE_COORDI_NUM+1]/SAMPLE_SVP_NNIE_QUANT_BASE)*
                    (HI_FLOAT)(ps32LocPreds[j*SAMpLE_SVP_NNIE_POLYGON+ 7]/SAMPLE_SVP_NNIE_QUANT_BASE)*f32PriorHeight+ps32PriorBoxes[j*SAMPLE_SVP_NNIE_COORDI_NUM+1];
                ps32AllDecodeBoxes[u32SrcIdx++] = (HI_S32)(ps32PriorVar[j*SAMPLE_SVP_NNIE_COORDI_NUM]/SAMPLE_SVP_NNIE_QUANT_BASE)*
                    (HI_FLOAT)(ps32LocPreds[j*SAMpLE_SVP_NNIE_POLYGON+ 8]/SAMPLE_SVP_NNIE_QUANT_BASE)*f32PriorWidth +ps32PriorBoxes[j*SAMPLE_SVP_NNIE_COORDI_NUM+2];
                ps32AllDecodeBoxes[u32SrcIdx++] = (HI_S32)(ps32PriorVar[j*SAMPLE_SVP_NNIE_COORDI_NUM+1]/SAMPLE_SVP_NNIE_QUANT_BASE)*
                    (HI_FLOAT)(ps32LocPreds[j*SAMpLE_SVP_NNIE_POLYGON+ 9]/SAMPLE_SVP_NNIE_QUANT_BASE)*f32PriorHeight+ps32PriorBoxes[j*SAMPLE_SVP_NNIE_COORDI_NUM+3];
                ps32AllDecodeBoxes[u32SrcIdx++] = (HI_S32)(ps32PriorVar[j*SAMPLE_SVP_NNIE_COORDI_NUM]/SAMPLE_SVP_NNIE_QUANT_BASE)*
                    (HI_FLOAT)(ps32LocPreds[j*SAMpLE_SVP_NNIE_POLYGON+10]/SAMPLE_SVP_NNIE_QUANT_BASE)*f32PriorWidth +ps32PriorBoxes[j*SAMPLE_SVP_NNIE_COORDI_NUM];
                ps32AllDecodeBoxes[u32SrcIdx++] = (HI_S32)(ps32PriorVar[j*SAMPLE_SVP_NNIE_COORDI_NUM+1]/SAMPLE_SVP_NNIE_QUANT_BASE)*
                    (HI_FLOAT)(ps32LocPreds[j*SAMpLE_SVP_NNIE_POLYGON+11]/SAMPLE_SVP_NNIE_QUANT_BASE)*f32PriorHeight+ps32PriorBoxes[j*SAMPLE_SVP_NNIE_COORDI_NUM+3];
            }
        }
    }
    //fprintf(stderr, "detection1 end.\n");
    /********** do NMS for each class *************/
    u32AfterTopK = 0;
    for (i = 0; i < u32ClassNum; i++)
    {
        #pragma omp parallel for
        for (j = 0; j < u32PriorNum; j++)
        {
            ps32SingleProposal[j * SAMPLE_SVP_NNIE_PROPOSAL_WIDTH] = ps32AllDecodeBoxes[j * SAMpLE_SVP_NNIE_POLYGON];
            ps32SingleProposal[j * SAMPLE_SVP_NNIE_PROPOSAL_WIDTH + 1] = ps32AllDecodeBoxes[j * SAMpLE_SVP_NNIE_POLYGON + 1];
            ps32SingleProposal[j * SAMPLE_SVP_NNIE_PROPOSAL_WIDTH + 2] = ps32AllDecodeBoxes[j * SAMpLE_SVP_NNIE_POLYGON + 2];
            ps32SingleProposal[j * SAMPLE_SVP_NNIE_PROPOSAL_WIDTH + 3] = ps32AllDecodeBoxes[j * SAMpLE_SVP_NNIE_POLYGON + 3];
            ps32SingleProposal[j * SAMPLE_SVP_NNIE_PROPOSAL_WIDTH + 4] = ps32AllDecodeBoxes[j * SAMpLE_SVP_NNIE_POLYGON + 4];
            ps32SingleProposal[j * SAMPLE_SVP_NNIE_PROPOSAL_WIDTH + 5] = ps32AllDecodeBoxes[j * SAMpLE_SVP_NNIE_POLYGON + 5];
            ps32SingleProposal[j * SAMPLE_SVP_NNIE_PROPOSAL_WIDTH + 6] = ps32AllDecodeBoxes[j * SAMpLE_SVP_NNIE_POLYGON + 6];
            ps32SingleProposal[j * SAMPLE_SVP_NNIE_PROPOSAL_WIDTH + 7] = ps32AllDecodeBoxes[j * SAMpLE_SVP_NNIE_POLYGON + 7];
            ps32SingleProposal[j * SAMPLE_SVP_NNIE_PROPOSAL_WIDTH + 8] = ps32AllDecodeBoxes[j * SAMpLE_SVP_NNIE_POLYGON + 8];
            ps32SingleProposal[j * SAMPLE_SVP_NNIE_PROPOSAL_WIDTH + 9] = ps32AllDecodeBoxes[j * SAMpLE_SVP_NNIE_POLYGON + 9];
            ps32SingleProposal[j * SAMPLE_SVP_NNIE_PROPOSAL_WIDTH +10] = ps32AllDecodeBoxes[j * SAMpLE_SVP_NNIE_POLYGON +10];
            ps32SingleProposal[j * SAMPLE_SVP_NNIE_PROPOSAL_WIDTH +11] = ps32AllDecodeBoxes[j * SAMpLE_SVP_NNIE_POLYGON +11];
            ps32SingleProposal[j * SAMPLE_SVP_NNIE_PROPOSAL_WIDTH +12] = ps32ConfScores[j*u32ClassNum + i];
            // fprintf(stderr, "class%d --- no.%d   conf: %d\n", i, j*u32ClassNum + i, ps32SingleProposal[j * SAMPLE_SVP_NNIE_PROPOSAL_WIDTH +12]);
            ps32SingleProposal[j * SAMPLE_SVP_NNIE_PROPOSAL_WIDTH +13] = 0;
        }
        s32Ret = SVP_NNIE_NonRecursiveArgQuickSort(ps32SingleProposal, 0, u32PriorNum - 1, pstStack,u32TopK);
        u32AfterFilter = (u32PriorNum < u32TopK) ? u32PriorNum : u32TopK;
        s32Ret = SVP_NNIE_NonMaxSuppression(ps32SingleProposal, u32AfterFilter, u32NmsThresh, u32AfterFilter);
        u32RoiOutCnt = 0;
        ps32DstScore = (HI_S32*)ps32DstScoreSrc;
        ps32DstBbox = (HI_S32*)ps32DstBboxSrc;
        ps32ClassRoiNum = (HI_S32*)ps32RoiOutCntSrc;
        ps32DstScore += (HI_S32)u32AfterTopK;
        ps32DstBbox += (HI_S32)(u32AfterTopK * SAMpLE_SVP_NNIE_POLYGON);
        for (j = 0; j < u32TopK; j++)
        {
            if (ps32SingleProposal[j * SAMPLE_SVP_NNIE_PROPOSAL_WIDTH + 13] == 0 &&
                ps32SingleProposal[j * SAMPLE_SVP_NNIE_PROPOSAL_WIDTH + 12] > (HI_S32)u32NmsThresh)
            {
                ps32DstScore[u32RoiOutCnt] = ps32SingleProposal[j * 14 + 12];
                //fprintf(stderr, "score: %d\n", ps32DstScore[u32RoiOutCnt]);
                ps32DstBbox[u32RoiOutCnt * SAMpLE_SVP_NNIE_POLYGON] = ps32SingleProposal[j * SAMPLE_SVP_NNIE_PROPOSAL_WIDTH];
                ps32DstBbox[u32RoiOutCnt * SAMpLE_SVP_NNIE_POLYGON + 1] = ps32SingleProposal[j * SAMPLE_SVP_NNIE_PROPOSAL_WIDTH + 1];
                ps32DstBbox[u32RoiOutCnt * SAMpLE_SVP_NNIE_POLYGON + 2] = ps32SingleProposal[j * SAMPLE_SVP_NNIE_PROPOSAL_WIDTH + 2];
                ps32DstBbox[u32RoiOutCnt * SAMpLE_SVP_NNIE_POLYGON + 3] = ps32SingleProposal[j * SAMPLE_SVP_NNIE_PROPOSAL_WIDTH + 3];
                ps32DstBbox[u32RoiOutCnt * SAMpLE_SVP_NNIE_POLYGON + 4] = ps32SingleProposal[j * SAMPLE_SVP_NNIE_PROPOSAL_WIDTH + 4];
                ps32DstBbox[u32RoiOutCnt * SAMpLE_SVP_NNIE_POLYGON + 5] = ps32SingleProposal[j * SAMPLE_SVP_NNIE_PROPOSAL_WIDTH + 5];
                ps32DstBbox[u32RoiOutCnt * SAMpLE_SVP_NNIE_POLYGON + 6] = ps32SingleProposal[j * SAMPLE_SVP_NNIE_PROPOSAL_WIDTH + 6];
                ps32DstBbox[u32RoiOutCnt * SAMpLE_SVP_NNIE_POLYGON + 7] = ps32SingleProposal[j * SAMPLE_SVP_NNIE_PROPOSAL_WIDTH + 7];
                ps32DstBbox[u32RoiOutCnt * SAMpLE_SVP_NNIE_POLYGON + 8] = ps32SingleProposal[j * SAMPLE_SVP_NNIE_PROPOSAL_WIDTH + 8];
                ps32DstBbox[u32RoiOutCnt * SAMpLE_SVP_NNIE_POLYGON + 9] = ps32SingleProposal[j * SAMPLE_SVP_NNIE_PROPOSAL_WIDTH + 9];
                ps32DstBbox[u32RoiOutCnt * SAMpLE_SVP_NNIE_POLYGON +10] = ps32SingleProposal[j * SAMPLE_SVP_NNIE_PROPOSAL_WIDTH +10];
                ps32DstBbox[u32RoiOutCnt * SAMpLE_SVP_NNIE_POLYGON +11] = ps32SingleProposal[j * SAMPLE_SVP_NNIE_PROPOSAL_WIDTH +11];
                u32RoiOutCnt++;
            }
        }
        ps32ClassRoiNum[i] = (HI_S32)u32RoiOutCnt;
        //fprintf(stderr, "no.%d class roi num: %d\n", i, ps32ClassRoiNum[i]);
        u32AfterTopK += u32RoiOutCnt;
    }

    u32KeepCnt = 0;
    u32Offset = 0;
    if (u32AfterTopK > u32KeepTopK)
    {
        u32Offset = ps32ClassRoiNum[0];
        for (i = 1; i < u32ClassNum; i++)
        {
            ps32DstScore = (HI_S32*)ps32DstScoreSrc;
            ps32DstBbox = (HI_S32*)ps32DstBboxSrc;
            ps32ClassRoiNum = (HI_S32*)ps32RoiOutCntSrc;
            ps32DstScore += (HI_S32)(u32Offset);
            ps32DstBbox += (HI_S32)(u32Offset * SAMpLE_SVP_NNIE_POLYGON);
            for (j = 0; j < (HI_U32)ps32ClassRoiNum[i]; j++)
            {
                ps32AfterTopK[u32KeepCnt * SAMPLE_SVP_NNIE_PROPOSAL_WIDTH] = ps32DstBbox[j * SAMpLE_SVP_NNIE_POLYGON];
                ps32AfterTopK[u32KeepCnt * SAMPLE_SVP_NNIE_PROPOSAL_WIDTH + 1] = ps32DstBbox[j * SAMpLE_SVP_NNIE_POLYGON + 1];
                ps32AfterTopK[u32KeepCnt * SAMPLE_SVP_NNIE_PROPOSAL_WIDTH + 2] = ps32DstBbox[j * SAMpLE_SVP_NNIE_POLYGON + 2];
                ps32AfterTopK[u32KeepCnt * SAMPLE_SVP_NNIE_PROPOSAL_WIDTH + 3] = ps32DstBbox[j * SAMpLE_SVP_NNIE_POLYGON + 3];
                ps32AfterTopK[u32KeepCnt * SAMPLE_SVP_NNIE_PROPOSAL_WIDTH + 4] = ps32DstBbox[j * SAMpLE_SVP_NNIE_POLYGON + 4];
                ps32AfterTopK[u32KeepCnt * SAMPLE_SVP_NNIE_PROPOSAL_WIDTH + 5] = ps32DstBbox[j * SAMpLE_SVP_NNIE_POLYGON + 5];
                ps32AfterTopK[u32KeepCnt * SAMPLE_SVP_NNIE_PROPOSAL_WIDTH + 6] = ps32DstBbox[j * SAMpLE_SVP_NNIE_POLYGON + 6];
                ps32AfterTopK[u32KeepCnt * SAMPLE_SVP_NNIE_PROPOSAL_WIDTH + 7] = ps32DstBbox[j * SAMpLE_SVP_NNIE_POLYGON + 7];
                ps32AfterTopK[u32KeepCnt * SAMPLE_SVP_NNIE_PROPOSAL_WIDTH + 8] = ps32DstBbox[j * SAMpLE_SVP_NNIE_POLYGON + 8];
                ps32AfterTopK[u32KeepCnt * SAMPLE_SVP_NNIE_PROPOSAL_WIDTH + 9] = ps32DstBbox[j * SAMpLE_SVP_NNIE_POLYGON + 9];
                ps32AfterTopK[u32KeepCnt * SAMPLE_SVP_NNIE_PROPOSAL_WIDTH +10] = ps32DstBbox[j * SAMpLE_SVP_NNIE_POLYGON +10];
                ps32AfterTopK[u32KeepCnt * SAMPLE_SVP_NNIE_PROPOSAL_WIDTH +11] = ps32DstBbox[j * SAMpLE_SVP_NNIE_POLYGON +11];
                ps32AfterTopK[u32KeepCnt * SAMPLE_SVP_NNIE_PROPOSAL_WIDTH +12] = ps32DstScore[j];
                ps32AfterTopK[u32KeepCnt * SAMPLE_SVP_NNIE_PROPOSAL_WIDTH +13] = i;
                u32KeepCnt++;
            }
            u32Offset = u32Offset + ps32ClassRoiNum[i];
            //fprintf(stderr, "KeepCnt: %d\n", u32KeepCnt);
        }
        s32Ret = SVP_NNIE_NonRecursiveArgQuickSort(ps32AfterTopK, 0, u32KeepCnt - 1, pstStack,u32KeepCnt);

        u32Offset = 0;
        u32Offset = ps32ClassRoiNum[0];
        for (i = 1; i < u32ClassNum; i++)
        {
            u32RoiOutCnt = 0;
            ps32DstScore = (HI_S32*)ps32DstScoreSrc;
            ps32DstBbox = (HI_S32*)ps32DstBboxSrc;
            ps32ClassRoiNum = (HI_S32*)ps32RoiOutCntSrc;
            ps32DstScore += (HI_S32)(u32Offset);
            ps32DstBbox += (HI_S32)(u32Offset * SAMpLE_SVP_NNIE_POLYGON);
            for (j = 0; j < u32KeepTopK; j++)
            {
                if (ps32AfterTopK[j * SAMPLE_SVP_NNIE_PROPOSAL_WIDTH + 13] == i)
                {
                    ps32DstScore[u32RoiOutCnt] = ps32AfterTopK[j * SAMPLE_SVP_NNIE_PROPOSAL_WIDTH + 12];
                    ps32DstBbox[u32RoiOutCnt * SAMpLE_SVP_NNIE_POLYGON] = ps32AfterTopK[j * SAMPLE_SVP_NNIE_PROPOSAL_WIDTH];
                    ps32DstBbox[u32RoiOutCnt * SAMpLE_SVP_NNIE_POLYGON + 1] = ps32AfterTopK[j * SAMPLE_SVP_NNIE_PROPOSAL_WIDTH + 1];
                    ps32DstBbox[u32RoiOutCnt * SAMpLE_SVP_NNIE_POLYGON + 2] = ps32AfterTopK[j * SAMPLE_SVP_NNIE_PROPOSAL_WIDTH + 2];
                    ps32DstBbox[u32RoiOutCnt * SAMpLE_SVP_NNIE_POLYGON + 3] = ps32AfterTopK[j * SAMPLE_SVP_NNIE_PROPOSAL_WIDTH + 3];
                    ps32DstBbox[u32RoiOutCnt * SAMpLE_SVP_NNIE_POLYGON + 4] = ps32AfterTopK[j * SAMPLE_SVP_NNIE_PROPOSAL_WIDTH + 4];
                    ps32DstBbox[u32RoiOutCnt * SAMpLE_SVP_NNIE_POLYGON + 5] = ps32AfterTopK[j * SAMPLE_SVP_NNIE_PROPOSAL_WIDTH + 5];
                    ps32DstBbox[u32RoiOutCnt * SAMpLE_SVP_NNIE_POLYGON + 6] = ps32AfterTopK[j * SAMPLE_SVP_NNIE_PROPOSAL_WIDTH + 6];
                    ps32DstBbox[u32RoiOutCnt * SAMpLE_SVP_NNIE_POLYGON + 7] = ps32AfterTopK[j * SAMPLE_SVP_NNIE_PROPOSAL_WIDTH + 7];
                    ps32DstBbox[u32RoiOutCnt * SAMpLE_SVP_NNIE_POLYGON + 8] = ps32AfterTopK[j * SAMPLE_SVP_NNIE_PROPOSAL_WIDTH + 8];
                    ps32DstBbox[u32RoiOutCnt * SAMpLE_SVP_NNIE_POLYGON + 9] = ps32AfterTopK[j * SAMPLE_SVP_NNIE_PROPOSAL_WIDTH + 9];
                    ps32DstBbox[u32RoiOutCnt * SAMpLE_SVP_NNIE_POLYGON +10] = ps32AfterTopK[j * SAMPLE_SVP_NNIE_PROPOSAL_WIDTH +10];
                    ps32DstBbox[u32RoiOutCnt * SAMpLE_SVP_NNIE_POLYGON +11] = ps32AfterTopK[j * SAMPLE_SVP_NNIE_PROPOSAL_WIDTH +11];
                    u32RoiOutCnt++;
                }
            }
            ps32ClassRoiNum[i] = (HI_S32)u32RoiOutCnt;
            u32Offset += u32RoiOutCnt;
            //fprintf(stderr, "ps32ClassNum: %d\n", ps32ClassRoiNum[i]);
        }
    }
    return s32Ret;
}

/*****************************************************************************
* Prototype :   SAMPLE_SVP_NNIE_Ssd_GetResultTmpBuf
* Description : this function is used to Get SSD GetResult tmp buffer size
* Input :     SAMPLE_SVP_NNIE_PARAM_S*               pstNnieParam     [IN]  the pointer to SSD NNIE parameter
*              SAMPLE_SVP_NNIE_SSD_SOFTWARE_PARAM_S*   pstSoftwareParam [IN]  the pointer to SSD software parameter
*
*
*
*
* Output :
* Return Value : HI_SUCCESS: Success;Error codes: Failure.
* Spec :
* Calls :
* Called By :
* History:
*
* 1. Date : 2017-11-10
* Author :
* Modification : Create
*
*****************************************************************************/
HI_U32 SAMPLE_SVP_NNIE_Ssd_GetResultTmpBuf(SAMPLE_SVP_NNIE_PARAM_S*pstNnieParam,
    SAMPLE_SVP_NNIE_SSD_SOFTWARE_PARAM_S* pstSoftwareParam)
{
    HI_U32 u32PriorBoxSize = 0;
    HI_U32 u32SoftMaxSize = 0;
    HI_U32 u32DetectionSize = 0;
    HI_U32 u32TotalSize = 0;
    HI_U32 u32PriorNum = 0;
    HI_U32 i = 0;

    /*priorbox size*/
    for(i = 0; i < pstNnieParam->pstModel->astSeg[0].u16DstNum/2; i++)
    {
        u32PriorBoxSize += pstSoftwareParam->au32PriorBoxHeight[i]*pstSoftwareParam->au32PriorBoxWidth[i]*
            SAMPLE_SVP_NNIE_COORDI_NUM*2*(pstSoftwareParam->u32MaxSizeNum+pstSoftwareParam->u32MinSizeNum+
            pstSoftwareParam->au32InputAspectRatioNum[i]*2*pstSoftwareParam->u32MinSizeNum)*sizeof(HI_U32) *2; //*2->textboxes
    }
    pstSoftwareParam->stPriorBoxTmpBuf.u32Size = u32PriorBoxSize;
    u32TotalSize+=u32PriorBoxSize;

    /*softmax size*/
    for(i = 0; i < pstSoftwareParam->u32ConcatNum; i++)
    {
        u32SoftMaxSize += pstSoftwareParam->au32SoftMaxInChn[i]*sizeof(HI_U32);
    }
    pstSoftwareParam->stSoftMaxTmpBuf.u32Size = u32SoftMaxSize;
    u32TotalSize+=u32SoftMaxSize;

    /*detection size*/
    for(i = 0; i < pstSoftwareParam->u32ConcatNum; i++)
    {
        u32PriorNum+=pstSoftwareParam->au32DetectInputChn[i]/SAMpLE_SVP_NNIE_POLYGON;
    }
    u32DetectionSize+=u32PriorNum*SAMpLE_SVP_NNIE_POLYGON*sizeof(HI_U32);
    u32DetectionSize+=u32PriorNum*SAMPLE_SVP_NNIE_PROPOSAL_WIDTH*sizeof(HI_U32)*2;
    u32DetectionSize+=u32PriorNum*2*sizeof(HI_U32);
    pstSoftwareParam->stGetResultTmpBuf.u32Size = u32DetectionSize;
    u32TotalSize+=u32DetectionSize;

    return u32TotalSize;
}


/*****************************************************************************
* Prototype :   SAMPLE_SVP_NNIE_Ssd_GetResult
* Description : this function is used to Get SSD result
* Input :     SAMPLE_SVP_NNIE_PARAM_S*               pstNnieParam     [IN]  the pointer to SSD NNIE parameter
*              SAMPLE_SVP_NNIE_SSD_SOFTWARE_PARAM_S*   pstSoftwareParam [IN]  the pointer to SSD software parameter
*
*
*
*
* Output :
* Return Value : HI_SUCCESS: Success;Error codes: Failure.
* Spec :
* Calls :
* Called By :
* History:
*
* 1. Date : 2017-11-10
* Author :
* Modification : Create
*
*****************************************************************************/
HI_S32 SAMPLE_SVP_NNIE_Ssd_GetResult(SAMPLE_SVP_NNIE_PARAM_S*pstNnieParam,
    SAMPLE_SVP_NNIE_SSD_SOFTWARE_PARAM_S* pstSoftwareParam)
{
    HI_S32* aps32PermuteResult[SAMPLE_SVP_NNIE_SSD_REPORT_NODE_NUM];
    HI_S32* aps32PriorboxOutputData[SAMPLE_SVP_NNIE_SSD_PRIORBOX_NUM];
    HI_S32* aps32SoftMaxInputData[SAMPLE_SVP_NNIE_SSD_SOFTMAX_NUM];
    HI_S32* aps32DetectionLocData[SAMPLE_SVP_NNIE_SSD_SOFTMAX_NUM];
    HI_S32* ps32SoftMaxOutputData = NULL;
    HI_S32* ps32DetectionOutTmpBuf = NULL;
    HI_U32 u32Size = 0;
    HI_S32 s32Ret = HI_SUCCESS;
    HI_U32 i = 0;

    /* 自己实现的CPU卷积 */
    HI_S32* aps32ConvReport;
    aps32ConvReport = (HI_S32*)pstNnieParam->astSegData[0].astDst[0].u64VirAddr;    //fc7

    Convolution layers[2];
    Convolution *pLayer;
    pLayer = &layers[0];
    int pload = load_param_fp("nnie.param", pLayer);
    if (pload != 0)
    {
        fprintf(stderr, "load_param_fp error!\n");
        return -1;
    }

    load_model_fp("nnie.bin", pLayer);

    int FeatureMapW = 24;
    int FeatureMapH = 24;
    int FeatureMapC[2] = {240, 40};
    float *PermuteData[2];
    size_t sizePermute1 = FeatureMapW*FeatureMapH*FeatureMapC[0]*sizeof(float);
    size_t sizePermute2 = FeatureMapW*FeatureMapH*FeatureMapC[1]*sizeof(float);

    PermuteData[0] = (float*)malloc(sizePermute1);
    PermuteData[1] = (float*)malloc(sizePermute2);
    memset(PermuteData[0], 0, sizePermute1);
    memset(PermuteData[1], 0, sizePermute2);

    float *output_temp[2];
    output_temp[0] = (float*)malloc(sizePermute1);
    memset(output_temp[0], 0, sizePermute1);
    output_temp[1] = (float*)malloc(sizePermute2);
    memset(output_temp[0], 0, sizePermute2);

    forward_conv_compile(aps32ConvReport, output_temp[0], &layers[0]);
    permute(output_temp[0], PermuteData[0], 0, layers[0].w, layers[0].h, layers[0].num_output);
    forward_conv_compile(aps32ConvReport, output_temp[1], &layers[1]);
    permute(output_temp[1], PermuteData[1], 0, layers[1].w, layers[1].h, layers[1].num_output);

    free(output_temp[0]);
    free(output_temp[1]);

    //fprintf(stderr, "forward by myself end.\n");

    /*get permut result*/
    for(i = 0; i < 2; i++)
    {
        aps32PermuteResult[i] = (HI_S32*)pstNnieParam->astSegData[0].astDst[i+1].u64VirAddr;
    }

    for(i = 4; i < SAMPLE_SVP_NNIE_SSD_REPORT_NODE_NUM; i++)
    {
        aps32PermuteResult[i] = (HI_S32*)pstNnieParam->astSegData[0].astDst[i-1].u64VirAddr;
    }

    //fprintf(stderr, "priorbox in.\n");
    /*priorbox*/
    aps32PriorboxOutputData[0] = (HI_S32*)pstSoftwareParam->stPriorBoxTmpBuf.u64VirAddr;
    for (i = 1; i < SAMPLE_SVP_NNIE_SSD_PRIORBOX_NUM; i++)
    {
        u32Size = pstSoftwareParam->au32PriorBoxHeight[i-1]*pstSoftwareParam->au32PriorBoxWidth[i-1]*
            SAMPLE_SVP_NNIE_COORDI_NUM*2*(pstSoftwareParam->u32MaxSizeNum+pstSoftwareParam->u32MinSizeNum+
            pstSoftwareParam->au32InputAspectRatioNum[i-1]*2*pstSoftwareParam->u32MinSizeNum) *2;
        aps32PriorboxOutputData[i] = aps32PriorboxOutputData[i - 1] + u32Size;
    }

    for (i = 0; i < SAMPLE_SVP_NNIE_SSD_PRIORBOX_NUM; i++)
    {
        // FIXME:
        s32Ret = SVP_NNIE_Ssd_PriorBoxForward(pstSoftwareParam->au32PriorBoxWidth[i],
            pstSoftwareParam->au32PriorBoxHeight[i], pstSoftwareParam->u32OriImWidth,
            pstSoftwareParam->u32OriImHeight, pstSoftwareParam->af32PriorBoxMinSize[i],
            pstSoftwareParam->u32MinSizeNum,pstSoftwareParam->af32PriorBoxMaxSize[i],
            pstSoftwareParam->u32MaxSizeNum, pstSoftwareParam->bFlip, pstSoftwareParam->bClip,
            pstSoftwareParam->au32InputAspectRatioNum[i],pstSoftwareParam->af32PriorBoxAspectRatio[i],
            pstSoftwareParam->af32PriorBoxStepWidth[i],pstSoftwareParam->af32PriorBoxStepHeight[i],
            pstSoftwareParam->f32Offset,pstSoftwareParam->as32PriorBoxVar,
            aps32PriorboxOutputData[i]);
        SAMPLE_SVP_CHECK_EXPR_RET(HI_SUCCESS != s32Ret,s32Ret,SAMPLE_SVP_ERR_LEVEL_ERROR,
            "Error,SVP_NNIE_Ssd_PriorBoxForward failed!\n");
    }

    //fprintf(stderr, "softmax in.\n");
    /*softmax*/
    ps32SoftMaxOutputData = (HI_S32*)pstSoftwareParam->stSoftMaxTmpBuf.u64VirAddr;
    for(i = 0; i < SAMPLE_SVP_NNIE_SSD_SOFTMAX_NUM; i++)
    {
        if ( i == 1)
            continue;
        aps32SoftMaxInputData[i] = aps32PermuteResult[i*2+1];
    }
    // FIXME:
    (void)SVP_NNIE_Ssd_SoftmaxForward(pstSoftwareParam->u32SoftMaxInHeight,
        pstSoftwareParam->au32SoftMaxInChn, pstSoftwareParam->u32ConcatNum,
        pstSoftwareParam->au32ConvStride, pstSoftwareParam->u32SoftMaxOutWidth,
        pstSoftwareParam->u32SoftMaxOutHeight, pstSoftwareParam->u32SoftMaxOutChn,
        aps32SoftMaxInputData, ps32SoftMaxOutputData, PermuteData);

    //fprintf(stderr, "detection in.\n");
    /*detection*/
    ps32DetectionOutTmpBuf = (HI_S32*)pstSoftwareParam->stGetResultTmpBuf.u64VirAddr;
    for(i = 0; i < SAMPLE_SVP_NNIE_SSD_PRIORBOX_NUM; i++)
    {
        if ( i == 1)
            continue;
        aps32DetectionLocData[i] = aps32PermuteResult[i*2];
    }

    // FIXME:
    (void)SVP_NNIE_Ssd_DetectionOutForward(pstSoftwareParam->u32ConcatNum,
        pstSoftwareParam->u32ConfThresh,pstSoftwareParam->u32ClassNum, pstSoftwareParam->u32TopK,
        pstSoftwareParam->u32KeepTopK, pstSoftwareParam->u32NmsThresh,pstSoftwareParam->au32DetectInputChn,
        aps32DetectionLocData, aps32PriorboxOutputData, ps32SoftMaxOutputData,
        ps32DetectionOutTmpBuf, (HI_S32*)pstSoftwareParam->stDstScore.u64VirAddr,
        (HI_S32*)pstSoftwareParam->stDstRoi.u64VirAddr,
        (HI_S32*)pstSoftwareParam->stClassRoiNum.u64VirAddr, PermuteData);

    return s32Ret;
}
#ifdef __cplusplus
}
#endif
