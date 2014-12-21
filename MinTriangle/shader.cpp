#include "shader.h"
#include <DirectXMath.h>
#include "imageutil.h"
#include "CompileShaderFromFile.h"
#include <d3d11.h>
#include <d3dcompiler.h>


// input-assembler
struct Vertex
{
    DirectX::XMFLOAT4 pos;
    DirectX::XMFLOAT4 color;
	DirectX::XMFLOAT2 tex;
};


static void OutputDebugPrintfA(LPCSTR pszFormat, ...)
{
	va_list	argp;
	char pszBuf[256];
	va_start(argp, pszFormat);
	vsprintf(pszBuf, pszFormat, argp);
	va_end(argp);
	OutputDebugStringA(pszBuf);
}


class InputAssemblerSource
{
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_pVertexBuf;
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_pIndexBuf;
public:

	bool Initialize(const Microsoft::WRL::ComPtr<ID3D11Device> &pDevice)
	{
		if (!createVB(pDevice)){
			return false;
		}
		if (!createIB(pDevice)){
			return false;
		}
		return true;
	}

	void Draw(const Microsoft::WRL::ComPtr<ID3D11DeviceContext> &pDeviceContext)
	{
		// VBのセット
		ID3D11Buffer* pBufferTbl[] = { m_pVertexBuf.Get() };
		UINT SizeTbl[] = { sizeof(Vertex) };
		UINT OffsetTbl[] = { 0 };
		pDeviceContext->IASetVertexBuffers(0, 1, pBufferTbl, SizeTbl, OffsetTbl);
		// IBのセット
		pDeviceContext->IASetIndexBuffer(m_pIndexBuf.Get(), DXGI_FORMAT_R32_UINT, 0);
		// プリミティブタイプのセット
		pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

		pDeviceContext->DrawIndexed(4 // index count
			, 0, 0);
	}

private:
	bool createVB(const Microsoft::WRL::ComPtr<ID3D11Device> &pDevice)
	{
		// Create VB
		auto size = 0.5f;
		Vertex pVertices[] =
		{
			{ DirectX::XMFLOAT4(-size, -size, 0.0f, 1.0f), DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f), DirectX::XMFLOAT2(0, 1) },
			{ DirectX::XMFLOAT4(-size, size, 0.0f, 1.0f), DirectX::XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f), DirectX::XMFLOAT2(0, 0) },
			{ DirectX::XMFLOAT4(size, -size, 0.0f, 1.0f), DirectX::XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f), DirectX::XMFLOAT2(1, 1) },
			{ DirectX::XMFLOAT4(size, size, 0.0f, 1.0f), DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), DirectX::XMFLOAT2(1, 0) },

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

		HRESULT hr = pDevice->CreateBuffer(&vdesc, &vertexData, m_pVertexBuf.GetAddressOf());
		if (FAILED(hr)){
			return false;
		}

		return true;
	}

	bool createIB(const Microsoft::WRL::ComPtr<ID3D11Device> &pDevice)
	{
		unsigned int pIndices[] =
		{
			0, 1, 2, 3
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

		HRESULT hr = pDevice->CreateBuffer(&idesc, &indexData, m_pIndexBuf.GetAddressOf());
		if (FAILED(hr)){
			return false;
		}

		return true;
	}
};


class ConstantBuffer
{
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_pBuffer;

public:
    struct T
    {
        DirectX::XMFLOAT4X4 Model;
    };
	T Buffer;

	bool Initialize(const Microsoft::WRL::ComPtr<ID3D11Device> &pDevice)
	{
		D3D11_BUFFER_DESC desc = { 0 };

		desc.ByteWidth = sizeof(DirectX::XMMATRIX);
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

		HRESULT hr = pDevice->CreateBuffer(&desc, nullptr, &m_pBuffer);
		if (FAILED(hr)){
			return false;
		}

		return true;
	}

	void Update(const Microsoft::WRL::ComPtr<ID3D11DeviceContext> &pDeviceContext)
	{
		if (!m_pBuffer){
			return;
		}
		pDeviceContext->UpdateSubresource(m_pBuffer.Get(), 0, NULL, &Buffer, 0, 0);
	}

