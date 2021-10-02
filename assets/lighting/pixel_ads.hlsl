struct VS_IN {
  float3 POSITION : POSITION;
  float3 NORMAL : NORMAL;
  float2 UV0 : TEXCOORD0;
  float4 COLOR : COLOR0;
  float4 TANGENT : TANGENT;
};

struct PS_IN {
  float4 POSITION : SV_POSITION;
  float3 COLOR : COLOR0;
  float2 UV0 : TEXCOORD0;
};

struct LightInfo {
  // w == 1 ? point : directional
  float4 Position;
  float4 Intensity;
};

cbuffer World {
  row_major float4x4 ModelViewMatrix : WORLDVIEW;
  row_major float3x3 NormalMatrix : OBJECT_NORMAL;
  row_major float4x4 MVP : WORLDVIEWPROJECTION;
  LightInfo Lights[5] : LIGHT_LIST;
}

cbuffer Material {
  float3 Kd : MATERIAL_COLOR;
  float3 Ka : MATERIAL_AMBIENT;
  float4 Ks : MATERIAL_SPECULAR;
}

float lambert(float3 n, float3 s) { return max(dot(n, s), 0); }

float specular(float3 n, float3 s, float3 v) {
  float3 r = reflect(-s, n);
  return pow(max(dot(r, v), 0), Ks.w);
}

float3 lightVector(LightInfo light, float3 viewPosition) {
  if (light.Position.w == 0) {
    return normalize(-mul(light.Position.xyz, NormalMatrix));
  } else {
    float3 light_position = mul(light.Position, ModelViewMatrix).xyz;
    return normalize(light_position - viewPosition);
  }
}

float3 ads(LightInfo light, float3 viewPosition, float3 n) {
  float3 s = lightVector(light, viewPosition);

  return light.Intensity * (Ka                   //
                            + Kd * lambert(n, s) //
                            + Ks * specular(n, s, normalize(-viewPosition)));
}

PS_IN vsMain(VS_IN IN) {
  float3 viewNormal = normalize(mul(IN.NORMAL, NormalMatrix));
  float3 viewPosition = mul(float4(IN.POSITION, 1), ModelViewMatrix).xyz;

  PS_IN OUT;
  OUT.COLOR = float3(0, 0, 0);
  OUT.COLOR += ads(Lights[0], viewPosition, viewNormal);
  OUT.COLOR += ads(Lights[1], viewPosition, viewNormal);
  OUT.COLOR += ads(Lights[2], viewPosition, viewNormal);
  OUT.COLOR += ads(Lights[3], viewPosition, viewNormal);
  OUT.COLOR += ads(Lights[4], viewPosition, viewNormal);
  OUT.POSITION = mul(float4(IN.POSITION, 1), MVP);
  OUT.UV0 = IN.UV0;
  return OUT;
}

float4 psMain(PS_IN IN) : SV_TARGET { return float4(IN.COLOR, 1); }
