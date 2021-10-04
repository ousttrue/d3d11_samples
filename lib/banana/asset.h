#pragma once
#include <assert.h>
#include <d3dcommon.h>
#include <memory>
#include <span>
#include <stdint.h>
#include <string_view>
#include <vector>

namespace banana {

class Asset : public ID3DInclude {
  std::string _key;
  std::vector<uint8_t> _bytes;

  Asset(std::string_view key, std::string_view source) : _key(key) {
    _bytes.assign(source.begin(), source.end());
  }

public:
  Asset(std::string_view key, std::istream &is);

  static std::shared_ptr<Asset> from_string(std::string_view source) {
    std::shared_ptr<Asset> asset(new Asset("", source));
    return asset;
  }

  std::string_view key() const { return _key; }

  HRESULT Open(D3D_INCLUDE_TYPE IncludeType, LPCSTR pFileName,
               LPCVOID pParentData, LPCVOID *ppData, UINT *pBytes);
  HRESULT Close(LPCVOID pData);

  std::string_view string_view() const {
    return std::string_view((char *)_bytes.data(), _bytes.size());
  }

  template <typename T> std::span<T> span() const {
    assert(_bytes.size() % sizeof(T) == 0);
    return std::span<T>((T *)_bytes.data(), _bytes.size() / sizeof(T));
  }

  std::span<const uint8_t> bytes() const { return span<uint8_t>(); }
};

std::shared_ptr<Asset> get_asset(std::string_view key);
// std::string_view get_string(std::string_view key);
// std::span<const uint8_t> get_bytes(std::string_view key);

} // namespace banana
