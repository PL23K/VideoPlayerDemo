//
// Created by Yangzhiyong on 2025/3/13.
//

#ifndef MAINFRAME_H
#define MAINFRAME_H

// duilib
#include "duilib/duilib.h"
#include "LogMessage.h"
#include "RecordReader.h"
#include <memory>
#include <deque>
#include "Base.h"

class VideoDisplay;

class MainFrame : public ui::WindowImplBase {
    typedef ui::WindowImplBase BaseClass;

public:
    MainFrame();
    virtual ~MainFrame() override;
    static MainFrame* GetInstance();


    virtual ui::Control* CreateControl(const DString& strClass) override;
    virtual DString GetSkinFolder() override;
    virtual DString GetSkinFile() override;
    virtual void OnInitWindow() override;

    bool IsUiThread(std::thread::id _iThreadId);

    void OnUpdateLog(LogLevel _logLevel, const DString& _message);
    void OnUpdateVideoChannelImage(int _iVideoChannel, std::shared_ptr<uint8_t[]> _ptrByData,
        size_t _sizeImageBuffer, std::shared_ptr<StFrameSeiInfo> _ptrStFrameSeiInfo);
    bool OnImageCallback(int _iVideoChannel,const uint8_t* _byImage,
    size_t _sizeImageBuffer, PStFrameSeiInfo _pStFrameSeiInfo);
    bool OnPlayFinishCallback();

private:
    static MainFrame* m_pPtrInstance; // 唯一实例对象
    std::thread::id m_iUiThreadId; // UI线程的ID

    VideoDisplay* m_pVideoChannel[2]; // 两个视频通道显示板

    ui::RichEdit* m_pEdtSystemLogMsg; // 日志消息控件
    uint16_t m_u16MessageMaxLine; // 日志最大行数
    std::mutex m_mtxQueueMessage; // 操作队列的锁
    std::deque<DString> m_queueMessage; // 日志队列，最长1000行

    ui::CheckBox* m_pChkVideoChannelPlay; // 播放/暂停
    RecordReader m_recordReader; //录像读取器

    std::chrono::time_point<std::chrono::steady_clock> m_tpLastLogRefreshTime; // 上此日志刷新的时间
};



#endif //MAINFRAME_H
