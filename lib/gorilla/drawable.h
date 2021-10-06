#pragma once
#include "input_assembler.h"
#include "pipeline.h"
#include "window.h"
#include <banana/types.h>

namespace gorilla {

struct State {
  template <typename T> using ComPtr = Microsoft::WRL::ComPtr<T>;

  D3D11_RASTERIZER_DESC rs_desc = {};
  ComPtr<ID3D11RasterizerState> rs;

  D3D11_BLEND_DESC bs_desc = {};
  ComPtr<ID3D11BlendState> bs;
  float blend_factor[4] = {0.0f, 0.0f, 0.0f, 0.0f};

  State();
  bool create(const ComPtr<ID3D11Device> &device,
              bool create_blend_state = false);
  void setup(const ComPtr<ID3D11DeviceContext> &context);
};

struct Drawable {
  template <typename T> using ComPtr = Microsoft::WRL::ComPtr<T>;

  gorilla::Pipeline pipeline;
  gorilla::InputAssembler ia;

  State state;

  void draw(const ComPtr<ID3D11DeviceContext> &context);

  void
  draw(const ComPtr<ID3D11DeviceContext> &context,
       const banana::OrbitCamera &camera,
       const std::unordered_map<banana::Semantics, banana::Variable> &map = {});
};

} // namespace gorilla
