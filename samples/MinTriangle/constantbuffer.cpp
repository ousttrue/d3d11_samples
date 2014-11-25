#include "constantbuffer.h"
#include "debugprint.h"
#include <d3d11.h>
#include <d3dcompiler.h>
#include <vector>


ConstantVariable::ConstantVariable()
{
	ZeroMemory(&Desc, sizeof(Desc));
}

ConstantVariable::ConstantVariable(SHADERSTAGE stage, size_t slot, const D3D11_SHADER_VARIABLE_DESC &desc)
	: Stage(stage), Slot(slot), Name(desc.Name), Desc(desc)
{
}

struct ConstantBufferSlot
{
	Microsoft::WRL::ComPtr<ID3D11Buffer> Buffer;
	std::vector<ConstantVariable> Variables;
	std::vector<unsigned char> BackingStore;

	ConstantBufferSlot(const Microsoft::WRL::ComPtr<ID3D11Buffer> &buffer, size_t size)
		: Buffer(buffer), BackingStore(size)
	{}

	void SetValue(size_t offset, size_t bytes, const unsigned char *begin)
	{
		std::copy(begin, begin + bytes, &BackingStore[offset]);
	}
};


class VariablesByStage
{
public:
	SHADERSTAGE Stage;
	std::vector<ConstantBufferSlot> CBSlots;
	std::vector<ShaderResourceSlot> SRVSlots;
	std::vector<ShaderResourceSlot> SamplerSlots;

	VariablesByStage(SHADERSTAGE stage)
		: Stage(stage)
	{}

    void AddCBSlot(const Microsoft::WRL::ComPtr<ID3D11Device> &device, UINT size)
    {
		D3D11_BUFFER_DESC desc = { 0 };
		desc.ByteWidth = size;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		Microsoft::WRL::ComPtr<ID3D11Buffer> pBuffer;
		HRESULT hr = device->CreateBuffer(&desc, nullptr, &pBuffer);

		CBSlots.push_back(ConstantBufferSlot(pBuffer, size));
    }

    void AddCBVariable(size_t slotIndex, D3D11_SHADER_VARIABLE_DESC &desc)
    {
		if (slotIndex >= CBSlots.size()){
			return;
		}
		auto &slot = CBSlots[slotIndex];
		slot.Variables.push_back(ConstantVariable(Stage
			, slotIndex, desc));
    }

    bool GetCBVariable(const std::string &name, ConstantVariable *pOut)
    {
		for (auto &slot : CBSlots)
		{
			for (auto &v : slot.Variables){
				if (name == v.Name)
				{
					if (pOut){
						*pOut = v;
						return true;
					}
				}
			}
		}
		return false;
    }

	void AddSRVSlot(const D3D11_SHADER_INPUT_BIND_DESC &desc)
	{
		SRVSlots.push_back(ShaderResourceSlot{Stage, desc.Name, desc});
	}

	void AddSamplerSlot(const D3D11_SHADER_INPUT_BIND_DESC &desc)
	{
		SamplerSlots.push_back(ShaderResourceSlot{Stage, desc.Name, desc });
	}
};


bool ConstantBuffer::Initialize(const Microsoft::WRL::ComPtr<ID3D11Device> &pDevice
        , SHADERSTAGE stage
        , const Microsoft::WRL::ComPtr<struct ID3D11ShaderReflection> &pReflector)
{
	assert(!m_constants[stage]);
    auto impl = std::make_shared<VariablesByStage>(stage);
	m_constants[stage] = impl;

    D3D11_SHADER_DESC shaderdesc;
    pReflector->GetDesc(&shaderdesc);

    // analize constant buffer
    for (UINT i = 0; i < shaderdesc.ConstantBuffers; ++i){
        auto cb = pReflector->GetConstantBufferByIndex(i);
        D3D11_SHADER_BUFFER_DESC desc;
        cb->GetDesc(&desc);
        OutputDebugPrintfA("[%d: %s]\n", i, desc.Name);
        impl->AddCBSlot(pDevice, desc.Size);
        for (UINT j = 0; j < desc.Variables; ++j){
            auto v = cb->GetVariableByIndex(j);
            D3D11_SHADER_VARIABLE_DESC vdesc;
            v->GetDesc(&vdesc);
            OutputDebugPrintfA("(%d) %s %d\n", j, vdesc.Name, vdesc.StartOffset);
            impl->AddCBVariable(i, vdesc);
        }
    }

	for (UINT i = 0; i < shaderdesc.BoundResources; ++i){
		D3D11_SHADER_INPUT_BIND_DESC desc;
		pReflector->GetResourceBindingDesc(i, &desc);
		switch (desc.Type)
		{
			case D3D_SIT_TEXTURE:
			impl->AddSRVSlot(desc);
			break;

		case D3D_SIT_SAMPLER:
			impl->AddSamplerSlot(desc);
			break;
		}
	}

    return true;
}

