#pragma once
#include <banana/orbit_camera.h>
#include <gorilla/window.h>

inline void update_camera(banana::OrbitCamera *camera,
                          const gorilla::ScreenState &state) {
  camera->update(state.mouse_x, state.mouse_y, state.width, state.height,
                 state.mouse_button_flag & gorilla::MouseButtonLeftDown,
                 state.mouse_button_flag & gorilla::MouseButtonRightDown,
                 state.mouse_button_flag & gorilla::MouseButtonMiddleDown,
                 state.wheel);
}