	void Set(const Microsoft::WRL::ComPtr<ID3D11DeviceContext> &pDeviceContext)
	{
		pDeviceContext->VSSetConstantBuffers(0, 1, m_pBuffer.GetAddressOf());
	}
};


class Texture
{
	Microsoft::WRL::ComPtr<ID3D11Texture2D> m_texture;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_srv;
    Microsoft::WRL::ComPtr<ID3D11SamplerState> m_sampler;

public:
	bool Initialize(const Microsoft::WRL::ComPtr<ID3D11Device> &device
            , const std::shared_ptr<imageutil::Image> &image)
	{
		D3D11_TEXTURE2D_DESC desc;
		desc.Width = image->Width();
		desc.Height = image->Height();
		desc.MipLevels = 1;
		desc.ArraySize = 1;
		desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA initData;
		initData.pSysMem = image->Pointer();
		initData.SysMemPitch = image->Stride();
		initData.SysMemSlicePitch = image->Size();

		auto hr = device->CreateTexture2D(&desc, &initData, &m_texture);
		if (FAILED(hr)){
			return false;
		}

		D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc = {};
		SRVDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		SRVDesc.Texture2D.MipLevels = 1;

		hr = device->CreateShaderResourceView(m_texture.Get(), &SRVDesc, &m_srv);
		if (FAILED(hr))
		{
			return false;
		}

        D3D11_SAMPLER_DESC samplerDesc;
        samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
        samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
        samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
        samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
        samplerDesc.MipLODBias = 0.0f;
        samplerDesc.MaxAnisotropy = 1;
        samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
        samplerDesc.BorderColor[0] = 0;
        samplerDesc.BorderColor[1] = 0;
        samplerDesc.BorderColor[2] = 0;
        samplerDesc.BorderColor[3] = 0;
        samplerDesc.MinLOD = 0;
        samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

        // Create the texture sampler state.
        hr = device->CreateSamplerState(&samplerDesc, &m_sampler);
        if(FAILED(hr))
        {
            return false;
        }

		return true;
	}

    void Set(const Microsoft::WRL::ComPtr<ID3D11DeviceContext> &deviceContext)
    {
        deviceContext->PSSetShaderResources(0, 1, m_srv.GetAddressOf());
        //deviceContext->PSSetSamplers(0, 1, m_sampler.GetAddressOf());
    }
};

Shader::Shader()
    : m_IASource(new InputAssemblerSource)
    , m_constant(new ConstantBuffer)
    , m_texture(new Texture)
{}

bool Shader::Initialize(const Microsoft::WRL::ComPtr<ID3D11Device> &pDevice
        , const std::wstring &shaderFile, const std::wstring &textureFile)
{
    if(!createShaders(pDevice, shaderFile, "vsMain", "psMain")){
        return false;
    }

    if (!m_IASource->Initialize(pDevice)){
        return false;
    }

    if (!m_constant->Initialize(pDevice)){
        return false;
    }

    auto wicFactory = std::make_shared<imageutil::Factory>();
    auto image=wicFactory->Load(textureFile);
    if(image){
        if (!m_texture->Initialize(pDevice, image)){
            return false;
        }
    }

    return true;
}

void Shader::Draw(const Microsoft::WRL::ComPtr<ID3D11DeviceContext> &pDeviceContext)
{
    // VS
    pDeviceContext->VSSetShader(m_pVsh.Get(), NULL, 0);
    // 定数バッファ
    m_constant->Update(pDeviceContext);
    m_constant->Set(pDeviceContext);

    // PS
    pDeviceContext->PSSetShader(m_pPsh.Get(), NULL, 0);
    // Texture
    m_texture->Set(pDeviceContext);

    // IA InputLayout
    pDeviceContext->IASetInputLayout(m_pInputLayout.Get());
    m_IASource->Draw(pDeviceContext);
}

void Shader::Animation()
{
    static float angleRadians = 0;
    const auto DELTA = DirectX::XMConvertToRadians(0.01f);
    angleRadians += DELTA;

    //auto m = DirectX::XMMatrixIdentity();
    auto m = DirectX::XMMatrixRotationZ(angleRadians);

    DirectX::XMStoreFloat4x4(&m_constant->Buffer.Model, m);
}

