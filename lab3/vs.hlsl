cbuffer ModelBuffer : register(b0) {
    matrix m;
};

cbuffer VPBuffer : register(b1) {
    matrix vp;
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
    float4 pos = float4(input.position, 1.0);
    output.position = mul(pos, m);
    output.position = mul(output.position, vp);
    output.color = input.color;
    return output;
}