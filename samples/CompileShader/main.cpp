#include "shader.h"
#include <assert.h>
#include <iostream>

constexpr const char vs[] = R"(
  void vsMain()
  {
  }  
)";

constexpr const char ps[] = R"(
  void psMain()
  {
    X = float4(1, 2);
  }  
)";

int main(int argc, char **argv) {

  {
    auto [compiled, error] = gorilla::compile_vs("vs", vs, "vsMain");
    if (!compiled) {
      if (error) {
        std::cerr << (char *)error->GetBufferPointer() << std::endl;
      }
      return 1;
    }
  }

  {
    auto [compiled, error] = gorilla::compile_ps("ps", ps, "vsMain");
    if (error) {
      std::cerr << (char *)error->GetBufferPointer() << std::endl;
    }
    if (compiled) {
      assert(false);
    }
  }

  return 0;
}
