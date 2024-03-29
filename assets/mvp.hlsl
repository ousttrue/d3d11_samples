void vsMain() {}

struct DummyInput {};
struct GS_OUTPUT {
  float4 position : SV_POSITION;
  float4 color : COLOR0;
};

row_major matrix MVP : WORLDVIEWPROJECTION;

[maxvertexcount(3)] void gsMain(point DummyInput input[1]
                                : POSITION,
                                  inout TriangleStream<GS_OUTPUT> output) {
  GS_OUTPUT element;

  // d3d default. clock wise

  // 0
  element.position = mul(float4(-1, -1, 0, 1), MVP);
  element.color = float4(1, 0, 0, 1);
  output.Append(element);
  // 1
  element.position = mul(float4(-1, 1, 0, 1), MVP);
  element.color = float4(0, 1, 0, 1);
  output.Append(element);
  // 2
  element.position = mul(float4(1, 1, 0, 1), MVP);
  element.color = float4(0, 0, 1, 1);
  output.Append(element);

  output.RestartStrip();
}

float4 psMain(GS_OUTPUT V)
    : SV_Target {
  return V.color;
}
