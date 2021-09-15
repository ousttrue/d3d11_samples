#include "asset.h"
#include <assert.h>
#include <filesystem>
#include <fstream>
#include <unordered_map>

struct Asset {
  std::string key;
  std::vector<uint8_t> bytes;

  std::string_view string_view() const {
    return std::string_view((char *)bytes.data(), bytes.size());
  }

  template <typename T> std::span<T> span() const {
    assert(bytes.size() % sizeof(T) == 0);
    return std::span<T>((T *)bytes.data(), bytes.size() / sizeof(T));
  }
};

class Database {
  const std::filesystem::path _root;

  std::list<Asset> _assets;
  std::unordered_map<std::string_view, Asset *> _map;

public:
  ~Database() {}
  Database(const std::filesystem::path &root) : _root(root) {}
  Database(const Database &) = delete;
  Database &operator=(const Database &) = delete;

  std::filesystem::path get_full(std::string_view key) const {
    auto item = _root;
    return item.append(key);
  }

  bool try_get(std::string_view key, Asset **asset) {
    if (key.empty()) {
      return false;
    }

    auto it = _map.find(key);
    if (it != _map.end()) {
      *asset = it->second;
      return true;
    }

    std::ifstream is(get_full(key), std::ios::binary);
    if (!is) {
      return false;
    }

    auto new_asset =
        &_assets.emplace_back(Asset{std::string(key.begin(), key.end())});
    _map.emplace(std::make_pair(new_asset->key, new_asset));

    is.seekg(0, is.end);
    new_asset->bytes.resize(is.tellg());
    is.seekg(0, is.beg);
    is.read((char *)new_asset->bytes.data(), new_asset->bytes.size());

    *asset = new_asset;
    return true;
  }
};
std::unique_ptr<Database> g_assets;

Database &get_or_default() {
  if (!g_assets) {
    g_assets.reset(
        new Database(std::filesystem::current_path().append("assets")));
  }
  return *g_assets;
}

namespace banana::asset {

std::string_view get_string(std::string_view key) {

  auto &db = get_or_default();
  Asset *asset;
  if (!db.try_get(key, &asset)) {
    return {};
  }
  return asset->string_view();
}

std::span<uint8_t> get_bytes(std::string_view key) {
  auto &db = get_or_default();
  Asset *asset;
  if (!db.try_get(key, &asset)) {
    return {};
  }
  return asset->span<uint8_t>();
}

} // namespace banana::asset
