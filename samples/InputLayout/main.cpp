#include "shader.h"
#include "input_layout.h"
#include <assert.h>
#include <iostream>

template <typename T> using ComPtr = Microsoft::WRL::ComPtr<T>;

constexpr const char vs[] = R"(
struct VS_IN
{
    float4 Position: POSITION;
    float4 Color: COLOR;
    float2 Tex: TEXCOORD0;
};

struct VS_OUT
{
    float4 Position: SV_POSITION;
    float4 Color: COLOR;
    float2 Tex: TEXCOORD0;
};

VS_OUT vsMain(VS_IN input)
{
    VS_OUT Output;
  	Output.Position = input.Position;
    Output.Color = input.Color;
    Output.Tex = input.Tex;
    return Output;
}
)";


int main(int argc, char **argv) {

  auto [compiled, error] = swtk::compile_vs("vs", vs, "vsMain");
  if (!compiled) {
    if (error) {
      std::cerr << (char *)error->GetBufferPointer() << std::endl;
    }

    return 1;
  }

  auto elements = swtk::get_elements(compiled);
  assert(elements.size() == 3);

  return 0;
}
