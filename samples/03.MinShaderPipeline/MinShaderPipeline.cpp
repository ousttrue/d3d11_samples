#include "MinShaderPipeline.h"
#include <assert.h>


MinShaderPipeline::MinShaderPipeline(const ShaderInfo &info)
    : m_swapchainRTV(new D3DSwapChainRTVResource)
    , m_shader(new D3DShader)
    , m_ia(new D3DInputAssembler)
{
    m_shader->SetInfo(info);
}

MinShaderPipeline::~MinShaderPipeline()
{
}

void MinShaderPipeline::OnDestroyDevice()
{
    m_swapchainRTV->Release();
    m_shader->Release();
    m_ia->Release();
}

void MinShaderPipeline::OnWindowResize(int w, int h)
{
    m_swapchainRTV->Release();
}

void MinShaderPipeline::Render(DXGIManager *pDxgi)
{
    // ensure created
    m_swapchainRTV->Create(pDxgi);
    m_shader->Create(pDxgi);
    m_ia->Create(pDxgi);

	if (auto pDeviceContext = pDxgi->GetD3DDeviceContext()){

		// clear
		float clearColor[] = { 0.0f, 0.0f, 0.0f, 1.0f };
		m_swapchainRTV->Clear(pDeviceContext.Get(), clearColor);

        // setup pipeline
        m_swapchainRTV->SetupPipeline(pDeviceContext.Get());
        m_ia->SetupPipeline(pDeviceContext.Get());
        m_shader->SetupPipeline(pDeviceContext.Get());

        // draw
        m_ia->Draw(pDeviceContext.Get());
	}
}
