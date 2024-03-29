struct VS_IN {
  float2 Position : POSITION;
};

struct PS_IN {
  float4 Position : SV_POSITION;
};

PS_IN vsMain(VS_IN input) {
  PS_IN Output;
  Output.Position = float4(input.Position, 0, 1);
  return Output;
}

float4 psMain(PS_IN input) : SV_TARGET { return float4(1, 1, 1, 1); }
