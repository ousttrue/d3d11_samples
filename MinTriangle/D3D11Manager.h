#include <d3d11.h>
#include "ResPtr.h"
#include <string>
#include <memory>


class D3D11Manager
{
    ResPtr<IDXGISwapChain> m_pSwapChain;
    ResPtr<ID3D11Device> m_pDevice;
    ResPtr<ID3D11DeviceContext> m_pDeviceContext;

    std::shared_ptr<class RenderTarget> m_renderTarget;
    std::shared_ptr<class Shader> m_shader;
    std::shared_ptr<class InputAssemblerSource> m_IASource;

public:
    D3D11Manager();
    ~D3D11Manager();
    bool Initialize(HWND hWnd, const std::wstring &shaderFile);
    void Resize(int w, int h);
    void Render();
};

