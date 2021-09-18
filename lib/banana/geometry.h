#pragma once
#include "scene.h"
#include <memory>

namespace banana::geometry {
std::shared_ptr<Mesh> create_cube(float size);
}
