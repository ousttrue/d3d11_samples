#pragma once
#include <banana/asset.h>
#include <banana/semantics.h>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace gorilla {

struct AnnotationSemantics {
  int line;
  std::string type;
  std::string name;
  banana::Semantics semantic;
  std::string annotation;
};

namespace hlsl {

enum class TokenTypes {
  End,
  Symbol,
  Colon,
  Semicolon,

  // [
  OpenBracket,
  // ]
  CloseBracket,
  // (
  OpenParenthesis,
  // )
  CloseParenthesis,
  // {
  OpenBrace,
  // }
  CloseBrace,
  // <
  LessThan,
  // >
  GreaterTham,
  //
  BinOperator,
  Integer,
  Float,
  LineComment,
  Directive,
};

struct Token {
  TokenTypes type;
  std::string_view view;

  std::string string() const { return {view.begin(), view.end()}; }

  bool operator==(const Token &rhs) const {
    if (type != rhs.type) {
      return false;
    }
    if (view != rhs.view) {
      return false;
    }
    return true;
  }
};

class Lexer {
  class LexerImpl *_impl = nullptr;

public:
  Lexer(const std::shared_ptr<banana::Asset> &asset);
  ~Lexer();
  bool try_get(Token *t);
  Token next() {
    Token t;
    try_get(&t);
    return t;
  }
  int line() const;

  std::vector<Token> list() {
    std::vector<Token> l;
    while (true) {
      Token t;
      if (!try_get(&t)) {
        break;
      }
      l.push_back(t);

      // Statement s;
      // if (is_prefix(t)) {
      //   s.prefix = t;
      // }
    }
    return l;
  }

  void skip_register();
  void skip(int init_level, TokenTypes open, TokenTypes close);
  void skip_block(int init_level = 1) {
    skip(init_level, TokenTypes::OpenBrace, TokenTypes::CloseBrace);
  }

  void skip_params(int init_level = 1) {
    skip(init_level, TokenTypes::OpenParenthesis, TokenTypes::CloseParenthesis);
  }
};

enum class StatementTypes {
  Empty, // ;
  Field, // row_major float4x4 MVP: WORLDVIEWPROJECTION;
  Struct,
};

struct Statement {
  StatementTypes type;
  Token prefix;
  Token value_type;
  Token name;
  Token semantic;
  std::vector<Statement> fields;
  int line = -1;

  bool operator==(const Statement &rhs) const {
    if (type != rhs.type) {
      return false;
    }
    return true;
  }
};

class AST {
  class ASTImpl *_impl = nullptr;

public:
  std::vector<Statement> statements;
  AST();
  ~AST();
  bool parse(const std::shared_ptr<banana::Asset> &asset);
};

} // namespace hlsl

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
