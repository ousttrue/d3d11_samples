void vsMain() {}
struct DummyInput {};

struct GS_OUT {
  linear float4 position : SV_POSITION;
  linear float3 ray : RAY;
  linear float2 uv : TEXCOORD0;
};

struct PS_OUT {
  float4 color : SV_Target;
  float depth : SV_Depth;
};

cbuffer SceneConstantBuffer : register(b0) {
  float4x4 b0View : CAMERA_VIEW;
  float4x4 b0Projection : CAMERA_PROJECTION;
  float3 b0CameraPosition : CAMERA_POSITION;
  float2 b0ScreenSize : RENDERTARGET_SIZE;
  float fovY : CAMERA_FOVY;
};

float3 ray(float x, float y) {
  float halfFov = fovY / 2;
  float t = tan(halfFov);
  float aspectRatio = b0ScreenSize.x / b0ScreenSize.y;
  return mul(float4(x * t * aspectRatio, y * t, -1, 0), b0View).xyz;
}

[maxvertexcount(6)] void gsMain(point DummyInput input[1]
                                : POSITION,
                                  inout TriangleStream<GS_OUT> output) {
  GS_OUT element;

  // Vertex{{-1, 1}, {0, 0}},
  // Vertex{{1, 1}, {1, 0}},
  // Vertex{{1, -1}, {1, 1}},
  // Vertex{{-1, -1}, {0, 1}}

  // 0
  element.position = float4(-1, 1, 0, 1);
  element.ray = ray(-1, 1);
  element.uv = float2(0, 0);
  output.Append(element);
  // 1
  element.position = float4(1, 1, 0, 1);
  element.ray = ray(1, 1);
  element.uv = float2(1, 0);
  output.Append(element);
  // 2
  element.position = float4(1, -1, 0, 1);
  element.ray = ray(1, -1);
  element.uv = float2(1, 1);
  output.Append(element);
  output.RestartStrip();

  // 3
  element.position = float4(1, -1, 0, 1);
  element.ray = ray(1, -1);
  element.uv = float2(1, 1);
  output.Append(element);
  // 4
  element.position = float4(-1, -1, 0, 1);
  element.ray = ray(-1, -1);
  element.uv = float2(0, 1);
  output.Append(element);
  // 5
  element.position = float4(-1, 1, 0, 1);
  element.ray = ray(-1, 1);
  element.uv = float2(0, 0);
  output.Append(element);
  output.RestartStrip();
}

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
