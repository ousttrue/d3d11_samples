#pragma once

#include <string>
#include <windows.h>

namespace gorilla {

class Window {
  HINSTANCE _instance = nullptr;
  std::string _class_name;
  HWND _hwnd = nullptr;

public:
  ~Window();
  LRESULT proc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
  HWND create(HINSTANCE instance, const char *class_name,
              const char *window_title, int width = 0,
              int height = 0);
  bool process_messages();
};

} // namespace gorilla
