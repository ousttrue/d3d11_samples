struct VS_IN {
  float3 Position : POSITION;
  float3 Normal : NORMAL;
  float2 Tex : TEXCOORD0;
  float4 Color : COLOR0;
};

struct PS_IN {
  float4 Position : SV_POSITION;
  float4 Color : COLOR0;
  float2 Tex : TEXCOORD0;
};

row_major matrix MVP;

PS_IN vsMain(VS_IN Input) {
  PS_IN Output;
  Output.Position = mul(float4(Input.Position, 1), MVP);
  Output.Color = Input.Color;
  Output.Tex = Input.Tex;
  return Output;
}

float4 psMain(PS_IN Input) : SV_TARGET { return Input.Color; }
