#ifndef HI_TBPP_H
#define HI_TBPP_H

#include "sample_nnie_main.h"

// 初始化，用于加载NNIE模型
void DKMultiClassDetectionInit();

// 运行NNIE执行检测，得到检测结果
DKSMultiDetectionRes DKMultiClassDetectionProcess(char * imgfilename, float f32PrintResultThresh);

// 释放NNIE资源
void DKMultiClassDetectionEnd();

// 手势识别
DKSMultiDetectionRes DKHandProcess(char * imgfilename, float f32PrintResultThresh);


#endif