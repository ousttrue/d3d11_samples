struct VS_IN {
  float3 POSITION : POSITION;
  float3 NORMAL : NORMAL;
  float2 UV0 : TEXCOORD0;
  float4 COLOR : COLOR0;
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
  row_major float4x4 ModelViewMatrix;
  row_major float3x3 NormalMatrix;
  row_major float4x4 MVP;
  LightInfo Lights[5];
}

cbuffer Material {
  float3 Kd;
  float3 Ka;
  float3 Ks;
  float Shininess;
}

float lambert(float3 viewNormal, float3 toLight) {
  return max(dot(viewNormal, toLight), 0);
}

float specular(float3 viewNormal, float3 fromPosition, float3 fromLight) {
  float3 v = normalize(fromPosition);
  float3 r = reflect(fromLight, viewNormal);
  return pow(max(dot(r, v), 0), Shininess);
}

float3 lightVector(LightInfo light, float3 viewPosition) {
  if (light.Position.w == 0) {
    return normalize(-mul(light.Position.xyz, NormalMatrix));
  } else {
    float3 light_position = mul(light.Position, ModelViewMatrix).xyz;
    return normalize(light_position - viewPosition);
  }
}

float3 ads(LightInfo light, float3 viewPosition, float3 viewNormal) {
  float3 toLight = lightVector(light, viewPosition);

  return light.Intensity * (Ka + Kd * lambert(viewNormal, toLight) +
                            Ks * specular(viewNormal, -viewPosition, -toLight));
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
