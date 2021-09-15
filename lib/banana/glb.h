#pragma once
#include "bytereader.h"
#include <string_view>

namespace banana::gltf {

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

} // namespace banana::gltf