#pragma once
#include <gorilla/input_assembler.h>
#include <gorilla/pipeline.h>

namespace gorilla {

struct Drawable {
  template <typename T> using ComPtr = Microsoft::WRL::ComPtr<T>;

  gorilla::Pipeline pipeline;
  gorilla::InputAssembler ia;

  void draw(const ComPtr<ID3D11DeviceContext> &context) {
    pipeline.setup(context);
    ia.draw(context);
  }
};

} // namespace gorilla
