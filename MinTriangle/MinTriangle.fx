Texture2D diffuseTexture: register(t0);
SamplerState diffuseTextureSampler: register(s0);

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

//cbuffer c0
//{
	float4x4 ModelMatrix;
//};

VS_OUT vsMain(VS_IN input)
{
    VS_OUT Output;
	Output.Position = mul(input.Position, ModelMatrix);
    Output.Color = input.Color;
    Output.Tex = input.Tex;
    return Output;    
}

float4 psMain(PS_IN input) : SV_TARGET
{
	float4 texel = diffuseTexture.Sample(diffuseTextureSampler, input.Tex);
    return input.Color * texel;
}
