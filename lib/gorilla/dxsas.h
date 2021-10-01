#pragma once
#include <string>
#include <string_view>
#include <unordered_map>
#include <banana/semantics.h>

namespace gorilla {

struct AnnotationSemantics {
  int line;
  std::string type;
  std::string name;
  banana::Semantics semantic;
  std::string annotation;
};

struct DXSAS {
  std::vector<AnnotationSemantics> semantics;
  void parse(std::string_view source);
  const AnnotationSemantics *find(banana::Semantics semantic,
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
