Texture2D t0;
SamplerState s0;

struct VS_IN {
  float2 Position : POSITION;
  float2 Tex : TEXCOORD0;
};

struct PS_IN {
  float4 Position : SV_POSITION;
  float2 Tex : TEXCOORD0;
};

cbuffer c0 { row_major matrix MVP; }

PS_IN vsMain(VS_IN In) {
  PS_IN Output;
  Output.Position = mul(float4(In.Position, 0, 1), MVP);
  Output.Tex = In.Tex;
  return Output;
}

float4 psMain(PS_IN In) : SV_TARGET {
  return t0.Sample(s0, In.Tex);
}
