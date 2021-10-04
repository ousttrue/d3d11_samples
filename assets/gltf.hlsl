struct VS_IN {
  float3 Position : POSITION;
  float2 Tex : TEXCOORD0;
  float3 Normal : NORMAL;
  float4 Tangent : TANGENT;
  float4 Color : COLOR0;
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
float4 BaseColor : MATERIAL_COLOR;

struct PS_IN {
  float4 Position : SV_POSITION;
  float2 Tex : TEXCOORD0;
  float3 Normal : NORMAL;
  float3 LightDir : TANGENT;
};

float3 get_light_dir(int i) { return normalize(float3(1, 2, 3)); }

float3x3 get_tspace(float3 object_normal, float4 object_tangent) {
  float3 normal = normalize(mul(object_normal, NormalMatrix));
  float3 tangent = normalize(mul(object_tangent.xyz, NormalMatrix));
  float3 binormal = normalize(cross(normal, tangent)) * object_tangent.w;
  return float3x3(tangent.x, binormal.x, normal.x, tangent.y, binormal.y,
                  normal.y, tangent.z, binormal.z, normal.z);
}

PS_IN vsMain(VS_IN In) {
  PS_IN Out;
  Out.Position = mul(float4(In.Position, 1), MVP);
  Out.Tex = In.Tex;
  Out.Normal = mul(In.Normal, NormalMatrix);
  Out.LightDir = mul(get_light_dir(0), get_tspace(Out.Normal, In.Tangent));
  return Out;
}

#if defined(TEXTURE_COLOR)
Texture2D BaseColorTexture : MATERIAL_COLOR;
SamplerState BaseColorSampler : MATERIAL_COLOR;
#endif
#if defined(TEXTURE_NORMAL)
Texture2D NormalMapTexture : MATERIAL_NORMAL;
SamplerState NormalMapSampler : MATERIAL_NORMAL;
#endif

float4 psMain(PS_IN In) : SV_TARGET {
#if TEXTURE_NORMAL
  float3 normal = NormalMapTexture.Sample(NormalMapSampler, In.Tex).xyz * 2 - 1;
  float I = max(dot(In.LightDir, normal), 0);
#else
  float I = max(dot(get_light_dir(0), In.Normal), 0);
#endif

#if defined(TEXTURE_COLOR)
  float4 color = BaseColorTexture.Sample(BaseColorSampler, In.Tex) * BaseColor;
#else
  float4 color = BaseColor;
#endif

  return float4(color.rgb * I, color.w);
}