void ConstantBuffer::UpdateCB(const Microsoft::WRL::ComPtr<ID3D11DeviceContext> &pDeviceContext)
{
	for (auto stage : m_constants){
		for (auto &slot : stage->CBSlots){
			pDeviceContext->UpdateSubresource(slot.Buffer.Get(), 0, nullptr
				, &slot.BackingStore[0], 0, 0);
		}
	}
}

void ConstantBuffer::SetCB(const Microsoft::WRL::ComPtr<ID3D11DeviceContext> &pDeviceContext)
{
	for (auto stage : m_constants){
		UINT slotIndex = 0;
		for (auto &slot : stage->CBSlots){
			switch (stage->Stage){
			case SHADERSTAGE_VERTEX:
				pDeviceContext->VSSetConstantBuffers(slotIndex, 1, slot.Buffer.GetAddressOf());
				break;

			case SHADERSTAGE_PIXEL:
				pDeviceContext->VSSetConstantBuffers(slotIndex, 1, slot.Buffer.GetAddressOf());
				break;

			default:
				break;
			}

			++slotIndex;
		}
	}
}

ConstantVariable ConstantBuffer::GetCBVariable(const std::string &name)
{
	ConstantVariable v;
	for (auto stage : m_constants)
	{
		if (stage){
			if (stage->GetCBVariable(name, &v)){
				break;
			}
		}
	}
	return v;
}

bool ConstantBuffer::SetCBValue(const ConstantVariable &v, const DirectX::XMFLOAT4X4 &m)
{
	m_constants[v.Stage]->CBSlots[v.Slot].SetValue(
		v.Desc.StartOffset, v.Desc.Size, (const unsigned char*)&m);
	return true;
}

ShaderResourceSlot ConstantBuffer::GetSRV(const std::string &name)const
{
	for (auto stage : m_constants)
	{
		if (stage){
			for (auto &slot : stage->SRVSlots){
				if (slot.Name == name){
					return slot;
				}
			}
		}
	}
	return ShaderResourceSlot();
}

void ConstantBuffer::SetSRV(const Microsoft::WRL::ComPtr<struct ID3D11DeviceContext> &pDeviceContext
	, const ShaderResourceSlot &slot
	, const Microsoft::WRL::ComPtr<struct ID3D11ShaderResourceView> &srv)
{
	switch (slot.Stage)
	{
	case SHADERSTAGE_VERTEX:
		pDeviceContext->VSSetShaderResources(slot.Desc.BindPoint, slot.Desc.BindCount, srv.GetAddressOf());
		break;

	case SHADERSTAGE_PIXEL:
		pDeviceContext->PSSetShaderResources(slot.Desc.BindPoint, slot.Desc.BindCount, srv.GetAddressOf());
		break;
	}
}

ShaderResourceSlot ConstantBuffer::GetSampler(const std::string &name)const
{
	for (auto stage : m_constants)
	{
		if (stage){
			for (auto &slot : stage->SamplerSlots){
				if (slot.Name == name){
					return slot;
				}
			}
		}
	}
	return ShaderResourceSlot();
}

void ConstantBuffer::SetSampler(const Microsoft::WRL::ComPtr<struct ID3D11DeviceContext> &pDeviceContext
	, const ShaderResourceSlot &slot
	, const Microsoft::WRL::ComPtr<struct ID3D11SamplerState> &sampler)
{
	switch (slot.Stage)
	{
	case SHADERSTAGE_VERTEX:
		pDeviceContext->VSSetSamplers(slot.Desc.BindPoint, slot.Desc.BindCount, sampler.GetAddressOf());
		break;

	case SHADERSTAGE_PIXEL:
		pDeviceContext->PSSetSamplers(slot.Desc.BindPoint, slot.Desc.BindCount, sampler.GetAddressOf());
		break;
	}
}
