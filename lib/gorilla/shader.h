#include <banana/asset.h>
#include <d3dcompiler.h>
#include <string_view>
#include <tuple>
#include <wrl/client.h>

namespace gorilla {

std::tuple<Microsoft::WRL::ComPtr<ID3DBlob>, Microsoft::WRL::ComPtr<ID3DBlob>>
compile_shader(const char *name, const std::shared_ptr<::banana::Asset> &asset,
               const char *entry_point, const char *target,
               const D3D_SHADER_MACRO *define);

inline std::tuple<Microsoft::WRL::ComPtr<ID3DBlob>,
                  Microsoft::WRL::ComPtr<ID3DBlob>>
compile_vs(const char *name, const std::shared_ptr<banana::Asset> &asset,
           const char *entry_point, const D3D_SHADER_MACRO *define = nullptr) {
  return compile_shader(name, asset, entry_point, "vs_4_0", define);
}
inline std::tuple<Microsoft::WRL::ComPtr<ID3DBlob>,
                  Microsoft::WRL::ComPtr<ID3DBlob>>
compile_vs(const char *name, const std::string_view source,
           const char *entry_point, const D3D_SHADER_MACRO *define = nullptr) {
  return compile_shader(name, banana::Asset::from_string(source), entry_point,
                        "vs_4_0", define);
}

inline std::tuple<Microsoft::WRL::ComPtr<ID3DBlob>,
                  Microsoft::WRL::ComPtr<ID3DBlob>>
compile_ps(const char *name, const std::shared_ptr<banana::Asset> &asset,
           const char *entry_point, const D3D_SHADER_MACRO *define = nullptr) {
  return compile_shader(name, asset, entry_point, "ps_4_0", define);
}
inline std::tuple<Microsoft::WRL::ComPtr<ID3DBlob>,
                  Microsoft::WRL::ComPtr<ID3DBlob>>
compile_ps(const char *name, const std::string_view source,
           const char *entry_point, const D3D_SHADER_MACRO *define = nullptr) {
  return compile_shader(name, banana::Asset::from_string(source), entry_point,
                        "ps_4_0", define);
}

inline std::tuple<Microsoft::WRL::ComPtr<ID3DBlob>,
                  Microsoft::WRL::ComPtr<ID3DBlob>>
compile_gs(const char *name, const std::shared_ptr<banana::Asset> &asset,
           const char *entry_point, const D3D_SHADER_MACRO *define = nullptr) {
  return compile_shader(name, asset, entry_point, "gs_4_0", define);
}

std::string read_file(std::string_view path);

std::string read_file(std::string_view dir, std::string_view path);

} // namespace gorilla
