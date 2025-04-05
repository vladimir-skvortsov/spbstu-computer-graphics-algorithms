#define NUM_INSTANCES 11
#define NUM_TEX 3

cbuffer ModelBuffer : register(b0) {
    float4x4 models[NUM_INSTANCES];
};

cbuffer VPBuffer : register(b1) {
    float4x4 vp;
};

struct VSInput {
    float3 position : POSITION;
    float3 normal : NORMAL;
    float2 texCoord : TEXCOORD0;
};

struct VSOutput {
    float4 position : SV_POSITION;
    float3 worldPos : TEXCOORD0;
    float3 normal : TEXCOORD1;
    float2 texCoord : TEXCOORD2;
    uint   texIndex : TEXCOORD3;
};

VSOutput main(VSInput input, uint instanceID : SV_InstanceID) {
    VSOutput output;
    float4 worldPos = mul(float4(input.position, 1.0), models[instanceID]);
    output.position = mul(worldPos, vp);
    output.worldPos = worldPos.xyz;
    output.normal = mul(input.normal, (float3x3)models[instanceID]);
    output.texCoord = input.texCoord;
    output.texIndex = instanceID % NUM_TEX;
    return output;
}