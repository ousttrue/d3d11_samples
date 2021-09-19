#pragma once

#include "constant_buffer.h"
#include <d3d11.h>
#include <gorilla/shader_reflection.h>
#include <string_view>
#include <vector>
#include <wrl/client.h>

namespace gorilla {

struct ShaderStage {
  ShaderReflection reflection;
  std::vector<ConstantBuffer> cb;
};

class Pipeline {

  template <typename T> using ComPtr = Microsoft::WRL::ComPtr<T>;

private:
  ComPtr<ID3D11VertexShader> _vs;
  ComPtr<ID3D11InputLayout> _input_layout;
  ComPtr<ID3D11RasterizerState> _rs;

public:
  ShaderStage vs_stage;

  // gs
private:
  ComPtr<ID3D11GeometryShader> _gs;

public:
  ShaderStage gs_stage;

  // ps
private:
  ComPtr<ID3D11PixelShader> _ps;

public:
  ShaderStage ps_stage;

private:
  void create_cb(ShaderStage &stage, const ComPtr<ID3D11Device> &device,
                 const ComPtr<ID3DBlob> &compiled);

  std::vector<ID3D11Buffer *> _tmp_list;

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

  std::pair<bool, std::string>
  compile_shader(const ComPtr<ID3D11Device> &device, std::string_view source,
                 const char *vs_entry, const char *gs_entry,
                 const char *ps_entry);

  void set_variable(std::string_view name, const void *p, size_t size);
  void update(const ComPtr<ID3D11DeviceContext> &context);
  void set_srv(const ComPtr<ID3D11DeviceContext> &context,
               std::string_view name,
               const ComPtr<ID3D11ShaderResourceView> &srv);
  void set_sampler(const ComPtr<ID3D11DeviceContext> &context,
                   std::string_view name,
                   const ComPtr<ID3D11SamplerState> &sampler);
  void setup(const ComPtr<ID3D11DeviceContext> &context);
  void draw_empty(const ComPtr<ID3D11DeviceContext> &context);
};

} // namespace gorilla