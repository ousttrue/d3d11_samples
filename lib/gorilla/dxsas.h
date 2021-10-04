#pragma once
#include <banana/asset.h>
#include <banana/semantics.h>
#include <string>
#include <string_view>
#include <unordered_map>

namespace gorilla {

namespace hlsl {

enum class TokenTypes {
  End,
};

struct Token {
  TokenTypes type;

  bool operator==(const Token &rhs) const { return type == rhs.type; }
};

class Lexer {
  std::shared_ptr<banana::Asset> _asset;

public:
  Lexer(const std::shared_ptr<banana::Asset> &asset) : _asset(asset) {}

  Token next() { return {}; };

  std::vector<Token> list() {
    std::vector<Token> l;
    while (true) {
      auto &t = l.emplace_back(next());
      if (t.type == TokenTypes::End) {
        break;
      }
    }
    return l;
  }
};

} // namespace hlsl

struct AnnotationSemantics {
  int line;
  std::string type;
  std::string name;
  banana::Semantics semantic;
  std::string annotation;
};

struct DXSAS {
  std::vector<AnnotationSemantics> semantics;
  void parse(const std::shared_ptr<banana::Asset> &asset);
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
