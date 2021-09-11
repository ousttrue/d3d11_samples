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
/// �f�o�C�X���琶�����郊�\�[�X
///
class IRenderResource
{
public:
    virtual ~IRenderResource(){}

    ///
    /// ��������
    ///
    virtual bool Create(DXGIManager *pDxgi)=0;

    ///
    /// �������
    ///
    virtual void Release()=0;

    ///
    /// �p�C�v���C���ɃZ�b�g����
    ///
    virtual void SetupPipeline(ID3D11DeviceContext *pDeviceContext)=0;
};


///
/// ���\�[�X�̊m�ہE����ƕ`����Ǘ�����
///
class IRenderResourceManager
{
public:
    virtual ~IRenderResourceManager(){}

    ///
    /// �f�o�C�X���j�����ꂽ
    ///
    virtual void OnDestroyDevice()=0;

    ///
    /// �E�C���h�E�T�C�Y���ύX���ꂽ
    ///
    virtual void OnWindowResize(int w, int h)=0;

    ///
    /// �`��
    ///
    virtual void Render(DXGIManager *pDxgi)=0;
};


///
/// ID3D11Device��IDXGISwapChain1���Ǘ�����
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

