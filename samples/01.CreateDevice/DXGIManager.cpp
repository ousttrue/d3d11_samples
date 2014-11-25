#include "DXGIManager.h"


DXGIManager::DXGIManager()
	: m_isFullscreen(FALSE)
{
}

DXGIManager::~DXGIManager()
{
    DestroyDevice();
}

void DXGIManager::DestroyDevice()
{
	m_pSwapChain.Reset();
	m_pDevice.Reset();
}

Microsoft::WRL::ComPtr<ID3D11Device> DXGIManager::GetD3DDevice()const
{
    return m_pDevice;
}

Microsoft::WRL::ComPtr<ID3D11DeviceContext> DXGIManager::GetD3DDeviceContext()const
{
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContext;
    m_pDevice->GetImmediateContext(pDeviceContext.GetAddressOf());
    return pDeviceContext;
}

Microsoft::WRL::ComPtr<IDXGISwapChain1> DXGIManager::GetSwapChain()const
{
    return m_pSwapChain;
}

Microsoft::WRL::ComPtr<IDXGIDevice1> DXGIManager::GetDXGIDevice()const
{
    Microsoft::WRL::ComPtr<IDXGIDevice1> pDXGIDevice;
    HRESULT hr = m_pDevice.As(&pDXGIDevice);
	if (FAILED(hr))return pDXGIDevice;

    return pDXGIDevice;
}

// ID3D11DeviceからIDXGIFactory2をゲットする
Microsoft::WRL::ComPtr<IDXGIFactory2> DXGIManager::GetFactory()const
{
    Microsoft::WRL::ComPtr<IDXGIFactory2> pDXGIFactory;
    if (!m_pDevice)return pDXGIFactory;

    auto pDXGIDevice=GetDXGIDevice();
    if(!pDXGIDevice)return pDXGIFactory;

    Microsoft::WRL::ComPtr<IDXGIAdapter> pDXGIAdapter;
    auto hr = pDXGIDevice->GetAdapter(pDXGIAdapter.GetAddressOf());
    if (FAILED(hr))return pDXGIFactory;

    hr = pDXGIAdapter->GetParent(IID_PPV_ARGS(pDXGIFactory.GetAddressOf()));
    if (FAILED(hr))return pDXGIFactory;

    return pDXGIFactory;
}

Microsoft::WRL::ComPtr<IDXGIAdapter> DXGIManager::GetAdapter(
        UINT adapterIndex)const
{
    Microsoft::WRL::ComPtr<IDXGIAdapter> pDXGIAdapter;

    auto pDXGIFactory=GetFactory();
    if(!pDXGIFactory)return pDXGIAdapter;

    auto hr=pDXGIFactory->EnumAdapters(adapterIndex, &pDXGIAdapter);
    if(FAILED(hr))return pDXGIAdapter;

    return pDXGIAdapter;
}

Microsoft::WRL::ComPtr<struct IDXGISurface> DXGIManager::GetSurface()const
{
    Microsoft::WRL::ComPtr<IDXGISurface> pDXGISurface;
    auto hr=m_pSwapChain->GetBuffer(0, IID_PPV_ARGS(pDXGISurface.GetAddressOf()));
    if(FAILED(hr))return pDXGISurface;

    return pDXGISurface;
}

bool DXGIManager::CreateDevice(UINT adapterIndex)
{
	DestroyDevice();

	// CreateDevice
	D3D_DRIVER_TYPE dtype = D3D_DRIVER_TYPE_HARDWARE;
	UINT flags = D3D11_CREATE_DEVICE_BGRA_SUPPORT; // for D2D
#ifdef _DEBUG
	flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
	D3D_FEATURE_LEVEL featureLevels[] = {
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
		D3D_FEATURE_LEVEL_9_3,
		D3D_FEATURE_LEVEL_9_2,
		D3D_FEATURE_LEVEL_9_1,
	};
	UINT numFeatureLevels = sizeof(featureLevels) / sizeof(D3D_FEATURE_LEVEL);
	UINT sdkVersion = D3D11_SDK_VERSION;
	D3D_FEATURE_LEVEL validFeatureLevel;
	HRESULT hr = D3D11CreateDevice(GetAdapter(adapterIndex).Get()
            , dtype, nullptr, flags
            , featureLevels, numFeatureLevels, sdkVersion
            , m_pDevice.ReleaseAndGetAddressOf(), &validFeatureLevel, nullptr);
	if (FAILED(hr))return false;

	return true;
}

void DXGIManager::Resize(int w, int h)
{
	if (!m_pSwapChain)return;

	// レンダーターゲット破棄
    for(auto it=m_resourceManagers.begin(); it!=m_resourceManagers.end(); ++it)
    {
        (*it)->OnWindowResize(w, h);
    }

    // resize
	m_resizeRequest = std::make_pair(w, h);

    // fullscreen
	Microsoft::WRL::ComPtr<IDXGIOutput> pOutput;
	BOOL isFullscreen;
	auto hr = m_pSwapChain->GetFullscreenState(&isFullscreen, pOutput.GetAddressOf());
	m_fullscreenRequest = isFullscreen;
}

void DXGIManager::Render(HWND hWnd)
{
    if(BeginRender(hWnd)){

        for(auto &manager: m_resourceManagers)
        {
			manager->Render(this);
        }

        EndRender();
    }
}

/// hWndに対するSwapChainを作る
static Microsoft::WRL::ComPtr<IDXGISwapChain1> 
CreateSwapChainForWindow(
        DXGIManager *pDxgi, HWND hWnd)
{
    Microsoft::WRL::ComPtr<IDXGISwapChain1> pSwapChain;

    auto pDXGIFactory=pDxgi->GetFactory();
    if(!pDXGIFactory)return pSwapChain;

	DXGI_SWAP_CHAIN_FULLSCREEN_DESC sfd = { 0 };
	sfd.Windowed = TRUE;

	DXGI_SWAP_CHAIN_DESC1 sd = { 0 };
	sd.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
#if 1
	sd.BufferCount = 2;
	sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
#else
	sd.BufferCount = 1;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
#endif
	//sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	//sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	
	auto hr = pDXGIFactory->CreateSwapChainForHwnd(pDxgi->GetDXGIDevice().Get(),
		hWnd,
		&sd,
		&sfd,
		nullptr,
		pSwapChain.GetAddressOf());
	if (FAILED(hr))return pSwapChain;

#if 1
	// alt + F5のフルスクリーンを止める
	pDXGIFactory->MakeWindowAssociation(hWnd, DXGI_MWA_NO_WINDOW_CHANGES
		| DXGI_MWA_NO_ALT_ENTER);
#endif

    return pSwapChain;
}

bool DXGIManager::BeginRender(HWND hWnd)
{
	if (!m_pSwapChain){
		m_pSwapChain = CreateSwapChainForWindow(this, hWnd);
	}
    if(!m_pSwapChain)return false;

    // resize buffer
    DXGI_SWAP_CHAIN_DESC sd;
    m_pSwapChain->GetDesc(&sd);

    if (m_resizeRequest){
        auto hr=m_pSwapChain->ResizeBuffers(sd.BufferCount,
            m_resizeRequest->first, m_resizeRequest->second,
            sd.BufferDesc.Format,
            sd.Flags // flags
            );
        m_resizeRequest.reset();
        if (FAILED(hr)){
            return false;
        }
    }

    if (m_fullscreenRequest){
        m_isFullscreen = *m_fullscreenRequest;
        m_fullscreenRequest.reset();
    }

    return true;
}

// update swapchain
void DXGIManager::EndRender()
{
	m_pSwapChain->Present(NULL, NULL);
}

