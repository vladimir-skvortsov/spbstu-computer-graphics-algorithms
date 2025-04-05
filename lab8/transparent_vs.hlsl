cbuffer ModelBuffer : register(b0)
{
    matrix model;
};

cbuffer VPBuffer : register(b1)
{
    matrix viewProj;
};

struct VS_INPUT
{
    float3 Pos : POSITION;
    float3 Normal : NORMAL;
    float2 Tex : TEXCOORD;
};

struct PS_INPUT
{
    float4 Pos : SV_POSITION;
    float3 Normal : NORMAL;
    float2 Tex : TEXCOORD;
    float3 WorldPos : WORLDPOS;
};

PS_INPUT main(VS_INPUT input)
{
    PS_INPUT output;
    float4 worldPos = mul(float4(input.Pos, 1.0), model);
    output.WorldPos = worldPos.xyz;
    output.Pos = mul(worldPos, viewProj);
    output.Normal = mul(input.Normal, (float3x3) model);
    output.Tex = input.Tex;
    return output;
}