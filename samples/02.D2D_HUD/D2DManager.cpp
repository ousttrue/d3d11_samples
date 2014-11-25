#include "D2DManager.h"
#include <sstream>

//    Microsoft::WRL::ComPtr<IDWriteTextFormat> m_pTextFormat;

D2DManager::D2DManager()
	: m_hud(new HUD)
{
}

D2DManager::~D2DManager()
{
}

void D2DManager::OnDestroyDevice()
{
	m_pBitmap.Reset();

	m_pD2DDeviceContext.Reset();
    m_pD2DDevice.Reset();
}

void D2DManager::OnWindowResize(int w, int h)
{
    if(m_pD2DDeviceContext){
        // clear rendertarget
        m_pD2DDeviceContext->SetTarget(nullptr);
    }

	m_pBitmap.Reset();
}

void D2DManager::Render(DXGIManager *pDxgi)
{
    if(!m_pD2DDeviceContext){
        // ID2D1Factory1
        Microsoft::WRL::ComPtr<ID2D1Factory1> pD2D1Factory;
        auto hr=D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED
                , IID_PPV_ARGS(pD2D1Factory.GetAddressOf()));
        if(FAILED(hr))return;

        // IDXGIDevice
        auto pDXGIDevice=pDxgi->GetDXGIDevice();
        if(!pDXGIDevice)return;

        // ID2D1Device
        hr=pD2D1Factory->CreateDevice(pDXGIDevice.Get()
                        , m_pD2DDevice.ReleaseAndGetAddressOf());
        if(FAILED(hr))return;

        // ID2D1DeviceContext
        hr=m_pD2DDevice->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE
                , m_pD2DDeviceContext.GetAddressOf());
        if(FAILED(hr))return;
    }

    if(!m_pBitmap)
    {
        // IDXGISurface
        auto pDXGISurface=pDxgi->GetSurface();
        if(!pDXGISurface)return;

        // ID2D1Bitmap1
        const auto bp = D2D1::BitmapProperties1(
                D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW, 
                D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_IGNORE));
		auto hr = m_pD2DDeviceContext->CreateBitmapFromDxgiSurface(pDXGISurface.Get()
                , &bp, m_pBitmap.ReleaseAndGetAddressOf());
        if(FAILED(hr))return;
    }

    // render d2d
    m_pD2DDeviceContext->SetTarget(m_pBitmap.Get());
    m_pD2DDeviceContext->BeginDraw();

    m_hud->Render(m_pD2DDeviceContext.Get());

    m_pD2DDeviceContext->EndDraw();
}

