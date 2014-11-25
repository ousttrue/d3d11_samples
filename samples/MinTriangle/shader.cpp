#include "shader.h"
#include <DirectXMath.h>
#include "imageutil.h"
#include "CompileShaderFromFile.h"
#include "constantbuffer.h"
#include "debugprint.h"
#include <d3d11.h>
#include <d3dcompiler.h>

// input-assembler
struct Vertex
{
    DirectX::XMFLOAT4 pos;
    DirectX::XMFLOAT4 color;
    DirectX::XMFLOAT2 tex;
};

class InputAssemblerSource
{
    Microsoft::WRL::ComPtr<ID3D11Buffer> m_pVertexBuf;
    Microsoft::WRL::ComPtr<ID3D11Buffer> m_pIndexBuf;
    int m_indices;

public:
    bool Initialize(const Microsoft::WRL::ComPtr<ID3D11Device> &pDevice)
    {
        if (!createVB(pDevice))
        {
            return false;
        }
        if (!createIB(pDevice))
        {
            return false;
        }
        return true;
    }

    void Draw(const Microsoft::WRL::ComPtr<ID3D11DeviceContext> &pDeviceContext)
    {
        // set vertexbuffer
        ID3D11Buffer *pBufferTbl[] = {m_pVertexBuf.Get()};
        UINT SizeTbl[] = {sizeof(Vertex)};
        UINT OffsetTbl[] = {0};
        pDeviceContext->IASetVertexBuffers(0, 1, pBufferTbl, SizeTbl, OffsetTbl);

        // set indexbuffer
        pDeviceContext->IASetIndexBuffer(m_pIndexBuf.Get(), DXGI_FORMAT_R32_UINT, 0);

        pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        pDeviceContext->DrawIndexed(m_indices // index count
                                    ,
                                    0, 0);
    }

private:
    bool createVB(const Microsoft::WRL::ComPtr<ID3D11Device> &pDevice)
    {
        // Create VB
        auto size = 0.5f;
        Vertex pVertices[] =
            {
                // x
                {DirectX::XMFLOAT4(-size, -size, -size, 1.0f), DirectX::XMFLOAT4(0.5f, 0.0f, 0.0f, 1.0f), DirectX::XMFLOAT2(0, 1)},
                {DirectX::XMFLOAT4(-size, -size, size, 1.0f), DirectX::XMFLOAT4(0.5f, 0.0f, 0.0f, 1.0f), DirectX::XMFLOAT2(0, 0)},
                {DirectX::XMFLOAT4(-size, size, size, 1.0f), DirectX::XMFLOAT4(0.5f, 0.0f, 0.0f, 1.0f), DirectX::XMFLOAT2(1, 0)},
                {DirectX::XMFLOAT4(-size, size, -size, 1.0f), DirectX::XMFLOAT4(0.5f, 0.0f, 0.0f, 1.0f), DirectX::XMFLOAT2(1, 1)},

                {DirectX::XMFLOAT4(size, -size, -size, 1.0f), DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f), DirectX::XMFLOAT2(0, 1)},
                {DirectX::XMFLOAT4(size, size, -size, 1.0f), DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f), DirectX::XMFLOAT2(1, 1)},
                {DirectX::XMFLOAT4(size, size, size, 1.0f), DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f), DirectX::XMFLOAT2(1, 0)},
                {DirectX::XMFLOAT4(size, -size, size, 1.0f), DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f), DirectX::XMFLOAT2(0, 0)},
                // y
                {DirectX::XMFLOAT4(-size, size, -size, 1.0f), DirectX::XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f), DirectX::XMFLOAT2(0, 1)},
                {DirectX::XMFLOAT4(-size, size, size, 1.0f), DirectX::XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f), DirectX::XMFLOAT2(0, 0)},
                {DirectX::XMFLOAT4(size, size, size, 1.0f), DirectX::XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f), DirectX::XMFLOAT2(1, 0)},
                {DirectX::XMFLOAT4(size, size, -size, 1.0f), DirectX::XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f), DirectX::XMFLOAT2(1, 1)},

                {DirectX::XMFLOAT4(-size, -size, -size, 1.0f), DirectX::XMFLOAT4(0.0f, 0.5f, 0.0f, 1.0f), DirectX::XMFLOAT2(0, 1)},
                {DirectX::XMFLOAT4(size, -size, -size, 1.0f), DirectX::XMFLOAT4(0.0f, 0.5f, 0.0f, 1.0f), DirectX::XMFLOAT2(1, 1)},
                {DirectX::XMFLOAT4(size, -size, size, 1.0f), DirectX::XMFLOAT4(0.0f, 0.5f, 0.0f, 1.0f), DirectX::XMFLOAT2(1, 0)},
                {DirectX::XMFLOAT4(-size, -size, size, 1.0f), DirectX::XMFLOAT4(0.0f, 0.5f, 0.0f, 1.0f), DirectX::XMFLOAT2(0, 0)},
                // z
                {DirectX::XMFLOAT4(-size, -size, -size, 1.0f), DirectX::XMFLOAT4(0.0f, 0.0f, 0.5f, 1.0f), DirectX::XMFLOAT2(0, 1)},
                {DirectX::XMFLOAT4(-size, size, -size, 1.0f), DirectX::XMFLOAT4(0.0f, 0.0f, 0.5f, 1.0f), DirectX::XMFLOAT2(0, 0)},
                {DirectX::XMFLOAT4(size, size, -size, 1.0f), DirectX::XMFLOAT4(0.0f, 0.0f, 0.5f, 1.0f), DirectX::XMFLOAT2(1, 0)},
                {DirectX::XMFLOAT4(size, -size, -size, 1.0f), DirectX::XMFLOAT4(0.0f, 0.0f, 0.5f, 1.0f), DirectX::XMFLOAT2(1, 1)},

                {DirectX::XMFLOAT4(-size, -size, size, 1.0f), DirectX::XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f), DirectX::XMFLOAT2(0, 1)},
                {DirectX::XMFLOAT4(size, -size, size, 1.0f), DirectX::XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f), DirectX::XMFLOAT2(1, 1)},
                {DirectX::XMFLOAT4(size, size, size, 1.0f), DirectX::XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f), DirectX::XMFLOAT2(1, 0)},
                {DirectX::XMFLOAT4(-size, size, size, 1.0f), DirectX::XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f), DirectX::XMFLOAT2(0, 0)},
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
        if (FAILED(hr))
        {
            return false;
        }

        return true;
    }

    bool createIB(const Microsoft::WRL::ComPtr<ID3D11Device> &pDevice)
    {
        unsigned int pIndices[] =
            {
                0,
                1,
                2,
                2,
                3,
                0,
                4,
                5,
                6,
                6,
                7,
                4,
                8,
                9,
                10,
                10,
                11,
                8,
                12,
                13,
                14,
                14,
                15,
                12,
                16,
                17,
                18,
                18,
                19,
                16,
                20,
                21,
                22,
                22,
                23,
                20,
            };
        unsigned int isize = sizeof(pIndices);
        m_indices = isize / sizeof(pIndices[0]);

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
        if (FAILED(hr))
        {
            return false;
        }

        return true;
    }
};

