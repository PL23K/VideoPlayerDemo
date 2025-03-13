//
// Created by Yangzhiyong on 2025/3/13.
//

#ifndef MAINTHREAD_H
#define MAINTHREAD_H

// duilib
#include "duilib/duilib.h"

class WorkerThread : public ui::FrameworkThread
{
public:
    WorkerThread();
    virtual ~WorkerThread() override;

private:
    /** 运行前初始化，在进入消息循环前调用
    */
    virtual void OnInit() override;

    /** 退出时清理，在退出消息循环后调用
    */
    virtual void OnCleanup() override;
};

class MainThread : public ui::FrameworkThread {
public:
    MainThread();
    virtual ~MainThread() override;

private:
    /** 运行前初始化，在进入消息循环前调用
    */
    virtual void OnInit() override;

    /** 退出时清理，在退出消息循环后调用
    */
    virtual void OnCleanup() override;

private:
    /** 工作线程(如果不需要多线程处理业务，可以移除工作线程的代码)
    */
    std::unique_ptr<WorkerThread> m_workerThread;
};

#endif //MAINTHREAD_H
