#include "dxsas.h"
#include <algorithm>
#include <assert.h>
#include <cctype>
#include <format>
#include <iterator>
#include <set>
#include <stdexcept>
#include <string_view>
#include <unordered_map>

static bool is_symbol(char c) {
  if (std::isalnum(c)) {
    return true;
  }
  switch (c) {
  case '_':
    return true;
  }
  return false;
}

namespace gorilla {
namespace hlsl {

class LexerImpl {
  std::shared_ptr<banana::Asset> _asset;
  std::string_view _source;
  std::string_view::iterator _it;

public:
  LexerImpl(const std::shared_ptr<banana::Asset> &asset) : _asset(asset) {
    _source = asset->string_view();
    _it = _source.begin();
  }

  Token next() {
    skip_space();
    if (is_end()) {
      return {TokenTypes::End};
    }

    switch (*_it) {
    case ':': {
      ++_it;
      return {TokenTypes::Colon};
    }

    case ';': {
      ++_it;
      return {TokenTypes::Semicolon};

    case 'a':
    case 'b':
    case 'c':
    case 'd':
    case 'e':
    case 'f':
    case 'g':
    case 'h':
    case 'i':
    case 'j':
    case 'k':
    case 'l':
    case 'm':
    case 'n':
    case 'o':
    case 'p':
    case 'q':
    case 'r':
    case 's':
    case 't':
    case 'u':
    case 'v':
    case 'w':
    case 'x':
    case 'y':
    case 'z':
    case 'A':
    case 'B':
    case 'C':
    case 'D':
    case 'E':
    case 'F':
    case 'G':
    case 'H':
    case 'I':
    case 'J':
    case 'K':
    case 'L':
    case 'M':
    case 'N':
    case 'O':
    case 'P':
    case 'Q':
    case 'R':
    case 'S':
    case 'T':
    case 'U':
    case 'V':
    case 'W':
    case 'X':
    case 'Y':
    case 'Z':
      return get_symbol();
    }

    default:
      throw std::runtime_error(std::format("unknown char: {}", *_it));
    }

    return {};
  }

private:
  bool is_end() { return _it == _source.end(); }

  void skip_space() {
    while (!is_end()) {
      switch (*_it) {
      case ' ':
      case '\t':
      case '\r':
      case '\n':
        ++_it;
        break;

      default:
        return;
      }
    }
  }

  Token get_symbol() {
    auto begin = _it;
    for (; !is_end(); ++_it) {
      if (!is_symbol(*_it)) {
        break;
      }
    }
    return Token{
        TokenTypes::Symbol,
        std::string_view(begin, _it),
    };
  }
};

Lexer::Lexer(const std::shared_ptr<banana::Asset> &asset)
    : _impl(new LexerImpl(asset)) {}
Lexer::~Lexer() { delete _impl; }

Token Lexer::next() { return _impl->next(); }

} // namespace hlsl
} // namespace gorilla

enum class TokenTypes {
  None,
  End,
  Semicolon,
  Colon,
  Symbol,
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

std::set<std::string_view> type_names = {
    "void",   "int",      "float",  "float2",    "float3",       "float3x3",
    "float4", "float4x4", "matrix", "Texture2D", "SamplerState",
};

std::set<std::string_view> prefix_names = {
    "linear",
    "row_major",
};

struct Token {
  TokenTypes type;
  std::string_view view;

  bool is_struct() const { return view == "struct"; }
  bool is_cbuffer() const { return view == "cbuffer"; }
  bool is_prefix() const {
    return prefix_names.find(view) != prefix_names.end();
  }
  bool is_type() const {
    if (type_names.find(view) != type_names.end()) {
      return true;
    }

    return false;
  }
};

class Tokenizer {
  std::shared_ptr<banana::Asset> _asset;
  std::string_view _source;
  std::string_view::iterator _it;

  Tokenizer(const Tokenizer &) = delete;
  Tokenizer &operator=(const Tokenizer &) = delete;

public:
  Tokenizer(const std::shared_ptr<banana::Asset> &asset) : _asset(asset) {
    _source = asset->string_view();
    _it = _source.begin();
  }

  void skip_space() {
    for (; !is_end();) {
      auto value = *_it;
      switch (value) {
      case ' ':
      case '\t':
      case '\r':
        ++_it;
        continue;
      case '\n':
        ++_it;
        ++current_line;
        continue;
      default:
        return;
      }
    }
  }

  bool is_end() { return _it == _source.end(); }

