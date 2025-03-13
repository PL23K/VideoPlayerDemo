#include "duilib/duilib_config_windows.h"
#include "MainThread.h"

#pragma comment(linker, "/SUBSYSTEM:windows")

//定义应用程序的入口点
int APIENTRY wWinMain(_In_ HINSTANCE /*hInstance*/,
                      _In_opt_ HINSTANCE /*hPrevInstance*/,
                      _In_ LPWSTR    /*lpCmdLine*/,
                      _In_ int       /*nCmdShow*/)
{
    // 创建主线程
    MainThread thread;

    // 执行主线程循环
    thread.RunOnCurrentThreadWithLoop();
    return 0;
}