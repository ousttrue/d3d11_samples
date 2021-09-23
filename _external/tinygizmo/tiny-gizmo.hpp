// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org>

#pragma once
#include <array>
#include <functional>
#include <memory>
#include <span>

namespace tinygizmo {

struct camera_parameters {
  float yfov = 1.0f;
  float near_clip = 0.01f;
  float far_clip = 100.0f;
  std::array<float, 3> position = {0, 0, 0};
  std::array<float, 4> orientation = {0, 0, 0, 1};
};

struct gizmo_application_state {
  bool mouse_left{false};
  bool hotkey_local{false};
  bool hotkey_ctrl{false};
  // If > 0.f, the gizmos are drawn scale-invariant with a screenspace value
  // defined here
  float screenspace_scale{0.f};
  // World-scale units used for snapping translation
  float snap_translation{0.f};
  // World-scale units used for snapping scale
  float snap_scale{0.f};
  // Radians used for snapping rotation quaternions (i.e. PI/8 or PI/16)
  float snap_rotation{0.f};

  // 3d viewport used to render the view
  std::array<float, 2> viewport_size;
  // world-space ray origin (i.e. the camera position)
  std::array<float, 3> ray_origin;
  // world-space ray direction
  std::array<float, 3> ray_direction;
  // Used for constructing inverse view projection for raycasting onto gizmo
  // geometry
  camera_parameters cam;
};

struct Vertex {
  std::array<float, 3> position;
  std::array<float, 3> normal;
  std::array<float, 4> color;
};

struct gizmo_context {
  struct gizmo_context_impl;
  std::unique_ptr<gizmo_context_impl> impl;
  gizmo_context();
  ~gizmo_context();
  // Clear geometry buffer and update internal `gizmo_application_state` data
  void update(const gizmo_application_state &state);
  // Trigger a render callback per call to `update(...)`
  std::tuple<std::span<const Vertex>, std::span<const uint32_t>> draw();
};

void position(const std::string &name, gizmo_context &g,
              const float orientation[4], float position[3]);

void orientation(const std::string &name, gizmo_context &g,
                 const std::array<float, 3> &center,
                 std::array<float, 4> &orientation);

void scale(const std::string &name, gizmo_context &g,
           const std::array<float, 4> &orientation,
           const std::array<float, 3> &center, std::array<float, 3> &scale);

} // namespace tinygizmo
