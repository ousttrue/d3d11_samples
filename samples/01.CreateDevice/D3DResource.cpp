#include "D3DResource.h"
#include <DirectXMath.h>
#include <d3dcompiler.h>
#include <vector>

//////////////////////////////////////////////////////////////////////////////
// D3DSwapChainRTVResource
//////////////////////////////////////////////////////////////////////////////
bool D3DSwapChainRTVResource::Create(DXGIManager *pDxgi)
{
    if(m_pRTV)return true;

	auto pDevice = pDxgi->GetD3DDevice();
	auto pSwapChain = pDxgi->GetSwapChain();

	// SwapChainからバックバッファを取得
    Microsoft::WRL::ComPtr<ID3D11Texture2D> pBackBuffer;
    auto hr = pSwapChain->GetBuffer(0, IID_PPV_ARGS(pBackBuffer.GetAddressOf()));
    if (FAILED(hr))return false;
    pBackBuffer->GetDesc(&m_backbufferDesc);

    // RTVを作る
    hr = pDevice->CreateRenderTargetView(pBackBuffer.Get()
            , NULL, m_pRTV.ReleaseAndGetAddressOf());
    if (FAILED(hr))return false;

    return true;
}

void D3DSwapChainRTVResource::Release()
{
    m_pRTV.Reset();
}

void D3DSwapChainRTVResource::SetupPipeline(ID3D11DeviceContext *pDeviceContext)
{
    // Output-Merger stage
	ID3D11RenderTargetView* rtvList[] = { m_pRTV.Get() };
	pDeviceContext->OMSetRenderTargets(1, rtvList, nullptr);

    if(m_pRTV){
        // Rasterizer stage
        D3D11_VIEWPORT vp;
        vp.Width = static_cast<float>(m_backbufferDesc.Width);
        vp.Height = static_cast<float>(m_backbufferDesc.Height);
        vp.MinDepth = 0.0f;
        vp.MaxDepth = 1.0f;
        vp.TopLeftX = 0;
        vp.TopLeftY = 0;
        pDeviceContext->RSSetViewports(1, &vp);
    }
}

void D3DSwapChainRTVResource::Clear(ID3D11DeviceContext *pDeviceContext, const float clearColor[4])
{
    pDeviceContext->ClearRenderTargetView(m_pRTV.Get(), clearColor);
}

