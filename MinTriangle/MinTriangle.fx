
struct VS_INPUT
{
    float4 Position   : POSITION;
    float4 Color      : COLOR;
};

struct VS_OUTPUT
{
    float4 Position   : SV_POSITION;
    float4 Color      : COLOR;
};

VS_OUTPUT vsMain( VS_INPUT In )
{
    VS_OUTPUT Output;
    Output.Position = In.Position;
    Output.Color    = In.Color;
    return Output;    
}

float4 psMain( VS_OUTPUT In ) : SV_TARGET
{
    return In.Color;
}
