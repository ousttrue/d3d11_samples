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

