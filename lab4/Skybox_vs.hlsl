cbuffer VPBuffer : register(b0) {
    matrix vp;
};

struct VSInput {
    float3 position : POSITION;
};

struct PSInput {
    float4 position : SV_POSITION;
    float3 texcoord : TEXCOORD;
};

PSInput main(VSInput input) {
    PSInput output;
    float4 pos = float4(input.position, 1.0);
    output.position = mul(pos, vp);
    output.texcoord = input.position;
    return output;
}