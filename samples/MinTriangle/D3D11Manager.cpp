#include "D3D11Manager.h"
#include "shader.h"
#include <d3d11.h>


class RenderTarget
{
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_pRenderTargetView;
	D3D11_TEXTURE2D_DESC m_colorDesc;

	Microsoft::WRL::ComPtr<ID3D11Texture2D> m_depthStencil;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_depthStencilView;

public:
    bool IsInitialized()const{ return m_pRenderTargetView ? true : false; }

	bool Initialize(const Microsoft::WRL::ComPtr<ID3D11Device> &pDevice
		, const Microsoft::WRL::ComPtr<ID3D11Texture2D> &pTexture)
    {
		pTexture->GetDesc(&m_colorDesc);

        HRESULT hr = pDevice->CreateRenderTargetView(pTexture.Get(), NULL, &m_pRenderTargetView);
        if (FAILED(hr)){
            return false;
        }

		D3D11_TEXTURE2D_DESC tdesc;
		pTexture->GetDesc(&tdesc);

		D3D11_TEXTURE2D_DESC depthDesc;
		ZeroMemory(&depthDesc, sizeof(depthDesc));
		depthDesc.Width = tdesc.Width;
		depthDesc.Height = tdesc.Height;
		depthDesc.MipLevels = 1;
		depthDesc.ArraySize = 1;
		depthDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthDesc.SampleDesc.Count = 1;
		depthDesc.SampleDesc.Quality = 0;
		depthDesc.Usage = D3D11_USAGE_DEFAULT;
		depthDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		depthDesc.CPUAccessFlags = 0;
		depthDesc.MiscFlags = 0;
		hr=pDevice->CreateTexture2D(&depthDesc, NULL, &m_depthStencil);
		if (FAILED(hr)){
			return false;
		}

		D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
		ZeroMemory(&dsvDesc, sizeof(dsvDesc));
		dsvDesc.Format = depthDesc.Format;
		dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
		dsvDesc.Texture2D.MipSlice = 0;
		hr=pDevice->CreateDepthStencilView(m_depthStencil.Get(), &dsvDesc, &m_depthStencilView);
		if (FAILED(hr)){
			return false;
		}

        return true;
    }

    void SetAndClear(const Microsoft::WRL::ComPtr<ID3D11DeviceContext> &pDeviceContext)
    {
        // Output-Merger stage
        pDeviceContext->OMSetRenderTargets(1, m_pRenderTargetView.GetAddressOf(), m_depthStencilView.Get());

        if(m_pRenderTargetView){
            // clear
            float clearColor[] = { 0.2f, 0.2f, 0.4f, 1.0f };
            pDeviceContext->ClearRenderTargetView(m_pRenderTargetView.Get(), clearColor);
			float clearDepth = 1.0f;
			pDeviceContext->ClearDepthStencilView(m_depthStencilView.Get()
				, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, clearDepth, 0);

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
        , const std::string &shaderSource, const std::wstring &textureFile)
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
    if(!m_shader->Initialize(m_pDevice, shaderSource, textureFile)){
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
            0, 0,	// reference ClientRect
            desc.BufferDesc.Format,
            0 // flags
            );
}

void D3D11Manager::Render()
{
    if(!m_renderTarget->IsInitialized()){
        Microsoft::WRL::ComPtr<ID3D11Texture2D> pBackBuffer;
        m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), &pBackBuffer);
		// Create RTV
        if(!m_renderTarget->Initialize(m_pDevice, pBackBuffer)){
            return;
        }
    }
	m_renderTarget->SetAndClear(m_pDeviceContext);

	// draw
	m_shader->Animation();
	m_shader->Draw(m_pDeviceContext);

    m_pDeviceContext->Flush();

    // output
    m_pSwapChain->Present(NULL, NULL);
}
