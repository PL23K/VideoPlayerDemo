//
// Created by Yangzhiyong on 2025/3/13.
//

#include "MainThread.h"
#include "MainFrame.h"

WorkerThread::WorkerThread()
    : FrameworkThread(_T("WorkerThread"), ui::kThreadWorker)
{
}

WorkerThread::~WorkerThread()
{
}

void WorkerThread::OnInit()
{
}

void WorkerThread::OnCleanup()
{
}


MainThread::MainThread() :
    FrameworkThread(_T("MainThread"), ui::kThreadUI)
{
}

MainThread::~MainThread()
{
}

void MainThread::OnInit()
{
    //启动工作线程
    m_workerThread = std::make_unique<WorkerThread>();
    m_workerThread->Start();

    //初始化全局资源
    constexpr ui::ResourceType resType = ui::ResourceType::kLocalFiles;

    //使用本地文件夹作为资源
    ui::FilePath resourcePath = ui::FilePathUtil::GetCurrentModuleDirectory();
    resourcePath += _T("resources\\");
    ui::GlobalManager::Instance().Startup(ui::LocalFilesResParam(resourcePath));

    // 创建一个默认带有阴影的居中窗口
    auto* window = new MainFrame(); // 这个指针是什么时候被释放的
    window->CreateWnd(nullptr, ui::WindowCreateParam(_T("main"), true));
    window->PostQuitMsgWhenClosed(true);
    window->ShowWindow(ui::kSW_SHOW_NORMAL);
}

void MainThread::OnCleanup()
{
    ui::GlobalManager::Instance().Shutdown();

    if (m_workerThread != nullptr) {
        m_workerThread->Stop();
        m_workerThread.reset(nullptr);
    }
}
