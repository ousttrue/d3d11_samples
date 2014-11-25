#pragma once
#include "DXGIManager.h"


///
/// SwapChain由来のRTV
///
class D3DSwapChainRTVResource: public IRenderResource
{
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_pRTV;
    D3D11_TEXTURE2D_DESC m_backbufferDesc;

public:
    /// 生成する
    bool Create(DXGIManager *pDxgi)override;

    /// 解放する
    void Release()override;

    /// パイプラインにセットする
    void SetupPipeline(ID3D11DeviceContext *pDeviceContext)override;

    /// レンダーターゲットをクリアする
    void Clear(ID3D11DeviceContext *pDeviceContext, const float clearColor[4]);
};
