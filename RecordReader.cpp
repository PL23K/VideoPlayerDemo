//
// Created by Yangzhiyong on 2025/3/13.
//

#include "RecordReader.h"
#include "LogMessage.h"

RecordReader::RecordReader():m_bExitRecordPlay(false), m_bPause(true),
                             m_u32Duration(20), m_pPtrRecordPlayThread(nullptr),
                             m_pPtrRecordFileStream(nullptr),
                             m_fncImageReadyCallback(nullptr),
                             m_bPrintDebugInfo(true),
                             m_u32ImageDataLength(640*512),
                             m_pByImageData(nullptr){

    m_pByImageData = new uint8_t[m_u32ImageDataLength*3]; // 测试图有3张图，一次性读取
    memset(m_pByImageData, 0, m_u32ImageDataLength*3);

    memset(&m_stFrameSeiInfo, 0, sizeof(StFrameSeiInfo));

}

RecordReader::~RecordReader(){
    Stop();

    delete[] m_pByImageData;
    m_pByImageData = nullptr;
}

void RecordReader::Start(){

    if (m_pPtrRecordFileStream != nullptr) {
        m_pPtrRecordFileStream->close();
        m_pPtrRecordFileStream = nullptr;
    }
    m_pPtrRecordFileStream = std::make_shared<std::ifstream>("./video_640x512.raw", std::ifstream::binary);

    // 创建线程
    m_pPtrRecordPlayThread = std::make_shared<std::thread>(&RecordReader::Run, this);
}

void RecordReader::Stop(){

    m_bExitRecordPlay = true;
    m_bPause = true;

    if(m_pPtrRecordPlayThread != nullptr){
        if(m_pPtrRecordPlayThread->joinable()){
            m_pPtrRecordPlayThread->join();
        }
        m_pPtrRecordPlayThread = nullptr;
    }

    if (m_pPtrRecordFileStream != nullptr) {
        m_pPtrRecordFileStream->close();
        m_pPtrRecordFileStream = nullptr;
    }
}

void RecordReader::Play(const bool _bPlay) {
    m_bPause = !_bPlay;
}

void RecordReader::SetImageReadyCallback(const RecordImageReadyCallback& _imageReadyCallback)
{
    m_fncImageReadyCallback = _imageReadyCallback;
}

void RecordReader::SetRecordUICallback(const RecordUICallback& _recordUICallback)
{
    m_fncRecordUICallback = _recordUICallback;
}

bool RecordReader::IsPlaying() {
    return !m_bPause;
}

void RecordReader::SetBPrintDebugInfo(bool _bPrintDebugInfo) {
    m_bPrintDebugInfo = _bPrintDebugInfo;
}

void inline RecordReader::PrintfDebugInfo(const DString& _strMessage) const {
    if (m_bPrintDebugInfo){
        LogMessage(_strMessage.c_str());
    }
}

void RecordReader::SetFPS(const int _iFps) {
    if (_iFps > 0) {
        m_u32Duration = static_cast<uint32_t>(round( 1000.0 / _iFps));
    }
}

void RecordReader::Run() {
  auto start = std::chrono::high_resolution_clock::now();
  auto end = std::chrono::high_resolution_clock::now();

  uint16_t u16StSeiLength = sizeof(StFrameSeiInfo);
  // 加载图像序列，测试图中一共有3张图像
  if(m_pPtrRecordFileStream->good()){
      m_pPtrRecordFileStream->read(reinterpret_cast<char *>(m_pByImageData), m_u32ImageDataLength);
      m_pPtrRecordFileStream->read(reinterpret_cast<char *>(m_pByImageData + m_u32ImageDataLength), m_u32ImageDataLength);
      m_pPtrRecordFileStream->read(reinterpret_cast<char *>(m_pByImageData + (m_u32ImageDataLength<<1)), m_u32ImageDataLength);
      PrintfDebugInfo(_T("视频文件读取成功..."));
  }else{
      PrintfDebugInfo(_T("视频文件读取失败！"));
  }


  uint32_t u32ImagePlayIdx = 0; // 播放帧数统计
  // 循环播放视频的线程函数
  while (!m_bExitRecordPlay) {
    start = std::chrono::high_resolution_clock::now();
    if (!m_bPause) {
        // 显示数据
        if (m_fncImageReadyCallback != nullptr)
        {
            // 设置图像指针
            uint8_t* pImageData = m_pByImageData + (u32ImagePlayIdx%3)*m_u32ImageDataLength;
            // 提取图像识别信息
            memcpy(&m_stFrameSeiInfo, pImageData + 2, u16StSeiLength);

            if (m_stFrameSeiInfo.stSrcFrameInfo.uWidth == 640 && m_stFrameSeiInfo.stSrcFrameInfo.uHeight == 512) {
                PrintfDebugInfo(ui::StringUtil::Printf(_T("播放第%6u帧图像..."),u32ImagePlayIdx));
                // 推给通道0
                m_fncImageReadyCallback(0, pImageData, m_u32ImageDataLength, &m_stFrameSeiInfo);
                // 推给通道1
                m_fncImageReadyCallback(1, pImageData, m_u32ImageDataLength, &m_stFrameSeiInfo);
            }else{
                PrintfDebugInfo(ui::StringUtil::Printf(_T("读取第%6u帧图像错误！"),u32ImagePlayIdx));
            }
        }

        u32ImagePlayIdx++;

        if (u32ImagePlayIdx > 300) { // 已经播放了300，停止播放

            m_bPause = true;
            u32ImagePlayIdx = 0;

            PrintfDebugInfo(_T("停止播放"));

            // 播放结束回调
            if (m_fncRecordUICallback != nullptr)
            {
                m_fncRecordUICallback();
            }
        }
    }

    end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> elapsed = end - start;
    auto u32Duration = static_cast<uint32_t>(elapsed.count());
    if (u32Duration < m_u32Duration) {
      std::this_thread::sleep_for(std::chrono::milliseconds(m_u32Duration - u32Duration));
    }else {
      std::this_thread::yield();
    }
  }

}