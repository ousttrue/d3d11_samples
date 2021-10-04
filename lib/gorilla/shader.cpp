#include "shader.h"
#include <assert.h>
#include <d3dcompiler.h>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <string_view>
#include <tuple>
#include <wrl/client.h>

namespace gorilla {
template <typename T> using ComPtr = Microsoft::WRL::ComPtr<T>;

class AssetInclude : public ID3DInclude {

  std::shared_ptr<banana::Asset> _asset;

public:
  AssetInclude(const std::shared_ptr<banana::Asset> &asset) : _asset(asset) {}

  HRESULT Open(D3D_INCLUDE_TYPE IncludeType, LPCSTR pFileName,
               LPCVOID pParentData, LPCVOID *ppData, UINT *pBytes) {
    auto nest = _asset->get(pFileName);
    auto bytes = nest->bytes();
    *ppData = bytes.data();
    *pBytes = static_cast<UINT>(bytes.size());
    return S_OK;
  }

  HRESULT Close(LPCVOID pData) { return S_OK; }
};

std::tuple<Microsoft::WRL::ComPtr<ID3DBlob>, Microsoft::WRL::ComPtr<ID3DBlob>>
compile_shader(const char *name, const std::shared_ptr<::banana::Asset> &asset,
               const char *entry_point, const char *target,
               const D3D_SHADER_MACRO *define) {
  UINT flags = D3DCOMPILE_ENABLE_STRICTNESS;
  ComPtr<ID3DBlob> ret;
  ComPtr<ID3DBlob> err;
  auto source = asset->string_view();
  AssetInclude include(asset);
  auto hr = D3DCompile(source.data(), source.size(), name, define, &include,
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

std::string read_file(std::string_view dir, std::string_view src) {
  std::stringstream ss;
  ss << dir << "/" << src;
  return read_file(ss.str());
}

} // namespace gorilla
