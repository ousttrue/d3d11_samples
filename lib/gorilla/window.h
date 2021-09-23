#pragma once

#include <chrono>
#include <functional>
#include <list>
#include <string>
#include <windows.h>

namespace gorilla {

using OnMouseButton_t = std::function<void(bool)>;
using OnMouseMove_t = std::function<void(int, int)>;
using OnMouseWheel_t = std::function<void(int)>;
using OnKeyDown_t = std::function<void(int)>;

enum MouseButtonFlags : int {
  MouseButtonNone = 0,
  MouseButtonLeftDown = 1,
  MouseButtonRightDown = 1 << 1,
  MouseButtonMiddleDown = 2 << 1,
};

struct ScreenState {
  float width;
  float height;
  float mouse_x;
  float mouse_y;
  float wheel;
  int mouse_button_flag;
  std::chrono::system_clock::time_point time = {};
};

class Window {
  HINSTANCE _instance = nullptr;
  std::string _class_name;
  HWND _hwnd = nullptr;
  std::list<OnMouseButton_t> _on_left;
  std::list<OnMouseButton_t> _on_right;
  std::list<OnMouseButton_t> _on_middle;
  std::list<OnMouseMove_t> _on_move;
  std::list<OnMouseWheel_t> _on_wheel;
  std::list<OnKeyDown_t> _on_key;

  ScreenState _state = {};

public:
  ~Window();
  LRESULT proc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
  HWND create(HINSTANCE instance, const char *class_name,
              const char *window_title, int width = 0, int height = 0);

  bool process_messages(ScreenState *pstate = {});

  void bind_mouse(const OnMouseButton_t &left, const OnMouseButton_t &middle,
                  const OnMouseButton_t &right, const OnMouseMove_t &move,
                  const OnMouseWheel_t &wheel) {
    _on_left.push_back(left);
    _on_middle.push_back(middle);
    _on_right.push_back(right);
    _on_move.push_back(move);
    _on_wheel.push_back(wheel);
  }

  void bind_key(const OnKeyDown_t &key) { _on_key.push_back(key); }
};

} // namespace gorilla
