cbuffer ModelBuffer : register(b0) {
    float4x4 model;
};

cbuffer VPBuffer : register(b1) {
    float4x4 vp;
};

struct VSInput {
    float3 position : POSITION;
    float3 normal : NORMAL;
    float2 texcoord : TEXCOORD;
};

struct VSOutput {
    float4 position : SV_POSITION;
    float3 worldPos : TEXCOORD;
    float3 normal : TEXCOORD1;
    float2 texcoord : TEXCOORD2;
};

VSOutput main(VSInput input) {
    VSOutput output;
    float4 worldPos = mul(float4(input.position, 1.0), model);
    output.position = mul(worldPos, vp);
    output.worldPos = worldPos.xyz;
    output.normal = mul(input.normal, (float3x3) model);
    output.texcoord = input.texcoord;
    return output;
}