cbuffer ModelBuffer : register(b0)
{
    float4x4 model;
};

cbuffer VPBuffer : register(b1)
{
    float4x4 vp;
};

struct VSInput
{
    float3 pos : POSITION;
    float4 color : COLOR;
};

struct VSOutput
{
    float4 pos : SV_Position;
    float4 color : COLOR;
};

VSOutput main(VSInput input)
{
    VSOutput output;
    
    float4 worldPos = mul(float4(input.pos, 1.0), model);
    output.pos = mul(worldPos, vp);
    
    output.color = input.color;
    
    return output;
}