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
  std::vector<AnnotationSemantics> semantics;
  void parse(std::string_view source);
  const AnnotationSemantics *find(std::string_view semantic,
                                  std::string_view type = {}) const {
    for (auto &s : semantics) {
      if (s.semantic == semantic) {
        if (type.empty() || s.type == type) {
          return &s;
        }
      }
    }
    return {};
  }
};

} // namespace gorilla
