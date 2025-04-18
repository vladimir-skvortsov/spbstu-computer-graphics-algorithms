cbuffer ModelBuffer : register(b0) {
    float4x4 model;
};

cbuffer VPBuffer : register(b1) {
    float4x4 vp;
};

struct VSInput {
    float3 position : POSITION;
    float2 texcoord : TEXCOORD;
};

struct VSOutput {
    float4 position : SV_POSITION;
    float2 texcoord : TEXCOORD;
};

VSOutput main(VSInput input) {
    VSOutput output;
    float4 worldPos = float4(input.position, 1.0);
    output.position = mul(worldPos, model);
    output.position = mul(output.position, vp);
    output.texcoord = input.texcoord;
    return output;
}