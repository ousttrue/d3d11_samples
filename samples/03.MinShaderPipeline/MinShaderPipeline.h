#pragma once
#include "D3DShaderResource.h"

///
/// D3D11‚ÌƒŠƒ\[ƒX‚ğŠÇ—‚·‚é
///
class MinShaderPipeline: public IRenderResourceManager
{
    std::shared_ptr<D3DSwapChainRTVResource> m_swapchainRTV;
    std::shared_ptr<D3DShader> m_shader;
    std::shared_ptr<D3DInputAssembler> m_ia;

public:
    MinShaderPipeline(const ShaderInfo &info);
    ~MinShaderPipeline();

    void OnDestroyDevice()override;
    void OnWindowResize(int w, int h)override;
    void Render(DXGIManager *pDxgi)override;
};

