Texture2D t0;
Texture2D diffuseTexture;
Texture2D t2;
SamplerState diffuseTextureSampler;

struct VS_IN
{
    float4 Position: POSITION;
    float4 Color: COLOR;
    float2 Tex: TEXCOORD0;
};

struct VS_OUT
{
    float4 Position: SV_POSITION;
    float4 Color: COLOR;
    float2 Tex: TEXCOORD0;
};

typedef VS_OUT PS_IN;

cbuffer cb0
{
	row_major matrix ModelMatrix;
}
cbuffer cb1
{
	row_major matrix ViewMatrix;
}
cbuffer cb2
{
	row_major matrix ProjectionMatrix;
};

VS_OUT vsMain(VS_IN input)
{
    VS_OUT Output;
	Output.Position = mul(input.Position, mul(ModelMatrix, mul(ViewMatrix, ProjectionMatrix)));
    Output.Color = input.Color;
    Output.Tex = input.Tex;
    return Output;    
}

float4 psMain(PS_IN input) : SV_TARGET
{
	float4 texel = diffuseTexture.Sample(diffuseTextureSampler, input.Tex);
    return input.Color * texel;
}
