#include "D3D11Manager.h"
#include "CompileShaderFromFile.h"
#include <DirectXMath.h>


// input-assembler
struct Vertex
{
    DirectX::XMFLOAT4 pos;
    DirectX::XMFLOAT4 color;
};


class RenderTarget
{
    ResPtr<ID3D11RenderTargetView> m_pRenderTargetView;
	D3D11_TEXTURE2D_DESC m_colorDesc;
    ResPtr<ID3D11Texture2D> m_pDepthStencil;
    ResPtr<ID3D11DepthStencilView> m_pDepthStencilView;

public:
    bool IsInitialized()const{ return m_pRenderTargetView ? true : false; }

    bool Initialize(ID3D11Device *pDevice, ID3D11Texture2D *pTexture)
    {
		pTexture->GetDesc(&m_colorDesc);

        // RenderTargetViewの作成
        HRESULT hr = pDevice->CreateRenderTargetView(pTexture, NULL, &m_pRenderTargetView);
        if (FAILED(hr)){
            return false;
        }

        // デプステクスチャの作成
        D3D11_TEXTURE2D_DESC depthDesc;
        ZeroMemory(&depthDesc, sizeof(depthDesc));
        depthDesc.Width = m_colorDesc.Width;
        depthDesc.Height = m_colorDesc.Height;
        depthDesc.MipLevels = 1;
        depthDesc.ArraySize = 1;
        depthDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
        depthDesc.SampleDesc.Count = 1;
        depthDesc.SampleDesc.Quality = 0;
        depthDesc.Usage = D3D11_USAGE_DEFAULT;
        depthDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
        depthDesc.CPUAccessFlags = 0;
        depthDesc.MiscFlags = 0;
        hr = pDevice->CreateTexture2D(&depthDesc, NULL, &m_pDepthStencil);
        if (FAILED(hr)){
            return false;
        }

        // DepthStencilViewの作成
        D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
        ZeroMemory(&dsvDesc, sizeof(dsvDesc));
        dsvDesc.Format = depthDesc.Format;
        dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
        dsvDesc.Texture2D.MipSlice = 0;
        hr = pDevice->CreateDepthStencilView(m_pDepthStencil, &dsvDesc, &m_pDepthStencilView);
        if (FAILED(hr)){
            return false;
        }

        return true;
    }

    void SetAndClear(ID3D11DeviceContext *pDeviceContext)
    {
        // Output-Merger stage
        pDeviceContext->OMSetRenderTargets(1, &m_pRenderTargetView, m_pDepthStencilView);

        if(m_pRenderTargetView){
            // clear
            float clearColor[] = { 0.0f, 0.0f, 1.0f, 0.0f };
            pDeviceContext->ClearRenderTargetView(m_pRenderTargetView, clearColor);

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
        if(m_pDepthStencilView){
            // clear
            pDeviceContext->ClearDepthStencilView(m_pDepthStencilView,
                    D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
        }
    }
};


class Shader
{
    ResPtr<ID3D11VertexShader> m_pVsh;
    ResPtr<ID3D11PixelShader> m_pPsh;
    ResPtr<ID3D11InputLayout> m_pInputLayout;

public:
    bool Initialize(ID3D11Device *pDevice, const std::wstring &shaderFile)
    {
        if(!createShaders(pDevice, shaderFile, "vsMain", "psMain")){
            return false;
        }

        return true;
    }

    void Setup(ID3D11DeviceContext *pDeviceContext)
    {
        // Shaderのセットアップ
        pDeviceContext->VSSetShader(m_pVsh, NULL, 0);
        pDeviceContext->PSSetShader(m_pPsh, NULL, 0);

        // ILのセット
        pDeviceContext->IASetInputLayout(m_pInputLayout);
    }

private:
    bool createShaders(ID3D11Device *pDevice
		, const std::wstring &shaderFile, const std::string &vsFunc, const std::string &psFunc)
    {
        // vertex shader
        ResPtr<ID3DBlob> vblob;
        HRESULT hr = CompileShaderFromFile(shaderFile.c_str(), vsFunc.c_str(), "vs_4_0_level_9_1", &vblob);
        if (FAILED(hr))
            return false;
        hr = pDevice->CreateVertexShader(vblob->GetBufferPointer(), vblob->GetBufferSize(), NULL, &m_pVsh);
        if (FAILED(hr))
            return false;

        // pixel shader
        ResPtr<ID3DBlob> pblob;
        hr = CompileShaderFromFile(shaderFile.c_str(), psFunc.c_str(), "ps_4_0_level_9_1", &pblob);
        if (FAILED(hr))
            return false;
        hr = pDevice->CreatePixelShader(pblob->GetBufferPointer(), pblob->GetBufferSize(), NULL, &m_pPsh);
        if (FAILED(hr))
            return false;

        // Create InputLayout
        D3D11_INPUT_ELEMENT_DESC vbElement[] =
        {
            { "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
        };

        hr = pDevice->CreateInputLayout(vbElement, sizeof(vbElement) / sizeof(D3D11_INPUT_ELEMENT_DESC),
                vblob->GetBufferPointer(), vblob->GetBufferSize(), &m_pInputLayout);
        if (FAILED(hr))
            return false;

        return true;
    }
};


class InputAssemblerSource
{
    ResPtr<ID3D11Buffer> m_pVertexBuf;
    ResPtr<ID3D11Buffer> m_pIndexBuf;
public:

    bool Initialize(ID3D11Device *pDevice)
    {
        if(!createVB(pDevice)){
            return false;
        }
        if(!createIB(pDevice)){
            return false;
        }
        return true;
    }

    void Draw(ID3D11DeviceContext *pDeviceContext)
    {
        // VBのセット
        ID3D11Buffer* pBufferTbl[] = { m_pVertexBuf };
        UINT SizeTbl[] = { sizeof(Vertex) };
        UINT OffsetTbl[] = { 0 };
        pDeviceContext->IASetVertexBuffers(0, 1, pBufferTbl, SizeTbl, OffsetTbl);
        // IBのセット
        pDeviceContext->IASetIndexBuffer(m_pIndexBuf, DXGI_FORMAT_R32_UINT, 0);
        // プリミティブタイプのセット
        pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

        pDeviceContext->DrawIndexed(3 // index count
                , 0, 0);
    }

private:
    bool createVB(ID3D11Device *pDevice)
    {
        // Create VB
        Vertex pVertices[] =
        {
            { DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f), DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f) },
            { DirectX::XMFLOAT4(0.5f, 0.5f, 0.0f, 1.0f), DirectX::XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f) },
            { DirectX::XMFLOAT4(0.5f, -0.5f, 0.0f, 1.0f), DirectX::XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f) }
        };
        unsigned int vsize = sizeof(pVertices);

