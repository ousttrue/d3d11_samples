#include <assert.h>
#include <d3dcompiler.h>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <string_view>
#include <tuple>
#include <wrl/client.h>

namespace swtk {
template <typename T> using ComPtr = Microsoft::WRL::ComPtr<T>;

std::tuple<ComPtr<ID3DBlob>, ComPtr<ID3DBlob>>
compile_shader(const char *name, std::string_view source,
               const char *entry_point, const char *target,
               const D3D_SHADER_MACRO *define) {
  UINT flags = D3DCOMPILE_ENABLE_STRICTNESS;
  ComPtr<ID3DBlob> ret;
  ComPtr<ID3DBlob> err;
  auto hr = D3DCompile(source.data(), source.size(), name, define, nullptr,
                       entry_point, target, flags, 0, &ret, &err);
  if (FAILED(hr)) {
    return {nullptr, err};
  }
  return {ret, err};
}

std::string read_file(std::string_view src) {
  std::filesystem::path path = src;
  std::ifstream in((path), std::ios_base::binary);
  if (!in) {
    return {};
  }

  std::stringstream buffer;
  buffer << in.rdbuf();
  return buffer.str();
}

} // namespace swtk
