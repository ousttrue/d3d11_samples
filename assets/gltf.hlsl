Texture2D t0;
SamplerState s0;

struct VS_IN {
  float3 Position : POSITION;
  float3 NORMAL : NORMAL;
  float2 Tex : TEXCOORD0;
  float4 COLOR: COLOR0;
};

struct PS_IN {
  float4 Position : SV_POSITION;
  float2 Tex : TEXCOORD0;
};

cbuffer c0 {
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
  return t0.Sample(s0, In.Tex) * BaseColor;
}
