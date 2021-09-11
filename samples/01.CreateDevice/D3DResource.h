#pragma once
#include "DXGIManager.h"


///
/// SwapChain�R����RTV
///
class D3DSwapChainRTVResource: public IRenderResource
{
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_pRTV;
    D3D11_TEXTURE2D_DESC m_backbufferDesc;

public:
    /// ��������
    bool Create(DXGIManager *pDxgi)override;

    /// �������
    void Release()override;

    /// �p�C�v���C���ɃZ�b�g����
    void SetupPipeline(ID3D11DeviceContext *pDeviceContext)override;

    /// �����_�[�^�[�Q�b�g���N���A����
    void Clear(ID3D11DeviceContext *pDeviceContext, const float clearColor[4]);
};
