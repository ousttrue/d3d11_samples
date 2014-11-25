#pragma once
#include <d3d11.h>
#include <dxgi1_2.h>
#include <Windows.h>
#include <wrl/client.h>
#include <boost/optional.hpp>
#include <list>
#include <memory>

class DXGIManager;

///
/// デバイスから生成するリソース
///
class IRenderResource
{
public:
    virtual ~IRenderResource(){}

    ///
    /// 生成する
    ///
    virtual bool Create(DXGIManager *pDxgi)=0;

    ///
    /// 解放する
    ///
    virtual void Release()=0;

    ///
    /// パイプラインにセットする
    ///
    virtual void SetupPipeline(ID3D11DeviceContext *pDeviceContext)=0;
};


///
/// リソースの確保・解放と描画を管理する
///
class IRenderResourceManager
{
public:
    virtual ~IRenderResourceManager(){}

    ///
    /// デバイスが破棄された
    ///
    virtual void OnDestroyDevice()=0;

    ///
    /// ウインドウサイズが変更された
    ///
    virtual void OnWindowResize(int w, int h)=0;

    ///
    /// 描画
    ///
    virtual void Render(DXGIManager *pDxgi)=0;
};


///
/// ID3D11DeviceとIDXGISwapChain1を管理する
///
class DXGIManager
{
	Microsoft::WRL::ComPtr<ID3D11Device> m_pDevice;
	Microsoft::WRL::ComPtr<IDXGISwapChain1> m_pSwapChain;

	BOOL m_isFullscreen;

	boost::optional<BOOL> m_fullscreenRequest;
	boost::optional<std::pair<int, int>> m_resizeRequest;

    std::list<std::shared_ptr<IRenderResourceManager>> m_resourceManagers;

    DXGIManager(const DXGIManager &);
    DXGIManager& operator=(const DXGIManager &);
public:
    DXGIManager();
    ~DXGIManager();

    Microsoft::WRL::ComPtr<ID3D11Device> GetD3DDevice()const;
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> GetD3DDeviceContext()const;
    Microsoft::WRL::ComPtr<IDXGISwapChain1> GetSwapChain()const;
	Microsoft::WRL::ComPtr<IDXGIDevice1> GetDXGIDevice()const;
	Microsoft::WRL::ComPtr<IDXGIFactory2> GetFactory()const;
    Microsoft::WRL::ComPtr<IDXGIAdapter> GetAdapter(UINT adapterIndex)const;
    Microsoft::WRL::ComPtr<IDXGISurface> GetSurface()const;

    bool CreateDevice(UINT adapterIndex=0);
    void Resize(int w, int h);
    void DestroyDevice();

    void AddResourceManager(const std::shared_ptr<IRenderResourceManager> &resourceManager)
    {
		m_resourceManagers.push_back(resourceManager);
    }

    void Render(HWND hWnd);

private:
    bool BeginRender(HWND hWnd);
    void EndRender();
};

