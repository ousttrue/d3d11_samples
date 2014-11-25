#pragma once
#include <wrl/client.h>
#include <d2d1_1.h>
#include <dwrite.h>
#include <string>


class ID2DResource
{
public:
    virtual ~ID2DResource(){}
    virtual void Release()=0;
    virtual bool Create(ID2D1DeviceContext *pRenderTarget)=0;
};


class D2DSolidColorBrush: public ID2DResource
{
    D2D1::ColorF m_color;
    Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> m_brush; 

public:
    D2DSolidColorBrush(const D2D1::ColorF &color) 
        : m_color(color)
    {}
	ID2D1SolidColorBrush* Get()const{ return m_brush.Get(); }
    void Release()override { m_brush.Reset(); }
    bool Create(ID2D1DeviceContext *pRenderTarget)override;
};


class D2DTextFormat: public ID2DResource
{
    Microsoft::WRL::ComPtr<IDWriteTextFormat> m_pTextFormat;
    std::wstring m_font;
    float m_fontsize;

public:
    D2DTextFormat(const std::wstring &font, float fontsize)
        : m_font(font), m_fontsize(fontsize)
    {}
	IDWriteTextFormat* Get()const{ return m_pTextFormat.Get(); }
    void Release()override{ m_pTextFormat.Reset(); }
    bool Create(ID2D1DeviceContext *pRenderTarget)override;
};
