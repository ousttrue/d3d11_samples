#pragma once
#include <span>
#include <stdint.h>

namespace banana {
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

} // namespace banana