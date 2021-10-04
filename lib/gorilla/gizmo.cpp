#include "gizmo.h"
#include <banana/asset.h>
#include <iostream>
#include <memory>

template <typename T> using ComPtr = Microsoft::WRL::ComPtr<T>;

namespace gorilla::gizmo {

std::shared_ptr<Drawable> create_grid(const ComPtr<ID3D11Device> &device) {

  auto shader = banana::get_asset("grid.hlsl");
  if (!shader) {
    return {};
  }

  auto drawable = std::make_shared<Drawable>();
  if (!drawable->state.create(device, true)) {
    return {};
  }
  auto [ok, error] = drawable->pipeline.compile_shader(device, shader, "vsMain",
                                                       "gsMain", "psMain");
  if (!ok) {
    std::cerr << error << std::endl;
    return {};
  }

  return drawable;
}

} // namespace gorilla::gizmo