#pragma once
#include <span>
#include <vector>

namespace gorilla::assets {
class Image {

  uint32_t _width = 0;
  uint32_t _height = 0;
  std::vector<uint8_t> _buffer;

public:
  uint32_t height() const { return _width; }
  uint32_t width() const { return _height; }
  const void *data() const { return _buffer.data(); }

  bool load(std::span<const uint8_t> bytes);
};
} // namespace gorilla::assets
