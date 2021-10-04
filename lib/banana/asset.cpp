#include "asset.h"
#include <assert.h>
#include <filesystem>
#include <fstream>
#include <unordered_map>

class Database {
  const std::filesystem::path _root;

  std::unordered_map<std::string_view, std::shared_ptr<banana::Asset>> _map;

public:
  ~Database() {}
  Database(const std::filesystem::path &root) : _root(root) {}
  Database(const Database &) = delete;
  Database &operator=(const Database &) = delete;

  std::filesystem::path get_full(std::string_view key) const {
    auto item = _root;
    return item.append(key);
  }

  std::shared_ptr<banana::Asset> try_get(std::string_view key) {
    if (key.empty()) {
      return {};
    }

    auto it = _map.find(key);
    if (it != _map.end()) {
      return it->second;
    }

    std::ifstream is(get_full(key), std::ios::binary);
    if (!is) {
      return {};
    }

    auto new_asset = std::make_shared<banana::Asset>(key, is);
    _map.insert(std::make_pair(new_asset->key(), new_asset));

    return new_asset;
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

namespace banana {

Asset::Asset(std::string_view key, std::string_view source) : _key(key) {
  _bytes.assign(source.begin(), source.end());

  get = get_asset;
}

Asset::Asset(std::string_view key, std::istream &is) : _key(key) {
  is.seekg(0, is.end);
  _bytes.resize(is.tellg());
  is.seekg(0, is.beg);
  is.read((char *)_bytes.data(), _bytes.size());

  get = get_asset;
}

std::shared_ptr<Asset> get_asset(std::string_view key) {
  auto &db = get_or_default();
  return db.try_get(key);
}

std::string_view get_string(std::string_view key) {
  auto asset = get_asset(key);
  if (!asset) {
    return {};
  }
  return asset->string_view();
}

std::span<const uint8_t> get_bytes(std::string_view key) {
  auto asset = get_asset(key);
  if (!asset) {
    return {};
  }
  return asset->span<uint8_t>();
}

} // namespace banana
