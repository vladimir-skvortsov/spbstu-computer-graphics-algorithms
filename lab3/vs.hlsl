cbuffer ModelBuffer : register(b0) {
    float4x4 model;
};

cbuffer VPBuffer : register(b1) {
    float4x4 vp;
};

struct VS_INPUT {
    float3 position : POSITION;
    float4 color : COLOR;
};

struct PS_INPUT {
    float4 position : SV_POSITION;
    float4 color : COLOR;
};

PS_INPUT main(VS_INPUT input) {
    PS_INPUT output;
    float4 worldPos = float4(input.position, 1.0);
    output.position = mul(worldPos, model);
    output.position = mul(output.position, vp);
    output.color = input.color;
    return output;
}