  Token get_symbol() {
    auto begin = _it;
    for (; !is_end(); ++_it) {
      if (!is_symbol(*_it)) {
        break;
      }
    }
    return Token{
        TokenTypes::Symbol,
        std::string_view(begin, _it),
    };
  }

  Token get_number() {
    auto begin = _it;
    for (; !is_end(); ++_it) {
      if (!std::isdigit(*_it)) {
        break;
      }
    }
    return Token{
        TokenTypes::Integer,
        std::string_view(begin, _it),
    };
  }

  Token get_line(TokenTypes t) {
    auto begin = _it;
    for (; !is_end(); ++_it) {
      if (*_it == '\n') {
        ++current_line;
        ++_it;
        break;
      }
    }
    return Token{
        t,
        std::string_view(begin, _it),
    };
  }

  Token get_line_comment() { return get_line(TokenTypes::LineComment); }

  Token get_directive() { return get_line(TokenTypes::Directive); }

  char peek() const {
    auto p = _it;
    ++p;
    return *p;
  }

  Token next() {
    if (is_end()) {
      throw std::runtime_error("eof");
    }

    while (true) {
      skip_space();
      if (is_end()) {
        break;
      }
      auto value = *_it;

      if (value == '/' && peek() == '/') {
        // comment
        auto comment = get_line_comment();
        continue;
      }

      switch (value) {
      case ';':
        return Token{TokenTypes::Semicolon, {_it, ++_it}};

      case ':':
        return Token{TokenTypes::Colon, {_it, ++_it}};

      case '[':
        return Token{TokenTypes::OpenBracket, {_it, ++_it}};

      case ']':
        return Token{TokenTypes::CloseBracket, {_it, ++_it}};

      case '(':
        return Token{TokenTypes::OpenParenthesis, {_it, ++_it}};

      case ')':
        return Token{TokenTypes::CloseParenthesis, {_it, ++_it}};

      case '{':
        return Token{TokenTypes::OpenBrace, {_it, ++_it}};

      case '}':
        return Token{TokenTypes::CloseBrace, {_it, ++_it}};

      case '<':
        return Token{TokenTypes::LessThan, {_it, ++_it}};

      case '>':
        return Token{TokenTypes::GreaterTham, {_it, ++_it}};

      case '=':
      case '+':
      case '-':
      case '*':
      case '/':
      case '%':
      case '.':
      case ',':
      case '?':
        return Token{TokenTypes::BinOperator, {_it, ++_it}};

      case '#':
        return get_directive();

      case '0':
      case '1':
      case '2':
      case '3':
      case '4':
      case '5':
      case '6':
      case '7':
      case '8':
        return get_number();

      case 'a':
      case 'b':
      case 'c':
      case 'd':
      case 'e':
      case 'f':
      case 'g':
      case 'h':
      case 'i':
      case 'j':
      case 'k':
      case 'l':
      case 'm':
      case 'n':
      case 'o':
      case 'p':
      case 'q':
      case 'r':
      case 's':
      case 't':
      case 'u':
      case 'v':
      case 'w':
      case 'x':
      case 'y':
      case 'z':
      case 'A':
      case 'B':
      case 'C':
      case 'D':
      case 'E':
      case 'F':
      case 'G':
      case 'H':
      case 'I':
      case 'J':
      case 'K':
      case 'L':
      case 'M':
      case 'N':
      case 'O':
      case 'P':
      case 'Q':
      case 'R':
      case 'S':
      case 'T':
      case 'U':
      case 'V':
      case 'W':
      case 'X':
      case 'Y':
      case 'Z':
        return get_symbol();

      default:
        throw std::runtime_error("unknown char");
      }
    }

    return {TokenTypes::End};
  }

  void skip(int init_level, TokenTypes open, TokenTypes close) {
    for (auto level = init_level; level;) {
      if (is_end()) {
        throw std::runtime_error("error");
      }
      auto token = next();
      if (token.type == open) {
        ++level;
      } else if (token.type == close) {
        --level;
      }
    }
  }

  void skip_block(int init_level = 1) {
    skip(init_level, TokenTypes::OpenBrace, TokenTypes::CloseBrace);
  }

  void skip_params(int init_level = 1) {
    skip(init_level, TokenTypes::OpenParenthesis, TokenTypes::CloseParenthesis);
  }

  // register(b0)
  void skip_register() {
    auto r = next();
    if (r.type != TokenTypes::Symbol) {
      throw std::runtime_error("not symbol");
    }
    if (r.view != "register") {
      throw std::runtime_error("not register");
    }
    auto token = next();
    if (token.type != TokenTypes::OpenParenthesis) {
      throw std::runtime_error("not (");
    }
    skip_params();
  }

