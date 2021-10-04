void vsMain() {}
struct DummyInput {};

struct GS_OUT {
  linear float4 position : SV_POSITION;
  linear float3 ray : NORMAL;
  linear float2 uv : TEXCOORD;
};

float4x4 b0View : CAMERA_VIEW;
float4x4 b0Projection : CAMERA_PROJECTION;
float3 b0CameraPosition : CAMERA_POSITION;
float4 b0ScreenSize : CURSOR_SCREEN_SIZE;
float4 NearFarFovY : CAMERA_NEAR_FAR_FOVY;

float3 ray(float x, float y) {
  float halfFov = NearFarFovY.z / 2;
  float t = tan(halfFov);
  float aspectRatio = b0ScreenSize.z / b0ScreenSize.w;
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
  element.position = float4(1, -1, 0, 1);
  element.ray = ray(1, -1);
  element.uv = float2(1, 1);
  output.Append(element);
  // 2
  element.position = float4(1, 1, 0, 1);
  element.ray = ray(1, 1);
  element.uv = float2(1, 0);
  output.Append(element);

  output.RestartStrip();

  // 3
  element.position = float4(1, -1, 0, 1);
  element.ray = ray(1, -1);
  element.uv = float2(1, 1);
  output.Append(element);
  // 4
  element.position = float4(-1, 1, 0, 1);
  element.ray = ray(-1, 1);
  element.uv = float2(0, 0);
  output.Append(element);
  // 5
  element.position = float4(-1, -1, 0, 1);
  element.ray = ray(-1, -1);
  element.uv = float2(0, 1);
  output.Append(element);

  output.RestartStrip();
}
