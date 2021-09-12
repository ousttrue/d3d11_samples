#pragma once

#include <functional>
#include <list>
#include <string>
#include <windows.h>

namespace gorilla {

using OnMouseButton_t = std::function<void(bool)>;
using OnMouseMove_t = std::function<void(int, int)>;
using OnMouseWheel_t = std::function<void(int)>;

class Window {
  HINSTANCE _instance = nullptr;
  std::string _class_name;
  HWND _hwnd = nullptr;
  OnMouseButton_t _on_left;
  OnMouseButton_t _on_right;
  OnMouseButton_t _on_middle;
  OnMouseMove_t _on_move;
  OnMouseWheel_t _on_wheel;

  enum ButtonFlags : int {
    ButtonFlagsNone = 0,
    ButtonFlagsLeft = 1,
    ButtonFlagsRight = 1 << 1,
    ButtonFlagsMiddle = 1 << 2,
  };
  int _button = ButtonFlagsNone;

public:
  ~Window();
  LRESULT proc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
  HWND create(HINSTANCE instance, const char *class_name,
              const char *window_title, int width = 0, int height = 0);
  bool process_messages();

  void bind_mouse(const OnMouseButton_t &left, const OnMouseButton_t &middle,
                  const OnMouseButton_t &right, const OnMouseMove_t &move,
                  const OnMouseWheel_t &wheel) {
    _on_left = left;
    _on_middle = middle;
    _on_right = right;
    _on_move = move;
    _on_wheel = wheel;
  }
};

} // namespace gorilla
