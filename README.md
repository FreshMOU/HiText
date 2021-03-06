# 1.16更新

将手势识别和文字检测代码整合成一个库。

此时手势识别输入为384x384，文字检测输入为384x384。（该项目还未添加GPU代码实现）

### TODO

两个检测的同时768x768实现

# 编译成库

`make`可以直接生成静态库libhitbpp.a

# 使用说明

本库提供了3个函数，作用分别如下所示。

```
// 初始化，用于加载NNIE模型
void DKMultiClassDetectionInit();

// 运行NNIE执行检测，得到检测结果
DKSMultiDetectionRes DKMultiClassDetectionProcess(char * imgfilename);

// 释放NNIE资源
void DKMultiClassDetectionEnd();

// 手势识别
DKSMultiDetectionRes DKHandProcess(char * imgfilename);
```

结构体定义如下

```
#define DKMAXBOXNUM 200

typedef struct
{
    int x1;
    int y1;
    int x2;
    int y2;
    int x3;
    int y3;
    int x4;
    int y4; //四边形坐标
    int xmin; 
    int ymin;
    int xmax;
    int ymax;
    
    float score;
}DKSBox;

typedef struct
{
    int num; // 当前检测出的物体总数目
    DKSBox boxes[DKMAXBOXNUM];
}DKSMultiDetectionRes;
```

[获取使用示例](https://github.com/FreshMOU/HiText/releases/download/v1.1/libHiText.zip)  
注意放在mpp/sample目录下可用，如果要放到其他目录，可以修改Makefile.param  
[jpg文件读取使用示例](https://github.com/FreshMOU/HiText/releases/download/v1.1/libHiText2.zip)  
[手势识别示例](https://github.com/FreshMOU/HiText/releases/download/v1.1/libHAND.zip)

release中有模型文件和示例图片，需要的是textboxes_fc7.wk，fc7.bin，fc7.parm，text_384x384.bgr文件。  
下载下来与编译好的hitest放在板子的同一个位置执行`./hitest text_384x384.bgr recog_300x300.rgb`即可  
注意: fc7.bin和fc7.param需要重命名为nnie.bin和nnie.param。

