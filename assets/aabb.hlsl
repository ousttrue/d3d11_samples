void vsMain() {}
struct DummyInput {};

struct GS_OUT {
  linear float4 position : SV_POSITION;
};

row_major float4x4 MVP : WORLDVIEWPROJECTION;
float3 MIN : AABB_MIN;
float3 MAX : AABB_MAX;

[maxvertexcount(2)] void gsMain(point DummyInput input[1]
                                : POSITION,
                                  inout LineStream<GS_OUT> output) {
  GS_OUT element;

  element.position = mul(float4(MIN, 1), MVP);
  output.Append(element);
  element.position = mul(float4(MAX, 1), MVP);
  output.Append(element);
  output.RestartStrip();
}

float4 psMain(GS_OUT ps)
    : SV_TARGET {
  return float4(1, 1, 1, 1);
}
