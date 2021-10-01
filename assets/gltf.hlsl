#if defined(TEXTURE_COLOR)
Texture2D BaseColorTexture : MATERIAL_COLOR;
SamplerState BaseColorSampler : MATERIAL_COLOR;
#endif
#if defined(TEXTURE_NORMAL)
Texture2D NormalMapTexture : MATERIAL_NORMAL;
SamplerState NormalMapSampler : MATERIAL_NORMAL;
#endif
struct VS_IN {
  float3 Position : POSITION;
  float3 NORMAL : NORMAL;
  float2 Tex : TEXCOORD0;
  float4 COLOR : COLOR0;
  float4 TANGENT : TANGENT;
};

struct PS_IN {
  float4 Position : SV_POSITION;
  float2 Tex : TEXCOORD0;
};

cbuffer c0 : register(b0) {
  row_major matrix MVP : WORLDVIEWPROJECTION;
  float4 BaseColor : MATERIAL_COLOR;
}

PS_IN vsMain(VS_IN In) {
  PS_IN Output;
  Output.Position = mul(float4(In.Position, 1), MVP);
  Output.Tex = In.Tex;
  return Output;
}

float4 psMain(PS_IN In) : SV_TARGET {
#if defined(TEXTURE_COLOR)
  return BaseColorTexture.Sample(BaseColorSampler, In.Tex) * BaseColor;
#else
  return BaseColor;
#endif
}