        D3D11_BUFFER_DESC vdesc;
        ZeroMemory(&vdesc, sizeof(vdesc));
        vdesc.ByteWidth = vsize;
        vdesc.Usage = D3D11_USAGE_DEFAULT;
        vdesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        vdesc.CPUAccessFlags = 0;

        D3D11_SUBRESOURCE_DATA vertexData;
        ZeroMemory(&vertexData, sizeof(vertexData));
        vertexData.pSysMem = pVertices;

        HRESULT hr = pDevice->CreateBuffer(&vdesc, &vertexData, &m_pVertexBuf);
        if (FAILED(hr)){
            return false;
        }

        return true;
    }

	bool createIB(ID3D11Device *pDevice)
    {
        unsigned int pIndices[] =
        {
            0, 1, 2
        };
        unsigned int isize = sizeof(pIndices);

        D3D11_BUFFER_DESC idesc;
        ZeroMemory(&idesc, sizeof(idesc));
        idesc.ByteWidth = isize;
        idesc.Usage = D3D11_USAGE_DEFAULT;
        idesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
        idesc.CPUAccessFlags = 0;

		D3D11_SUBRESOURCE_DATA indexData;
		ZeroMemory(&indexData, sizeof(indexData));
		indexData.pSysMem = pIndices;

        HRESULT hr = pDevice->CreateBuffer(&idesc, &indexData, &m_pIndexBuf);
		if (FAILED(hr)){
			return false;
		}

        return true;
    }
};


//////////////////////////////////////////////////////////////////////////////
// D3D11Manager
//////////////////////////////////////////////////////////////////////////////
D3D11Manager::D3D11Manager()
    : m_renderTarget(new RenderTarget)
    , m_shader(new Shader)
    , m_IASource(new InputAssemblerSource)
{
}

D3D11Manager::~D3D11Manager()
{
}

bool D3D11Manager::Initialize(HWND hWnd, const std::wstring &shaderFile)
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
    if(!m_shader->Initialize(m_pDevice, shaderFile)){
        return false;
    }

    // vertex buffer
    if(!m_IASource->Initialize(m_pDevice)){
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
        // バックバッファの取得
        ResPtr<ID3D11Texture2D> pBackBuffer;
        m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D),
                reinterpret_cast<void**>(&pBackBuffer));

        if(!m_renderTarget->Initialize(m_pDevice, pBackBuffer)){
            return;
        }
    }
    m_renderTarget->SetAndClear(m_pDeviceContext);

    // shader
    m_shader->Setup(m_pDeviceContext);

    // 描画
    // vertex buffer(Input-Assembler stage)
    m_IASource->Draw(m_pDeviceContext);

    // render targetへの描画
    m_pDeviceContext->Flush();

    // 描画済みのrender targetをモニタに出力
    m_pSwapChain->Present(NULL, NULL);
}

