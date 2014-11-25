//------------------------------------------------------------------------------
// <copyright file="DX11Utils.cpp" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
//------------------------------------------------------------------------------

#include "CompileShaderFromFile.h"
#include <vector>
#include <fstream>

#if 0
#include <d3dx11.h>

/// <summary>
/// Helper for compiling shaders with D3DX11
/// </summary>
/// <param name="szFileName">full path to shader to compile</param>
/// <param name="szEntryPoint">entry point of shader</param>
/// <param name="szShaderModel">shader model to compile for</param>
/// <param name="ppBlobOut">holds result of compilation</param>
/// <returns>S_OK for success, or failure code</returns>
HRESULT CompileShaderFromFile(const WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3D10Blob** ppBlobOut)
{
    HRESULT hr = S_OK;

    ID3D10Blob* pErrorBlob = NULL;

    hr = D3DX11CompileFromFileW( szFileName, NULL, NULL, szEntryPoint, szShaderModel, 
        0, 0, NULL, ppBlobOut, &pErrorBlob, NULL );

    if ( FAILED(hr) )
    {
        if (NULL != pErrorBlob)
        {
            OutputDebugStringA( (char*)pErrorBlob->GetBufferPointer() );
        }
    }

    SAFE_RELEASE(pErrorBlob);

    return hr;
}

#else
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
	// trim tail zero
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

HRESULT CompileShaderFromFile
(
 const WCHAR*      szFileName,
 LPCSTR      szEntryPoint,
 LPCSTR      szShaderModel,
 ID3DBlob**  ppBlobOut
 )
{
    HRESULT hr = S_OK;

    DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;

#if defined(DEBUG) || defined(_DEBUG)
    dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif//defiend(DEBUG) || defined(_DEBUG)

#if defined(NDEBUG) || defined(_NDEBUG)
    dwShaderFlags |= D3DCOMPILE_OPTIMIZATION_LEVEL3;
#endif//defined(NDEBUG) || defined(_NDEBUG)

    ID3DBlob* pErrorBlob = NULL;

    hr = D3DCompileFromFile(
            szFileName,
            NULL,
            D3D_COMPILE_STANDARD_FILE_INCLUDE,
            szEntryPoint,
            szShaderModel,
            dwShaderFlags,
            0,
            ppBlobOut,
            &pErrorBlob 
            );

    if ( FAILED( hr ) )
    {
        if ( pErrorBlob != NULL )
        { OutputDebugStringA( (char*)pErrorBlob->GetBufferPointer() ); }
    }

    if ( pErrorBlob )
    {
        pErrorBlob->Release();
        pErrorBlob = NULL;
    }

    return hr;
}
#endif

HRESULT CompileShaderFromSource(const CHAR* szFileName, const CHAR *source, int sourceSize, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3D10Blob** ppBlobOut)
{
	HRESULT hr = S_OK;

	DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;

#if defined(DEBUG) || defined(_DEBUG)
	dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif//defiend(DEBUG) || defined(_DEBUG)

#if defined(NDEBUG) || defined(_NDEBUG)
	dwShaderFlags |= D3DCOMPILE_OPTIMIZATION_LEVEL3;
#endif//defined(NDEBUG) || defined(_NDEBUG)

	ID3DBlob* pErrorBlob = NULL;

	hr = D3DCompile(
		source,
		sourceSize,
		szFileName,
		NULL,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		szEntryPoint,
		szShaderModel,
		dwShaderFlags,
		0,
		ppBlobOut,
		&pErrorBlob
	);

	if (FAILED(hr))
	{
		if (pErrorBlob != NULL)
		{
			OutputDebugStringA((char*)pErrorBlob->GetBufferPointer());
		}
	}

	if (pErrorBlob)
	{
		pErrorBlob->Release();
		pErrorBlob = NULL;
	}

	return hr;
}
