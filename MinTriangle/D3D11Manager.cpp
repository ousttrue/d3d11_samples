#include "D3D11Manager.h"
#include "CompileShaderFromFile.h"
#include <d3dcompiler.h>
#include <vector>


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

        return true;
    }

    void SetAndClear(ID3D11DeviceContext *pDeviceContext)
    {
        // Output-Merger stage
        pDeviceContext->OMSetRenderTargets(1, &m_pRenderTargetView, NULL);

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
	DXGI_FORMAT GetDxgiFormat(D3D10_REGISTER_COMPONENT_TYPE type, BYTE mask)
	{
		if (mask & 0x0F)
		{
			// xyzw
			switch (type)
			{
			case D3D10_REGISTER_COMPONENT_FLOAT32:
				return DXGI_FORMAT_R32G32B32A32_FLOAT;
			}
		}

		if (mask & 0x07)
		{
			// xyz
			switch (type)
			{
			case D3D10_REGISTER_COMPONENT_FLOAT32:
				return DXGI_FORMAT_R32G32B32_FLOAT;
			}
		}

		if (mask & 0x3)
		{
			// xy
			switch (type)
			{
			case D3D10_REGISTER_COMPONENT_FLOAT32:
				return DXGI_FORMAT_R32G32_FLOAT;
			}
		}

		if (mask & 0x1)
		{
			// x
			switch (type)
			{
			case D3D10_REGISTER_COMPONENT_FLOAT32:
				return DXGI_FORMAT_R32_FLOAT;
			}
		}

		return DXGI_FORMAT_UNKNOWN;
	}


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

		// vertex shader reflection
		ResPtr<ID3D11ShaderReflection> pReflector;
		hr = D3DReflect(vblob->GetBufferPointer(), vblob->GetBufferSize(), IID_ID3D11ShaderReflection, (void**)&pReflector);
		if (FAILED(hr))
			return false;

		D3D11_SHADER_DESC shaderdesc;
		pReflector->GetDesc(&shaderdesc);

		// Create InputLayout
		std::vector<D3D11_INPUT_ELEMENT_DESC> vbElement;
		for (int i = 0; i < shaderdesc.InputParameters; ++i){
			D3D11_SIGNATURE_PARAMETER_DESC sigdesc;
			pReflector->GetInputParameterDesc(i, &sigdesc);

			auto format = GetDxgiFormat(sigdesc.ComponentType, sigdesc.Mask);

			D3D11_INPUT_ELEMENT_DESC eledesc = {
				sigdesc.SemanticName
				, sigdesc.SemanticIndex
				, format
				, 0 // 決め打ち
				, D3D11_APPEND_ALIGNED_ELEMENT // 決め打ち
				, D3D11_INPUT_PER_VERTEX_DATA // 決め打ち
				, 0 // 決め打ち
			};
			vbElement.push_back(eledesc);
		}

		if (!vbElement.empty()){
			hr = pDevice->CreateInputLayout(&vbElement[0], vbElement.size(),
				vblob->GetBufferPointer(), vblob->GetBufferSize(), &m_pInputLayout);
			if (FAILED(hr))
				return false;
		}

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
	, m_constant(new ConstantBuffer<TriangleVariables>)
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

	// constant buffer
	if (!m_constant->Initialize(m_pDevice)){
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

	// 描画
	{
		// shader
		m_shader->Setup(m_pDeviceContext);

		static float angleRadians = 0;
		const auto DELTA = DirectX::XMConvertToRadians(0.1f);
		angleRadians += DELTA;

		//auto m = DirectX::XMMatrixIdentity();
		auto m = DirectX::XMMatrixRotationZ(angleRadians);

		DirectX::XMStoreFloat4x4(&m_constant->Buffer.Model, m);
		 
		m_constant->Update(m_pDeviceContext);
		m_constant->Set(m_pDeviceContext);

		// vertex buffer(Input-Assembler stage)
		m_IASource->Draw(m_pDeviceContext);
	}

    // render targetへの描画
    m_pDeviceContext->Flush();

    // 描画済みのrender targetをモニタに出力
    m_pSwapChain->Present(NULL, NULL);
}

