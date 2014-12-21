#pragma once
#include <wrl/client.h>
#include <DirectXMath.h>
#include <d3dcompiler.h>
#include <vector>
#include <memory>


enum SHADERSTAGE
{
	SHADERSTAGE_VERTEX,
	SHADERSTAGE_PIXEL,

	SHADERSTAGE_SIZE,
};


struct ConstantVariable
{
    SHADERSTAGE Stage;
    size_t Slot;
    std::string Name;
	D3D11_SHADER_VARIABLE_DESC Desc;

    ConstantVariable();
	ConstantVariable(SHADERSTAGE stage, size_t slot, const D3D11_SHADER_VARIABLE_DESC &desc);
};


struct ShaderResourceSlot
{
	SHADERSTAGE Stage;
	std::string Name;
	D3D11_SHADER_INPUT_BIND_DESC Desc;
};


class ConstantBuffer
{
	std::shared_ptr<class VariablesByStage> m_constants[SHADERSTAGE_SIZE];

public:
	bool Initialize(const Microsoft::WRL::ComPtr<struct ID3D11Device> &pDevice
			, SHADERSTAGE stage
            , const Microsoft::WRL::ComPtr<struct ID3D11ShaderReflection> &pReflector);

	void UpdateCB(const Microsoft::WRL::ComPtr<struct ID3D11DeviceContext> &pDeviceContext);
	void SetCB(const Microsoft::WRL::ComPtr<struct ID3D11DeviceContext> &pDeviceContext);
	ConstantVariable GetCBVariable(const std::string &name);
    bool SetCBValue(const ConstantVariable &v, const DirectX::XMFLOAT4X4 &m);

	ShaderResourceSlot GetSRV(const std::string &name)const;
	void SetSRV(const Microsoft::WRL::ComPtr<struct ID3D11DeviceContext> &pDeviceContext
		, const ShaderResourceSlot &slot
		, const Microsoft::WRL::ComPtr<struct ID3D11ShaderResourceView> &srv);

	ShaderResourceSlot GetSampler(const std::string &name)const;
	void SetSampler(const Microsoft::WRL::ComPtr<struct ID3D11DeviceContext> &pDeviceContext
		, const ShaderResourceSlot &slot
		, const Microsoft::WRL::ComPtr<struct ID3D11SamplerState> &sampler);
};
