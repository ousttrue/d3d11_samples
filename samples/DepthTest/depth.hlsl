struct VS_IN {
  float4 Position : POSITION;
  float4 Color : COLOR0;
  float2 Tex : TEXCOORD0;
};

struct PS_IN {
  float4 Position : SV_POSITION;
  float4 Color : COLOR0;
  float2 Tex : TEXCOORD0;
};

row_major matrix MVP;

PS_IN vsMain(VS_IN Input) {
  PS_IN Output;
  Output.Position = mul(Input.Position, MVP);
  Output.Color = Input.Color;
  Output.Tex = Input.Tex;
  return Output;
}

float4 psMain(PS_IN Input) : SV_TARGET { return Input.Color; }
