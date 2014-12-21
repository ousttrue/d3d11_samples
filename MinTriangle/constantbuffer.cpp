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

class ConstantBufferByStage
{
public:
	SHADERSTAGE Stage;
	std::vector<ConstantBufferSlot> Slots;

	ConstantBufferByStage(SHADERSTAGE stage)
		: Stage(stage)
	{}

    void AddSlot(const Microsoft::WRL::ComPtr<ID3D11Device> &device, size_t size)
    {
		D3D11_BUFFER_DESC desc = { 0 };
		desc.ByteWidth = size;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		Microsoft::WRL::ComPtr<ID3D11Buffer> pBuffer;
		HRESULT hr = device->CreateBuffer(&desc, nullptr, &pBuffer);

		Slots.push_back(ConstantBufferSlot(pBuffer, size));
    }

    void AddVariable(size_t slotIndex, D3D11_SHADER_VARIABLE_DESC &desc)
    {
		if (slotIndex >= Slots.size()){
			return;
		}
		auto &slot = Slots[slotIndex];
		slot.Variables.push_back(ConstantVariable(Stage
			, slotIndex, desc));
    }

    bool GetVariable(const std::string &name, ConstantVariable *pOut)
    {
		for (auto &slot : Slots)
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
};


bool ConstantBuffer::Initialize(const Microsoft::WRL::ComPtr<ID3D11Device> &pDevice
        , SHADERSTAGE stage
        , const Microsoft::WRL::ComPtr<struct ID3D11ShaderReflection> &pReflector)
{
	assert(!m_constants[stage]);
    auto impl = std::make_shared<ConstantBufferByStage>(stage);
	m_constants[stage] = impl;

    D3D11_SHADER_DESC shaderdesc;
    pReflector->GetDesc(&shaderdesc);

    // analize constant buffer
    for (size_t i = 0; i < shaderdesc.ConstantBuffers; ++i){
        auto cb = pReflector->GetConstantBufferByIndex(i);
        D3D11_SHADER_BUFFER_DESC desc;
        cb->GetDesc(&desc);
        OutputDebugPrintfA("[%d: %s]\n", i, desc.Name);
        impl->AddSlot(pDevice, desc.Size);
        for (size_t j = 0; j < desc.Variables; ++j){
            auto v = cb->GetVariableByIndex(j);
            D3D11_SHADER_VARIABLE_DESC vdesc;
            v->GetDesc(&vdesc);
            OutputDebugPrintfA("(%d) %s %d\n", j, vdesc.Name, vdesc.StartOffset);
            impl->AddVariable(i, vdesc);
        }
    }

    return true;
}

void ConstantBuffer::Update(const Microsoft::WRL::ComPtr<ID3D11DeviceContext> &pDeviceContext)
{
	for (auto stage : m_constants){
		for (auto &slot : stage->Slots){
			pDeviceContext->UpdateSubresource(slot.Buffer.Get(), 0, nullptr
				, &slot.BackingStore[0], 0, 0);
		}
	}
}

void ConstantBuffer::Set(const Microsoft::WRL::ComPtr<ID3D11DeviceContext> &pDeviceContext)
{
	for (auto stage : m_constants){
		UINT slotIndex = 0;
		for (auto &slot : stage->Slots){
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

ConstantVariable ConstantBuffer::GetVariable(const std::string &name)
{
	ConstantVariable v;
	for (auto stage : m_constants)
	{
		if (stage){
			if (stage->GetVariable(name, &v)){
				break;
			}
		}
	}
	return v;
}

bool ConstantBuffer::SetValue(const ConstantVariable &v, const DirectX::XMFLOAT4X4 &m)
{
	m_constants[v.Stage]->Slots[v.Slot].SetValue(
		v.Desc.StartOffset, v.Desc.Size, (const unsigned char*)&m);
	return true;
}
