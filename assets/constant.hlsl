void vsMain() {}

struct DummyInput {};
struct GS_OUTPUT {
  float4 position : SV_POSITION;
  float4 color : COLOR0;
};

float4 xywh;

float pixel_to_screen_x(float x) { return (x / xywh.z) * 2 - 1; }
float pixel_to_screen_y(float y) { return (y / xywh.w) * 2 - 1; }

[maxvertexcount(3)] void gsMain(point DummyInput input[1]
                                : POSITION,
                                  inout TriangleStream<GS_OUTPUT> output) {
  GS_OUTPUT element;

  // rect
  const float size = 10;
  float l = pixel_to_screen_x(xywh.x - size);
  float r = pixel_to_screen_x(xywh.x + size);
  float b = -pixel_to_screen_y(xywh.y + size);
  float t = -pixel_to_screen_y(xywh.y - size);

  // 0
  element.position = float4(l, b, 0, 1);
  element.color = float4(1, 0, 0, 1);
  output.Append(element);
  // 1
  element.position = float4(l, t, 0, 1);
  element.color = float4(0, 1, 0, 1);
  output.Append(element);
  // 2
  element.position = float4(r, t, 0, 1);
  element.color = float4(0, 0, 1, 1);
  output.Append(element);
  output.RestartStrip();
}

float4 psMain(GS_OUTPUT V)
    : SV_Target {
  return V.color;
}
