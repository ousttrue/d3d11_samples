#pragma once
#include <span>
#include <stdint.h>
#include <string_view>

namespace banana::asset {

std::string_view get_string(std::string_view key);
std::span<uint8_t> get_bytes(std::string_view key);

} // namespace banana::asset
