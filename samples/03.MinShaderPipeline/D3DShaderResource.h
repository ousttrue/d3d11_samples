#pragma once
#include "../01.CreateDevice/D3DResource.h"


struct StageInfo
{
    // "vsMain"
    std::string entrypoint;
    // "vs_4_0_level_9_1"
    std::string model;
};

struct ShaderInfo
{
    std::wstring path;
    StageInfo vs;
    StageInfo gs;
    StageInfo ps;
};


///
/// Shader
///
class D3DShader: public IRenderResource
{
    ShaderInfo m_info;

    Microsoft::WRL::ComPtr<struct ID3D11VertexShader> m_pVsh;
    Microsoft::WRL::ComPtr<struct ID3D11PixelShader> m_pPsh;
    Microsoft::WRL::ComPtr<struct ID3D11InputLayout> m_pInputLayout;

public:
    void SetInfo(const ShaderInfo &info){ m_info=info; }
    ShaderInfo GetInfo()const{ return m_info; }

    /// ��������
    bool Create(DXGIManager *pDxgi)override;

    /// �������
    void Release()override;

    /// �p�C�v���C���ɃZ�b�g����
    void SetupPipeline(ID3D11DeviceContext *pDeviceContext)override;
};


///
/// InputAssembler(���_�o�b�t�@)
///
class D3DInputAssembler: public IRenderResource
{
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_pVertexBuf;
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_pIndexBuf;
    int m_indices;

public:
    /// ��������
    bool Create(DXGIManager *pDxgi)override;

    /// �������
    void Release()override;

    /// �p�C�v���C���ɃZ�b�g����
    void SetupPipeline(ID3D11DeviceContext *pDeviceContext)override;

    /// �p�C�v���C����`�悷��
    void Draw(ID3D11DeviceContext *pDeviceContext);
};

