#include "dxsas.h"
#include <assert.h>

enum TokenTypes {

};

struct Token {};

class Tokenizer {
  std::string_view _source;
  std::string_view::iterator _it;

  Tokenizer(const Tokenizer &) = delete;
  Tokenizer &operator=(const Tokenizer &) = delete;

public:
  Tokenizer(std::string_view source) : _source(source), _it(_source.begin()) {}
  bool is_end() const { return _it == _source.end(); }
  void next() {

    // skip space
    while (!is_end()) {
      auto value = *_it++;
      switch (value) {
      case ' ':
      case '\t':
      case '\r':
        break;
      case '\n':
        ++current_line;
        break;

      default:
        assert(false);
        break;
      }
    }
  }

  int current_line = 1;
  Token current;
};

namespace gorilla {
void DXSAS::parse(std::string_view source) {
  for (Tokenizer tokenizer(source); !tokenizer.is_end(); tokenizer.next()) {
  }
}

} // namespace gorilla