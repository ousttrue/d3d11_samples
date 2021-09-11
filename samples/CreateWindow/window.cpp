#include "window.h"

namespace swtk {

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
  }
    return 0;
  }
  return DefWindowProcA(hWnd, message, wParam, lParam);
}

HWND Window::create(HINSTANCE instance, const char *class_name,
                    const char *window_title) {

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

  _hwnd =
      CreateWindowA(class_name, window_title, WS_OVERLAPPEDWINDOW,
                    CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
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

} // namespace swtk
