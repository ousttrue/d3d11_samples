Texture2D BaseColorTexture : register(t0);
SamplerState BaseColorSampler : register(s0);
Texture2D NormalMapTexture : register(t1);
SamplerState NormalMapSampler : register(s1);

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
  row_major matrix MVP;
  float4 BaseColor;
}

PS_IN vsMain(VS_IN In) {
  PS_IN Output;
  Output.Position = mul(float4(In.Position, 1), MVP);
  Output.Tex = In.Tex;
  return Output;
}

float4 psMain(PS_IN In) : SV_TARGET {
  return BaseColorTexture.Sample(BaseColorSampler, In.Tex) * BaseColor;
  // return NormalMapTexture.Sample(NormalMapSampler, In.Tex) * BaseColor;
}
