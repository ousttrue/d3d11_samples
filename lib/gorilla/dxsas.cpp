#include "dxsas.h"
#include <algorithm>
#include <assert.h>
#include <cctype>
#include <format>
#include <iterator>
#include <set>
#include <stack>
#include <stdexcept>
#include <string_view>
#include <unordered_map>

static bool is_struct(std::string_view view) { return view == "struct"; }
static bool is_cbuffer(std::string_view view) { return view == "cbuffer"; }

std::set<std::string_view> prefix_names = {
    "linear",
    "row_major",
};

std::set<std::string_view> type_names = {
    "void",   "int",      "float",  "float2",    "float3",       "float3x3",
    "float4", "float4x4", "matrix", "Texture2D", "SamplerState",
};

static bool is_prefix(std::string_view view) {
  return prefix_names.find(view) != prefix_names.end();
}

static bool is_type(std::string_view view,
                    const std::set<std::string_view> &user_types = {}) {
  if (type_names.find(view) != type_names.end()) {
    return true;
  }
  if (user_types.find(view) != user_types.end()) {
    return true;
  }
  return true;
}

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

static bool is_space(char c) {
  switch (c) {
  case ' ':
  case '\t':
  case '\r':
  case '\n':
    return true;

  default:
    return false;
  }
}

std::string_view::iterator skip(std::string_view::iterator it,
                                std::string_view::iterator end,
                                const std::function<bool(char)> &pred) {
  while (it != end) {
    if (pred(*it)) {
      ++it;
    } else {
      break;
    }
  }
  return it;
}

namespace gorilla {
namespace hlsl {

class LexerImpl {
  struct Source {
    std::shared_ptr<banana::Asset> _asset;
    std::string_view _source;
    std::string_view::iterator _it;

    Source(const std::shared_ptr<banana::Asset> &asset) : _asset(asset) {
      _source = asset->string_view();
      _it = _source.begin();
    }

    bool is_end() { return _it == _source.end(); }

    bool view_is(std::string_view target) {
      auto end = _it;
      for (size_t i = 0; i < target.size(); ++i, ++end) {
        if (end == _source.end()) {
          return false;
        }
      }
      return std::string_view(_it, end) == target;
    }

    void skip_space() { _it = skip(_it, _source.end(), is_space); }

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

    std::string_view get_include_key() {
      auto p = _it;
      for (auto _ : "#include") {
        ++p;
      }

      p = skip(p, _source.end(), is_space);
      if (*p != '"') {
        throw std::runtime_error("not double quote");
      }
      // skip "
      ++p;
      auto begin = p;

      // skip other than "
      p = skip(p, _source.end(), [](char c) { return c != '"'; });
      if (*p != '"') {
        throw std::runtime_error("not double quote");
      }

      return {begin, p};
    }
  };
  std::stack<Source> _stack;

public:
  LexerImpl(const std::shared_ptr<banana::Asset> &asset) {
    _stack.emplace(Source(asset));
  }

