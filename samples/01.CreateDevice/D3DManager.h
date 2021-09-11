#include "D3DResource.h"

///
/// D3D11�̃��\�[�X���Ǘ�����
///
class D3DManager: public IRenderResourceManager
{
    std::shared_ptr<D3DSwapChainRTVResource> m_swapchainRTV;

public:
    D3DManager();
    ~D3DManager();

    void OnDestroyDevice()override;
    void OnWindowResize(int w, int h)override;
    void Render(DXGIManager *pDxgi)override;
};

