//
// Created by Yangzhiyong on 2025/3/13.
//

#ifndef BASE_H
#define BASE_H

#define MAX_STRLEN 256
#define OBJ_TX_MAX_COUNTS 20

typedef struct
{
    int x;
    int y;
    int w;
    int h;
    int label;
    float score;
} stObjInfo;

typedef struct
{
    int x;
    int y;
    int w;
    int h;
    int id;
} stObjInfoTrack;

typedef struct
{
    int count;
    int countTrack;
    stObjInfo objs[OBJ_TX_MAX_COUNTS];
    stObjInfoTrack objsTrack[OBJ_TX_MAX_COUNTS];
} stYoloV8Objs_tx;

struct list_head {
    list_head *next, *prev;
};

typedef struct
{
    list_head head; // 节点头
    uint32_t uFrameNumber;    // 识别图像帧号，帧号0表示数据无效，>0表示已经赋值
    uint64_t u64FramePts; // 帧时间戳
    uint32_t byFormat; // 图像数据格式 用u32是凑4个倍数字节，以免在不同平台系统中传输错误
    uint32_t uWidth; // 图像宽
    uint32_t uHeight; // 图像高
    uint64_t u64FrameDuration; //帧间时差 微秒
    uint64_t u64RecognitionDuration; //识别耗时 微秒
    uint64_t u64VideoDuration; // 视频帧时差 微秒
    stYoloV8Objs_tx stResult; // 识别结果
} StFrameResultNode, *PStFrameResultNode; // 帧结果信息  可以写入图像第一行

typedef struct {
    uint32_t uFrameNumber; // 帧号，系统生成的，贯穿整个SOC处理生命周期，是偶数
    uint64_t uPts; // 时间戳，微秒
    uint32_t uWidth; // 图像宽
    uint32_t uHeight; // 图像高
    uint32_t otPixelFormat; // 像素格式，枚举类型
    uint32_t otVideoFormat; // 视频格式，枚举类型
    uint32_t otCompressMode; //压缩格式，枚举类型
} StSrcFrameInfo, *PStSrcFrameInfo; // 视频帧原始信息

typedef struct {
    uint32_t uSenorType; // 设备类型， 0可见光 1红外
    uint32_t uVencFrameNumber; //VENC给的自定义帧号
    uint64_t uVencFramePts; // VENC的时间戳
    uint32_t uVencFrameDuration; // VENC的帧间隔
    StSrcFrameInfo stSrcFrameInfo; // 当前帧信息 VPSS
    StFrameResultNode stFrameResultNode; // 识别结果信息
} StFrameSeiInfo, *PStFrameSeiInfo;



#endif //BASE_H
