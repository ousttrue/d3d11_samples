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
    int _line = 1;

    Source(const std::shared_ptr<banana::Asset> &asset) : _asset(asset) {
      _source = asset->string_view();
      _it = _source.begin();
    }

    bool peek_is(std::string_view target) const {
      auto end = _it;
      for (auto _ : target) {
        if (is_end()) {
          return false;
        }
        ++end;
      }
      return std::string_view{_it, end} == target;
    }

    bool is_end() const { return _it == _source.end(); }

    bool view_is(std::string_view target) {
      auto end = _it;
      for (size_t i = 0; i < target.size(); ++i, ++end) {
        if (end == _source.end()) {
          return false;
        }
      }
      return std::string_view(_it, end) == target;
    }

    void skip_space() {
      _it = skip(_it, _source.end(), [self = this](char c) {
        auto is = is_space(c);
        if (is) {
          ++self->_line;
        }
        return is;
      });
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

    Token get_line(TokenTypes t) {
      auto begin = _it;
      for (; !is_end(); ++_it) {
        if (*_it == '\n') {
          ++_line;
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
    Token get_macro() { return get_line(TokenTypes::Macro); }
  };

public:
  std::stack<Source> _stack;
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
    if (top.peek_is("//")) {
      // comment
      auto _comment = top.get_line_comment();
      goto HEAD;
    }

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
        return top.get_macro();
      }
      break;
    }

    case '[':
      return Token{TokenTypes::OpenBracket, {top._it, ++top._it}};

    case ']':
      return Token{TokenTypes::CloseBracket, {top._it, ++top._it}};

    case '(':
      return Token{TokenTypes::OpenParenthesis, {top._it, ++top._it}};

    case ')':
      return Token{TokenTypes::CloseParenthesis, {top._it, ++top._it}};

    case '{':
      return Token{TokenTypes::OpenBrace, {top._it, ++top._it}};

    case '}':
      return Token{TokenTypes::CloseBrace, {top._it, ++top._it}};

    case '<':
      return Token{TokenTypes::LessThan, {top._it, ++top._it}};

    case '>':
      return Token{TokenTypes::GreaterTham, {top._it, ++top._it}};

    case '=':
    case '+':
    case '-':
    case '*':
    case '/':
    case '%':
    case '.':
    case ',':
    case '?':
      return Token{TokenTypes::BinOperator, {top._it, ++top._it}};

    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
      return top.get_number();

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

int Lexer::line() const { return _impl->_stack.top()._line; }

bool Lexer::try_get(Token *t) {
  *t = _impl->next();
  return t->type != TokenTypes::End;
}

// register(b0)
void Lexer::skip_register() {
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

void Lexer::skip(int init_level, TokenTypes open, TokenTypes close) {
  for (auto level = init_level; level;) {
    // if (is_end()) {
    //   throw std::runtime_error("error");
    // }
    auto token = next();
    if (token.type == open) {
      ++level;
    } else if (token.type == close) {
      --level;
    }
  }
}

Statement parse_field(Lexer &z, Token token) {
  Statement s;

  if (is_prefix(token.view)) {
    s.prefix = token;
    token = z.next();
  }

  if (is_type(token.view)) {
    s.value_type = token;
    token = z.next();
  } else {
    throw std::runtime_error(std::format("not type: {}", token.view));
  }

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
      s.type = StatementTypes::Function;
      // function body
      z.skip_block();
      // without smicolon
    } else {
      throw std::runtime_error("no function body");
      // s.type = StatementTypes::FunctionDeclaration;
      // if (token.type != TokenTypes::Semicolon) {
      //   throw std::runtime_error("not ;");
      // }
    }
  } else {
    s.type = StatementTypes::Field;
    if (token.type == TokenTypes::OpenBracket) {
      // array
      z.skip(1, TokenTypes::OpenBracket, TokenTypes::CloseBracket);
      token = z.next();
    }
    if (token.type == TokenTypes::Colon) {
      s.semantic = z.next();
      s.line = z.line();
      token = z.next();
    }
    if (token.type != TokenTypes::Semicolon) {
      throw std::runtime_error("field not ;");
    }
  }
  return s;
}

static Statement parse_struct(Lexer &z, bool is_struct) {
  auto token = z.next();
  if (token.type != TokenTypes::Symbol) {
    throw std::runtime_error("struct has no name");
  }

  Statement s = {};
  s.type = StatementTypes::Struct;
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
      s.fields.push_back(field);
      break;
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

      if (::is_struct(t.view) || ::is_cbuffer(t.view)) {
        // struct
        auto s = parse_struct(lexer, t.view == "struct");
        statements.push_back(s);
      } else if (t.type == TokenTypes::Symbol) {
        auto s = parse_field(lexer, t);
        statements.push_back(s);
      } else if (t.type == hlsl::TokenTypes::OpenBracket) {
        // [maxvertexcount(6)]
        lexer.skip(1, hlsl::TokenTypes::OpenBracket,
                   hlsl::TokenTypes::CloseBracket);
      } else if (t.type == hlsl::TokenTypes::Macro) {
        // skip
      } else if (t.type == hlsl::TokenTypes::Semicolon) {
        // skip
      } else if (t.type == hlsl::TokenTypes::End) {
        break;
      } else {
        throw std::runtime_error("unknown token");
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

AnnotationSemantics create_annotation_semantics(const Statement &s) {
  AnnotationSemantics a;
  a.line = s.line;
  a.type = s.value_type.string();
  a.name = s.name.string();
  a.semantic = banana::semantics_from_string(s.semantic.view);
  return a;
}

} // namespace hlsl

void DXSAS::parse(const std::shared_ptr<banana::Asset> &asset) {

  hlsl::AST ast;
  ast.parse(asset);

  for (auto &s : ast.statements) {

    switch (s.type) {
    case hlsl::StatementTypes::Struct:
      for (auto &f : s.fields) {
        if (!f.semantic.view.empty()) {
          auto a = hlsl::create_annotation_semantics(f);
          semantics.push_back(a);
        }
      }
      break;

    case hlsl::StatementTypes::Field:
      if (!s.semantic.view.empty()) {
        auto a = hlsl::create_annotation_semantics(s);
        semantics.push_back(a);
        break;
      }
    }
  }
}

} // namespace gorilla