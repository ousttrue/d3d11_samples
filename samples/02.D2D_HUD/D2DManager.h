#include "../01.CreateDevice/DXGIManager.h"
#include "HUD.h"
#include <memory>
#include <d2d1_1.h>
#include <dwrite.h>


///
/// D2DÇÃÉäÉ\Å[ÉXä«óù
///
class D2DManager: public IRenderResourceManager
{
	Microsoft::WRL::ComPtr<ID2D1Device> m_pD2DDevice;
	Microsoft::WRL::ComPtr<ID2D1DeviceContext> m_pD2DDeviceContext;
    Microsoft::WRL::ComPtr<ID2D1Bitmap1> m_pBitmap;

    // ui
    std::shared_ptr<HUD> m_hud;

public:
    D2DManager();
    ~D2DManager();
    void OnWindowResize(int w, int h)override;
    void OnDestroyDevice()override;
    void Render(DXGIManager *pDxgi)override;

	std::shared_ptr<HUD> GetHUD()const{ return m_hud; }
};

