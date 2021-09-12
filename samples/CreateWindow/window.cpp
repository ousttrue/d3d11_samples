#include "window.h"

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
    // if (auto pD3D = (DXGIManager *)GetWindowLongPtr(hWnd, GWL_USERDATA)) {
    //   pD3D->Resize(LOWORD(lParam), HIWORD(lParam));
    // }
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
    if (_on_move) {
      auto pos = MAKEPOINTS(lParam);
      _on_move(pos.x, pos.y);
    }
    return 0;
  }
  case WM_LBUTTONDOWN:
    if (_on_left) {
      _button |= ButtonFlagsLeft;
      SetCapture(hWnd);
      _on_left(true);
    }
    return 0;

  case WM_RBUTTONDOWN:
    if (_on_right) {
      _button |= ButtonFlagsRight;
      SetCapture(hWnd);
      _on_right(true);
    }
    return 0;

  case WM_MBUTTONDOWN:
    if (_on_middle) {
      _button |= ButtonFlagsMiddle;
      SetCapture(hWnd);
      _on_middle(true);
    }
    return 0;

  case WM_LBUTTONUP:
    if (_on_left) {
      _button &= ~ButtonFlagsLeft;
      if (_button == ButtonFlagsNone) {
        ReleaseCapture();
      }
      _on_left(false);
    }
    return 0;

  case WM_RBUTTONUP:
    if (_on_right) {
      _button &= ~ButtonFlagsRight;
      if (_button == ButtonFlagsNone) {
        ReleaseCapture();
      }
      _on_right(false);
    }
    return 0;

  case WM_MBUTTONUP:
    if (_on_middle) {
      _button &= ~ButtonFlagsMiddle;
      if (_button == ButtonFlagsNone) {
        ReleaseCapture();
      }
      _on_middle(false);
    }
    return 0;

  case WM_MOUSEWHEEL:
    if (_on_wheel) {
      auto d = GET_WHEEL_DELTA_WPARAM(wParam);
      _on_wheel(d);
    }
    return 0;
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

bool Window::process_messages() {
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

  return true;
}

} // namespace gorilla
