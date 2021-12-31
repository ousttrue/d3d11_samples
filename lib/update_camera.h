#pragma once
#include <banana/orbit_camera.h>
#include <gorilla/window.h>

inline void update_camera(banana::OrbitCamera *camera,
                          const gorilla::ScreenState &state) {
  static unsigned int frame_count = 0;
  static float last_x = 0;
  static float last_y = 0;
  camera->update(
      frame_count ? state.mouse_x - last_x : 0,
      (frame_count++) ? state.mouse_y - last_y : 0, state.width, state.height,
      state.mouse_button_flag & gorilla::MouseButtonLeftDown,
      state.mouse_button_flag & gorilla::MouseButtonRightDown,
      state.mouse_button_flag & gorilla::MouseButtonMiddleDown, state.wheel);
  last_x = state.mouse_x;
  last_y = state.mouse_y;
}
