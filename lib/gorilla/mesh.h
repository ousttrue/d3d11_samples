#pragma once

#include "input_assembler.h"
#include "pipeline.h"
#include "texture.h"
#include <DirectXMath.h>
#include <array>
#include <d3d11.h>
#include <memory>
#include <wrl/client.h>

namespace gorilla {

struct Material {
  template <typename T> using ComPtr = Microsoft::WRL::ComPtr<T>;
  Pipeline pipeline;

  ComPtr<ID3D11RasterizerState> rs;

  std::array<float, 4> base_color = {1, 1, 1, 1};
  std::shared_ptr<gorilla::Texture> base_color_texture;

  std::shared_ptr<gorilla::Texture> normal_map;
  float normal_map_scale = 1.0f;
};

struct SubMesh {
  UINT offset = 0;
  UINT draw_count = 0;
  std::shared_ptr<Material> material;
};

struct Mesh {
  template <typename T> using ComPtr = Microsoft::WRL::ComPtr<T>;
  InputAssembler ia;
  std::vector<SubMesh> submeshes;
};

} // namespace gorilla