#pragma once

#include "constant_buffer.h"
#include <d3d11.h>
#include <string_view>
#include <wrl/client.h>
#include <vector>

namespace gorilla {

class Pipeline {

  template <typename T> using ComPtr = Microsoft::WRL::ComPtr<T>;

  // vs
private:
  ComPtr<ID3D11VertexShader> _vs;
  ComPtr<ID3D11InputLayout> _input_layout;
public:
  std::vector<ConstantBuffer> vs_cb;

  // gs
private:
  ComPtr<ID3D11GeometryShader> _gs;
public:
  std::vector<ConstantBuffer> gs_cb;

  // ps
private:
  ComPtr<ID3D11PixelShader> _ps;
public:
  std::vector<ConstantBuffer> ps_cb;

private:
  void create_cb(std::vector<ConstantBuffer> &buffers, const ComPtr<ID3D11Device> &device, const ComPtr<ID3DBlob> &compiled);

  std::vector<ID3D11Buffer*> _tmp_list;

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