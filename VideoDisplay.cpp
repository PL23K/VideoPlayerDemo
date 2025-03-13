//
// Created by Yangzhiyong on 2025/3/13.
//

#include "VideoDisplay.h"
#include <iostream>
#include <coroutine>
#include <LogMessage.h>


VideoDisplay::VideoDisplay(ui::Window* pWindow):
        ui::Control(pWindow),m_iChannelId(0),m_pClsBitmap(nullptr), m_bPrintDebugInfo(false)
{
}

VideoDisplay::~VideoDisplay()
{
    delete m_pClsBitmap;
}

void VideoDisplay::SetChannelId(int _id) {
    m_iChannelId = _id;
}

void VideoDisplay::AlphaPaint(ui::IRender* pRender, const ui::UiRect& rcPaint)
{
    Paint(pRender, rcPaint);
}

void VideoDisplay::Paint(ui::IRender* pRender, const ui::UiRect& rcPaint)
{
    if(m_pClsBitmap != nullptr)
    {
        // 绘制图像
        auto start = std::chrono::high_resolution_clock::now();

        // pRender->BitBlt(); // 正确应该使用设个函数？  另外，是否可以将本过程迁移至“协程”来处理更合适，图像数据通过队列的形式传递。个人认为：4.0GHz的CPU，应该支持60Hz的图像刷新率以上的性能算正常
        pRender->DrawImage(rcPaint, m_pClsBitmap, GetRect(), m_rcImageRect);


        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> elapsed = end - start;
        auto u32Duration = static_cast<uint32_t>(elapsed.count());
        PrintfDebugInfo(ui::StringUtil::Printf(_T("渲染图像耗时：%6ums"), u32Duration));

    }else {
        // 设置空白底图
        pRender->FillRect(GetRect(), ui::UiColor(ui::UiColors::DimGray));
    }
}

void VideoDisplay::PaintChild(ui::IRender* pRender, const ui::UiRect& rcPaint)
{
    //std::cout<<" PaintChild"<<std::endl;
    //BaseClass::PaintChild(pRender, rcPaint);
}


void VideoDisplay::UpdateImage(std::shared_ptr<uint8_t[]> _ptrByData, size_t _sizeImageBuffer, std::shared_ptr<StFrameSeiInfo> _ptrStFrameSeiInfo)
{
    auto start = std::chrono::high_resolution_clock::now();

    if(m_pClsBitmap != nullptr)
    {
        delete m_pClsBitmap;
        m_pClsBitmap = nullptr;
    }

    // _pByImage  是灰度数据， 需要转换为RGBA
    uint32_t _uImageWidth = _ptrStFrameSeiInfo->stSrcFrameInfo.uWidth;
    uint32_t _uImageHeight = _ptrStFrameSeiInfo->stSrcFrameInfo.uHeight;
    auto* pByRGBAData = new uint8_t[_sizeImageBuffer<<2]; // 乘以4
    for (int row = 0; row < _uImageHeight; row++) {
        uint32_t uGrayLineLength = row * _uImageWidth;
        uint32_t uRGBALineLength = uGrayLineLength << 2;
        for (int col = 0; col < _uImageWidth; col++) {
            uint8_t pixel = _ptrByData[uGrayLineLength + col];
            pByRGBAData[uRGBALineLength + (col<<2)] = pixel;
            pByRGBAData[uRGBALineLength + (col<<2) + 1] = pixel;
            pByRGBAData[uRGBALineLength + (col<<2) + 2] = pixel;
            pByRGBAData[uRGBALineLength + (col<<2) + 3] = 0xFF;
        }
    }

    m_pClsBitmap = new ui::Bitmap_Skia();
    m_pClsBitmap->Init(_uImageWidth, _uImageHeight, true, pByRGBAData);

    m_rcImageRect.top = 0;
    m_rcImageRect.left = 0;
    m_rcImageRect.right = static_cast<int32_t>(_uImageWidth);
    m_rcImageRect.bottom = static_cast<int32_t>(_uImageHeight);


    delete pByRGBAData;

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> elapsed = end - start;
    auto u32Duration = static_cast<uint32_t>(elapsed.count());
    PrintfDebugInfo(ui::StringUtil::Printf(_T("转移图像耗时：%6ums"), u32Duration));

    // 触发绘制
    RelayoutOrRedraw();
}

void VideoDisplay::SetBPrintDebugInfo(bool _bPrintDebugInfo) {
    m_bPrintDebugInfo = _bPrintDebugInfo;
}

void inline VideoDisplay::PrintfDebugInfo(const DString& _strMessage) const {
    if (m_bPrintDebugInfo) {
        LogMessage(_strMessage.c_str());
    }
}
