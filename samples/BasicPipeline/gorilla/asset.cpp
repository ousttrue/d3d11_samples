#include "asset.h"
#include <filesystem>
#include <fstream>
#include <unordered_map>

struct Asset {
  std::string key;
  std::vector<uint8_t> bytes;

  std::string_view string_view() const {
    return std::string_view((char *)bytes.data(), bytes.size());
  }
};

class Database {
  std::filesystem::path _root;

  std::list<Asset> _assets;
  std::unordered_map<std::string_view, Asset *> _map;

public:
  ~Database() {}
  Database(const std::filesystem::path &root) : _root(root) {}
  Database(const Database &) = delete;
  Database &operator=(const Database &) = delete;

  bool try_get(std::string_view key, Asset **asset) {
    if (key.empty()) {
      return false;
    }

    auto it = _map.find(key);
    if (it != _map.end()) {
      *asset = it->second;
      return true;
    }

    std::ifstream is(_root.append(key), std::ios::binary);
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

namespace gorilla::assets {

std::string_view get_shader(std::string_view file) {

  auto &db = get_or_default();
  Asset *asset;
  if (!db.try_get(file, &asset)) {
    return {};
  }
  return asset->string_view();
}

} // namespace gorilla
