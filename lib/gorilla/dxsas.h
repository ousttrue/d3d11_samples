#pragma once
#include <string>
#include <string_view>
#include <unordered_map>

namespace gorilla {
struct AnnotationSemantics {
  int line;
  std::string type;
  std::string name;
  std::string semantic;
  std::string annotation;
};
struct DXSAS {
  std::unordered_map<std::string, AnnotationSemantics> semantics_map;
  void parse(std::string_view source);
};

} // namespace gorilla
