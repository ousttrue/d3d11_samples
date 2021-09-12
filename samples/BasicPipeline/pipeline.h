#pragma once

#include <d3d11.h>
#include <string_view>
#include <wrl/client.h>

namespace gorilla {

class Pipeline {

  template <typename T> using ComPtr = Microsoft::WRL::ComPtr<T>;

  ComPtr<ID3D11VertexShader> _vs;
  ComPtr<ID3D11GeometryShader> _gs;
  ComPtr<ID3D11PixelShader> _ps;

public:
  std::tuple<ComPtr<ID3DBlob>, ComPtr<ID3DBlob>>
  compile_vs(const ComPtr<ID3D11Device> &device, const char *name,
             std::string_view source, const char *entry_point);
  std::tuple<ComPtr<ID3DBlob>, ComPtr<ID3DBlob>>
  compile_gs(const ComPtr<ID3D11Device> &device, const char *name,
             std::string_view source, const char *entry_point);
  std::tuple<ComPtr<ID3DBlob>, ComPtr<ID3DBlob>>
  compile_ps(const ComPtr<ID3D11Device> &device, const char *name,
             std::string_view source, const char *entry_point);

  void setup(const ComPtr<ID3D11DeviceContext> &context);
  void draw_empty(const ComPtr<ID3D11DeviceContext> &context);
};

} // namespace gorilla