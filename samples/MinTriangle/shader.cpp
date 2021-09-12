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

