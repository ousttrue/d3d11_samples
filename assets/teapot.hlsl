struct VS_IN {
  float3 Position : POSITION;
  float3 Normal : NORMAL;
};

struct PS_IN {
  float4 Position : SV_POSITION;
  float3 Color : COLOR0;
};

cbuffer c0 {
  row_major matrix MVP;
  row_major matrix M;
};

PS_IN vsMain(VS_IN In) {
  PS_IN Output;
  Output.Position = mul(float4(In.Position, 1), MVP);
  float3 n = normalize(mul(float4(In.Normal, 0), M).xyz);
  float3 s = normalize(float3(2, -1, 3));
  float intensity = max(dot(n, s), 0);
  Output.Color = float3(intensity, intensity, intensity);
  return Output;
}

float4 psMain(PS_IN In) : SV_TARGET { return float4(In.Color, 1); }
