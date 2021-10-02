struct VS_IN {
  float3 Position : POSITION;
  float3 Normal : NORMAL;
  float2 Tex : TEXCOORD0;
  float4 COLOR : COLOR0;
  float4 Tangent : TANGENT;
};

row_major matrix MVP : WORLDVIEWPROJECTION;
row_major matrix ModelViewMatrix : WORLDVIEW;
row_major float3x3 NormalMatrix : NORMAL_MATRIX;
struct LightInfo {
  // w == 1 ? point : directional
  float4 Position;
  float4 Intensity;
};
LightInfo Lights[5] : LIGHT_LIST;

struct PS_IN {
  float4 Position : SV_POSITION;
  float3 LightDir : NORMAL0;
  float3 ViewDir : NORMAL1;
  float2 Tex : TEXCOORD;
};

PS_IN vsMain(VS_IN In) {
  float3 normal = normalize(mul(In.Normal, NormalMatrix));
  float3 tangent = normalize(mul(In.Tangent.xyz, NormalMatrix));
  float3 binormal = normalize(cross(normal, tangent)) * In.Tangent.w;
  float3x3 toObjectLocal =
      float3x3(tangent.x, binormal.x, normal.x, tangent.y, binormal.y, normal.y,
               tangent.z, binormal.z, normal.z);
  float3 pos = mul(float4(In.Position, 1.0), ModelViewMatrix).xyz;

  PS_IN Out;
  Out.Position = mul(float4(In.Position, 1.0), MVP);
  Out.LightDir = normalize(mul(Lights[0].Position.xyz, toObjectLocal));
  Out.ViewDir = mul(normalize(-pos), toObjectLocal);
  Out.Tex = In.Tex;
  return Out;
};

#if defined(TEXTURE_NORMAL)
Texture2D NormalMapTexture : MATERIAL_NORMAL;
SamplerState NormalMapSampler : MATERIAL_NORMAL;
#endif

float4 psMain(PS_IN In) : SV_TARGET {
  float4 normal = NormalMapTexture.Sample(NormalMapSampler, In.Tex);
  float I = max(dot(In.LightDir, (normal.xyz * 2 - 1)), 0);
  return float4(I, I, I, 1);
};
