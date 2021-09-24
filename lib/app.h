#pragma once

#include "update_camera.h"
#include <banana/orbit_camera.h>
#include <chrono>
#include <d3d11.h>
#include <gorilla/renderer.h>
#include <gorilla/window.h>
#include <wrl/client.h>

class App {
  template <typename T> using ComPtr = Microsoft::WRL::ComPtr<T>;

  gorilla::Window _window;
  gorilla::Renderer _renderer;
  ComPtr<ID3D11Device> _device;
  ComPtr<ID3D11DeviceContext> _context;
  UINT _frame_count = 0;

  std::chrono::system_clock::time_point _last = {};

public:
  float clear[4] = {0.5f, 0.5f, 0.5f, 1.0f};

  ~App();
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