class Texture
{
    Microsoft::WRL::ComPtr<ID3D11Texture2D> m_texture;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_srv;
    Microsoft::WRL::ComPtr<ID3D11SamplerState> m_sampler;

public:
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> GetSRV() const { return m_srv; }
    Microsoft::WRL::ComPtr<ID3D11SamplerState> GetSampler() const { return m_sampler; }

    bool Initialize(const Microsoft::WRL::ComPtr<ID3D11Device> &device, const std::shared_ptr<imageutil::Image> &image)
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
        if (FAILED(hr))
        {
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
        if (FAILED(hr))
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
    : m_IASource(new InputAssemblerSource), m_constant(new ConstantBuffer), m_texture(new Texture)
{
}

bool Shader::Initialize(const Microsoft::WRL::ComPtr<ID3D11Device> &pDevice, const std::string &shaderSource, const std::wstring &textureFile)
{
    if (!createShaders(pDevice, shaderSource, "vsMain", "psMain"))
    {
        return false;
    }

    if (!m_IASource->Initialize(pDevice))
    {
        return false;
    }

    auto wicFactory = std::make_shared<imageutil::Factory>();
    auto image = wicFactory->Load(textureFile);
    if (image)
    {
        if (!m_texture->Initialize(pDevice, image))
        {
            return false;
        }
    }

    return true;
}

void Shader::Draw(const Microsoft::WRL::ComPtr<ID3D11DeviceContext> &pDeviceContext)
{
    // VS
    pDeviceContext->VSSetShader(m_pVsh.Get(), NULL, 0);

    // PS
    pDeviceContext->PSSetShader(m_pPsh.Get(), NULL, 0);

    // ConstantBuffer
    m_constant->UpdateCB(pDeviceContext);
    m_constant->SetCB(pDeviceContext);

    // Texture
    {
        auto v = m_constant->GetSRV("diffuseTexture");
        m_constant->SetSRV(pDeviceContext, v, m_texture->GetSRV());
    }
    {
        auto v = m_constant->GetSampler("diffuseTextureSampler");
        m_constant->SetSampler(pDeviceContext, v, m_texture->GetSampler());
    }

    // IA InputLayout
    pDeviceContext->IASetInputLayout(m_pInputLayout.Get());
    m_IASource->Draw(pDeviceContext);
}

void Shader::Animation()
{
    {
        static float angleRadiansX = 0;
        {
            const auto DELTA = DirectX::XMConvertToRadians(0.01f);
            angleRadiansX += DELTA;
        }
        auto mx = DirectX::XMMatrixRotationZ(angleRadiansX);

        static float angleRadiansY = 0;
        {
            const auto DELTA = DirectX::XMConvertToRadians(0.02f);
            angleRadiansY += DELTA;
        }
        auto my = DirectX::XMMatrixRotationZ(angleRadiansY);

        static float angleRadiansZ = 0;
        {
            const auto DELTA = DirectX::XMConvertToRadians(0.03f);
            angleRadiansZ += DELTA;
        }
        auto mz = DirectX::XMMatrixRotationZ(angleRadiansZ);

        //auto m = DirectX::XMMatrixIdentity();
        auto m = DirectX::XMMatrixRotationRollPitchYaw(angleRadiansX, angleRadiansY, angleRadiansZ);

        DirectX::XMFLOAT4X4 matrix;
        DirectX::XMStoreFloat4x4(&matrix, m);
        auto v = m_constant->GetCBVariable("ModelMatrix");
        m_constant->SetCBValue(v, matrix);
    }

    {
        auto hEye = DirectX::XMVectorSet(0.0f, 0.0f, -3.0f, 0.0f);
        auto hAt = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
        auto hUp = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
        auto m = DirectX::XMMatrixLookAtLH(hEye, hAt, hUp);

        DirectX::XMFLOAT4X4 matrix;
        DirectX::XMStoreFloat4x4(&matrix, m);
        auto v = m_constant->GetCBVariable("ViewMatrix");
        m_constant->SetCBValue(v, matrix);
    }

    {
        auto m = DirectX::XMMatrixPerspectiveFovLH(DirectX::XMConvertToRadians(45.0f), 1.0f, 1.0f, 1000.0f);
        DirectX::XMFLOAT4X4 matrix;
        DirectX::XMStoreFloat4x4(&matrix, m);
        auto v = m_constant->GetCBVariable("ProjectionMatrix");
        m_constant->SetCBValue(v, matrix);
    }
}

static DXGI_FORMAT GetDxgiFormat(D3D10_REGISTER_COMPONENT_TYPE type, BYTE mask)
{
    if ((mask & 0x0F) == 0x0F)
    {
        // xyzw
        switch (type)
        {
        case D3D10_REGISTER_COMPONENT_FLOAT32:
            return DXGI_FORMAT_R32G32B32A32_FLOAT;
        }
    }

    if ((mask & 0x07) == 0x07)
    {
        // xyz
        switch (type)
        {
        case D3D10_REGISTER_COMPONENT_FLOAT32:
            return DXGI_FORMAT_R32G32B32_FLOAT;
        }
    }

    if ((mask & 0x03) == 0x03)
    {
        // xy
        switch (type)
        {
        case D3D10_REGISTER_COMPONENT_FLOAT32:
            return DXGI_FORMAT_R32G32_FLOAT;
        }
    }

    if ((mask & 0x1) == 0x1)
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

bool Shader::createShaders(const Microsoft::WRL::ComPtr<ID3D11Device> &pDevice, const std::string &shaderSource, const std::string &vsFunc, const std::string &psFunc)
{
    // vertex shader
    {
        Microsoft::WRL::ComPtr<ID3DBlob> vblob;
        HRESULT hr = CompileShaderFromSource("SOURCE", shaderSource.c_str(), static_cast<UINT>(shaderSource.size()), vsFunc.c_str(), "vs_4_0_level_9_1", &vblob);
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
        if (!m_constant->Initialize(pDevice, SHADERSTAGE_VERTEX, pReflector))
        {
            return false;
        }

        D3D11_SHADER_DESC shaderdesc;
        pReflector->GetDesc(&shaderdesc);

        // Create InputLayout
        std::vector<D3D11_INPUT_ELEMENT_DESC> vbElement;
        for (UINT i = 0; i < shaderdesc.InputParameters; ++i)
        {
            D3D11_SIGNATURE_PARAMETER_DESC sigdesc;
            pReflector->GetInputParameterDesc(i, &sigdesc);

            auto format = GetDxgiFormat(sigdesc.ComponentType, sigdesc.Mask);

            D3D11_INPUT_ELEMENT_DESC eledesc = {
                sigdesc.SemanticName, sigdesc.SemanticIndex, format, 0 // hardcoding
                ,
                D3D11_APPEND_ALIGNED_ELEMENT // hardcoding
                ,
                D3D11_INPUT_PER_VERTEX_DATA // hardcoding
                ,
                0 // hardcoding
            };
            vbElement.push_back(eledesc);
        }

        if (!vbElement.empty())
        {
            hr = pDevice->CreateInputLayout(&vbElement[0], static_cast<UINT>(vbElement.size()),
                                            vblob->GetBufferPointer(), vblob->GetBufferSize(), &m_pInputLayout);
            if (FAILED(hr))
                return false;
        }
    }

    // pixel shader
    {
        Microsoft::WRL::ComPtr<ID3DBlob> pblob;
        auto hr = CompileShaderFromSource("SOURCE", shaderSource.c_str(), static_cast<UINT>(shaderSource.size()), psFunc.c_str(), "ps_4_0_level_9_1", &pblob);
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
        if (!m_constant->Initialize(pDevice, SHADERSTAGE_PIXEL, pReflector))
        {
            return false;
        }
    }

    return true;
}
