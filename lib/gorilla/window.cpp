#include "window.h"
#include <chrono>
#include <fstream>

#include <nlohmann/json.hpp>
auto INI_FILE = "sample.ini";
struct WindowIni {
  int x;
  int y;
  int w;
  int h;
  bool maximized;
};
namespace ns {
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(WindowIni, x, y, w, h, maximized);
}

static BOOL SetClientSize(HWND hWnd, int width, int height) {
  RECT rw, rc;
  ::GetWindowRect(hWnd, &rw);
  ::GetClientRect(hWnd, &rc);

  int new_width = (rw.right - rw.left) - (rc.right - rc.left) + width;
  int new_height = (rw.bottom - rw.top) - (rc.bottom - rc.top) + height;

  return ::SetWindowPos(hWnd, NULL, 0, 0, new_width, new_height,
                        SWP_NOMOVE | SWP_NOZORDER);
}
namespace gorilla {

static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam,
                                LPARAM lParam) {
  if (message == WM_CREATE) {
    auto p = reinterpret_cast<LPCREATESTRUCT>(lParam);
    SetWindowLongPtr(hWnd, GWLP_USERDATA,
                     reinterpret_cast<LONG_PTR>(p->lpCreateParams));
    return 0;
  }

  auto w = reinterpret_cast<Window *>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
  if (w) {
    return w->proc(hWnd, message, wParam, lParam);
  } else {
    return DefWindowProcA(hWnd, message, wParam, lParam);
  }
}

Window::~Window() { UnregisterClassA(_class_name.c_str(), _instance); }

LRESULT Window::proc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
  switch (message) {
  case WM_DESTROY: {
    PostQuitMessage(0);
    return 0;
  }

  case WM_ERASEBKGND:
    return 0;

  case WM_SIZE:
    _state.width = LOWORD(lParam);
    _state.height = HIWORD(lParam);
    _is_maximized = wParam == SIZE_MAXIMIZED;
    return 0;

  case WM_PAINT: {
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(hWnd, &ps);
    EndPaint(hWnd, &ps);
    return 0;
  }

  //
  // MOUSE EVENTS
  //
  case WM_MOUSEMOVE: {
    auto pos = MAKEPOINTS(lParam);
    _state.mouse_x = pos.x;
    _state.mouse_y = pos.y;
    for (auto &callback : _on_move) {
      callback(pos.x, pos.y);
    }
    return 0;
  }

  case WM_LBUTTONDOWN: {
    _state.mouse_button_flag |= MouseButtonLeftDown;
    SetCapture(hWnd);
    for (auto &callback : _on_left) {
      callback(true);
    }
    return 0;
  }

  case WM_RBUTTONDOWN: {
    _state.mouse_button_flag |= MouseButtonRightDown;
    SetCapture(hWnd);
    for (auto &callback : _on_right) {
      callback(true);
    }
    return 0;
  }

  case WM_MBUTTONDOWN: {
    _state.mouse_button_flag |= MouseButtonMiddleDown;
    SetCapture(hWnd);
    for (auto &callback : _on_middle) {
      callback(true);
    }
    return 0;
  }

  case WM_LBUTTONUP: {
    _state.mouse_button_flag &= ~MouseButtonLeftDown;
    if (_state.mouse_button_flag == MouseButtonNone) {
      ReleaseCapture();
    }
    for (auto &callback : _on_left) {
      callback(false);
    }
    return 0;
  }

  case WM_RBUTTONUP: {
    _state.mouse_button_flag &= ~MouseButtonRightDown;
    if (_state.mouse_button_flag == MouseButtonNone) {
      ReleaseCapture();
    }
    for (auto &callback : _on_right) {
      callback(false);
    }
    return 0;
  }

  case WM_MBUTTONUP: {
    _state.mouse_button_flag &= ~MouseButtonMiddleDown;
    if (_state.mouse_button_flag == MouseButtonNone) {
      ReleaseCapture();
    }
    for (auto &callback : _on_middle) {
      callback(false);
    }
    return 0;
  }

  case WM_MOUSEWHEEL: {
    auto d = GET_WHEEL_DELTA_WPARAM(wParam) / (float)WHEEL_DELTA;
    _state.wheel = d;
    for (auto &callback : _on_wheel) {
      callback(static_cast<int>(d));
    }
    return 0;
  }

  //
  // key events
  //
  case WM_CHAR: {
    for (auto &callback : _on_key) {
      callback(static_cast<int>(wParam));
    }
    return 0;
  }
  }

  return DefWindowProcA(hWnd, message, wParam, lParam);
}

HWND Window::create(HINSTANCE instance, const char *class_name,
                    const char *window_title, int width, int height) {

  WNDCLASSEXA windowClass = {0};
  windowClass.cbSize = (UINT)sizeof(WNDCLASSEXW);
  windowClass.style = CS_HREDRAW | CS_VREDRAW;
  windowClass.lpfnWndProc = WndProc;
  windowClass.hInstance = instance;
  windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
  windowClass.lpszClassName = class_name;
  if (!RegisterClassExA(&windowClass)) {
    return nullptr;
  }

  if (width && height) {
    RECT windowRect = {0, 0, width, height};
    AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, FALSE);
    width = windowRect.right - windowRect.left;
    height = windowRect.bottom - windowRect.top;
  } else {
    width = CW_USEDEFAULT;
    height = CW_USEDEFAULT;
  }

  _hwnd = CreateWindowA(class_name, window_title, WS_OVERLAPPEDWINDOW,
                        CW_USEDEFAULT, CW_USEDEFAULT, width, height,
                        nullptr, // We have no parent window.
                        nullptr, // We aren't using menus.
                        instance, this);
  return _hwnd;
}

bool Window::process_messages(ScreenState *pstate) {

  _state.time = std::chrono::system_clock::now();

  MSG msg = {};
  while (true) {
    if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
      if (msg.message == WM_QUIT) {
        return false;
      }
      TranslateMessage(&msg);
      DispatchMessage(&msg);
    } else {
      break;
    }
  }

  if (pstate) {
    *pstate = _state;
    _state.wheel = 0;
  }

  return true;
}

void Window::load_ini() {
  nlohmann::json json;
  std::ifstream is(INI_FILE, std::ios::binary);
  if (!is) {
    return;
  }
  is >> json;
  WindowIni ini;
  ns::from_json(json, ini);

  if (ini.maximized) {
    ShowWindow(_hwnd, SW_MAXIMIZE);
  } else {
    SetClientSize(_hwnd, ini.w, ini.h);
  }
}

void Window::save_ini() {
  WindowIni ini{
      0,
      0,
      static_cast<int>(_state.width),
      static_cast<int>(_state.height),
      _is_maximized,
  };
  nlohmann::json json;
  ns::to_json(json, ini);

  std::ofstream os(INI_FILE, std::ios::binary);
  os << json;
}

} // namespace gorilla
