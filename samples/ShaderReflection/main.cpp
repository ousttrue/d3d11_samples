#include "shader.h"
#include "shader_reflection.h"
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

constexpr const char ps[] = R"(
Texture2D t0;
Texture2D diffuseTexture;
Texture2D t2;
SamplerState diffuseTextureSampler;

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

typedef VS_OUT PS_IN;

cbuffer cb0
{
	row_major matrix ModelMatrix;
}
cbuffer cb1
{
	row_major matrix ViewMatrix;
}
cbuffer cb2
{
	row_major matrix ProjectionMatrix;
};

VS_OUT vsMain(VS_IN input)
{
    VS_OUT Output;
  	Output.Position = mul(input.Position, mul(ModelMatrix, mul(ViewMatrix, ProjectionMatrix)));
    Output.Color = input.Color;
    Output.Tex = input.Tex;
    return Output;    
}

float4 psMain(PS_IN input) : SV_TARGET
{
	  float4 texel = diffuseTexture.Sample(diffuseTextureSampler, input.Tex);
    return input.Color * texel;
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

    auto elements = gorilla::get_elements(compiled);
    assert(elements.size() == 3);
  }

  {
    auto [compiled, error] = gorilla::compile_ps("ps", ps, "psMain");
    if (error) {
      std::cerr << (char *)error->GetBufferPointer() << std::endl;
    }

    if (compiled) {
      gorilla::ShaderVariables cb;
      cb.reflect(compiled);

      assert(cb.cb_slots.empty());
      assert(cb.srv_slots.size() == 1);
      assert(cb.sampler_slots.size() == 1);
    }
  }

  return 0;
}