  int current_line = 1;
};

namespace gorilla {

// auto token = z.next();

//   // type name;
//   // type name : SEMANTIC;
//   Token semantic_token = {};
//   if (token.type == TokenTypes::Colon) {
//     semantic_token = z.next();
//     token = z.next();
//   }
//   if (token.type != TokenTypes::Semicolon) {
//     // ;
//     throw std::runtime_error("not ;");
//   }
//   if (!semantic_token.view.empty()) {
//     auto &s = semantics.emplace_back(AnnotationSemantics{});
//     s.line = z.current_line;
//     s.name = name.view;
//     s.type = first.view;
//     s.semantic = banana::semantics_from_string(semantic_token.view);
//   }
// }

std::optional<AnnotationSemantics> parse_field(Tokenizer &z, Token token) {
  std::optional<AnnotationSemantics> result;
  auto type = token;
  if (type.is_prefix()) {
    type = z.next();
  }
  // if (!type.is_type()) {
  //   throw std::runtime_error(
  //       (std::string("not type: ") + std::string(type.view)).c_str());
  // }
  auto name = z.next();
  token = z.next();
  if (token.type == TokenTypes::OpenParenthesis) {
    // function ()
    z.skip_params();
    token = z.next();
    Token semantic = {};
    if (token.type == TokenTypes::Colon) {
      semantic = z.next(); // float4 psMAin() : SV_TARGET
      token = z.next();
    }
    if (token.type == TokenTypes::OpenBrace) {
      // function body
      z.skip_block();
      // without smicolon
    } else {
      if (token.type != TokenTypes::Semicolon) {
        throw std::runtime_error("not ;");
      }
    }
  } else {
    if (token.type == TokenTypes::OpenBracket) {
      // array
      z.skip(1, TokenTypes::OpenBracket, TokenTypes::CloseBracket);
      token = z.next();
    }
    if (token.type == TokenTypes::Colon) {
      auto semantic_token = z.next();
      token = z.next();

      auto s = std::string(semantic_token.view);
      AnnotationSemantics field{};
      field.line = z.current_line;
      field.name = name.view;
      field.type = type.view;
      field.semantic = banana::semantics_from_string(semantic_token.view);
      result = field;
    }
    if (token.type != TokenTypes::Semicolon) {
      throw std::runtime_error("field not ;");
    }
  }
  return result;
}

static std::vector<AnnotationSemantics> parse_struct(Tokenizer &z,
                                                     bool is_struct) {
  auto name = z.next();
  if (name.type != TokenTypes::Symbol) {
    throw std::runtime_error("struct not name");
  }
  auto token = z.next();
  if (token.type == TokenTypes::Colon) {
    z.skip_register();
    token = z.next();
  }
  if (token.type != TokenTypes::OpenBrace) {
    throw std::runtime_error("struct not {");
  }

  // struct body
  std::vector<AnnotationSemantics> fields;
  for (auto level = 1; level;) {
    auto token = z.next();
    switch (token.type) {
    case TokenTypes::OpenBrace:
      ++level;
      break;

    case TokenTypes::CloseBrace:
      --level;
      break;

    case TokenTypes::Symbol: {
      auto field = parse_field(z, token);
      if (field.has_value()) {
        fields.push_back(field.value());
      }
    }

    default:
      break;
    }
  }
  if (is_struct) {
    auto fourth = z.next();
    if (fourth.type != TokenTypes::Semicolon) {
      throw std::runtime_error("struct not ;");
    }
  }
  return fields;
}

void DXSAS::parse(const std::shared_ptr<banana::Asset> &asset) {
  Tokenizer z(asset);
  while (true) {
    auto first = z.next();
    if (first.type == TokenTypes::End) {
      break;
    }

    if (first.is_struct() || first.is_cbuffer()) {
      // struct
      auto fields = parse_struct(z, first.view == "struct");
      for (auto &f : fields) {
        semantics.push_back(f);
      }
    } else if (first.type == TokenTypes::Symbol) {
      // constant ?
      auto field = parse_field(z, first);
      if (field.has_value()) {
        semantics.push_back(field.value());
      }
    } else if (first.type == TokenTypes::OpenBracket) {
      // [maxvertexcount(6)]
      z.skip(1, TokenTypes::OpenBracket, TokenTypes::CloseBracket);
    } else if (first.type == TokenTypes::Directive) {
      // skip
    } else if (first.type == TokenTypes::Semicolon) {
      // skip
    } else {
      throw std::runtime_error("unknown token");
    }
  }
}

} // namespace gorilla