#pragma once
#include <optional>
#include <span>
#include <stdint.h>
#include <string_view>
#include <vector>

namespace gorilla::gltf {

class ByteReader {
  const uint8_t *_p;
  size_t _pos = 0;
  size_t _size;

public:
  ByteReader(const void *p, size_t size)
      : _p((const uint8_t *)p), _size(size) {}

  size_t pos() const { return _pos; }

  template <typename T> bool try_read(T *t) {
    if (_pos + sizeof(T) > _size) {
      return false;
    }
    *t = *((T *)(_p + _pos));
    _pos += sizeof(T);
    return true;
  }

  std::span<const uint8_t> read(size_t size) {
    if (_pos + size > _size) {
      return {};
    }
    std::span<const uint8_t> value(_p + _pos, size);
    _pos += size;
    return value;
  }
};

// https://github.com/KhronosGroup/glTF/tree/master/specification/2.0#glb-file-format-specification
struct Glb {

  std::string_view json;
  std::span<const uint8_t> bin;

  bool parse(std::span<const uint8_t> bytes) {
    ByteReader r(bytes.data(), bytes.size());

    uint32_t magic;
    if (!r.try_read(&magic)) {
      return false;
    }
    if (magic != 0x46546C67) {
      return false;
    }
    uint32_t version;
    if (!r.try_read(&version)) {
      return false;
    }
    if (version != 2) {
      return false;
    }
    uint32_t length;
    if (!r.try_read(&length)) {
      return false;
    }

    while (r.pos() < length) {
      uint32_t chunkLength;
      if (!r.try_read(&chunkLength)) {
        return false;
      }
      uint32_t chunkType;
      if (!r.try_read(&chunkType)) {
        return false;
      }

      auto data = r.read(chunkLength);
      if (data.size() != chunkLength) {
        return false;
      }

      switch (chunkType) {
      case 0x4E4F534A:
        json = std::string_view((const char *)data.data(), data.size());
        break;

      case 0x004E4942:
        bin = data;
        break;

      default:
        // unknown
        break;
      }
    }

    return true;
  }
};

struct float2 {
  float x;
  float y;
};

struct float3 {
  float x;
  float y;
  float z;
};

struct float4 {
  float x;
  float y;
  float z;
  float w;
};

enum class ImageType {
  Unknown,
  Png,
  Jpg,
};

struct Texture {
  ImageType type = ImageType::Unknown;
  std::span<const uint8_t> bytes;
};

struct Material {
  std::optional<uint32_t> base_color_texture_index;
};

struct SubMesh {
  uint32_t offset = 0;
  uint32_t draw_count = 0;
  std::optional<uint32_t> material_index;
};

struct Mesh {
  struct Vertex {
    float3 position;
    float2 tex0;
  };
  using Index = uint32_t;

  std::vector<Vertex> vertices;
  std::vector<Index> indices;
  std::vector<SubMesh> submeshes;
};

struct GltfLoader {
  std::string json;
  std::vector<uint8_t> bin;
  std::vector<Texture> textures;
  std::vector<Material> materials;
  std::vector<Mesh> meshes;

  GltfLoader(std::string_view json, std::span<const uint8_t> bin)
  : json(json), bin(bin.begin(), bin.end())
  {
  }

  bool load();
};

} // namespace gorilla::gltf
