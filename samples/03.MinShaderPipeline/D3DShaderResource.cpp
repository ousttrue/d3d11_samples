#include "D3DShaderResource.h"
#include <vector>
#include <fstream>
#include <wrl/client.h>
#include <DirectXMath.h>
#include <d3dcompiler.h>
#ifndef D3D_COMPILE_STANDARD_FILE_INCLUDE
#define D3D_COMPILE_STANDARD_FILE_INCLUDE ((ID3DInclude*)(UINT_PTR)1)

inline std::string to_MultiByte(UINT uCodePage, const std::wstring &text)
{
    int size=WideCharToMultiByte(uCodePage, 0, text.c_str(), -1, NULL, 0, 0, NULL);
	if(size<=1){
		return "";
	}
    std::vector<char> buf(size);
    size=WideCharToMultiByte(uCodePage, 0, text.c_str(), -1, &buf[0], buf.size(), 0, NULL);
	// 末尾の\0を落とす
    return std::string(buf.begin(), buf.begin()+size-1);
}

inline HRESULT D3DCompileFromFile(
  LPCWSTR pFileName,
  const D3D_SHADER_MACRO *pDefines,
  ID3DInclude *pInclude,
  LPCSTR pEntrypoint,
  LPCSTR pTarget,
  UINT Flags1,
  UINT Flags2,
  ID3DBlob **ppCode,
  ID3DBlob **ppErrorMsgs
  )
{
    std::ifstream ifs(pFileName, std::ios::binary);
    if(!ifs){
        return E_FAIL;
    }

	ifs.seekg (0, std::ios::end);
    std::vector<unsigned char> buffer(static_cast<unsigned int>(ifs.tellg ()));
	ifs.seekg (0, std::ios::beg);
    if(buffer.empty()){
        return E_FAIL;
    }
    ifs.read ((char*)&buffer[0], buffer.size());

    return D3DCompile(&buffer[0], buffer.size(), to_MultiByte(932, pFileName).c_str()
            , pDefines
            , pInclude
            , pEntrypoint
            , pTarget
            , Flags1
            , Flags2
            , ppCode
            , ppErrorMsgs);
}

#endif

HRESULT CompileShaderFromFile(const std::wstring &file
        , const StageInfo &info, ID3D10Blob** ppBlobOut)
{
    DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined(DEBUG) || defined(_DEBUG)
    dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif//defiend(DEBUG) || defined(_DEBUG)
#if defined(NDEBUG) || defined(_NDEBUG)
    dwShaderFlags |= D3DCOMPILE_OPTIMIZATION_LEVEL3;
#endif//defined(NDEBUG) || defined(_NDEBUG)

    Microsoft::WRL::ComPtr<ID3DBlob> pErrorBlob;
    auto hr = D3DCompileFromFile(
            file.c_str()
            , NULL
            , D3D_COMPILE_STANDARD_FILE_INCLUDE
            , info.entrypoint.c_str()
            , info.model.c_str()
            , dwShaderFlags
            , 0
            , ppBlobOut
            , pErrorBlob.GetAddressOf() 
            );
    if ( FAILED( hr ) ) {
        if (pErrorBlob) { 
            // エラーメッセージを出力.
            OutputDebugStringA( (char*)pErrorBlob->GetBufferPointer() ); 
        }
    }

    return hr;
}

//////////////////////////////////////////////////////////////////////////////
// Shader
//////////////////////////////////////////////////////////////////////////////
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

/// 生成する
bool D3DShader::Create(DXGIManager *pDxgi)
{
    if(m_pVsh)return true;

    auto pDevice=pDxgi->GetD3DDevice();

    // vertex shader
    {
        Microsoft::WRL::ComPtr<ID3DBlob> vblob;
        HRESULT hr = CompileShaderFromFile(m_info.path, m_info.vs
                , vblob.GetAddressOf());
        if (FAILED(hr)) return false;

        hr = pDevice->CreateVertexShader(
                vblob->GetBufferPointer(), vblob->GetBufferSize()
                , NULL, m_pVsh.ReleaseAndGetAddressOf());
        if (FAILED(hr)) return false;

        // vertex shader reflection
        Microsoft::WRL::ComPtr<ID3D11ShaderReflection> pReflector;
        hr = D3DReflect(vblob->GetBufferPointer(), vblob->GetBufferSize()
                , IID_ID3D11ShaderReflection, &pReflector);
        if (FAILED(hr)) return false;

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
            if (FAILED(hr)) return false;
        }
    }

    // pixel shader
    {
        Microsoft::WRL::ComPtr<ID3DBlob> pblob;
        auto hr = CompileShaderFromFile(m_info.path, m_info.ps
                , pblob.GetAddressOf());
        if (FAILED(hr)) return false;

        hr = pDevice->CreatePixelShader(
                pblob->GetBufferPointer(), pblob->GetBufferSize()
                , NULL, m_pPsh.ReleaseAndGetAddressOf());
        if (FAILED(hr)) return false;
    }

    return true;
}

/// 解放する
void D3DShader::Release()
{
    m_pVsh.Reset();
    m_pPsh.Reset();
    m_pInputLayout.Reset();
}

/// パイプラインにセットする
void D3DShader::SetupPipeline(ID3D11DeviceContext *pDeviceContext)
{
    pDeviceContext->VSSetShader(m_pVsh.Get(), NULL, 0);
    pDeviceContext->PSSetShader(m_pPsh.Get(), NULL, 0);

    pDeviceContext->IASetInputLayout(m_pInputLayout.Get());
    pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}


//////////////////////////////////////////////////////////////////////////////
// InputAssembler
//////////////////////////////////////////////////////////////////////////////
struct Vertex
{
    DirectX::XMFLOAT4 pos;
    DirectX::XMFLOAT4 color;
};

/// 生成する
bool D3DInputAssembler::Create(DXGIManager *pDxgi)
{
    if(m_pVertexBuf)return true;

    auto pDevice=pDxgi->GetD3DDevice();

    {
        // Create VB
        auto size = 0.5f;
        Vertex pVertices[] =
        {
            // x
			{ { 1, -1, 0, 1}, { 0, 1, 0, 1 } },
			{ { -1, -1, 0, 1 }, { 1, 0, 0, 1 } },
			{ { 0, 1, 0, 1 }, { 0, 0, 1, 1 } },
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
    }

    {
        unsigned int pIndices[] =
        {
			0, 1, 2,
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
        if (FAILED(hr)){
            return false;
        }
    }

    return true;
}

/// 解放する
void D3DInputAssembler::Release()
{
	m_pVertexBuf.Reset();
	m_pIndexBuf.Reset();
}

/// パイプラインにセットする
void D3DInputAssembler::SetupPipeline(ID3D11DeviceContext *pDeviceContext)
{
    // VBのセット
    ID3D11Buffer* pBufferTbl[] = { m_pVertexBuf.Get() };
    UINT SizeTbl[] = { sizeof(Vertex) };
    UINT OffsetTbl[] = { 0 };
    pDeviceContext->IASetVertexBuffers(0, 1, pBufferTbl, SizeTbl, OffsetTbl);

    // IBのセット
    pDeviceContext->IASetIndexBuffer(m_pIndexBuf.Get(), DXGI_FORMAT_R32_UINT, 0);

}

/// パイプラインを描画する
void D3DInputAssembler::Draw(ID3D11DeviceContext *pDeviceContext)
{
    pDeviceContext->DrawIndexed(m_indices // index count
            , 0, 0);
}

