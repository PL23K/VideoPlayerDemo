# VideoPlayerDemo
Win64 C++ 视频播放器测试代码，基于nim-duilib[skia]开发

## 第三方包结构
项目路径/ThridPart/

![](./images/1.png)
![](./images/2.png)
![](./images/3.png)
![](./images/4.png)
![](./images/5.png)

项目路径/ThridPart/nim_duilib/include下面则是 duilib 头文件。skia不需要头文件
![](./images/6.png)

## 效果
![](./images/7.png)

## 优化
本项目就是传上来给nim__duilib检查图像刷新性能问题的，解决刷新卡顿，提升刷新率。

nim_duilib: https://github.com/rhett-lee/nim_duilib