  Token next() {

  HEAD:
    _stack.top().skip_space();
    if (_stack.top().is_end()) {
      _stack.pop();
      if (_stack.empty()) {
        return {TokenTypes::End};
      }
      goto HEAD;
    }

    auto &top = _stack.top();
    switch (*top._it) {
    case ':': {
      ++top._it;
      return {TokenTypes::Colon};
    }

    case ';': {
      ++top._it;
      return {TokenTypes::Semicolon};

    case '#': {
      // macro
      if (top.view_is("#include")) {
        auto key = top.get_include_key();
        auto nest = top._asset->get(key);
        top._it =
            skip(top._it, top._source.end(), [](char c) { return c != '\n'; });
        _stack.emplace(Source(nest));
        goto HEAD;
      } else {
        // skip line
        throw std::runtime_error("not implemented");
      }
      break;
    }

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
      return top.get_symbol();
    }

    default:
      throw std::runtime_error(std::format("unknown char: {}", *top._it));
    }

    return {};
  }
};

Lexer::Lexer(const std::shared_ptr<banana::Asset> &asset)
    : _impl(new LexerImpl(asset)) {}
Lexer::~Lexer() { delete _impl; }

bool Lexer::try_get(Token *t) {
  *t = _impl->next();
  return t->type != TokenTypes::End;
}

class ASTImpl {

public:
  bool parse(const std::shared_ptr<banana::Asset> &asset,
             std::vector<Statement> &statements) {
    Lexer lexer(asset);

    while (true) {
      Token t;
      if (!lexer.try_get(&t)) {
        break;
      }

      if (t.type == TokenTypes::Symbol) {
        Statement s;
        if (::is_prefix(t.view)) {
          s.prefix = t;
          if (!lexer.try_get(&t)) {
            throw std::runtime_error("invlaid end");
          }
        }
        if (!::is_type(t.view)) {
          throw std::runtime_error("not type");
        }

      } else {
        throw std::runtime_error("not implemented");
      }
    }

    return false;
  }
};

AST::AST() : _impl(new ASTImpl) {}

AST::~AST() { delete _impl; }

bool AST::parse(const std::shared_ptr<banana::Asset> &asset) {
  return _impl->parse(asset, statements);
}

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

Statement parse_field(Tokenizer &z, Token token) {
  if (is_prefix(token.view)) {
    token = z.next();
  }

  if (!is_type(token.view)) {
    throw std::runtime_error(std::format("not type: {}", token.view));
  }
  Statement s;
  s.value_type = token;

  token = z.next();
  s.name = token;

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
      s.semantic = z.next();
      s.line = z.current_line;
      token = z.next();
    }
    if (token.type != TokenTypes::Semicolon) {
      throw std::runtime_error("field not ;");
    }
  }
  return s;
}

AnnotationSemantics create_annotation_semantics(const Statement &s) {
  AnnotationSemantics a;
  a.line = s.line;
  a.type = s.value_type.string();
  a.name = s.name.string();
  a.semantic = banana::semantics_from_string(s.semantic.view);
  return a;
}

static Statement parse_struct(Tokenizer &z, bool is_struct) {
  auto token = z.next();
  if (token.type != TokenTypes::Symbol) {
    throw std::runtime_error("struct has no name");
  }
  Statement s = {};
  s.name = token;

  token = z.next();
  if (token.type == TokenTypes::Colon) {
    z.skip_register();
    token = z.next();
  }

  if (token.type != TokenTypes::OpenBrace) {
    throw std::runtime_error("struct not {");
  }

  // struct body
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
      AnnotationSemantics a = {};
      s.fields.push_back(a);
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

  return s;
}

} // namespace hlsl

void DXSAS::parse(const std::shared_ptr<banana::Asset> &asset) {
  hlsl::Tokenizer z(asset);
  while (true) {
    auto first = z.next();
    if (first.type == hlsl::TokenTypes::End) {
      break;
    }

    if (::is_struct(first.view) || ::is_cbuffer(first.view)) {
      // struct
      auto s = parse_struct(z, first.view == "struct");
      for (auto &f : s.fields) {
        semantics.push_back(f);
      }
    } else if (first.type == hlsl::TokenTypes::Symbol) {
      // constant ?
      auto field = parse_field(z, first);
      auto a = create_annotation_semantics(field);
      semantics.push_back(a);
    } else if (first.type == hlsl::TokenTypes::OpenBracket) {
      // [maxvertexcount(6)]
      z.skip(1, hlsl::TokenTypes::OpenBracket, hlsl::TokenTypes::CloseBracket);
    } else if (first.type == hlsl::TokenTypes::Directive) {
      // skip
    } else if (first.type == hlsl::TokenTypes::Semicolon) {
      // skip
    } else {
      throw std::runtime_error("unknown token");
    }
  }
}

} // namespace gorilla