#include "D3DManager.h"
#include <assert.h>


D3DManager::D3DManager()
    : m_swapchainRTV(new D3DSwapChainRTVResource)
{}

D3DManager::~D3DManager()
{
}

void D3DManager::OnDestroyDevice()
{
    m_swapchainRTV->Release();
}

void D3DManager::OnWindowResize(int w, int h)
{
    m_swapchainRTV->Release();
}

void D3DManager::Render(DXGIManager *pDxgi)
{
    // ensure created
    m_swapchainRTV->Create(pDxgi);

	if (auto pDeviceContext = pDxgi->GetD3DDeviceContext()){

		// draw
		float clearColor[] = { 0.0f, 1.0f, 0.0f, 1.0f };
		m_swapchainRTV->Clear(pDeviceContext.Get(), clearColor);

	}
}

