//
// Created by Yangzhiyong on 2025/3/13.
//

#ifndef RECORDREADER_H
#define RECORDREADER_H

#include "duilib/duilib.h"
#include <fstream>
#include "Base.h"

typedef std::function<bool(int, const unsigned char*, size_t, PStFrameSeiInfo)> RecordImageReadyCallback;
typedef std::function<bool()> RecordUICallback;

class RecordReader {
public:
    RecordReader();
    ~RecordReader();

    void Start();
    void Stop();

    void Play(bool _bPlay);
    bool IsPlaying();

    void SetFPS(int _iFps);

    void SetImageReadyCallback(const RecordImageReadyCallback& _imageReadyCallback);
    void SetRecordUICallback(const RecordUICallback& _recordUICallback);
    void SetBPrintDebugInfo(bool _bPrintDebugInfo);

private:
    void Run();
    void inline PrintfDebugInfo(const DString& _strMessage) const;


    bool m_bExitRecordPlay;
    bool m_bPause;
    uint32_t m_u32Duration; //录像播放帧间隔 毫秒

    std::shared_ptr<std::thread> m_pPtrRecordPlayThread;
    std::shared_ptr<std::ifstream> m_pPtrRecordFileStream;

    RecordImageReadyCallback m_fncImageReadyCallback;
    RecordUICallback m_fncRecordUICallback;
    bool m_bPrintDebugInfo;

    uint32_t m_u32ImageDataLength; // 通道的数据大小
    uint8_t* m_pByImageData; // 通道的数据缓存
    StFrameSeiInfo m_stFrameSeiInfo; // 通道的识别信息
};



#endif //RECORDREADER_H
