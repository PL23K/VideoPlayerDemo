//
// Created by Yangzhiyong on 2025/3/13.
//

#include "MainFrame.h"
#include <iostream>
#include "duilib/Utils/StringUtil.h"
#include "VideoDisplay.h"
#include "LogMessage.h"

MainFrame* MainFrame::m_pPtrInstance = nullptr;

MainFrame::MainFrame():m_pVideoChannel{nullptr, nullptr},
                       m_pEdtSystemLogMsg(nullptr),m_u16MessageMaxLine(1000),
                       m_pChkVideoChannelPlay(nullptr), m_tpLastLogRefreshTime(std::chrono::high_resolution_clock::now()){

    if (m_pPtrInstance == nullptr) {
        m_pPtrInstance = this;
    }

    m_iUiThreadId = std::this_thread::get_id();
    LogSetCallback(std::bind(&MainFrame::OnUpdateLog, this, std::placeholders::_1,std::placeholders::_2));

    m_recordReader.SetBPrintDebugInfo(true);
    m_recordReader.SetImageReadyCallback(std::bind(&MainFrame::OnImageCallback, this, std::placeholders::_1, std::placeholders::_2,
        std::placeholders::_3, std::placeholders::_4));
    m_recordReader.SetRecordUICallback(std::bind(&MainFrame::OnPlayFinishCallback, this));
    m_recordReader.Start();
}


MainFrame::~MainFrame(){
    // std::cout<<"窗口被释放！"<<std::endl;
}

MainFrame* MainFrame::GetInstance(){
    // if (m_pPtrInstance == nullptr){
    //     m_pPtrInstance = new MainFrame();
    // }
    return m_pPtrInstance;
}

void MainFrame::OnUpdateLog(LogLevel _logLevel,const DString& _message) {
    if (IsUiThread(std::this_thread::get_id())) {
        if (!_message.empty())
        {
            {
                std::lock_guard<std::mutex> lock(m_mtxQueueMessage);  // 自动加锁，作用域结束时自动释放锁
                // 加入时间戳
                const DString dateMessage = ui::StringUtil::Printf(_T("[%s] %s"), GetFormatDate().c_str(), _message.c_str());
                m_queueMessage.push_back(dateMessage);
                if (m_queueMessage.size() > m_u16MessageMaxLine) { // 控制行数
                    m_queueMessage.pop_front();
                }
            }

            // 控制 log 窗口刷新率为25Hz 40ms 40000us
            auto tpCurLogRefreshTime = std::chrono::high_resolution_clock::now(); // 上此日志刷新的时间
            std::chrono::duration<double, std::micro> elapsed = tpCurLogRefreshTime - m_tpLastLogRefreshTime;
            auto u64Duration = static_cast<uint64_t>(elapsed.count());
            if (u64Duration > 40000ull) { //间隔了40ms 才会刷新日志控件
                m_tpLastLogRefreshTime = tpCurLogRefreshTime; // 更新刷新时间
                DString logMessage;
                for (auto it = m_queueMessage.begin(); it != m_queueMessage.end(); ++it) {
                    logMessage.append(*it);
                    logMessage.append(_T("\n"));
                }
                m_pEdtSystemLogMsg->SetText(logMessage);
            }
        }
    }else {
        // 不是UI线程，通过线程任务发送给UI线程执行
        ui::GlobalManager::Instance().Thread().PostTask(ui::kThreadUI,
            UiBind(&MainFrame::OnUpdateLog, this, _logLevel, _message));
    }
}

DString MainFrame::GetSkinFolder(){
    return _T("main");
}

DString MainFrame::GetSkinFile(){
    return _T("main.xml");
}

ui::Control* MainFrame::CreateControl(const DString& strClass)
{
    if (strClass == _T("VideoDisplay")) {
        auto* pVideoPlay = new VideoDisplay(this);
        pVideoPlay->SetBPrintDebugInfo(true);
        return pVideoPlay;
    }
    return nullptr;
}

void MainFrame::OnInitWindow() {

    m_pVideoChannel[0] = dynamic_cast<VideoDisplay*>(FindControl(_T("video_channel_0")));
    ASSERT(m_pVideoChannel[0] != nullptr);
    m_pVideoChannel[1] = dynamic_cast<VideoDisplay*>(FindControl(_T("video_channel_1")));
    ASSERT(m_pVideoChannel[1] != nullptr);

    m_pChkVideoChannelPlay = dynamic_cast<ui::CheckBox*>(FindControl(_T("video_channel_play")));
    ASSERT(m_pChkVideoChannelPlay != nullptr);
    m_pChkVideoChannelPlay->AttachSelect([this](const ui::EventArgs& args) {
        auto* pEdtVideoChannelFps = dynamic_cast<ui::RichEdit*>(FindControl(_T("edt_video_channel_fps")));
        // 播放
        m_recordReader.SetFPS(std::_tcstol(pEdtVideoChannelFps->GetText().c_str(), nullptr, 10));

        // 如果发生，改变就要重新开始
        m_recordReader.Play(true);
        return true;
    });
    m_pChkVideoChannelPlay->AttachUnSelect([this](const ui::EventArgs& args) {
        // 暂停
        m_recordReader.Play(false);
        return true;
    });

    m_pEdtSystemLogMsg = dynamic_cast<ui::RichEdit*>(FindControl(_T("edt_system_log_msg")));
    ASSERT(m_pEdtSystemLogMsg != nullptr);
}

bool MainFrame::IsUiThread(std::thread::id _iThreadId)
{
    return _iThreadId == m_iUiThreadId;
}

void MainFrame::OnUpdateVideoChannelImage(int _iVideoChannel, std::shared_ptr<uint8_t[]> _ptrByData,
        size_t _sizeImageBuffer, std::shared_ptr<StFrameSeiInfo> _ptrStFrameSeiInfo) {

    auto* pVdChannel = m_pVideoChannel[_iVideoChannel];

    if (pVdChannel != nullptr)
    {
        pVdChannel->UpdateImage(_ptrByData, _sizeImageBuffer, _ptrStFrameSeiInfo);
    }
}

bool MainFrame::OnImageCallback(int _iVideoChannel,const uint8_t* _byImage,
    size_t _sizeImageBuffer, PStFrameSeiInfo _pStFrameSeiInfo) {
    // 将图像复制走
    std::shared_ptr<uint8_t[]> ptrByImageCopy = std::make_shared<uint8_t[]>(_sizeImageBuffer);
    memcpy(ptrByImageCopy.get(), _byImage, _sizeImageBuffer);

    // 将结果数据复制走
    std::shared_ptr<StFrameSeiInfo> ptrStFrameSeiInfoCopy = std::make_shared<StFrameSeiInfo>();
    memcpy(ptrStFrameSeiInfoCopy.get(), _pStFrameSeiInfo, sizeof(StFrameSeiInfo));

    // 发送给UI线程处理
    ui::GlobalManager::Instance().Thread().PostTask(ui::kThreadUI,
        UiBind(&MainFrame::OnUpdateVideoChannelImage,
            MainFrame::GetInstance(),
            _iVideoChannel, ptrByImageCopy,
            _sizeImageBuffer, ptrStFrameSeiInfoCopy
        ));
    return true;
}

bool MainFrame::OnPlayFinishCallback() {
    // 不是UI线程，通过线程任务发送给UI线程执行
    ui::GlobalManager::Instance().Thread().PostTask(ui::kThreadUI, [this] {
        m_pChkVideoChannelPlay->Selected(false);
    });
    return true;
}



