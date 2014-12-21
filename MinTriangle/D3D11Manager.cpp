#include "D3D11Manager.h"
#include "shader.h"
#include <d3d11.h>


class RenderTarget
{
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_pRenderTargetView;
	D3D11_TEXTURE2D_DESC m_colorDesc;

public:
    bool IsInitialized()const{ return m_pRenderTargetView ? true : false; }

	bool Initialize(const Microsoft::WRL::ComPtr<ID3D11Device> &pDevice
		, const Microsoft::WRL::ComPtr<ID3D11Texture2D> &pTexture)
    {
		pTexture->GetDesc(&m_colorDesc);

        // RenderTargetViewの作成
        HRESULT hr = pDevice->CreateRenderTargetView(pTexture.Get(), NULL, &m_pRenderTargetView);
        if (FAILED(hr)){
            return false;
        }

        return true;
    }

    void SetAndClear(const Microsoft::WRL::ComPtr<ID3D11DeviceContext> &pDeviceContext)
    {
        // Output-Merger stage
        pDeviceContext->OMSetRenderTargets(1, m_pRenderTargetView.GetAddressOf(), NULL);

        if(m_pRenderTargetView){
            // clear
            float clearColor[] = { 0.0f, 0.0f, 1.0f, 0.0f };
            pDeviceContext->ClearRenderTargetView(m_pRenderTargetView.Get(), clearColor);

            // Rasterizer stage
            D3D11_VIEWPORT vp;
            vp.Width = static_cast<float>(m_colorDesc.Width);
            vp.Height = static_cast<float>(m_colorDesc.Height);
            vp.MinDepth = 0.0f;
            vp.MaxDepth = 1.0f;
            vp.TopLeftX = 0;
            vp.TopLeftY = 0;
            pDeviceContext->RSSetViewports(1, &vp);
        }
    }
};



//////////////////////////////////////////////////////////////////////////////
// D3D11Manager
//////////////////////////////////////////////////////////////////////////////
D3D11Manager::D3D11Manager()
    : m_renderTarget(new RenderTarget)
    , m_shader(new Shader)
{
}

D3D11Manager::~D3D11Manager()
{
}

bool D3D11Manager::Initialize(HWND hWnd
        , const std::wstring &shaderFile, const std::wstring &textureFile)
{
    D3D_DRIVER_TYPE dtype = D3D_DRIVER_TYPE_HARDWARE;
    UINT flags = 0;
    D3D_FEATURE_LEVEL featureLevels[] = {
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0,
        D3D_FEATURE_LEVEL_9_3,
        D3D_FEATURE_LEVEL_9_2,
        D3D_FEATURE_LEVEL_9_1,
    };
    UINT numFeatureLevels = sizeof(featureLevels) / sizeof(D3D_FEATURE_LEVEL);
    UINT sdkVersion = D3D11_SDK_VERSION;
    D3D_FEATURE_LEVEL validFeatureLevel;

    DXGI_SWAP_CHAIN_DESC scDesc;
    ZeroMemory(&scDesc, sizeof(scDesc));
    scDesc.BufferCount = 1;
    scDesc.BufferDesc.Width = 0;
    scDesc.BufferDesc.Height = 0;
    scDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
    scDesc.BufferDesc.RefreshRate.Numerator = 60;
    scDesc.BufferDesc.RefreshRate.Denominator = 1;
    scDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    scDesc.OutputWindow = hWnd;
    scDesc.SampleDesc.Count = 1;
    scDesc.SampleDesc.Quality = 0;
    scDesc.Windowed = TRUE;

    HRESULT hr = D3D11CreateDeviceAndSwapChain(
            NULL, // adapter
            dtype,
            NULL,
            flags,
            featureLevels,
            numFeatureLevels,
            sdkVersion,
            &scDesc,
            &m_pSwapChain,
            &m_pDevice,
            &validFeatureLevel,
            &m_pDeviceContext);
    if (FAILED(hr)){
        return false;
    }

    // shader 
    if(!m_shader->Initialize(m_pDevice, shaderFile, textureFile)){
        return false;
    }

    return true;
}

void D3D11Manager::Resize(int w, int h)
{
	if (!m_pDeviceContext){
		return;
	}
    // clear render target
    m_renderTarget=std::make_shared<RenderTarget>();
    m_renderTarget->SetAndClear(m_pDeviceContext);
    // resize swapchain
    DXGI_SWAP_CHAIN_DESC desc;
    m_pSwapChain->GetDesc(&desc);
    m_pSwapChain->ResizeBuffers(desc.BufferCount,
            0, 0,	// ClientRect を参照する
            desc.BufferDesc.Format,
            0 // flags
            );
}

void D3D11Manager::Render()
{
    if(!m_renderTarget->IsInitialized()){
        // SwapChainからバックバッファを取得
        Microsoft::WRL::ComPtr<ID3D11Texture2D> pBackBuffer;
        m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), &pBackBuffer);
		// Create RTV
        if(!m_renderTarget->Initialize(m_pDevice, pBackBuffer)){
            return;
        }
    }
	m_renderTarget->SetAndClear(m_pDeviceContext);

	// 描画
	m_shader->Animation();
	m_shader->Draw(m_pDeviceContext);

    // render targetへの描画
    m_pDeviceContext->Flush();

    // 描画済みのrender targetをモニタに出力
    m_pSwapChain->Present(NULL, NULL);
}
