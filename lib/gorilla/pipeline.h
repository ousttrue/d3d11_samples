#pragma once

#include "constant_buffer.h"
#include "dxsas.h"
#include "shader_reflection.h"
#include <banana/asset.h>
#include <banana/orbit_camera.h>
#include <banana/semantics.h>
#include <d3d11.h>
#include <string_view>
#include <vcruntime.h>
#include <vector>
#include <wrl/client.h>

namespace gorilla {

struct VariablePosition {
  UINT slot;
  UINT offset;
  UINT size;
};

struct ShaderStage {
  ShaderReflection reflection;
  std::vector<ConstantBuffer> cb;

  std::unordered_map<banana::Semantics, VariablePosition> semantics_map;
  std::unordered_map<banana::Semantics, UINT> semantics_srv_map;
  std::unordered_map<banana::Semantics, UINT> semantics_sampler_map;

  void create_semantics_map(std::span<const AnnotationSemantics> semantics);

  void set_variable(banana::Semantics semantic, const void *p, size_t size,
                    size_t offset = 0);

  template <typename T>
  void set_variable(banana::Semantics semantic, const T &t) {
    set_variable(semantic, &t, sizeof(T));
  }
  void set_variables(const banana::OrbitCamera &camera);
};

class Pipeline {

  template <typename T> using ComPtr = Microsoft::WRL::ComPtr<T>;

  ComPtr<ID3D11InputLayout> _input_layout;
  ComPtr<ID3D11VertexShader> _vs;
  ComPtr<ID3D11GeometryShader> _gs;
  ComPtr<ID3D11PixelShader> _ps;
  std::vector<ID3D11Buffer *> _tmp_list;
  DXSAS _dxsas;

  void create_cb(ShaderStage &stage, const ComPtr<ID3D11Device> &device,
                 const ComPtr<ID3DBlob> &compiled);

public:
  ShaderStage vs_stage;
  ShaderStage gs_stage;
  ShaderStage ps_stage;
  std::tuple<ComPtr<ID3DBlob>, ComPtr<ID3DBlob>>
  compile_vs(const ComPtr<ID3D11Device> &device, const char *name,
             const std::shared_ptr<banana::Asset> &asset,
             const char *entry_point, const D3D_SHADER_MACRO *define = {});
  std::tuple<ComPtr<ID3DBlob>, ComPtr<ID3DBlob>>
  compile_gs(const ComPtr<ID3D11Device> &device, const char *name,
             const std::shared_ptr<banana::Asset> &asset,
             const char *entry_point, const D3D_SHADER_MACRO *define = {});
  std::tuple<ComPtr<ID3DBlob>, ComPtr<ID3DBlob>>
  compile_ps(const ComPtr<ID3D11Device> &device, const char *name,
             const std::shared_ptr<banana::Asset> &asset,
             const char *entry_point, const D3D_SHADER_MACRO *define = {});

  std::pair<bool, std::string>
  compile_shader(const ComPtr<ID3D11Device> &device,
                 const std::shared_ptr<banana::Asset> &asset,
                 const D3D_SHADER_MACRO *define, const char *vs_entry,
                 const char *gs_entry, const char *ps_entry);

  std::pair<bool, std::string>
  compile_shader(const ComPtr<ID3D11Device> &device,
                 const std::shared_ptr<banana::Asset> &asset,
                 const char *vs_entry, const char *gs_entry,
                 const char *ps_entry) {
    return compile_shader(device, asset, {}, vs_entry, gs_entry, ps_entry);
  }

  void set_variable(std::string_view name, const void *p, size_t size,
                    size_t offset = 0);
  void set_variable(banana::Semantics semantic, const void *p, size_t size,
                    size_t offset = 0) {
    vs_stage.set_variable(semantic, p, size, offset);
    gs_stage.set_variable(semantic, p, size, offset);
    ps_stage.set_variable(semantic, p, size, offset);
  }
  void update(const ComPtr<ID3D11DeviceContext> &context);
  void setup(const ComPtr<ID3D11DeviceContext> &context);
  void draw_empty(const ComPtr<ID3D11DeviceContext> &context);
};

} // namespace gorilla