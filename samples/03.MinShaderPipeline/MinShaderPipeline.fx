struct VS_IN
{
    float4 Position: POSITION;
    float4 Color: COLOR;
};

struct VS_OUT
{
    float4 Position: SV_POSITION;
    float4 Color: COLOR;
};

typedef VS_OUT PS_IN;

VS_OUT vsMain(VS_IN input)
{
    VS_OUT Output;
	Output.Position = input.Position;
    Output.Color = input.Color;
    return Output;    
}

float4 psMain(PS_IN input) : SV_TARGET
{
    return input.Color;
}

