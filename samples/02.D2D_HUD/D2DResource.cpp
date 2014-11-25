#include "D2DResource.h"


bool D2DSolidColorBrush::Create(ID2D1DeviceContext *pRenderTarget)
{
    if(m_brush)return true;

    // Create a gray brush.
    auto hr = pRenderTarget->CreateSolidColorBrush(
            m_color
            , m_brush.ReleaseAndGetAddressOf()
            );
    if(FAILED(hr))return false;
    return true;
}


bool D2DTextFormat::Create(ID2D1DeviceContext *pRenderTarget)
{
    if(m_pTextFormat)return true;

    // Create a DirectWrite factory.
    Microsoft::WRL::ComPtr<IDWriteFactory> pDWriteFactory;
    auto hr = DWriteCreateFactory(
            DWRITE_FACTORY_TYPE_SHARED,
            __uuidof(pDWriteFactory),
            reinterpret_cast<IUnknown **>(pDWriteFactory.GetAddressOf())
            );
    if(FAILED(hr))return false;

    // Create a DirectWrite text format object.
    hr = pDWriteFactory->CreateTextFormat(
            m_font.c_str(),
            NULL,
            DWRITE_FONT_WEIGHT_NORMAL,
            DWRITE_FONT_STYLE_NORMAL,
            DWRITE_FONT_STRETCH_NORMAL,
            m_fontsize,
            L"", //locale
            m_pTextFormat.GetAddressOf()
            );
    if (FAILED(hr))return false;

    m_pTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
    m_pTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);

    return true;
}

