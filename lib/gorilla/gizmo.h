#pragma once
#include "drawable.h"
#include <memory.h>

namespace gorilla {
namespace gizmo {

std::shared_ptr<Drawable>
create_aabb(const Microsoft::WRL::ComPtr<ID3D11Device> &device);

std::shared_ptr<Drawable>
create_grid(const Microsoft::WRL::ComPtr<ID3D11Device> &device);

} // namespace gizmo
} // namespace gorilla
