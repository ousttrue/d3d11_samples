#pragma once
#include <wrl/client.h>
#include <memory>
#include <string>


class Shader
{
    Microsoft::WRL::ComPtr<struct ID3D11VertexShader> m_pVsh;
    Microsoft::WRL::ComPtr<struct ID3D11PixelShader> m_pPsh;
    Microsoft::WRL::ComPtr<struct ID3D11InputLayout> m_pInputLayout;
	std::shared_ptr<class InputAssemblerSource> m_IASource;
	std::shared_ptr<class ConstantBuffer> m_constant;
	std::shared_ptr<class Texture> m_texture;

public:
	Shader();
    bool Initialize(
            const Microsoft::WRL::ComPtr<struct ID3D11Device> &pDevice
            , const std::string &shaderSource, const std::wstring &textureFile);
	void Draw(
            const Microsoft::WRL::ComPtr<struct ID3D11DeviceContext> &pDeviceContext);
	void Animation();

private:
	bool createShaders(
            const Microsoft::WRL::ComPtr<struct ID3D11Device> &pDevice
		, const std::string &shaderSource, const std::string &vsFunc, const std::string &psFunc);
};

