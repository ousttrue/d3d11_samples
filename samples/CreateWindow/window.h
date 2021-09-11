#pragma once

#include <string>
#include <windows.h>

namespace swtk {

class Window {
  HINSTANCE _instance = nullptr;
  std::string _class_name;
  HWND _hwnd = nullptr;

public:
  ~Window();
  LRESULT proc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
  HWND create(HINSTANCE instance, const char *class_name,
              const char *window_title);
  bool process_messages();
};

} // namespace swtk
