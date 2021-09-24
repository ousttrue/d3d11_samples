#include "app.h"
#include <banana/asset.h>
#include <banana/types.h>
#include <gorilla/device.h>
#include <gorilla/pipeline.h>
#include <gorilla/swapchain.h>
#include <gorilla/window.h>
#include <imgui.h>
#include <imgui_impl_dx11.h>
#include <iostream>

template <typename T> using ComPtr = Microsoft::WRL::ComPtr<T>;

App::~App() {
  // Cleanup
  ImGui_ImplDX11_Shutdown();
  // ImGui_ImplWin32_Shutdown();
  ImGui::DestroyContext();
}

ComPtr<ID3D11Device> App::initialize(HINSTANCE hInstance, LPSTR lpCmdLine,
                                     int nCmdShow, const char *CLASS_NAME,
                                     const char *WINDOW_TITLE, int width,
                                     int height) {
  auto hwnd =
      _window.create(hInstance, CLASS_NAME, WINDOW_TITLE, width, height);
  if (!hwnd) {
    return {};
  }
  ShowWindow(hwnd, nCmdShow);
  UpdateWindow(hwnd);
  _window.bind_key([hwnd](int key) {
    if (key == 27) {
      // esc
      SendMessage(hwnd, WM_CLOSE, 0, 0);

    } else {
      std::cout << key << std::endl;
    }
  });

  std::tie(_device, _context) = _renderer.create(hwnd);
  if (!_device) {
    return {};
  }

  // imgui
  {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void)io;
    // io.ConfigFlags |=
    //     ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
    // io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable
    // Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;   // Enable Docking
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable; // Enable Multi-Viewport
                                                        // / Platform Windows
    // io.ConfigViewportsNoAutoMerge = true;
    // io.ConfigViewportsNoTaskBarIcon = true;
    // io.ConfigViewportsNoDefaultParent = true;
    // io.ConfigDockingAlwaysTabBar = true;
    // io.ConfigDockingTransparentPayload = true;
    // io.ConfigFlags |= ImGuiConfigFlags_DpiEnableScaleFonts;     // FIXME-DPI:
    // Experimental. THIS CURRENTLY DOESN'T WORK AS EXPECTED. DON'T USE IN USER
    // APP! io.ConfigFlags |= ImGuiConfigFlags_DpiEnableScaleViewports; //
    // FIXME-DPI: Experimental.

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    // ImGui::StyleColorsClassic();

    // When viewports are enabled we tweak WindowRounding/WindowBg so platform
    // windows can look identical to regular ones.
    ImGuiStyle &style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
      style.WindowRounding = 0.0f;
      style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }

    // Setup Platform/Renderer backends
    // ImGui_ImplWin32_Init(hwnd);
    ImGui_ImplDX11_Init(_device.Get(), _context.Get());
  }

  // gizmo
  // auto shader = banana::get_string("grid.hlsl");
  // if (shader.empty()) {
  //   return {};
  // }
  // auto [ok, error] =
  //     _grid.compile_shader(_device, shader, "vsMain", "gsMain", "psMain");
  // if (!ok) {
  //   std::cerr << error << std::endl;
  //   return {};
  // }

  return _device;
}

bool App::begin_frame(gorilla::ScreenState *pstate) {

  if (!_window.process_messages(pstate)) {
    return false;
  }

  // imgui
  {
    ImGuiIO &io = ImGui::GetIO();

    //
    // update custom backend
    //
    if (_last == std::chrono::system_clock::time_point{}) {

    } else {
      io.DeltaTime = std::chrono::duration_cast<std::chrono::milliseconds>(
                         pstate->time - _last)
                         .count() *
                     0.001f;
    }
    if (io.DeltaTime == 0) {
      io.DeltaTime = 0.016f;
    }
    _last = pstate->time;
    io.DisplaySize = {pstate->width, pstate->height};
    io.MousePos = {pstate->mouse_x, pstate->mouse_y};
    io.MouseDown[0] = pstate->mouse_button_flag & gorilla::MouseButtonLeftDown;
    io.MouseDown[1] = pstate->mouse_button_flag & gorilla::MouseButtonRightDown;
    io.MouseWheel = pstate->wheel;

    // update

    // Start the Dear ImGui frame
    ImGui_ImplDX11_NewFrame();
    // ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
  }

  // clear RTV
  _renderer.begin_frame(*pstate, clear);

  // gizmo
#pragma pack(push)
#pragma pack(16)
  struct Constants {
    banana::Matrix4x4 view;
    banana::Matrix4x4 projection;
    DirectX::XMFLOAT3 cameraPosition;
    float _padding2;
    banana::Float2 screenSize;
    float fovY;
    float _padding3;
  };
#pragma pack(pop)
  static_assert(sizeof(Constants) == 16 * 10, "sizeof ConstantsSize");
  // Constants constant;
  // constant.fovY = _camera.fovYRad;
  // constant.screenSize.x = static_cast<float>(w);
  // constant.screenSize.y = static_cast<float>(h);
  // constant.view = _camera.view;
  // constant.projection = _camera.projection;
  // constant.cameraPosition = _camera.position();
  // _grid.gs_stage.cb[0].update(_context, constant);
  // _grid.ps_stage.cb[0].update(_context, constant);
  // _grid.setup(_context);
  // _grid.draw_empty(_context);

  return true;
}

void App::end_frame() {
  // imgui
  ImGui::Render();
  ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
  _renderer.end_frame();
  _frame_count++;
}

void App::clear_depth() { _renderer.clear_depth(); }