static DXGI_FORMAT GetDxgiFormat(D3D10_REGISTER_COMPONENT_TYPE type, BYTE mask)
{
    if ((mask & 0x0F)==0x0F)
    {
        // xyzw
        switch (type)
        {
        case D3D10_REGISTER_COMPONENT_FLOAT32:
            return DXGI_FORMAT_R32G32B32A32_FLOAT;
        }
    }

    if ((mask & 0x07)==0x07)
    {
        // xyz
        switch (type)
        {
        case D3D10_REGISTER_COMPONENT_FLOAT32:
            return DXGI_FORMAT_R32G32B32_FLOAT;
        }
    }

    if ((mask & 0x03)==0x03)
    {
        // xy
        switch (type)
        {
        case D3D10_REGISTER_COMPONENT_FLOAT32:
            return DXGI_FORMAT_R32G32_FLOAT;
        }
    }

    if ((mask & 0x1)==0x1)
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

void Shader::parseConstantBuffer(const Microsoft::WRL::ComPtr<ID3D11ShaderReflection> &pReflector)
{
    D3D11_SHADER_DESC shaderdesc;
    pReflector->GetDesc(&shaderdesc);

    // analize constant buffer
    for (size_t i = 0; i < shaderdesc.ConstantBuffers; ++i){
        auto cb = pReflector->GetConstantBufferByIndex(i);
        D3D11_SHADER_BUFFER_DESC desc;
        cb->GetDesc(&desc);
        OutputDebugPrintfA("[%d: %s]\n", i, desc.Name);

        for (size_t j = 0; j < desc.Variables; ++j){
            auto v = cb->GetVariableByIndex(j);
            D3D11_SHADER_VARIABLE_DESC vdesc;
            v->GetDesc(&vdesc);
            OutputDebugPrintfA("(%d) %s %d\n", j, vdesc.Name, vdesc.StartOffset);
        }
    }
}

bool Shader::createShaders(const Microsoft::WRL::ComPtr<ID3D11Device> &pDevice
    , const std::wstring &shaderFile, const std::string &vsFunc, const std::string &psFunc)
{
    // vertex shader
    {
        Microsoft::WRL::ComPtr<ID3DBlob> vblob;
        HRESULT hr = CompileShaderFromFile(shaderFile.c_str(), vsFunc.c_str(), "vs_4_0_level_9_1", &vblob);
        if (FAILED(hr))
            return false;
        hr = pDevice->CreateVertexShader(vblob->GetBufferPointer(), vblob->GetBufferSize(), NULL, &m_pVsh);
        if (FAILED(hr))
            return false;

        // vertex shader reflection
        Microsoft::WRL::ComPtr<ID3D11ShaderReflection> pReflector;
        hr = D3DReflect(vblob->GetBufferPointer(), vblob->GetBufferSize(), IID_ID3D11ShaderReflection, &pReflector);
        if (FAILED(hr))
            return false;

        OutputDebugPrintfA("#### VertexShader ####\n");
        parseConstantBuffer(pReflector);

        D3D11_SHADER_DESC shaderdesc;
        pReflector->GetDesc(&shaderdesc);

        // Create InputLayout
        std::vector<D3D11_INPUT_ELEMENT_DESC> vbElement;
        for (size_t i = 0; i < shaderdesc.InputParameters; ++i){
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
    }

    // pixel shader
    {
        Microsoft::WRL::ComPtr<ID3DBlob> pblob;
        auto hr = CompileShaderFromFile(shaderFile.c_str(), psFunc.c_str(), "ps_4_0_level_9_1", &pblob);
        if (FAILED(hr))
            return false;
        hr = pDevice->CreatePixelShader(pblob->GetBufferPointer(), pblob->GetBufferSize(), NULL, &m_pPsh);
        if (FAILED(hr))
            return false;

        // pixel shader reflection
        Microsoft::WRL::ComPtr<ID3D11ShaderReflection> pReflector;
        hr = D3DReflect(pblob->GetBufferPointer(), pblob->GetBufferSize(), IID_ID3D11ShaderReflection, 
            &pReflector);
        if (FAILED(hr))
            return false;

        OutputDebugPrintfA("#### PixelShader ####\n");
        parseConstantBuffer(pReflector);
    }

    return true;
}

