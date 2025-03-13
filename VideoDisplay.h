//
// Created by Yangzhiyong on 2025/3/13.
//

#ifndef VIDEODISPLAY_H
#define VIDEODISPLAY_H

#include <duilib/duilib.h>
#include "duilib/RenderSkia/Bitmap_Skia.h"
#include "Base.h"

class VideoDisplay : public ui::Control{
    typedef ui::Control BaseClass;

public:
    explicit VideoDisplay(ui::Window* pWindow);
    virtual ~VideoDisplay() override;

public:
    /**
    * @brief 待补充
    * @param[in] 待补充
    * @return 待补充
    */
    virtual void AlphaPaint(ui::IRender* pRender, const ui::UiRect& rcPaint) override;

    /**
    * @brief 绘制控件的入口函数
    * @param[in] pRender 指定绘制区域
    * @param[in] rcPaint 指定绘制坐标
    * @return 无
    */
    virtual void Paint(ui::IRender* pRender, const ui::UiRect& rcPaint) override;

    /**
    * @brief 绘制控件子项入口函数
    * @param[in] pRender 指定绘制区域
    * @param[in] rcPaint 指定绘制坐标
    * @return 无
    */
    virtual void PaintChild(ui::IRender* pRender, const ui::UiRect& rcPaint) override;

    void UpdateImage(std::shared_ptr<uint8_t[]> _ptrByData, size_t _sizeImageBuffer, std::shared_ptr<StFrameSeiInfo> _ptrStFrameSeiInfo);

    void SetChannelId(int _id);
    void SetBPrintDebugInfo(bool _bPrintDebugInfo);

private:
    void inline PrintfDebugInfo(const DString& _strMessage) const;

private:
    ui::Bitmap_Skia* m_pClsBitmap;
    ui::UiRect m_rcImageRect;
    int m_iChannelId;
    bool m_bPrintDebugInfo;
};



#endif //VIDEODISPLAY_H
