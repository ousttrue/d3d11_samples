#include "dxsas.h"
#include <assert.h>
#include <cctype>
#include <iterator>
#include <set>
#include <stdexcept>
#include <string_view>
#include <unordered_map>

enum class TokenTypes {
  None,
  Delimiter,
  Symbol,
  // (...)
  FunctionParmas,
  // {
  Open,
  // }
  Close,
};

std::set<std::string_view> type_names = {
    "void",
};

struct Token {
  TokenTypes type;
  std::string_view view;

  bool is_type() const { return type_names.find(view) != type_names.end(); }
};

class Tokenizer {
  std::string_view _source;
  std::string_view::iterator _it;

  Tokenizer(const Tokenizer &) = delete;
  Tokenizer &operator=(const Tokenizer &) = delete;

public:
  Tokenizer(std::string_view source) : _source(source), _it(_source.begin()) {}
  bool is_end() const { return _it == _source.end(); }

  Token get_symbol() {

    auto begin = _it;
    for (; !is_end(); ++_it) {
      if (!std::isalnum(*_it)) {
        break;
      }
    }
    return Token{
        TokenTypes::Symbol,
        std::string_view(begin, _it),
    };
  }

  Token get_function_params() {
    auto begin = _it;
    for (; !is_end(); ++_it) {
      if (*_it == ')') {
        ++_it;
        break;
      }
    }
    return Token{
        TokenTypes::FunctionParmas,
        std::string_view(begin, _it),
    };
  }

  Token next() {
    for (; !is_end(); ++_it) {
      auto value = *_it;
      switch (value) {
      case ' ':
      case '\t':
      case '\r':
        break;
      case '\n':
        ++current_line;
        break;

      case '(':
        return get_function_params();

      case ';':
        return Token{TokenTypes::Delimiter, {_it, ++_it}};

      case '{':
        return Token{TokenTypes::Open, {_it, ++_it}};

      case '}':
        return Token{TokenTypes::Close, {_it, ++_it}};

      case '0':
      case '1':
      case '2':
      case '3':
      case '4':
      case '5':
      case '6':
      case '7':
      case '8':
        assert(false);
        break;

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
        assert(false);
        break;
      }
    }

    return {};
  }

  int current_line = 1;
};

namespace gorilla {
void DXSAS::parse(std::string_view source) {
  for (Tokenizer tokenizer(source); !tokenizer.is_end();) {
    auto first = tokenizer.next();

    if (first.is_type()) {
      auto second = tokenizer.next();
      if (second.type == TokenTypes::Symbol) {
        auto third = tokenizer.next();
        if (third.type == TokenTypes::FunctionParmas) {
          auto fourth = tokenizer.next();
          if (fourth.type == TokenTypes::Open) {
            for (auto level = 1; level;) {
              if (tokenizer.is_end()) {
                throw std::runtime_error("error");
              }
              auto token = tokenizer.next();
              switch (token.type) {
              case TokenTypes::Open:
                ++level;
                break;

              case TokenTypes::Close:
                --level;
                break;

              default:
                break;
              }
            }
            auto fifth = tokenizer.next();
            if (fifth.type == TokenTypes::Delimiter) {
              // ok
            } else {
              throw std::runtime_error("error");
            }
          } else if (fourth.type == TokenTypes::Delimiter) {
            // ok
          } else {
            throw std::runtime_error("error");
          }
        }
      } else {
        assert(false);
      }
    } else {
      assert(false);
    }
  }
}

} // namespace gorilla