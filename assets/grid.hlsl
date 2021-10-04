#include "gs_screen.cginc"

struct PS_OUT {
  float4 color : SV_TARGET;
  float depth : SV_DEPTH;
};

// https://gamedev.stackexchange.com/questions/141916/antialiasing-shader-grid-lines
float gridX(float x) {
  float wrapped = frac(x + 0.5) - 0.5;
  float range = abs(wrapped);
  return range;
}

// https://stackoverflow.com/questions/50659949/hlsl-modify-depth-in-pixel-shader
PS_OUT psMain(GS_OUT ps) {
  float3 n = b0CameraPosition.y >= 0 ? float3(0, -1, 0) : float3(0, 1, 0);

  float d = dot(n, ps.ray);
  clip(d);

  float t = dot(-b0CameraPosition, n) / d;
  float3 world = b0CameraPosition + t * ps.ray;

  float3 forward = float3(b0View[2][0], b0View[2][1], b0View[2][2]);
  float fn = 0.2 + smoothstep(0, 0.8, abs(dot(forward, n)));
  float near = 30 * fn;
  float far = near * 3;

  float distance = length(b0CameraPosition - world);
  float fade = smoothstep(1, 0, (distance - near) / (far - near));

  int modX = trunc(abs(world.x) + 0.5) % 5;
  int modY = trunc(abs(world.z) + 0.5) % 5;
  float thicknessX = modX ? 0.005 : 0.02;
  float thicknessY = modY ? 0.005 : 0.02;

  float2 dd = fwidth(world.xz);
  float gx = gridX(world.x);
  float gy = gridX(world.z);
  float lx = 1 - saturate(gx - thicknessX) / dd.x;
  float ly = 1 - saturate(gy - thicknessY) / dd.y;
  float c = max(lx, ly);
  c *= fade;

  float4 projected = mul(b0Projection, mul(b0View, float4(world, 1)));

  PS_OUT output = (PS_OUT)0;
  output.color = float4(float3(0.8, 0.8, 0.8) * c, 0.5);
  output.depth = projected.z / projected.w;

  return output;
}
