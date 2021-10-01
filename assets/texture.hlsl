Texture2D t0;
SamplerState s0;

void vsMain() {}

struct DummyInput {};
struct GS_OUTPUT {
  float4 position : SV_POSITION;
  float2 uv : TEXCOORD0;
};

[maxvertexcount(6)] void gsMain(point DummyInput input[1]
                                : POSITION,
                                  inout TriangleStream<GS_OUTPUT> output) {
  GS_OUTPUT element;

  // d3d default clock wise

  // 0
  element.position = float4(-1, -1, 0, 1);
  element.uv = float2(0, 1);
  output.Append(element);
  // 1
  element.position = float4(-1, 1, 0, 1);
  element.uv = float2(0, 0);
  output.Append(element);
  // 2
  element.position = float4(1, 1, 0, 1);
  element.uv = float2(1, 0);
  output.Append(element);

  output.RestartStrip();

  // 3
  element.position = float4(1, 1, 0, 1);
  element.uv = float2(1, 0);
  output.Append(element);
  // 4
  element.position = float4(1, -1, 0, 1);
  element.uv = float2(1, 1);
  output.Append(element);
  // 5s
  element.position = float4(-1, -1, 0, 1);
  element.uv = float2(0, 1);
  output.Append(element);

  output.RestartStrip();
}

float4 psMain(GS_OUTPUT V)
    : SV_Target {
  // return float4(V.uv, 0, 1);
  return t0.Sample(s0, V.uv);
}
