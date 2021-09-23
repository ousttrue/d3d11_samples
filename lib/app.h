#pragma once

#include <banana/orbit_camera.h>
#include <d3d11.h>
#include <gorilla/pipeline.h>
#include <gorilla/render_target.h>
#include <gorilla/window.h>
#include <wrl/client.h>

class App {
  template <typename T> using ComPtr = Microsoft::WRL::ComPtr<T>;

  gorilla::Window _window;
  HWND _hwnd = nullptr;
  ComPtr<ID3D11Device> _device;
  ComPtr<ID3D11DeviceContext> _context;
  ComPtr<IDXGISwapChain> _swapchain;

  DXGI_SWAP_CHAIN_DESC _desc;
  UINT _frame_count = 0;
  gorilla::RenderTarget _render_target;

  // gizmo
  gorilla::Pipeline _grid;

public:
  gorilla::Window &window() { return _window; }
  ComPtr<ID3D11DeviceContext> context() const { return _context; }
  ComPtr<ID3D11Device> initialize(HINSTANCE hInstance, LPSTR lpCmdLine,
                                  int nCmdShow, const char *CLASS_NAME,
                                  const char *WINDOW_TITLE, int width = 1024,
                                  int height = 768);
  bool begin_frame(gorilla::ScreenState *pstate = {});
  void end_frame();
  void clear_depth();
};

inline void update_camera(banana::OrbitCamera *camera,
                          const gorilla::ScreenState &state) {
  camera->update(state.mouse_x, state.mouse_y, state.width, state.height,
                 state.mouse_button_flag & gorilla::MouseButtonLeftDown,
                 state.mouse_button_flag & gorilla::MouseButtonRightDown,
                 state.mouse_button_flag & gorilla::MouseButtonMiddleDown,
                 state.wheel);
}
