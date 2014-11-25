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

    /// 生成する
    bool Create(DXGIManager *pDxgi)override;

    /// 解放する
    void Release()override;

    /// パイプラインにセットする
    void SetupPipeline(ID3D11DeviceContext *pDeviceContext)override;
};


///
/// InputAssembler(頂点バッファ)
///
class D3DInputAssembler: public IRenderResource
{
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_pVertexBuf;
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_pIndexBuf;
    int m_indices;

public:
    /// 生成する
    bool Create(DXGIManager *pDxgi)override;

    /// 解放する
    void Release()override;

    /// パイプラインにセットする
    void SetupPipeline(ID3D11DeviceContext *pDeviceContext)override;

    /// パイプラインを描画する
    void Draw(ID3D11DeviceContext *pDeviceContext);
};

