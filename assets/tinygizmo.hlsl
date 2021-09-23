cbuffer c0 {
  row_major float4x4 VP;
  float3 CameraPosition;
}

struct VS_IN {
  float3 Position : POSITION;
  float3 Normal : NORMAL;
  float4 Color : COLOR0;
};

struct PS_IN {
  float4 Position : SV_POSITION;
  float4 Color : COLOR0;
  float3 World : POSITION1;
  float3 Normal : NORMAL;
};

PS_IN vsMain(VS_IN In) {
  PS_IN Out;
  Out.Position = mul(float4(In.Position, 1), VP);
  Out.Color = In.Color;
  Out.World = In.Position;
  Out.Normal = In.Normal;
  return Out;
}

float4 psMain(PS_IN In) : SV_TARGET {
  float3 light =
      float3(1, 1, 1) *
          max(dot(In.Normal, normalize(CameraPosition - In.World)), 0.50) +
      0.25;
  return In.Color * float4(light, 1);
}
