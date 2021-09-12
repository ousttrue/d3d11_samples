// Texture2D t0;
// Texture2D diffuseTexture;
// Texture2D t2;
// SamplerState diffuseTextureSampler;

struct VS_IN {
  float4 Position : POSITION;
  float4 Color : COLOR;
  float2 Tex : TEXCOORD0;
};

struct VS_OUT {
  float4 Position : SV_POSITION;
  float4 Color : COLOR;
  float2 Tex : TEXCOORD0;
};

typedef VS_OUT PS_IN;

row_major matrix MVP;

VS_OUT vsMain(VS_IN input) {
  VS_OUT Output;
  Output.Position = mul(input.Position, MVP);
  Output.Color = input.Color;
  Output.Tex = input.Tex;
  return Output;
}

float4 psMain(PS_IN input) : SV_TARGET {
  // float4 texel = diffuseTexture.Sample(diffuseTextureSampler, input.Tex);
  return float4(1, 1, 1, 1); // input.Color; // * texel;
}
